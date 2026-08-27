#include "global.h"
#include "bg.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "window.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "wot_randomizer.h"

// ============================================================================
//  WISHES OF TOMORROW -- RANDOMIZER SETTINGS SCREEN
//  Modelled on src/option_menu.c (same BG/window/frame scaffolding) so it
//  looks and behaves like a native options page: a title bar, a list of
//  toggles with their values on the right, and a description box that
//  explains whatever is highlighted.
//
//  Reached from two places:
//    * New Game, straight after the naming screen (CB2_WotRandomizerMenu);
//    * the PC in the player's bedroom, any time before the adventure has
//      really started (special WotOpenRandomizerMenu).
//
//  Everything is stored in flags, so nothing new is added to the save block.
// ============================================================================

#define tSelection      data[0]
#define tMaster         data[1]
#define tStarters       data[2]
#define tWild           data[3]
#define tTrainers       data[4]

enum
{
    MENUITEM_MASTER,
    MENUITEM_STARTERS,
    MENUITEM_WILD,
    MENUITEM_TRAINERS,
    MENUITEM_START,
    MENUITEM_COUNT,
};

enum
{
    WIN_HEADER,
    WIN_OPTIONS,
    WIN_DESC,
};

#define YPOS_OF(item) ((item) * 16)

static void Task_FadeIn(u8 taskId);
static void Task_ProcessInput(u8 taskId);
static void Task_FadeOut(u8 taskId);
static void HighlightItem(u8 selection);
static void DrawChoices(u8 taskId);
static s16 *ValueForRow(u8 taskId, u8 row);
static void DrawDescription(u8 selection);
static void DrawHeaderText(void);
static void DrawItemNames(void);
static void DrawBgWindowFrames(void);

static MainCallback sSavedCallback;

static const u8 sText_Title[]        = _("RANDOMIZER");
static const u8 sText_Off[]          = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}OFF");
static const u8 sText_On[]           = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}ON");
static const u8 sText_Random[]       = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}RANDOM");
static const u8 sText_Begin[]        = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}START THE GAME");

static const u8 *const sItemNames[MENUITEM_COUNT] =
{
    [MENUITEM_MASTER]   = COMPOUND_STRING("RANDOMIZER"),
    [MENUITEM_STARTERS] = COMPOUND_STRING("STARTER PKMN"),
    [MENUITEM_WILD]     = COMPOUND_STRING("WILD PKMN"),
    [MENUITEM_TRAINERS] = COMPOUND_STRING("TRAINER PKMN"),
    [MENUITEM_START]    = COMPOUND_STRING("START"),
};

static const u8 *const sDescriptions[MENUITEM_COUNT] =
{
    [MENUITEM_MASTER]   = COMPOUND_STRING("Play the game randomized.\nSettings below!"),
    [MENUITEM_STARTERS] = COMPOUND_STRING("The three sealed starters become\nsomething else entirely."),
    [MENUITEM_WILD]     = COMPOUND_STRING("Every wild Pokemon is swapped.\nLevels never change."),
    [MENUITEM_TRAINERS] = COMPOUND_STRING("Every Trainer's team is swapped.\nShadows keep their identity."),
    [MENUITEM_START]    = COMPOUND_STRING("Begin the adventure with these\nsettings. They are permanent."),
};

static const struct WindowTemplate sWinTemplates[] =
{
    [WIN_HEADER] = {
        .bg = 1, .tilemapLeft = 2, .tilemapTop = 1,
        .width = 26, .height = 2, .paletteNum = 1, .baseBlock = 2
    },
    [WIN_OPTIONS] = {
        .bg = 0, .tilemapLeft = 2, .tilemapTop = 5,
        .width = 26, .height = 10, .paletteNum = 1, .baseBlock = 0x36
    },
    [WIN_DESC] = {
        .bg = 0, .tilemapLeft = 2, .tilemapTop = 16,
        .width = 26, .height = 4, .paletteNum = 1, .baseBlock = 0x36 + (26 * 10)
    },
    DUMMY_WIN_TEMPLATE
};

static const struct BgTemplate sBgTemplates[] =
{
    { .bg = 1, .charBaseIndex = 1, .mapBaseIndex = 30, .screenSize = 0,
      .paletteMode = 0, .priority = 0, .baseTile = 0 },
    { .bg = 0, .charBaseIndex = 1, .mapBaseIndex = 31, .screenSize = 0,
      .paletteMode = 0, .priority = 1, .baseTile = 0 },
};

static const u16 sBg_Pal[]   = {RGB(17, 18, 31)};
static const u16 sText_Pal[] = {RGB(0, 0, 0),    RGB(26, 26, 25), RGB(31, 31, 31), RGB(15, 15, 15),
                                RGB(31, 31, 31), RGB(8, 8, 8),    RGB(31, 12, 12), RGB(31, 31, 31),
                                RGB(31, 31, 31), RGB(31, 24, 8),  RGB(31, 31, 31), RGB(31, 31, 31),
                                RGB(31, 31, 31), RGB(31, 31, 31), RGB(31, 31, 31), RGB(31, 31, 31)};

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

// The sub-toggles only mean anything while the master switch is on; they are
// drawn greyed out otherwise, which is also why they cannot be edited then.
static bool32 SubOptionsLive(u8 taskId)
{
    return gTasks[taskId].tMaster != 0;
}

// Local copy of option_menu.c's DrawOptionMenuChoice (static there). Style 1
// recolours the text to the "selected" red by overwriting the control bytes,
// which is why every string above carries a COLOR/SHADOW prefix.
static void DrawChoiceText(const u8 *text, u8 x, u8 y, u8 style)
{
    u8 dst[24];
    u16 i;

    for (i = 0; *text != EOS && i < ARRAY_COUNT(dst) - 1; i++)
        dst[i] = *(text++);

    if (style != 0)
    {
        dst[2] = TEXT_COLOR_RED;
        dst[5] = TEXT_COLOR_LIGHT_RED;
    }

    dst[i] = EOS;
    AddTextPrinterParameterized(WIN_OPTIONS, FONT_NORMAL, dst, x, y + 1, TEXT_SKIP_DRAW, NULL);
}

// One row = two values sitting side by side. The one in force is drawn red,
// the other plain. An INACTIVE row draws both plain, so a greyed-out row can
// never look like it has something selected.
static void DrawRow(const u8 *left, const u8 *right, u8 y, bool32 rightInForce, bool32 active)
{
    DrawChoiceText(left,  104, y, (active && !rightInForce) ? 1 : 0);
    DrawChoiceText(right, GetStringRightAlignXOffset(FONT_NORMAL, right, 198), y,
                   (active && rightInForce) ? 1 : 0);
}

static void DrawChoices(u8 taskId)
{
    bool32 live = SubOptionsLive(taskId);
    s16 *data = gTasks[taskId].data;
    u8 i;

    // Clear and redraw the labels too: values change colour as well as text, and
    // repainting the whole window is the only way to be sure nothing lingers.
    FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(1));
    for (i = 0; i < MENUITEM_COUNT; i++)
        AddTextPrinterParameterized(WIN_OPTIONS, FONT_NORMAL, sItemNames[i], 8, YPOS_OF(i) + 1, TEXT_SKIP_DRAW, NULL);

    DrawRow(sText_Off, sText_On,     YPOS_OF(MENUITEM_MASTER),   tMaster,   TRUE);
    DrawRow(sText_Off, sText_Random, YPOS_OF(MENUITEM_STARTERS), tStarters, live);
    DrawRow(sText_Off, sText_Random, YPOS_OF(MENUITEM_WILD),     tWild,     live);
    DrawRow(sText_Off, sText_Random, YPOS_OF(MENUITEM_TRAINERS), tTrainers, live);

    DrawChoiceText(sText_Begin, GetStringRightAlignXOffset(FONT_NORMAL, sText_Begin, 198),
                   YPOS_OF(MENUITEM_START), 1);

    CopyWindowToVram(WIN_OPTIONS, COPYWIN_GFX);
}

static void DrawDescription(u8 selection)
{
    FillWindowPixelBuffer(WIN_DESC, PIXEL_FILL(1));
    // y=0, not 1: FONT_NORMAL advances 15px a line, so two lines need all 32px
    // of this four-row window to clear the bottom of the screen.
    AddTextPrinterParameterized(WIN_DESC, FONT_NORMAL, sDescriptions[selection], 8, 0, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(WIN_DESC, COPYWIN_FULL);
}

void CB2_WotRandomizerMenu(void)
{
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
        gMain.state++;
        break;
    case 1:
        DmaClearLarge16(3, (void *)(VRAM), VRAM_SIZE, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
        ChangeBgX(0, 0, BG_COORD_SET);
        ChangeBgY(0, 0, BG_COORD_SET);
        ChangeBgX(1, 0, BG_COORD_SET);
        ChangeBgY(1, 0, BG_COORD_SET);
        InitWindows(sWinTemplates);
        DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG0);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_CLR);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_DARKEN);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 4);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        ShowBg(0);
        ShowBg(1);
        gMain.state++;
        break;
    case 2:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        gMain.state++;
        break;
    case 3:
        LoadBgTiles(1, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, 0x1A2);
        gMain.state++;
        break;
    case 4:
        LoadPalette(sBg_Pal, BG_PLTT_ID(0), sizeof(sBg_Pal));
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        gMain.state++;
        break;
    case 5:
        LoadPalette(sText_Pal, BG_PLTT_ID(1), sizeof(sText_Pal));
        gMain.state++;
        break;
    case 6:
        PutWindowTilemap(WIN_HEADER);
        DrawHeaderText();
        gMain.state++;
        break;
    case 7:
        PutWindowTilemap(WIN_DESC);
        gMain.state++;
        break;
    case 8:
        PutWindowTilemap(WIN_OPTIONS);
        DrawItemNames();
        gMain.state++;
        break;
    case 9:
        DrawBgWindowFrames();
        gMain.state++;
        break;
    case 10:
    {
        u8 taskId = CreateTask(Task_FadeIn, 0);

        gTasks[taskId].tSelection = 0;
        gTasks[taskId].tMaster    = FlagGet(FLAG_WOT_RANDOMIZER);
        gTasks[taskId].tStarters  = FlagGet(FLAG_WOT_RAND_STARTERS);
        gTasks[taskId].tWild      = FlagGet(FLAG_WOT_RAND_WILD);
        gTasks[taskId].tTrainers  = FlagGet(FLAG_WOT_RAND_TRAINERS);

        DrawChoices(taskId);
        DrawDescription(0);
        HighlightItem(0);
        CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
        gMain.state++;
        break;
    }
    case 11:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB);
        SetMainCallback2(MainCB2);
        return;
    }
}

// Entry point used by the New Game flow; the PC uses the special below.
void WotStartRandomizerMenu(MainCallback returnCallback)
{
    sSavedCallback = returnCallback;
    SetMainCallback2(CB2_WotRandomizerMenu);
}

static void Task_FadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_ProcessInput;
}

// Maps a row to the task word holding its value; NULL for rows with no value.
static s16 *ValueForRow(u8 taskId, u8 row)
{
    switch (row)
    {
    case MENUITEM_MASTER:   return &gTasks[taskId].tMaster;
    case MENUITEM_STARTERS: return &gTasks[taskId].tStarters;
    case MENUITEM_WILD:     return &gTasks[taskId].tWild;
    case MENUITEM_TRAINERS: return &gTasks[taskId].tTrainers;
    }
    return NULL;
}

static void Task_ProcessInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (JOY_NEW(A_BUTTON) && tSelection == MENUITEM_START)
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].func = Task_FadeOut;
        return;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].func = Task_FadeOut;
        return;
    }
    else if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        tSelection = (tSelection > 0) ? tSelection - 1 : MENUITEM_COUNT - 1;
        HighlightItem(tSelection);
        DrawDescription(tSelection);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        tSelection = (tSelection < MENUITEM_COUNT - 1) ? tSelection + 1 : 0;
        HighlightItem(tSelection);
        DrawDescription(tSelection);
    }
    else if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT | A_BUTTON))
    {
        s16 *value = ValueForRow(taskId, tSelection);
        s16 want;

        if (value == NULL)
            return;                                 // START row: A handled above
        if (tSelection != MENUITEM_MASTER && !SubOptionsLive(taskId))
            return;                                 // sub-rows are dead while the master is OFF

        // LEFT always means the left-hand value and RIGHT always the right-hand
        // one -- deliberately NO wrap-around, so pressing a direction can never
        // leave you unsure which side is actually in force. A still toggles.
        if (JOY_NEW(DPAD_LEFT))
            want = FALSE;
        else if (JOY_NEW(DPAD_RIGHT))
            want = TRUE;
        else
            want = !*value;

        if (want == *value)
            return;                                 // already there: no sound, no redraw

        *value = want;

        // Turning the whole thing on should be immediately useful, so the three
        // sub-options come on with it the first time.
        if (tSelection == MENUITEM_MASTER && want && !tStarters && !tWild && !tTrainers)
            tStarters = tWild = tTrainers = TRUE;

        PlaySE(SE_SELECT);
        DrawChoices(taskId);
    }
}

static void Task_FadeOut(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    // Commit. A master switch that is on with every sub-option off would be a
    // lie, so it is folded back to off.
    if (tMaster && (tStarters || tWild || tTrainers))
        FlagSet(FLAG_WOT_RANDOMIZER);
    else
        FlagClear(FLAG_WOT_RANDOMIZER);

    if (tStarters) FlagSet(FLAG_WOT_RAND_STARTERS); else FlagClear(FLAG_WOT_RAND_STARTERS);
    if (tWild)     FlagSet(FLAG_WOT_RAND_WILD);     else FlagClear(FLAG_WOT_RAND_WILD);
    if (tTrainers) FlagSet(FLAG_WOT_RAND_TRAINERS); else FlagClear(FLAG_WOT_RAND_TRAINERS);

    // The flags above are what the PC path needs. On the NEW GAME path they are
    // about to be wiped by NewGameInitData(), so park a copy outside the save
    // block; new_game.c re-applies it once the save block is initialised.
    WotQueueRandomizerSettings(tMaster && (tStarters || tWild || tTrainers),
                               tStarters, tWild, tTrainers);

    // Kick the fade once, then wait for it. (Calling BeginNormalPaletteFade
    // every frame would restart it forever and the screen would never leave.)
    if (!gTasks[taskId].data[5])
    {
        gTasks[taskId].data[5] = TRUE;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        return;
    }

    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        FreeAllWindowBuffers();
        SetMainCallback2(sSavedCallback);
    }
}

static void HighlightItem(u8 selection)
{
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(16, DISPLAY_WIDTH - 16));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(YPOS_OF(selection) + 40, YPOS_OF(selection) + 56));
}

static void DrawHeaderText(void)
{
    FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(1));
    AddTextPrinterParameterized(WIN_HEADER, FONT_NORMAL, sText_Title, 8, 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(WIN_HEADER, COPYWIN_FULL);
}

static void DrawItemNames(void)
{
    u8 i;

    FillWindowPixelBuffer(WIN_OPTIONS, PIXEL_FILL(1));
    for (i = 0; i < MENUITEM_COUNT; i++)
        AddTextPrinterParameterized(WIN_OPTIONS, FONT_NORMAL, sItemNames[i], 8, YPOS_OF(i) + 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(WIN_OPTIONS, COPYWIN_FULL);
}

#define TILE_TOP_CORNER_L 0x1A2
#define TILE_TOP_EDGE     0x1A3
#define TILE_TOP_CORNER_R 0x1A4
#define TILE_LEFT_EDGE    0x1A5
#define TILE_RIGHT_EDGE   0x1A7
#define TILE_BOT_CORNER_L 0x1A8
#define TILE_BOT_EDGE     0x1A9
#define TILE_BOT_CORNER_R 0x1AA

static void DrawBgWindowFrames(void)
{
    // title bar
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_L,  1,  0,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_EDGE,      2,  0, 27,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_R, 28,  0,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1,  1,  1,  2,  7);
    FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28,  1,  1,  2,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_L,  1,  3,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_EDGE,      2,  3, 27,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_R, 28,  3,  1,  1,  7);

    // settings list
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_L,  1,  4,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_EDGE,      2,  4, 26,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_R, 28,  4,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1,  5,  1, 10,  7);
    FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28,  5,  1, 10,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_L,  1, 15,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_EDGE,      2, 15, 26,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_R, 28, 15,  1,  1,  7);

    // Description box. Only the side edges are drawn: a top edge would have sat
    // ON the window's first text row (that is what clipped the second line --
    // 20 screen rows cannot hold three fully-bordered boxes), and the list's own
    // bottom border at row 15 already reads as the separator. The bottom edge
    // would land on row 20, off screen.
    FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1, 16,  1,  4,  7);
    FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28, 16,  1,  4,  7);

    CopyBgTilemapBufferToVram(1);
}
