#ifndef GUARD_WOT_REMATCH_H
#define GUARD_WOT_REMATCH_H

// Talk to a route or gym trainer you already beat and they offer a rematch.
// The rematch team is the same trainer's team, raised to the current level cap
// (or, with no cap, to the player's best Pokemon) and evolved to fit.
#define WOT_REMATCHES TRUE

// Every rematch Pokemon but the last sits this far under the target level.
#define WOT_REMATCH_LEVEL_GAP 2
// Evolutions with no level of their own (stones, trades, friendship, time of
// day) happen in a rematch once the Pokemon reaches these levels.
#define WOT_REMATCH_EVO_LEVEL_CONDITION 30 // a level-up evolution with conditions only
#define WOT_REMATCH_EVO_LEVEL_OTHER     36 // stone, trade and the rest

bool8 WotRematch_CanOffer(void);
void WotRematch_BufferName(void);
void WotRematch_Arm(void);
void WotRematch_Disarm(void);
bool32 WotRematch_IsActiveFor(u16 trainerId);
u32 WotRematch_TargetLevel(void);
u32 WotRematch_ScaleLevel(u32 baseLevel, u32 slot, u32 monsCount);
u32 WotRematch_EvolveSpecies(u32 species, u32 level);

#endif // GUARD_WOT_REMATCH_H
