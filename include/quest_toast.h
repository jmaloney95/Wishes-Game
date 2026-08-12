#ifndef GUARD_QUEST_TOAST_H
#define GUARD_QUEST_TOAST_H

#include "config/ui.h"
#include "constants/quests.h"

void QuestToast_Enqueue(u8 questId, u8 eventType);
void QuestToast_Reset(void);

#endif // GUARD_QUEST_TOAST_H
