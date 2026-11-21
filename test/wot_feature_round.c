#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "event_data.h"
#include "item.h"
#include "item_use.h"
#include "text.h"
#include "wot_battle_rules.h"
#include "wot_permanent_mega.h"
#include "ow_synchronize.h"
#include "pokemon.h"
#include "test/test.h"
#include "constants/form_change_types.h"
#include "constants/items.h"
#include "constants/opponents.h"

// WoT: instant text, Semi Shift, the TM floor, evolution fixes, permanent Megas.

const u16 *WotTmShop_BuildStock(u32 clerk);

static bool32 StockHas(const u16 *stock, u16 item)
{
    while (*stock != ITEM_NONE)
    {
        if (*stock++ == item)
            return TRUE;
    }
    return FALSE;
}

TEST("(WoT) Options: INSTANT is a text speed the game reads")
{
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_INSTANT;
    EXPECT_EQ(GetPlayerTextSpeed(), OPTIONS_TEXT_SPEED_INSTANT);
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST;
}

TEST("(WoT) Semi Shift: Set against a boss, Shift against anyone else")
{
    gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SHIFT;
    gSaveBlock2Ptr->optionsBattleStyleSemi = TRUE;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    TRAINER_BATTLE_PARAM.opponentB = 0;

    TRAINER_BATTLE_PARAM.opponentA = TRAINER_BLAKE;             // a level-cap boss
    EXPECT_EQ(WotGetBattleStyle(), OPTIONS_BATTLE_STYLE_SET);
    // Boss classes (gym leaders, the Z-A rivals...) need the game's trainer
    // data, which the test build swaps out, so only the cap bosses are checked.
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_ROUTE2_HIKER;      // an ordinary trainer
    EXPECT_EQ(WotGetBattleStyle(), OPTIONS_BATTLE_STYLE_SHIFT);

    // Plain SET and SHIFT ignore who the opponent is.
    gSaveBlock2Ptr->optionsBattleStyleSemi = FALSE;
    gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;
    EXPECT_EQ(WotGetBattleStyle(), OPTIONS_BATTLE_STYLE_SET);
    gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SHIFT;
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_BLAKE;
    EXPECT_EQ(WotGetBattleStyle(), OPTIONS_BATTLE_STYLE_SHIFT);
    gBattleTypeFlags = 0;
}

TEST("(WoT) TMs are reusable, and the TM floor's stock grows with the story")
{
    EXPECT(GetItemImportance(ITEM_TM_TOXIC));

    FlagClear(TRAINER_FLAGS_START + TRAINER_SENNEN_CAPTAIN_KANNON);
    FlagClear(TRAINER_FLAGS_START + TRAINER_BLAKE);
    EXPECT(StockHas(WotTmShop_BuildStock(0), ITEM_TM_WATER_PULSE));
    EXPECT(StockHas(WotTmShop_BuildStock(1), ITEM_TM_TOXIC));
    EXPECT(!StockHas(WotTmShop_BuildStock(0), ITEM_TM_FLAMETHROWER));
    EXPECT(!StockHas(WotTmShop_BuildStock(1), ITEM_TM_CALM_MIND));

    FlagSet(TRAINER_FLAGS_START + TRAINER_SENNEN_CAPTAIN_KANNON);
    EXPECT(StockHas(WotTmShop_BuildStock(0), ITEM_TM_FLAMETHROWER));
    EXPECT(StockHas(WotTmShop_BuildStock(1), ITEM_TM_CALM_MIND));
    EXPECT(!StockHas(WotTmShop_BuildStock(0), ITEM_TM_OVERHEAT));

    FlagSet(TRAINER_FLAGS_START + TRAINER_BLAKE);
    EXPECT(StockHas(WotTmShop_BuildStock(0), ITEM_TM_OVERHEAT));

    FlagClear(TRAINER_FLAGS_START + TRAINER_SENNEN_CAPTAIN_KANNON);
    FlagClear(TRAINER_FLAGS_START + TRAINER_BLAKE);
}

static const u16 sLevelEvos[][3] =
{
    // from, level, to
    {SPECIES_LARVESTA,         36, SPECIES_VOLCARONA},
    {SPECIES_DEINO,            16, SPECIES_ZWEILOUS},
    {SPECIES_ZWEILOUS,         36, SPECIES_HYDREIGON},
    {SPECIES_DRATINI,          16, SPECIES_DRAGONAIR},
    {SPECIES_DRAGONAIR,        36, SPECIES_DRAGONITE},
    {SPECIES_LARVITAR,         16, SPECIES_PUPITAR},
    {SPECIES_PUPITAR,          36, SPECIES_TYRANITAR},
    {SPECIES_BAGON,            16, SPECIES_SHELGON},
    {SPECIES_SHELGON,          36, SPECIES_SALAMENCE},
    {SPECIES_DREEPY,           16, SPECIES_DRAKLOAK},
    {SPECIES_DRAKLOAK,         36, SPECIES_DRAGAPULT},
    {SPECIES_GOOMY,            30, SPECIES_SLIGGOO},
    {SPECIES_JANGMO_O,         16, SPECIES_HAKAMO_O},
    {SPECIES_HAKAMO_O,         36, SPECIES_KOMMO_O},
    {SPECIES_PAWNIARD,         36, SPECIES_BISHARP},
    {SPECIES_VULLABY,          36, SPECIES_MANDIBUZZ},
    {SPECIES_NOIBAT,           36, SPECIES_NOIVERN},
    {SPECIES_VANILLITE,        30, SPECIES_VANILLISH},
    {SPECIES_VANILLISH,        40, SPECIES_VANILLUXE},
    {SPECIES_MANTYKE,          25, SPECIES_MANTINE},
    {SPECIES_YAMASK_GALAR,     34, SPECIES_RUNERIGUS},
    {SPECIES_GIMMIGHOUL_CHEST, 40, SPECIES_GHOLDENGO},
};

TEST("(WoT) Evolutions that came too late or never: the new levels")
{
    u32 i, n = 0;
    struct Pokemon mon;
    bool32 canStop = FALSE;

    for (i = 0; i < ARRAY_COUNT(sLevelEvos); i++)
        PARAMETRIZE { n = i; }

    CreateMon(&mon, sLevelEvos[n][0], sLevelEvos[n][1] - 1, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, CHECK_EVO), SPECIES_NONE);
    CreateMon(&mon, sLevelEvos[n][0], sLevelEvos[n][1], 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, CHECK_EVO), sLevelEvos[n][2]);
}

TEST("(WoT) Leader's Crest evolves Bisharp when used on it")
{
    struct Pokemon mon;
    bool32 canStop = FALSE;

    CreateMon(&mon, SPECIES_BISHARP, 40, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_ITEM_USE, ITEM_LEADERS_CREST, NULL, &canStop, CHECK_EVO), SPECIES_KINGAMBIT);
}

TEST("(WoT) Permanent Mega: the stone keeps it, taking it off or depositing reverts it")
{
    struct Pokemon mon;
    u16 item;

    EXPECT_EQ(WotGetMegaStone(SPECIES_VENUSAUR_MEGA), ITEM_VENUSAURITE);
    EXPECT_EQ(WotGetMegaBaseSpecies(SPECIES_VENUSAUR_MEGA), SPECIES_VENUSAUR);
    EXPECT_EQ(WotGetMegaStone(SPECIES_AMOONGUSS_MEGA), ITEM_AMOONGUSSITE);
    EXPECT_EQ(WotGetMegaBaseSpecies(SPECIES_AMOONGUSS_MEGA), SPECIES_AMOONGUSS);

    CreateMon(&mon, SPECIES_VENUSAUR_MEGA, 50, 0, OTID_STRUCT_PLAYER_ID);
    item = ITEM_VENUSAURITE;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    TryFormChange(&mon, FORM_CHANGE_ITEM_HOLD);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_VENUSAUR_MEGA);

    gWotPermanentMegaInTests = TRUE;
    TryFormChange(&mon, FORM_CHANGE_FAINT);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_VENUSAUR_MEGA);
    gWotPermanentMegaInTests = FALSE;

    item = ITEM_NONE;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    TryFormChange(&mon, FORM_CHANGE_ITEM_HOLD);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_VENUSAUR);

    EXPECT_EQ(WotGetOutOfBattleMegaTarget(FORM_CHANGE_DEPOSIT, SPECIES_AMOONGUSS_MEGA, ITEM_AMOONGUSSITE), SPECIES_AMOONGUSS);
    EXPECT_EQ(WotGetOutOfBattleMegaTarget(FORM_CHANGE_ITEM_HOLD, SPECIES_AMOONGUSS_MEGA, ITEM_LEFTOVERS), SPECIES_AMOONGUSS);
    EXPECT_EQ(WotGetOutOfBattleMegaTarget(FORM_CHANGE_ITEM_HOLD, SPECIES_AMOONGUSS_MEGA, ITEM_AMOONGUSSITE), SPECIES_AMOONGUSS_MEGA);
}

TEST("(WoT) Synchronize in the lead gives wild Pokemon its nature about half the time")
{
    u32 i, matched = 0, nature;
    u8 abilityNum = 0;   // Abra's first ability: Synchronize

    ZeroPlayerPartyMons();
    CreateMon(&gPlayerParty[0], SPECIES_ABRA, 10, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gPlayerParty[0], MON_DATA_ABILITY_NUM, &abilityNum);
    EXPECT_EQ(GetMonAbility(&gPlayerParty[0]), ABILITY_SYNCHRONIZE);
    nature = GetMonData(&gPlayerParty[0], MON_DATA_PERSONALITY) % NUM_NATURES;
    for (i = 0; i < 200; i++)
    {
        if (GetSynchronizedNature(WILDMON_ORIGIN, SPECIES_ZUBAT) == nature)
            matched++;
    }
    EXPECT_GE(matched, 70);
    EXPECT_LE(matched, 130);
    // Gifts are left alone.
    EXPECT_EQ(GetSynchronizedNature(GIFTMON_ORIGIN, SPECIES_ZUBAT), NATURE_RANDOM);
}

TEST("(WoT) A wild Scyther can hold a Metal Coat")
{
    EXPECT_EQ(gSpeciesInfo[SPECIES_SCYTHER].itemRare, ITEM_METAL_COAT);
}

// WoT: five more Ability Machines on Tradewind's TM floor (the attack clerk),
// each granting its ability through the usual Ability Machine use.
TEST("(WoT) Tradewind's TM floor sells the No Guard, Contrary, Protean and Primal AMs")
{
    static const u16 sNewAMs[][2] =
    {
        {ITEM_AM_NO_GUARD,       ABILITY_NO_GUARD},
        {ITEM_AM_CONTRARY,       ABILITY_CONTRARY},
        {ITEM_AM_PROTEAN,        ABILITY_PROTEAN},
        {ITEM_AM_DESOLATE_LAND,  ABILITY_DESOLATE_LAND},
        {ITEM_AM_PRIMORDIAL_SEA, ABILITY_PRIMORDIAL_SEA},
    };
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sNewAMs); i++)
    {
        EXPECT(StockHas(WotTmShop_BuildStock(0), sNewAMs[i][0]));
        EXPECT_EQ(GetItemSecondaryId(sNewAMs[i][0]), sNewAMs[i][1]);
        EXPECT(gItemsInfo[sNewAMs[i][0]].fieldUseFunc == ItemUseOutOfBattle_AbilityMachine);
        EXPECT_GT(GetItemPrice(sNewAMs[i][0]), 0);
    }
}
