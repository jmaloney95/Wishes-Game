#include "global.h"
#include "battle.h"
#include "battle_interface.h"
#include "item.h"
#include "test/battle.h"

// WoT Shadow system: the R quick-throw ("last used ball") also shows in a
// trainer battle while one of the trainer's Shadows is out, so a snag is one
// button away, and a Shadow battle opens on a Snag Ball. CanThrowLastUsedBall
// and WotIsQuickBallTarget decide whether R shows; WotPickShadowQuickBall
// picks the ball it offers.

static void GiveOnlyPokeBall(void)
{
    ClearBag();
    AddBagItem(ITEM_POKE_BALL, 1);
    gBallToDisplay = ITEM_POKE_BALL;
}

// SINGLE/DOUBLE_BATTLE_TEST play a recorded link battle, where R never shows
// (no items in link battles). Drop the link bits to judge it as the in-game
// trainer battle it stands in for.
static void AsInGameTrainerBattle(void)
{
    EXPECT(!CanThrowLastUsedBall());
    gBattleTypeFlags &= ~(BATTLE_TYPE_RECORDED_LINK | BATTLE_TYPE_RECORDED_IS_MASTER);
}

TEST("(WoT) Quick-throw opens a Shadow battle on a Snag Ball")
{
    ClearBag();
    AddBagItem(ITEM_ULTRA_BALL, 1);
    AddBagItem(ITEM_SNAG_BALL, 1);
    gLastThrownBall = ITEM_ULTRA_BALL;
    EXPECT_EQ(WotPickShadowQuickBall(ITEM_ULTRA_BALL, TRUE), ITEM_SNAG_BALL);
    gLastThrownBall = ITEM_NONE;
    ClearBag();
}

TEST("(WoT) Quick-throw keeps the player's own pick mid-battle")
{
    ClearBag();
    AddBagItem(ITEM_ULTRA_BALL, 1);
    AddBagItem(ITEM_SNAG_BALL, 1);
    EXPECT_EQ(WotPickShadowQuickBall(ITEM_ULTRA_BALL, FALSE), ITEM_ULTRA_BALL);
    ClearBag();
}

TEST("(WoT) Quick-throw falls back to a Snag Ball, then the last ball thrown, then any ball")
{
    ClearBag();
    gLastThrownBall = ITEM_GREAT_BALL;
    AddBagItem(ITEM_NET_BALL, 1);
    AddBagItem(ITEM_GREAT_BALL, 1);
    // No Snag Ball: the last ball thrown.
    EXPECT_EQ(WotPickShadowQuickBall(ITEM_SNAG_BALL, TRUE), ITEM_GREAT_BALL);
    // The ball on show ran out mid-battle: a Snag Ball comes first again.
    AddBagItem(ITEM_SNAG_BALL, 1);
    EXPECT_EQ(WotPickShadowQuickBall(ITEM_MASTER_BALL, FALSE), ITEM_SNAG_BALL);
    // Neither a Snag Ball nor the last ball thrown: the first ball in the bag.
    ClearBag();
    AddBagItem(ITEM_NET_BALL, 1);
    EXPECT_EQ(WotPickShadowQuickBall(ITEM_SNAG_BALL, TRUE), ITEM_NET_BALL);
    // No balls at all: left as it was (R stays hidden, the bag has nothing).
    ClearBag();
    EXPECT_EQ(WotPickShadowQuickBall(ITEM_SNAG_BALL, TRUE), ITEM_SNAG_BALL);
    gLastThrownBall = ITEM_NONE;
}

SINGLE_BATTLE_TEST("(WoT) Quick-throw shows against a trainer's Shadow and not its other Pokemon")
{
    bool32 isShadow;

    PARAMETRIZE { isShadow = FALSE; }
    PARAMETRIZE { isShadow = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Shadow(isShadow); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        GiveOnlyPokeBall();
        AsInGameTrainerBattle();
        EXPECT_EQ(WotIsQuickBallTarget(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)), isShadow);
        EXPECT_EQ(CanThrowLastUsedBall(), isShadow);
        ClearBag();
    }
}

WILD_BATTLE_TEST("(WoT) Quick-throw still shows in a wild battle, Shadow or not")
{
    bool32 isShadow;

    PARAMETRIZE { isShadow = FALSE; }
    PARAMETRIZE { isShadow = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Shadow(isShadow); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        GiveOnlyPokeBall();
        EXPECT(CanThrowLastUsedBall());
        // Without the ball on show in the bag, R stays hidden.
        ClearBag();
        EXPECT(!CanThrowLastUsedBall());
    }
}

DOUBLE_BATTLE_TEST("(WoT) Quick-throw in a trainer double aims only at Shadows")
{
    bool32 leftShadow, rightShadow;

    PARAMETRIZE { leftShadow = FALSE; rightShadow = FALSE; }
    PARAMETRIZE { leftShadow = TRUE;  rightShadow = FALSE; }
    PARAMETRIZE { leftShadow = FALSE; rightShadow = TRUE; }
    PARAMETRIZE { leftShadow = TRUE;  rightShadow = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Shadow(leftShadow); }
        OPPONENT(SPECIES_WOBBUFFET) { Shadow(rightShadow); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        GiveOnlyPokeBall();
        AsInGameTrainerBattle();
        EXPECT_EQ(WotIsQuickBallTarget(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)), leftShadow);
        EXPECT_EQ(WotIsQuickBallTarget(GetBattlerAtPosition(B_POSITION_OPPONENT_RIGHT)), rightShadow);
        EXPECT_EQ(WotIsQuickBallTarget(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), FALSE);
        EXPECT_EQ(CanThrowLastUsedBall(), leftShadow || rightShadow);
        ClearBag();
    }
}
