// Pokemon Wishes of Tomorrow: BW-style main menu (continue screen).
// Ported from Shiny-Miner/C-injections-FR (Main-Menu-BW branch) to pokeemerald-expansion.
// Layout: BG2 = Poke Ball backdrop, BG1 = selection panels (palette-swap highlight),
// BG0 = one fullscreen text window. Sprites: player overworld + party mon icons.
#include "global.h"
#include "bg.h"
#include "decompress.h"
#include "dma3.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "gpu_regs.h"
#include "main.h"
#include "menu.h"
#include "option_menu.h"
#include "overworld.h"
#include "palette.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "region_map.h"
#include "save.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "main_menu.h"
#include "naming_screen.h"
#include "random.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "title_screen.h"
#include "window.h"
#include "constants/characters.h"
#include "constants/rgb.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/songs.h"

void CB2_InitMainMenuBW(void);

static const u8 sText_Continue[] = _("CONTINUE");
static const u8 sText_NewGame[] = _("NEW GAME");
static const u8 sText_Option[]  = _("OPTION");
static const u8 sText_Team[]    = _("Team:");
static const u8 sText_Badges[]  = _("Badges: ");
static const u8 sText_Time[]    = _("Time: ");
static const u8 sText_Pokedex[] = _("Pokédex: ");
static const u8 sText_SaveFileErasedBW[]    = _("The save file has been erased\ndue to corruption or damage.");
static const u8 sText_SaveFileCorruptedBW[] = _("The save file is corrupted.\nThe previous save will be loaded.");
static const u8 sText_NoFlashBW[]           = _("The 1M sub-circuit board is\nnot installed.");

enum BwMenuAction
{
    ACTION_BW_NEWGAME,
    ACTION_BW_CONTINUE,
    ACTION_BW_OPTION,
};

enum MainMenuType
{
    MAIN_MENU_NEWGAME = 0,
    MAIN_MENU_CONTINUE,
};

enum MainMenuWindow
{
    MAIN_MENU_WINDOW_TEXT = 0,
    MAIN_MENU_WINDOW_ERROR,
    MAIN_MENU_WINDOW_COUNT,
};

#define tMenuType  data[0]
#define tCursorPos data[1]
#define tFromOptions data[8]

static const u16 sMainMenuTextPal[] = {RGB(0, 0, 0), RGB(30, 30, 30), RGB(17, 17, 17), RGB(7, 20, 31), RGB(7, 13, 20)};
static const u16 sMainMenuTextFemalePal[] = {RGB(31, 9, 11), RGB(21, 8, 8)};
static const u16 sPalMainMenuBG[]    = INCBIN_U16("graphics/main_menu_bw/bg.gbapal");
static const u16 sPalMainMenuNoSel[] = INCBIN_U16("graphics/main_menu_bw/no_sel.gbapal");
static const u16 sPalMainMenuSel[]   = INCBIN_U16("graphics/main_menu_bw/sel.gbapal");
static const u32 sTilesMainMenuBG1[] = INCBIN_U32("graphics/main_menu_bw/tiles_bg1.4bpp.lz");
static const u32 sTilesMainMenuBG2[] = INCBIN_U32("graphics/main_menu_bw/tiles_bg2.4bpp.lz");
static const u32 sMapMainMenuBG2[]     = INCBIN_U32("graphics/main_menu_bw/bg.bin.lz");
static const u32 sMapMainMenuNewGame[] = INCBIN_U32("graphics/main_menu_bw/new_game.bin.lz");
static const u32 sMapMainMenuContinue[] = INCBIN_U32("graphics/main_menu_bw/continue.bin.lz");

static bool32 MainMenuGpuInit(u8 fromOptions);
static void Task_SetWin0BldRegsAndCheckSaveFile(u8 taskId);
static void PrintSaveErrorStatus(u8 taskId, const u8 *str);
static void Task_SaveErrorStatus_RunPrinterThenWaitButton(u8 taskId);
static void Task_SetWin0BldRegsNoSaveFileCheck(u8 taskId);
static void Task_WaitFadeAndPrintMainMenuText(u8 taskId);
static void Task_PrintMainMenuText(u8 taskId);
static void Task_WaitDma3AndFadeIn(u8 taskId);
static void Task_UpdateVisualSelection(u8 taskId);
static void Task_HandleMenuInput(u8 taskId);
static void Task_ExecuteMainMenuSelection(u8 taskId);
static void Task_ReturnToTitleScreen(u8 taskId);
static void MoveWindowByMenuTypeAndCursorPos(u8 menuType, u8 cursorPos);
static bool8 HandleMenuInput(u8 taskId);
static void PrintContinueStats(void);
static void CB2_BWNewGameFromNamingScreen(void);

// Wishes of Tomorrow: the naming screen returns here with the chosen name.
// An emptied-out entry falls back to a random preset, then the opening starts
// with the same black-palette treatment the old skip-Birch path used.
static void CB2_BWNewGameFromNamingScreen(void)
{
    if (gSaveBlock2Ptr->playerName[0] == EOS)
        WotSetRandomPresetPlayerName();
    gPlttBufferUnfaded[0] = RGB_BLACK;
    gPlttBufferFaded[0] = RGB_BLACK;
    BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
    SetMainCallback2(CB2_NewGame);
}
static void DestroyAllMenuSprites(void);
static void LoadPlayerOverworldSprite(u8 anim);
static void LoadPartyMonIcons(u8 anim);
static void PrintMainMenuItem(const u8 *string, u8 left, u8 top, u8 textColor);
static void PrintErrorWindowMessage(const u8 *str);

static const u8 sErrorTextColor[] = { 1, 2, 3 };

static const struct WindowTemplate sMainMenuWindowTemplates[] = {
    [MAIN_MENU_WINDOW_TEXT] = {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 20,
        .paletteNum = 15,
        .baseBlock = 0
    },
    [MAIN_MENU_WINDOW_ERROR] = {
        .bg = 0,
        .tilemapLeft = 3,
        .tilemapTop = 15,
        .width = 24,
        .height = 4,
        .paletteNum = 14,
        .baseBlock = 0x260
    },
    [MAIN_MENU_WINDOW_COUNT] = DUMMY_WIN_TEMPLATE
};

static const struct BgTemplate sMainMenuBGTemplates[] = {
    {
        .bg = 0,                // text
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    }, {
        .bg = 1,                // selection panels
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0,
    }, {
        .bg = 2,                // Poke Ball backdrop
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0,
    }
};

static void CB2_MainMenuBW(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB_MainMenuBW(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_InitMainMenuBW(void)
{
    MainMenuGpuInit(FALSE);
}

static void CB2_InitMainMenuBWFromOptions(void)
{
    MainMenuGpuInit(TRUE);
}

static bool32 MainMenuGpuInit(u8 fromOptions)
{
    u8 taskId;

    SetVBlankCallback(NULL);
    SetHBlankCallback(NULL);
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    SetGpuReg(REG_OFFSET_BG2CNT, 0);
    SetGpuReg(REG_OFFSET_BG1CNT, 0);
    SetGpuReg(REG_OFFSET_BG0CNT, 0);
    SetGpuReg(REG_OFFSET_BG2HOFS, 0);
    SetGpuReg(REG_OFFSET_BG2VOFS, 0);
    SetGpuReg(REG_OFFSET_BG1HOFS, 0);
    SetGpuReg(REG_OFFSET_BG1VOFS, -4);
    SetGpuReg(REG_OFFSET_BG0HOFS, 0);
    SetGpuReg(REG_OFFSET_BG0VOFS, 0);
    DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
    DmaClear32(3, (void *)OAM, OAM_SIZE);
    DmaClear16(3, (void *)PLTT, PLTT_SIZE);
    ScanlineEffect_Stop();
    ResetTasks();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetPaletteFade();
    ResetBgsAndClearDma3BusyFlags(FALSE);
    InitBgsFromTemplates(0, sMainMenuBGTemplates, ARRAY_COUNT(sMainMenuBGTemplates));
    InitWindows(sMainMenuWindowTemplates);
    DeactivateAllTextPrinters();
    LoadPalette(sPalMainMenuBG, 0, 32);
    LoadPalette(sPalMainMenuNoSel, 16, 96);
    LoadPalette(sPalMainMenuSel, 16, 32);
    LoadPalette(sMainMenuTextPal, 240, sizeof(sMainMenuTextPal));
    if (gSaveBlock2Ptr->playerGender != MALE)
        LoadPalette(sMainMenuTextFemalePal, 243, sizeof(sMainMenuTextFemalePal));
    SetGpuReg(REG_OFFSET_WIN0H, 0);
    SetGpuReg(REG_OFFSET_WIN0V, 0);
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG1 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG2);
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(8, 0));
    SetGpuReg(REG_OFFSET_BLDY, 0);
    LZ77UnCompVram(sTilesMainMenuBG1, (void *)BG_CHAR_ADDR(0));
    LZ77UnCompVram(sTilesMainMenuBG2, (void *)BG_CHAR_ADDR(1));
    LZ77UnCompVram(sMapMainMenuBG2, (void *)BG_SCREEN_ADDR(30));
    SetMainCallback2(CB2_MainMenuBW);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
    taskId = CreateTask(Task_SetWin0BldRegsAndCheckSaveFile, 0);
    gTasks[taskId].tCursorPos = 0;
    gTasks[taskId].tFromOptions = fromOptions;
    return FALSE;
}

static void Task_SetWin0BldRegsAndCheckSaveFile(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(0, 0));
        SetGpuReg(REG_OFFSET_BLDY, 7);
        switch (gSaveFileStatus)
        {
        case SAVE_STATUS_OK:
            gTasks[taskId].tMenuType = MAIN_MENU_CONTINUE;
            gTasks[taskId].func = Task_SetWin0BldRegsNoSaveFileCheck;
            break;
        case SAVE_STATUS_CORRUPT:
            gTasks[taskId].tMenuType = MAIN_MENU_CONTINUE;
            PrintSaveErrorStatus(taskId, sText_SaveFileCorruptedBW);
            break;
        case SAVE_STATUS_ERROR:
            gTasks[taskId].tMenuType = MAIN_MENU_NEWGAME;
            PrintSaveErrorStatus(taskId, sText_SaveFileErasedBW);
            break;
        case SAVE_STATUS_EMPTY:
        default:
            gTasks[taskId].tMenuType = MAIN_MENU_NEWGAME;
            gTasks[taskId].func = Task_SetWin0BldRegsNoSaveFileCheck;
            break;
        case SAVE_STATUS_NO_FLASH:
            gTasks[taskId].tMenuType = MAIN_MENU_NEWGAME;
            PrintSaveErrorStatus(taskId, sText_NoFlashBW);
            break;
        }
    }
}

static void PrintSaveErrorStatus(u8 taskId, const u8 *str)
{
    LoadUserWindowBorderGfx(MAIN_MENU_WINDOW_ERROR, 0x1B1, BG_PLTT_ID(14));
    PrintErrorWindowMessage(str);
    gTasks[taskId].func = Task_SaveErrorStatus_RunPrinterThenWaitButton;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_WHITE);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    SetVBlankCallback(VBlankCB_MainMenuBW);
}

static void Task_SaveErrorStatus_RunPrinterThenWaitButton(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        RunTextPrinters();
        if (!IsTextPrinterActiveOnWindow(MAIN_MENU_WINDOW_ERROR) && JOY_NEW(A_BUTTON))
        {
            FillWindowPixelBuffer(MAIN_MENU_WINDOW_ERROR, PIXEL_FILL(0));
            ClearStdWindowAndFrame(MAIN_MENU_WINDOW_ERROR, TRUE);
            ClearWindowTilemap(MAIN_MENU_WINDOW_ERROR);
            CopyBgTilemapBufferToVram(0);
            gTasks[taskId].func = Task_PrintMainMenuText;
        }
    }
}

static void Task_SetWin0BldRegsNoSaveFileCheck(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(0, 0));
        gTasks[taskId].func = Task_WaitFadeAndPrintMainMenuText;
    }
}

static void Task_WaitFadeAndPrintMainMenuText(u8 taskId)
{
    if (!gPaletteFade.active)
        Task_PrintMainMenuText(taskId);
}

static void Task_PrintMainMenuText(u8 taskId)
{
    SetGpuReg(REG_OFFSET_WIN0H, 0);
    SetGpuReg(REG_OFFSET_WIN0V, 0);
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG1 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG2);
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(16, 7));
    SetGpuReg(REG_OFFSET_BLDY, 0);
    FillWindowPixelBuffer(MAIN_MENU_WINDOW_TEXT, PIXEL_FILL(0));
    switch (gTasks[taskId].tMenuType)
    {
    case MAIN_MENU_NEWGAME:
    default:
        LZ77UnCompVram(sMapMainMenuNewGame, (void *)BG_SCREEN_ADDR(29));
        PrintMainMenuItem(sText_NewGame, 24, 16, 0);
        PrintMainMenuItem(sText_Option, 24, 40, 0);
        break;
    case MAIN_MENU_CONTINUE:
        LZ77UnCompVram(sMapMainMenuContinue, (void *)BG_SCREEN_ADDR(29));
        PrintMainMenuItem(sText_Continue, 24, 8, 0);
        PrintMainMenuItem(sText_NewGame, 24, 112, 0);
        PrintMainMenuItem(sText_Option, 24, 136, 0);
        PrintContinueStats();
        break;
    }
    PutWindowTilemap(MAIN_MENU_WINDOW_TEXT);
    CopyWindowToVram(MAIN_MENU_WINDOW_TEXT, COPYWIN_FULL);
    gTasks[taskId].func = Task_WaitDma3AndFadeIn;
}

static void Task_WaitDma3AndFadeIn(u8 taskId)
{
    if (CheckForSpaceForDma3Request(-1) != -1)
    {
        gTasks[taskId].func = Task_UpdateVisualSelection;
        if (gTasks[taskId].tFromOptions == TRUE)
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        else
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_WHITE);
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        SetVBlankCallback(VBlankCB_MainMenuBW);
    }
}

static void Task_UpdateVisualSelection(u8 taskId)
{
    MoveWindowByMenuTypeAndCursorPos(gTasks[taskId].tMenuType, gTasks[taskId].tCursorPos);
    gTasks[taskId].func = Task_HandleMenuInput;
}

static void Task_HandleMenuInput(u8 taskId)
{
    if (!gPaletteFade.active && HandleMenuInput(taskId))
        gTasks[taskId].func = Task_UpdateVisualSelection;
}

static void Task_ExecuteMainMenuSelection(u8 taskId)
{
    u8 menuAction;
    if (!gPaletteFade.active)
    {
        switch (gTasks[taskId].tMenuType)
        {
        default:
        case MAIN_MENU_NEWGAME:
            switch (gTasks[taskId].tCursorPos)
            {
            default:
            case 0: menuAction = ACTION_BW_NEWGAME; break;
            case 1: menuAction = ACTION_BW_OPTION;  break;
            }
            break;
        case MAIN_MENU_CONTINUE:
            switch (gTasks[taskId].tCursorPos)
            {
            default:
            case 0: menuAction = ACTION_BW_CONTINUE; break;
            case 1: menuAction = ACTION_BW_NEWGAME;  break;
            case 2: menuAction = ACTION_BW_OPTION;   break;
            }
            break;
        }
        switch (menuAction)
        {
        default:
        case ACTION_BW_NEWGAME:
            // Wishes of Tomorrow: skip the Birch intro but ASK THE NAME --
            // straight into the (B2W2-themed) naming screen with a random
            // preset prefilled; CB2_BWNewGameFromNamingScreen then starts the
            // opening. THIS is the menu the game actually runs -- main_menu.c
            // has the same hook for completeness, but it is dormant.
            gSaveBlock2Ptr->playerGender = MALE;
            WotSetRandomPresetPlayerName();
            DestroyTask(taskId);
            FreeAllWindowBuffers();
            DoNamingScreen(NAMING_SCREEN_PLAYER, gSaveBlock2Ptr->playerName, gSaveBlock2Ptr->playerGender, 0, 0, CB2_BWNewGameFromNamingScreen);
            break;
        case ACTION_BW_CONTINUE:
            gPlttBufferUnfaded[0] = RGB_BLACK;
            gPlttBufferFaded[0] = RGB_BLACK;
            FreeAllWindowBuffers();
            SetMainCallback2(CB2_ContinueSavedGame);
            DestroyTask(taskId);
            break;
        case ACTION_BW_OPTION:
            gMain.savedCallback = CB2_InitMainMenuBWFromOptions;
            FreeAllWindowBuffers();
            SetMainCallback2(CB2_InitOptionMenu);
            DestroyTask(taskId);
            break;
        }
    }
}

static void Task_ReturnToTitleScreen(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        FreeAllWindowBuffers();
        SetMainCallback2(CB2_InitTitleScreen);
        DestroyTask(taskId);
    }
}

static void MoveWindowByMenuTypeAndCursorPos(u8 menuType, u8 cursorPos)
{
    LoadPalette(sPalMainMenuNoSel, 16, 96);
    switch (menuType)
    {
    default:
    case MAIN_MENU_NEWGAME:
        switch (cursorPos)
        {
        case 0:
        default:
            LoadPalette(sPalMainMenuSel, 16, 32);
            break;
        case 1:
            LoadPalette(sPalMainMenuSel, 32, 32);
            break;
        }
        break;
    case MAIN_MENU_CONTINUE:
        switch (cursorPos)
        {
        case 0:
        default:
            LoadPalette(sPalMainMenuSel, 16, 32);
            DestroyAllMenuSprites();
            LoadPlayerOverworldSprite(1);
            LoadPartyMonIcons(1);
            break;
        case 1:
            LoadPalette(sPalMainMenuSel, 32, 32);
            DestroyAllMenuSprites();
            LoadPlayerOverworldSprite(0);
            LoadPartyMonIcons(0);
            break;
        case 2:
            LoadPalette(sPalMainMenuSel, 48, 32);
            break;
        }
        break;
    }
}

static bool8 HandleMenuInput(u8 taskId)
{
    u8 menuItemCount = (gTasks[taskId].tMenuType == MAIN_MENU_CONTINUE) ? 3 : 2;

    if (JOY_NEW(A_BUTTON) || JOY_NEW(START_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_ExecuteMainMenuSelection;
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(0, DISPLAY_WIDTH));
        SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(0, DISPLAY_HEIGHT));
        gTasks[taskId].func = Task_ReturnToTitleScreen;
    }
    else if (JOY_NEW(DPAD_UP) && gTasks[taskId].tCursorPos > 0)
    {
        gTasks[taskId].tCursorPos--;
        return TRUE;
    }
    else if (JOY_NEW(DPAD_DOWN) && gTasks[taskId].tCursorPos < menuItemCount - 1)
    {
        gTasks[taskId].tCursorPos++;
        return TRUE;
    }
    return FALSE;
}

static void PrintErrorWindowMessage(const u8 *str)
{
    FillWindowPixelBuffer(MAIN_MENU_WINDOW_TEXT, PIXEL_FILL(0));
    FillWindowPixelBuffer(MAIN_MENU_WINDOW_ERROR, PIXEL_FILL(1));
    DrawTextBorderOuter(MAIN_MENU_WINDOW_ERROR, 0x1B1, 14);
    AddTextPrinterParameterized3(MAIN_MENU_WINDOW_ERROR, FONT_NORMAL, 0, 2, sErrorTextColor, 2, str);
    PutWindowTilemap(MAIN_MENU_WINDOW_ERROR);
    CopyWindowToVram(MAIN_MENU_WINDOW_ERROR, COPYWIN_GFX);
}

static void PrintContinueStats(void)
{
    u8 strbuf[32];
    u8 numbuf[8];
    u8 *ptr;
    u16 i, count;

    // player name
    PrintMainMenuItem(gSaveBlock2Ptr->playerName, 72, 40, 1);
    // location
    GetMapName(gStringVar4, Overworld_GetMapHeaderByGroupAndId(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum)->regionMapSectionId, 0);
    PrintMainMenuItem(gStringVar4, 128, 8, 0);
    // badges
    count = 0;
    for (i = FLAG_BADGE01_GET; i <= FLAG_BADGE08_GET; i++)
    {
        if (FlagGet(i))
            count++;
    }
    if (count)
    {
        StringCopy(strbuf, sText_Badges);
        ConvertIntToDecimalStringN(numbuf, count, STR_CONV_MODE_LEFT_ALIGN, 1);
        StringAppend(strbuf, numbuf);
        PrintMainMenuItem(strbuf, 128, 24, 0);
    }
    // pokedex
    if (FlagGet(FLAG_SYS_POKEDEX_GET) == TRUE)
    {
        StringCopy(strbuf, sText_Pokedex);
        if (IsNationalPokedexEnabled())
            count = GetNationalPokedexCount(FLAG_GET_CAUGHT);
        else
            count = GetHoennPokedexCount(FLAG_GET_CAUGHT);
        ConvertIntToDecimalStringN(numbuf, count, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringAppend(strbuf, numbuf);
        PrintMainMenuItem(strbuf, 128, 40, 0);
    }
    // play time
    ptr = ConvertIntToDecimalStringN(numbuf, gSaveBlock2Ptr->playTimeHours, STR_CONV_MODE_LEFT_ALIGN, 3);
    *ptr++ = CHAR_COLON;
    ConvertIntToDecimalStringN(ptr, gSaveBlock2Ptr->playTimeMinutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringCopy(strbuf, sText_Time);
    StringAppend(strbuf, numbuf);
    PrintMainMenuItem(strbuf, 128, 56, 0);
    // team label + sprites
    PrintMainMenuItem(sText_Team, 24, 56, 0);
    LoadPlayerOverworldSprite(1);
    LoadPartyMonIcons(1);
}

static void DestroyAllMenuSprites(void)
{
    ResetSpriteData();
    FreeSpriteTileRanges();
    FreeAllSpritePalettes();
}

static void LoadPlayerOverworldSprite(u8 anim)
{
    u16 gfxId = GetPlayerAvatarGraphicsIdByStateIdAndGender(PLAYER_AVATAR_STATE_NORMAL, gSaveBlock2Ptr->playerGender);
    u8 spriteId = CreateObjectGraphicsSprite(gfxId, SpriteCallbackDummy, 40, 40, 0);

    gSprites[spriteId].oam.priority = 0;
    if (anim == 0)
        StartSpriteAnim(&gSprites[spriteId], 0);
    else
        StartSpriteAnim(&gSprites[spriteId], 4);
}

static void LoadPartyMonIcons(u8 anim)
{
    u8 i, spriteId;
    u32 personality;
    u16 species;

    LoadMonIconPalettes();
    for (i = 0; i < gPlayerPartyCount; i++)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES_OR_EGG, NULL);
        personality = GetMonData(&gPlayerParty[i], MON_DATA_PERSONALITY, NULL);
        if (anim == 0)
            spriteId = CreateMonIcon(species, SpriteCallbackDummy, 32 * i + 40, 88, 0, personality);
        else
            spriteId = CreateMonIcon(species, SpriteCB_MonIcon, 32 * i + 40, 88, 0, personality);
        StartSpriteAnim(&gSprites[spriteId], 0);
        gSprites[spriteId].oam.priority = 0;
    }
}

static void PrintMainMenuItem(const u8 *string, u8 left, u8 top, u8 textColor)
{
    u8 color[3];

    if (textColor == 1)
    {
        color[0] = 0;
        color[1] = 3;
        color[2] = 4;
    }
    else
    {
        color[0] = 0;
        color[1] = 1;
        color[2] = 2;
    }
    AddTextPrinterParameterized3(MAIN_MENU_WINDOW_TEXT, FONT_NORMAL, left, top + 1, color, TEXT_SKIP_DRAW, string);
    CopyWindowToVram(MAIN_MENU_WINDOW_TEXT, COPYWIN_GFX);
}
