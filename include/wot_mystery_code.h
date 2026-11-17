#ifndef GUARD_WOT_MYSTERY_CODE_H
#define GUARD_WOT_MYSTERY_CODE_H

// Mystery Codes: each Mega card carries one fixed code, and typing it at the
// code NPC gives that card's Pokemon, once per save. The table is generated
// from the card list by tools/wot_mystery_codes.py (src/data/wot_mystery_codes.h).

#include "constants/wot_mystery_code.h"

struct BoxPokemon;

struct WotMysteryCodeGift
{
    const char code[WOT_MYSTERY_CODE_LENGTH + 1]; // capitals, ASCII
    u16 species;
    u16 item;
    u16 extraMove;  // taught on top of the level-up moves (MOVE_NONE: none)
    bool8 shiny;
    bool8 shadow;
    bool8 epic;     // a rare or secret card on the site: "Epic pull!"
    const u8 *name; // the card's name, e.g. "Mega Lugia"
};

// Specials.
void WotMysteryCode_OpenEntry(void);
void WotMysteryCode_Check(void);
void WotMysteryCode_Give(void);

// Also used by the tests.
u32 WotMysteryCode_Count(void);
const struct WotMysteryCodeGift *WotMysteryCode_Get(u32 index);
s32 WotMysteryCode_Find(const u8 *input);
u32 WotMysteryCode_CheckInput(const u8 *input, s32 *index);
bool32 WotMysteryCode_IsUsed(u32 index);
u32 WotMysteryCode_GiveGift(u32 index);
u32 WotMysteryCode_GiftLevel(void);
bool32 WotMysteryCode_IsGiftBoxMon(struct BoxPokemon *boxMon);
bool32 WotMysteryCode_IsSpeciesRedeemed(u16 species);
u16 WotMysteryCode_MegaSpecies(const struct WotMysteryCodeGift *gift);
u16 WotMysteryCode_PortraitSpecies(const struct WotMysteryCodeGift *gift);
u16 WotMysteryCode_GivenSpecies(void);

#endif // GUARD_WOT_MYSTERY_CODE_H
