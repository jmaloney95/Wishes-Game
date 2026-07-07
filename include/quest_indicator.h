#ifndef GUARD_QUEST_INDICATOR_H
#define GUARD_QUEST_INDICATOR_H

#include "config/ui.h"

// Shows a floating "!" over a quest-giver NPC's head while their quest is
// still available to pick up. Call from a map's OnResume/OnLoad script.
void QuestIndicator_TryShow(u8 localId, u8 questId);
void QuestIndicator_ClearAll(void);
void QuestIndicator_Reset(void);

#endif // GUARD_QUEST_INDICATOR_H
