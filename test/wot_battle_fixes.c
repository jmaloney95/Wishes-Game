#include "global.h"
#include "battle_gimmick.h"
#include "malloc.h"
#include "item.h"
#include "item_use.h"
#include "wot_mega_dimension.h"
#include "pokemon_animation.h"
#include "test/battle.h"
#include "battle_controllers.h"

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

// WoT: switches run after ball throws, so a trainer AI that chose to switch
// its Shadow mon out on the turn the player snagged it used to print
// "withdrew" for a mon that had already left the field, then run the
// switch-in on the empty slot (player report: Shadow Moltres, v1.3.0). A
// switch for a battler that is no longer there is now dropped, like a move.
SINGLE_BATTLE_TEST("(WoT) A trainer cannot switch out a Shadow mon snagged that turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MOLTRES) { Level(51); Shadow(TRUE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { USE_ITEM(player, ITEM_MASTER_BALL); SWITCH(opponent, 1); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF { MESSAGE("2 withdrew Moltres!"); }
        MESSAGE("2 sent out Wobbuffet!");
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WOBBUFFET);
    }
}

// WoT: an AI partner on the player's side leaves Shadow Pokémon for the
// player to snag (player report: the jail escort partner one-shot the Shadows
// before they could be snagged). A move that would knock a Shadow foe out is a
// bad move for the partner; against an ordinary foe it still goes for the KO.
AI_TWO_VS_ONE_BATTLE_TEST("(WoT) An AI partner won't knock out a Shadow foe")
{
    bool32 shadow;
    enum Move expected;

    PARAMETRIZE { shadow = FALSE; expected = MOVE_CROSS_CHOP; }
    PARAMETRIZE { shadow = TRUE;  expected = MOVE_SCARY_FACE; }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        MULTI_PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        MULTI_PARTNER(SPECIES_MACHAMP) { Level(41); Moves(MOVE_CROSS_CHOP, MOVE_SCARY_FACE); }
        MULTI_OPPONENT_A(SPECIES_TEDDIURSA) { Level(20); Shadow(shadow); Moves(MOVE_CELEBRATE); }
        MULTI_OPPONENT_A(SPECIES_TEDDIURSA) { Level(20); Shadow(shadow); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); EXPECT_MOVE(playerRight, expected); }
    }
}

AI_TWO_VS_ONE_BATTLE_TEST("(WoT) An AI partner won't use a spread move that knocks out the other Shadow foe")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        MULTI_PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        MULTI_PARTNER(SPECIES_MACHAMP) { Level(41); Moves(MOVE_ROCK_SLIDE, MOVE_BULK_UP); }
        MULTI_OPPONENT_A(SPECIES_SNORLAX) { Level(41); Moves(MOVE_CELEBRATE); }
        MULTI_OPPONENT_A(SPECIES_TEDDIURSA) { Level(20); Shadow(TRUE); HP(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); EXPECT_MOVE(playerRight, MOVE_BULK_UP); }
    }
}

// WoT: a wild shiny Riolu froze the battle on its opening text (player
// reports, v1.2.4-v1.2.6). Riolu's front animation (ANIM_RAPID_H_HOPS) ended
// one pixel off centre, and the shiny stars only finish while the sprite sits
// at x2 == 0, so the intro waited forever. Upstream expansion's fix (#10397)
// resets the offset; this pins the reported case with the animations on.
WILD_BATTLE_TEST("(WoT) A wild shiny Riolu's battle intro finishes")
{
    GIVEN {
        ASSUME(gSpeciesInfo[SPECIES_RIOLU].frontAnimId == ANIM_RAPID_H_HOPS);
        FORCE_MOVE_ANIM(TRUE);
        PLAYER(SPECIES_GIBLE) { Level(10); }
        OPPONENT(SPECIES_RIOLU) { Level(8); Shiny(TRUE); }
    } WHEN {
        TURN { }
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
    }
}

// WoT: cancelling the ball-target picker in a double battle put a ball back in
// the bag even when the bag never took it out (a ball thrown at a trainer's
// Pokemon with no Shadow out is handed back), so every cancel made a free ball.
TEST("(WoT) Cancelling a ball's target gives back only a ball the bag took")
{
    u32 savedFlags = gBattleTypeFlags;

    ASSUME(B_TRY_CATCH_TRAINER_BALL >= GEN_4);
    ClearBag();
    AddBagItem(ITEM_SNAG_BALL, 5);

    // Trainer battle, no Shadow out: the bag keeps the ball, so a cancel adds nothing.
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    if (WotBagUseRemovesItem(ITEM_SNAG_BALL))
        RemoveBagItem(ITEM_SNAG_BALL, 1);
    WotRefundCancelledBall(ITEM_SNAG_BALL);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_SNAG_BALL), 5);

    // Wild battle: the bag takes the ball, and a cancel puts it back.
    gBattleTypeFlags = BATTLE_TYPE_DOUBLE;
    if (WotBagUseRemovesItem(ITEM_SNAG_BALL))
        RemoveBagItem(ITEM_SNAG_BALL, 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_SNAG_BALL), 4);
    WotRefundCancelledBall(ITEM_SNAG_BALL);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_SNAG_BALL), 5);

    gBattleTypeFlags = savedFlags;
    ClearBag();
}
