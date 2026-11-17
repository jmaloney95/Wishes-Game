#include "global.h"
#include "pokemon.h"
#include "event_object_movement.h"
#include "wot_surf_mon.h"
#include "test/test.h"
#include "constants/species.h"

// WoT surf Pokemon: who the player rides while surfing.

static void Party(u16 a, u16 b, u16 c)
{
    ZeroPlayerPartyMons();
    if (a != SPECIES_NONE)
        CreateMon(&gPlayerParty[0], a, 30, 0, OTID_STRUCT_PLAYER_ID);
    if (b != SPECIES_NONE)
        CreateMon(&gPlayerParty[1], b, 30, 0, OTID_STRUCT_PLAYER_ID);
    if (c != SPECIES_NONE)
        CreateMon(&gPlayerParty[2], c, 30, 0, OTID_STRUCT_PLAYER_ID);
    // CreateMon leaves HP (and max HP) at 0 here; give them some.
    for (u32 i = 0; i < 3; i++)
    {
        u32 hp = 50;
        SetMonData(&gPlayerParty[i], MON_DATA_HP, &hp);
    }
}

TEST("(WoT) Surf mon: the lead carries the player when it has a surf sprite")
{
    Party(SPECIES_LAPRAS, SPECIES_GYARADOS, SPECIES_NONE);
    EXPECT_EQ(WotGetSurfMon(), &gPlayerParty[0]);
}

TEST("(WoT) Surf mon: a lead without one passes to the first party Pokemon with one")
{
    Party(SPECIES_PIDGEY, SPECIES_RATTATA, SPECIES_GYARADOS);
    EXPECT_EQ(WotGetSurfMon(), &gPlayerParty[2]);
}

TEST("(WoT) Surf mon: no surf sprite in the party means the blob")
{
    Party(SPECIES_PIDGEY, SPECIES_RATTATA, SPECIES_NONE);
    EXPECT(WotGetSurfMon() == NULL);
}

TEST("(WoT) Surf mon: a fainted Pokemon or an egg doesn't carry the player")
{
    u32 hp = 0;
    bool32 egg = TRUE;

    Party(SPECIES_LAPRAS, SPECIES_SQUIRTLE, SPECIES_GYARADOS);
    SetMonData(&gPlayerParty[0], MON_DATA_HP, &hp);
    SetMonData(&gPlayerParty[1], MON_DATA_IS_EGG, &egg);
    EXPECT_EQ(WotGetSurfMon(), &gPlayerParty[2]);
}

TEST("(WoT) Surf mon: a kept Mega rides as its base form's sprite")
{
    EXPECT(WotHasSurfSprite(SPECIES_SWAMPERT_MEGA));
    EXPECT(WotHasSurfSprite(SPECIES_GYARADOS_MEGA));
    EXPECT(!WotHasSurfSprite(SPECIES_PIDGEY));
}


