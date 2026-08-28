#ifndef GUARD_WOT_SHADOW_LOG_H
#define GUARD_WOT_SHADOW_LOG_H

void WotShadowLog_MarkSnaggedSpecies(u16 species);
void WotShadowLog_MarkPurifiedSpecies(u16 species);
void WotShadowLog_MarkMon(struct Pokemon *mon);
u32 WotShadowLog_CountSnagged(void);
u32 WotShadowLog_CountPurified(void);
void WotShadowLog_Backfill(void);
void WotShowShadowLog(void);

// TRUE for the story Shadow species, which are Shadow by their very identity
// rather than by the MON_DATA_IS_SHADOW flag.
bool32 WotSpeciesIsShadow(u16 species);

// TRUE if this battler should be treated as Shadow for presentation: the flag,
// or one of the inherently-Shadow species.
bool32 WotMonIsShadow(struct Pokemon *mon);

// Folds a story Shadow species back to the ordinary one it stands in for.
u16 WotBaseSpeciesForShadow(u16 species);

#endif // GUARD_WOT_SHADOW_LOG_H
