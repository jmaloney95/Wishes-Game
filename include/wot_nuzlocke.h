#ifndef GUARD_WOT_NUZLOCKE_H
#define GUARD_WOT_NUZLOCKE_H

// Nuzlocke mode, chosen on the NEW GAME SETTINGS page (or the bedroom PC
// before the starter is taken) and locked after that.
//   * Only the first wild Pokemon met in each area can be caught. A shiny can
//     always be caught, and a Pokemon whose family you already own doesn't use
//     up the area (dupes clause). Gifts, eggs, snags and scripted encounters
//     are free.
//   * A Pokemon that faints is laid in the last PC box, which nothing leaves.
//   * Blacking out ends the run: the rules switch off and the game goes on.
#define WOT_NUZLOCKE TRUE

#define WOT_NUZLOCKE_GRAVE_BOX (TOTAL_BOXES_COUNT - 1)

// SaveBlock3.wotNuzlocke bits
#define WOT_NUZLOCKE_ON           (1 << 0)
#define WOT_NUZLOCKE_ENDED_NOTICE (1 << 1) // show "the run is over" once

bool32 WotNuzlocke_IsOn(void);
void WotNuzlocke_SetOn(bool32 on);
void WotNuzlocke_ResetSave(void);
bool32 WotNuzlocke_IsGraveBox(u32 boxId);

void WotNuzlocke_NextScriptedWildCounts(void);
void WotNuzlocke_OnWildBattleStart(bool32 scripted);
void WotNuzlocke_OnBattleInit(void);
void WotNuzlocke_RecordFaint(u32 battler);
bool32 WotNuzlocke_CanCatch(void);
const u8 *WotNuzlocke_CantCatchText(void);
void WotNuzlocke_AfterBattle(void);
void WotNuzlocke_OnWhiteOut(void);
bool8 WotNuzlocke_TakeEndedNotice(void);

#endif // GUARD_WOT_NUZLOCKE_H
