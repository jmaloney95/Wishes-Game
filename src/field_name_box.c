#include "global.h"
#include "main.h"
#include "menu.h"
#include "bg.h"
#include "window.h"
#include "text.h"
#include "string_util.h"
#include "international_string_util.h"
#include "script_menu.h"
#include "field_message_box.h"
#include "graphics.h"
#include "script.h"
#include "field_name_box.h"
#include "event_data.h"
#include "match_call.h"
#include "malloc.h"
#include "data.h"
#include "constants/opponents.h"
#include "constants/speaker_names.h"
#include "data/speaker_names.h"

static EWRAM_INIT u8 sNameboxWindowId = WINDOW_NONE;
EWRAM_DATA const u8 *gSpeakerName = NULL;
static EWRAM_DATA u8 sSpeakerTier = SPEAKER_TIER_DEFAULT;

static const u32 sNameBoxDefaultGfx[] = INCGFX_U32("graphics/text_window/name_box.png", ".4bpp");
static const u32 sNameBoxPokenavGfx[] = INCGFX_U32("graphics/pokenav/name_box.png", ".4bpp");

static void DestroyNameboxFrame(void);
static void WindowFunc_DrawNamebox(u32, u32, u32, u32, u32, u32, u32);
static void WindowFunc_ClearNamebox(u8, u8, u8, u8, u8, u8);

void PrepareNamebox(u32 tileNum)
{
    u8 *strbuf = AllocZeroed(32 * sizeof(u8));
    if (FlagGet(OW_FLAG_SUPPRESS_NAME_BOX) || !gSpeakerName || !strbuf)
    {
        // Re-check again in case anything but !strbuf is TRUE.
        if (strbuf)
            Free(strbuf);

        DestroyNamebox();
        RedrawDialogueFrame();
        return;
    }

    StringExpandPlaceholders(strbuf, gSpeakerName);

    u32 fontId = FONT_SMALL;
    u32 winWidth = OW_NAME_BOX_DEFAULT_WIDTH;

    if (OW_NAME_BOX_USE_DYNAMIC_WIDTH)
    {
        winWidth = ConvertPixelWidthToTileWidth(GetStringWidth(fontId, strbuf, -1));
        if (winWidth > OW_NAME_BOX_DEFAULT_WIDTH)
            winWidth = OW_NAME_BOX_DEFAULT_WIDTH;
    }

    if (sNameboxWindowId != WINDOW_NONE)
    {
        DestroyNameboxFrame();
        RedrawDialogueFrame();
    }

    bool32 matchCall = IsMatchCallTaskActive();

    struct WindowTemplate template =
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 13,
        .width = winWidth,
        .height = OW_NAME_BOX_DEFAULT_HEIGHT,
        .paletteNum = matchCall ? 14 : DLG_WINDOW_PALETTE_NUM,
        .baseBlock = tileNum,
    };

    sNameboxWindowId = AddWindow(&template);
    FillNamebox();

    u8 colors[3] = {TEXT_COLOR_TRANSPARENT, OW_NAME_BOX_FOREGROUND_COLOR, OW_NAME_BOX_SHADOW_COLOR};
    int strX = GetStringCenterAlignXOffset(fontId, strbuf, (winWidth * 8));
    switch (sSpeakerTier)
    {
    case SPEAKER_TIER_VILLAIN:
        colors[1] = OW_NAME_BOX_VILLAIN_FG_COLOR;
        colors[2] = OW_NAME_BOX_VILLAIN_SHADOW_COLOR;
        break;
    case SPEAKER_TIER_QUEST:
        colors[1] = OW_NAME_BOX_QUEST_FG_COLOR;
        colors[2] = OW_NAME_BOX_QUEST_SHADOW_COLOR;
        break;
    }
    if (matchCall)
    {
        colors[1] = 1;
        colors[2] = 0;
    }

    union TextColor savedTextColors = SaveTextColors();
    AddTextPrinterParameterized3(sNameboxWindowId, fontId, strX, 0, colors, TEXT_SKIP_DRAW, strbuf);
    RestoreTextColors(savedTextColors);
    Free(strbuf);
}

u32 GetNameboxWindowId(void)
{
    return sNameboxWindowId;
}

void ResetNameboxData(void)
{
    sNameboxWindowId = WINDOW_NONE;
    gSpeakerName = NULL;
    sSpeakerTier = SPEAKER_TIER_DEFAULT;
}

static void DestroyNameboxFrame(void)
{
    ClearNamebox(sNameboxWindowId, FALSE);
    ClearWindowTilemap(sNameboxWindowId);
    RemoveWindow(sNameboxWindowId);
}

void DestroyNamebox(void)
{
    if (sNameboxWindowId == WINDOW_NONE)
        return;

    DestroyNameboxFrame();
    ResetNameboxData();
}

u32 GetNameboxWidth(void)
{
    return gWindows[sNameboxWindowId].window.width;
}

static const u32 *GetNameboxGraphics(void)
{
    if (IsMatchCallTaskActive())
        return sNameBoxPokenavGfx;
    else
        return sNameBoxDefaultGfx;
}

void FillNamebox(void)
{
    u32 winSize = GetNameboxWidth();
    const u32 *gfx = GetNameboxGraphics();

    for (u32 i = 0; i < winSize; i++)
    {
        #define TILE(x) (8 * x)
        CopyToWindowPixelBuffer(sNameboxWindowId, &gfx[TILE(1)], TILE_SIZE_4BPP, i);
        CopyToWindowPixelBuffer(sNameboxWindowId, &gfx[TILE(4)], TILE_SIZE_4BPP, i + winSize);
        #undef TILE
    }
}

void DrawNamebox(u32 windowId, u32 tileNum, bool32 copyToVram)
{
    // manual instead of using CallWindowFunction for extra tileNum param
    struct WindowTemplate *w = &gWindows[windowId].window;
    u32 size = TILE_OFFSET_4BPP(NAME_BOX_BASE_TILES_TOTAL);

    LoadBgTiles(GetWindowAttribute(sNameboxWindowId, WINDOW_BG), GetNameboxGraphics(), size, tileNum);
    WindowFunc_DrawNamebox(w->bg, w->tilemapLeft, w->tilemapTop, w->width, w->height, w->paletteNum, tileNum);
    PutWindowTilemap(windowId);
    if (copyToVram == TRUE)
        CopyWindowToVram(windowId, COPYWIN_FULL);
}

void ClearNamebox(u32 windowId, bool32 copyToVram)
{
    CallWindowFunction(windowId, WindowFunc_ClearNamebox);
    ClearWindowTilemap(windowId);
    if (copyToVram == TRUE)
        CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void WindowFunc_DrawNamebox(u32 bg, u32 L, u32 T, u32 w, u32 h, u32 p, u32 tileNum)
{
    // left-most
    FillBgTilemapBufferRect(bg, tileNum,     L - 1, T,     1, 1, p);
    FillBgTilemapBufferRect(bg, tileNum + 3, L - 1, T + 1, 1, 1, p);

    // right-most
    FillBgTilemapBufferRect(bg, tileNum + 2, L + w, T,     1, 1, p);
    FillBgTilemapBufferRect(bg, tileNum + 5, L + w, T + 1, 1, 1, p);
}

static void WindowFunc_ClearNamebox(u8 bg, u8 L, u8 T, u8 w, u8 h, u8 p)
{
    FillBgTilemapBufferRect(bg, 0, L - 1, T, w + 2, h, 0); // palette doesn't matter
}

void SetSpeaker(struct ScriptContext *ctx)
{
    u32 arg = ScriptReadWord(ctx);
    const u8 *speaker = NULL;
    u8 tier = SPEAKER_TIER_DEFAULT;

    if (arg < SP_NAME_COUNT)
    {
        speaker = gSpeakerNamesTable[arg];
        tier = gSpeakerNameTiers[arg];
    }
    else if (arg >= ROM_START && arg < ROM_END)
    {
        speaker = (const u8 *)arg;
    }

    gSpeakerName = speaker;
    sSpeakerTier = tier;
}

// Named-cast trainers that should show a name plate on their battle-intro
// speech, mapped to the SP_NAME_* the plate displays (which may differ from
// the trainer's data name, e.g. TRAINER_JOSH shows "MikManc"). Random
// trainers are absent and get no plate. Used by the approach code instead of
// a `setspeaker` in the script, which is impossible before `trainerbattle`.
static const struct { u16 trainerId; u8 speakerName; } sTrainerSpeakers[] =
{
    { TRAINER_JOSH,                    SP_NAME_MIKMANC },
    { TRAINER_TOMMY,                   SP_NAME_MINISTER },
    { TRAINER_MARC,                    SP_NAME_YIFFER },
    { TRAINER_ROXANNE_1,               SP_NAME_RED_FATALITY },
    { TRAINER_MADAM_TSUJI,             SP_NAME_MADAM_TSUJI },
    { TRAINER_ROUTE2_SWIMMER_ALLISON,  SP_NAME_ALLISON },
    { TRAINER_HARU_HAS_TYRUNT,         SP_NAME_DRACO },
    { TRAINER_HARU_HAS_AMAURA,         SP_NAME_DRACO },
    { TRAINER_HARU_HAS_ANORITH,        SP_NAME_DRACO },
    { TRAINER_DRACO_TORII_AURORUS,     SP_NAME_DRACO },
    { TRAINER_DRACO_TORII_ARMALDO,     SP_NAME_DRACO },
    { TRAINER_DRACO_TORII_TYRANTRUM,   SP_NAME_DRACO },
    { TRAINER_DRACO_LAB_AURORUS,       SP_NAME_DRACO },
    { TRAINER_DRACO_LAB_ARMALDO,       SP_NAME_DRACO },
    { TRAINER_DRACO_LAB_TYRANTRUM,     SP_NAME_DRACO },
    { TRAINER_STARSUMMIT_BOSS,         SP_NAME_MUTRID_LEADER },
};

// Set the plate for an approaching trainer from the named-cast table (or clear
// it for a random trainer). Honors OW_NAME_BOX_NPC_TRAINER: when that is TRUE,
// every trainer instead shows their raw data name.
void SetSpeakerFromTrainer(u16 trainerId)
{
    u32 i;

    if (OW_NAME_BOX_NPC_TRAINER)
    {
        gSpeakerName = GetTrainerNameFromId(trainerId);
        sSpeakerTier = SPEAKER_TIER_DEFAULT;
        return;
    }

    for (i = 0; i < ARRAY_COUNT(sTrainerSpeakers); i++)
    {
        if (sTrainerSpeakers[i].trainerId == trainerId)
        {
            gSpeakerName = gSpeakerNamesTable[sTrainerSpeakers[i].speakerName];
            sSpeakerTier = gSpeakerNameTiers[sTrainerSpeakers[i].speakerName];
            return;
        }
    }

    gSpeakerName = NULL;
    sSpeakerTier = SPEAKER_TIER_DEFAULT;
}

// useful for other context e.g. match call
void TrySpawnAndShowNamebox(const u8 *speaker, u32 tileNum)
{
    gSpeakerName = speaker;
    sSpeakerTier = SPEAKER_TIER_DEFAULT;
    if (sNameboxWindowId != WINDOW_NONE && gSpeakerName == NULL)
    {
        ClearNamebox(sNameboxWindowId, TRUE);
        DestroyNamebox();
        RedrawDialogueFrame();
        return;
    }

    PrepareNamebox(tileNum);
    DrawNamebox(sNameboxWindowId, tileNum - NAME_BOX_BASE_TILES_TOTAL, TRUE);
}

bool32 IsSpeakerBuffered(const u8 *str)
{
    if (str[0] == EXT_CTRL_CODE_BEGIN
     && str[1] == EXT_CTRL_CODE_SPEAKER
     && str[2] >= SP_NAME_NONE)
    {
        gSpeakerName = gSpeakerNamesTable[str[2]];
        sSpeakerTier = (str[2] < SP_NAME_COUNT) ? gSpeakerNameTiers[str[2]] : SPEAKER_TIER_DEFAULT;
    }

    u32 res = FALSE;
    if (gSpeakerName)
        res = TRUE;

    return res;
}
