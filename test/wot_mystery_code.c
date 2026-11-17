#include "global.h"
#include "caps.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "string_util.h"
#include "wot_mega_dimension.h"
#include "wot_mystery_code.h"
#include "wot_permanent_mega.h"
#include "wot_shadow_log.h"
#include "test/test.h"
#include "constants/characters.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/opponents.h"
#include "constants/region_map_sections.h"

// WoT Mystery Codes (src/wot_mystery_code.c): every card's code gives its
// Pokemon once per save, at the story's level cap, holding its item, with a
// real moveset. Test saves start with no milestones, so the cap is 15.

// Types text the way the naming screen leaves it in gStringVar2.
static void Type(const char *text)
{
    u32 i;

    for (i = 0; text[i] != '\0'; i++)
    {
        char c = text[i];

        if (c >= 'A' && c <= 'Z')
            gStringVar2[i] = CHAR_A + (c - 'A');
        else if (c >= 'a' && c <= 'z')
            gStringVar2[i] = CHAR_a + (c - 'a');
        else if (c >= '0' && c <= '9')
            gStringVar2[i] = CHAR_0 + (c - '0');
        else
            gStringVar2[i] = CHAR_SPACE;
    }
    gStringVar2[i] = EOS;
}

static const u8 *Typed(const char *text)
{
    Type(text);
    return gStringVar2;
}

// The runner clears the save blocks once per test, not once per PARAMETRIZE
// run, and never empties the PC. Start every run clean; tests that fill boxes
// empty them again at the end so later tests in the same runner see an empty PC.
static void ResetState(void)
{
    memset(gSaveBlock3Ptr->wotMysteryCodesUsed, 0, sizeof(gSaveBlock3Ptr->wotMysteryCodesUsed));
    FlagClear(TRAINER_FLAGS_START + TRAINER_ROXANNE_1);
    FlagClear(TRAINER_FLAGS_START + TRAINER_DRACO_LAB_ARMALDO);
    FlagClear(TRAINER_FLAGS_START + TRAINER_KATHLEEN);
    FlagClear(TRAINER_FLAGS_START + TRAINER_WALLY_VR_5);
    FlagClear(FLAG_WOT_LEVEL_CAPS_OFF);
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
}

// The moves the gift should know: the level-up set at that level, plus the
// card's extra move in a free slot or over the oldest move.
static void ExpectedMoves(const struct WotMysteryCodeGift *gift, u32 level, u16 *moves)
{
    struct Pokemon ref;
    u32 i;

    CreateMon(&ref, gift->species, level, 0, OTID_STRUCT_PLAYER_ID);
    GiveMonInitialMoveset(&ref);
    for (i = 0; i < MAX_MON_MOVES; i++)
        moves[i] = GetMonData(&ref, MON_DATA_MOVE1 + i);
    if (gift->extraMove == MOVE_NONE)
        return;
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (moves[i] == gift->extraMove)
            return;
    }
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (moves[i] == MOVE_NONE)
        {
            moves[i] = gift->extraMove;
            return;
        }
    }
    for (i = 0; i < MAX_MON_MOVES - 1; i++)
        moves[i] = moves[i + 1];
    moves[MAX_MON_MOVES - 1] = gift->extraMove;
}

// In the party a gift is its card's Mega; in the PC, its usual form.
static void ExpectGiftMon(struct Pokemon *mon, u32 index, u32 level, bool32 inParty)
{
    const struct WotMysteryCodeGift *gift = WotMysteryCode_Get(index);
    u16 moves[MAX_MON_MOVES];
    u32 i;

    EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), inParty ? WotMysteryCode_MegaSpecies(gift) : gift->species);
    if (inParty && gift->item != ITEM_NONE)
        EXPECT(WotIsKeptMega(GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HELD_ITEM)));
    EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), gift->item);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), level);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP));
    EXPECT_NE(GetMonData(mon, MON_DATA_HP), 0);
    ExpectedMoves(gift, level, moves);
    EXPECT_NE(moves[0], MOVE_NONE);
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        EXPECT_EQ(GetMonData(mon, MON_DATA_MOVE1 + i), moves[i]);
        if (moves[i] != MOVE_NONE)
            EXPECT_NE(GetMonData(mon, MON_DATA_PP1 + i), 0);
    }
    if (gift->shiny)
        EXPECT_EQ(GetMonData(mon, MON_DATA_IS_SHINY), TRUE);
    EXPECT_EQ(GetMonData(mon, MON_DATA_IS_SHADOW), gift->shadow);
    EXPECT_EQ(GetMonData(mon, MON_DATA_MET_LOCATION), METLOC_FATEFUL_ENCOUNTER);
    EXPECT_EQ(GetMonData(mon, MON_DATA_MODERN_FATEFUL_ENCOUNTER), TRUE);
}

TEST("(WoT) Mystery Code: the table has the 30 Mega cards, each code eight capitals")
{
    u32 i, j;

    EXPECT_EQ(WotMysteryCode_Count(), 30);
    for (i = 0; i < WotMysteryCode_Count(); i++)
    {
        const struct WotMysteryCodeGift *gift = WotMysteryCode_Get(i);

        for (j = 0; j < WOT_MYSTERY_CODE_LENGTH; j++)
            EXPECT(gift->code[j] >= 'A' && gift->code[j] <= 'Z' && gift->code[j] != 'I' && gift->code[j] != 'O');
        EXPECT_EQ(gift->code[WOT_MYSTERY_CODE_LENGTH], '\0');
        EXPECT_NE(gift->species, SPECIES_NONE);
        // Find returns the first match, so this also rules out duplicates.
        EXPECT_EQ(WotMysteryCode_Find(Typed(gift->code)), (s32)i);
    }
    // The two special cards.
    EXPECT_EQ(WotMysteryCode_Get(28)->species, SPECIES_RAYQUAZA);
    EXPECT_EQ(WotMysteryCode_Get(28)->shiny, TRUE);
    EXPECT_EQ(WotMysteryCode_Get(28)->extraMove, MOVE_DRAGON_ASCENT);
    EXPECT_EQ(WotMysteryCode_Get(29)->species, SPECIES_HO_OH);
    EXPECT_EQ(WotMysteryCode_Get(29)->item, ITEM_HO_OHNITE);
    EXPECT_EQ(WotMysteryCode_Get(29)->shadow, TRUE);
    // Every card has a Mega to give and to show.
    for (i = 0; i < WotMysteryCode_Count(); i++)
    {
        u16 mega = WotMysteryCode_MegaSpecies(WotMysteryCode_Get(i));

        EXPECT_NE(mega, SPECIES_NONE);
        EXPECT(gSpeciesInfo[mega].isMegaEvolution);
        EXPECT_EQ(WotMysteryCode_PortraitSpecies(WotMysteryCode_Get(i)), mega);
    }
    EXPECT_EQ(WotMysteryCode_MegaSpecies(WotMysteryCode_Get(12)), SPECIES_LUGIA_MEGA);
    EXPECT_EQ(WotMysteryCode_MegaSpecies(WotMysteryCode_Get(28)), SPECIES_RAYQUAZA_MEGA);
    EXPECT_EQ(WotMysteryCode_MegaSpecies(WotMysteryCode_Get(29)), SPECIES_HO_OH_MEGA);

    // "Epic pull!": the site's rare finishes (Rainbow swirl, Gold, Cosmos)
    // and the two secret rares; 12 of the 30.
    j = 0;
    for (i = 0; i < WotMysteryCode_Count(); i++)
        j += WotMysteryCode_Get(i)->epic;
    EXPECT_EQ(j, 12);
    EXPECT_EQ(WotMysteryCode_Get(0)->epic, TRUE);   // Butterfree, Rainbow swirl
    EXPECT_EQ(WotMysteryCode_Get(6)->epic, TRUE);   // Snorlax, Gold
    EXPECT_EQ(WotMysteryCode_Get(21)->epic, TRUE);  // Darkrai, Cosmos
    EXPECT_EQ(WotMysteryCode_Get(28)->epic, TRUE);  // Rayquaza, secret
    EXPECT_EQ(WotMysteryCode_Get(29)->epic, TRUE);  // Shadow Ho-Oh, secret
    EXPECT_EQ(WotMysteryCode_Get(1)->epic, FALSE);  // Dugtrio, B/W vortex
    EXPECT_EQ(WotMysteryCode_Get(7)->epic, FALSE);  // Meganium, Emerald
}

TEST("(WoT) Mystery Code: each card's code gives its Pokemon exactly once")
{
    u32 i = 0, j, k;
    const struct WotMysteryCodeGift *gift;

    for (j = 0; j < 30; j++)
        PARAMETRIZE { i = j; }

    ResetState();
    gift = WotMysteryCode_Get(i);
    Type(gift->code);
    WotMysteryCode_Check();
    EXPECT_EQ(gSpecialVar_Result, WOT_MYSTERY_CODE_OK);
    EXPECT_EQ(gSpecialVar_0x8005, i);
    EXPECT_EQ(gSpecialVar_0x8006, gift->epic);
    EXPECT_EQ(StringCompare(gStringVar1, gift->name), 0);
    // The portrait: the card's Mega, shiny when the card is.
    EXPECT_EQ(gSpecialVar_0x8004, WotMysteryCode_MegaSpecies(gift));
    EXPECT_EQ(gSpecialVar_0x8007, gift->shiny);

    WotMysteryCode_Give();
    EXPECT_EQ(gSpecialVar_Result, MON_GIVEN_TO_PARTY);
    EXPECT_EQ(StringCompare(gStringVar1, GetSpeciesName(WotMysteryCode_MegaSpecies(gift))), 0);
    EXPECT_EQ(VarGet(VAR_TEMP_TRANSFERRED_SPECIES), WotMysteryCode_MegaSpecies(gift));
    ExpectGiftMon(&gPlayerParty[0], i, 15, TRUE);
    for (k = 0; k < WotMysteryCode_Count(); k++)
        EXPECT_EQ(WotMysteryCode_IsUsed(k), k == i);

    // Second try: refused, nothing more given.
    Type(gift->code);
    WotMysteryCode_Check();
    EXPECT_EQ(gSpecialVar_Result, WOT_MYSTERY_CODE_USED);
    EXPECT_EQ(StringCompare(gStringVar1, gift->name), 0);
    EXPECT_EQ(WotMysteryCode_GiveGift(i), MON_CANT_GIVE);
    EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_SPECIES), SPECIES_NONE);
}

TEST("(WoT) Mystery Code: shiny Rayquaza knows Dragon Ascent at every cap")
{
    u32 level = 0, i;
    bool32 knows;

    PARAMETRIZE { level = 15; }
    PARAMETRIZE { level = 21; }
    PARAMETRIZE { level = 42; }
    PARAMETRIZE { level = 58; }
    PARAMETRIZE { level = 100; }

    ResetState();
    // Milestones: Red Fatality 21, Draco at Munen Lab 42, the Tower Badge 58,
    // the Oni (after the credits) 100.
    if (level >= 21)
        FlagSet(TRAINER_FLAGS_START + TRAINER_ROXANNE_1);
    if (level >= 42)
        FlagSet(TRAINER_FLAGS_START + TRAINER_DRACO_LAB_ARMALDO);
    if (level >= 58)
        FlagSet(TRAINER_FLAGS_START + TRAINER_KATHLEEN);
    if (level >= 100)
        FlagSet(TRAINER_FLAGS_START + TRAINER_WALLY_VR_5);
    EXPECT_EQ(WotMysteryCode_GiftLevel(), level);

    EXPECT_EQ(WotMysteryCode_GiveGift(28), MON_GIVEN_TO_PARTY);
    ExpectGiftMon(&gPlayerParty[0], 28, level, TRUE);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_IS_SHINY), TRUE);
    knows = FALSE;
    for (i = 0; i < MAX_MON_MOVES; i++)
        knows |= (GetMonData(&gPlayerParty[0], MON_DATA_MOVE1 + i) == MOVE_DRAGON_ASCENT);
    EXPECT(knows);
}

TEST("(WoT) Mystery Code: caps on, the story's cap; caps off, the strongest party Pokemon's level")
{
    u32 one = TRUE;

    ResetState();
    FlagSet(TRAINER_FLAGS_START + TRAINER_ROXANNE_1);
    CreateMon(&gPlayerParty[0], SPECIES_ZIGZAGOON, 12, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gPlayerParty[1], SPECIES_WURMPLE, 37, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gPlayerParty[2], SPECIES_TORCHIC, 60, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gPlayerParty[2], MON_DATA_IS_EGG, &one); // an egg doesn't count
    CalculatePlayerPartyCount();

    // Caps on: the cap, whatever the party.
    EXPECT_EQ(WotMysteryCode_GiftLevel(), 21);

    // Caps off: the strongest Pokemon in the party.
    FlagSet(FLAG_WOT_LEVEL_CAPS_OFF);
    EXPECT_EQ(WotMysteryCode_GiftLevel(), 37);
    EXPECT_EQ(WotMysteryCode_GiveGift(12), MON_GIVEN_TO_PARTY);
    ExpectGiftMon(&gPlayerParty[3], 12, 37, TRUE);

    // Up to 100.
    CreateMon(&gPlayerParty[4], SPECIES_ZIGZAGOON, 100, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    EXPECT_EQ(WotMysteryCode_GiftLevel(), 100);

    // Only Lv 1s: 1.
    ZeroPlayerPartyMons();
    CreateMon(&gPlayerParty[0], SPECIES_ZIGZAGOON, 1, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    EXPECT_EQ(WotMysteryCode_GiftLevel(), 1);
    EXPECT_EQ(WotMysteryCode_GiveGift(0), MON_GIVEN_TO_PARTY);
    ExpectGiftMon(&gPlayerParty[1], 0, 1, TRUE);
}

TEST("(WoT) Mystery Code: lowercase and spaces are fine; wrong, short and empty input are not")
{
    s32 index;

    ResetState();
    // Card 1 is FRRKESMV.
    EXPECT_EQ(WotMysteryCode_CheckInput(Typed("frrkesmv"), &index), WOT_MYSTERY_CODE_OK);
    EXPECT_EQ(index, 0);
    EXPECT_EQ(WotMysteryCode_CheckInput(Typed("FrRkEsMv"), &index), WOT_MYSTERY_CODE_OK);
    EXPECT_EQ(WotMysteryCode_CheckInput(Typed("FRRK ESMV"), &index), WOT_MYSTERY_CODE_OK);
    EXPECT_EQ(WotMysteryCode_CheckInput(Typed("FRRKESMX"), &index), WOT_MYSTERY_CODE_NO_MATCH);
    EXPECT_EQ(index, -1);
    EXPECT_EQ(WotMysteryCode_CheckInput(Typed("FRRKESM"), &index), WOT_MYSTERY_CODE_NO_MATCH);
    EXPECT_EQ(WotMysteryCode_CheckInput(Typed("FRRKESMV1"), &index), WOT_MYSTERY_CODE_NO_MATCH);
    EXPECT_EQ(WotMysteryCode_CheckInput(Typed("PIKACHU1"), &index), WOT_MYSTERY_CODE_NO_MATCH);
    EXPECT_EQ(WotMysteryCode_CheckInput(Typed(""), &index), WOT_MYSTERY_CODE_EMPTY);
    EXPECT_EQ(WotMysteryCode_CheckInput(Typed("   "), &index), WOT_MYSTERY_CODE_EMPTY);

    // The script's view of a wrong code: refused, nothing given or marked.
    ZeroPlayerPartyMons();
    Type("ABCDEFGH");
    WotMysteryCode_Check();
    EXPECT_EQ(gSpecialVar_Result, WOT_MYSTERY_CODE_NO_MATCH);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), SPECIES_NONE);

    // A lowercase code gives the gift.
    Type("ynhpukwy"); // card 13, Mega Lugia
    WotMysteryCode_Check();
    EXPECT_EQ(gSpecialVar_Result, WOT_MYSTERY_CODE_OK);
    EXPECT_EQ(gSpecialVar_0x8005, 12);
    WotMysteryCode_Give();
    EXPECT_EQ(gSpecialVar_Result, MON_GIVEN_TO_PARTY);
    ExpectGiftMon(&gPlayerParty[0], 12, 15, TRUE);
    Type("YNHPUKWY");
    WotMysteryCode_Check();
    EXPECT_EQ(gSpecialVar_Result, WOT_MYSTERY_CODE_USED);
}

TEST("(WoT) Mystery Code: all 30 in one save; a full party sends the rest to the PC; no flag changes")
{
    static u8 flagsBefore[NUM_FLAG_BYTES];
    u32 i;

    ResetState();
    memcpy(flagsBefore, gSaveBlock1Ptr->flags, sizeof(flagsBefore));
    for (i = 0; i < WotMysteryCode_Count(); i++)
    {
        Type(WotMysteryCode_Get(i)->code);
        WotMysteryCode_Check();
        EXPECT_EQ(gSpecialVar_Result, WOT_MYSTERY_CODE_OK);
        WotMysteryCode_Give();
        EXPECT_EQ(gSpecialVar_Result, i < PARTY_SIZE ? MON_GIVEN_TO_PARTY : MON_GIVEN_TO_PC);
    }
    for (i = 0; i < WotMysteryCode_Count(); i++)
    {
        EXPECT(WotMysteryCode_IsUsed(i));
        Type(WotMysteryCode_Get(i)->code);
        WotMysteryCode_Check();
        EXPECT_EQ(gSpecialVar_Result, WOT_MYSTERY_CODE_USED);
    }
    for (i = 0; i < PARTY_SIZE; i++)
        ExpectGiftMon(&gPlayerParty[i], i, 15, TRUE);
    for (i = PARTY_SIZE; i < WotMysteryCode_Count(); i++)
    {
        struct Pokemon mon;

        BoxMonToMon(GetBoxedMonPtr(0, i - PARTY_SIZE), &mon);
        ExpectGiftMon(&mon, i, 15, FALSE);
    }
    // No Mega Dimension claim, tally, quest or caught flag moved
    // (FLAG_CAUGHT_LUGIA / FLAG_CAUGHT_HO_OH are the rift claims).
    EXPECT_EQ(memcmp(flagsBefore, gSaveBlock1Ptr->flags, sizeof(flagsBefore)), 0);
    EXPECT(!FlagGet(FLAG_CAUGHT_LUGIA));
    EXPECT(!FlagGet(FLAG_CAUGHT_HO_OH));
    ResetPokemonStorageSystem();
}

TEST("(WoT) Mystery Code: with the party and every box full, nothing is given and the code stays unused")
{
    struct Pokemon filler;
    u32 box, pos;

    ResetState();
    CreateMon(&filler, SPECIES_ZIGZAGOON, 5, 0, OTID_STRUCT_PLAYER_ID);
    for (pos = 0; pos < PARTY_SIZE; pos++)
        gPlayerParty[pos] = filler;
    CalculatePlayerPartyCount();
    for (box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        for (pos = 0; pos < IN_BOX_COUNT; pos++)
            SetBoxMonAt(box, pos, &filler.box);
    }

    Type("FRRKESMV");
    WotMysteryCode_Check();
    EXPECT_EQ(gSpecialVar_Result, WOT_MYSTERY_CODE_OK);
    WotMysteryCode_Give();
    EXPECT_EQ(gSpecialVar_Result, MON_CANT_GIVE);
    EXPECT(!WotMysteryCode_IsUsed(0));
    Type("FRRKESMV");
    WotMysteryCode_Check();
    EXPECT_EQ(gSpecialVar_Result, WOT_MYSTERY_CODE_OK);

    // Room in the last box: now it goes through.
    ZeroBoxMonAt(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1);
    WotMysteryCode_Give();
    EXPECT_EQ(gSpecialVar_Result, MON_GIVEN_TO_PC);
    EXPECT(WotMysteryCode_IsUsed(0));
    EXPECT_EQ(GetBoxMonDataAt(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1, MON_DATA_SPECIES), SPECIES_BUTTERFREE);
    EXPECT_EQ(WotMysteryCode_GivenSpecies(), SPECIES_BUTTERFREE);
    EXPECT_EQ(VarGet(VAR_TEMP_TRANSFERRED_SPECIES), SPECIES_BUTTERFREE);
    ResetPokemonStorageSystem();
}

TEST("(WoT) Mystery Code: the gifted Shadow Ho-Oh fills the Shadow Log's Ho-Oh row; plain gifts don't")
{
    u32 snagged;

    ResetState();
    snagged = WotShadowLog_CountSnagged();

    // A plain gift of a species on the log leaves its row alone.
    EXPECT_EQ(WotMysteryCode_GiveGift(12), MON_GIVEN_TO_PARTY); // Lugia
    EXPECT(!WotShadowLog_IsSpeciesSnagged(SPECIES_LUGIA));
    EXPECT(!WotShadowLog_IsSpeciesSnagged(SPECIES_HO_OH));

    // The Shadow Ho-Oh fills its row at once, like a snag.
    EXPECT_EQ(WotMysteryCode_GiveGift(29), MON_GIVEN_TO_PARTY);
    EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_IS_SHADOW), TRUE);
    EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_SPECIES), SPECIES_HO_OH_MEGA);
    EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_MODERN_FATEFUL_ENCOUNTER), TRUE);
    EXPECT(WotShadowLog_IsSpeciesSnagged(SPECIES_HO_OH));
    EXPECT_EQ(WotShadowLog_CountSnagged(), snagged + 1);
    EXPECT(!WotShadowLog_IsSpeciesSnagged(SPECIES_LUGIA));

    // The Continue backfill agrees, from the party or a box.
    WotShadowLog_Backfill();
    EXPECT_EQ(WotShadowLog_CountSnagged(), snagged + 1);
    CopyMonToPC(&gPlayerParty[1]);
    ZeroPlayerPartyMons();
    WotShadowLog_Backfill();
    EXPECT_EQ(WotShadowLog_CountSnagged(), snagged + 1);
    ResetPokemonStorageSystem();
}

TEST("(WoT) Mystery Code: a redeemed card's Mega Log entry shows as obtained, with no claim set")
{
    struct WotMegaLogEntry entry;
    u32 i, shown;

    ResetState();
    for (i = 0; i < WotMegaLog_RogueCount() + WotMegaLog_LegendCount(); i++)
    {
        WotMegaLog_GetEntry(i, &entry);
        EXPECT(!entry.claimed);
    }

    EXPECT_EQ(WotMysteryCode_GiveGift(0), MON_GIVEN_TO_PARTY);  // Butterfree, a rogue Mega
    EXPECT_EQ(WotMysteryCode_GiveGift(12), MON_GIVEN_TO_PARTY); // Lugia, a legend
    EXPECT_EQ(WotMysteryCode_GiveGift(28), MON_GIVEN_TO_PARTY); // Rayquaza, a legend
    EXPECT_EQ(WotMysteryCode_GiveGift(29), MON_GIVEN_TO_PARTY); // Shadow Ho-Oh: the Ho-Oh legend
    EXPECT_EQ(WotMysteryCode_GiveGift(18), MON_GIVEN_TO_PARTY); // Deoxys: not in the log

    shown = 0;
    for (i = 0; i < WotMegaLog_RogueCount() + WotMegaLog_LegendCount(); i++)
    {
        bool32 gifted;

        WotMegaLog_GetEntry(i, &entry);
        gifted = entry.iconSpecies == SPECIES_BUTTERFREE || entry.iconSpecies == SPECIES_LUGIA
              || entry.iconSpecies == SPECIES_RAYQUAZA || entry.iconSpecies == SPECIES_HO_OH;
        EXPECT_EQ(entry.claimed, gifted);
        shown += entry.claimed;
    }
    EXPECT_EQ(shown, 4);

    // Only the log counts it: no claim, tally or caught flag moved.
    EXPECT_EQ(WotMegaDim_CountClaimed(), 0);
    EXPECT(!FlagGet(FLAG_CAUGHT_LUGIA));
    EXPECT(!FlagGet(FLAG_CAUGHT_HO_OH));
}
