#include "global.h"
#include "test/battle.h"
#include "constants/battle_anim.h"

// WoT: the Gen X starter evolutions (the design doc, the artist's
// Gen X Evos sheet). Each line evolves at Lv. 16 and Lv. 36; each final form
// learns its signature move on evolving.

static const u16 sGenXEvos[][3] =
{
    // from, level, to
    {SPECIES_BROWT,      16, SPECIES_SPROUTBILL},
    {SPECIES_SPROUTBILL, 36, SPECIES_HORNBREW},
    {SPECIES_GECQUA,     16, SPECIES_TOKIQUA},
    {SPECIES_TOKIQUA,    36, SPECIES_GECQUADORF},
    {SPECIES_POMBON,     16, SPECIES_PYRAPOM},
    {SPECIES_PYRAPOM,    36, SPECIES_CALDERUFF},
};

static const u16 sGenXSignatureMoves[][2] =
{
    {SPECIES_HORNBREW,   MOVE_KOPI_RUSH},
    {SPECIES_GECQUADORF, MOVE_OMEN_WAVE},
    {SPECIES_CALDERUFF,  MOVE_CALDERA_CRASH},
};

TEST("(WoT) Gen X starter lines evolve at Lv. 16 and Lv. 36")
{
    u32 i, n = 0;
    struct Pokemon mon;
    bool32 canStop = FALSE;

    for (i = 0; i < ARRAY_COUNT(sGenXEvos); i++)
        PARAMETRIZE { n = i; }

    CreateMon(&mon, sGenXEvos[n][0], sGenXEvos[n][1] - 1, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, CHECK_EVO), SPECIES_NONE);

    CreateMon(&mon, sGenXEvos[n][0], sGenXEvos[n][1], 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, CHECK_EVO), sGenXEvos[n][2]);
}

TEST("(WoT) Gen X final forms learn their signature move on evolving")
{
    u32 i, n = 0;
    const struct LevelUpMove *learnset;

    for (i = 0; i < ARRAY_COUNT(sGenXSignatureMoves); i++)
        PARAMETRIZE { n = i; }

    learnset = GetSpeciesLevelUpLearnset(sGenXSignatureMoves[n][0]);
    EXPECT_EQ(learnset[0].level, 0);
    EXPECT_EQ(learnset[0].move, sGenXSignatureMoves[n][1]);
}

TEST("(WoT) Gen X final forms have the design doc's types")
{
    EXPECT_EQ(GetSpeciesType(SPECIES_HORNBREW, 0), TYPE_GRASS);
    EXPECT_EQ(GetSpeciesType(SPECIES_HORNBREW, 1), TYPE_FLYING);
    EXPECT_EQ(GetSpeciesType(SPECIES_GECQUADORF, 0), TYPE_WATER);
    EXPECT_EQ(GetSpeciesType(SPECIES_GECQUADORF, 1), TYPE_PSYCHIC);
    EXPECT_EQ(GetSpeciesType(SPECIES_CALDERUFF, 0), TYPE_FIRE);
    EXPECT_EQ(GetSpeciesType(SPECIES_CALDERUFF, 1), TYPE_GROUND);
}

SINGLE_BATTLE_TEST("(WoT) Omen Wave doubles its power on a target that switched in this turn", s16 damage)
{
    bool32 switchIn;
    PARAMETRIZE { switchIn = FALSE; }
    PARAMETRIZE { switchIn = TRUE; }
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_OMEN_WAVE) == EFFECT_OMEN_WAVE);
        PLAYER(SPECIES_GECQUADORF) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); }
    } WHEN {
        TURN {
            MOVE(player, MOVE_OMEN_WAVE);
            if (switchIn)
                SWITCH(opponent, 1);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_OMEN_WAVE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("(WoT) Kopi Rush can raise the user's Speed")
{
    PASSES_RANDOMLY(30, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_HORNBREW);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_KOPI_RUSH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KOPI_RUSH, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("(WoT) Caldera Crash can burn the target")
{
    PASSES_RANDOMLY(20, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_CALDERUFF);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CALDERA_CRASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALDERA_CRASH, player);
        STATUS_ICON(opponent, burn: TRUE);
    }
}

// The custom animation hides the user during its leap and loads the magma geyser
// sheet; both have to be undone when it ends, from either side of the field.
SINGLE_BATTLE_TEST("(WoT) Caldera Crash's animation plays out and cleans up")
{
    struct BattlePokemon *user = NULL;
    PARAMETRIZE { user = player; }
    PARAMETRIZE { user = opponent; }
    FORCE_MOVE_ANIM(TRUE);
    GIVEN {
        PLAYER(SPECIES_CALDERUFF) { Moves(MOVE_CALDERA_CRASH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CALDERUFF) { Moves(MOVE_CALDERA_CRASH, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(user, MOVE_CALDERA_CRASH); MOVE(user == player ? opponent : player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALDERA_CRASH, user);
    } THEN {
        FORCE_MOVE_ANIM(FALSE);
        EXPECT(!gSprites[gBattlerSpriteIds[user - gBattleMons]].invisible);
        EXPECT_EQ(gSprites[gBattlerSpriteIds[user - gBattleMons]].x2, 0);
        EXPECT_EQ(gSprites[gBattlerSpriteIds[user - gBattleMons]].y2, 0);
        EXPECT_EQ(GetSpriteTileStartByTag(ANIM_TAG_CALDERA_MAGMA), 0xFFFF);
        EXPECT_EQ(IndexOfSpritePaletteTag(ANIM_TAG_CALDERA_MAGMA), 0xFF);
    }
}
