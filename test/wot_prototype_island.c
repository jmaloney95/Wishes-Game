#include "global.h"
#include "event_data.h"
#include "wot_shadow_log.h"
#include "test/test.h"
#include "constants/flags.h"
#include "constants/species.h"

// WoT: Prototype Island. The pier guard's Shadow Pokemon are snag targets, so
// each must be a row on the Shadow Log; the lab gate's flag must be a plain
// saved flag (not a temp, trainer or daily one, which get cleared).
// (The test runner swaps in its own trainer table, so the guard's party in
// trainers.party is named here rather than read: Archeops and Arctozolt.)

TEST("(WoT) Prototype Island: the pier guard's Shadow Archeops and Arctozolt are on the Shadow Log")
{
    EXPECT(WotShadowLog_SpeciesCounts(SPECIES_ARCHEOPS));
    EXPECT(WotShadowLog_SpeciesCounts(SPECIES_ARCTOZOLT));
}

TEST("(WoT) Prototype Island: the lab gate flag is a plain saved flag")
{
    EXPECT_GT(FLAG_WOT_PROTOTYPE_LAB_UNLOCKED, TEMP_FLAGS_END);
    EXPECT_LT(FLAG_WOT_PROTOTYPE_LAB_UNLOCKED, TRAINER_FLAGS_START);
    FlagClear(FLAG_WOT_PROTOTYPE_LAB_UNLOCKED);
    FlagSet(FLAG_WOT_PROTOTYPE_LAB_UNLOCKED);
    EXPECT(FlagGet(FLAG_WOT_PROTOTYPE_LAB_UNLOCKED));
    FlagClear(FLAG_WOT_PROTOTYPE_LAB_UNLOCKED);
}
