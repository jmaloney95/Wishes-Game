#include "global.h"
#include "event_data.h"
#include "item.h"
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
