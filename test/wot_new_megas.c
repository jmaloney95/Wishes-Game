#include "global.h"
#include "test/battle.h"
#include "battle_gimmick.h"
#include "battle_interface.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_specials.h"
#include "palette.h"
#include "wot_shadow_art.h"
#include "constants/event_objects.h"

// WoT: Mega Haxorus and Mega Deoxys (the artist's Gen X Evos art).
// The Prototype Lab's Shadow Deoxys holds Deoxysite and has the smart-wild-AI
// flag, so it Mega Evolves on its own. It stays a Shadow (flag, purple flame
// icon), but its Mega art is shown as drawn: no violet tint, and no shiny
// colours either, because the Mega has no shiny art.

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_HAXORUS_MEGA].isMegaEvolution);
    ASSUME(gSpeciesInfo[SPECIES_DEOXYS_MEGA].isMegaEvolution);
    ASSUME(GetItemHoldEffect(ITEM_HAXORITE) == HOLD_EFFECT_MEGA_STONE);
    ASSUME(GetItemHoldEffect(ITEM_DEOXYSITE) == HOLD_EFFECT_MEGA_STONE);
}

SINGLE_BATTLE_TEST("(WoT) Haxorus holding Haxorite Mega Evolves")
{
    GIVEN {
        PLAYER(SPECIES_HAXORUS) { Item(ITEM_HAXORITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, player);
        MESSAGE("Haxorus has Mega Evolved into Mega Haxorus!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_HAXORUS_MEGA);
    }
}

SINGLE_BATTLE_TEST("(WoT) Deoxys holding Deoxysite Mega Evolves from its Normal Forme only")
{
    u32 species;
    PARAMETRIZE { species = SPECIES_DEOXYS_NORMAL; }
    PARAMETRIZE { species = SPECIES_DEOXYS_ATTACK; }
    GIVEN {
        PLAYER(species) { Item(ITEM_DEOXYSITE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, gimmick: species == SPECIES_DEOXYS_NORMAL ? GIMMICK_MEGA : GIMMICK_NONE); }
    } THEN {
        EXPECT_EQ(player->species, species == SPECIES_DEOXYS_NORMAL ? SPECIES_DEOXYS_MEGA : SPECIES_DEOXYS_ATTACK);
    }
}

TEST("(WoT) Mega Deoxys keeps its own art as a Shadow, and has no shiny colours")
{
    EXPECT(WotShadowKeepsOwnArt(SPECIES_DEOXYS_MEGA));
    EXPECT(!WotShadowKeepsOwnArt(SPECIES_DEOXYS_NORMAL));
    EXPECT(!WotShadowKeepsOwnArt(SPECIES_SNORLAX_MEGA));
    for (u32 i = 0; i < 16; i++)
        EXPECT_EQ(gSpeciesInfo[SPECIES_DEOXYS_MEGA].shinyPalette[i], gSpeciesInfo[SPECIES_DEOXYS_MEGA].palette[i]);
}

// The Prototype Lab battle: Deoxys @ Deoxysite, Shadow, smart-wild flag set.
// Animations are forced on so the Mega's sprite swap runs. Deoxys then hides
// behind a Substitute that the player breaks on turn two, which reloads its
// battle sprite through BattleLoadMonSpriteGfx (the path that coming back
// from the Bag takes too). That reload is where the Shadow tint used to land.
// (The test body runs on a 1 KB stack, so the reload can't be called from
// THEN directly.)
WILD_BATTLE_TEST("(WoT) The Prototype Lab's Shadow Deoxys Mega Evolves and stays a Shadow")
{
    bool32 shiny;
    PARAMETRIZE { shiny = FALSE; }
    PARAMETRIZE { shiny = TRUE; }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        FLAG_SET(B_SMART_WILD_AI_FLAG);
        PLAYER(SPECIES_MEWTWO) { Level(100); Moves(MOVE_SHADOW_BALL, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DEOXYS) { Level(50); Item(ITEM_DEOXYSITE); Shadow(TRUE); Shiny(shiny); Moves(MOVE_SUBSTITUTE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_SHADOW_BALL); }
    } SCENE {
        MESSAGE("The wild Deoxys is overflowing with Mega energy!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
        MESSAGE("The wild Deoxys has Mega Evolved into Mega Deoxys!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_BALL, player);
        MESSAGE("The wild Deoxys's substitute faded!");
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        const u16 *megaPal = gSpeciesInfo[SPECIES_DEOXYS_MEGA].palette;

        FORCE_MOVE_ANIM(FALSE);
        EXPECT_EQ(opponent->species, SPECIES_DEOXYS_MEGA);
        EXPECT_EQ(GetMonData(&gEnemyParty[gBattlerPartyIndexes[battler]], MON_DATA_IS_SHADOW), TRUE);
        // The purple flame by its level, not the Mega icon.
        EXPECT_EQ(GetIndicatorPalTag(battler), TAG_MISC_INDICATOR_PAL);
        // After the reload: the Mega's own colours, untinted, shiny or not.
        for (u32 i = 1; i < 16; i++)
            EXPECT_EQ(gPlttBufferUnfaded[OBJ_PLTT_ID(battler) + i], megaPal[i]);
    }
}

// Control for the palette check above: any other Shadow Mega still gets the
// violet tint on the same reload.
WILD_BATTLE_TEST("(WoT) Other Shadow Megas keep the violet tint")
{
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        FLAG_SET(B_SMART_WILD_AI_FLAG);
        PLAYER(SPECIES_MEWTWO) { Level(100); Moves(MOVE_PSYSTRIKE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SNORLAX) { Level(50); Item(ITEM_SNORLAXITE); Shadow(TRUE); Moves(MOVE_SUBSTITUTE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_PSYSTRIKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
        MESSAGE("The wild Snorlax's substitute faded!");
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);
        const u16 *megaPal = gSpeciesInfo[SPECIES_SNORLAX_MEGA].palette;
        u32 same = 0;

        FORCE_MOVE_ANIM(FALSE);
        EXPECT_EQ(opponent->species, SPECIES_SNORLAX_MEGA);
        EXPECT_EQ(GetIndicatorPalTag(battler), TAG_MISC_INDICATOR_PAL);
        for (u32 i = 1; i < 16; i++)
            same += (gPlttBufferUnfaded[OBJ_PLTT_ID(battler) + i] == megaPal[i]);
        EXPECT_LT(same, 15);
    }
}

WILD_BATTLE_TEST("(WoT) A non-Shadow wild Mega shows the Mega icon")
{
    GIVEN {
        FLAG_SET(B_SMART_WILD_AI_FLAG);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DEOXYS) { Item(ITEM_DEOXYSITE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponent);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_DEOXYS_MEGA);
        EXPECT_EQ(GetIndicatorPalTag(GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)), TAG_MEGA_INDICATOR_PAL);
    }
}

TEST("(WoT) The starters have their own follower sprites")
{
    static const u16 starters[] = {SPECIES_GECQUA, SPECIES_POMBON, SPECIES_BROWT};

    for (u32 i = 0; i < ARRAY_COUNT(starters); i++)
    {
        EXPECT(gSpeciesInfo[starters[i]].overworldData.images != NULL);
        EXPECT(gSpeciesInfo[starters[i]].overworldData.anims == sAnimTable_Following_Asym);
        EXPECT(gSpeciesInfo[starters[i]].overworldPalette != NULL);
    }
}

// The Distortion World's floor stones: Garchompite (DW5) and Baxcalibrite (DW4)
// for the Gible and Frigibax lines, and at DW3 Haxorite for a player who chose
// Axew (VAR_STARTER_CHOICE 3), Gyaradosite for everyone else.
TEST("(WoT) Distortion World 3's floor stone is Haxorite only for an Axew starter")
{
    for (u32 choice = 0; choice <= 3; choice++)
    {
        VarSet(VAR_STARTER_CHOICE, choice);
        FlagClear(FLAG_WOT_RAND_STARTERS);
        WotDistortionWorld_StarterStone();
        EXPECT_EQ(gSpecialVar_Result, choice == 3 ? ITEM_HAXORITE : ITEM_GYARADOSITE);
        EXPECT_EQ(VarGet(VAR_OBJ_GFX_ID_0), choice == 3 ? OBJ_EVENT_GFX_ITEM_HAXORITE : OBJ_EVENT_GFX_ITEM_GYARADOSITE);
    }
    // A randomized ball 3 isn't an Axew, so the floor keeps its Gyaradosite.
    VarSet(VAR_STARTER_CHOICE, 3);
    FlagSet(FLAG_WOT_RAND_STARTERS);
    WotDistortionWorld_StarterStone();
    EXPECT_EQ(gSpecialVar_Result, ITEM_GYARADOSITE);
    EXPECT_EQ(VarGet(VAR_OBJ_GFX_ID_0), OBJ_EVENT_GFX_ITEM_GYARADOSITE);
    FlagClear(FLAG_WOT_RAND_STARTERS);
    VarSet(VAR_STARTER_CHOICE, 0);
}

static bool32 MegaEvolvesWith(u16 species, u16 mega, u16 stone)
{
    const struct FormChange *table = gSpeciesInfo[species].formChangeTable;

    for (u32 i = 0; table != NULL && table[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (table[i].method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM && table[i].targetSpecies == mega && table[i].param1 == stone)
            return TRUE;
    }
    return FALSE;
}

TEST("(WoT) Each starter line's final stage Mega Evolves with its Distortion World stone")
{
    EXPECT(MegaEvolvesWith(SPECIES_GARCHOMP, SPECIES_GARCHOMP_MEGA, ITEM_GARCHOMPITE));
    EXPECT(MegaEvolvesWith(SPECIES_BAXCALIBUR, SPECIES_BAXCALIBUR_MEGA, ITEM_BAXCALIBRITE));
    EXPECT(MegaEvolvesWith(SPECIES_HAXORUS, SPECIES_HAXORUS_MEGA, ITEM_HAXORITE));
    EXPECT(MegaEvolvesWith(SPECIES_DEOXYS_NORMAL, SPECIES_DEOXYS_MEGA, ITEM_DEOXYSITE));
}
