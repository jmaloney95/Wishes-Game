TowerMid_MapScripts::
	.byte 0

@ ============================================================================
@  MUTRID TOWER -- UPPER FLOORS (shell). One gatekeeper: SUZAKU, the vermilion
@  bird of the tower's guardian-beast duelists (Lv49-51, Shadow HO-OH ace).
@  All dialogue PLACEHOLDER (Joe).
@ ============================================================================

TowerMid_EventScript_StairsDown::
	lockall
	warp MAP_TOWER_STREET, 4, 4
	waitstate
	releaseall
	end

@ The stairwell gate only releases once the floor's guardian is down.
TowerMid_EventScript_StairsUp::
	lockall
	goto_if_not_defeated TRAINER_SYLVIA, TowerMid_EventScript_StairsSealed
	warp MAP_TOWER_COMMAND, 7, 12
	waitstate
	releaseall
	end

TowerMid_EventScript_StairsSealed::
	msgbox TowerMid_Text_StairsSealed, MSGBOX_DEFAULT
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, TowerMid_Movement_StepBack
	waitmovement 0
	setvar VAR_TEMP_8, 0
	releaseall
	end

TowerMid_Movement_StepBack:
	walk_down
	step_end

TowerMid_EventScript_Suzaku::
	trainerbattle_single TRAINER_SYLVIA, TowerMid_Text_SuzakuIntro, TowerMid_Text_SuzakuDefeat
	msgbox TowerMid_Text_SuzakuPost, MSGBOX_AUTOCLOSE
	call Wot_EventScript_SnagProgress
	end

TowerMid_Text_SuzakuIntro::
	.string "So the TIGER's claw broke.\p"
	.string "I am SUZAKU. Every wing in\n"
	.string "this tower answers my call --\p"
	.string "even the rainbow one they\n"
	.string "dragged out of the sky and\l"
	.string "sealed in the dark.\p"
	.string "Burn with me.$"

TowerMid_Text_SuzakuDefeat::
	.string "...Even the rainbow...\n"
	.string "guttered out...$"

TowerMid_Text_SuzakuPost::
	.string "The CAPTAIN holds the floor\n"
	.string "above. Nobody holds harder.\p"
	.string "Go. My feathers are ash.$"

TowerMid_Text_StairsSealed::
	.string "The stairwell gate is locked to\n"
	.string "floor security.\p"
	.string "The floor's guardian is still\n"
	.string "standing.$"
