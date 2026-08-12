#include "global.h"
#include "event_data.h"
#include "international_string_util.h"
#include "list_menu.h"
#include "menu.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text_window.h"
#include "window.h"
#include "constants/songs.h"
#include "wot_shadow_log.h"

// WoT SHADOW LOG -- the post-game collection ledger.
//
// The regular Pokedex was never part of this hack's loop, so the log tracks
// its own thing: every SHADOW Pokemon obtainable in the game (wilds on the
// generator's plume, patrol/trainer snag targets, and the two scripted
// legendaries), whether it has been SNAGGED and whether it has since been
// PURIFIED at the shrine.
//
// Storage is var-bitfields (4 vars snagged + 4 vars purified = 64 species of
// headroom), so it is save-compatible. Old saves backfill on Continue: any
// mon in the party or PC that is still a Shadow proves a snag; any mon
// wearing the NATIONAL RIBBON (only the shrine grants it) proves a snag AND
// a purification. Traded-away mons keep their bits -- the log records what
// you did, not what you hold.

// One species per bit, in rough encounter order. Adding species is safe
// anywhere in the list ONLY for fresh saves; append to the END to keep old
// saves' bits aligned.
static const u16 sWotShadowLogSpecies[] =
{
    // Celebi Island wilds
    SPECIES_WIGGLYTUFF,
    SPECIES_CLEFABLE,
    SPECIES_TEDDIURSA,
    SPECIES_ABSOL,
    SPECIES_UMBREON,
    // Route 129 water + rods
    SPECIES_SQUIRTLE,
    SPECIES_LANTURN,
    SPECIES_MUDKIP,
    SPECIES_PIPLUP,
    SPECIES_POLITOED,
    SPECIES_KINGDRA,
    SPECIES_DRAGONAIR,
    SPECIES_EMPOLEON,
    // Red-alert patrol pool (base)
    SPECIES_WEAVILE,
    SPECIES_HONCHKROW,
    SPECIES_TOXICROAK,
    SPECIES_DRAPION,
    SPECIES_CORVISQUIRE,
    SPECIES_CENTISKORCH,
    SPECIES_TINKATON,
    SPECIES_ARCTOZOLT,
    SPECIES_ARCHEOPS,
    // Red-alert patrol pool (post-heist)
    SPECIES_RILLABOOM,
    SPECIES_HAXORUS,
    SPECIES_MELMETAL,
    SPECIES_URSALUNA,
    SPECIES_RHYPERIOR,
    SPECIES_ANNIHILAPE,
    SPECIES_ALAKAZAM,
    SPECIES_RAPIDASH,
    // Story trainers' snag targets
    SPECIES_KROOKODILE,
    SPECIES_ONIX,
    SPECIES_RAIKOU,
    SPECIES_BISHARP,
    SPECIES_MOLTRES,
    SPECIES_HO_OH,
    SPECIES_SKUNTANK,
    SPECIES_LIEPARD,
    SPECIES_SALAMENCE,
    SPECIES_CHANDELURE,
    SPECIES_GARCHOMP,
    SPECIES_METAGROSS,
    SPECIES_MIGHTYENA,
    // The two scripted legendaries
    SPECIES_DEOXYS,
    SPECIES_JIRACHI,
    // Wave 3 additions (2026-08-11) -- APPEND-ONLY BELOW THIS LINE
    SPECIES_MAROWAK,     // jail squad (JUZO)
    SPECIES_LUXRAY,      // jail squad (SAYA)
    SPECIES_VAPOREON,    // Routes 129/130 surfing, medium-rare
    SPECIES_ARCANINE,    // patrol pools
    SPECIES_MANECTRIC,   // patrol pools
    SPECIES_FLYGON,      // patrol pools
    SPECIES_VENUSAUR,    // patrol pools
    SPECIES_BLASTOISE,   // Celebi Island super rod
    SPECIES_AZURILL,     // Celebi Island rods, common
};

#define WOT_SHADOW_LOG_COUNT ARRAY_COUNT(sWotShadowLogSpecies)

static const u16 sSnagBitVars[] =
{
    VAR_WOT_SHADOWLOG_SNAG_0, VAR_WOT_SHADOWLOG_SNAG_1,
    VAR_WOT_SHADOWLOG_SNAG_2, VAR_WOT_SHADOWLOG_SNAG_3,
};
static const u16 sPureBitVars[] =
{
    VAR_WOT_SHADOWLOG_PURE_0, VAR_WOT_SHADOWLOG_PURE_1,
    VAR_WOT_SHADOWLOG_PURE_2, VAR_WOT_SHADOWLOG_PURE_3,
};

static s32 LogIndexOfSpecies(u16 species)
{
    u32 i;

    for (i = 0; i < WOT_SHADOW_LOG_COUNT; i++)
    {
        if (sWotShadowLogSpecies[i] == species)
            return i;
    }
    return -1;
}

static bool32 GetLogBit(const u16 *vars, u32 index)
{
    return (VarGet(vars[index / 16]) >> (index % 16)) & 1;
}

static void SetLogBit(const u16 *vars, u32 index)
{
    VarSet(vars[index / 16], VarGet(vars[index / 16]) | (1u << (index % 16)));
}

void WotShadowLog_MarkSnaggedSpecies(u16 species)
{
    s32 i = LogIndexOfSpecies(species);

    if (i >= 0)
        SetLogBit(sSnagBitVars, i);
}

void WotShadowLog_MarkPurifiedSpecies(u16 species)
{
    s32 i = LogIndexOfSpecies(species);

    if (i >= 0)
    {
        SetLogBit(sSnagBitVars, i); // a purified mon was necessarily snagged
        SetLogBit(sPureBitVars, i);
    }
}

// Call with any mon that just entered the player's possession; no-ops unless
// it is (or was) a Shadow.
void WotShadowLog_MarkMon(struct Pokemon *mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);

    if (GetMonData(mon, MON_DATA_IS_SHADOW))
        WotShadowLog_MarkSnaggedSpecies(species);
    else if (GetMonData(mon, MON_DATA_NATIONAL_RIBBON))
        WotShadowLog_MarkPurifiedSpecies(species);
}

u32 WotShadowLog_CountSnagged(void)
{
    u32 i, n = 0;

    for (i = 0; i < WOT_SHADOW_LOG_COUNT; i++)
        n += GetLogBit(sSnagBitVars, i);
    return n;
}

u32 WotShadowLog_CountPurified(void)
{
    u32 i, n = 0;

    for (i = 0; i < WOT_SHADOW_LOG_COUNT; i++)
        n += GetLogBit(sPureBitVars, i);
    return n;
}

// Continue-game migration: derive log bits from every mon the player holds.
// Idempotent and cheap; runs on every Continue so nothing is ever missed.
void WotShadowLog_Backfill(void)
{
    u32 i, box, pos;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE)
            WotShadowLog_MarkMon(&gPlayerParty[i]);
    }
    for (box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        for (pos = 0; pos < IN_BOX_COUNT; pos++)
        {
            u16 species = GetBoxMonDataAt(box, pos, MON_DATA_SPECIES);

            if (species == SPECIES_NONE)
                continue;
            if (GetBoxMonDataAt(box, pos, MON_DATA_IS_SHADOW))
                WotShadowLog_MarkSnaggedSpecies(species);
            else if (GetBoxMonDataAt(box, pos, MON_DATA_NATIONAL_RIBBON))
                WotShadowLog_MarkPurifiedSpecies(species);
        }
    }
}

// Script bridges (specialvar): the Defector's analysis dialogue branches on
// how far the hunt has come.
void WotShadowLogSnaggedCount(void)
{
    gSpecialVar_Result = WotShadowLog_CountSnagged();
}

void WotShadowLogPurifiedCount(void)
{
    gSpecialVar_Result = WotShadowLog_CountPurified();
}

// ---------------------------------------------------------------------------
//  The log screen: a scrollable ledger over the live field (debug-menu
//  pattern -- header window + ListMenu window on BG0, script waits on us).
// ---------------------------------------------------------------------------

static const struct WindowTemplate sHeaderWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 4,
    .tilemapTop = 1,
    .width = 22,
    .height = 2,
    .paletteNum = 15,
    .baseBlock = 1,
};

static const struct WindowTemplate sListWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 4,
    .tilemapTop = 4,
    .width = 22,
    .height = 14,
    .paletteNum = 15,
    .baseBlock = 1 + 22 * 2,
};

static EWRAM_DATA u8 sRowText[WOT_SHADOW_LOG_COUNT][28] = {0};
static EWRAM_DATA struct ListMenuItem sRowItems[WOT_SHADOW_LOG_COUNT] = {0};

static const u8 sText_StatusSnagged[]  = _("SNAGGED");
static const u8 sText_StatusPurified[] = _("PURIFIED");
static const u8 sText_StatusLoose[]    = _("LOOSE");
static const u8 sText_WotShadowLogHeader[] = _("SHADOW LOG  SNAGGED ");
static const u8 sText_WotShadowLogPure[]   = _("  PURE ");

#define tHeaderWindowId data[0]
#define tListWindowId   data[1]
#define tListTaskId     data[2]

static void Task_WotShadowLogInput(u8 taskId);

static void BuildRows(void)
{
    u32 i;

    for (i = 0; i < WOT_SHADOW_LOG_COUNT; i++)
    {
        bool32 snag = GetLogBit(sSnagBitVars, i);
        bool32 pure = GetLogBit(sPureBitVars, i);
        u8 *p = sRowText[i];

        // Full roster is always visible (Joe); status shows the hunt state:
        // LOOSE (still out there) -> SNAGGED -> PURIFIED.
        p = StringCopy(p, GetSpeciesName(sWotShadowLogSpecies[i]));
        while (p - sRowText[i] < 13)
            *p++ = CHAR_SPACE;
        *p = EOS;
        if (pure)
            p = StringCopy(p, sText_StatusPurified);
        else if (snag)
            p = StringCopy(p, sText_StatusSnagged);
        else
            p = StringCopy(p, sText_StatusLoose);
        *p = EOS;
        sRowItems[i].name = sRowText[i];
        sRowItems[i].id = i;
    }
}

void WotShowShadowLog(void)
{
    u8 taskId = CreateTask(Task_WotShadowLogInput, 80);
    struct Task *task = &gTasks[taskId];
    struct ListMenuTemplate listTemplate;
    u8 *hdr = gStringVar4;

    BuildRows();

    task->tHeaderWindowId = AddWindow(&sHeaderWindowTemplate);
    DrawStdWindowFrame(task->tHeaderWindowId, FALSE);
    // "SHADOW LOG   SNAGGED 12/45  PURE 3"
    hdr = StringCopy(hdr, sText_WotShadowLogHeader);
    hdr = ConvertIntToDecimalStringN(hdr, WotShadowLog_CountSnagged(), STR_CONV_MODE_LEFT_ALIGN, 2);
    *hdr++ = CHAR_SLASH;
    hdr = ConvertIntToDecimalStringN(hdr, WOT_SHADOW_LOG_COUNT, STR_CONV_MODE_LEFT_ALIGN, 2);
    hdr = StringCopy(hdr, sText_WotShadowLogPure);
    hdr = ConvertIntToDecimalStringN(hdr, WotShadowLog_CountPurified(), STR_CONV_MODE_LEFT_ALIGN, 2);
    *hdr = EOS;
    AddTextPrinterParameterized(task->tHeaderWindowId, FONT_SMALL, gStringVar4, 2, 3, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(task->tHeaderWindowId, COPYWIN_FULL);

    task->tListWindowId = AddWindow(&sListWindowTemplate);
    DrawStdWindowFrame(task->tListWindowId, FALSE);

    listTemplate = gMultiuseListMenuTemplate;
    listTemplate.items = sRowItems;
    listTemplate.totalItems = WOT_SHADOW_LOG_COUNT;
    listTemplate.maxShowed = 7;
    listTemplate.windowId = task->tListWindowId;
    listTemplate.header_X = 0;
    listTemplate.item_X = 8;
    listTemplate.cursor_X = 0;
    listTemplate.upText_Y = 1;
    listTemplate.cursorPal = 2;
    listTemplate.fillValue = 1;
    listTemplate.cursorShadowPal = 3;
    listTemplate.lettersSpacing = 0;
    listTemplate.itemVerticalPadding = 0;
    listTemplate.scrollMultiple = LIST_MULTIPLE_SCROLL_DPAD;
    listTemplate.fontId = FONT_NARROW;
    listTemplate.cursorKind = CURSOR_BLACK_ARROW;
    listTemplate.moveCursorFunc = NULL;
    listTemplate.itemPrintFunc = NULL;
    task->tListTaskId = ListMenuInit(&listTemplate, 0, 0);
    CopyWindowToVram(task->tListWindowId, COPYWIN_FULL);
}

static void Task_WotShadowLogInput(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    s32 input = ListMenu_ProcessInput(task->tListTaskId);

    if (input == LIST_NOTHING_CHOSEN)
        return;
    // A on a row and B both close -- the log is read-only.
    PlaySE(SE_SELECT);
    DestroyListMenuTask(task->tListTaskId, NULL, NULL);
    ClearStdWindowAndFrame(task->tListWindowId, TRUE);
    RemoveWindow(task->tListWindowId);
    ClearStdWindowAndFrame(task->tHeaderWindowId, TRUE);
    RemoveWindow(task->tHeaderWindowId);
    ScriptContext_Enable();
    DestroyTask(taskId);
}

#undef tHeaderWindowId
#undef tListWindowId
#undef tListTaskId
