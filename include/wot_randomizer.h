#ifndef GUARD_WOT_RANDOMIZER_H
#define GUARD_WOT_RANDOMIZER_H

#include "main.h"   // MainCallback

// Wishes of Tomorrow: Randomizer Mode (see src/wot_randomizer.c).
bool32 WotRandomizerActive(void);
bool32 WotRandomizerOption(u16 flag);
u16 WotRandomizeSpecies(u16 species);
bool32 WotTrainerIsRandomizerExempt(void);

u16 WotRandomizeStarter(u16 species);

// The settings screen (src/wot_randomizer_menu.c): the whole page on NEW
// GAME, the randomizer rows alone from the bedroom PC.
void WotStartNewGameSettings(MainCallback returnCallback);
void WotStartRandomizerMenu(MainCallback returnCallback);
void WotQueueNewGameSettings(bool32 starters, bool32 wild, bool32 trainers, bool32 levelCapsOff, bool32 nuzlocke);
void WotApplyQueuedRandomizerSettings(void);

#endif // GUARD_WOT_RANDOMIZER_H
