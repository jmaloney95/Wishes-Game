// ---------------------------------------------------------------------------
// Wishes of Tomorrow: Nuzlocke mode.
//
// Switched on from the NEW GAME SETTINGS page, or the bedroom PC before the
// starter is taken (src/wot_randomizer_menu.c). State lives at the end of
// SaveBlock3, so older saves read zeros and are simply not Nuzlocke runs.
//
// Encounters. Each map section is one area; inside the Mega Dimension, where
// every map shares MAPSEC_MEGA_DIMENSION, each map is its own area. The first
// wild Pokemon met in an area is the only one that can be caught there, met in
// grass, water, fishing, a DexNav chase or a Mega Dimension visible wild.
//   * Shiny clause: a shiny can always be caught and never uses up the area.
//   * Dupes clause: a Pokemon whose family is already registered as caught in
//     the Pokedex doesn't count. It can't be caught and the area stays open.
//   * Scripted encounters (legendaries, statics), gifts, eggs, trades and
//     Shadow snags are free. Trainer battles are never affected.
//
// Fainting. Every player Pokemon that faints in a battle is laid in the last
// PC box (renamed GRAVES) when the field comes back, even if it was revived
// before the battle ended. Nothing leaves that box and nothing new goes in
// (pokemon_storage_system.c, CopyMonToPC). If the box is full, the Pokemon is
// simply gone.
//
// Blacking out ends the run: the rules switch off, the party is healed as
// usual and the nurse says so once (EventScript_WotNuzlockeEndedNotice). With
// the rules off, the GRAVES box opens like any other.
//
// Never counted: link, Frontier, Trainer Hill and tutorial battles, debug
// battles, battles the story means you to lose (B_FLAG_NO_WHITEOUT), early
// rival battles that heal you afterwards and the Giratina dream.
// ---------------------------------------------------------------------------
#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "debug.h"
#include "event_data.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "string_util.h"
#include "strings.h"
#include "wot_mega_dimension.h"
#include "wot_nuzlocke.h"
#include "constants/battle.h"
#include "constants/battle_setup.h"
#include "constants/flags.h"
#include "constants/region_map_sections.h"

// The Mega Dimension's maps share one map section; they get their own bits
// after the map sections, by map number.
#define WOT_NUZLOCKE_MD_SLOTS 32
STATIC_ASSERT(MAPSEC_COUNT + WOT_NUZLOCKE_MD_SLOTS <= WOT_NUZLOCKE_AREA_BYTES * 8, WotNuzlockeAreaBits);

enum
{
    NUZ_CATCH_OK,
    NUZ_CATCH_AREA_USED,
    NUZ_CATCH_DUPE,
};

static EWRAM_DATA u8 sCatchBlock = NUZ_CATCH_OK;
static EWRAM_DATA bool8 sCatchDecided = FALSE; // this battle's start made the call
static EWRAM_DATA bool8 sNextScriptedWildCounts = FALSE;
static EWRAM_DATA bool8 sBattlePending = FALSE;
static EWRAM_DATA u8 sFaintedCount = 0;
static EWRAM_DATA u32 sFaintedPersonality[PARTY_SIZE] = {0};

static const u8 sText_GraveBoxName[] = _("GRAVES");
static const u8 sText_BoxPrefix[] = _("BOX");
static const u8 sText_CantCatch_AreaUsed[] = _("Nuzlocke rules! You already had\nyour encounter in this area.\p");
static const u8 sText_CantCatch_Dupe[] = _("Nuzlocke rules! You already own\nthis Pokémon's family.\p");

bool32 WotNuzlocke_IsOn(void)
{
    return WOT_NUZLOCKE && (gSaveBlock3Ptr->wotNuzlocke & WOT_NUZLOCKE_ON);
}

static void SetGraveBoxName(bool32 graves)
{
    u8 *name = GetBoxNamePtr(WOT_NUZLOCKE_GRAVE_BOX);

    if (graves)
    {
        StringCopy(name, sText_GraveBoxName);
    }
    else
    {
        u8 *end = StringCopy(name, sText_BoxPrefix);
        ConvertIntToDecimalStringN(end, WOT_NUZLOCKE_GRAVE_BOX + 1, STR_CONV_MODE_LEFT_ALIGN, 2);
    }
}

// The settings pages. Switching it off before the starter is taken only ever
// renames an empty box back.
void WotNuzlocke_SetOn(bool32 on)
{
    if (on && !WotNuzlocke_IsOn())
    {
        gSaveBlock3Ptr->wotNuzlocke = WOT_NUZLOCKE_ON;
        memset(gSaveBlock3Ptr->wotNuzlockeAreas, 0, sizeof(gSaveBlock3Ptr->wotNuzlockeAreas));
        SetGraveBoxName(TRUE);
    }
    else if (!on && WotNuzlocke_IsOn())
    {
        gSaveBlock3Ptr->wotNuzlocke = 0;
        SetGraveBoxName(FALSE);
    }
}

// NewGameInitData: a fresh file is never a Nuzlocke run until the settings
// page's choice is applied after it.
void WotNuzlocke_ResetSave(void)
{
    gSaveBlock3Ptr->wotNuzlocke = 0;
    memset(gSaveBlock3Ptr->wotNuzlockeAreas, 0, sizeof(gSaveBlock3Ptr->wotNuzlockeAreas));
}

bool32 WotNuzlocke_IsGraveBox(u32 boxId)
{
    return boxId == WOT_NUZLOCKE_GRAVE_BOX && WotNuzlocke_IsOn();
}

// --- Encounters --------------------------------------------------------------

static u32 CurrentArea(void)
{
    if (WotMegaDim_IsActive())
        return MAPSEC_COUNT + (gSaveBlock1Ptr->location.mapNum % WOT_NUZLOCKE_MD_SLOTS);
    return gMapHeader.regionMapSectionId;
}

static bool32 AreaUsed(u32 area)
{
    return (gSaveBlock3Ptr->wotNuzlockeAreas[area / 8] >> (area % 8)) & 1;
}

static void MarkAreaUsed(u32 area)
{
    gSaveBlock3Ptr->wotNuzlockeAreas[area / 8] |= 1 << (area % 8);
}

static bool32 SpeciesCaught(u32 species)
{
    return GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_CAUGHT);
}

static bool32 FamilyCaughtFrom(u32 species, u32 depth)
{
    const struct Evolution *evos;
    u32 i;

    if (SpeciesCaught(species))
        return TRUE;
    if (depth >= 3)
        return FALSE;
    evos = GetSpeciesEvolutions(species);
    if (evos == NULL)
        return FALSE;
    for (i = 0; evos[i].method != EVOLUTIONS_END; i++)
    {
        if (evos[i].method == EVO_NONE || evos[i].targetSpecies == SPECIES_NONE || evos[i].targetSpecies == species)
            continue;
        if (FamilyCaughtFrom(evos[i].targetSpecies, depth + 1))
            return TRUE;
    }
    return FALSE;
}

// The dupes clause: anything in this species' evolution family already caught.
static bool32 FamilyCaught(u32 species)
{
    u32 i;

    for (i = 0; i < 3; i++)
    {
        u32 pre = GetSpeciesPreEvolution(species);
        if (pre == SPECIES_NONE || pre == species)
            break;
        species = pre;
    }
    return FamilyCaughtFrom(species, 0);
}

// DexNav and the Mega Dimension's visible wilds start through the scripted
// wild battle path, but they are ordinary encounters.
void WotNuzlocke_NextScriptedWildCounts(void)
{
    sNextScriptedWildCounts = TRUE;
}

// Every wild battle start, once gEnemyParty is filled.
void WotNuzlocke_OnWildBattleStart(bool32 scripted)
{
    bool32 counts = !scripted || sNextScriptedWildCounts;
    struct Pokemon *mon = &gEnemyParty[0];
    u32 area;

    sNextScriptedWildCounts = FALSE;
    sCatchBlock = NUZ_CATCH_OK;
    sCatchDecided = TRUE;
    if (!WotNuzlocke_IsOn() || !counts)
        return;
    if (GetMonData(mon, MON_DATA_IS_SHINY))
        return; // shiny clause
    area = CurrentArea();
    if (AreaUsed(area))
        sCatchBlock = NUZ_CATCH_AREA_USED;
    else if (FamilyCaught(GetMonData(mon, MON_DATA_SPECIES)))
        sCatchBlock = NUZ_CATCH_DUPE; // dupes clause: the area stays open
    else
        MarkAreaUsed(area); // this is the area's encounter
}

bool32 WotNuzlocke_CanCatch(void)
{
    if (!WotNuzlocke_IsOn() || (gBattleTypeFlags & BATTLE_TYPE_TRAINER))
        return TRUE;
    return sCatchBlock == NUZ_CATCH_OK;
}

const u8 *WotNuzlocke_CantCatchText(void)
{
    return (sCatchBlock == NUZ_CATCH_DUPE) ? sText_CantCatch_Dupe : sText_CantCatch_AreaUsed;
}

// --- Fainting ------------------------------------------------------------------

// CB2_InitBattle, for every battle. It runs after the battle start function, so
// a wild battle's catch call (WotNuzlocke_OnWildBattleStart) is already made;
// any other battle (a legendary, a trainer) starts with catching allowed.
void WotNuzlocke_OnBattleInit(void)
{
    if (!sCatchDecided)
        sCatchBlock = NUZ_CATCH_OK;
    sCatchDecided = FALSE;
    sBattlePending = FALSE;
    sFaintedCount = 0;
    if (!WotNuzlocke_IsOn() || gIsDebugBattle)
        return;
    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED | BATTLE_TYPE_FRONTIER
                          | BATTLE_TYPE_TRAINER_HILL | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_SECRET_BASE
                          | BATTLE_TYPE_FIRST_BATTLE | BATTLE_TYPE_CATCH_TUTORIAL | BATTLE_TYPE_SAFARI))
        return;
    if (FlagGet(B_FLAG_NO_WHITEOUT))
        return;
    if (FlagGet(FLAG_WOT_GIRATINA_DREAM_FIGHTING))
        return; // a dream can't kill
    if ((gBattleTypeFlags & BATTLE_TYPE_TRAINER)
     && GetTrainerBattleMode() == TRAINER_BATTLE_EARLY_RIVAL
     && (GetRivalBattleFlags() & RIVAL_BATTLE_HEAL_AFTER))
        return;
    sBattlePending = TRUE;
}

// Cmd_tryfaintmon, for a battler on the player's side.
void WotNuzlocke_RecordFaint(u32 battler)
{
    u32 personality, i;

    if (!sBattlePending || sFaintedCount >= PARTY_SIZE)
        return;
    // An in-game partner's Pokemon sit in party slots 3-5 during the battle.
    if ((gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER) && gBattlerPartyIndexes[battler] >= MULTI_PARTY_SIZE)
        return;
    personality = GetMonData(GetBattlerMon(battler), MON_DATA_PERSONALITY);
    for (i = 0; i < sFaintedCount; i++)
    {
        if (sFaintedPersonality[i] == personality)
            return;
    }
    sFaintedPersonality[sFaintedCount++] = personality;
}

static bool32 FaintedThisBattle(struct Pokemon *mon)
{
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY);
    u32 i;

    if (GetMonData(mon, MON_DATA_HP) == 0)
        return TRUE;
    for (i = 0; i < sFaintedCount; i++)
    {
        if (sFaintedPersonality[i] == personality)
            return TRUE;
    }
    return FALSE;
}

static void LayToRest(struct Pokemon *mon)
{
    u32 pos;

    for (pos = 0; pos < IN_BOX_COUNT; pos++)
    {
        if (GetBoxMonDataAt(WOT_NUZLOCKE_GRAVE_BOX, pos, MON_DATA_SPECIES) == SPECIES_NONE)
        {
            CopyMon(GetBoxedMonPtr(WOT_NUZLOCKE_GRAVE_BOX, pos), &mon->box, sizeof(mon->box));
            break;
        }
    }
    ZeroMonData(mon);
}

static void EndRun(void)
{
    gSaveBlock3Ptr->wotNuzlocke = WOT_NUZLOCKE_ENDED_NOTICE;
}

// CB2_ReturnToField: the battle is over and the field is coming back.
void WotNuzlocke_AfterBattle(void)
{
    bool8 fallen[PARTY_SIZE] = {0};
    u32 i, standing = 0, buried = 0;

    if (!sBattlePending)
        return;
    sBattlePending = FALSE;
    if (!WotNuzlocke_IsOn())
        return;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gPlayerParty[i];

        if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE || GetMonData(mon, MON_DATA_IS_EGG))
            continue;
        if (FaintedThisBattle(mon))
            fallen[i] = TRUE;
        else
            standing++;
    }
    sFaintedCount = 0;

    // Nobody left standing outside a blackout (a draw, a scripted end): the
    // run is over, the same as a blackout.
    if (standing == 0)
    {
        EndRun();
        return;
    }

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (fallen[i])
        {
            LayToRest(&gPlayerParty[i]);
            buried++;
        }
    }
    if (buried != 0)
        CompactPartySlots();
}

// CB2_WhiteOut, before the party is healed.
void WotNuzlocke_OnWhiteOut(void)
{
    bool32 counted = sBattlePending;

    sBattlePending = FALSE;
    sFaintedCount = 0;
    if (counted && WotNuzlocke_IsOn())
        EndRun();
}

// specialvar, from the nurse's blackout scripts: TRUE once after a run ends.
bool8 WotNuzlocke_TakeEndedNotice(void)
{
    if (!(gSaveBlock3Ptr->wotNuzlocke & WOT_NUZLOCKE_ENDED_NOTICE))
        return FALSE;
    gSaveBlock3Ptr->wotNuzlocke &= ~WOT_NUZLOCKE_ENDED_NOTICE;
    return TRUE;
}
