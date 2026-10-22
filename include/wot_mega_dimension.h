#ifndef GUARD_WOT_MEGA_DIMENSION_H
#define GUARD_WOT_MEGA_DIMENSION_H

// Seconds the player gets inside before the dimension collapses.
#define MEGA_DIM_TIME_LIMIT 120

bool32 WotMegaDim_IsActive(void);
bool32 WotMegaDim_TakeForceOnFoot(void);
bool32 WotMegaDim_IsPlayerPaletteTag(u16 tag);
void WotMegaDim_UpdatePortalFlag(void);
void WotMegaDim_Tick(void);
bool32 WotMegaDim_TrySetWhiteoutWarp(void);
u8 WotMegaDim_GetWildLevel(void);
void WotMegaDim_GrayBattleUi(void);
bool32 WotMegaDim_IsRivalTrainer(u16 trainerId);
bool32 WotMegaDim_IsRivalPaletteTag(u16 tag);
void WotMegaDim_AdjustRivalParty(struct Pokemon *party, u16 trainerId);
bool32 WotMegaDim_IsLegendBoss(void);
u16 WotMegaDim_GetRivalMusic(u16 trainerId);

// Specials
void WotMegaDim_CheckPortal(void);
void WotMegaDim_EnterPortal(void);
void WotMegaDim_LeaveOut(void);
void WotMegaDim_CollapseOut(void);
void WotMegaDim_MarkWildGone(void);
void WotMegaDim_DebugForceShiny(void);
u16 WotMegaDim_MableReport(void);
void WotMegaDim_StartLegendQuest(void);
void WotMegaDim_BufferPortalAlert(void);
void WotMegaDim_DebugMableNext(void);
void WotHotelZ_SetExit(void);
void WotMegaDim_GetMega(void);
void WotMegaDim_SetMegaGfx(void);
void WotMegaDim_SetupMegaBattle(void);
void WotMegaDim_EndMegaBattle(void);
void WotMegaDim_CloseTodaysPortal(void);
void WotMegaDim_GetRival(void);
u16 WotMegaDim_CountClaimed(void);
void WotMegaDim_BufferTalkedMon(void);
void WotMegaDim_StartOverworldMonBattle(void);
void WotMegaDim_DebugForceRival(void);
void WotMegaDim_MarkClaimed(void);
void WotMegaDim_StartHud(void);
void WotMegaDim_DebugOpenPortal(void);
void WotMegaDim_DebugShortTimer(void);

#endif // GUARD_WOT_MEGA_DIMENSION_H
