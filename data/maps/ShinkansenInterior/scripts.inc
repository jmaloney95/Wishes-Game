	.set LOCALID_SHINKANSEN_GOON, 1

ShinkansenInterior_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ShinkansenInterior_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, ShinkansenInterior_OnResume
	.byte 0

ShinkansenInterior_OnResume::
	questindicator LOCALID_SHINKANSEN_GOON, SIDE_QUEST_FROZEN_TRACKS
	end

@ After the EMBER BADGE the train is gone from SennenVillage entirely (the cleared
@ track tile has no door behavior, so the door warp dies and this interior becomes
@ unreachable). Belt-and-braces: if it IS ever entered (debug warps, future edits),
@ make sure the goon is gone too. (Side effect: also marks the goon "defeated" for
@ the gym's re-talk dialogue branch -- harmless, since this can only run post-badge.)
ShinkansenInterior_OnTransition::
	call_if_set FLAG_BADGE01_GET, ShinkansenInterior_EventScript_HideGoon
	end

ShinkansenInterior_EventScript_HideGoon::
	setflag FLAG_HIDE_SHINKANSEN_GOON
	return

@ The masked goon (??? -- placeholder Team Rocket sprite/Magma-grunt battle pic; swap for the
@ oni-masked custom sprite later). Talk to him -> taunt -> battle (weak team) -> he flees.
ShinkansenInterior_EventScript_Goon::
	lock
	faceplayer
	setspeaker SP_NAME_GOON
	msgbox ShinkansenInterior_Text_GoonIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_SHINKANSEN_GOON, ShinkansenInterior_Text_GoonDefeat
	setspeaker SP_NAME_GOON
	msgbox ShinkansenInterior_Text_GoonAfter, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SHINKANSEN_GOON, ShinkansenInterior_Movement_GoonRunOff
	waitmovement 0
	setflag FLAG_HIDE_SHINKANSEN_GOON
	removeobject LOCALID_SHINKANSEN_GOON
	startquest SIDE_QUEST_FROZEN_TRACKS
	release
	end

ShinkansenInterior_Movement_GoonRunOff:
	walk_fast_down
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

@ Frozen control consoles. Locked until the EMBER BADGE thaws the system.
ShinkansenInterior_EventScript_Computer::
	lockall
	goto_if_set FLAG_BADGE01_GET, ShinkansenInterior_EventScript_ComputerFixed
	msgbox ShinkansenInterior_Text_ComputerFrozen, MSGBOX_DEFAULT
	releaseall
	end

ShinkansenInterior_EventScript_ComputerFixed::
	msgbox ShinkansenInterior_Text_ComputerFixed, MSGBOX_DEFAULT
	releaseall
	end

ShinkansenInterior_Text_GoonIntro:
	.string "…Took you long enough, kid.\p"
	.string "I jammed this old SHINKANSEN's gears\n"
	.string "with ice -- frozen solid as steel.\p"
	.string "Nobody rides the SENNEN LINE up the\n"
	.string "mountain now. Boss's orders.\p"
	.string "And you? You're just a brat who\n"
	.string "wandered where you shouldn't have.\p"
	.string "Beat it -- or I'll flatten you!$"

ShinkansenInterior_Text_GoonDefeat:
	.string "Tch…! Flattened by a kid…?$"

ShinkansenInterior_Text_GoonAfter:
	.string "Fine, FINE -- I'm gone!\p"
	.string "But good luck thawing those gears.\n"
	.string "The whole system's locked up tight.\p"
	.string "You'll NEVER get up that mountain!$"

ShinkansenInterior_Text_ComputerFrozen:
	.string "The control console is sheathed in\n"
	.string "ice. Every readout blinks ERROR.\p"
	.string "The SENNEN LINE's systems are frozen\n"
	.string "solid -- the train can't move an inch.$"

ShinkansenInterior_Text_ComputerFixed:
	.string "The console hums back to life.\p"
	.string "SENNEN LINE: now departing for\n"
	.string "SHIN-TOKYO. All systems nominal.$"

@ ============================================================================
@  ACT 2 BOARDING (added): once the keycard door opens at Sennen
@  (FLAG_HIDE_SHINKANSEN_ACT2_GOONS cleared), the car is full of masked
@  passengers who do NOT battle -- just unsettling smalltalk.
@  Taking the seat at the far end (coord triggers at 12,2 / 12,3) departs
@  the train: shake, fade to black, "ACT III: COOKED".
@  Seat/passenger coords are best-guess vs the 14x5 car -- nudge in Porymap.
@ ============================================================================
ShinkansenInterior_EventScript_Passenger1::
	lock
	faceplayer
	msgbox ShinkansenInterior_Text_Passenger1, MSGBOX_DEFAULT
	release
	end

ShinkansenInterior_EventScript_Passenger2::
	lock
	faceplayer
	msgbox ShinkansenInterior_Text_Passenger2, MSGBOX_DEFAULT
	release
	end

ShinkansenInterior_EventScript_Passenger3::
	lock
	faceplayer
	msgbox ShinkansenInterior_Text_Passenger3, MSGBOX_DEFAULT
	release
	end

ShinkansenInterior_EventScript_TakeSeat::
	goto_if_set FLAG_HIDE_SHINKANSEN_ACT2_GOONS, ShinkansenInterior_EventScript_SeatInert
	goto_if_set FLAG_ACT3_STARTED, ShinkansenInterior_EventScript_SeatInert
	setvar VAR_TEMP_2, 1
	lockall
	msgbox ShinkansenInterior_Text_TakeSeat, MSGBOX_DEFAULT
	closemessage
	delay 30
	playse SE_TRUCK_DOOR
	setvar VAR_0x8004, 1  @ vertical pan
	setvar VAR_0x8005, 1  @ horizontal pan
	setvar VAR_0x8006, 6  @ num shakes
	setvar VAR_0x8007, 5  @ shake delay
	special ShakeCamera
	waitstate
	msgbox ShinkansenInterior_Text_Departing1, MSGBOX_DEFAULT
	closemessage
	playse SE_TRUCK_MOVE
	setvar VAR_0x8004, 2  @ vertical pan
	setvar VAR_0x8005, 2  @ horizontal pan
	setvar VAR_0x8006, 12 @ num shakes
	setvar VAR_0x8007, 3  @ shake delay
	special ShakeCamera
	waitstate
	msgbox ShinkansenInterior_Text_Departing2, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	delay 60
	setflag FLAG_ACT3_STARTED
	msgbox ShinkansenInterior_Text_ActThree, MSGBOX_DEFAULT
	closemessage
	delay 40
	@ The ride is skipped: the title card holds on black while the train runs,
	@ then Act 3 opens on the rainy Shin-Tokyo platform (the arrival cutscene
	@ lives in ShinTokyoStation/scripts.inc).
	playse SE_TRUCK_MOVE
	delay 60
	warp MAP_SHIN_TOKYO_STATION, 9, 3
	waitstate
	releaseall
	end

ShinkansenInterior_EventScript_SeatInert::
	end

@ ============================================================================
@  Text (Act 2 boarding)
@ ============================================================================
ShinkansenInterior_Text_Passenger1::
	.string "The masked passenger doesn't turn\n"
	.string "its head.\p"
	.string "“Sit anywhere. Everyone gets\n"
	.string "where they're going, in the end.”$"

ShinkansenInterior_Text_Passenger2::
	.string "“First trip east? The lights of\n"
	.string "Shin-Tokyo never turn off.\p"
	.string "“The master says sleep is just\n"
	.string "practice for something lazier.”\p"
	.string "Behind the mask, something that\n"
	.string "is not laughter, laughs.$"

ShinkansenInterior_Text_Passenger3::
	.string "“Ticket? Kekeke. Nobody checks\n"
	.string "tickets anymore.\p"
	.string "“The train knows who it's\n"
	.string "carrying.”$"

ShinkansenInterior_Text_TakeSeat::
	.string "{PLAYER} sank into the last seat\n"
	.string "in the car.\p"
	.string "Through the window: smoke over\n"
	.string "Sennen, and tiny distant flashes\l"
	.string "of the battle still burning.\p"
	.string "Everything you have ever loved is\n"
	.string "on the wrong side of this glass.$"

ShinkansenInterior_Text_Departing1::
	.string "The doors hiss shut, one after\n"
	.string "another, all down the line.\p"
	.string "Somewhere ahead, the rails snap\n"
	.string "to GREEN -- all the way east.\p"
	.string "Minister kept his promise.$"

ShinkansenInterior_Text_Departing2::
	.string "The Shinkansen LURCHES--\p"
	.string "--and Sennen's platform lights\n"
	.string "tear away into long white\l"
	.string "streaks.\p"
	.string "No one on this train looks out\n"
	.string "the window. Except you.$"

ShinkansenInterior_Text_ActThree::
	.string "ACT III\p"
	.string "“THE INFILTRATION”$"

ShinkansenInterior_Text_TrainHum::
	.string "The train hums through the dark,\n"
	.string "eastbound.\p"
	.string "Next stop: Shin-Tokyo.$"
