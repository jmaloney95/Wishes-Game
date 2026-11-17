#include "global.h"
#include "battle_gimmick.h"
#include "malloc.h"
#include "item.h"
#include "wot_mega_dimension.h"
#include "test/battle.h"

// WoT: the Mega Sol check asked a semi-invulnerable move (Phantom Force, Fly,
// Dig...) for its weather, which only two-turn moves like Solar Beam have, and
// the assert stopped the battle.

SINGLE_BATTLE_TEST("(WoT) Phantom Force charges and strikes after a Z-Move")
{
    GIVEN {
        PLAYER(SPECIES_DRAGONITE) { Level(30); Item(ITEM_DRAGONIUM_Z); Moves(MOVE_DRAGON_RUSH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GOLURK) { Moves(MOVE_PHANTOM_FORCE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RUSH, gimmick: GIMMICK_Z_MOVE); MOVE(opponent, MOVE_PHANTOM_FORCE); }
        TURN { MOVE(player, MOVE_CELEBRATE); SKIP_TURN(opponent); }
    } SCENE {
        MESSAGE("The opposing Golurk vanished instantly!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PHANTOM_FORCE, opponent);
    }
}

SINGLE_BATTLE_TEST("(WoT) Fly and Dig still take two turns")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_FLY; }
    PARAMETRIZE { move = MOVE_DIG; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GARCHOMP) { Moves(move); }
    } WHEN {
        TURN { MOVE(opponent, move); }
        TURN { SKIP_TURN(opponent); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
    }
}

TEST("(WoT) The Rogue Core: the player's Mega gets a crystal-free Z-Move outside the rifts")
{
    struct BattleStruct *saved = gBattleStruct;

    gBattleStruct = AllocZeroed(sizeof(*gBattleStruct));
    ClearBag();
    gBattlerPositions[0] = B_POSITION_PLAYER_LEFT;
    gBattlerPositions[1] = B_POSITION_OPPONENT_LEFT;
    SetActiveGimmick(0, GIMMICK_MEGA);
    SetActiveGimmick(1, GIMMICK_MEGA);
    EXPECT(!WotMegaDim_IsActive());
    EXPECT(!WotMegaDim_IsRogueCoreZUser(0));
    AddBagItem(ITEM_ROGUE_CORE, 1);
    EXPECT(WotMegaDim_IsRogueCoreZUser(0));
    EXPECT(!WotMegaDim_IsRogueCoreZUser(1));   // not the opponent's
    SetActiveGimmick(0, GIMMICK_NONE);
    EXPECT(!WotMegaDim_IsRogueCoreZUser(0));   // only a Mega
    ClearBag();
    Free(gBattleStruct);
    gBattleStruct = saved;
}
