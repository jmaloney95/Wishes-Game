#ifndef GUARD_WOT_RANDOMIZER_H
#define GUARD_WOT_RANDOMIZER_H

#include "main.h"   // MainCallback

// Wishes of Tomorrow: Randomizer Mode (see src/wot_randomizer.c).
bool32 WotRandomizerActive(void);
u16 WotRandomizeSpecies(u16 species);
bool32 WotTrainerIsRandomizerExempt(void);

u16 WotRandomizeStarter(u16 species);

// The settings screen (src/wot_randomizer_menu.c).
void CB2_WotRandomizerMenu(void);
void WotStartRandomizerMenu(MainCallback returnCallback);
void WotQueueRandomizerSettings(bool32 master, bool32 starters, bool32 wild, bool32 trainers);
void WotApplyQueuedRandomizerSettings(void);

#endif // GUARD_WOT_RANDOMIZER_H
