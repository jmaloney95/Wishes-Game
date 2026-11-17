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
#include "wot_permanent_mega.h"
#include "wot_shadow_log.h"
#include "constants/species.h"

bool32 WotSpeciesIsShadow(u16 species)
{
    return species == SPECIES_JIRACHI_SHADOW || species == SPECIES_LUGIA_SHADOW;
}

bool32 WotMonIsShadow(struct Pokemon *mon)
{
    if (GetMonData(mon, MON_DATA_IS_SHADOW, NULL))
        return TRUE;

    // A story Shadow species is Shadow by its very identity, flag or no flag --
    // UNLESS it has been purified. Purification clears the flag, and the shrine
    // must stay done: the National Ribbon is the record that it happened.
    return WotSpeciesIsShadow(GetMonData(mon, MON_DATA_SPECIES, NULL))
        && !GetMonData(mon, MON_DATA_NATIONAL_RIBBON, NULL);
}

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
    // Wave 4 (2026-09-17) -- the shadow legendaries the post-Oni Shin Tokyo
    // patrols can field. Their art was already imported and unused.
    SPECIES_SUICUNE,
    SPECIES_CRESSELIA,
    SPECIES_GIRATINA,
    SPECIES_ARCEUS,
    SPECIES_LUGIA,       // the harbour, post-credits
    // Wave 5 -- the rest of the custom Shadow art, now in the
    // research-path patrol pools. Rows 64+ live in SaveBlock3.
    SPECIES_DRAGONITE,   // research taken
    SPECIES_TYPHLOSION,  // research taken
    SPECIES_CHARIZARD,   // research taken
    SPECIES_GARDEVOIR,   // research taken
    SPECIES_GALLADE,     // research taken
    SPECIES_CAMERUPT,    // research destroyed
    SPECIES_PIKACHU,     // research destroyed
    SPECIES_CHIMCHAR,    // research destroyed
    SPECIES_CHARMELEON,  // research destroyed
    SPECIES_TURTWIG,     // research destroyed
};

#define WOT_SHADOW_LOG_COUNT ARRAY_COUNT(sWotShadowLogSpecies)

// Rows 0-63: 4 snag vars + 4 purified vars, 16 bits each. Rows 64-127: the
// SaveBlock3 words (no saved var was free). Past 128 needs more
// SaveBlock3 words.
#define WOT_SHADOW_LOG_VAR_ROWS 64
STATIC_ASSERT(WOT_SHADOW_LOG_COUNT <= WOT_SHADOW_LOG_VAR_ROWS + 16 * ARRAY_COUNT(gSaveBlock3Ptr->wotShadowLogSnagExtra), WotShadowLogOutOfBits);

// Rows that don't count toward finishing the log:
// ARCEUS is the reward for finishing it (it waits at Tower Top), and
// CRESSELIA is optional: it waits in Emma's Rift Sanctum after the story.
// Both rows still fill if one is caught.
static bool32 IsUncountedSpecies(u16 species)
{
    return species == SPECIES_ARCEUS || species == SPECIES_CRESSELIA;
}

u32 WotShadowLog_SpeciesCount(void)
{
    return WOT_SHADOW_LOG_COUNT;
}

u16 WotShadowLog_SpeciesAt(u32 index)
{
    if (index >= WOT_SHADOW_LOG_COUNT)
        return SPECIES_NONE;
    return sWotShadowLogSpecies[index];
}

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

// The story Shadow species are their own species ids, but the log is keyed on
// the ORDINARY species and its saved bit indices must never move. Fold them
// back before looking up, so a Shadow Jirachi fills the Jirachi row exactly as
// a flagged plain one used to -- and old saves keep the bit they already have.
// A Mega (a Shadow Mega kept after battle, or a Mystery Code gift) is its base.
u16 WotBaseSpeciesForShadow(u16 species)
{
    switch (species)
    {
    case SPECIES_JIRACHI_SHADOW: return SPECIES_JIRACHI;
    case SPECIES_LUGIA_SHADOW:   return SPECIES_LUGIA;
    default:
        if (species < NUM_SPECIES && gSpeciesInfo[species].isMegaEvolution && WotGetMegaBaseSpecies(species) != SPECIES_NONE)
            return WotGetMegaBaseSpecies(species);
        return species;
    }
}

static s32 LogIndexOfSpecies(u16 species)
{
    u32 i;

    species = WotBaseSpeciesForShadow(species);
    for (i = 0; i < WOT_SHADOW_LOG_COUNT; i++)
    {
        if (sWotShadowLogSpecies[i] == species)
            return i;
    }
    return -1;
}

static u16 *GetLogWord(const u16 *vars, u32 index)
{
    if (index < WOT_SHADOW_LOG_VAR_ROWS)
        return GetVarPointer(vars[index / 16]);
    index -= WOT_SHADOW_LOG_VAR_ROWS;
    if (vars == sPureBitVars)
        return &gSaveBlock3Ptr->wotShadowLogPureExtra[index / 16];
    return &gSaveBlock3Ptr->wotShadowLogSnagExtra[index / 16];
}

static bool32 GetLogBit(const u16 *vars, u32 index)
{
    return (*GetLogWord(vars, index) >> (index % 16)) & 1;
}

static void SetLogBit(const u16 *vars, u32 index)
{
    *GetLogWord(vars, index) |= 1u << (index % 16);
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

    if (WotMonIsShadow(mon))
        WotShadowLog_MarkSnaggedSpecies(species);
    else if (GetMonData(mon, MON_DATA_NATIONAL_RIBBON))
        WotShadowLog_MarkPurifiedSpecies(species);
}

// The counts (and the "SNAGGED x/y" header) cover only the rows that count
// toward finishing the log.
u32 WotShadowLog_CountSnagged(void)
{
    u32 i, n = 0;

    for (i = 0; i < WOT_SHADOW_LOG_COUNT; i++)
    {
        if (!IsUncountedSpecies(sWotShadowLogSpecies[i]))
            n += GetLogBit(sSnagBitVars, i);
    }
    return n;
}

u32 WotShadowLog_CountPurified(void)
{
    u32 i, n = 0;

    for (i = 0; i < WOT_SHADOW_LOG_COUNT; i++)
    {
        if (!IsUncountedSpecies(sWotShadowLogSpecies[i]))
            n += GetLogBit(sPureBitVars, i);
    }
    return n;
}

u32 WotShadowLog_CountedRows(void)
{
    u32 i, n = 0;

    for (i = 0; i < WOT_SHADOW_LOG_COUNT; i++)
        n += !IsUncountedSpecies(sWotShadowLogSpecies[i]);
    return n;
}

bool32 WotShadowLog_SpeciesCounts(u16 species)
{
    return LogIndexOfSpecies(species) >= 0 && !IsUncountedSpecies(WotBaseSpeciesForShadow(species));
}

bool32 WotShadowLog_IsSpeciesSnagged(u16 species)
{
    s32 i = LogIndexOfSpecies(species);

    return i >= 0 && GetLogBit(sSnagBitVars, i);
}

// Finished = every counted row snagged (purifying is optional).
bool32 WotShadowLog_IsComplete(void)
{
    return WotShadowLog_CountSnagged() == WotShadowLog_CountedRows();
}

// Script bridges for the reward and the missed tower legends.
// Shadow Arceus waits at Tower Top while the log is finished and its row is
// still empty.
void WotShadowLogArceusWaits(void)
{
    gSpecialVar_Result = WotShadowLog_IsComplete() && !WotShadowLog_IsSpeciesSnagged(SPECIES_ARCEUS);
}

// VAR_0x8004 = species.
void WotShadowLogSpeciesSnagged(void)
{
    gSpecialVar_Result = WotShadowLog_IsSpeciesSnagged(gSpecialVar_0x8004);
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
static const u8 sText_StatusSealed[]   = _("SEALED");
static const u8 sText_SealedName[]     = _("?????");
static const u8 sText_WotShadowLogHeader[] = _("SHADOW LOG  SNAGGED ");
static const u8 sText_WotShadowLogPure[]   = _("  PURE ");

#define tHeaderWindowId data[0]
#define tListWindowId   data[1]
#define tListTaskId     data[2]

static void Task_WotShadowLogInput(u8 taskId);

static u32 BuildRows(void)
{
    u32 i, n = 0;

    for (i = 0; i < WOT_SHADOW_LOG_COUNT; i++)
    {
        u16 species = sWotShadowLogSpecies[i];
        bool32 snag = GetLogBit(sSnagBitVars, i);
        bool32 pure = GetLogBit(sPureBitVars, i);
        u8 *p = sRowText[n];

        // Cresselia is the log's secret: its row stays off the list until
        // one is caught, in Emma's Rift Sanctum.
        if (species == SPECIES_CRESSELIA && !snag)
            continue;
        // Full roster is always visible; status shows the hunt state:
        // LOOSE (still out there) -> SNAGGED -> PURIFIED. The reward row reads
        // ??? / SEALED until the rest of the log is done.
        if (species == SPECIES_ARCEUS && !snag && !WotShadowLog_IsComplete())
        {
            p = StringCopy(p, sText_SealedName);
            while (p - sRowText[n] < 13)
                *p++ = CHAR_SPACE;
            p = StringCopy(p, sText_StatusSealed);
        }
        else
        {
            p = StringCopy(p, GetSpeciesName(species));
            while (p - sRowText[n] < 13)
                *p++ = CHAR_SPACE;
            *p = EOS;
            if (pure)
                p = StringCopy(p, sText_StatusPurified);
            else if (snag)
                p = StringCopy(p, sText_StatusSnagged);
            else
                p = StringCopy(p, sText_StatusLoose);
        }
        *p = EOS;
        sRowItems[n].name = sRowText[n];
        sRowItems[n].id = i;
        n++;
    }
    return n;
}

void WotShowShadowLog(void)
{
    u8 taskId = CreateTask(Task_WotShadowLogInput, 80);
    struct Task *task = &gTasks[taskId];
    struct ListMenuTemplate listTemplate;
    u8 *hdr = gStringVar4;
    u32 rows = BuildRows();

    task->tHeaderWindowId = AddWindow(&sHeaderWindowTemplate);
    DrawStdWindowFrame(task->tHeaderWindowId, FALSE);
    // "SHADOW LOG   SNAGGED 12/45  PURE 3"
    hdr = StringCopy(hdr, sText_WotShadowLogHeader);
    hdr = ConvertIntToDecimalStringN(hdr, WotShadowLog_CountSnagged(), STR_CONV_MODE_LEFT_ALIGN, 2);
    *hdr++ = CHAR_SLASH;
    hdr = ConvertIntToDecimalStringN(hdr, WotShadowLog_CountedRows(), STR_CONV_MODE_LEFT_ALIGN, 2);
    hdr = StringCopy(hdr, sText_WotShadowLogPure);
    hdr = ConvertIntToDecimalStringN(hdr, WotShadowLog_CountPurified(), STR_CONV_MODE_LEFT_ALIGN, 2);
    *hdr = EOS;
    AddTextPrinterParameterized(task->tHeaderWindowId, FONT_SMALL, gStringVar4, 2, 3, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(task->tHeaderWindowId, COPYWIN_FULL);

    task->tListWindowId = AddWindow(&sListWindowTemplate);
    DrawStdWindowFrame(task->tListWindowId, FALSE);

    listTemplate = gMultiuseListMenuTemplate;
    listTemplate.items = sRowItems;
    listTemplate.totalItems = rows;
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
