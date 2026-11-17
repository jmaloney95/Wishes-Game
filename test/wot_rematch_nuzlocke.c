#include "global.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "event_data.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "wot_nuzlocke.h"
#include "wot_rematch.h"
#include "test/battle.h"
#include "constants/battle_setup.h"
#include "constants/opponents.h"
#include "constants/region_map_sections.h"

// WoT trainer rematches (src/wot_rematch.c) and Nuzlocke mode
// (src/wot_nuzlocke.c). In the test runner the level cap is never active, so
// the rematch level is the player's best Pokemon.

static void GivePlayerOneMon(u32 species, u32 level)
{
    ZeroPlayerPartyMons();
    CreateMon(&gPlayerParty[0], species, level, 0, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(&gPlayerParty[0]);
    CalculatePlayerPartyCount();
}

// CreateMon leaves HP at 0 (and personality 0 with the test OT rolls shiny).
static void MakePartyMon(u32 slot, u32 species)
{
    u32 notShiny = FALSE;

    CreateMon(&gPlayerParty[slot], species, 20, 0x1000 + slot, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gPlayerParty[slot], MON_DATA_IS_SHINY, &notShiny);
    CalculateMonStats(&gPlayerParty[slot]);
}

static void MakeWildMon(u32 species, bool32 shiny)
{
    CreateMon(&gEnemyParty[0], species, 5, 0x2000, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY, &shiny);
}

TEST("(WoT) Rematch: the ace sits at the target level, the rest two under, never below the original")
{
    GivePlayerOneMon(SPECIES_MACHOP, 30);
    EXPECT_EQ(WotRematch_TargetLevel(), 30);
    EXPECT_EQ(WotRematch_ScaleLevel(6, 0, 3), 28);
    EXPECT_EQ(WotRematch_ScaleLevel(6, 1, 3), 28);
    EXPECT_EQ(WotRematch_ScaleLevel(6, 2, 3), 30);
    EXPECT_EQ(WotRematch_ScaleLevel(6, 0, 1), 30);
    EXPECT_EQ(WotRematch_ScaleLevel(40, 0, 3), 40);
}

TEST("(WoT) Rematch: Pokemon evolve to fit their new level")
{
    EXPECT_EQ(WotRematch_EvolveSpecies(SPECIES_CHARMANDER, 15), SPECIES_CHARMANDER);
    EXPECT_EQ(WotRematch_EvolveSpecies(SPECIES_CHARMANDER, 20), SPECIES_CHARMELEON);
    EXPECT_EQ(WotRematch_EvolveSpecies(SPECIES_CHARMANDER, 36), SPECIES_CHARIZARD);
    // A stone evolution waits for WOT_REMATCH_EVO_LEVEL_OTHER.
    EXPECT_EQ(WotRematch_EvolveSpecies(SPECIES_POLIWHIRL, 35), SPECIES_POLIWHIRL);
    EXPECT_EQ(WotRematch_EvolveSpecies(SPECIES_POLIWHIRL, 36), SPECIES_POLIWRATH);
    // A friendship evolution (no level of its own) waits for WOT_REMATCH_EVO_LEVEL_CONDITION.
    EXPECT_EQ(WotRematch_EvolveSpecies(SPECIES_GOLBAT, 29), SPECIES_GOLBAT);
    EXPECT_EQ(WotRematch_EvolveSpecies(SPECIES_GOLBAT, 30), SPECIES_CROBAT);
}

// The test build has its own trainer table, so this is Route 2's Hiker Clyde
// rebuilt by hand: Buizel 6, Poliwhirl 7, Binacle 6, no authored moves.
static const struct TrainerMon sClydeParty[] =
{
    { .species = SPECIES_BUIZEL, .lvl = 6, .ball = POKEBALL_COUNT },
    { .species = SPECIES_POLIWHIRL, .lvl = 7, .ball = POKEBALL_COUNT },
    { .species = SPECIES_BINACLE, .lvl = 6, .ball = POKEBALL_COUNT },
};

static const struct Trainer sClyde =
{
    .party = sClydeParty,
    .partySize = ARRAY_COUNT(sClydeParty),
    .battleType = TRAINER_BATTLE_TYPE_SINGLES,
};

TEST("(WoT) Rematch: an armed rematch raises and evolves the trainer's team; an unarmed battle doesn't")
{
    const struct Trainer *hiker = &sClyde;

    GivePlayerOneMon(SPECIES_MACHOP, 40);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_ROUTE2_HIKER;
    TRAINER_BATTLE_PARAM.opponentB = 0;

    // First battle: Buizel 6, Poliwhirl 7, Binacle 6.
    WotRematch_Disarm();
    CreateNPCTrainerPartyFromTrainer(gEnemyParty, hiker, TRUE, BATTLE_TYPE_TRAINER);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), SPECIES_BUIZEL);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_LEVEL), 6);
    EXPECT_EQ(GetMonData(&gEnemyParty[1], MON_DATA_SPECIES), SPECIES_POLIWHIRL);
    EXPECT_EQ(GetMonData(&gEnemyParty[1], MON_DATA_LEVEL), 7);

    // Rematch at 40: Floatzel 38, Poliwrath 38, Barbaracle 40.
    WotRematch_Arm();
    CreateNPCTrainerPartyFromTrainer(gEnemyParty, hiker, TRUE, BATTLE_TYPE_TRAINER);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), SPECIES_FLOATZEL);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_LEVEL), 38);
    EXPECT_EQ(GetMonData(&gEnemyParty[1], MON_DATA_SPECIES), SPECIES_POLIWRATH);
    EXPECT_EQ(GetMonData(&gEnemyParty[1], MON_DATA_LEVEL), 38);
    EXPECT_EQ(GetMonData(&gEnemyParty[2], MON_DATA_SPECIES), SPECIES_BARBARACLE);
    EXPECT_EQ(GetMonData(&gEnemyParty[2], MON_DATA_LEVEL), 40);
    // An evolved mon learns its new species' moves rather than keeping none.
    EXPECT_NE(GetMonData(&gEnemyParty[0], MON_DATA_MOVE1), MOVE_NONE);

    // The rematch belongs to that trainer only.
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_ROUTE2_CAMPER;
    EXPECT(!WotRematch_IsActiveFor(TRAINER_ROUTE2_CAMPER));
    WotRematch_Disarm();
    EXPECT(!WotRematch_IsActiveFor(TRAINER_ROUTE2_HIKER));
}

static void StartNuzlockeRun(u32 mapsec)
{
    WotNuzlocke_ResetSave();
    WotNuzlocke_SetOn(TRUE);
    FlagClear(B_FLAG_NO_WHITEOUT);
    FlagClear(FLAG_WOT_GIRATINA_DREAM_FIGHTING);
    gMapHeader.regionMapSectionId = mapsec;
}

static void MeetWild(u32 species, bool32 shiny)
{
    MakeWildMon(species, shiny);
    WotNuzlocke_OnWildBattleStart(FALSE);
    gBattleTypeFlags = 0;
    WotNuzlocke_OnBattleInit();
}

TEST("(WoT) Nuzlocke: only the first wild Pokemon in an area can be caught")
{
    StartNuzlockeRun(MAPSEC_ROUTE_2);
    MeetWild(SPECIES_SKARMORY, FALSE);
    EXPECT(WotNuzlocke_CanCatch());
    MeetWild(SPECIES_SANDSHREW, FALSE);
    EXPECT(!WotNuzlocke_CanCatch());

    // A new area has its own first encounter.
    gMapHeader.regionMapSectionId = MAPSEC_ROUTE_ONE;
    MeetWild(SPECIES_SANDSHREW, FALSE);
    EXPECT(WotNuzlocke_CanCatch());

    // Shiny clause: always catchable.
    gMapHeader.regionMapSectionId = MAPSEC_ROUTE_2;
    MeetWild(SPECIES_SANDSHREW, TRUE);
    EXPECT(WotNuzlocke_CanCatch());

    // Scripted encounters are free, even in a used area.
    MakeWildMon(SPECIES_SUICUNE, FALSE);
    WotNuzlocke_OnWildBattleStart(TRUE);
    WotNuzlocke_OnBattleInit();
    EXPECT(WotNuzlocke_CanCatch());

    // A legendary battle that never asks starts catchable, whatever came before.
    MeetWild(SPECIES_SANDSHREW, FALSE);
    EXPECT(!WotNuzlocke_CanCatch());
    WotNuzlocke_OnBattleInit();
    EXPECT(WotNuzlocke_CanCatch());

    // A DexNav chase is an ordinary encounter.
    WotNuzlocke_NextScriptedWildCounts();
    MakeWildMon(SPECIES_SANDSHREW, FALSE);
    WotNuzlocke_OnWildBattleStart(TRUE);
    WotNuzlocke_OnBattleInit();
    EXPECT(!WotNuzlocke_CanCatch());

    // Rules off: anything goes.
    WotNuzlocke_SetOn(FALSE);
    MeetWild(SPECIES_SANDSHREW, FALSE);
    EXPECT(WotNuzlocke_CanCatch());
}

TEST("(WoT) Nuzlocke: dupes clause skips an owned family and keeps the area open")
{
    StartNuzlockeRun(MAPSEC_LAKE_MUNEN);
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_MACHOKE), FLAG_SET_CAUGHT);
    MeetWild(SPECIES_MACHOP, FALSE);
    EXPECT(!WotNuzlocke_CanCatch());
    MeetWild(SPECIES_ZUBAT, FALSE);
    EXPECT(WotNuzlocke_CanCatch());
    MeetWild(SPECIES_ODDISH, FALSE);
    EXPECT(!WotNuzlocke_CanCatch());
}

TEST("(WoT) Nuzlocke: the fallen go to the GRAVES box and the rest stay")
{
    u32 pos, hp = 0;

    StartNuzlockeRun(MAPSEC_ROUTE_2);
    for (pos = 0; pos < IN_BOX_COUNT; pos++)
        ZeroBoxMonAt(WOT_NUZLOCKE_GRAVE_BOX, pos);
    ZeroPlayerPartyMons();
    MakePartyMon(0, SPECIES_MACHOP);
    MakePartyMon(1, SPECIES_ABRA);
    MakePartyMon(2, SPECIES_ZUBAT);
    CalculatePlayerPartyCount();
    SetMonData(&gPlayerParty[1], MON_DATA_HP, &hp);

    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    TRAINER_BATTLE_PARAM.mode = TRAINER_BATTLE_SINGLE;
    WotNuzlocke_OnBattleInit();
    WotNuzlocke_AfterBattle();

    EXPECT(WotNuzlocke_IsOn());
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), SPECIES_MACHOP);
    EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_SPECIES), SPECIES_ZUBAT);
    EXPECT_EQ(GetMonData(&gPlayerParty[2], MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(GetBoxMonDataAt(WOT_NUZLOCKE_GRAVE_BOX, 0, MON_DATA_SPECIES), SPECIES_ABRA);
    EXPECT(WotNuzlocke_IsGraveBox(WOT_NUZLOCKE_GRAVE_BOX));
    EXPECT(!WotNuzlocke_IsGraveBox(0));

    // Nothing new is sent into the GRAVES box, even when it's the current box.
    gPokemonStoragePtr->currentBox = WOT_NUZLOCKE_GRAVE_BOX;
    VarSet(VAR_PC_BOX_TO_SEND_MON, WOT_NUZLOCKE_GRAVE_BOX);
    EXPECT_EQ(CopyMonToPC(&gPlayerParty[0]), MON_GIVEN_TO_PC);
    EXPECT_NE(gSpecialVar_MonBoxId, WOT_NUZLOCKE_GRAVE_BOX);
    gPokemonStoragePtr->currentBox = 0;
}

TEST("(WoT) Nuzlocke: a battle the story means you to lose buries nobody")
{
    u32 hp = 0;

    StartNuzlockeRun(MAPSEC_ROUTE_2);
    ZeroPlayerPartyMons();
    MakePartyMon(0, SPECIES_MACHOP);
    MakePartyMon(1, SPECIES_ABRA);
    CalculatePlayerPartyCount();
    SetMonData(&gPlayerParty[1], MON_DATA_HP, &hp);

    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    TRAINER_BATTLE_PARAM.mode = TRAINER_BATTLE_SINGLE;
    FlagSet(B_FLAG_NO_WHITEOUT);
    WotNuzlocke_OnBattleInit();
    FlagClear(B_FLAG_NO_WHITEOUT);
    WotNuzlocke_AfterBattle();
    EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_SPECIES), SPECIES_ABRA);
}

TEST("(WoT) Nuzlocke: nobody left standing after a battle ends the run and buries nobody")
{
    u32 hp = 0;

    StartNuzlockeRun(MAPSEC_ROUTE_2);
    ZeroPlayerPartyMons();
    MakePartyMon(0, SPECIES_MACHOP);
    CalculatePlayerPartyCount();
    SetMonData(&gPlayerParty[0], MON_DATA_HP, &hp);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    TRAINER_BATTLE_PARAM.mode = TRAINER_BATTLE_SINGLE;
    WotNuzlocke_OnBattleInit();
    WotNuzlocke_AfterBattle();
    EXPECT(!WotNuzlocke_IsOn());
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), SPECIES_MACHOP);
    EXPECT(WotNuzlocke_TakeEndedNotice());
}

TEST("(WoT) Nuzlocke: a blackout ends the run and the nurse says so once")
{
    StartNuzlockeRun(MAPSEC_ROUTE_2);
    gBattleTypeFlags = 0;
    WotNuzlocke_OnBattleInit();
    WotNuzlocke_OnWhiteOut();
    EXPECT(!WotNuzlocke_IsOn());
    EXPECT(!WotNuzlocke_IsGraveBox(WOT_NUZLOCKE_GRAVE_BOX));
    EXPECT(WotNuzlocke_TakeEndedNotice());
    EXPECT(!WotNuzlocke_TakeEndedNotice());
}
