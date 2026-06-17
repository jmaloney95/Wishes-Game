// Pokemon Wishes of Tomorrow -- Flatfoot Games studio intro.
//
// A self-contained "Flatfoot Games presents" card that plays right after the
// Nintendo copyright screen, replacing the stock RHH / FRLG game intros. It is
// intentionally simple and robust (no sprites): two text BGs -- a warm dark
// gradient background and a transparent overlay holding the FLATFOOT GAMES logo +
// "presents" text -- animated entirely with palette fades.
//
// Flow: black -> fade in logo -> fade in "presents" -> hold -> fade to black ->
// hand off to the title screen. Any key press skips ahead.

#include "global.h"
#include "main.h"
#include "palette.h"
#include "gpu_regs.h"
#include "bg.h"
#include "sprite.h"
#include "decompress.h"
#include "task.h"
#include "scanline_effect.h"
#include "title_screen.h"
#include "flatfoot_intro.h"
#include "constants/rgb.h"

// The gradient BG occupies BG palette bank 0; the FLATFOOT overlay bank 1.
#define PALTAG_INTRO_BG      0
#define PALTAG_INTRO_OVERLAY 1

static const u32 sIntroBgGfx[]        = INCGFX_U32("graphics/intro_flatfoot/intro_bg.png", ".4bpp.smol", "-num_tiles 16 -Wnum_tiles");
static const u32 sIntroBgTilemap[]    = INCBIN_U32("graphics/intro_flatfoot/intro_bg.bin.smolTM");
static const u16 sIntroBgPal[]        = INCGFX_U16("graphics/intro_flatfoot/intro_bg.pal", ".gbapal");

static const u32 sFlatfootOverlayGfx[]     = INCGFX_U32("graphics/intro_flatfoot/flatfoot_overlay.png", ".4bpp.smol", "-num_tiles 144 -Wnum_tiles");
static const u32 sFlatfootOverlayTilemap[] = INCBIN_U32("graphics/intro_flatfoot/flatfoot_overlay.bin.smolTM");
static const u16 sFlatfootOverlayPal[]     = INCGFX_U16("graphics/intro_flatfoot/flatfoot_overlay.pal", ".gbapal");

static void MainCB2_FlatfootIntro(void);
static void Task_FlatfootIntro(u8 taskId);
static void GoToTitleScreen(void);

#define tState   data[0]
#define tTimer   data[1]

static void VBlankCB_FlatfootIntro(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_FlatfootIntro(void)
{
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        DmaFill16(3, 0, (void *)VRAM, VRAM_SIZE);
        DmaFill32(3, 0, (void *)OAM, OAM_SIZE);
        DmaFill16(3, 0, (void *)(PLTT + 2), PLTT_SIZE - 2);
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        FreeAllSpritePalettes();
        gMain.state = 1;
        break;
    case 1:
        // BG1 = warm gradient background (priority 1, char base 0, screen base 28).
        DecompressDataWithHeaderVram(sIntroBgGfx, (void *)(BG_CHAR_ADDR(0)));
        DecompressDataWithHeaderVram(sIntroBgTilemap, (void *)(BG_SCREEN_ADDR(28)));
        LoadPalette(sIntroBgPal, BG_PLTT_ID(PALTAG_INTRO_BG), PLTT_SIZE_4BPP);
        // BG0 = FLATFOOT overlay (priority 0, char base 1, screen base 30), transparent idx0.
        DecompressDataWithHeaderVram(sFlatfootOverlayGfx, (void *)(BG_CHAR_ADDR(1)));
        DecompressDataWithHeaderVram(sFlatfootOverlayTilemap, (void *)(BG_SCREEN_ADDR(30)));
        LoadPalette(sFlatfootOverlayPal, BG_PLTT_ID(PALTAG_INTRO_OVERLAY), PLTT_SIZE_4BPP);
        SetGpuReg(REG_OFFSET_BG0CNT, BGCNT_PRIORITY(0) | BGCNT_CHARBASE(1) | BGCNT_SCREENBASE(30) | BGCNT_16COLOR | BGCNT_TXT256x256);
        SetGpuReg(REG_OFFSET_BG1CNT, BGCNT_PRIORITY(1) | BGCNT_CHARBASE(0) | BGCNT_SCREENBASE(28) | BGCNT_16COLOR | BGCNT_TXT256x256);
        SetGpuReg(REG_OFFSET_BG0HOFS, 0);
        SetGpuReg(REG_OFFSET_BG0VOFS, 0);
        SetGpuReg(REG_OFFSET_BG1HOFS, 0);
        SetGpuReg(REG_OFFSET_BG1VOFS, 0);
        gMain.state = 2;
        break;
    case 2:
        // Start fully black; the task fades things in.
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 16, RGB_BLACK);
        EnableInterrupts(INTR_FLAG_VBLANK);
        SetVBlankCallback(VBlankCB_FlatfootIntro);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON);
        CreateTask(Task_FlatfootIntro, 0);
        SetMainCallback2(MainCB2_FlatfootIntro);
        gMain.state = 0;
        break;
    }
}

static void MainCB2_FlatfootIntro(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

// Timeline (frames at 60fps):
//   state 0: hold black briefly, then fade everything up from black.
//   state 1: logo + bg fully shown; short beat.
//   state 2: hold the full card (~2s).
//   state 3: fade to black.
//   state 4: hand off to the title screen.
// A key press from any visible state jumps straight to the fade-out.
static void Task_FlatfootIntro(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case 0:
        if (++tTimer > 8)
        {
            tTimer = 0;
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);  // fade up from black
            tState = 1;
        }
        break;
    case 1:
        if (!gPaletteFade.active)
        {
            tTimer = 0;
            tState = 2;
        }
        break;
    case 2:
        if (++tTimer > 300 || JOY_NEW(A_BUTTON | B_BUTTON | START_BUTTON | SELECT_BUTTON))
        {
            tTimer = 0;
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);  // fade to black
            tState = 3;
        }
        break;
    case 3:
        if (!gPaletteFade.active)
            tState = 4;
        break;
    case 4:
        GoToTitleScreen();
        break;
    }
}

static void GoToTitleScreen(void)
{
    SetMainCallback2(CB2_InitTitleScreen);
}

#undef t