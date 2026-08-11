@ ============================================================================
@  WoT: Harness Darkness progress -- called after any battle that can snag.
@  Stage 1: first Shadow snagged -> toast + reward. Stage 2: five -> done.
@ ============================================================================
Wot_EventScript_SnagProgress::
	goto_if_set FLAG_WOT_SNAG_STAGE2, Wot_EventScript_SnagProgressEnd
	questmenu QUEST_MENU_CHECK_ACTIVE, SIDE_QUEST_HARNESS_DARKNESS
	goto_if_ne VAR_RESULT, TRUE, Wot_EventScript_SnagProgressEnd
	goto_if_set FLAG_WOT_SNAG_STAGE1, Wot_EventScript_SnagProgressStage2
	compare VAR_WOT_SNAG_COUNT, 1
	goto_if_lt Wot_EventScript_SnagProgressEnd
	setflag FLAG_WOT_SNAG_STAGE1
	questtoast QUEST_TOAST_UPDATE, SIDE_QUEST_HARNESS_DARKNESS
	msgbox Wot_Text_SnagStage1, MSGBOX_DEFAULT
	giveitem ITEM_ULTRA_BALL, 5
	closemessage
	return
Wot_EventScript_SnagProgressStage2::
	compare VAR_WOT_SNAG_COUNT, 5
	goto_if_lt Wot_EventScript_SnagProgressEnd
	setflag FLAG_WOT_SNAG_STAGE2
	completequest SIDE_QUEST_HARNESS_DARKNESS
Wot_EventScript_SnagProgressEnd::
	return

Wot_Text_SnagStage1::
	.string "The SNAG MACHINE thrums--\n"
	.string "its first stolen soul secured.\p"
	.string "Four more and Mutrid will\n"
	.string "feel it.$"
