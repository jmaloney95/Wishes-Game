#include "global.h"
#include "main.h"
#include "pokemon.h"
#include "test/test.h"
#include "wot_shadow_art.h"

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

TEST("(WoT) Shadow Jirachi is created as a Shadow, plain Jirachi is not")
{
    struct Pokemon mon;

    CreateMon(&mon, SPECIES_JIRACHI_SHADOW, 70, 0x12345678, OTID_STRUCT_PRESET(0x87654321));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHADOW), TRUE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SANITY_IS_BAD_EGG), FALSE);

    CreateMon(&mon, SPECIES_JIRACHI, 70, 0x12345678, OTID_STRUCT_PRESET(0x87654321));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHADOW), FALSE);
}

// Taking over a snagged mon's OT must not re-roll its shininess
// (it is derived from the OT ID). Player OT 0 here; personality 0x12340000
// with OT 0x00001234 is shiny, and turns non-shiny under OT 0 if not kept.
TEST("(WoT) A shiny mon stays shiny when its OT changes on delivery")
{
    struct Pokemon mon;

    ZeroPlayerPartyMons();
    memset(gSaveBlock2Ptr->playerTrainerId, 0, TRAINER_ID_LENGTH);
    CreateMon(&mon, SPECIES_ABSOL, 30, 0x12340000, OTID_STRUCT_PRESET(0x00001234));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHINY), TRUE);
    GiveCapturedMonToPlayer(&mon);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_IS_SHINY), TRUE);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SANITY_IS_BAD_EGG), FALSE);
}

// ...and a normal one must not turn shiny: personality 0x12341234 is shiny
// under OT 0, not under OT 0xFFFF0000.
TEST("(WoT) A normal mon does not turn shiny when its OT changes on delivery")
{
    struct Pokemon mon;

    ZeroPlayerPartyMons();
    memset(gSaveBlock2Ptr->playerTrainerId, 0, TRAINER_ID_LENGTH);
    CreateMon(&mon, SPECIES_ABSOL, 30, 0x12341234, OTID_STRUCT_PRESET(0xFFFF0000));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHINY), FALSE);
    GiveCapturedMonToPlayer(&mon);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_IS_SHINY), FALSE);
}

TEST("(WoT) Every Shadow art has a shiny palette of its own")
{
    u32 i;

    for (i = 0; i < WotShadowPicCount(); i++)
    {
        const struct WotShadowPic *art = WotFindShadowPic(WotShadowPicSpeciesAt(i));

        EXPECT(art != NULL);
        EXPECT(art->shinyPal != NULL);
        EXPECT(memcmp(art->pal, art->shinyPal, PLTT_SIZE_4BPP) != 0);
        EXPECT(WotShadowPicPal(art, TRUE) == art->shinyPal);
        EXPECT(WotShadowPicPal(art, FALSE) == art->pal);
    }
}
