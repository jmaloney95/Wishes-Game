#ifndef GUARD_WOT_SHADOW_LOG_H
#define GUARD_WOT_SHADOW_LOG_H

void WotShadowLog_MarkSnaggedSpecies(u16 species);
void WotShadowLog_MarkPurifiedSpecies(u16 species);
void WotShadowLog_MarkMon(struct Pokemon *mon);
u32 WotShadowLog_CountSnagged(void);
u32 WotShadowLog_CountPurified(void);
void WotShadowLog_Backfill(void);
void WotShowShadowLog(void);

#endif // GUARD_WOT_SHADOW_LOG_H
