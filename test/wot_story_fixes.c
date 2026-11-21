#include "global.h"
#include "event_data.h"
#include "item.h"
#include "constants/maps.h"
#include "test/overworld_script.h"
#include "test/test.h"

// WoT: the Rebel base Minister's giveitem of the SHINKANSEN KEY failed without
// a word when the Key Items pocket was full (player report: "all he talks
// about is his plan"), and the train stayed locked. Key items can't be tossed,
// so the Dojo script now says the pocket is full instead. This pins down the
// condition it reacts to: a full Key Items pocket makes the give fail.
TEST("(WoT) A full Key Items pocket makes giving the Shinkansen Key fail")
{
    u32 i;

    ASSUME(GetItemPocket(ITEM_SHINKANSEN_KEY) == POCKET_KEY_ITEMS);
    ClearBag();
    for (i = 0; i < BAG_KEYITEMS_COUNT; i++)
        BagPocket_SetSlotItemIdAndCount(&gBagPockets[POCKET_KEY_ITEMS], i, ITEM_TOWN_MAP, 1);

    RUN_OVERWORLD_SCRIPT(
        additem ITEM_SHINKANSEN_KEY;
    );
    EXPECT_EQ(VarGet(VAR_RESULT), FALSE);
    EXPECT(!CheckBagHasItem(ITEM_SHINKANSEN_KEY, 1));

    ClearBag();
    RUN_OVERWORLD_SCRIPT(
        additem ITEM_SHINKANSEN_KEY;
    );
    EXPECT_EQ(VarGet(VAR_RESULT), TRUE);
    EXPECT(CheckBagHasItem(ITEM_SHINKANSEN_KEY, 1));
    ClearBag();
}

// WoT: the Shin-Tokyo PC's Giratina dream is a post-game secret. Mid-story
// players met its silent nurse and took it for the old blank-text bug. Before
// the credits the PC door must lead back to the city, even on a save that was
// armed before this change; after the credits an armed save walks into the dream.
TEST("(WoT) Before the credits, the Shin-Tokyo PC door never leads into the dream")
{
    FlagClear(FLAG_SYS_GAME_CLEAR);
    FlagSet(FLAG_WOT_GIRATINA_DREAM_ARMED);
    RUN_OVERWORLD_SCRIPT(
        goto Tradewind_PC_EventScript_AimExit;
    );
    EXPECT(!FlagGet(FLAG_WOT_GIRATINA_DREAM_ARMED));
    EXPECT_EQ(gSaveBlock1Ptr->dynamicWarp.mapGroup, MAP_GROUP(MAP_SHIN_TOKYO));
    EXPECT_EQ(gSaveBlock1Ptr->dynamicWarp.mapNum, MAP_NUM(MAP_SHIN_TOKYO));

    FlagSet(FLAG_SYS_GAME_CLEAR);
    FlagSet(FLAG_WOT_GIRATINA_DREAM_ARMED);
    RUN_OVERWORLD_SCRIPT(
        goto Tradewind_PC_EventScript_AimExit;
    );
    EXPECT(FlagGet(FLAG_WOT_GIRATINA_DREAM_ARMED));
    EXPECT_EQ(gSaveBlock1Ptr->dynamicWarp.mapGroup, MAP_GROUP(MAP_SHIN_TOKYO_PC_DREAM));
    EXPECT_EQ(gSaveBlock1Ptr->dynamicWarp.mapNum, MAP_NUM(MAP_SHIN_TOKYO_PC_DREAM));

    FlagClear(FLAG_SYS_GAME_CLEAR);
    FlagClear(FLAG_WOT_GIRATINA_DREAM_ARMED);
}
