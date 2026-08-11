#include "global.h"
#include "main.h"
#include "pokemon.h"
#include "test/test.h"

// WoT Shadow system regression tests: a snagged trainer mon must survive the
// delivery write-sequence intact (playtest report: snagged Shadows arrived in
// the party as Bad Eggs).

TEST("(WoT) Snag delivery write-sequence does not Bad-Egg the mon")
{
    struct Pokemon mon;
    u32 one = TRUE;
    u32 hp = 0;

    // An NPC trainer's mon: fixed personality, an OT id that is NOT the player's.
    CreateMon(&mon, SPECIES_KROOKODILE, 41, 0x00C0FFEE, OTID_STRUCT_PRESET(0xDEADBEEF));
    SetMonData(&mon, MON_DATA_IS_SHADOW, &one);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHADOW), TRUE);

    // FinalizeCapture: HP zeroed so the faint machinery runs.
    SetMonData(&mon, MON_DATA_HP, &hp);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES_OR_EGG), SPECIES_KROOKODILE);
    // Delivery: healed, dex-flagged (no mon writes), OT rewritten, handed over.
    hp = GetMonData(&mon, MON_DATA_MAX_HP);
    SetMonData(&mon, MON_DATA_HP, &hp);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES_OR_EGG), SPECIES_KROOKODILE);
    SetMonData(&mon, MON_DATA_OT_NAME, gSaveBlock2Ptr->playerName);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES_OR_EGG), SPECIES_KROOKODILE);
    SetMonData(&mon, MON_DATA_OT_GENDER, &gSaveBlock2Ptr->playerGender);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES_OR_EGG), SPECIES_KROOKODILE);
    SetMonData(&mon, MON_DATA_OT_ID, gSaveBlock2Ptr->playerTrainerId);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES_OR_EGG), SPECIES_KROOKODILE);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_SANITY_IS_BAD_EGG), FALSE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHADOW), TRUE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP), GetMonData(&mon, MON_DATA_MAX_HP));
}

TEST("(WoT) Shadow bits round-trip and purification clears them")
{
    struct Pokemon mon;
    u32 one = TRUE, zero = FALSE;

    CreateMon(&mon, SPECIES_DRAPION, 42, 0x12345678, OTID_STRUCT_PRESET(0x87654321));
    SetMonData(&mon, MON_DATA_IS_SHADOW, &one);
    SetMonData(&mon, MON_DATA_SHADOW_OPENED, &one);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHADOW), TRUE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SHADOW_OPENED), TRUE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SANITY_IS_BAD_EGG), FALSE);

    SetMonData(&mon, MON_DATA_IS_SHADOW, &zero);
    SetMonData(&mon, MON_DATA_SHADOW_OPENED, &zero);
    SetMonData(&mon, MON_DATA_NATIONAL_RIBBON, &one);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHADOW), FALSE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_NATIONAL_RIBBON), TRUE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SANITY_IS_BAD_EGG), FALSE);
}
