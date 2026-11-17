#include "global.h"
#include "bg.h"
#include "decompress.h"
#include "gpu_regs.h"
#include "item.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "trainer_pokemon_sprites.h"
#include "window.h"
#include "wot_mega_dimension.h"
#include "wot_mega_log.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/rgb.h"
#include "constants/songs.h"

// ============================================================================
//  WISHES OF TOMORROW -- THE MEGA LOG
//
//  START -> LOG. A trophy case of everything the rifts hold: the 23 rogue Megas
//  and Mable's legends, as a grid of icons. Unclaimed ones are silhouettes
//  until claimed; the cursor's one shows large beside the grid, with its stone
//  and ability. It reads the dimension's claim flags and the redeemed Mystery
//  Codes (WotMegaLog_GetEntry), nothing more.
//  Art and the layout constants
// ============================================================================

#define GRID_COLS       6
#define GRID_ROWS       5
#define GRID_X          8
#define GRID_Y          27
#define CELL_PITCH_X    24
#define CELL_PITCH_Y    25
#define CELL_W          22
#define CELL_H          24
#define PIC_X           194
#define PIC_Y           44

#define TAG_CORNER      0x5A60
#define TAG_SILHOUETTE  0x5A61

#define CELL_NONE       0xFF
#define ENTRY_COUNT     (WotMegaLog_RogueCount() + WotMegaLog_LegendCount())
#define MAX_ENTRIES     (GRID_COLS * GRID_ROWS)

enum
{
    WIN_HEADER,
    WIN_INFO,
};

struct MegaLog
{
    u8 cursor;      // entry
    u8 iconIds[MAX_ENTRIES];
    u8 cornerIds[4];
    u16 picSpriteId;
    u16 *tilemap;
};

static EWRAM_DATA struct MegaLog *sLog = NULL;

static const u16 sBgTiles[] = INCGFX_U16("graphics/wot_mega_log/bg_tiles.png", ".4bpp");
static const u16 sBgPal[] = INCGFX_U16("graphics/wot_mega_log/bg_tiles.png", ".gbapal");
static const u16 sBgMap[] = INCBIN_U16("graphics/wot_mega_log/bg_map.bin");
static const u32 sCornerGfx[] = INCGFX_U32("graphics/wot_partner_card/corner.png", ".4bpp");
static const u16 sCornerPal[] = INCGFX_U16("graphics/wot_partner_card/corner.png", ".gbapal");

// BG0 text: 0 clear, 1 white, 2 shadow, 3 grey, 4 gold
static const u16 sTextPal[] =
{
    RGB(0, 0, 0), RGB(31, 31, 31), RGB(2, 3, 6), RGB(21, 22, 26),
    RGB(31, 26, 10), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0),
    RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0),
    RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0),
};
static const u8 sColorWhite[] = {TEXT_COLOR_TRANSPARENT, 1, 2};
static const u8 sColorGrey[]  = {TEXT_COLOR_TRANSPARENT, 3, 2};
static const u8 sColorGold[]  = {TEXT_COLOR_TRANSPARENT, 4, 2};

// The shape an unclaimed Pokemon shows as: darker than its grid cell, lighter
// than the frame around the big picture.
#define SILHOUETTE_COLOR_ICON RGB(2, 3, 8)
#define SILHOUETTE_COLOR_PIC  RGB(5, 7, 15)

// The grid, entry at each cell: four rows of rogue Megas, then the legends
// in story order. Entries 23+ are sLegends' rows: Groudon, Kyogre, Rayquaza,
// Zygarde, Lugia, Ho-Oh.
static const u8 sCells[GRID_ROWS][GRID_COLS] =
{
    {0, 1, 2, 3, 4, 5},
    {6, 7, 8, 9, 10, 11},
    {12, 13, 14, 15, 16, 17},
    {18, 19, 20, 21, 22, CELL_NONE},
    {23, 24, 27, 28, 25, 26},
};

static const struct BgTemplate sBgTemplates[] =
{
    {.bg = 0, .charBaseIndex = 0, .mapBaseIndex = 31, .priority = 0},
    {.bg = 1, .charBaseIndex = 3, .mapBaseIndex = 30, .priority = 1},
};

static const struct WindowTemplate sWindowTemplates[] =
{
    [WIN_HEADER] = {.bg = 0, .tilemapLeft = 1,  .tilemapTop = 1,  .width = 17, .height = 2, .paletteNum = 15, .baseBlock = 1},
    [WIN_INFO]   = {.bg = 0, .tilemapLeft = 20, .tilemapTop = 11, .width = 9,  .height = 8, .paletteNum = 15, .baseBlock = 35},
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

static const u8 sText_Title[] = _("MEGA LOG");
static const u8 sText_Unknown[] = _("?????");
static const u8 sText_NotYet[] = _("Still loose in\nthe rifts.");
static const u8 sText_NotYetLegend[] = _("Mable is still\nlooking for it.");
static const u8 sText_Prefix_Mega[] = _("Mega ");
static const u8 sText_Prefix_Primal[] = _("Primal ");

static void Task_MegaLogInput(u8 taskId);

static void VBlankCB_MegaLog(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void MainCB2_MegaLog(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void GetEntryCell(u32 entry, s32 *col, s32 *row)
{
    s32 r, c;

    for (r = 0; r < GRID_ROWS; r++)
    {
        for (c = 0; c < GRID_COLS; c++)
        {
            if (sCells[r][c] == entry)
            {
                *col = c;
                *row = r;
                return;
            }
        }
    }
    *col = 0;
    *row = 0;
}

static void DrawHeader(void)
{
    u32 i, rogues = WotMegaLog_RogueCount(), rogueClaimed = 0, legendClaimed = 0;
    struct WotMegaLogEntry entry;
    u8 *str = gStringVar4;

    for (i = 0; i < ENTRY_COUNT; i++)
    {
        WotMegaLog_GetEntry(i, &entry);
        if (entry.claimed && i < rogues)
            rogueClaimed++;
        else if (entry.claimed)
            legendClaimed++;
    }
    FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(0));
    AddTextPrinterParameterized3(WIN_HEADER, FONT_NORMAL, 6, 1, sColorWhite, TEXT_SKIP_DRAW, sText_Title);
    str = ConvertIntToDecimalStringN(str, rogueClaimed, STR_CONV_MODE_LEFT_ALIGN, 2);
    *str++ = CHAR_SLASH;
    str = ConvertIntToDecimalStringN(str, rogues, STR_CONV_MODE_LEFT_ALIGN, 2);
    *str++ = CHAR_SPACE;
    *str++ = CHAR_SPACE;
    str = ConvertIntToDecimalStringN(str, legendClaimed, STR_CONV_MODE_LEFT_ALIGN, 1);
    *str++ = CHAR_SLASH;
    ConvertIntToDecimalStringN(str, WotMegaLog_LegendCount(), STR_CONV_MODE_LEFT_ALIGN, 1);
    AddTextPrinterParameterized3(WIN_HEADER, FONT_SMALL, 72, 3, sColorGold, TEXT_SKIP_DRAW, gStringVar4);
    CopyWindowToVram(WIN_HEADER, COPYWIN_FULL);
}

static void DrawInfo(const struct WotMegaLogEntry *entry)
{
    bool32 isLegend = sLog->cursor >= WotMegaLog_RogueCount();

    FillWindowPixelBuffer(WIN_INFO, PIXEL_FILL(0));
    if (!entry->claimed)
    {
        AddTextPrinterParameterized3(WIN_INFO, FONT_SMALL, 2, 2, sColorGold, TEXT_SKIP_DRAW, sText_Unknown);
        AddTextPrinterParameterized3(WIN_INFO, FONT_SMALL, 2, 18, sColorGrey, TEXT_SKIP_DRAW, isLegend ? sText_NotYetLegend : sText_NotYet);
    }
    else
    {
        u8 *str = StringCopy(gStringVar4, entry->primal ? sText_Prefix_Primal : sText_Prefix_Mega);

        StringCopy(str, GetSpeciesName(entry->iconSpecies));
        AddTextPrinterParameterized3(WIN_INFO, FONT_SMALL, 2, 2, sColorGold, TEXT_SKIP_DRAW, gStringVar4);
        if (entry->stone != ITEM_NONE)
            AddTextPrinterParameterized3(WIN_INFO, FONT_SMALL, 2, 18, sColorWhite, TEXT_SKIP_DRAW, GetItemName(entry->stone));
        AddTextPrinterParameterized3(WIN_INFO, FONT_SMALL, 2, 34, sColorGrey, TEXT_SKIP_DRAW, gAbilitiesInfo[gSpeciesInfo[entry->species].abilities[0]].name);
    }
    CopyWindowToVram(WIN_INFO, COPYWIN_FULL);
}

// The cursor's Pokemon, large. A silhouette paints its palette one colour.
static void ShowPic(const struct WotMegaLogEntry *entry)
{
    if (sLog->picSpriteId != 0xFFFF)
        FreeAndDestroyMonPicSprite(sLog->picSpriteId);
    // The helper loads the palette under the species id, so that's the tag.
    sLog->picSpriteId = CreateMonPicSprite(entry->species, FALSE, 0, TRUE, PIC_X, PIC_Y, 0, entry->species);
    if (sLog->picSpriteId == 0xFFFF)
        return;
    gSprites[sLog->picSpriteId].oam.priority = 0;
    if (!entry->claimed)
    {
        u16 silhouette[16];

        CpuFill16(SILHOUETTE_COLOR_PIC, silhouette, sizeof(silhouette));
        LoadPalette(silhouette, OBJ_PLTT_ID(gSprites[sLog->picSpriteId].oam.paletteNum), PLTT_SIZE_4BPP);
    }
}

static void MoveCorners(void)
{
    s32 col, row, x0, y0, x1, y1;
    u32 i;

    GetEntryCell(sLog->cursor, &col, &row);
    x0 = GRID_X + col * CELL_PITCH_X - 1;
    y0 = GRID_Y + row * CELL_PITCH_Y - 1;
    x1 = x0 + CELL_W + 2;
    y1 = y0 + CELL_H + 2;
    for (i = 0; i < 4; i++)
    {
        struct Sprite *corner = &gSprites[sLog->cornerIds[i]];

        corner->x = ((i & 1) ? x1 - 8 : x0) + 4;
        corner->y = ((i & 2) ? y1 - 8 : y0) + 4;
    }
}

static void SetCursor(u32 entryIndex)
{
    struct WotMegaLogEntry entry;

    // Only the cursor's icon bobs.
    if (sLog->iconIds[sLog->cursor] != MAX_SPRITES)
        gSprites[sLog->iconIds[sLog->cursor]].callback = SpriteCallbackDummy;
    sLog->cursor = entryIndex;
    if (sLog->iconIds[entryIndex] != MAX_SPRITES)
        gSprites[sLog->iconIds[entryIndex]].callback = SpriteCB_MonIcon;
    WotMegaLog_GetEntry(entryIndex, &entry);
    MoveCorners();
    ShowPic(&entry);
    DrawInfo(&entry);
}

static void CreateGridSprites(void)
{
    static const u16 sSilhouettePal[16] =
    {
        0, SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON,
        SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON,
        SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON, SILHOUETTE_COLOR_ICON,
    };
    struct SpritePalette silhouette = {.data = sSilhouettePal, .tag = TAG_SILHOUETTE};
    struct SpriteSheet cornerSheet = {.data = sCornerGfx, .size = 32, .tag = TAG_CORNER};
    struct SpritePalette cornerPal = {.data = sCornerPal, .tag = TAG_CORNER};
    u8 silhouetteSlot;
    u32 i;

    LoadMonIconPalettes();
    silhouetteSlot = LoadSpritePalette(&silhouette);
    for (i = 0; i < ENTRY_COUNT; i++)
    {
        struct WotMegaLogEntry entry;
        s32 col, row;

        WotMegaLog_GetEntry(i, &entry);
        GetEntryCell(i, &col, &row);
        sLog->iconIds[i] = CreateMonIcon(entry.iconSpecies, SpriteCallbackDummy,
                                         GRID_X + col * CELL_PITCH_X + CELL_W / 2,
                                         GRID_Y + row * CELL_PITCH_Y + CELL_H / 2, 1, 0);
        if (sLog->iconIds[i] != MAX_SPRITES && !entry.claimed)
            gSprites[sLog->iconIds[i]].oam.paletteNum = silhouetteSlot;
    }

    LoadSpriteSheet(&cornerSheet);
    LoadSpritePalette(&cornerPal);
    for (i = 0; i < 4; i++)
    {
        sLog->cornerIds[i] = CreateSprite(&sSpriteTemplate_Corner, 0, 0, 0);
        // bits 3/4 of matrixNum are the h/v flips when affine is off
        gSprites[sLog->cornerIds[i]].oam.matrixNum = ((i & 1) ? ST_OAM_HFLIP : 0) | ((i & 2) ? ST_OAM_VFLIP : 0);
    }
}

static void CB2_SetupMegaLog(void)
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
        ResetAllPicSprites();
        gMain.state++;
        break;
    case 3:
        sLog->tilemap = AllocZeroed(BG_SCREEN_SIZE);
        LoadBgTiles(1, sBgTiles, sizeof(sBgTiles), 0);
        CpuCopy16(sBgMap, sLog->tilemap, BG_SCREEN_SIZE);
        SetBgTilemapBuffer(1, sLog->tilemap);
        CopyBgTilemapBufferToVram(1);
        LoadPalette(sBgPal, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        LoadPalette(sTextPal, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        gMain.state++;
        break;
    case 4:
        PutWindowTilemap(WIN_HEADER);
        PutWindowTilemap(WIN_INFO);
        DrawHeader();
        gMain.state++;
        break;
    case 5:
        CreateGridSprites();
        sLog->picSpriteId = 0xFFFF;
        sLog->cursor = 0;
        SetCursor(0);
        gMain.state++;
        break;
    case 6:
        CreateTask(Task_MegaLogInput, 0);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        ShowBg(0);
        ShowBg(1);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB_MegaLog);
        SetMainCallback2(MainCB2_MegaLog);
        gMain.state = 0;
        break;
    }
}

static void Task_CloseMegaLog(u8 taskId)
{
    if (gPaletteFade.active)
        return;
    DestroyTask(taskId);
    if (sLog->picSpriteId != 0xFFFF)
        FreeAndDestroyMonPicSprite(sLog->picSpriteId);
    FreeAllWindowBuffers();
    TRY_FREE_AND_SET_NULL(sLog->tilemap);
    FREE_AND_SET_NULL(sLog);
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

// One step on the grid. Across a row the gap is skipped; into the gap from
// above or below lands on the next filled cell. Wraps at the edges.
static u32 StepCursor(s32 dx, s32 dy)
{
    s32 col, row, n;

    GetEntryCell(sLog->cursor, &col, &row);
    if (dx != 0)
    {
        for (n = 0; n < GRID_COLS; n++)
        {
            col = (col + dx + GRID_COLS) % GRID_COLS;
            if (sCells[row][col] != CELL_NONE)
                break;
        }
    }
    else
    {
        row = (row + dy + GRID_ROWS) % GRID_ROWS;
        for (n = 0; sCells[row][col] == CELL_NONE && n < GRID_COLS; n++)
            col = (col + 1) % GRID_COLS;
    }
    return sCells[row][col];
}

static void Task_MegaLogInput(u8 taskId)
{
    u32 next = sLog->cursor;
    struct WotMegaLogEntry entry;

    if (gPaletteFade.active)
        return;
    if (JOY_NEW(B_BUTTON | START_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_CloseMegaLog;
        return;
    }
    if (JOY_NEW(A_BUTTON))
    {
        WotMegaLog_GetEntry(sLog->cursor, &entry);
        if (entry.claimed)
            PlayCry_Normal(entry.species, 0);
        return;
    }
    if (JOY_NEW(DPAD_LEFT))
        next = StepCursor(-1, 0);
    else if (JOY_NEW(DPAD_RIGHT))
        next = StepCursor(1, 0);
    else if (JOY_NEW(DPAD_UP))
        next = StepCursor(0, -1);
    else if (JOY_NEW(DPAD_DOWN))
        next = StepCursor(0, 1);
    if (next != sLog->cursor)
    {
        PlaySE(SE_SELECT);
        SetCursor(next);
    }
}

// Script special: use after fadescreen, then waitstate.
void WotMegaLog_Open(void)
{
    if ((sLog = AllocZeroed(sizeof(*sLog))) == NULL)
    {
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
        return;
    }
    gMain.state = 0;
    SetMainCallback2(CB2_SetupMegaLog);
}

STATIC_ASSERT(ARRAY_COUNT(sCells[0]) * ARRAY_COUNT(sCells) <= MAX_ENTRIES, MegaLogGridFits);
