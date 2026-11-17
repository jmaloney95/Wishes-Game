#include "global.h"
#include "event_data.h"
#include "test/battle.h"
#include "constants/flags.h"
#include "constants/maps.h"
#include "constants/species.h"

// WoT Mega Dimension: Mable's Primal step (Primal Groudon and Kyogre, Mega
// Lugia and Ho-Oh), the rift bosses' sets and Z-Moves, and the flags that
// used to share bits with the rift masks.

u32 WotMegaDim_Test_LegendRow(u16 species);
u32 WotMegaDim_Test_LegendCount(void);
void WotMegaDim_Test_SetMableStage(u32 stage);
u32 WotMegaDim_Test_GetMableStage(void);
void WotMegaDim_Test_CatchLegend(u32 row);
bool32 WotMegaDim_Test_IsLegendWanted(u32 row);
u32 WotMegaDim_Test_LegendMap(u32 row);
s32 WotMegaDim_Test_FindBadBossSet(void);

// MABLE_STAGE_* in src/wot_mega_dimension.c
#define STAGE_TESTS         1
#define STAGE_PRIMALS       2
#define STAGE_RAYQUAZA_CALL 3
#define STAGE_RAYQUAZA      4
#define STAGE_DONE          5

static void ResetLegends(void)
{
    FlagClear(FLAG_WOT_CAUGHT_PRIMAL_GROUDON);
    FlagClear(FLAG_WOT_CAUGHT_PRIMAL_KYOGRE);
    FlagClear(FLAG_WOT_CAUGHT_MEGA_LUGIA);
    FlagClear(FLAG_WOT_CAUGHT_MEGA_HO_OH);
    FlagClear(FLAG_WOT_CAUGHT_MEGA_RAYQUAZA);
    FlagClear(FLAG_WOT_CAUGHT_ZYGARDE);
}

TEST("(WoT) Rift legends: Lugia and Ho-Oh are on the list, at Navel Rock")
{
    u32 lugia = WotMegaDim_Test_LegendRow(SPECIES_LUGIA), hoOh = WotMegaDim_Test_LegendRow(SPECIES_HO_OH);

    EXPECT_LT(lugia, WotMegaDim_Test_LegendCount());
    EXPECT_LT(hoOh, WotMegaDim_Test_LegendCount());
    EXPECT_EQ(WotMegaDim_Test_LegendMap(lugia), MAP_MEGA_DIMENSION_NAVEL_ROCK_BOTTOM);
    EXPECT_EQ(WotMegaDim_Test_LegendMap(hoOh), MAP_MEGA_DIMENSION_NAVEL_ROCK_TOP);
}

TEST("(WoT) Rift legends: the Primal step waits for all four")
{
    ResetLegends();
    WotMegaDim_Test_SetMableStage(STAGE_PRIMALS);
    WotMegaDim_Test_CatchLegend(WotMegaDim_Test_LegendRow(SPECIES_GROUDON));
    WotMegaDim_Test_CatchLegend(WotMegaDim_Test_LegendRow(SPECIES_KYOGRE));
    EXPECT_EQ(WotMegaDim_Test_GetMableStage(), STAGE_PRIMALS);
    WotMegaDim_Test_CatchLegend(WotMegaDim_Test_LegendRow(SPECIES_HO_OH));
    EXPECT_EQ(WotMegaDim_Test_GetMableStage(), STAGE_PRIMALS);
    WotMegaDim_Test_CatchLegend(WotMegaDim_Test_LegendRow(SPECIES_LUGIA));
    EXPECT_EQ(WotMegaDim_Test_GetMableStage(), STAGE_RAYQUAZA_CALL);
    EXPECT(FlagGet(FLAG_WOT_CAUGHT_MEGA_LUGIA));
    EXPECT(FlagGet(FLAG_WOT_CAUGHT_MEGA_HO_OH));
}

TEST("(WoT) Rift legends: during the Primal step any of the four can be in a portal")
{
    ResetLegends();
    WotMegaDim_Test_SetMableStage(STAGE_PRIMALS);
    EXPECT(WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_GROUDON)));
    EXPECT(WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_KYOGRE)));
    EXPECT(WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_LUGIA)));
    EXPECT(WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_HO_OH)));
    EXPECT(!WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_RAYQUAZA)));
    FlagSet(FLAG_WOT_CAUGHT_MEGA_LUGIA);
    EXPECT(!WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_LUGIA)));
    WotMegaDim_Test_SetMableStage(STAGE_TESTS);
    EXPECT(!WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_HO_OH)));
}

TEST("(WoT) Rift legends: a save already past the Primal step still finds Lugia and Ho-Oh")
{
    ResetLegends();
    FlagSet(FLAG_WOT_CAUGHT_PRIMAL_GROUDON);
    FlagSet(FLAG_WOT_CAUGHT_PRIMAL_KYOGRE);
    WotMegaDim_Test_SetMableStage(STAGE_RAYQUAZA);
    EXPECT(WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_LUGIA)));
    EXPECT(WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_RAYQUAZA)));
    EXPECT(!WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_GROUDON)));
    FlagSet(FLAG_WOT_CAUGHT_MEGA_RAYQUAZA);
    WotMegaDim_Test_SetMableStage(STAGE_DONE);
    EXPECT(WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_HO_OH)));
    // Zygarde never rides a portal.
    EXPECT(!WotMegaDim_Test_IsLegendWanted(WotMegaDim_Test_LegendRow(SPECIES_ZYGARDE_COMPLETE)));
}

TEST("(WoT) Rift bosses: every set names a damaging move of its own as the Z-Move")
{
    EXPECT_EQ(WotMegaDim_Test_FindBadBossSet(), -1);
}

// The rift masks (0x8E5-0x91D) swallowed these four in T2.
STATIC_ASSERT(FLAG_WOT_LEVEL_CAPS_OFF < FLAG_WOT_MEGA_DIM_MASKS_START || FLAG_WOT_LEVEL_CAPS_OFF > FLAG_UNUSED_0x91D, CapsOutsideMasks);
STATIC_ASSERT(FLAG_WOT_INVERSE_BATTLE < FLAG_WOT_MEGA_DIM_MASKS_START || FLAG_WOT_INVERSE_BATTLE > FLAG_UNUSED_0x91D, InverseOutsideMasks);
STATIC_ASSERT(FLAG_WOT_MAGLEV_FLOOR < FLAG_WOT_MEGA_DIM_MASKS_START || FLAG_WOT_MAGLEV_FLOOR > FLAG_UNUSED_0x91D, MaglevOutsideMasks);
STATIC_ASSERT(FLAG_WOT_ALLISON_CHOICE_SPECS < FLAG_WOT_MEGA_DIM_MASKS_START || FLAG_WOT_ALLISON_CHOICE_SPECS > FLAG_UNUSED_0x91D, AllisonOutsideMasks);

void WotMegaDim_Test_SetRiftBoss(u16 zMove);

// The battle harness runs the AI only in trainer battles, so these mark the
// test trainer's Pokemon as a rift boss with a Z-Move.
// The harness allows one gimmick per Pokemon, so these bosses hold no stone:
// their Z-Move is ready on turn one instead of turn two. Torterra resists
// Earthquake, so the AI alone would pick Stone Edge.
AI_SINGLE_BATTLE_TEST("(WoT) Rift bosses: the boss throws its Z-Move the first turn it can")
{
    WotMegaDim_Test_SetRiftBoss(MOVE_EARTHQUAKE);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_TORTERRA) { Level(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DUGTRIO) { Level(50); Moves(MOVE_EARTHQUAKE, MOVE_STONE_EDGE, MOVE_IRON_HEAD, MOVE_THROAT_CHOP); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, MOVE_EARTHQUAKE, gimmick: GIMMICK_Z_MOVE); }
    } THEN {
        WotMegaDim_Test_SetRiftBoss(MOVE_NONE);
    }
}

AI_SINGLE_BATTLE_TEST("(WoT) Rift bosses: no Z-Move into an immune foe")
{
    WotMegaDim_Test_SetRiftBoss(MOVE_EARTHQUAKE);
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT | AI_FLAG_PREFER_HIGHEST_DAMAGE_MOVE);
        PLAYER(SPECIES_GYARADOS) { Level(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DUGTRIO) { Level(50); Moves(MOVE_EARTHQUAKE, MOVE_STONE_EDGE, MOVE_IRON_HEAD, MOVE_THROAT_CHOP); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); NOT_EXPECT_MOVE(opponent, MOVE_EARTHQUAKE); }
    } THEN {
        WotMegaDim_Test_SetRiftBoss(MOVE_NONE);
    }
}
