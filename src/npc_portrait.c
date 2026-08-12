#include "global.h"
#include "npc_portrait.h"
#include "constants/speaker_names.h"
#include "sprite.h"
#include "main.h"
#include "gpu_regs.h"

// 64x64 character portraits shown beside the message box during important
// NPC dialogue (visual-novel style). One hardware sprite, one 16-color OBJ
// palette, shown/hidden by the showportrait/hideportrait script commands.
// The sprite persists across message boxes until hidden; it is defensively
// cleared when a script ends (src/script.c) and dies naturally on warp
// (map load resets all sprites - state here self-validates against that).

#define PORTRAIT_PAL_TAG   0x4B10
#define PORTRAIT_SIZE_4BPP 0x800 // 64x64 4bpp = 64 tiles

#define PORTRAIT_X_LEFT    40
#define PORTRAIT_X_RIGHT   200
#define PORTRAIT_Y         84  // sprite center; bottom edge (y=116) tucks under the dialogue
                               // frame + namebox plate -- no gap. OAM priority 1 puts the
                               // portrait BEHIND those BG0 windows (they overlap it), while
                               // it still draws over the map layers.

struct NpcPortrait
{
    const u8 *tiles;    // raw 4bpp, 64x64
    const u16 *palette; // 16 colors
};

static const u8 sPortraitPlaceholderGfx[] = INCGFX_U8("graphics/portraits/placeholder.png", ".4bpp");
static const u16 sPortraitPlaceholderPal[] = INCGFX_U16("graphics/portraits/placeholder.png", ".gbapal");
static const u8 sPortraitPlaceholder2Gfx[] = INCGFX_U8("graphics/portraits/placeholder2.png", ".4bpp");
static const u16 sPortraitPlaceholder2Pal[] = INCGFX_U16("graphics/portraits/placeholder2.png", ".gbapal");
static const u8 sPortraitClarksonGfx[] = INCGFX_U8("graphics/portraits/clarkson.png", ".4bpp");
static const u16 sPortraitClarksonPal[] = INCGFX_U16("graphics/portraits/clarkson.png", ".gbapal");
static const u8 sPortraitMinisterGfx[] = INCGFX_U8("graphics/portraits/minister.png", ".4bpp");
static const u16 sPortraitMinisterPal[] = INCGFX_U16("graphics/portraits/minister.png", ".gbapal");
static const u8 sPortraitRedFatalityGfx[] = INCGFX_U8("graphics/portraits/red_fatality.png", ".4bpp");
static const u16 sPortraitRedFatalityPal[] = INCGFX_U16("graphics/portraits/red_fatality.png", ".gbapal");
static const u8 sPortraitEdwardsGfx[] = INCGFX_U8("graphics/portraits/edwards.png", ".4bpp");
static const u16 sPortraitEdwardsPal[] = INCGFX_U16("graphics/portraits/edwards.png", ".gbapal");
static const u8 sPortraitMikmancGfx[] = INCGFX_U8("graphics/portraits/mikmanc.png", ".4bpp");
static const u16 sPortraitMikmancPal[] = INCGFX_U16("graphics/portraits/mikmanc.png", ".gbapal");
static const u8 sPortraitAllisonGfx[] = INCGFX_U8("graphics/portraits/allison.png", ".4bpp");
static const u16 sPortraitAllisonPal[] = INCGFX_U16("graphics/portraits/allison.png", ".gbapal");
static const u8 sPortraitYifferGfx[] = INCGFX_U8("graphics/portraits/yiffer.png", ".4bpp");
static const u16 sPortraitYifferPal[] = INCGFX_U16("graphics/portraits/yiffer.png", ".gbapal");
static const u8 sPortraitNessaGfx[] = INCGFX_U8("graphics/portraits/nessa.png", ".4bpp");
static const u16 sPortraitNessaPal[] = INCGFX_U16("graphics/portraits/nessa.png", ".gbapal");
static const u8 sPortraitDracoGfx[] = INCGFX_U8("graphics/portraits/draco.png", ".4bpp");
static const u16 sPortraitDracoPal[] = INCGFX_U16("graphics/portraits/draco.png", ".gbapal");

// Real art later = add the PNG, repoint that id's row. Nothing else changes.
// PORTRAIT_COBRA intentionally points at the second placeholder to prove the
// drop-in path works (see the UI features handoff acceptance criteria).
static const struct NpcPortrait sNpcPortraits[PORTRAIT_COUNT] =
{
    [PORTRAIT_PLACEHOLDER]     = {sPortraitPlaceholderGfx,  sPortraitPlaceholderPal},
    [PORTRAIT_CLARKSON_GENGAR] = {sPortraitClarksonGfx,     sPortraitClarksonPal},
    [PORTRAIT_MADAM_TSUJI]     = {sPortraitPlaceholderGfx,  sPortraitPlaceholderPal},
    [PORTRAIT_MUTRID_LEADER]   = {sPortraitPlaceholderGfx,  sPortraitPlaceholderPal},
    [PORTRAIT_RED_FATALITY]    = {sPortraitRedFatalityGfx,  sPortraitRedFatalityPal},
    [PORTRAIT_DRACO]           = {sPortraitDracoGfx,        sPortraitDracoPal},
    [PORTRAIT_COBRA]           = {sPortraitPlaceholder2Gfx, sPortraitPlaceholder2Pal},
    [PORTRAIT_MINISTER]        = {sPortraitMinisterGfx,     sPortraitMinisterPal},
    [PORTRAIT_EDWARDS]         = {sPortraitEdwardsGfx,      sPortraitEdwardsPal},
    [PORTRAIT_MIKMANC]         = {sPortraitMikmancGfx,      sPortraitMikmancPal},
    [PORTRAIT_ALLISON]         = {sPortraitAllisonGfx,      sPortraitAllisonPal},
    [PORTRAIT_YIFFER]          = {sPortraitYifferGfx,       sPortraitYifferPal},
    [PORTRAIT_NESSA]           = {sPortraitNessaGfx,        sPortraitNessaPal},
};

// Speaker-linked portraits: setspeaker with one of these SP_NAME_* ids
// auto-shows the portrait (and the namebox teardown auto-hides it). Explicit
// showportrait/hideportrait still works and is never auto-hidden.
static const struct { u8 speaker; u8 portrait; } sSpeakerPortraits[] =
{
    { SP_NAME_MINISTER,     PORTRAIT_MINISTER },
    { SP_NAME_RED_FATALITY, PORTRAIT_RED_FATALITY },
    { SP_NAME_EDWARDS,      PORTRAIT_EDWARDS },
    { SP_NAME_MIKMANC,      PORTRAIT_MIKMANC },
    { SP_NAME_ALLISON,      PORTRAIT_ALLISON },
    { SP_NAME_YIFFER,       PORTRAIT_YIFFER },
    { SP_NAME_CLARKSON,     PORTRAIT_CLARKSON_GENGAR },
    { SP_NAME_OPERATIVE,    PORTRAIT_NESSA },
    { SP_NAME_DRACO,        PORTRAIT_DRACO },
    // MASTER GEN and MOM are deliberately unbound: no portrait art exists, and
    // an unmatched speaker shows the nameplate alone (WotShowSpeakerPortrait
    // hides any auto portrait on a miss). Add art -> add a row here.
};

static bool8 sAutoPortrait = FALSE;

// Boss intro card (and future systems) borrow portrait art without touching
// the field-portrait sprite: hand out the raw tile/palette pointers.
void GetNpcPortraitGfx(u8 portraitId, const u8 **tiles, const u16 **palette)
{
    if (portraitId >= PORTRAIT_COUNT)
        portraitId = PORTRAIT_PLACEHOLDER;
    *tiles = sNpcPortraits[portraitId].tiles;
    *palette = sNpcPortraits[portraitId].palette;
}

// Hide only what auto-show put up; explicit portraits are left alone.
void WotHideAutoPortrait(void)
{
    if (sAutoPortrait)
    {
        HideNpcPortrait();
        sAutoPortrait = FALSE;
    }
}

// Called by SetSpeaker/SetSpeakerFromTrainer with the SP_NAME_* id.
void WotShowSpeakerPortrait(u32 speakerId)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sSpeakerPortraits); i++)
    {
        if (sSpeakerPortraits[i].speaker == speakerId)
        {
            ShowNpcPortrait(sSpeakerPortraits[i].portrait, PORTRAIT_LEFT);
            sAutoPortrait = TRUE;
            return;
        }
    }
    WotHideAutoPortrait();
}

static EWRAM_INIT u8 sPortraitSpriteId = SPRITE_NONE;
static EWRAM_DATA struct SpriteFrameImage sPortraitImage = {0};

static const struct OamData sOamData_Portrait =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .size = SPRITE_SIZE(64x64),
    .priority = 1,
};

// A real frame command is required: image-based sprites only DMA their frame
// when an ANIMCMD_FRAME executes (gDummySpriteAnimTable never loads one).
static const union AnimCmd sAnim_Portrait[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Portrait[] =
{
    sAnim_Portrait,
};

static const struct SpriteTemplate sSpriteTemplate_Portrait =
{
    .tileTag = TAG_NONE,
    .paletteTag = PORTRAIT_PAL_TAG,
    .oam = &sOamData_Portrait,
    .anims = sAnims_Portrait,
    .images = &sPortraitImage,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// The sprite pool is wiped wholesale on map load; our saved id may then be
// stale or reused. Only treat it as ours if it still points at our frame image.
static bool32 PortraitSpriteIsAlive(void)
{
    return sPortraitSpriteId != SPRITE_NONE
        && gSprites[sPortraitSpriteId].inUse
        && gSprites[sPortraitSpriteId].images == &sPortraitImage;
}

void HideNpcPortrait(void)
{
    // Undo the flash-darkness exemption (harmless when windows are off).
    ClearGpuRegBits(REG_OFFSET_WINOUT, WINOUT_WIN01_OBJ);
    if (PortraitSpriteIsAlive())
        DestroySprite(&gSprites[sPortraitSpriteId]);
    if (sPortraitSpriteId != SPRITE_NONE)
        FreeSpritePaletteByTag(PORTRAIT_PAL_TAG);
    sPortraitSpriteId = SPRITE_NONE;
}

void ShowNpcPortrait(u8 portraitId, u8 side)
{
    struct SpritePalette palette = { .tag = PORTRAIT_PAL_TAG };
    s16 x;

    if (!USE_NPC_PORTRAITS)
        return;
    if (portraitId >= PORTRAIT_COUNT)
        portraitId = PORTRAIT_PLACEHOLDER;

    HideNpcPortrait(); // showing over an existing portrait swaps cleanly

    sPortraitImage.data = sNpcPortraits[portraitId].tiles;
    sPortraitImage.size = PORTRAIT_SIZE_4BPP;
    palette.data = sNpcPortraits[portraitId].palette;
    LoadSpritePalette(&palette);

    sAutoPortrait = FALSE;
    // Flash-dark maps (the jail) mask sprites outside the light circle
    // (WINOUT = BG0 only). Let OBJ through while a portrait is up so the
    // face is visible in the dark; restored on hide. No-op on lit maps.
    SetGpuRegBits(REG_OFFSET_WINOUT, WINOUT_WIN01_OBJ);
    x = (side == PORTRAIT_LEFT) ? PORTRAIT_X_LEFT : PORTRAIT_X_RIGHT;
    sPortraitSpriteId = CreateSprite(&sSpriteTemplate_Portrait, x, PORTRAIT_Y, 0);
    if (sPortraitSpriteId == MAX_SPRITES)
    {
        FreeSpritePaletteByTag(PORTRAIT_PAL_TAG);
        sPortraitSpriteId = SPRITE_NONE;
    }
}
