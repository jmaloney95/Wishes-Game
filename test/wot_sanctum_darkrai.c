#include "global.h"
#include "test/battle.h"

// Tulip's sanctum Darkrai fights as a Mega. Gen 7+ Dark Void only
// works for Darkrai, by species id, and Mega Darkrai is a different id.
SINGLE_BATTLE_TEST("(WoT) Mega Darkrai can still use Dark Void")
{
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Ability(ABILITY_NO_GUARD); }
        OPPONENT(SPECIES_DARKRAI) { Item(ITEM_DARKRANITE); Moves(MOVE_DARK_VOID, MOVE_DARK_PULSE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DARK_VOID, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_VOID, opponent);
        STATUS_ICON(player, sleep: TRUE);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_DARKRAI_MEGA);
    }
}

WILD_BATTLE_TEST("(WoT) A Mega Darkrai caught mid-battle comes back as Darkrai, stone in hand")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DARKRAI) { Item(ITEM_DARKRANITE); Moves(MOVE_DARK_PULSE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DARK_PULSE, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_CELEBRATE); }
        TURN { USE_ITEM(player, ITEM_MASTER_BALL); MOVE(opponent, MOVE_DARK_PULSE); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_SPECIES), SPECIES_DARKRAI);
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_HELD_ITEM), ITEM_DARKRANITE);
    }
}
