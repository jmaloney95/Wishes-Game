#ifndef GUARD_WOT_PERMANENT_MEGA_H
#define GUARD_WOT_PERMANENT_MEGA_H

// Permanent Mega Evolution: one of the player's Pokemon that Mega Evolves keeps
// the form after battle while it holds its stone (src/wot_permanent_mega.c).

extern bool8 gWotPermanentMegaInTests;

u16 WotGetMegaStone(u16 megaSpecies);
u16 WotGetMegaBaseSpecies(u16 megaSpecies);
bool32 WotIsKeptMega(u16 species, u16 heldItem);
u32 WotGetOutOfBattleMegaTarget(u32 method, u16 species, u16 heldItem);
bool32 WotKeepsMegaInBattle(u32 battler);
bool32 WotShouldKeepPartyMegaAtEnd(u32 partyIndex);
bool32 WotIsMegaBlockedByStoredMega(u32 battler);
void WotTryMarkStoredMega(u32 battler);

#endif // GUARD_WOT_PERMANENT_MEGA_H
