#ifndef GUARD_WOT_MEGA_DIMENSION_H
#define GUARD_WOT_MEGA_DIMENSION_H

bool32 WotMegaDim_SaveMissedIntro(void);

#ifdef WOT_TESTER
void WotTester_ResetMegaDimension(void);
void WotTester_SetSeenMaps(bool32 shinTokyo);
void WotTester_OpenMegaDimension(void);
void WotTester_MaxPartners(void);
void WotTester_SetMableTally(u32 claimed);
#endif

// Seconds the player gets inside before the dimension collapses.
#define MEGA_DIM_TIME_LIMIT 120
// Partner friendship tops out here; it unlocks their Rift Sanctum ability.
#define FRIENDSHIP_MAX_LEVEL 4

bool32 WotMegaDim_IsActive(void);
bool32 WotMegaDim_IsInSanctum(void);
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
void WotMegaDim_AdjustSanctumParty(struct Pokemon *party, u16 trainerId);
const u8 *WotPartner_GetAbilityName(u32 i);
void WotSanctum_CheckBarrier(void);
void WotSanctum_Enter(void);
void WotSanctum_Leave(void);
void WotSanctum_Setup(void);
void WotSanctum_ShadeLegend(void);
void WotSanctum_HoldoutBeaten(void);
void WotSanctum_SetupLegendBattle(void);
void WotSanctum_LegendCaught(void);
void WotMegaDim_DebugSanctum(void);
bool32 WotMegaDim_IsLegendBoss(void);
bool32 WotMegaDim_IsRogueZMoveUser(u32 battler);
bool32 WotMegaDim_IsBossZMove(u16 move);
bool32 WotMegaDim_IsRiftBossBattle(void);
u16 WotMegaDim_GetBossZMoveFor(u32 battler);
s32 WotMegaDim_CapCatchRate(s32 catchRate, u32 battler);
bool32 WotMegaDim_TryBossLastStand(u32 battler);
bool32 WotMegaDim_TakeBossLastStand(u32 battler);
void WotMegaDim_SetMegaClaimed(u32 index, bool32 claimed);
u16 WotMegaDim_GetRivalMusic(u16 trainerId);

// Specials
void WotMegaDim_CheckPortal(void);
void WotMegaDim_TaunieScout(void);
void WotMegaDim_PortalArrivalDone(void);
void WotMegaDim_NoteZMoveUsed(u32 battler);
bool32 WotMegaDim_IsRogueCoreZUser(u32 battler);
void WotZShards_Give(void);
void WotZShards_GiveBoss(void);
u16 WotMegaDim_MableGiftDue(void);
u32 WotZShop_GetPrice(u16 item);
const u16 *WotZShop_BuildStock(void);
void WotZShop_Open(void);
void WotPartner_TakeCrystalGift(void);
void WotMegaDim_DebugZKit(void);
u16 WotZygarde_GetStage(void);
void WotZygarde_SetStage(void);
u16 WotZygarde_PebbleDue(void);
u16 WotZygarde_RevealDue(void);
void WotZygarde_SetupDesk(void);
void WotZygarde_SetupDrainedHotel(void);
u16 WotZygarde_IsTalkedCell(void);
void WotZygarde_TakeCell(void);
u16 WotZygarde_DogCounts(void);
u16 WotZygarde_MableLine(void);
void WotZygarde_SetupDogBattle(void);
void WotZygarde_SetupFloetteBattle(void);
void WotZygarde_SetupZygardeBattle(void);
void WotZygarde_AdjustAZParty(struct Pokemon *party, u16 trainerId);
void WotZygarde_DebugNext(void);

// The Mega Log (src/wot_mega_log.c).
struct WotMegaLogEntry
{
    u16 species;        // the form it shows
    u16 iconSpecies;    // its grid icon
    u16 stone;          // ITEM_NONE for Mega Rayquaza
    bool8 claimed;
    bool8 primal;
};
u32 WotMegaLog_RogueCount(void);
u32 WotMegaLog_LegendCount(void);
void WotMegaLog_GetEntry(u32 i, struct WotMegaLogEntry *entry);

void WotMegaDim_GetReturnPortal(void);
void WotMegaDim_ShowReturningPlayer(void);
void WotMegaDim_PortalGrow(void);
void WotMegaDim_PortalShrink(void);
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
bool32 WotMegaDim_HasPartnerHere(void);
void WotMegaDim_AdjustPartnerParty(void);
void WotMegaDim_EndPartnerBattle(void);
bool32 WotMegaDim_PartnerPlayerControlled(void);
bool32 WotMegaDim_IsControlledPartner(u32 battler);
void WotMegaDim_GetRivalState(void);
void WotMegaDim_SetPartner(void);
void WotMegaDim_BufferPartner(void);
void WotMegaDim_TakeTwoHearts(void);
void WotMegaDim_TakeFriendshipUp(void);

// Partners and friendship; i = 0-5 in sRivals order
u32 WotPartner_GetCurrent(void);
void WotPartner_SetCurrent(u32 partner);
u32 WotPartner_GetRuns(u32 i);
u32 WotPartner_GetLevel(u32 i);
u32 WotPartner_GetRunsForNextLevel(u32 i);
const u8 *WotPartner_GetPerkText(u32 i, u32 level);
const u8 *WotPartner_GetNextPerkText(u32 i);
const u8 *WotPartner_GetOrder(u32 i);
void WotPartner_SetOrder(u32 i, const u8 *slots);
void WotPartner_SaveMoves(u32 i, u32 slot, struct Pokemon *mon);
void WotPartner_BuildTeam(u32 i, u32 best, bool32 forBattle);
void WotPartner_BufferFieldLine(void);
void WotPartner_BufferHotelLine(void);
extern u8 gWotPartnerLine[];
u32 WotMegaDim_GetBestLevel(void);
void WotHotelZ_ShowRivals(void);
void WotMegaDim_DebugPartner(void);
void WotMegaDim_DebugPartnerRuns(void);
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
