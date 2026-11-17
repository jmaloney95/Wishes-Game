#include "global.h"
#include "wot_nuzlocke.h"
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
//  WISHES OF TOMORROW -- NEW GAME SETTINGS / RANDOMIZER SCREEN
//  Modelled on src/option_menu.c (same BG/window/frame scaffolding and the
//  same text colours) so it reads like the OPTION page: one framed panel with
//  a title, a list of settings with their values on the right, and a help box
//  at the bottom that explains whatever is highlighted.
//
//  Two ways in:
//    * NEW GAME, straight after the naming screen (WotStartNewGameSettings):
//      FAST FORWARD, LEVEL CAP and the three randomizer switches, all on their
//      defaults, then START THE GAME;
//    * the PC in the player's bedroom, any time before the starter is taken
//      (special WotOpenRandomizerMenu -> WotStartRandomizerMenu): just the
//      three randomizer switches and DONE.
//
//  Nothing new goes into the save: fast forward is the existing option, the
//  level cap and the randomizer are existing flags.
// ============================================================================

#define tSelection      data[0]     // index into the mode's row list
#define tInputDelay     data[1]
#define tFadeStarted    data[2]
#define VALUE_SLOT(kind) (3 + (kind))   // data[3..] hold one value per row kind
#define tScroll         data[15]    // the first row on screen

enum
{
    ROW_FAST_FORWARD,
    ROW_LEVEL_CAP,
    ROW_NUZLOCKE,
    ROW_STARTERS,
    ROW_WILD,
    ROW_TRAINERS,
    ROW_START,
    ROW_KIND_COUNT,
};

enum
{
    WIN_HEADER,
    WIN_LIST,
    WIN_DESC,
};

// FAST FORWARD as the row shows it, left to right. The save numbers these
// differently (OPTIONS_FAST_FORWARD_*), see FastForwardToSave.
enum
{
    FF_OFF,
    FF_2X,
    FF_3X,
    FF_COUNT,
};

#if WOT_NUZLOCKE
static const u8 sNewGameRows[] = {ROW_FAST_FORWARD, ROW_LEVEL_CAP, ROW_NUZLOCKE, ROW_STARTERS, ROW_WILD, ROW_TRAINERS, ROW_START};
static const u8 sPcRows[]      = {ROW_NUZLOCKE, ROW_STARTERS, ROW_WILD, ROW_TRAINERS, ROW_START};
#else
static const u8 sNewGameRows[] = {ROW_FAST_FORWARD, ROW_LEVEL_CAP, ROW_STARTERS, ROW_WILD, ROW_TRAINERS, ROW_START};
static const u8 sPcRows[]      = {ROW_STARTERS, ROW_WILD, ROW_TRAINERS, ROW_START};
#endif

#define ROW_HEIGHT     16          // FONT_NORMAL's ink is 15px; it cannot be packed tighter
#define LIST_TOP_TILE   3
#define VISIBLE_ROWS    6           // the list window is 12 tiles tall; longer lists scroll
#define YPOS_OF(row)    ((row) * ROW_HEIGHT)
#define VALUE_RIGHT     198         // right edge of the value column, as on the OPTION page
#define ARROW_X         200         // the scroll arrows, right of the value column

static void Task_FadeIn(u8 taskId);
static void Task_ProcessInput(u8 taskId);
static void Task_FadeOut(u8 taskId);
static void HighlightRow(u8 row);
static void DrawRows(u8 taskId);
static void DrawDescription(u8 taskId);
static void DrawHeaderText(void);
static void DrawBgWindowFrames(void);
static void StartMenu(MainCallback returnCallback, bool32 newGame);

static EWRAM_DATA MainCallback sSavedCallback = NULL;
static EWRAM_DATA bool8 sNewGame = FALSE;

static const u16 sText_Pal[] = INCGFX_U16("graphics/interface/option_menu_text.pal", ".gbapal");
static const u16 sBg_Pal[]   = {RGB(17, 18, 31)};

// Palette indices in sText_Pal: 1 white, 2/3 orange (labels), 4/5 red (a
// setting that is on), 6/7 dark/light grey (a setting that is off, help text).
static const u8 sColors_Label[] = {1, 2, 3};
static const u8 sColors_On[]    = {1, 4, 5};
static const u8 sColors_Off[]   = {1, 6, 7};
#define DIVIDER_COLOR 7

static const u8 *const sRowNames[ROW_KIND_COUNT] =
{
    [ROW_FAST_FORWARD] = COMPOUND_STRING("FAST FORWARD"),
    [ROW_LEVEL_CAP]    = COMPOUND_STRING("LEVEL CAP"),
    [ROW_NUZLOCKE]     = COMPOUND_STRING("NUZLOCKE"),
    [ROW_STARTERS]     = COMPOUND_STRING("RANDOM STARTERS"),
    [ROW_WILD]         = COMPOUND_STRING("RANDOM WILD PKMN"),
    [ROW_TRAINERS]     = COMPOUND_STRING("RANDOM TRAINERS"),
    [ROW_START]        = COMPOUND_STRING("START THE GAME"),
};

static const u8 *const sDescriptions[ROW_KIND_COUNT] =
{
    [ROW_FAST_FORWARD] = COMPOUND_STRING("Runs the whole game faster.\n2x is recommended. Also in OPTIONS."),
    [ROW_LEVEL_CAP]    = COMPOUND_STRING("Pokémon can't level up past the\ncurrent cap. Also in OPTIONS."),
    [ROW_NUZLOCKE]     = COMPOUND_STRING("First catch per area only. The\nfallen go to the GRAVES box."),
    [ROW_STARTERS]     = COMPOUND_STRING("The three sealed starters become\nsomething else entirely."),
    [ROW_WILD]         = COMPOUND_STRING("Every wild Pokémon is swapped.\nLevels never change."),
    [ROW_TRAINERS]     = COMPOUND_STRING("Every Trainer's team is swapped.\nShadows keep their identity."),
    [ROW_START]        = COMPOUND_STRING("Begin the adventure. Nuzlocke and\nrandomizer lock with your starter."),
};

static const u8 sText_TitleNewGame[]   = _("NEW GAME SETTINGS");
static const u8 sText_TitleRandomizer[] = _("CHALLENGE SETTINGS");
static const u8 sText_Done[]           = _("DONE");
static const u8 sText_DoneDescription[] = _("Keep these choices and close\nthe setup screen.");
static const u8 sText_On[]             = _("ON");
static const u8 sText_Off[]            = _("OFF");

static const u8 *const sFastForwardNames[FF_COUNT] =
{
    [FF_OFF] = COMPOUND_STRING("OFF"),
    [FF_2X]  = COMPOUND_STRING("2x (RECOMMENDED)"),
    [FF_3X]  = COMPOUND_STRING("3x"),
};

// One panel holds everything: frame on rows 0 and 19, title on rows 1-2, six
// settings on rows 3-14 and two lines of help on rows 15-18. Nothing runs
// past the frame or off the screen.
static const struct WindowTemplate sWinTemplates[] =
{
    [WIN_HEADER] = {
        .bg = 1, .tilemapLeft = 2, .tilemapTop = 1,
        .width = 26, .height = 2, .paletteNum = 1, .baseBlock = 2
    },
    [WIN_LIST] = {
        .bg = 0, .tilemapLeft = 2, .tilemapTop = LIST_TOP_TILE,
        .width = 26, .height = 12, .paletteNum = 1, .baseBlock = 0x36
    },
    [WIN_DESC] = {
        .bg = 1, .tilemapLeft = 2, .tilemapTop = 15,
        .width = 26, .height = 4, .paletteNum = 1, .baseBlock = 0x36 + 26 * 12
    },
    DUMMY_WIN_TEMPLATE
};

// The frame tiles share the windows' tile space, so they start after the last
// window's blocks.
#define TILE_FRAME_BASE   (0x36 + 26 * 12 + 26 * 4)
#define TILE_TOP_CORNER_L (TILE_FRAME_BASE + 0)
#define TILE_TOP_EDGE     (TILE_FRAME_BASE + 1)
#define TILE_TOP_CORNER_R (TILE_FRAME_BASE + 2)
#define TILE_LEFT_EDGE    (TILE_FRAME_BASE + 3)
#define TILE_RIGHT_EDGE   (TILE_FRAME_BASE + 5)
#define TILE_BOT_CORNER_L (TILE_FRAME_BASE + 6)
#define TILE_BOT_EDGE     (TILE_FRAME_BASE + 7)
#define TILE_BOT_CORNER_R (TILE_FRAME_BASE + 8)

static const struct BgTemplate sBgTemplates[] =
{
    { .bg = 1, .charBaseIndex = 1, .mapBaseIndex = 30, .screenSize = 0,
      .paletteMode = 0, .priority = 0, .baseTile = 0 },
    { .bg = 0, .charBaseIndex = 1, .mapBaseIndex = 31, .screenSize = 0,
      .paletteMode = 0, .priority = 1, .baseTile = 0 },
};

static const u8 *GetRowList(u8 *count)
{
    if (sNewGame)
    {
        *count = ARRAY_COUNT(sNewGameRows);
        return sNewGameRows;
    }
    *count = ARRAY_COUNT(sPcRows);
    return sPcRows;
}

static u8 RowKind(u8 row)
{
    u8 count;
    const u8 *rows = GetRowList(&count);

    return rows[min(row, count - 1)];
}

static u8 RowCount(void)
{
    u8 count;

    GetRowList(&count);
    return count;
}

static u8 FastForwardToSave(u8 value)
{
    switch (value)
    {
    case FF_2X:
        return OPTIONS_FAST_FORWARD_2X;
    case FF_3X:
        return OPTIONS_FAST_FORWARD_3X;
    default:
        return OPTIONS_FAST_FORWARD_OFF;
    }
}

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

// The value column: one word, the value in force, right-aligned. Red when the
// setting is on, grey when it is off.
static void DrawRowValue(u8 taskId, u8 row, u8 screenRow)
{
    s16 *data = gTasks[taskId].data;
    u8 kind = RowKind(row);
    const u8 *text;
    const u8 *colors;

    switch (kind)
    {
    case ROW_START:
        return;
    case ROW_FAST_FORWARD:
        text = sFastForwardNames[data[VALUE_SLOT(kind)]];
        colors = (data[VALUE_SLOT(kind)] == FF_OFF) ? sColors_Off : sColors_On;
        break;
    default:
        text = data[VALUE_SLOT(kind)] ? sText_On : sText_Off;
        colors = data[VALUE_SLOT(kind)] ? sColors_On : sColors_Off;
        break;
    }
    AddTextPrinterParameterized3(WIN_LIST, FONT_NORMAL, GetStringRightAlignXOffset(FONT_NORMAL, text, VALUE_RIGHT),
                                 YPOS_OF(screenRow) + 1, colors, TEXT_SKIP_DRAW, text);
}

static void DrawRows(u8 taskId)
{
    static const u8 sText_ArrowUp[] = _("{UP_ARROW}");
    static const u8 sText_ArrowDown[] = _("{DOWN_ARROW}");
    s16 *data = gTasks[taskId].data;
    u8 screenRow;

    // Values change length and colour, so the whole list is repainted. The new
    // game page has more rows than fit: it shows VISIBLE_ROWS from tScroll,
    // with an arrow on the side that has more.
    FillWindowPixelBuffer(WIN_LIST, PIXEL_FILL(1));
    for (screenRow = 0; screenRow < VISIBLE_ROWS && tScroll + screenRow < RowCount(); screenRow++)
    {
        u8 row = tScroll + screenRow;
        u8 kind = RowKind(row);
        const u8 *name = (kind == ROW_START && !sNewGame) ? sText_Done : sRowNames[kind];

        AddTextPrinterParameterized3(WIN_LIST, FONT_NORMAL, 8, YPOS_OF(screenRow) + 1, sColors_Label, TEXT_SKIP_DRAW, name);
        DrawRowValue(taskId, row, screenRow);
    }
    if (tScroll > 0)
        AddTextPrinterParameterized3(WIN_LIST, FONT_NORMAL, ARROW_X, YPOS_OF(0) + 1, sColors_Off, TEXT_SKIP_DRAW, sText_ArrowUp);
    if (tScroll + VISIBLE_ROWS < RowCount())
        AddTextPrinterParameterized3(WIN_LIST, FONT_NORMAL, ARROW_X, YPOS_OF(VISIBLE_ROWS - 1) + 1, sColors_Off, TEXT_SKIP_DRAW, sText_ArrowDown);
    CopyWindowToVram(WIN_LIST, COPYWIN_GFX);
}

static void DrawDescription(u8 taskId)
{
    u8 kind = RowKind(gTasks[taskId].tSelection);
    const u8 *text = (kind == ROW_START && !sNewGame) ? sText_DoneDescription : sDescriptions[kind];

    // A one-pixel rule across the top keeps the help apart from the last row
    // when that row is the highlighted (white) one.
    FillWindowPixelBuffer(WIN_DESC, PIXEL_FILL(1));
    FillWindowPixelRect(WIN_DESC, PIXEL_FILL(DIVIDER_COLOR), 0, 0, 26 * 8, 1);
    AddTextPrinterParameterized3(WIN_DESC, FONT_NORMAL, 8, 1, sColors_Off, TEXT_SKIP_DRAW, text);
    CopyWindowToVram(WIN_DESC, COPYWIN_FULL);
}

static void LoadStartingValues(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    bool32 master;

    if (sNewGame)
    {
        // A new game starts from the defaults, whatever the file that was
        // loaded at boot had: fast forward off, level cap on, no randomizer.
        data[VALUE_SLOT(ROW_FAST_FORWARD)] = FF_OFF;
        data[VALUE_SLOT(ROW_LEVEL_CAP)] = TRUE;
        data[VALUE_SLOT(ROW_NUZLOCKE)] = FALSE;
        data[VALUE_SLOT(ROW_STARTERS)] = FALSE;
        data[VALUE_SLOT(ROW_WILD)] = FALSE;
        data[VALUE_SLOT(ROW_TRAINERS)] = FALSE;
        return;
    }

    // The PC shows what is in force. Older saves can hold a row that is on
    // under a master that is off; that player chose no randomizer.
    master = FlagGet(FLAG_WOT_RANDOMIZER);
    data[VALUE_SLOT(ROW_NUZLOCKE)] = WotNuzlocke_IsOn();
    data[VALUE_SLOT(ROW_STARTERS)] = master && FlagGet(FLAG_WOT_RAND_STARTERS);
    data[VALUE_SLOT(ROW_WILD)]     = master && FlagGet(FLAG_WOT_RAND_WILD);
    data[VALUE_SLOT(ROW_TRAINERS)] = master && FlagGet(FLAG_WOT_RAND_TRAINERS);
}

static void CB2_WotSettingsMenu(void)
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
        LoadBgTiles(1, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, TILE_FRAME_BASE);
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
        PutWindowTilemap(WIN_LIST);
        gMain.state++;
        break;
    case 9:
        DrawBgWindowFrames();
        gMain.state++;
        break;
    case 10:
    {
        u8 taskId = CreateTask(Task_FadeIn, 0);

        // Open on the first row, so its help -- for a new game, the fast
        // forward recommendation -- is the first thing on screen. START jumps
        // to the last row, so taking the defaults is still two presses.
        gTasks[taskId].tSelection = 0;
        gTasks[taskId].tScroll = 0;
        LoadStartingValues(taskId);
        DrawRows(taskId);
        DrawDescription(taskId);
        HighlightRow(0);
        CopyWindowToVram(WIN_LIST, COPYWIN_FULL);
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

static void StartMenu(MainCallback returnCallback, bool32 newGame)
{
    sSavedCallback = returnCallback;
    sNewGame = newGame;
    SetMainCallback2(CB2_WotSettingsMenu);
}

// NEW GAME, after the naming screen.
void WotStartNewGameSettings(MainCallback returnCallback)
{
    StartMenu(returnCallback, TRUE);
}

// The bedroom PC (special WotOpenRandomizerMenu).
void WotStartRandomizerMenu(MainCallback returnCallback)
{
    StartMenu(returnCallback, FALSE);
}

// Frames of deafness once the menu is live. This screen opens immediately
// after name entry, where the player has been mashing A to confirm, and those
// presses otherwise land here. Half a second is enough to break the streak
// without feeling unresponsive.
#define MENU_INPUT_DELAY 30

static void Task_FadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].tInputDelay = MENU_INPUT_DELAY;
        gTasks[taskId].func = Task_ProcessInput;
    }
}

static void MoveCursor(u8 taskId, u8 row)
{
    s16 *data = gTasks[taskId].data;
    s16 oldScroll = tScroll;

    tSelection = row;
    if (row < tScroll)
        tScroll = row;
    else if (row >= tScroll + VISIBLE_ROWS)
        tScroll = row - VISIBLE_ROWS + 1;
    if (tScroll != oldScroll)
        DrawRows(taskId);
    HighlightRow(row - tScroll);
    DrawDescription(taskId);
}

static void Task_ProcessInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 count = RowCount();
    u8 kind = RowKind(tSelection);

    // Swallow whatever was still being mashed on the way in.
    if (tInputDelay > 0)
    {
        tInputDelay--;
        return;
    }

    if ((JOY_NEW(A_BUTTON) && kind == ROW_START) || JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].func = Task_FadeOut;
    }
    else if (JOY_NEW(START_BUTTON))
    {
        // START moves to the last row rather than starting outright: a second
        // A confirms. Two deliberate presses, no accidental launch.
        PlaySE(SE_SELECT);
        MoveCursor(taskId, count - 1);
    }
    else if (JOY_NEW(DPAD_UP))
    {
        PlaySE(SE_SELECT);
        MoveCursor(taskId, (tSelection > 0) ? tSelection - 1 : count - 1);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        PlaySE(SE_SELECT);
        MoveCursor(taskId, (tSelection < count - 1) ? tSelection + 1 : 0);
    }
    else if (JOY_NEW(DPAD_LEFT | DPAD_RIGHT | A_BUTTON) && kind != ROW_START)
    {
        s16 *value = &data[VALUE_SLOT(kind)];

        if (kind == ROW_FAST_FORWARD)
        {
            // OFF -> 2x -> 3x; LEFT steps back.
            if (JOY_NEW(DPAD_LEFT))
                *value = (*value > 0) ? *value - 1 : FF_COUNT - 1;
            else
                *value = (*value + 1) % FF_COUNT;
        }
        else
        {
            *value = !*value;
        }
        PlaySE(SE_SELECT);
        DrawRows(taskId);
    }
}

static void CommitSettings(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    bool32 starters = data[VALUE_SLOT(ROW_STARTERS)];
    bool32 wild = data[VALUE_SLOT(ROW_WILD)];
    bool32 trainers = data[VALUE_SLOT(ROW_TRAINERS)];
    bool32 nuzlocke = data[VALUE_SLOT(ROW_NUZLOCKE)];

    // The randomizer's master flag is on exactly while one of its rows is.
    if (starters || wild || trainers) FlagSet(FLAG_WOT_RANDOMIZER);  else FlagClear(FLAG_WOT_RANDOMIZER);
    if (starters) FlagSet(FLAG_WOT_RAND_STARTERS); else FlagClear(FLAG_WOT_RAND_STARTERS);
    if (wild)     FlagSet(FLAG_WOT_RAND_WILD);     else FlagClear(FLAG_WOT_RAND_WILD);
    if (trainers) FlagSet(FLAG_WOT_RAND_TRAINERS); else FlagClear(FLAG_WOT_RAND_TRAINERS);

    if (!sNewGame)
    {
        // The bedroom PC: the save is live, so Nuzlocke switches right here.
        WotNuzlocke_SetOn(nuzlocke);
        return;
    }

    // Fast forward is an option in save block 2, which a new game keeps.
    gSaveBlock2Ptr->optionsFastForward = FastForwardToSave(data[VALUE_SLOT(ROW_FAST_FORWARD)]);

    // The flags above are about to be wiped by NewGameInitData(), so park a
    // copy outside the save block; new_game.c re-applies it once the save
    // block is initialised.
    WotQueueNewGameSettings(starters, wild, trainers, !data[VALUE_SLOT(ROW_LEVEL_CAP)], nuzlocke);
}

static void Task_FadeOut(u8 taskId)
{
    // Commit and kick the fade once, then wait for it. (Calling
    // BeginNormalPaletteFade every frame would restart it forever.)
    if (!gTasks[taskId].tFadeStarted)
    {
        gTasks[taskId].tFadeStarted = TRUE;
        CommitSettings(taskId);
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

static void HighlightRow(u8 row)
{
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(16, DISPLAY_WIDTH - 16));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(YPOS_OF(row) + LIST_TOP_TILE * 8, YPOS_OF(row) + LIST_TOP_TILE * 8 + ROW_HEIGHT));
}

static void DrawHeaderText(void)
{
    FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(1));
    AddTextPrinterParameterized3(WIN_HEADER, FONT_NORMAL, 8, 1, sColors_Label, TEXT_SKIP_DRAW,
                                 sNewGame ? sText_TitleNewGame : sText_TitleRandomizer);
    // The same rule as the help box's, along the bottom: the first row is
    // highlighted when the page opens, and white on white would merge. Drawn
    // after the text, whose cell would otherwise paint over it.
    FillWindowPixelRect(WIN_HEADER, PIXEL_FILL(DIVIDER_COLOR), 0, 15, 26 * 8, 1);
    CopyWindowToVram(WIN_HEADER, COPYWIN_FULL);
}

static void DrawBgWindowFrames(void)
{
    //                     bg, tile,              x, y, width, height, palNum
    // One panel around the title, the list and the help, as on the OPTION page.
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_L,  1,  0,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_EDGE,      2,  0, 26,  1,  7);
    FillBgTilemapBufferRect(1, TILE_TOP_CORNER_R, 28,  0,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_LEFT_EDGE,     1,  1,  1, 18,  7);
    FillBgTilemapBufferRect(1, TILE_RIGHT_EDGE,   28,  1,  1, 18,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_L,  1, 19,  1,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_EDGE,      2, 19, 26,  1,  7);
    FillBgTilemapBufferRect(1, TILE_BOT_CORNER_R, 28, 19,  1,  1,  7);

    CopyBgTilemapBufferToVram(1);
}
