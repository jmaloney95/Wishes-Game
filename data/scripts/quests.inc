@ ============================================================================
@  QUEST SYSTEM (Pokémon Wishes of Tomorrow)
@  Lightweight, native quest tracking:
@   - One VAR per quest (see include/constants/vars.h):
@       0 = not started, 1..N = active stage, 100 = complete.
@   - On-screen notifications: buffer the quest title into STR_VAR_1, then
@     call one of the Notify helpers below (map-name-popup window + jingle;
@     `special ShowQuestPopup` is implemented in src/map_name_popup.c).
@   - The QUEST LOG key item (register it to SELECT!) lists every quest's
@     title, description and current objective, driven by the quest vars.
@  To add a quest: allocate a VAR, add a QuestLog_ sub below and call it
@  from EventScript_QuestLog, and sprinkle setvar+Notify at story beats.
@ ============================================================================

@ --- Notifications (expect STR_VAR_1 = quest title) ------------------------
Quest_EventScript_NotifyStarted::
	special ShowQuestPopup
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	return

Quest_EventScript_NotifyUpdated::
	special ShowQuestPopup
	playse SE_PIN
	return

Quest_EventScript_NotifyCompleted::
	special ShowQuestPopup
	playfanfare MUS_OBTAIN_BADGE
	waitfanfare
	return

@ --- The Quest Log (opened by using the QUEST LOG key item) ----------------
EventScript_QuestLog::
	lockall
	setvar VAR_TEMP_9, 0
	call QuestLog_FathersFinalWish
	compare VAR_TEMP_9, 0
	goto_if_eq QuestLog_NoQuests
	releaseall
	end

QuestLog_NoQuests::
	msgbox QuestLog_Text_NoQuests, MSGBOX_DEFAULT
	releaseall
	end

@ ----------------------------------------------------------------------------
@  QUEST: A FATHER'S FINAL WISH (main) -- VAR_QUEST_FATHERS_WISH
@  1: fetch the FENTANYL for the dying old prophet
@  2: his story is told; retrieve the CATALPA BOW from his house
@  3: use the bow beside the National Park gravesite
@  100: complete
@ ----------------------------------------------------------------------------
QuestLog_FathersFinalWish::
	compare VAR_QUEST_FATHERS_WISH, 0
	goto_if_eq QuestLog_FFW_Done
	setvar VAR_TEMP_9, 1
	compare VAR_QUEST_FATHERS_WISH, 100
	goto_if_eq QuestLog_FFW_Complete
	compare VAR_QUEST_FATHERS_WISH, 1
	goto_if_eq QuestLog_FFW_Stage1
	compare VAR_QUEST_FATHERS_WISH, 2
	goto_if_eq QuestLog_FFW_Stage2
	msgbox QuestLog_Text_FFW_Stage3, MSGBOX_DEFAULT
	return

QuestLog_FFW_Stage1::
	msgbox QuestLog_Text_FFW_Stage1, MSGBOX_DEFAULT
	return

QuestLog_FFW_Stage2::
	msgbox QuestLog_Text_FFW_Stage2, MSGBOX_DEFAULT
	return

QuestLog_FFW_Complete::
	msgbox QuestLog_Text_FFW_Complete, MSGBOX_DEFAULT
	return

QuestLog_FFW_Done::
	return

@ --- Text -------------------------------------------------------------------
Quest_Text_FathersFinalWishTitle::
	.string "A Father's Final Wish$"

QuestLog_Text_NoQuests::
	.string "QUEST LOG\p"
	.string "No quests are underway.\p"
	.string "The world, briefly, asks nothing\n"
	.string "of you. Enjoy it while it lasts.$"

QuestLog_Text_FFW_Stage1::
	.string "QUEST: A FATHER'S FINAL WISH\n"
	.string "-- Main Quest -- IN PROGRESS\p"
	.string "A dying old man in the Ashlands\n"
	.string "clings to one last story, and to\l"
	.string "the ash in his lungs.\p"
	.string "OBJECTIVE: Find his medicine --\n"
	.string "FENTANYL, in an old supply ball\l"
	.string "west of where he lies -- and\l"
	.string "bring it to him.$"

QuestLog_Text_FFW_Stage2::
	.string "QUEST: A FATHER'S FINAL WISH\n"
	.string "-- Main Quest -- IN PROGRESS\p"
	.string "The old prophet's story is told:\n"
	.string "one son lost to the mountain, one\l"
	.string "grandson lost to the mask. His\l"
	.string "last breath opened his door.\p"
	.string "OBJECTIVE: Retrieve his\n"
	.string "grandfather's CATALPA BOW from\l"
	.string "inside his house.$"

QuestLog_Text_FFW_Stage3::
	.string "QUEST: A FATHER'S FINAL WISH\n"
	.string "-- Main Quest -- IN PROGRESS\p"
	.string "The azusa bow looses no arrows.\n"
	.string "Its song opens what grief has\l"
	.string "sealed shut.\p"
	.string "OBJECTIVE: Carry the bow to the\n"
	.string "gravesite in the National Park,\l"
	.string "across the water -- and USE it\l"
	.string "there, beside the stone.$"

QuestLog_Text_FFW_Complete::
	.string "QUEST: A FATHER'S FINAL WISH\n"
	.string "-- Main Quest -- COMPLETE\p"
	.string "The bow sang at the graveside,\n"
	.string "and the seam between worlds\l"
	.string "opened.\p"
	.string "A father's last wish, carried\n"
	.string "all the way through.$"
