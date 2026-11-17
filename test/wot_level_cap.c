#include "global.h"
#include "caps.h"
#include "event_data.h"
#include "test/battle.h"
#include "constants/opponents.h"

// WoT level caps (src/caps.c): the cap is the next required boss's ace,
// stepping on trainer-defeated flags, and the LATEST milestone reached wins.

static const u16 sMilestoneFlags[] =
{
    TRAINER_FLAGS_START + TRAINER_ROXANNE_1,
    TRAINER_FLAGS_START + TRAINER_TUNNEL_CAPTAIN,
    TRAINER_FLAGS_START + TRAINER_DRACO_TORII_ARMALDO,
    TRAINER_FLAGS_START + TRAINER_DRACO_TORII_AURORUS,
    TRAINER_FLAGS_START + TRAINER_DRACO_TORII_TYRANTRUM,
    TRAINER_FLAGS_START + TRAINER_MADAM_TSUJI,
    TRAINER_FLAGS_START + TRAINER_DISTORTION_LEADER,
    TRAINER_FLAGS_START + TRAINER_DRACO_LAB_ARMALDO,
    TRAINER_FLAGS_START + TRAINER_DRACO_LAB_AURORUS,
    TRAINER_FLAGS_START + TRAINER_DRACO_LAB_TYRANTRUM,
    TRAINER_FLAGS_START + TRAINER_SENNEN_CAPTAIN_KANNON,
    FLAG_WOT_HIDEOUT_REVEAL_DONE,
    TRAINER_FLAGS_START + TRAINER_BLAKE,
    TRAINER_FLAGS_START + TRAINER_SYLVIA,
    TRAINER_FLAGS_START + TRAINER_KATHLEEN,
    TRAINER_FLAGS_START + TRAINER_WALLY_VR_5,
};

static void ClearCapMilestones(void)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sMilestoneFlags); i++)
        FlagClear(sMilestoneFlags[i]);
}

TEST("(WoT) Level cap steps up with each required boss")
{
    ClearCapMilestones();
    EXPECT_EQ(WotGetMilestoneLevelCap(), 15);
    FlagSet(TRAINER_FLAGS_START + TRAINER_ROXANNE_1);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 21);
    FlagSet(TRAINER_FLAGS_START + TRAINER_TUNNEL_CAPTAIN);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 25);
    FlagSet(TRAINER_FLAGS_START + TRAINER_DRACO_TORII_AURORUS);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 32);
    FlagSet(TRAINER_FLAGS_START + TRAINER_MADAM_TSUJI);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 36);
    FlagSet(TRAINER_FLAGS_START + TRAINER_DISTORTION_LEADER);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 39);
    FlagSet(TRAINER_FLAGS_START + TRAINER_DRACO_LAB_TYRANTRUM);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 42);
    FlagSet(TRAINER_FLAGS_START + TRAINER_SENNEN_CAPTAIN_KANNON);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 45);
    FlagSet(FLAG_WOT_HIDEOUT_REVEAL_DONE);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 50);
    FlagSet(TRAINER_FLAGS_START + TRAINER_BLAKE);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 52);
    FlagSet(TRAINER_FLAGS_START + TRAINER_SYLVIA);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 55);
    FlagSet(TRAINER_FLAGS_START + TRAINER_KATHLEEN);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 58);
    FlagSet(TRAINER_FLAGS_START + TRAINER_WALLY_VR_5);
    EXPECT_EQ(WotGetMilestoneLevelCap(), MAX_LEVEL);
    ClearCapMilestones();
}

TEST("(WoT) Level cap uses the latest milestone reached, in any order")
{
    ClearCapMilestones();
    // An old save, or a sequence break: only a late flag is set.
    FlagSet(FLAG_WOT_HIDEOUT_REVEAL_DONE);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 50);
    // Passing an earlier milestone afterwards never lowers it.
    FlagSet(TRAINER_FLAGS_START + TRAINER_ROXANNE_1);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 50);
    ClearCapMilestones();
}

TEST("(WoT) Any of Draco's three versions passes his milestone")
{
    ClearCapMilestones();
    FlagSet(TRAINER_FLAGS_START + TRAINER_TUNNEL_CAPTAIN);
    FlagSet(TRAINER_FLAGS_START + TRAINER_DRACO_TORII_TYRANTRUM);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 32);
    FlagClear(TRAINER_FLAGS_START + TRAINER_DRACO_TORII_TYRANTRUM);
    FlagSet(TRAINER_FLAGS_START + TRAINER_DRACO_TORII_ARMALDO);
    EXPECT_EQ(WotGetMilestoneLevelCap(), 32);
    ClearCapMilestones();
}

// The maglev floor (src/wot_battle_rules.c): Volten's arena floats every
// Electric and Steel type, on both sides, like Levitate.
SINGLE_BATTLE_TEST("(WoT) Maglev floor: Ground moves miss Electric and Steel types")
{
    u32 species;
    PARAMETRIZE { species = SPECIES_MAGNEZONE; } // Electric/Steel
    PARAMETRIZE { species = SPECIES_LUXRAY; }    // Electric
    PARAMETRIZE { species = SPECIES_STEELIX; }   // Steel/Ground: normally takes Ground for double
    GIVEN {
        FLAG_SET(FLAG_WOT_MAGLEV_FLOOR);
        ASSUME(GetMoveType(MOVE_EARTHQUAKE) == TYPE_GROUND);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE); }
    } SCENE {
        NONE_OF { HP_BAR(opponent); }
    }
}

SINGLE_BATTLE_TEST("(WoT) Maglev floor does not lift other types")
{
    GIVEN {
        FLAG_SET(FLAG_WOT_MAGLEV_FLOOR);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GOLURK);
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("(WoT) Gravity grounds Pokemon floating on the maglev floor")
{
    GIVEN {
        FLAG_SET(FLAG_WOT_MAGLEV_FLOOR);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MAGNEZONE);
    } WHEN {
        TURN { MOVE(player, MOVE_GRAVITY); }
        TURN { MOVE(player, MOVE_EARTHQUAKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("(WoT) Without the maglev floor, Ground moves hit Electric types")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MAGNEZONE);
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE); }
    } SCENE {
        HP_BAR(opponent);
    }
}
