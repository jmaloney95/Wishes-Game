	.equ LOCALID_RYUDEN_WARDEN_A, 3
	.equ LOCALID_RYUDEN_WARDEN_B, 4

DragonKeeper_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DragonKeeper_OnTransition
	.byte 0

@ ============================================================================
@  RYUDEN ISLAND -- the dragon keepers' shrine settlement. Two DRAGON WARDENS
@  guard the incubator; stepping up to the eggs summons them from their homes
@  for a 2-on-1 dragon double battle. Victory earns ONE egg of the three --
@  a random dragon line (pool in src/wot_ryuden.c).
@ ============================================================================

DragonKeeper_OnTransition::
	goto_if_set FLAG_WOT_DRAGON_WARDENS_BEATEN, DragonKeeper_OnTransition_Beaten
	setflag FLAG_HIDE_RYUDEN_WARDENS
	end
DragonKeeper_OnTransition_Beaten::
	clearflag FLAG_HIDE_RYUDEN_WARDENS
	end

DragonKeeper_EventScript_EggTrigger::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_set FLAG_WOT_DRAGON_WARDENS_BEATEN, DragonKeeper_EventScript_EggTriggerDone
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, DragonKeeper_Movement_Exclaim
	waitmovement 0
	playse SE_DOOR
	addobject LOCALID_RYUDEN_WARDEN_A
	playse SE_DOOR
	addobject LOCALID_RYUDEN_WARDEN_B
	applymovement LOCALID_RYUDEN_WARDEN_A, DragonKeeper_Movement_WardenAIn
	applymovement LOCALID_RYUDEN_WARDEN_B, DragonKeeper_Movement_WardenBIn
	waitmovement LOCALID_RYUDEN_WARDEN_A
	waitmovement LOCALID_RYUDEN_WARDEN_B
	msgbox DragonKeeper_Text_WardenChallenge, MSGBOX_DEFAULT
	closemessage
	trainerbattle_two_trainers TRAINER_AMY_AND_LIV_3, DragonKeeper_Text_WardenALose, TRAINER_GINA_AND_MIA_2, DragonKeeper_Text_WardenBLose
	setflag FLAG_WOT_DRAGON_WARDENS_BEATEN
	clearflag FLAG_HIDE_RYUDEN_WARDENS
	msgbox DragonKeeper_Text_WardenYield, MSGBOX_DEFAULT
DragonKeeper_EventScript_EggTriggerDone::
	releaseall
	end

DragonKeeper_Movement_Exclaim:
	emote_exclamation_mark
	delay_16
	step_end

@ Warden A emerges at (10,11) and crosses to flank the incubator's west side.
DragonKeeper_Movement_WardenAIn:
	walk_down
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	face_right
	step_end

@ Warden B emerges at (25,9) and crosses to flank the east side.
DragonKeeper_Movement_WardenBIn:
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	face_left
	step_end

DragonKeeper_EventScript_WardenA::
	lock
	faceplayer
	msgbox DragonKeeper_Text_WardenAPost, MSGBOX_DEFAULT
	release
	end

DragonKeeper_EventScript_WardenB::
	lock
	faceplayer
	msgbox DragonKeeper_Text_WardenBPost, MSGBOX_DEFAULT
	release
	end

@ --- the incubator -----------------------------------------------------------
DragonKeeper_EventScript_EggIncubator::
	lockall
	goto_if_set FLAG_WOT_RYUDEN_EGG_TAKEN, DragonKeeper_EventScript_EggGone
	goto_if_unset FLAG_WOT_DRAGON_WARDENS_BEATEN, DragonKeeper_EventScript_EggGuarded
	msgbox DragonKeeper_Text_TakeEgg, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, DragonKeeper_EventScript_EggNo
@ The pool lives in src/wot_ryuden.c (gWotRyudenEggPool). The egg only counts
@ as taken once it was actually delivered (party or PC).
	special WotGiveRyudenEgg
	goto_if_eq VAR_RESULT, MON_CANT_GIVE, DragonKeeper_EventScript_EggNoRoom
	goto DragonKeeper_EventScript_EggTaken

DragonKeeper_EventScript_EggNoRoom::
	msgbox DragonKeeper_Text_EggNoRoom, MSGBOX_DEFAULT
	releaseall
	end

DragonKeeper_EventScript_EggTaken::
	setflag FLAG_WOT_RYUDEN_EGG_TAKEN
	msgbox DragonKeeper_Text_EggTaken, MSGBOX_DEFAULT
	releaseall
	end

DragonKeeper_EventScript_EggNo::
	msgbox DragonKeeper_Text_EggNo, MSGBOX_DEFAULT
	releaseall
	end

DragonKeeper_EventScript_EggGuarded::
	msgbox DragonKeeper_Text_EggGuarded, MSGBOX_SIGN
	releaseall
	end

DragonKeeper_EventScript_EggGone::
	msgbox DragonKeeper_Text_EggGone, MSGBOX_SIGN
	releaseall
	end

@ --- island dragons ----------------------------------------------------------
DragonKeeper_EventScript_MonFlygon::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_FLYGON, CRY_MODE_NORMAL
	waitmoncry
	msgbox DragonKeeper_Text_MonFlygon, MSGBOX_DEFAULT
	release
	end

DragonKeeper_EventScript_MonBagon::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_BAGON, CRY_MODE_NORMAL
	waitmoncry
	msgbox DragonKeeper_Text_MonBagon, MSGBOX_DEFAULT
	release
	end

@ --- Text -------------------------------------------
DragonKeeper_Text_WardenChallenge::
	.string "You reach for the eggs…\p"
	.string "TWO shadows fall across you.\p"
	.string "“Outsider. Those clutches carry\n"
	.string "the old blood.\p"
	.string "“Prove your worth to the wardens\n"
	.string "of Ryuden, or leave empty.”$"

DragonKeeper_Text_WardenALose::
	.string "The old blood... accepts you.$"

DragonKeeper_Text_WardenBLose::
	.string "Even the sky-serpent bows...$"

DragonKeeper_Text_WardenYield::
	.string "“It is decided.\p"
	.string "“ONE egg. Choose with your\n"
	.string "heart, the shell already\l"
	.string "knows what sleeps inside.”$"

DragonKeeper_Text_WardenAPost::
	.string "Raise it under open sky.\p"
	.string "Dragons remember the roof\n"
	.string "they hatched beneath.$"

DragonKeeper_Text_WardenBPost::
	.string "The eggs choose their moment.\n"
	.string "Yours chose you.$"

DragonKeeper_Text_TakeEgg::
	.string "Three eggs rest in the warm\n"
	.string "ash of the incubator.\p"
	.string "Take one?$"

DragonKeeper_Text_EggTaken::
	.string "{PLAYER} received a DRAGON EGG!\p"
	.string "It is warm... and something\n"
	.string "enormous turns inside.$"

DragonKeeper_Text_EggNoRoom::
	.string "There is no room to carry an\n"
	.string "egg. Make space and return.$"

DragonKeeper_Text_EggNo::
	.string "The eggs settle back into\n"
	.string "the ash.$"

DragonKeeper_Text_EggGuarded::
	.string "Three great eggs rest in the\n"
	.string "warm ash.\p"
	.string "You feel watched.$"

DragonKeeper_Text_EggGone::
	.string "Two eggs remain, promised\n"
	.string "to other hands.$"

DragonKeeper_Text_MonFlygon::
	.string "FLYGON hums a low note.\p"
	.string "The eggs hum back.$"

DragonKeeper_Text_MonBagon::
	.string "BAGON headbutts the incubator\n"
	.string "wall, dreaming of the sky.$"

@ Surfing off the south shore crosses to Route 130 (and on to Prototype
@ Island / Celebi Island beyond it).
DragonKeeper_EventScript_SurfToRoute130::
	lockall
	warp MAP_ROUTE130, 10, 1
	waitstate
	releaseall
	end
