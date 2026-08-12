ShinkansenInterior_Act2_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ShinkansenInterior_Act2_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, ShinkansenInterior_Act2_OnFrame
	.byte 0

@ Boarding with the driver's-cab keycard reveals the act-2 passengers and
@ arms the seat (ShinkansenInterior_EventScript_TakeSeat bails while
@ FLAG_HIDE_SHINKANSEN_ACT2_GOONS is set). The station door is a plain map
@ warp (SennenAct2 19,15 -- the old scripted door was retired), so this
@ ON_TRANSITION does the reveal for every way in.
ShinkansenInterior_Act2_OnTransition::
	checkitem ITEM_SHINKANSEN_KEY
	goto_if_eq VAR_RESULT, FALSE, ShinkansenInterior_Act2_OnTransition_End
	clearflag FLAG_HIDE_SHINKANSEN_ACT2_GOONS
ShinkansenInterior_Act2_OnTransition_End::
	end

@ Without the Minister's keycard the car must not be boardable: the plain
@ warp can't gate items, so the first frame inside bounces the player back
@ onto the platform with a pointer at the fix (the Minister in the Dojo).
ShinkansenInterior_Act2_OnFrame:
	map_script_2 VAR_TEMP_0, 0, ShinkansenInterior_Act2_EventScript_NoKeyEject
	.2byte 0

ShinkansenInterior_Act2_EventScript_NoKeyEject::
	lockall
	setvar VAR_TEMP_0, 1
	checkitem ITEM_SHINKANSEN_KEY
	goto_if_eq VAR_RESULT, TRUE, ShinkansenInterior_Act2_EventScript_NoKeyEjectEnd
	msgbox ShinkansenInterior_Act2_Text_NoKey, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	warp MAP_SENNEN_ACT2, 19, 15
	waitstate
ShinkansenInterior_Act2_EventScript_NoKeyEjectEnd::
	releaseall
	end

ShinkansenInterior_Act2_Text_NoKey::
	.string "The car is sealed and dark.\p"
	.string "A reader by the door blinks red:\n"
	.string "“KEYCARD REQUIRED.”\p"
	.string "The MINISTER at the dojo was\n"
	.string "holding the rebellion's card.$"
