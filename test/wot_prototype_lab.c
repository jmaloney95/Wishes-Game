#include "global.h"
#include "event_data.h"
#include "field_specials.h"
#include "wot_film_shiny.h"
#include "test/test.h"
#include "constants/event_objects.h"

// WoT: the Prototype Lab Deoxys. Each trip in rolls whether it is
// shiny (always, while the debug shiny switch is on); the overworld sprite
// and the battle both follow that one roll.

static void SetUpDeoxys(u32 shiny)
{
    ZeroEnemyPartyMons();
    CreateMon(&gEnemyParty[0], SPECIES_DEOXYS, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY, &shiny);
}

TEST("(WoT) Prototype Lab: the shiny switch shows a shiny Deoxys and battles it shiny")
{
    FlagClear(FLAG_WOT_DEOXYS_CAUGHT);
    WotFilmShiny_SetMode(WOT_FILM_SHINY_ALL);
    WotPrototypeLab_RollDeoxysShiny();
    EXPECT_EQ(VarGet(VAR_TEMP_1), 1);
    EXPECT_EQ(VarGet(VAR_OBJ_GFX_ID_0), OBJ_EVENT_GFX_SPECIES_SHINY(DEOXYS));
    SetUpDeoxys(FALSE);
    WotPrototypeLab_SetDeoxysShiny();
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY), TRUE);
    WotFilmShiny_SetMode(WOT_FILM_SHINY_OFF);
}

TEST("(WoT) Prototype Lab: a Deoxys that showed normal colours is not shiny in battle")
{
    FlagClear(FLAG_WOT_DEOXYS_CAUGHT);
    WotFilmShiny_SetMode(WOT_FILM_SHINY_OFF);
    VarSet(VAR_TEMP_1, 0);
    SetUpDeoxys(TRUE);
    WotPrototypeLab_SetDeoxysShiny();
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY), FALSE);
}

TEST("(WoT) Prototype Lab: once Deoxys is caught there is no shiny roll")
{
    FlagSet(FLAG_WOT_DEOXYS_CAUGHT);
    WotFilmShiny_SetMode(WOT_FILM_SHINY_ALL);
    WotPrototypeLab_RollDeoxysShiny();
    EXPECT_EQ(VarGet(VAR_TEMP_1), 0);
    EXPECT_EQ(VarGet(VAR_OBJ_GFX_ID_0), OBJ_EVENT_GFX_SPECIES(DEOXYS));
    WotFilmShiny_SetMode(WOT_FILM_SHINY_OFF);
    FlagClear(FLAG_WOT_DEOXYS_CAUGHT);
}
