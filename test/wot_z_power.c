#include "global.h"
#include "event_data.h"
#include "item.h"
#include "wot_mega_dimension.h"
#include "test/test.h"
#include "constants/items.h"

// WoT Z-Power: Mable's gifts, AZ's stock, the partners' crystals,
// and the tally that the followers option used to share bits with.

#define CLAIM_COUNT 23

u16 WotMegaDim_MableReport(void);

static void ResetZPower(void)
{
    u32 i;

    ClearBag();
    for (i = 0; i < CLAIM_COUNT; i++)
        WotMegaDim_SetMegaClaimed(i, FALSE);
    FlagClear(FLAG_WOT_CAUGHT_MEGA_RAYQUAZA);
    for (i = 0; i < 5; i++)
        FlagClear(FLAG_WOT_MABLE_TALLY_START + i);
    for (i = 0; i < 3; i++)
        FlagClear(FLAG_WOT_MABLE_STAGE_START + i);
    for (i = 0; i < WOT_PARTNER_SAVE_COUNT; i++)
    {
        gSaveBlock1Ptr->wotPartners[i].runs = 0;
        gSaveBlock1Ptr->wotPartners[i].sanctum = 0;
    }
}

static bool32 StockHas(const u16 *stock, u16 item)
{
    while (*stock != ITEM_NONE)
    {
        if (*stock++ == item)
            return TRUE;
    }
    return FALSE;
}

static u32 StockCount(const u16 *stock)
{
    u32 n = 0;

    while (*stock++ != ITEM_NONE)
        n++;
    return n;
}

TEST("(WoT) Z-Power: the followers option no longer shares bits with Mable's tally")
{
    STATIC_ASSERT(FLAG_WOT_FOLLOWERS_OFF < FLAG_WOT_MABLE_TALLY_START
               || FLAG_WOT_FOLLOWERS_OFF >= FLAG_WOT_MABLE_TALLY_START + 5, FollowersOutsideTally);
}

TEST("(WoT) Z-Power: a tally inflated by the old flag clash repairs itself")
{
    ResetZPower();
    FlagSet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS);
    FlagSet(FLAG_WOT_MABLE_TALLY_START + 1); // the old followers bit: reads as 2
    EXPECT_EQ(WotMegaDim_MableReport(), 0);
    EXPECT_EQ(gSpecialVar_0x8004, 1);
    EXPECT(!FlagGet(FLAG_WOT_MABLE_TALLY_START + 1));
    FlagSet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + 1);
    EXPECT_EQ(WotMegaDim_MableReport(), 1);
    EXPECT_EQ(gSpecialVar_0x8004, 2);
}

TEST("(WoT) Z-Power: Mable owes the ring after a claim, then the core after Rayquaza")
{
    ResetZPower();
    EXPECT_EQ(WotMegaDim_MableGiftDue(), 0);
    FlagSet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS);
    EXPECT_EQ(WotMegaDim_MableGiftDue(), 1);
    AddBagItem(ITEM_Z_POWER_RING, 1);
    EXPECT_EQ(WotMegaDim_MableGiftDue(), 0);
    FlagSet(FLAG_WOT_CAUGHT_MEGA_RAYQUAZA);
    EXPECT_EQ(WotMegaDim_MableGiftDue(), 2);
    AddBagItem(ITEM_ROGUE_CORE, 1);
    EXPECT_EQ(WotMegaDim_MableGiftDue(), 0);
}

TEST("(WoT) Z-Power: the Distortion World's Rift Core is not the Rogue Core")
{
    ResetZPower();
    FlagSet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS);
    AddBagItem(ITEM_Z_POWER_RING, 1);
    FlagSet(FLAG_WOT_CAUGHT_MEGA_RAYQUAZA);
    AddBagItem(ITEM_RIFT_CORE, 1);
    EXPECT_EQ(WotMegaDim_MableGiftDue(), 2);
}

TEST("(WoT) Z-Power: AZ's stock follows the claimed rogues' types")
{
    const u16 *stock;

    ResetZPower();
    stock = WotZShop_BuildStock();
    EXPECT_EQ(StockCount(stock), 1);
    EXPECT(StockHas(stock, ITEM_NORMALIUM_Z));

    FlagSet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS); // Grass/Poison
    stock = WotZShop_BuildStock();
    EXPECT_EQ(StockCount(stock), 3);
    EXPECT(StockHas(stock, ITEM_GRASSIUM_Z));
    EXPECT(StockHas(stock, ITEM_POISONIUM_Z));
    EXPECT(!StockHas(stock, ITEM_ICIUM_Z));
}

TEST("(WoT) Z-Power: all 23 claims stock every crystal and Snorlium")
{
    u32 i;
    const u16 *stock;

    ResetZPower();
    for (i = 0; i < CLAIM_COUNT; i++)
        WotMegaDim_SetMegaClaimed(i, TRUE);
    stock = WotZShop_BuildStock();
    EXPECT_EQ(StockCount(stock), 19);
    EXPECT(StockHas(stock, ITEM_ICIUM_Z));
    EXPECT(StockHas(stock, ITEM_GHOSTIUM_Z));
    EXPECT(StockHas(stock, ITEM_SNORLIUM_Z));
    EXPECT_EQ(WotZShop_GetPrice(ITEM_SNORLIUM_Z), 60);
    EXPECT_EQ(WotZShop_GetPrice(ITEM_FIRIUM_Z), 40);
}

TEST("(WoT) Z-Power: a partner gives their crystal once, at friendship Lv3")
{
    ResetZPower();
    gSpecialVar_0x8004 = 1; // Canari
    WotPartner_TakeCrystalGift();
    EXPECT_EQ(gSpecialVar_Result, ITEM_NONE);

    gSaveBlock1Ptr->wotPartners[0].runs = 255;
    EXPECT_GE(WotPartner_GetLevel(0), 3);
    gSaveBlock1Ptr->wotPartners[0].sanctum = 1 << 4; // bit 4 isn't the gift's
    WotPartner_TakeCrystalGift();
    EXPECT_EQ(gSpecialVar_Result, ITEM_ELECTRIUM_Z);
    EXPECT_EQ(gSaveBlock1Ptr->wotPartners[0].sanctum, (1 << 4) | (1 << 5));

    WotPartner_TakeCrystalGift();
    EXPECT_EQ(gSpecialVar_Result, ITEM_NONE);
}

TEST("(WoT) Z-Power: a Zygarde catch flagged before the machine is sealed is false")
{
    ResetZPower();
    FlagSet(FLAG_WOT_CAUGHT_ZYGARDE);
    AddBagItem(ITEM_ZYGARDE_CUBE, 1);
    gSpecialVar_0x8004 = 0;
    WotZygarde_SetStage();
    EXPECT_EQ(WotMegaDim_MableGiftDue(), 0);
    EXPECT(!FlagGet(FLAG_WOT_CAUGHT_ZYGARDE));
    EXPECT(!CheckBagHasItem(ITEM_ZYGARDE_CUBE, 1));
}

TEST("(WoT) Z-Power: a real Zygarde catch still earns the Cube")
{
    ResetZPower();
    FlagSet(FLAG_WOT_CAUGHT_ZYGARDE);
    // Stage 3: the machine is sealed, the only time Zygarde can be caught.
    gSpecialVar_0x8004 = 3;
    WotZygarde_SetStage();
    EXPECT_EQ(WotMegaDim_MableGiftDue(), 3);
    EXPECT(FlagGet(FLAG_WOT_CAUGHT_ZYGARDE));
    gSpecialVar_0x8004 = 0;
    WotZygarde_SetStage();
    FlagClear(FLAG_WOT_CAUGHT_ZYGARDE);
}
