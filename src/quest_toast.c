#include "global.h"
#include "quest_toast.h"
#include "field_message_box.h"
#include "map_name_popup.h"
#include "script.h"
#include "quests.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "constants/songs.h"

// Non-blocking quest toast (UI features handoff, Feature 4): quest start /
// update / complete notifications ride the map-name popup (slide in, hold,
// slide out - no input, no lockall) instead of a blocking message box.
// A small FIFO queue plays back-to-back events in order, each with its sound
// on the frame its toast spawns. The popup task already cleans itself up on
// menus/battles/warps; QuestToast_Reset() (called on field reload) drops any
// queued toasts so they cannot fire on a stale context.

#define QUEST_TOAST_QUEUE_LEN 4

struct QuestToastEvent
{
    u8 questId;
    u8 eventType;
};

static EWRAM_DATA struct QuestToastEvent sQueue[QUEST_TOAST_QUEUE_LEN] = {0};
static EWRAM_DATA u8 sQueueCount = 0;
static EWRAM_DATA u8 sPumpTaskId = 0;
static EWRAM_DATA bool8 sPumpActive = FALSE;

static const u8 sText_ToastStart[] = _("NEW QUEST");
static const u8 sText_ToastUpdate[] = _("QUEST UPDATED");
static const u8 sText_ToastComplete[] = _("QUEST COMPLETE");

static void Task_QuestToastPump(u8 taskId)
{
    const u8 *label;

    if (MapNamePopupIsActive())
        return; // wait for the current toast (or map-name popup) to finish

    // Defer while any script or field dialogue is live: the popup windows
    // share BG0 tile VRAM (namebox/yes-no/msgbox ranges), palette 14, and
    // the unified HBlank BG0VOFS scroll with the message system -- firing
    // mid-dialogue shreds the text box. The queue itself is the deferral;
    // toasts (and their SEs) fire on the first free frame after releaseall.
    if (ScriptContext_IsEnabled() || !IsFieldMessageBoxHidden() || ArePlayerFieldControlsLocked())
        return;

    if (sQueueCount == 0)
    {
        sPumpActive = FALSE;
        DestroyTask(taskId);
        return;
    }

    switch (sQueue[0].eventType)
    {
    case QUEST_TOAST_COMPLETE:
        label = sText_ToastComplete;
        PlaySE(SE_SUCCESS);
        break;
    case QUEST_TOAST_UPDATE:
        label = sText_ToastUpdate;
        PlaySE(SE_PIN);
        break;
    default:
        label = sText_ToastStart;
        PlaySE(SE_PIN);
        break;
    }

    QuestMenu_CopyQuestName(gStringVar1, sQueue[0].questId);
    ShowQuestToastPopup(label);

    sQueueCount--;
    memmove(&sQueue[0], &sQueue[1], sQueueCount * sizeof(sQueue[0]));
}

void QuestToast_Enqueue(u8 questId, u8 eventType)
{
    if (!USE_QUEST_TOAST)
        return;
    if (sQueueCount >= QUEST_TOAST_QUEUE_LEN)
        return; // drop overflow rather than stall the script

    sQueue[sQueueCount].questId = questId;
    sQueue[sQueueCount].eventType = eventType;
    sQueueCount++;

    if (!sPumpActive)
    {
        sPumpTaskId = CreateTask(Task_QuestToastPump, 80);
        sPumpActive = (sPumpTaskId != TASK_NONE);
    }
}

// Field reload wipes all tasks; drop queued toasts and pump bookkeeping too.
void QuestToast_Reset(void)
{
    sQueueCount = 0;
    sPumpActive = FALSE;
}
