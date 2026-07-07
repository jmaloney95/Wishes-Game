#ifndef GUARD_CONSTANTS_QUESTS_H
#define GUARD_CONSTANTS_QUESTS_H

//questmenu scripting command params
#define QUEST_MENU_OPEN                 0   //opens the quest menu (questId = 0)
#define QUEST_MENU_UNLOCK_QUEST         1   //questId = QUEST_X (0-indexed)
#define QUEST_MENU_SET_ACTIVE           2   //questId = QUEST_X (0-indexed)
#define QUEST_MENU_SET_REWARD           3   //questId = QUEST_X (0-indexed)
#define QUEST_MENU_COMPLETE_QUEST       4   //questId = QUEST_X (0-indexed)
#define QUEST_MENU_CHECK_UNLOCKED       5   //checks if questId has been unlocked. Returns result to gSpecialVar_Result
#define QUEST_MENU_CHECK_INACTIVE       6   //check if a questID is inactive. Returns result to gSpecialVar_Result
#define QUEST_MENU_CHECK_ACTIVE         7   //checks if questId is active. Returns result to gSpecialVar_Result
#define QUEST_MENU_CHECK_REWARD         8   //checks if questId is in Reward state. Returns result to gSpecialVar_Result
#define QUEST_MENU_CHECK_COMPLETE       9   //checks if questId has been completed. Returns result to gSpecialVar_Result
#define QUEST_MENU_BUFFER_QUEST_NAME    10  //buffers a quest name to gStringVar1

// Wishes of Tomorrow quest roster (see Quest_Plan_Handoff.md).
// Keep this order stable once shipped: saveblock quest states are indexed by id.
#define QUEST_WAKING_MOUNTAIN            0  // MAIN - Act 1 main line
#define SIDE_QUEST_ONE_THAT_GOT_AWAY     1  // Side - fisherman's son (Munen)
#define SIDE_QUEST_FROZEN_TRACKS         2  // Side - Shinkansen sabotage
#define SIDE_QUEST_EIGHT_WAYS            3  // Side - Eevee breeder (park)
#define SIDE_QUEST_HEAVY_METAL           4  // Side - Scyther collector (park)
#define SIDE_QUEST_SAVE_FACE             5  // Side - MaskMaker (Pompeii)
#define QUEST_GRANDFATHERS_WISH          6  // MAIN - Catalpa Bow rite
#define QUEST_LIFES_WORK                 7  // MAIN - destroy Clarkson's research
#define QUEST_TOASTY_TIME                8  // Side - fetch the gym leader's sisters (Frostwood)
#define QUEST_COUNT                      9

// No subquests yet; the parked purification line (one subquest per shadow
// legendary) will use these when it is unparked.
#define SUB_QUEST_COUNT                  0

#define QUEST_ARRAY_COUNT (SUB_QUEST_COUNT > QUEST_COUNT ? SUB_QUEST_COUNT : QUEST_COUNT)

// Quest toast event types (questtoast script command / src/quest_toast.c)
#define QUEST_TOAST_START     0
#define QUEST_TOAST_UPDATE    1
#define QUEST_TOAST_COMPLETE  2

// Saveblock storage caps. SaveBlock2's quest bitfields are sized from these
// (not from QUEST_COUNT) so adding quests up to the caps does NOT change the
// save layout and existing saves keep working. Raising a cap breaks saves.
#define QUEST_STORAGE_COUNT             32
#define SUB_QUEST_STORAGE_COUNT         32

#endif // GUARD_CONSTANTS_QUESTS_H
