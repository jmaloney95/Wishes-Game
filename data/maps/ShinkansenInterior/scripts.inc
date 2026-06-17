	.set LOCALID_SHINKANSEN_GOON, 1

ShinkansenInterior_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ShinkansenInterior_OnTransition
	.byte 0

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
	msgbox ShinkansenInterior_Text_GoonIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_SHINKANSEN_GOON, ShinkansenInterior_Text_GoonDefeat
	msgbox ShinkansenInterior_Text_GoonAfter, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SHINKANSEN_GOON, ShinkansenInterior_Movement_GoonRunOff
	waitmovement 0
	setflag FLAG_HIDE_SHINKANSEN_GOON
	removeobject LOCALID_SHINKANSEN_GOON
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
