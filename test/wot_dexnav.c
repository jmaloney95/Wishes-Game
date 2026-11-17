#include "global.h"
#include "dexnav.h"
#include "pokedex.h"
#include "wild_encounter.h"
#include "test/test.h"
#include "constants/maps.h"

// WoT DexNav: search levels live in a compact SaveBlock3 array
// (one byte per wild species), and species with no National Dex number
// (Hellraiser, WOT_GHOST) are #0, which the dex flags can't hold.

TEST("(WoT) Pokedex flag #0 reads and writes nothing")
{
    u8 seen[sizeof(gSaveBlock1Ptr->dexSeen)];
    u8 caught[sizeof(gSaveBlock1Ptr->dexCaught)];

    memcpy(seen, gSaveBlock1Ptr->dexSeen, sizeof(seen));
    memcpy(caught, gSaveBlock1Ptr->dexCaught, sizeof(caught));
    GetSetPokedexFlag(NATIONAL_DEX_NONE, FLAG_SET_SEEN);
    GetSetPokedexFlag(NATIONAL_DEX_NONE, FLAG_SET_CAUGHT);
    EXPECT_EQ(GetSetPokedexFlag(NATIONAL_DEX_NONE, FLAG_GET_SEEN), 0);
    EXPECT_EQ(GetSetPokedexFlag(NATIONAL_DEX_NONE, FLAG_GET_CAUGHT), 0);
    EXPECT_EQ(memcmp(seen, gSaveBlock1Ptr->dexSeen, sizeof(seen)), 0);
    EXPECT_EQ(memcmp(caught, gSaveBlock1Ptr->dexCaught, sizeof(caught)), 0);
}

static u32 CountMissingSlots(const struct WildPokemonInfo *info, u32 count)
{
    u32 i, missing = 0;

    if (info == NULL)
        return 0;
    for (i = 0; i < count; i++)
    {
        u32 species = info->wildPokemon[i].species;
        if (species != SPECIES_NONE && !WotDexNavHasSlot(species))
        {
            DebugPrintf("no DexNav slot: species %d", species);
            missing++;
        }
    }
    return missing;
}

TEST("(WoT) Every wild species has a DexNav search-level slot")
{
    u32 i, time, missing = 0;

    for (i = 0; gWildMonHeaders[i].mapGroup != MAP_GROUP(MAP_UNDEFINED); i++)
    {
        for (time = 0; time < TIMES_OF_DAY_COUNT; time++)
        {
            const struct WildEncounterTypes *types = &gWildMonHeaders[i].encounterTypes[time];
            missing += CountMissingSlots(types->landMonsInfo, LAND_WILD_COUNT);
            missing += CountMissingSlots(types->waterMonsInfo, WATER_WILD_COUNT);
            missing += CountMissingSlots(types->rockSmashMonsInfo, ROCK_WILD_COUNT);
            missing += CountMissingSlots(types->fishingMonsInfo, FISH_WILD_COUNT);
            missing += CountMissingSlots(types->hiddenMonsInfo, HIDDEN_WILD_COUNT);
        }
    }
    // Fix: python3 tools/wot_dexnav_species.py
    EXPECT_EQ(missing, 0);
}

TEST("(WoT) DexNav search level counts up, caps, and keeps the seen bit")
{
    u32 i;

    memset(gSaveBlock3Ptr->wotDexNavSearchLevels, 0, sizeof(gSaveBlock3Ptr->wotDexNavSearchLevels));

    // A normal species: level only, "seen" comes from the dex flag.
    EXPECT_EQ(WotDexNavGetSearchLevel(SPECIES_HELLRAISER), 0);
    EXPECT_EQ(WotDexNavSeen(SPECIES_WOT_GHOST), FALSE);
    WotDexNavIncrementSearchLevel(SPECIES_DODRIO_DELTA);
    WotDexNavIncrementSearchLevel(SPECIES_DODRIO_DELTA);
    EXPECT_EQ(WotDexNavGetSearchLevel(SPECIES_DODRIO_DELTA), 2);
    WotDexNavMarkSeen(SPECIES_DODRIO_DELTA); // has a dex number: no-op
    EXPECT_EQ(WotDexNavGetSearchLevel(SPECIES_DODRIO_DELTA), 2);

    // A dex #0 species: seen bit and level share the byte.
    WotDexNavMarkSeen(SPECIES_WOT_GHOST);
    EXPECT_EQ(WotDexNavSeen(SPECIES_WOT_GHOST), TRUE);
    EXPECT_EQ(WotDexNavGetSearchLevel(SPECIES_WOT_GHOST), 0);
    for (i = 0; i < 300; i++)
        WotDexNavIncrementSearchLevel(SPECIES_WOT_GHOST);
    EXPECT_EQ(WotDexNavGetSearchLevel(SPECIES_WOT_GHOST), 127);
    EXPECT_EQ(WotDexNavSeen(SPECIES_WOT_GHOST), TRUE);

    // A species with no slot reads 0 and ignores writes.
    EXPECT_EQ(WotDexNavHasSlot(SPECIES_MEWTWO_MEGA_X), FALSE);
    WotDexNavIncrementSearchLevel(SPECIES_MEWTWO_MEGA_X);
    EXPECT_EQ(WotDexNavGetSearchLevel(SPECIES_MEWTWO_MEGA_X), 0);
}
