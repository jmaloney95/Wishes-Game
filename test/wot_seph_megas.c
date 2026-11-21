#include "global.h"
#include "test/battle.h"
#include "event_data.h"
#include "item.h"
#include "wot_mega_shop.h"

// WoT: Mega Lugia and Mega Ho-Oh (the artist's SEPH art, draft
// stats). Their stones come from the Minister, who looks the lead Pokemon's
// stone up in its form-change table.

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_LUGIA_MEGA].isMegaEvolution);
    ASSUME(gSpeciesInfo[SPECIES_HO_OH_MEGA].isMegaEvolution);
    ASSUME(GetItemHoldEffect(ITEM_LUGIANITE) == HOLD_EFFECT_MEGA_STONE);
    ASSUME(GetItemHoldEffect(ITEM_HO_OHNITE) == HOLD_EFFECT_MEGA_STONE);
}

SINGLE_BATTLE_TEST("(WoT) Lugia holding Lugianite Mega Evolves")
{
    GIVEN {
        PLAYER(SPECIES_LUGIA) { Item(ITEM_LUGIANITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, player);
        MESSAGE("Lugia has Mega Evolved into Mega Lugia!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_LUGIA_MEGA);
        EXPECT_EQ(player->ability, ABILITY_MULTISCALE);
    }
}

SINGLE_BATTLE_TEST("(WoT) Ho-Oh holding Ho-Ohnite Mega Evolves")
{
    GIVEN {
        PLAYER(SPECIES_HO_OH) { Item(ITEM_HO_OHNITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, player);
        MESSAGE("Ho-Oh has Mega Evolved into Mega Ho-Oh!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_HO_OH_MEGA);
        EXPECT_EQ(player->ability, ABILITY_DROUGHT);
    }
}

static const u16 sMinisterStones[][2] =
{
    {SPECIES_LUGIA, ITEM_LUGIANITE},
    {SPECIES_HO_OH, ITEM_HO_OHNITE},
};

TEST("(WoT) The Minister offers Lugianite and Ho-Ohnite for a Lugia or Ho-Oh lead")
{
    u32 i, n = 0;

    for (i = 0; i < ARRAY_COUNT(sMinisterStones); i++)
        PARAMETRIZE { n = i; }

    CreateMon(&gPlayerParty[0], sMinisterStones[n][0], 50, 0, OTID_STRUCT_PLAYER_ID);
    gSpecialVar_0x8004 = 0;
    EXPECT_EQ(WotGetPartyLeaderMegaStone(), sMinisterStones[n][1]);
    EXPECT_EQ(gSpecialVar_0x8006, 1);
}

// WoT: a Mega Stone in the bag counts as held for the player's own Pokemon, so
// it can Mega Evolve while holding something else, and the message names the
// stone from the bag.
SINGLE_BATTLE_TEST("(WoT) A Mega Stone in the bag lets the player's Pokemon Mega Evolve")
{
    ClearBag();
    AddBagItem(ITEM_LUCARIONITE, 1);
    GIVEN {
        PLAYER(SPECIES_LUCARIO) { Item(ITEM_LEFTOVERS); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } SCENE {
        MESSAGE("Lucario's Lucarionite is reacting to 1's Mega Ring!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, player);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_LUCARIO_MEGA);
        ClearBag();
    }
}
