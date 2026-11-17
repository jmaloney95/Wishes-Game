#include "global.h"
#include "battle_partner.h"
#include "bg.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "item_icon.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "npc_portrait.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_summary_screen.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "window.h"
#include "wot_mega_dimension.h"
#include "wot_partner_card.h"
#include "constants/battle_partner.h"
#include "constants/opponents.h"
#include "constants/portraits.h"
#include "constants/rgb.h"
#include "constants/songs.h"

// The partner card: Hotel Z opens it when you talk to a Z-A rival
// you have beaten. It shows their portrait, friendship level and runs, their
// perk, and their three Pokemon; you can reorder the three, look at each in
// the summary screen (and change moves there, the level-up relearner
// included), and invite them along or go solo.
//
// The art is generated offline; the layout numbers here
// mirror the ones used to draw it.

#define CARD_Y      108
#define CARD_W      52
#define CARD_H      40
#define BAR_TILE_X  1
#define BAR_TILE_Y  5
#define BAR_SEGS    4   // one per friendship level
#define BAR_SEG_TILES 4

#define TAG_CORNER      0x5A40
#define TAG_PORTRAIT    0x5A41
#define TAG_ITEM_FIRST  0x5A42
#define TAG_EMBLEM      0x5A48

enum
{
    WIN_NAME,
    WIN_INFO,
    WIN_PERK,
    WIN_LEVELS,
    WIN_BUTTON,
};

enum
{
    MODE_BROWSE,    // left/right over the three Pokemon and the button
    MODE_MENU,      // a Pokemon picked: SUMMARY / MOVE / BACK
    MODE_SWAP,      // MOVE: pick where it goes
};

enum
{
    MENU_SUMMARY,
    MENU_MOVE,
    MENU_BACK,
    MENU_COUNT,
};

#define TARGET_BUTTON   WOT_PARTNER_TEAM_SIZE
#define TARGET_COUNT    (WOT_PARTNER_TEAM_SIZE + 1)

struct PartnerCard
{
    MainCallback savedCallback;
    u8 rival;                               // 0-5, sRivals order
    u8 mode;
    u8 target;
    u8 menuSel;
    u8 swapFrom;
    u8 inputDelay;
    u8 result;                              // VAR_RESULT on exit
    bool8 teamBuilt;
    u8 slots[WOT_PARTNER_TEAM_SIZE];        // battle_partners.party slot at each position
    u8 cornerIds[4];
    u8 iconIds[WOT_PARTNER_TEAM_SIZE];
    u8 itemIds[WOT_PARTNER_TEAM_SIZE];
    struct Pokemon team[WOT_PARTNER_TEAM_SIZE];
    struct Pokemon partyBackup[PARTY_SIZE];
    u8 partyCountBackup;
    u16 *tilemap;
};

static EWRAM_DATA struct PartnerCard *sCard = NULL;
static EWRAM_DATA struct SpriteFrameImage sPortraitImage = {0};

static const u8 sCardX[WOT_PARTNER_TEAM_SIZE] = {8, 64, 120};
static const u8 sButton[4] = {180, 112, 232, 140};

static const u16 sBgTiles[] = INCGFX_U16("graphics/wot_partner_card/bg_tiles.png", ".4bpp");
static const u16 sBgPal[] = INCGFX_U16("graphics/wot_partner_card/bg_tiles.png", ".gbapal");
static const u16 sBgMap[] = INCBIN_U16("graphics/wot_partner_card/bg_map.bin");
static const u32 sCornerGfx[] = INCGFX_U32("graphics/wot_partner_card/corner.png", ".4bpp");
static const u16 sCornerPal[] = INCGFX_U16("graphics/wot_partner_card/corner.png", ".gbapal");
// Each partner's ability emblem, 16x16
// frames: bolt, clock, eye, heart, fist, wind. Lit at max friendship.
static const u32 sEmblemGfx[] = INCGFX_U32("graphics/wot_partner_card/emblems.png", ".4bpp");
static const u16 sEmblemPal[] = INCGFX_U16("graphics/wot_partner_card/emblems.png", ".gbapal");
static const u8 sEmblemFrame[WOT_PARTNER_SAVE_COUNT] = {0, 3, 1, 5, 2, 4}; // sRivals order

// BG0 text: 0 clear, 1 white, 2 shadow, 3 grey, 4 gold, 5-6 the name, 7 button shadow
#define TEXT_PAL_NAME 5
static const u16 sTextPal[] =
{
    RGB(0, 0, 0), RGB(31, 31, 31), RGB(2, 3, 6), RGB(21, 22, 26),
    RGB(31, 26, 10), RGB(31, 31, 31), RGB(2, 3, 6), RGB(4, 6, 15),
    RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0),
    RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0),
};
static const u8 sColorWhite[] = {TEXT_COLOR_TRANSPARENT, 1, 2};
static const u8 sColorGrey[]  = {TEXT_COLOR_TRANSPARENT, 3, 2};
static const u8 sColorGold[]  = {TEXT_COLOR_TRANSPARENT, 4, 2};
static const u8 sColorName[]  = {TEXT_COLOR_TRANSPARENT, 5, 6};
static const u8 sColorButton[] = {TEXT_COLOR_TRANSPARENT, 1, 7};

// Each rival's name in their nameplate colour (field_name_box.c), and its
// shadow. Emma's black name gets a light shadow to stand off the dark plate.
static const struct
{
    const u8 *name;
    u8 portrait;
    u16 color;
    u16 shadow;
} sRivalLooks[WOT_PARTNER_SAVE_COUNT] =
{
    {COMPOUND_STRING("CANARI"),   PORTRAIT_ZA_CANARI,   RGB(31, 27, 8),  RGB(2, 3, 6)},
    {COMPOUND_STRING("LIDA"),     PORTRAIT_ZA_LIDA,     RGB(17, 25, 31), RGB(2, 3, 6)},
    {COMPOUND_STRING("EMMA"),     PORTRAIT_ZA_EMMA,     RGB(3, 3, 4),    RGB(20, 22, 28)},
    {COMPOUND_STRING("TAUNIE"),   PORTRAIT_ZA_TAUNIE,   RGB(30, 30, 30), RGB(2, 3, 6)},
    {COMPOUND_STRING("TULIP"),    PORTRAIT_ZA_TULIP,    RGB(31, 18, 25), RGB(2, 3, 6)},
    {COMPOUND_STRING("JACINTHE"), PORTRAIT_ZA_JACINTHE, RGB(23, 15, 31), RGB(2, 3, 6)},
};

static const struct BgTemplate sBgTemplates[] =
{
    {.bg = 0, .charBaseIndex = 0, .mapBaseIndex = 31, .priority = 0},
    {.bg = 1, .charBaseIndex = 3, .mapBaseIndex = 30, .priority = 1},
};

static const struct WindowTemplate sWindowTemplates[] =
{
    [WIN_NAME]   = {.bg = 0, .tilemapLeft = 1,  .tilemapTop = 1,  .width = 17, .height = 2, .paletteNum = 15, .baseBlock = 1},
    [WIN_INFO]   = {.bg = 0, .tilemapLeft = 1,  .tilemapTop = 3,  .width = 19, .height = 5, .paletteNum = 15, .baseBlock = 35},
    [WIN_PERK]   = {.bg = 0, .tilemapLeft = 1,  .tilemapTop = 10, .width = 28, .height = 3, .paletteNum = 15, .baseBlock = 130},
    [WIN_LEVELS] = {.bg = 0, .tilemapLeft = 1,  .tilemapTop = 17, .width = 21, .height = 2, .paletteNum = 15, .baseBlock = 214},
    [WIN_BUTTON] = {.bg = 0, .tilemapLeft = 22, .tilemapTop = 14, .width = 7,  .height = 4, .paletteNum = 15, .baseBlock = 256},
    DUMMY_WIN_TEMPLATE
};

static const struct OamData sOam_Corner =
{
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 0,
};

static const struct SpriteTemplate sSpriteTemplate_Corner =
{
    .tileTag = TAG_CORNER,
    .paletteTag = TAG_CORNER,
    .oam = &sOam_Corner,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct OamData sOam_Portrait =
{
    .shape = SPRITE_SHAPE(64x64),
    .size = SPRITE_SIZE(64x64),
    .priority = 1,
};

// image-based sprites only load their frame on an ANIMCMD_FRAME (npc_portrait.c)
static const union AnimCmd sAnim_Portrait[] = {ANIMCMD_FRAME(0, 0), ANIMCMD_END};
static const union AnimCmd *const sAnims_Portrait[] = {sAnim_Portrait};

static const struct SpriteTemplate sSpriteTemplate_Portrait =
{
    .tileTag = TAG_NONE,
    .paletteTag = TAG_PORTRAIT,
    .oam = &sOam_Portrait,
    .anims = sAnims_Portrait,
    .images = &sPortraitImage,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const u8 sText_Friendship[] = _("FRIENDSHIP");
static const u8 sText_Lv[] = _("Lv {STR_VAR_1}");
static const u8 sText_Runs[] = _("PORTAL RUNS");
static const u8 sText_Next[] = _("next: {STR_VAR_1}");
static const u8 sText_Max[] = _("MAX");
static const u8 sText_NextPerk[] = _("Lv{STR_VAR_1} at {STR_VAR_2} runs: {STR_VAR_3}");
static const u8 sText_MaxPerk[] = _("ABILITY: {STR_VAR_1}");
static const u8 sText_MonLevel[] = _("{LV}{STR_VAR_1}");
static const u8 sText_Invite[] = _("INVITE");
static const u8 sText_GoSolo[] = _("GO SOLO");
static const u8 *const sMenuItems[MENU_COUNT] =
{
    COMPOUND_STRING("SUMMARY"),
    COMPOUND_STRING("MOVE"),
    COMPOUND_STRING("BACK"),
};
static const u8 sText_MenuHint[] = _("Moves can be changed in SUMMARY.");
static const u8 sText_SwapHint[] = _("Swap with which Pokémon?");
static const u8 sText_SwapHint2[] = _("They go out in this order.");

static void CB2_SetupCard(void);
static void CB2_ReturnFromSummary(void);
static void Task_CardInput(u8 taskId);

static void VBlankCB_Card(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void MainCB2_Card(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static bool32 IsCurrentPartner(void)
{
    return WotPartner_GetCurrent() == sCard->rival + 1u;
}

// The partner's three as the card shows them: FillPartnerParty writes into
// gPlayerParty[3..5], so the player's own Pokemon there are put back after.
static void BuildTeam(void)
{
    struct Pokemon saved[WOT_PARTNER_TEAM_SIZE];
    const u8 *order = WotPartner_GetOrder(sCard->rival);
    u32 best = WotMegaDim_GetBestLevel();
    u32 i;

    for (i = 0; i < WOT_PARTNER_TEAM_SIZE; i++)
        saved[i] = gPlayerParty[MULTI_PARTY_SIZE + i];
    FillPartnerParty(TRAINER_PARTNER(PARTNER_MEGADIM_CANARI) + sCard->rival);
    WotPartner_BuildTeam(sCard->rival, best, FALSE);
    for (i = 0; i < WOT_PARTNER_TEAM_SIZE; i++)
    {
        sCard->team[i] = gPlayerParty[MULTI_PARTY_SIZE + i];
        sCard->slots[i] = order[i];
        gPlayerParty[MULTI_PARTY_SIZE + i] = saved[i];
    }
    sCard->teamBuilt = TRUE;
}

// One segment per friendship level reached: Lv1 lights one, Lv4
// all four. Runs toward the next level show as numbers beside it, not here.
static void LightBar(void)
{
    u32 level = WotPartner_GetLevel(sCard->rival);
    u32 seg, x;

    for (seg = 0; seg < level && seg < BAR_SEGS; seg++)
    {
        for (x = 0; x < BAR_SEG_TILES; x++)
            sCard->tilemap[BAR_TILE_Y * 32 + BAR_TILE_X + seg * BAR_SEG_TILES + x] |= 1 << 12;
    }
}

static void Print(u32 win, u32 font, u32 x, u32 y, const u8 *colors, const u8 *str)
{
    AddTextPrinterParameterized3(win, font, x, y, colors, TEXT_SKIP_DRAW, str);
}

static void DrawInfo(void)
{
    u32 rival = sCard->rival, level = WotPartner_GetLevel(rival);

    FillWindowPixelBuffer(WIN_NAME, PIXEL_FILL(0));
    Print(WIN_NAME, FONT_NORMAL, 6, 1, sColorName, sRivalLooks[rival].name);
    CopyWindowToVram(WIN_NAME, COPYWIN_FULL);

    FillWindowPixelBuffer(WIN_INFO, PIXEL_FILL(0));
    Print(WIN_INFO, FONT_SMALL, 0, 3, sColorGrey, sText_Friendship);
    ConvertIntToDecimalStringN(gStringVar1, level, STR_CONV_MODE_LEFT_ALIGN, 1);
    StringExpandPlaceholders(gStringVar4, sText_Lv);
    Print(WIN_INFO, FONT_SMALL, 56, 3, sColorGold, gStringVar4);
    Print(WIN_INFO, FONT_SMALL, 0, 28, sColorGrey, sText_Runs);
    ConvertIntToDecimalStringN(gStringVar4, WotPartner_GetRuns(rival), STR_CONV_MODE_LEFT_ALIGN, 3);
    Print(WIN_INFO, FONT_SMALL, 64, 28, sColorWhite, gStringVar4);
    if (level >= 4)
    {
        Print(WIN_INFO, FONT_SMALL, 92, 28, sColorGold, sText_Max);
    }
    else
    {
        ConvertIntToDecimalStringN(gStringVar1, WotPartner_GetRunsForNextLevel(rival), STR_CONV_MODE_LEFT_ALIGN, 3);
        StringExpandPlaceholders(gStringVar4, sText_Next);
        Print(WIN_INFO, FONT_SMALL, 92, 28, sColorGrey, gStringVar4);
    }
    CopyWindowToVram(WIN_INFO, COPYWIN_FULL);
}

static void DrawButton(void)
{
    const u8 *label = IsCurrentPartner() ? sText_GoSolo : sText_Invite;
    s32 x = 30 - GetStringWidth(FONT_SMALL, label, 0) / 2;

    FillWindowPixelBuffer(WIN_BUTTON, PIXEL_FILL(0));
    Print(WIN_BUTTON, FONT_SMALL, x, 9, sColorButton, label);
    CopyWindowToVram(WIN_BUTTON, COPYWIN_FULL);
}

static void DrawLevels(void)
{
    u32 i;

    FillWindowPixelBuffer(WIN_LEVELS, PIXEL_FILL(0));
    for (i = 0; i < WOT_PARTNER_TEAM_SIZE; i++)
    {
        if (GetMonData(&sCard->team[i], MON_DATA_SPECIES) == SPECIES_NONE)
            continue;
        ConvertIntToDecimalStringN(gStringVar1, GetMonData(&sCard->team[i], MON_DATA_LEVEL), STR_CONV_MODE_LEFT_ALIGN, 3);
        StringExpandPlaceholders(gStringVar4, sText_MonLevel);
        Print(WIN_LEVELS, FONT_SMALL, sCardX[i] - 8 + 4, 0, sColorWhite, gStringVar4);
    }
    CopyWindowToVram(WIN_LEVELS, COPYWIN_FULL);
}

// The perk panel: the perk and the next level's change, or the Pokemon menu.
static void DrawPanel(void)
{
    u32 rival = sCard->rival, i, x;
    const u8 *next;

    FillWindowPixelBuffer(WIN_PERK, PIXEL_FILL(0));
    switch (sCard->mode)
    {
    case MODE_BROWSE:
        Print(WIN_PERK, FONT_SMALL, 6, 0, sColorWhite, WotPartner_GetPerkText(rival, WotPartner_GetLevel(rival)));
        next = WotPartner_GetNextPerkText(rival);
        if (next == NULL)
        {
            StringCopy(gStringVar1, WotPartner_GetAbilityName(rival));
            StringExpandPlaceholders(gStringVar4, sText_MaxPerk);
            Print(WIN_PERK, FONT_SMALL, 6, 11, sColorGold, gStringVar4);
        }
        else
        {
            ConvertIntToDecimalStringN(gStringVar1, WotPartner_GetLevel(rival) + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
            ConvertIntToDecimalStringN(gStringVar2, WotPartner_GetRunsForNextLevel(rival), STR_CONV_MODE_LEFT_ALIGN, 3);
            StringCopy(gStringVar3, next);
            StringExpandPlaceholders(gStringVar4, sText_NextPerk);
            Print(WIN_PERK, FONT_SMALL, 6, 11, sColorGrey, gStringVar4);
        }
        break;
    case MODE_MENU:
        for (i = 0, x = 6; i < MENU_COUNT; i++)
        {
            Print(WIN_PERK, FONT_SMALL, x, 0, (i == sCard->menuSel) ? sColorGold : sColorWhite, sMenuItems[i]);
            x += GetStringWidth(FONT_SMALL, sMenuItems[i], 0) + 16;
        }
        Print(WIN_PERK, FONT_SMALL, 6, 11, sColorGrey, sText_MenuHint);
        break;
    case MODE_SWAP:
        Print(WIN_PERK, FONT_SMALL, 6, 0, sColorGold, sText_SwapHint);
        Print(WIN_PERK, FONT_SMALL, 6, 11, sColorGrey, sText_SwapHint2);
        break;
    }
    CopyWindowToVram(WIN_PERK, COPYWIN_FULL);
}

static void MoveCorners(void)
{
    s32 x0, y0, x1, y1;
    u32 i;

    if (sCard->target == TARGET_BUTTON)
    {
        x0 = sButton[0];
        y0 = sButton[1];
        x1 = sButton[2];
        y1 = sButton[3];
    }
    else
    {
        x0 = sCardX[sCard->target];
        y0 = CARD_Y;
        x1 = x0 + CARD_W;
        y1 = y0 + CARD_H;
    }
    x0 -= 3; y0 -= 3; x1 += 3; y1 += 3;
    for (i = 0; i < 4; i++)
    {
        struct Sprite *corner = &gSprites[sCard->cornerIds[i]];

        corner->x = ((i & 1) ? x1 - 8 : x0) + 4;
        corner->y = ((i & 2) ? y1 - 8 : y0) + 4;
    }
}

static void CreateCorners(void)
{
    struct SpriteSheet sheet = {.data = sCornerGfx, .size = 32, .tag = TAG_CORNER};
    struct SpritePalette pal = {.data = sCornerPal, .tag = TAG_CORNER};
    u32 i;

    LoadSpriteSheet(&sheet);
    LoadSpritePalette(&pal);
    for (i = 0; i < 4; i++)
    {
        sCard->cornerIds[i] = CreateSprite(&sSpriteTemplate_Corner, 0, 0, 0);
        // bits 3/4 of matrixNum are the h/v flips when affine is off
        gSprites[sCard->cornerIds[i]].oam.matrixNum = ((i & 1) ? ST_OAM_HFLIP : 0) | ((i & 2) ? ST_OAM_VFLIP : 0);
    }
    MoveCorners();
}

static const struct OamData sOam_Emblem =
{
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 0,
};

static const struct SpriteTemplate sSpriteTemplate_Emblem =
{
    .tileTag = TAG_EMBLEM,
    .paletteTag = TAG_EMBLEM,
    .oam = &sOam_Emblem,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// The partner's ability emblem beside their friendship level: in colour at
// max friendship, a dim grey silhouette until then.
static void CreateEmblem(void)
{
    struct SpriteSheet sheet = {.data = sEmblemGfx, .size = sizeof(sEmblemGfx), .tag = TAG_EMBLEM};
    struct SpritePalette pal = {.data = sEmblemPal, .tag = TAG_EMBLEM};
    u16 grey[16];
    u8 spriteId;
    u32 i;

    if (WotPartner_GetLevel(sCard->rival) < FRIENDSHIP_MAX_LEVEL)
    {
        for (i = 0; i < 16; i++)
        {
            u32 c = sEmblemPal[i];
            u32 y = (GET_R(c) * 3 + GET_G(c) * 6 + GET_B(c)) / 10;

            y = 4 + y / 3;
            grey[i] = RGB(y, y, y + 1);
        }
        pal.data = grey;
    }
    LoadSpriteSheet(&sheet);
    LoadSpritePalette(&pal);
    spriteId = CreateSprite(&sSpriteTemplate_Emblem, 104, 32, 0);
    if (spriteId != MAX_SPRITES)
        gSprites[spriteId].oam.tileNum += sEmblemFrame[sCard->rival] * 4;
}

static void CreatePortrait(void)
{
    struct SpritePalette pal = {.tag = TAG_PORTRAIT};
    const u8 *tiles;
    const u16 *palette;

    GetNpcPortraitGfx(sRivalLooks[sCard->rival].portrait, &tiles, &palette);
    pal.data = palette;
    LoadSpritePalette(&pal);
    sPortraitImage.data = tiles;
    sPortraitImage.size = 0x800;
    CreateSprite(&sSpriteTemplate_Portrait, 164 + 32, 10 + 32, 2);
}

static void CreateTeamSprites(void)
{
    u32 i;

    LoadMonIconPalettes();
    for (i = 0; i < WOT_PARTNER_TEAM_SIZE; i++)
    {
        struct Pokemon *mon = &sCard->team[i];
        u16 species = GetMonData(mon, MON_DATA_SPECIES);
        u16 item = GetMonData(mon, MON_DATA_HELD_ITEM);

        sCard->iconIds[i] = MAX_SPRITES;
        sCard->itemIds[i] = MAX_SPRITES;
        if (species == SPECIES_NONE)
            continue;
        sCard->iconIds[i] = CreateMonIcon(species, SpriteCB_MonIcon, sCardX[i] + 18, CARD_Y + 13, 1, GetMonData(mon, MON_DATA_PERSONALITY));
        if (item != ITEM_NONE)
        {
            u8 spriteId = AddItemIconSprite(TAG_ITEM_FIRST + i, TAG_ITEM_FIRST + i, item);

            sCard->itemIds[i] = spriteId;
            if (spriteId != MAX_SPRITES)
            {
                gSprites[spriteId].x = sCardX[i] + 39;
                gSprites[spriteId].y = CARD_Y + 26;
                gSprites[spriteId].oam.priority = 0;
            }
        }
    }
}

static void CB2_SetupCard(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        gMain.state++;
        break;
    case 1:
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
        ResetAllBgsCoordinates();
        InitWindows(sWindowTemplates);
        DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        gMain.state++;
        break;
    case 2:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        FreeAllSpritePalettes();
        gMain.state++;
        break;
    case 3:
    {
        u16 lit[16];

        sCard->tilemap = AllocZeroed(BG_SCREEN_SIZE);
        LoadBgTiles(1, sBgTiles, sizeof(sBgTiles), 0);
        CpuCopy16(sBgMap, sCard->tilemap, BG_SCREEN_SIZE);
        if (!sCard->teamBuilt)
            BuildTeam();
        LightBar();
        SetBgTilemapBuffer(1, sCard->tilemap);
        CopyBgTilemapBufferToVram(1);
        LoadPalette(sBgPal, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        // palette 1: the same, with the bar's empty fill turned pink (lit segments)
        CpuCopy16(sBgPal, lit, sizeof(lit));
        lit[6] = RGB(29, 13, 19);
        LoadPalette(lit, BG_PLTT_ID(1), PLTT_SIZE_4BPP);
        LoadPalette(sTextPal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        LoadPalette(&sRivalLooks[sCard->rival].color, BG_PLTT_ID(15) + TEXT_PAL_NAME, PLTT_SIZEOF(1));
        LoadPalette(&sRivalLooks[sCard->rival].shadow, BG_PLTT_ID(15) + TEXT_PAL_NAME + 1, PLTT_SIZEOF(1));
        gMain.state++;
        break;
    }
    case 4:
    {
        u32 i;

        for (i = 0; i <= WIN_BUTTON; i++)
            PutWindowTilemap(i);
        DrawInfo();
        DrawPanel();
        DrawLevels();
        DrawButton();
        gMain.state++;
        break;
    }
    case 5:
        CreatePortrait();
        CreateEmblem();
        CreateTeamSprites();
        CreateCorners();
        gMain.state++;
        break;
    case 6:
        CreateTask(Task_CardInput, 0);
        sCard->inputDelay = 20;
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        ShowBg(0);
        ShowBg(1);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB_Card);
        SetMainCallback2(MainCB2_Card);
        gMain.state = 0;
        break;
    }
}

static void FreeCardScreen(void)
{
    FreeAllWindowBuffers();
    TRY_FREE_AND_SET_NULL(sCard->tilemap);
}

// Off to the summary screen with the partner's three standing in as the
// party, so its move swapping and the START relearner work on them as they
// are (both write to gPlayerParty).
static void Task_OpenSummary(u8 taskId)
{
    u32 i;

    if (gPaletteFade.active)
        return;
    DestroyTask(taskId);
    FreeCardScreen();
    for (i = 0; i < PARTY_SIZE; i++)
        sCard->partyBackup[i] = gPlayerParty[i];
    sCard->partyCountBackup = gPlayerPartyCount;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (i < WOT_PARTNER_TEAM_SIZE)
            gPlayerParty[i] = sCard->team[i];
        else
            ZeroMonData(&gPlayerParty[i]);
    }
    gPlayerPartyCount = WOT_PARTNER_TEAM_SIZE;
    ShowPokemonSummaryScreen(SUMMARY_MODE_NORMAL, gPlayerParty, sCard->target, WOT_PARTNER_TEAM_SIZE - 1, CB2_ReturnFromSummary);
}

static void CB2_ReturnFromSummary(void)
{
    u32 i;

    for (i = 0; i < WOT_PARTNER_TEAM_SIZE; i++)
    {
        sCard->team[i] = gPlayerParty[i];
        WotPartner_SaveMoves(sCard->rival, sCard->slots[i], &gPlayerParty[i]);
    }
    for (i = 0; i < PARTY_SIZE; i++)
        gPlayerParty[i] = sCard->partyBackup[i];
    gPlayerPartyCount = sCard->partyCountBackup;
    sCard->mode = MODE_BROWSE;
    gMain.state = 0;
    SetMainCallback2(CB2_SetupCard);
}

static void Task_CloseCard(u8 taskId)
{
    MainCallback callback;

    if (gPaletteFade.active)
        return;
    DestroyTask(taskId);
    FreeCardScreen();
    gSpecialVar_Result = sCard->result;
    callback = sCard->savedCallback;
    FREE_AND_SET_NULL(sCard);
    SetMainCallback2(callback);
}

static void FadeOutTo(TaskFunc next, u8 taskId)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = next;
}

static void SwapTeam(u32 a, u32 b)
{
    struct Pokemon mon;
    u8 slot;

    mon = sCard->team[a];
    sCard->team[a] = sCard->team[b];
    sCard->team[b] = mon;
    slot = sCard->slots[a];
    sCard->slots[a] = sCard->slots[b];
    sCard->slots[b] = slot;
    WotPartner_SetOrder(sCard->rival, sCard->slots);
}

// The icons and item icons are recreated in their new places.
static void RedrawTeam(void)
{
    u32 i;

    for (i = 0; i < WOT_PARTNER_TEAM_SIZE; i++)
    {
        if (sCard->iconIds[i] != MAX_SPRITES)
            FreeAndDestroyMonIconSprite(&gSprites[sCard->iconIds[i]]);
        if (sCard->itemIds[i] != MAX_SPRITES)
            DestroySprite(&gSprites[sCard->itemIds[i]]);
        FreeSpriteTilesByTag(TAG_ITEM_FIRST + i);
        FreeSpritePaletteByTag(TAG_ITEM_FIRST + i);
    }
    FreeMonIconPalettes();
    CreateTeamSprites();
    DrawLevels();
}

static void Task_CardInput(u8 taskId)
{
    s32 step = 0;
    u32 count;

    if (gPaletteFade.active)
        return;
    if (sCard->inputDelay != 0)
    {
        sCard->inputDelay--;
        return;
    }

    if (JOY_NEW(DPAD_LEFT))
        step = -1;
    else if (JOY_NEW(DPAD_RIGHT))
        step = 1;

    switch (sCard->mode)
    {
    case MODE_BROWSE:
    case MODE_SWAP:
        count = (sCard->mode == MODE_SWAP) ? WOT_PARTNER_TEAM_SIZE : TARGET_COUNT;
        if (step != 0)
        {
            PlaySE(SE_SELECT);
            sCard->target = (sCard->target + count + step) % count;
            MoveCorners();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (sCard->mode == MODE_SWAP)
            {
                if (sCard->target != sCard->swapFrom)
                {
                    SwapTeam(sCard->swapFrom, sCard->target);
                    RedrawTeam();
                }
                sCard->mode = MODE_BROWSE;
                DrawPanel();
            }
            else if (sCard->target == TARGET_BUTTON)
            {
                // invite them along, or (already the partner) go solo
                sCard->result = IsCurrentPartner() ? 2 : 1;
                WotPartner_SetCurrent(IsCurrentPartner() ? 0 : sCard->rival + 1);
                FadeOutTo(Task_CloseCard, taskId);
            }
            else
            {
                sCard->mode = MODE_MENU;
                sCard->menuSel = MENU_SUMMARY;
                DrawPanel();
            }
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            if (sCard->mode == MODE_SWAP)
            {
                sCard->target = sCard->swapFrom;
                sCard->mode = MODE_BROWSE;
                MoveCorners();
                DrawPanel();
            }
            else
            {
                sCard->result = 0;
                FadeOutTo(Task_CloseCard, taskId);
            }
        }
        break;
    case MODE_MENU:
        if (step != 0)
        {
            PlaySE(SE_SELECT);
            sCard->menuSel = (sCard->menuSel + MENU_COUNT + step) % MENU_COUNT;
            DrawPanel();
        }
        else if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            switch (sCard->menuSel)
            {
            case MENU_SUMMARY:
                FadeOutTo(Task_OpenSummary, taskId);
                return;
            case MENU_MOVE:
                sCard->mode = MODE_SWAP;
                sCard->swapFrom = sCard->target;
                break;
            default:
                sCard->mode = MODE_BROWSE;
                break;
            }
            DrawPanel();
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            sCard->mode = MODE_BROWSE;
            DrawPanel();
        }
        break;
    }
}

// Script special: VAR_0x8004 = the rival (1-6). Use after fadescreen, then
// waitstate. VAR_RESULT: 0 = closed, 1 = invited along, 2 = went solo.
void WotPartnerCard_Open(void)
{
    gSpecialVar_Result = 0;
    if (gSpecialVar_0x8004 == 0 || gSpecialVar_0x8004 > WOT_PARTNER_SAVE_COUNT
     || (sCard = AllocZeroed(sizeof(*sCard))) == NULL)
    {
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        return;
    }
    sCard->rival = gSpecialVar_0x8004 - 1;
    sCard->savedCallback = CB2_ReturnToFieldContinueScriptPlayMapMusic;
    gMain.state = 0;
    SetMainCallback2(CB2_SetupCard);
}
