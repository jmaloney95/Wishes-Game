#include "global.h"
#include "test/battle.h"
#include "wot_film_shiny.h"

// WoT: the debug "Catch: 100%" switch. With every shake check
// rigged to fail, a Poke Ball at a full-HP Beldum (catch rate 3) only lands
// while the switch is on.

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_BELDUM].catchRate == 3);
}

WILD_BATTLE_TEST("(WoT) Catch 100%: every ball catches only while it is on")
{
    bool32 on;

    PARAMETRIZE(on = FALSE);
    PARAMETRIZE(on = TRUE);

    GIVEN {
        WotFilmCatch_Set(on);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BELDUM) { Level(50); }
    } WHEN {
        TURN { USE_ITEM(player, ITEM_POKE_BALL, WITH_RNG(RNG_BALLTHROW_SHAKE, MAX_u16)); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_SPECIES), on ? SPECIES_BELDUM : SPECIES_NONE);
        WotFilmCatch_Set(FALSE);
    }
}
