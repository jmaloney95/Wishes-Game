	.equ LOCALID_DEN_DRACO, 1

MewtwoPostGame_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, MewtwoPostGame_OnFrame
	.byte 0

MewtwoPostGame_OnFrame:
	map_script_2 VAR_TEMP_1, 0, MewtwoPostGame_EventScript_LockedCheck
	.2byte 0

@ Before Draco's call the dockhouse is sealed: anyone who slips in gets
@ bounced straight back onto the street.
MewtwoPostGame_EventScript_LockedCheck::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_set FLAG_WOT_DRACO_CALL_DONE, MewtwoPostGame_EventScript_LockedCheckDone
	msgbox MewtwoPostGame_Text_Locked, MSGBOX_DEFAULT
	closemessage
	warp MAP_SHIN_TOKYO, 54, 40
	waitstate
MewtwoPostGame_EventScript_LockedCheckDone::
	releaseall
	end

@ ============================================================================
@  THE PRIMITIVE LIFEFORM -- Draco's post-game gauntlet. One battle against
@  his Lv80 ARMORED MEWTWO (TRAINER_WALLY_VR_4 repurposed -- Victory Road is
@  unreachable in WoT); winning hands the MEWTWO itself over. If the party
@  and boxes are full the handover waits: talk to him again with room.
@ ============================================================================
MewtwoPostGame_EventScript_Draco::
	lock
	faceplayer
	goto_if_set FLAG_WOT_ARMORED_MEWTWO_GIVEN, MewtwoPostGame_EventScript_DracoAfter
	setspeaker SP_NAME_DRACO
	msgbox MewtwoPostGame_Text_DracoIntro, MSGBOX_DEFAULT
	closemessage
	hideportrait
	trainerbattle_no_intro TRAINER_WALLY_VR_4, MewtwoPostGame_Text_DracoDefeat
	setspeaker SP_NAME_DRACO
	msgbox MewtwoPostGame_Text_DracoHandOver, MSGBOX_DEFAULT
	closemessage
	hideportrait
	givemon SPECIES_MEWTWO_ARMORED, 80
	goto_if_eq VAR_RESULT, 2, MewtwoPostGame_EventScript_NoRoom
	playfanfare MUS_OBTAIN_ITEM
	message MewtwoPostGame_Text_ReceivedMewtwo
	waitmessage
	waitfanfare
	closemessage
	setflag FLAG_WOT_ARMORED_MEWTWO_GIVEN
	setspeaker SP_NAME_DRACO
	msgbox MewtwoPostGame_Text_DracoParting, MSGBOX_DEFAULT
	release
	end

MewtwoPostGame_EventScript_NoRoom::
	setspeaker SP_NAME_DRACO
	msgbox MewtwoPostGame_Text_DracoNoRoom, MSGBOX_DEFAULT
	release
	end

MewtwoPostGame_EventScript_DracoAfter::
	setspeaker SP_NAME_DRACO
	msgbox MewtwoPostGame_Text_DracoAfter, MSGBOX_DEFAULT
	release
	end

MewtwoPostGame_Text_Locked::
	.string "The dockhouse is sealed from\n"
	.string "the inside.$"

MewtwoPostGame_Text_DracoIntro::
	.string "You came. Good.\p"
	.string "MUTRID's labs had one project\n"
	.string "even THE ONI feared.\p"
	.string "A living weapon. Older than the\n"
	.string "machine. Angrier than the sea.\p"
	.string "The primitive lifeform --\n"
	.string "ARMORED MEWTWO.\p"
	.string "Show me true power unleashed!$"

MewtwoPostGame_Text_DracoDefeat::
	.string "…It lowered its head to you.\p"
	.string "It never did that for me.$"

MewtwoPostGame_Text_DracoHandOver::
	.string "A deal's a deal.\p"
	.string "The armor stays on -- it keeps\n"
	.string "the anger in.\p"
	.string "Take it.$"

MewtwoPostGame_Text_ReceivedMewtwo::
	.string "{PLAYER} received\n"
	.string "ARMORED MEWTWO!$"

MewtwoPostGame_Text_DracoNoRoom::
	.string "Your party and boxes are\n"
	.string "stuffed.\p"
	.string "Make room. This thing does\n"
	.string "not share a cage.$"

MewtwoPostGame_Text_DracoParting::
	.string "Treat it like a partner, not\n"
	.string "a weapon.\p"
	.string "That was MUTRID's mistake.$"

MewtwoPostGame_Text_DracoAfter::
	.string "It never obeyed me. Not once.\p"
	.string "But it follows you.\p"
	.string "…Maybe that's what true power\n"
	.string "unleashed actually is.$"
