#include "global.h"
#include "battle.h"
#include "wot_film_shiny.h"
#include "test/test.h"
#include "constants/battle.h"

// WoT: the debug "Shiny: next wild" / "Shiny: all wild" switches.
// WotFilmShiny_OnBattleStart runs as every battle starts; it only touches a
// wild battle's Pokemon, and only while a switch is on.

static void SetUpEnemy(u32 battleTypeFlags)
{
    u32 shiny = FALSE;

    ZeroEnemyPartyMons();
    CreateMon(&gEnemyParty[0], SPECIES_ZIGZAGOON, 5, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY, &shiny);
    gBattleTypeFlags = battleTypeFlags;
}

TEST("(WoT) Film shiny: nothing is forced while it is off")
{
    WotFilmShiny_SetMode(WOT_FILM_SHINY_OFF);
    SetUpEnemy(0);
    WotFilmShiny_OnBattleStart();
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY), FALSE);
    SetUpEnemy(BATTLE_TYPE_TRAINER);
    WotFilmShiny_OnBattleStart();
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY), FALSE);
}

TEST("(WoT) Film shiny: next wild is shiny once, then clears itself")
{
    WotFilmShiny_SetMode(WOT_FILM_SHINY_NEXT);
    SetUpEnemy(0);
    WotFilmShiny_OnBattleStart();
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY), TRUE);
    EXPECT_EQ(WotFilmShiny_GetMode(), WOT_FILM_SHINY_OFF);
    SetUpEnemy(0);
    WotFilmShiny_OnBattleStart();
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY), FALSE);
}

TEST("(WoT) Film shiny: trainer battles neither turn shiny nor use up next")
{
    WotFilmShiny_SetMode(WOT_FILM_SHINY_NEXT);
    SetUpEnemy(BATTLE_TYPE_TRAINER);
    WotFilmShiny_OnBattleStart();
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY), FALSE);
    EXPECT_EQ(WotFilmShiny_GetMode(), WOT_FILM_SHINY_NEXT);
    WotFilmShiny_SetMode(WOT_FILM_SHINY_OFF);
}

TEST("(WoT) Film shiny: all wild stays on until switched off")
{
    WotFilmShiny_SetMode(WOT_FILM_SHINY_ALL);
    SetUpEnemy(0);
    WotFilmShiny_OnBattleStart();
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY), TRUE);
    SetUpEnemy(BATTLE_TYPE_DOUBLE);
    WotFilmShiny_OnBattleStart();
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY), TRUE);
    EXPECT_EQ(WotFilmShiny_GetMode(), WOT_FILM_SHINY_ALL);
    WotFilmShiny_SetMode(WOT_FILM_SHINY_OFF);
    SetUpEnemy(0);
    WotFilmShiny_OnBattleStart();
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY), FALSE);
}

TEST("(WoT) Film shiny: a trainer team with a Shadow uses up next, one without does not")
{
    u32 shadow = TRUE;

    WotFilmShiny_SetMode(WOT_FILM_SHINY_NEXT);
    SetUpEnemy(BATTLE_TYPE_TRAINER);
    WotFilmShiny_OnTrainerPartyMade();
    EXPECT_EQ(WotFilmShiny_GetMode(), WOT_FILM_SHINY_NEXT);
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHADOW, &shadow);
    WotFilmShiny_OnTrainerPartyMade();
    EXPECT_EQ(WotFilmShiny_GetMode(), WOT_FILM_SHINY_OFF);
}

TEST("(WoT) Film shiny: all is not used up by a trainer Shadow team")
{
    u32 shadow = TRUE;

    WotFilmShiny_SetMode(WOT_FILM_SHINY_ALL);
    SetUpEnemy(BATTLE_TYPE_TRAINER);
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHADOW, &shadow);
    WotFilmShiny_OnTrainerPartyMade();
    EXPECT_EQ(WotFilmShiny_GetMode(), WOT_FILM_SHINY_ALL);
    WotFilmShiny_SetMode(WOT_FILM_SHINY_OFF);
}
