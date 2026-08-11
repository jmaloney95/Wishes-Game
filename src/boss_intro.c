#include "global.h"
#include "boss_intro.h"
#include "decompress.h"
#include "event_data.h"
#include "main.h"
#include "npc_portrait.h"
#include "palette.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "m4a.h"
#include "util.h"
#include "constants/boss_intro.h"
#include "constants/rgb.h"
#include "constants/portraits.h"
#include "constants/songs.h"

// ============================================================================
//  Boss intro card -- the "boss subtitles" slam (see boss_intro_card_handoff).
//  One data-driven task: field freezes under the calling script's lockall, a
//  tint washes over everything except the card, the banner slams in from the
//  left with the boss's pre-rendered name, the portrait pops in from the
//  right, hold, exit, tint lifts, script resumes (special+waitstate pattern:
//  the bossintro macro embeds a waitstate; this task calls
//  ScriptContext_Enable() exactly once, from its final state).
//
//  Banner/name are OBJ subsprite assemblies on purpose -- BG0 carries the
//  text windows and map popups, so nothing here touches BG offsets.
// ============================================================================

#define TAG_BI_BANNER   0x4B20 // tiles + the shared card palette
#define TAG_BI_NAME     0x4B21 // tiles only (shares the banner palette)
#define TAG_BI_PORTRAIT 0x4B22 // palette only (tiles stream via images frame)

#define BI_BANNER_X_OFF   (-136)
#define BI_BANNER_X_ON    120
#define BI_BANNER_Y       120
#define BI_NAME_X_NUDGE   6    // name rides the banner with a slight offset
#define BI_PORTRAIT_X_OFF 272
#define BI_PORTRAIT_X_ON  176
#define BI_PORTRAIT_Y     56

struct BossIntroData
{
    u16 portraitId;                              // existing NPC portrait system id
    const struct CompressedSpriteSheet *nameSheet;
    u16 seCue;                                   // frame 0
    u16 seSlam;                                  // banner settle
    u8 holdFrames;
    u16 tintColor;
    bool8 cutMusic;
    u16 battleBGM;                               // nonzero: start at the slam and
                                                 // carry into the battle unbroken
};

// TRUE between the card's slam and the battle actually starting: the battle
// theme is ALREADY playing, so battle_setup must not restart it (and uses the
// quick white-bars transition instead of the long class one).
EWRAM_DATA bool8 gWotBossIntroPrimed = FALSE;

// -mwidth 8 -mheight 4 = serialize in 64x32 metatiles so each subsprite
// piece's 32 tiles are consecutive (without it the sheet reads row-major
// across the full width and the banner renders scrambled).
static const u32 sBiBannerGfx[] = INCGFX_U32("graphics/boss_intro/banner.png", ".4bpp.smol", "-mwidth 8 -mheight 4");
static const u16 sBiBannerPal[] = INCGFX_U16("graphics/boss_intro/banner.pal", ".gbapal");
static const u32 sBiNameGfx_Edwards[] = INCGFX_U32("graphics/boss_intro/name_edwards.png", ".4bpp.smol", "-mwidth 8 -mheight 4");
static const u32 sBiNameGfx_Allison[] = INCGFX_U32("graphics/boss_intro/name_allison.png", ".4bpp.smol", "-mwidth 8 -mheight 4");

static const struct CompressedSpriteSheet sBiBannerSheet = { sBiBannerGfx, 0x1000, TAG_BI_BANNER };
static const struct CompressedSpriteSheet sBiNameSheet_Edwards = { sBiNameGfx_Edwards, 0xC00, TAG_BI_NAME };
static const struct CompressedSpriteSheet sBiNameSheet_Allison = { sBiNameGfx_Allison, 0xC00, TAG_BI_NAME };
static const struct SpritePalette sBiBannerPalette = { sBiBannerPal, TAG_BI_BANNER };

static const struct BossIntroData sBossIntroData[BOSS_INTRO_COUNT] =
{
    [BOSS_INTRO_EDWARDS] =
    {
        .portraitId = PORTRAIT_EDWARDS,
        .nameSheet = &sBiNameSheet_Edwards,
        .seCue = SE_M_DETECT,
        .seSlam = 0, // SE_MUGSHOT is silent in this fork and only cut the shimmer
        .holdFrames = 180,
        .tintColor = RGB(6, 2, 12),
        .cutMusic = TRUE,
        .battleBGM = MUS_DP_VS_UXIE_MESPRIT_AZELF,
    },
    [BOSS_INTRO_ALLISON] =
    {
        .portraitId = PORTRAIT_ALLISON,
        .nameSheet = &sBiNameSheet_Allison,
        .seCue = SE_M_DETECT,
        .seSlam = 0, // SE_MUGSHOT is silent in this fork and only cut the shimmer
        .holdFrames = 180,
        .tintColor = RGB(2, 6, 14),   // deep sea-blue for the siren
        .cutMusic = TRUE,
        .battleBGM = 0,               // normal battle music/transition
    },
};

// 256x32 banner = 4x 64x32 pieces on one sprite; 192x32 name = 3 pieces.
static const struct Subsprite sBiBannerSubsprites[] =
{
    { .x = -128, .y = -16, .shape = SPRITE_SHAPE(64x32), .size = SPRITE_SIZE(64x32), .tileOffset = 0,  .priority = 0 },
    { .x = -64,  .y = -16, .shape = SPRITE_SHAPE(64x32), .size = SPRITE_SIZE(64x32), .tileOffset = 32, .priority = 0 },
    { .x = 0,    .y = -16, .shape = SPRITE_SHAPE(64x32), .size = SPRITE_SIZE(64x32), .tileOffset = 64, .priority = 0 },
    { .x = 64,   .y = -16, .shape = SPRITE_SHAPE(64x32), .size = SPRITE_SIZE(64x32), .tileOffset = 96, .priority = 0 },
};
static const struct SubspriteTable sBiBannerSubTable[] = {{ ARRAY_COUNT(sBiBannerSubsprites), sBiBannerSubsprites }};

static const struct Subsprite sBiNameSubsprites[] =
{
    { .x = -96, .y = -16, .shape = SPRITE_SHAPE(64x32), .size = SPRITE_SIZE(64x32), .tileOffset = 0,  .priority = 0 },
    { .x = -32, .y = -16, .shape = SPRITE_SHAPE(64x32), .size = SPRITE_SIZE(64x32), .tileOffset = 32, .priority = 0 },
    { .x = 32,  .y = -16, .shape = SPRITE_SHAPE(64x32), .size = SPRITE_SIZE(64x32), .tileOffset = 64, .priority = 0 },
};
static const struct SubspriteTable sBiNameSubTable[] = {{ ARRAY_COUNT(sBiNameSubsprites), sBiNameSubsprites }};

static const struct OamData sBiOam_64x32 =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x32),
    .size = SPRITE_SIZE(64x32),
    .priority = 0,
};
static const struct OamData sBiOam_Portrait =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .size = SPRITE_SIZE(64x64),
    .priority = 0,
};

static const struct SpriteTemplate sBiBannerTemplate =
{
    .tileTag = TAG_BI_BANNER,
    .paletteTag = TAG_BI_BANNER,
    .oam = &sBiOam_64x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};
static const struct SpriteTemplate sBiNameTemplate =
{
    .tileTag = TAG_BI_NAME,
    .paletteTag = TAG_BI_BANNER,
    .oam = &sBiOam_64x32,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// Portrait streams its (uncompressed) tiles through an images frame like the
// field portrait does -- a real ANIMCMD_FRAME is required for the DMA.
static EWRAM_DATA struct SpriteFrameImage sBiPortraitImage = {0};
static const union AnimCmd sBiAnim_Portrait[] = { ANIMCMD_FRAME(0, 0), ANIMCMD_END };
static const union AnimCmd *const sBiAnims_Portrait[] = { sBiAnim_Portrait };
static const struct SpriteTemplate sBiPortraitTemplate =
{
    .tileTag = TAG_NONE,
    .paletteTag = TAG_BI_PORTRAIT,
    .oam = &sBiOam_Portrait,
    .anims = sBiAnims_Portrait,
    .images = &sBiPortraitImage,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

enum
{
    BI_STATE_INIT,
    BI_STATE_ENTER,
    BI_STATE_HOLD,
    BI_STATE_EXIT,
    BI_STATE_DONE,
};

#define tState     data[0]
#define tBossId    data[1]
#define tElapsed   data[2]
#define tSprBanner data[3]
#define tSprName   data[4]
#define tSprPort   data[5]
#define tHold      data[6]
#define tFlash     data[7]
#define tVelBanner data[8]
#define tVelPort   data[9]
#define tExitT     data[10]
#define tMusicWait data[11]

// Every palette except the card's two OBJ palettes. Call only after the card
// palettes are loaded. OBJ palette n = bit 16+n.
static u32 BossIntro_FadeMask(void)
{
    u32 mask = PALETTES_ALL;
    u8 idx = IndexOfSpritePaletteTag(TAG_BI_BANNER);

    if (idx != 0xFF)
        mask &= ~(1u << (16 + idx));
    idx = IndexOfSpritePaletteTag(TAG_BI_PORTRAIT);
    if (idx != 0xFF)
        mask &= ~(1u << (16 + idx));
    return mask;
}

static void BossIntro_RestoreCardPalettes(void)
{
    u8 idx = IndexOfSpritePaletteTag(TAG_BI_BANNER);

    if (idx != 0xFF)
        CpuCopy16(&gPlttBufferUnfaded[OBJ_PLTT_ID(idx)], &gPlttBufferFaded[OBJ_PLTT_ID(idx)], PLTT_SIZE_4BPP);
    idx = IndexOfSpritePaletteTag(TAG_BI_PORTRAIT);
    if (idx != 0xFF)
        CpuCopy16(&gPlttBufferUnfaded[OBJ_PLTT_ID(idx)], &gPlttBufferFaded[OBJ_PLTT_ID(idx)], PLTT_SIZE_4BPP);
}

static void BossIntro_FlashCardWhite(void)
{
    u8 idx = IndexOfSpritePaletteTag(TAG_BI_BANNER);

    if (idx != 0xFF)
        BlendPalette(OBJ_PLTT_ID(idx), 16, 14, RGB_WHITE);
    idx = IndexOfSpritePaletteTag(TAG_BI_PORTRAIT);
    if (idx != 0xFF)
        BlendPalette(OBJ_PLTT_ID(idx), 16, 14, RGB_WHITE);
}

// Launch a boss battle theme and prime the battle-start path. Public so
// scripts can detonate the theme mid-scene (Edwards's landing) long before
// the card runs -- the card then leaves the music alone entirely.
void WotStartBossTheme(u16 song)
{
    ResetMapMusic();   // no queued song may resurrect over the theme
    // BGM player only -- m4aMPlayAllStop() also silences the SE
    // players and audibly chopped the slam SE.
    m4aMPlayStop(&gMPlayInfo_BGM);
    m4aSongNumStart(song);
    gMPlayInfo_BGM.fadeOI = 0; // kill any in-flight fade so the theme holds volume
    gWotBossIntroPrimed = TRUE;
}

// special: song id in VAR_0x8004.
void WotPrimeBossTheme(void)
{
    WotStartBossTheme(gSpecialVar_0x8004);
}

static void BossIntro_StartBattleTheme(const struct BossIntroData *boss)
{
    WotStartBossTheme(boss->battleBGM);
}

// Ease-out: velocity decays 3/4 per frame, min 2. Returns TRUE on arrival.
static bool32 MoveTowardX(struct Sprite *sprite, s16 target, s16 *vel)
{
    s16 d = target - sprite->x;
    s16 step = *vel;

    if (d == 0)
        return TRUE;
    if (step > abs(d))
        step = abs(d);
    sprite->x += (d > 0) ? step : -step;
    *vel = *vel * 3 / 4;
    if (*vel < 2)
        *vel = 2;
    return sprite->x == target;
}

static void Task_BossIntro(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    const struct BossIntroData *boss = &sBossIntroData[tBossId];

    if (tState != BI_STATE_INIT)
        tElapsed++;
    if (tFlash > 0 && --tFlash == 0)
        BossIntro_RestoreCardPalettes();

    switch (tState)
    {
    case BI_STATE_INIT:
        // Never fight another fade (weather, transitions): wait our turn.
        if (gPaletteFade.active)
            return;
        // THE card sound: the seCue shimmer, fired at frame 0 so it rings
        // through the slide + slam untouched. seSlam is 0 for every boss --
        // SE_MUGSHOT turned out to be a SILENT song in this fork (its voice
        // in voicegroup_rs_sfx_2 produces nothing), and every PlaySE of it
        // replaced the ringing shimmer on SE1 with silence. That was the
        // entire "card sound never plays / muted partway" saga.
        PlaySE(boss->seCue);
        // FadeOutMapMusic (not raw FadeOutBGM): it also clears the map-music
        // state machine. The trainer-approach encounter jingle queues itself
        // there (state 6, "play the queued song once the BGM stops") -- a raw
        // stop at the slam SATISFIES that wait and the old track relaunches
        // on top of the boss theme.
        // Already primed = a script detonated the boss theme earlier in the
        // scene (Edwards's landing): leave the running theme completely
        // alone (per Joe: no ducking).
        if (!gWotBossIntroPrimed && boss->cutMusic)
            FadeOutMapMusic(4);
        LoadCompressedSpriteSheet(&sBiBannerSheet);
        LoadCompressedSpriteSheet(boss->nameSheet);
        LoadSpritePalette(&sBiBannerPalette);
        {
            struct SpritePalette portraitPal = { .tag = TAG_BI_PORTRAIT };
            const u8 *tiles;
            const u16 *pal;

            GetNpcPortraitGfx(boss->portraitId, &tiles, &pal);
            portraitPal.data = pal;
            LoadSpritePalette(&portraitPal);
            sBiPortraitImage.data = tiles;
            sBiPortraitImage.size = 0x800;
        }
        // Subpriority = OBJ-vs-OBJ layering (lower draws in front): the name
        // must sit ON the banner, never behind it.
        tSprBanner = CreateSprite(&sBiBannerTemplate, BI_BANNER_X_OFF, BI_BANNER_Y, 2);
        SetSubspriteTables(&gSprites[tSprBanner], sBiBannerSubTable);
        tSprName = CreateSprite(&sBiNameTemplate, BI_BANNER_X_OFF + BI_NAME_X_NUDGE, BI_BANNER_Y, 0);
        SetSubspriteTables(&gSprites[tSprName], sBiNameSubTable);
        gSprites[tSprName].invisible = TRUE;
        tSprPort = CreateSprite(&sBiPortraitTemplate, BI_PORTRAIT_X_OFF, BI_PORTRAIT_Y, 1);
        BeginNormalPaletteFade(BossIntro_FadeMask(), 0, 0, 10, boss->tintColor);
        tVelBanner = 64;
        tVelPort = 26;
        tState = BI_STATE_ENTER;
        break;
    case BI_STATE_ENTER:
        if (tElapsed >= 6)
            MoveTowardX(&gSprites[tSprPort], BI_PORTRAIT_X_ON, &tVelPort);
        if (tElapsed >= 14)
        {
            bool32 arrived = MoveTowardX(&gSprites[tSprBanner], BI_BANNER_X_ON, &tVelBanner);

            gSprites[tSprName].x = gSprites[tSprBanner].x + BI_NAME_X_NUDGE;
            if (arrived)
            {
                // The slam: name appears this frame under a 2-frame white pop.
                // No SE here unless a boss defines one -- a second PlaySE on
                // SE1 would cut the shimmer mid-ring. The battle theme drops a
                // hair AFTER (tMusicWait) so its dense opening chord can't
                // fight the shimmer for m4a channels.
                if (boss->seSlam != 0)
                    PlaySE(boss->seSlam);
                tMusicWait = (boss->battleBGM != 0 && !gWotBossIntroPrimed) ? 10 : 0;
                gSprites[tSprName].invisible = FALSE;
                BossIntro_FlashCardWhite();
                tFlash = 3;
                tHold = boss->holdFrames;
                tState = BI_STATE_HOLD;
            }
        }
        break;
    case BI_STATE_HOLD:
        if (tMusicWait > 0 && --tMusicWait == 0)
            BossIntro_StartBattleTheme(boss);
        MoveTowardX(&gSprites[tSprPort], BI_PORTRAIT_X_ON, &tVelPort);
        if (--tHold <= 0 || (tElapsed > 30 && (JOY_NEW(A_BUTTON | B_BUTTON))))
        {
            // A/B inside the 10-frame window must not lose the theme (and the
            // primed flag the battle transition depends on).
            if (tMusicWait > 0)
            {
                tMusicWait = 0;
                BossIntro_StartBattleTheme(boss);
            }
            tVelBanner = 6;
            tExitT = 0;
            tState = BI_STATE_EXIT;
        }
        break;
    case BI_STATE_EXIT:
        tExitT++;
        tVelBanner = tVelBanner * 5 / 4 + 1;
        if (tVelBanner > 40)
            tVelBanner = 40;
        gSprites[tSprBanner].x -= tVelBanner;
        gSprites[tSprName].x = gSprites[tSprBanner].x + BI_NAME_X_NUDGE;
        gSprites[tSprPort].x += tVelBanner;
        if (tExitT == 8)
            BeginNormalPaletteFade(BossIntro_FadeMask(), 0, 10, 0, boss->tintColor);
        if (tExitT > 8 && gSprites[tSprBanner].x <= BI_BANNER_X_OFF
         && gSprites[tSprPort].x >= BI_PORTRAIT_X_OFF && !gPaletteFade.active)
            tState = BI_STATE_DONE;
        break;
    case BI_STATE_DONE:
        DestroySprite(&gSprites[tSprBanner]);
        DestroySprite(&gSprites[tSprName]);
        DestroySprite(&gSprites[tSprPort]);
        FreeSpriteTilesByTag(TAG_BI_BANNER);
        FreeSpriteTilesByTag(TAG_BI_NAME);
        FreeSpritePaletteByTag(TAG_BI_BANNER);
        FreeSpritePaletteByTag(TAG_BI_PORTRAIT);
        DestroyTask(taskId);
        ScriptContext_Enable();   // resume the script -- exactly once, from here
        break;
    }
}

void BossIntro_Start(u32 bossId)
{
    u8 taskId;

    if (bossId >= BOSS_INTRO_COUNT)
        bossId = BOSS_INTRO_EDWARDS;
    // Do NOT reset gWotBossIntroPrimed here: when a script primed the theme
    // earlier in the scene (Edwards's landing), clearing it made the card
    // fade + RESTART the running song. The flag's only consumer-reset is
    // battle start.
    if (FuncIsActiveTask(Task_BossIntro))
    {
        // Double trigger: skip the card rather than deadlocking the script.
        ScriptContext_Enable();
        return;
    }
    taskId = CreateTask(Task_BossIntro, 80);
    gTasks[taskId].tBossId = bossId;
}

// bossintro macro: callnative + embedded waitstate (special+waitstate shape).
void BossIntro_StartFromScript(struct ScriptContext *ctx)
{
    u32 bossId = ScriptReadWord(ctx);

    BossIntro_Start(bossId);
}
