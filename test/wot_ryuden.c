#include "global.h"
#include "event_data.h"
#include "main.h"
#include "pokemon.h"
#include "script_pokemon_util.h"
#include "wot_ryuden.h"
#include "test/test.h"

// Ryuden Island incubator: players reported a Bad Egg from it.
// Every egg in the pool must be a real, non-legendary species that survives
// the same give path the script uses (WotGiveRyudenEgg -> ScriptGiveEgg ->
// CreateEgg + GiveCapturedMonToPlayer) as a sane egg of that species.

TEST("(WoT) Ryuden egg pool holds only valid, non-legendary species")
{
    u32 i;

    EXPECT_GT(gWotRyudenEggPoolCount, 0);
    for (i = 0; i < gWotRyudenEggPoolCount; i++)
    {
        u32 species = gWotRyudenEggPool[i];

        EXPECT_GT(species, SPECIES_NONE);
        EXPECT_LT(species, NUM_SPECIES);
        EXPECT(IsSpeciesEnabled(species));
        EXPECT(!gSpeciesInfo[species].isRestrictedLegendary);
        EXPECT(!gSpeciesInfo[species].isSubLegendary);
        EXPECT(!gSpeciesInfo[species].isMythical);
        EXPECT(!gSpeciesInfo[species].isUltraBeast);
        EXPECT(!gSpeciesInfo[species].isParadox);
        EXPECT(!gSpeciesInfo[species].isFrontierBanned);
        EXPECT(gSpeciesInfo[species].eggCycles != 0);
    }
}

TEST("(WoT) Every Ryuden egg arrives as a sane egg of its species")
{
    u32 i, roll;

    for (i = 0; i < gWotRyudenEggPoolCount; i++)
    {
        // several rolls each: personality (and so the substruct order) is random
        for (roll = 0; roll < 8; roll++)
        {
            ZeroPlayerPartyMons();
            EXPECT_EQ(ScriptGiveEgg(gWotRyudenEggPool[i]), MON_GIVEN_TO_PARTY);
            EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SANITY_IS_BAD_EGG), FALSE);
            EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_IS_EGG), TRUE);
            EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), gWotRyudenEggPool[i]);
            EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES_OR_EGG), SPECIES_EGG);
        }
    }
}

TEST("(WoT) The Ryuden special gives a pool egg and reports where it went")
{
    u32 i;
    bool32 inPool = FALSE;

    ZeroPlayerPartyMons();
    WotGiveRyudenEgg();
    EXPECT_EQ(gSpecialVar_Result, MON_GIVEN_TO_PARTY);
    for (i = 0; i < gWotRyudenEggPoolCount; i++)
    {
        if (gWotRyudenEggPool[i] == gSpecialVar_0x8004)
            inPool = TRUE;
    }
    EXPECT(inPool);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), gSpecialVar_0x8004);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SANITY_IS_BAD_EGG), FALSE);
}
