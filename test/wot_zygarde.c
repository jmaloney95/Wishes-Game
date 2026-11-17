#include "global.h"
#include "event_data.h"
#include "item.h"
#include "test/battle.h"
#include "wot_mega_dimension.h"

// WoT: the Zygarde arc.

static void ResetArc(void)
{
    u32 i;

    ClearBag();
    for (i = 0; i < 19; i++)
        FlagClear(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + i);
    gSpecialVar_0x8004 = 0;
    WotZygarde_SetStage();
    FlagClear(FLAG_WOT_CAUGHT_ZYGARDE);
    FlagClear(FLAG_WOT_MEGA_DOOMSDAY_INTRO);
}

TEST("(WoT) Zygarde: the dog wants the ring, 3 claims, and no pebble yet")
{
    u32 i;

    ResetArc();
    FlagSet(FLAG_WOT_MEGA_DOOMSDAY_INTRO);
    for (i = 0; i < 3; i++)
        FlagSet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + i);
    EXPECT(!WotZygarde_PebbleDue());
    AddBagItem(ITEM_Z_POWER_RING, 1);
    EXPECT(WotZygarde_PebbleDue());
    AddBagItem(ITEM_ODD_PEBBLE, 1);
    EXPECT(!WotZygarde_PebbleDue());
}

TEST("(WoT) Zygarde: AZ's reveal waits for Mable's call and 20 Cells")
{
    ResetArc();
    AddBagItem(ITEM_ZYGARDE_CELL, 20);
    EXPECT(!WotZygarde_RevealDue());
    RemoveBagItem(ITEM_ZYGARDE_CELL, 1);
    gSpecialVar_0x8004 = 1;
    WotZygarde_SetStage();
    EXPECT_EQ(WotZygarde_GetStage(), 1);
    EXPECT(!WotZygarde_RevealDue());
    AddBagItem(ITEM_ZYGARDE_CELL, 1);
    EXPECT(WotZygarde_RevealDue());
    gSpecialVar_0x8004 = 2;
    WotZygarde_SetStage();
    EXPECT(!WotZygarde_RevealDue());
}

TEST("(WoT) Zygarde: Mable hands over the Cube once it's caught")
{
    ResetArc();
    AddBagItem(ITEM_Z_POWER_RING, 1);
    EXPECT_EQ(WotMegaDim_MableGiftDue(), 0);
    // Zygarde waits only once the machine is sealed (stage 3).
    gSpecialVar_0x8004 = 3;
    WotZygarde_SetStage();
    FlagSet(FLAG_WOT_CAUGHT_ZYGARDE);
    EXPECT_EQ(WotMegaDim_MableGiftDue(), 3);
    AddBagItem(ITEM_ZYGARDE_CUBE, 1);
    EXPECT_EQ(WotMegaDim_MableGiftDue(), 0);
    ResetArc();
    FlagClear(FLAG_WOT_CAUGHT_ZYGARDE);
}

WILD_BATTLE_TEST("(WoT) Zygarde: a Complete holding Zygardite Mega Evolves, and is caught as Complete")
{
    GIVEN {
        FLAG_SET(B_SMART_WILD_AI_FLAG);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ZYGARDE_COMPLETE) { Item(ITEM_ZYGARDITE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_CELEBRATE); }
        TURN { USE_ITEM(player, ITEM_MASTER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_SPECIES), SPECIES_ZYGARDE_COMPLETE);
    }
}

WILD_BATTLE_TEST("(WoT) Zygarde: Eternal Floette holding Floettite becomes Mega Floette")
{
    GIVEN {
        FLAG_SET(B_SMART_WILD_AI_FLAG);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_FLOETTE_ETERNAL) { Item(ITEM_FLOETTITE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_FLOETTE_MEGA);
    }
}

TEST("(WoT) Zygarde: with 20 Cells after the drain call, Mable sends you to the desk")
{
    ResetArc();
    gSpecialVar_0x8004 = 1; // ZYGARDE_STAGE_DRAIN_CALLED
    WotZygarde_SetStage();
    AddBagItem(ITEM_ZYGARDE_CELL, 19);
    EXPECT_EQ(WotZygarde_MableLine(), 2);
    EXPECT(!WotZygarde_RevealDue());
    AddBagItem(ITEM_ZYGARDE_CELL, 1);
    EXPECT_EQ(WotZygarde_MableLine(), 4);
    EXPECT(WotZygarde_RevealDue());
}
