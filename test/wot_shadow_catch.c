#include "global.h"
#include "test/battle.h"

// WoT Shadow system: Shadow Pokemon get a catch-rate floor (90, or 45 for
// legends) in ComputeCaptureOdds. At full HP the recorded odds are
// catchRate / 3 with a plain Poke Ball.

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_BELDUM].catchRate == 3);
    ASSUME(gSpeciesInfo[SPECIES_DEOXYS_NORMAL].catchRate == 3);
    ASSUME(gSpeciesInfo[SPECIES_DEOXYS_NORMAL].isMythical);
    ASSUME(gSpeciesInfo[SPECIES_CLEFFA].catchRate == 150);
}

WILD_BATTLE_TEST("(WoT) Shadow catch-rate floor raises low-rate Shadows only")
{
    u32 expectedOdds;
    u32 recordedOdds;
    u32 species;
    bool32 isShadow;

    PARAMETRIZE(expectedOdds = 1,  species = SPECIES_BELDUM,        isShadow = FALSE);
    PARAMETRIZE(expectedOdds = 30, species = SPECIES_BELDUM,        isShadow = TRUE);
    PARAMETRIZE(expectedOdds = 1,  species = SPECIES_DEOXYS_NORMAL, isShadow = FALSE);
    PARAMETRIZE(expectedOdds = 15, species = SPECIES_DEOXYS_NORMAL, isShadow = TRUE);
    PARAMETRIZE(expectedOdds = 50, species = SPECIES_CLEFFA,        isShadow = TRUE);

    GIVEN {
        WITH_CONFIG(B_MISSING_BADGE_CATCH_MALUS, GEN_7);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species) { Level(50); Shadow(isShadow); }
    } WHEN {
        TURN { USE_ITEM(player, ITEM_POKE_BALL); }
    } SCENE {
        CATCHING_CHANCE(&recordedOdds);
    } THEN {
        EXPECT_EQ(expectedOdds, recordedOdds);
    }
}

WILD_BATTLE_TEST("(WoT) Snag Ball stacks on top of the Shadow floor")
{
    u32 recordedOdds;

    GIVEN {
        WITH_CONFIG(B_MISSING_BADGE_CATCH_MALUS, GEN_7);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BELDUM) { Level(50); Shadow(TRUE); }
    } WHEN {
        TURN { USE_ITEM(player, ITEM_SNAG_BALL); }
    } SCENE {
        CATCHING_CHANCE(&recordedOdds);
    } THEN {
        EXPECT_EQ(120, recordedOdds);
    }
}
