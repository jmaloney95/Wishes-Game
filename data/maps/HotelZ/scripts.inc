@ ============================================================================
@  HOTEL Z -- Team MZ's base (placeholder interior: a copy of the Slateport
@  Pokemon Fan Club layout with its raised stage flattened, LAYOUT_HOTEL_Z).
@  Two ways in, both once the Tradewind intro has played: Tradewind's
@  (23,17) building and the back door in the Rebel Hideout. The exits use
@  the dynamic warp, so you leave the way you came.
@  The first time the player walks in, Mable briefs them on rogue Mega
@  Evolution: Mega Doomsday completes and Two Minutes Is a Long Time starts
@  (bring her the stones of all 19 rogue Megas; she keeps a tally). Then her
@  calls start Primal Signatures and Sky-High Mega (data/scripts/mega_dimension.inc).
@ ============================================================================
HotelZ_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, HotelZ_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, HotelZ_OnFrame
	.byte 0

HotelZ_OnTransition::
	special WotHotelZ_SetExit
	call_if_unset FLAG_WOT_MEGA_DOOMSDAY_INTRO, HotelZ_EventScript_HideTeam
	call_if_set FLAG_WOT_MEGA_DOOMSDAY_INTRO, HotelZ_EventScript_ShowTeam
	end

HotelZ_EventScript_HideTeam::
	setflag FLAG_HIDE_HOTEL_Z_TEAM
	return

HotelZ_EventScript_ShowTeam::
	clearflag FLAG_HIDE_HOTEL_Z_TEAM
	return

HotelZ_OnFrame:
	map_script_2 VAR_TEMP_1, 0, HotelZ_EventScript_Briefing
	.2byte 0

HotelZ_EventScript_Briefing::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_set FLAG_WOT_HOTEL_Z_MET, HotelZ_EventScript_BriefingDone
	goto_if_unset FLAG_WOT_MEGA_DOOMSDAY_INTRO, HotelZ_EventScript_BriefingDone
	applymovement LOCALID_HOTEL_Z_LIDA, Common_Movement_ExclamationMark
	waitmovement 0
	@ Whoever is talking hops (Lida, Canari) or steps up (Mable), so the
	@ player learns who's who; the nameplate colours match.
	applymovement LOCALID_HOTEL_Z_LIDA, HotelZ_Movement_Hop
	waitmovement 0
	setspeaker SP_NAME_LIDA
	msgbox HotelZ_Text_LidaWelcome, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_HOTEL_Z_CANARI, HotelZ_Movement_Hop
	waitmovement 0
	setspeaker SP_NAME_CANARI
	msgbox HotelZ_Text_CanariWelcome, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_HOTEL_Z_MABLE, Common_Movement_WalkDown
	waitmovement 0
	setspeaker SP_NAME_MABLE
	msgbox HotelZ_Text_MableResearch, MSGBOX_DEFAULT
	msgbox HotelZ_Text_MableOffer, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_HOTEL_Z_MABLE, HotelZ_Movement_MableStepBack
	waitmovement 0
	applymovement LOCALID_HOTEL_Z_LIDA, HotelZ_Movement_Hop
	waitmovement 0
	setspeaker SP_NAME_LIDA
	msgbox HotelZ_Text_LidaLongTime, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	setflag FLAG_WOT_HOTEL_Z_MET
	completequest QUEST_MEGA_DOOMSDAY
	startquest QUEST_TWO_MINUTES
HotelZ_EventScript_BriefingDone::
	releaseall
	end

HotelZ_Movement_Hop:
	jump_in_place_down
	step_end

HotelZ_Movement_MableStepBack:
	walk_up
	face_down
	step_end

@ Mable keeps a tally of the rogue Megas' stones (the player keeps the
@ stones). Only the dimension's own claims count. At 19 she runs her tests;
@ after that she has a line for each stage of her quest chain.
HotelZ_EventScript_Mable::
	lock
	faceplayer
	setspeaker SP_NAME_MABLE
	specialvar VAR_RESULT, WotMegaDim_MableReport
	buffernumberstring STR_VAR_1, VAR_0x8004
	goto_if_eq VAR_RESULT, 3, HotelZ_EventScript_MableStage
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_MableAllStones
	goto_if_eq VAR_RESULT, 1, HotelZ_EventScript_MableRecorded
	goto_if_eq VAR_0x8004, 0, HotelZ_EventScript_MableWaiting
	msgbox HotelZ_Text_MableTally, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableWaiting::
	msgbox HotelZ_Text_MableWaiting, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableRecorded::
	call_if_eq VAR_0x8004, 1, HotelZ_EventScript_MableFirstSample
	msgbox HotelZ_Text_MableRecorded, MSGBOX_DEFAULT
	closemessage
	questtoast QUEST_TOAST_UPDATE, QUEST_TWO_MINUTES
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableFirstSample::
	msgbox HotelZ_Text_MableSample, MSGBOX_DEFAULT
	return

HotelZ_EventScript_MableAllStones::
	msgbox HotelZ_Text_MableAllStones, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	goto_if_quest_complete QUEST_TWO_MINUTES, HotelZ_EventScript_MableEnd
	completequest QUEST_TWO_MINUTES
	goto HotelZ_EventScript_MableEnd

@ VAR_0x8005 = her stage (WotMegaDim MABLE_STAGE_*).
HotelZ_EventScript_MableStage::
	goto_if_eq VAR_0x8005, 1, HotelZ_EventScript_MableTesting
	goto_if_eq VAR_0x8005, 2, HotelZ_EventScript_MablePrimals
	goto_if_eq VAR_0x8005, 3, HotelZ_EventScript_MableTesting
	goto_if_eq VAR_0x8005, 4, HotelZ_EventScript_MableRayquaza
	msgbox HotelZ_Text_MableAfter, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableTesting::
	msgbox HotelZ_Text_MableTesting, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MablePrimals::
	msgbox HotelZ_Text_MablePrimals, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableRayquaza::
	msgbox HotelZ_Text_MableRayquaza, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableEnd::
	closemessage
	setspeaker SP_NAME_NONE
	release
	end

HotelZ_EventScript_Lida::
	lock
	faceplayer
	setspeaker SP_NAME_LIDA
	msgbox HotelZ_Text_LidaIdle, MSGBOX_DEFAULT
	setspeaker SP_NAME_NONE
	release
	end

HotelZ_EventScript_Canari::
	lock
	faceplayer
	setspeaker SP_NAME_CANARI
	msgbox HotelZ_Text_CanariIdle, MSGBOX_DEFAULT
	setspeaker SP_NAME_NONE
	release
	end

HotelZ_Text_LidaWelcome:
	.string "{PLAYER}! You actually came!\p"
	.string "Welcome to Hotel Z. Officially\n"
	.string "it's a hotel. Unofficially…\l"
	.string "it's Team MZ's base.$"

HotelZ_Text_CanariWelcome:
	.string "Unofficially-officially. We have\n"
	.string "a logo and everything.$"

HotelZ_Text_MableResearch:
	.string "So you're the one who stood up\n"
	.string "to The Oni. I'm Mable, Team MZ's\l"
	.string "researcher. Report back to me.\p"
	.string "I study Mega Evolution. Lately,\n"
	.string "the kind that's gone wrong.\p"
	.string "Rogue Megas. Pokémon locked in\n"
	.string "Mega form, with no Trainer and\l"
	.string "no Mega Ring holding them.\p"
	.string "They're being pulled through\n"
	.string "rifts, from a gray mirror of\l"
	.string "our world. A Mega Dimension.$"

HotelZ_Text_MableOffer:
	.string "I can hold a rift open for two\n"
	.string "minutes. Not a second more.\p"
	.string "Go in, find the rogue Mega and\n"
	.string "neutralize it. Bring me back its\l"
	.string "Mega Stone as a sample.\p"
	.string "There are 19 of them out there.\n"
	.string "I need a reading from each one.\p"
	.string "The rifts open at 8:00 every\n"
	.string "morning. Watch for the light.$"

HotelZ_Text_LidaLongTime:
	.string "Two minutes is a long time…\n"
	.string "if you're fast!$"

HotelZ_Text_MableSample:
	.string "Is that… a rogue Mega's stone?\n"
	.string "You actually did it.\p"
	.string "This is exactly the sample I\n"
	.string "needed. Thank you, {PLAYER}.$"

HotelZ_Text_MableRecorded:
	.string "Let me take a reading… there.\n"
	.string "Keep the stone, it's yours.\p"
	.string "Recorded! That's {STR_VAR_1}\n"
	.string "of 19.$"

HotelZ_Text_MableTally:
	.string "Your tally so far: {STR_VAR_1} of 19.\n"
	.string "Find a rift and keep going!$"

HotelZ_Text_MableAllStones:
	.string "That's the last one. 19 of 19.\n"
	.string "Every rogue Mega's stone.\p"
	.string "Now I can run the tests.\n"
	.string "Give me a day. I'll call you.$"

HotelZ_Text_MableTesting:
	.string "The tests are still running.\n"
	.string "I'll call you the moment I\l"
	.string "have something.$"

HotelZ_Text_MablePrimals:
	.string "Primal Groudon and Primal Kyogre.\n"
	.string "They're waiting in the portals.\p"
	.string "Catch them both, {PLAYER}.\n"
	.string "A knockout won't do it.$"

HotelZ_Text_MableRayquaza:
	.string "Mega Rayquaza is in a portal.\n"
	.string "Catch it before it tears the\l"
	.string "rifts wide open.$"

HotelZ_Text_MableWaiting:
	.string "Find a rift, take down the rogue\n"
	.string "Mega and bring me its stone.\l"
	.string "Two minutes. Go!$"

HotelZ_Text_MableAfter:
	.string "The rifts are settling. Thanks\n"
	.string "to you, {PLAYER}, Team MZ has\l"
	.string "every reading I could want.$"

HotelZ_Text_LidaIdle:
	.string "Team MZ's always short on hands.\n"
	.string "Glad you're on board!$"

HotelZ_Text_CanariIdle:
	.string "Don't mind the camera. Okay,\n"
	.string "maybe mind it a little.$"
