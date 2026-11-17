#include "global.h"
#include "caps.h"
#include "event_data.h"
#include "main.h"
#include "naming_screen.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "string_util.h"
#include "wot_mystery_code.h"
#include "wot_shadow_log.h"
#include "constants/characters.h"
#include "constants/flags.h"
#include "constants/form_change_types.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/region_map_sections.h"
#include "constants/species.h"
#include "constants/vars.h"

// ---------------------------------------------------------------------------
// Mystery Codes.
//
// Each Mega card carries one fixed eight-letter code (the same for everyone).
// The code NPC (data/scripts/wot_mystery_code.inc) opens a naming screen, and
// a code that matches gives that card's Pokemon:
//  - the card's species at the story's level cap, holding the card's item;
//  - its level-up moves (CreateMon gives none), plus the card's extra move;
//  - shiny or Shadow when the card says so;
//  - already Mega Evolved, as the card shows it. Into the party it comes as the
//    Mega, which then behaves like any Mega that stayed Mega after a battle
//    (wot_permanent_mega.c): it keeps the form while it holds its stone. Sent to
//    the PC it comes in its usual form, since a Mega changes back in the PC.
//    Mega Rayquaza holds no stone, so it changes back after its first battle.
// While the man accepts the code, a portrait of the Mega is on screen (with the
// shiny sparkle for a shiny card).
// Each code works once per save: card n is bit n - 1 of
// gSaveBlock3Ptr->wotMysteryCodesUsed, set only once the Pokemon is given.
//
// A gift is marked as a fateful encounter (met location and the modern flag,
// as Mystery Gift Pokemon are). It counts in the logs: a Shadow gift fills its
// Shadow Log row like a snagged one, and the Mega Log shows a redeemed card's
// entry as obtained (WotMysteryCode_IsSpeciesRedeemed). Nothing here touches
// the Mega Dimension's claims, Mable's tally or any quest flag.
// ---------------------------------------------------------------------------

#include "data/wot_mystery_codes.h"

STATIC_ASSERT(WOT_MYSTERY_CODE_COUNT <= 8 * WOT_MYSTERY_CODE_BYTES, WotMysteryCodeBitsFit);

u32 WotMysteryCode_Count(void)
{
    return WOT_MYSTERY_CODE_COUNT;
}

const struct WotMysteryCodeGift *WotMysteryCode_Get(u32 index)
{
    return index < WOT_MYSTERY_CODE_COUNT ? &sWotMysteryCodes[index] : NULL;
}

bool32 WotMysteryCode_IsUsed(u32 index)
{
    if (index >= WOT_MYSTERY_CODE_COUNT)
        return FALSE;
    return (gSaveBlock3Ptr->wotMysteryCodesUsed[index / 8] >> (index % 8)) & 1;
}

static void SetUsed(u32 index)
{
    gSaveBlock3Ptr->wotMysteryCodesUsed[index / 8] |= 1 << (index % 8);
}

// Reads typed text as a code: letters in either case, spaces ignored.
// FALSE for anything else, or the wrong length.
static bool32 ReadCode(const u8 *input, char *out)
{
    u32 n = 0;

    for (; *input != EOS; input++)
    {
        u32 c = *input;

        if (c == CHAR_SPACE)
            continue;
        if (c >= CHAR_a && c <= CHAR_z)
            c = c - CHAR_a + CHAR_A;
        if (c < CHAR_A || c > CHAR_Z || n >= WOT_MYSTERY_CODE_LENGTH)
            return FALSE;
        out[n++] = 'A' + (c - CHAR_A);
    }
    out[n] = '\0';
    return n == WOT_MYSTERY_CODE_LENGTH;
}

// The code's index, or -1.
s32 WotMysteryCode_Find(const u8 *input)
{
    char code[WOT_MYSTERY_CODE_LENGTH + 1];
    u32 i, j;

    if (!ReadCode(input, code))
        return -1;
    for (i = 0; i < WOT_MYSTERY_CODE_COUNT; i++)
    {
        for (j = 0; j < WOT_MYSTERY_CODE_LENGTH; j++)
        {
            if (sWotMysteryCodes[i].code[j] != code[j])
                break;
        }
        if (j == WOT_MYSTERY_CODE_LENGTH)
            return i;
    }
    return -1;
}

// A WOT_MYSTERY_CODE_* answer; *index is the code's index, or -1.
u32 WotMysteryCode_CheckInput(const u8 *input, s32 *index)
{
    const u8 *p = input;

    *index = -1;
    while (*p == CHAR_SPACE)
        p++;
    if (*p == EOS)
        return WOT_MYSTERY_CODE_EMPTY;
    *index = WotMysteryCode_Find(input);
    if (*index < 0)
        return WOT_MYSTERY_CODE_NO_MATCH;
    if (WotMysteryCode_IsUsed(*index))
        return WOT_MYSTERY_CODE_USED;
    return WOT_MYSTERY_CODE_OK;
}

// With level caps on, the story's cap (MAX_LEVEL after the credits). With caps
// switched off in Options, the level of the strongest Pokemon in the party.
u32 WotMysteryCode_GiftLevel(void)
{
    u32 i, level, best = 0;

    if (!FlagGet(FLAG_WOT_LEVEL_CAPS_OFF))
        return WotGetMilestoneLevelCap();
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) == SPECIES_NONE
         || GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
            continue;
        level = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
        if (level > best)
            best = level;
    }
    if (best == 0)
        return WotGetMilestoneLevelCap();
    return min(best, (u32)MAX_LEVEL);
}

// Whether a redeemed code gave this species (the Mega Log counts it).
bool32 WotMysteryCode_IsSpeciesRedeemed(u16 species)
{
    u32 i;

    for (i = 0; i < WOT_MYSTERY_CODE_COUNT; i++)
    {
        if (sWotMysteryCodes[i].species == species && WotMysteryCode_IsUsed(i))
            return TRUE;
    }
    return FALSE;
}

bool32 WotMysteryCode_IsGiftBoxMon(struct BoxPokemon *boxMon)
{
    return GetBoxMonData(boxMon, MON_DATA_MODERN_FATEFUL_ENCOUNTER) != 0;
}

// The Mega a card shows: the base species' Mega Evolution entry for the card's
// stone, or for its move (Rayquaza and Dragon Ascent). SPECIES_NONE if none.
u16 WotMysteryCode_MegaSpecies(const struct WotMysteryCodeGift *gift)
{
    const struct FormChange *formChanges = GetSpeciesFormChanges(gift->species);
    u32 i;

    if (formChanges == NULL)
        return SPECIES_NONE;
    for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM
         && gift->item != ITEM_NONE && formChanges[i].param1 == gift->item)
            return formChanges[i].targetSpecies;
        if (formChanges[i].method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE
         && gift->extraMove != MOVE_NONE && formChanges[i].param1 == gift->extraMove)
            return formChanges[i].targetSpecies;
    }
    return SPECIES_NONE;
}

// The species the portrait shows: the Mega, else the card's species.
u16 WotMysteryCode_PortraitSpecies(const struct WotMysteryCodeGift *gift)
{
    u16 mega = WotMysteryCode_MegaSpecies(gift);

    return mega != SPECIES_NONE ? mega : gift->species;
}

// Changes the gift's species in place (form change): moves, nature, IVs,
// ability slot and the rest stay as they are, and the stats follow the form.
static void SetGiftSpecies(struct Pokemon *mon, u16 species)
{
    SetMonData(mon, MON_DATA_SPECIES, &species);
    CalculateMonStats(mon);
}

static void CreateGiftMon(struct Pokemon *mon, const struct WotMysteryCodeGift *gift)
{
    u32 value;
    u16 item = gift->item;

    CreateMonWithIVs(mon, gift->species, WotMysteryCode_GiftLevel(), Random32(), OTID_STRUCT_PLAYER_ID, USE_RANDOM_IVS);
    GiveMonInitialMoveset(mon);
    // Learned like a level-up move: into a free slot, else over the oldest.
    if (gift->extraMove != MOVE_NONE && GiveMoveToMon(mon, gift->extraMove) == MON_HAS_MAX_MOVES)
        DeleteFirstMoveAndGiveMoveToMon(mon, gift->extraMove);
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    if (gift->shiny)
    {
        value = TRUE;
        SetMonData(mon, MON_DATA_IS_SHINY, &value);
    }
    if (gift->shadow)
    {
        value = TRUE;
        SetMonData(mon, MON_DATA_IS_SHADOW, &value);
    }
    value = METLOC_FATEFUL_ENCOUNTER;
    SetMonData(mon, MON_DATA_MET_LOCATION, &value);
    value = TRUE;
    SetMonData(mon, MON_DATA_MODERN_FATEFUL_ENCOUNTER, &value);
    CalculateMonStats(mon);
}

static u16 sGivenSpecies;

// MON_GIVEN_TO_PARTY / MON_GIVEN_TO_PC, or MON_CANT_GIVE (party and boxes full,
// or the code is already used), in which case the code stays unused.
u32 WotMysteryCode_GiveGift(u32 index)
{
    struct Pokemon mon;
    u32 result;
    u16 mega;

    sGivenSpecies = SPECIES_NONE;
    if (index >= WOT_MYSTERY_CODE_COUNT || WotMysteryCode_IsUsed(index))
        return MON_CANT_GIVE;
    CreateGiftMon(&mon, &sWotMysteryCodes[index]);
    // The moves above came from the base species' level-up list; now the form.
    // A full party sends it to the PC, where a Mega is always in its usual form.
    mega = WotMysteryCode_MegaSpecies(&sWotMysteryCodes[index]);
    if (mega != SPECIES_NONE && CalculatePlayerPartyCount() < PARTY_SIZE)
        SetGiftSpecies(&mon, mega);
    result = GiveScriptedMonToPlayer(&mon, PARTY_SIZE);
    if (result != MON_CANT_GIVE)
    {
        SetUsed(index);
        sGivenSpecies = GetMonData(&mon, MON_DATA_SPECIES);
        WotShadowLog_MarkMon(&mon); // a Shadow gift fills its row now, not at the next Continue
    }
    return result;
}

// The species the last gift was given as (the Mega, or its usual form in the PC).
u16 WotMysteryCode_GivenSpecies(void)
{
    return sGivenSpecies;
}

// --- Specials ---------------------------------------------------------------

// Opens the code entry (8 letters); the script waits, and the typed text is
// left in gStringVar2. Nothing typed leaves it empty.
void WotMysteryCode_OpenEntry(void)
{
    gStringVar2[0] = EOS;
    DoNamingScreen(NAMING_SCREEN_MYSTERY_CODE, gStringVar2, 0, 0, 0, CB2_ReturnToFieldContinueScript);
}

// VAR_RESULT = WOT_MYSTERY_CODE_*. For a known code (OK or USED):
// VAR_0x8005 = its index, VAR_0x8006 = TRUE for an epic (rare or secret)
// card, STR_VAR_1 = the card's name, and for the portrait (showmonpic)
// VAR_0x8004 = the species it shows and VAR_0x8007 = whether it's shiny.
void WotMysteryCode_Check(void)
{
    s32 index;

    gSpecialVar_Result = WotMysteryCode_CheckInput(gStringVar2, &index);
    if (index >= 0)
    {
        gSpecialVar_0x8004 = WotMysteryCode_PortraitSpecies(&sWotMysteryCodes[index]);
        gSpecialVar_0x8005 = index;
        gSpecialVar_0x8006 = sWotMysteryCodes[index].epic;
        gSpecialVar_0x8007 = sWotMysteryCodes[index].shiny;
        StringCopy(gStringVar1, sWotMysteryCodes[index].name);
    }
}

// Gives the gift for VAR_0x8005. VAR_RESULT = MON_GIVEN_TO_PARTY /
// MON_GIVEN_TO_PC / MON_CANT_GIVE. When given, STR_VAR_1 = the species name
// and VAR_TEMP_TRANSFERRED_SPECIES = the species, for the shared nickname
// and sent-to-PC scripts.
void WotMysteryCode_Give(void)
{
    u32 index = gSpecialVar_0x8005;

    gSpecialVar_Result = WotMysteryCode_GiveGift(index);
    if (gSpecialVar_Result != MON_CANT_GIVE)
    {
        StringCopy(gStringVar1, GetSpeciesName(sGivenSpecies));
        VarSet(VAR_TEMP_TRANSFERRED_SPECIES, sGivenSpecies);
    }
}
