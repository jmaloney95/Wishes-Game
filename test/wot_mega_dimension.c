#include "global.h"
#include "test/battle.h"

// WoT Mega Dimension: the dimension's Mega is a wild Pokemon holding its own
// stone. It has no trainer (or Mega Ring) to name, so it gets its own line.
//
// A plain wild Pokemon can never Mega Evolve: OpponentHandleChooseMove's wild
// branch picks a random move and never sends RET_GIMMICK. The encounter sets
// the smart-wild-AI flag (B_SMART_WILD_AI_FLAG), which routes the wild side
// through the AI branch -- the one that does. These tests set it the same way.

WILD_BATTLE_TEST("(WoT) A smart wild Pokemon holding its stone can Mega Evolve")
{
    GIVEN {
        FLAG_SET(B_SMART_WILD_AI_FLAG);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SNORLAX) { Item(ITEM_SNORLAXITE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("The wild Snorlax is overflowing with Mega energy!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
        MESSAGE("The wild Snorlax has Mega Evolved into Mega Snorlax!");
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_SNORLAX_MEGA);
    }
}

WILD_BATTLE_TEST("(WoT) Without the smart-wild flag a wild Pokemon never Mega Evolves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SNORLAX) { Item(ITEM_SNORLAXITE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_SNORLAX);
    }
}

// Playtest bug (round 125): KO-ing the rogue Mega awarded EXP, then the same
// Mega came back out for a second round. A KO must end the wild battle.
WILD_BATTLE_TEST("(WoT) A KO'd smart wild Mega ends the battle")
{
    GIVEN {
        FLAG_SET(B_SMART_WILD_AI_FLAG);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SNORLAX) { Item(ITEM_SNORLAXITE); HP(1); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
        MESSAGE("The wild Snorlax fainted!");
    } THEN {
        EXPECT_EQ(gBattleOutcome, B_OUTCOME_WON);
    }
}

WILD_BATTLE_TEST("(WoT) A smart wild Mega KO'd on a later turn ends the battle")
{
    GIVEN {
        FLAG_SET(B_SMART_WILD_AI_FLAG);
        PLAYER(SPECIES_MEWTWO) { Level(100); Moves(MOVE_PSYSTRIKE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SNORLAX) { Level(30); Item(ITEM_SNORLAXITE); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_PSYSTRIKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
        MESSAGE("The wild Snorlax fainted!");
    } THEN {
        EXPECT_EQ(gBattleOutcome, B_OUTCOME_WON);
    }
}

// Mable's legends (round 128): a portal's Primal Groudon/Kyogre is a wild
// Groudon/Kyogre holding its orb. Primal Reversion fires on switch-in for the
// wild side too, with no smart-wild flag.
WILD_BATTLE_TEST("(WoT) A wild Groudon holding the Red Orb reverts on its own")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GROUDON) { Item(ITEM_RED_ORB); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_PRIMAL_REVERSION, opponent);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_GROUDON_PRIMAL);
    }
}

// Mable's Mega Rayquaza (round 129): Rayquaza Mega Evolves through Dragon
// Ascent, no stone. The portal's wild one knows it and has the smart flag.
WILD_BATTLE_TEST("(WoT) A smart wild Rayquaza with Dragon Ascent can Mega Evolve")
{
    GIVEN {
        FLAG_SET(B_SMART_WILD_AI_FLAG);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_RAYQUAZA) { Moves(MOVE_DRAGON_ASCENT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("The wild Rayquaza is overflowing with Mega energy!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_RAYQUAZA_MEGA);
    }
}
