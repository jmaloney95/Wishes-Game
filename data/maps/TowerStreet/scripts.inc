TowerStreet_MapScripts::
	.byte 0

@ ============================================================================
@  MUTRID TOWER -- STREET LEVEL (shell). One gatekeeper: THE WHITE TIGER,
@  first of the tower's guardian-beast duelists (Lv44-46, Shadow RAIKOU ace).
@  Ascent: district door (24,4) -> here -> Mid -> Command (Badge 8) -> Top.
@  All dialogue PLACEHOLDER (Joe).
@ ============================================================================

TowerStreet_EventScript_ExitToDistrict::
	lockall
	warp MAP_SHIN_TOKYO, 24, 5
	waitstate
	releaseall
	end

@ The stairwell gate only releases once the floor's guardian is down.
TowerStreet_EventScript_StairsUp::
	lockall
	goto_if_not_defeated TRAINER_BLAKE, TowerStreet_EventScript_StairsSealed
	warp MAP_TOWER_MID, 6, 10
	waitstate
	releaseall
	end

TowerStreet_EventScript_StairsSealed::
	msgbox TowerStreet_Text_StairsSealed, MSGBOX_DEFAULT
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, TowerStreet_Movement_StepBack
	waitmovement 0
	setvar VAR_TEMP_8, 0
	releaseall
	end

TowerStreet_Movement_StepBack:
	walk_down
	step_end

TowerStreet_EventScript_WhiteTiger::
	trainerbattle_single TRAINER_BLAKE, TowerStreet_Text_WhiteTigerIntro, TowerStreet_Text_WhiteTigerDefeat
	msgbox TowerStreet_Text_WhiteTigerPost, MSGBOX_AUTOCLOSE
	call Wot_EventScript_SnagProgress
	end

TowerStreet_Text_WhiteTigerIntro::
	.string "The tower keeps four beasts.\p"
	.string "I am the WHITE TIGER -- the\n"
	.string "claw at its foot.\p"
	.string "The thunder in my ranks was\n"
	.string "sealed screaming. It fights\l"
	.string "for me anyway.\p"
	.string "Come and be clawed.$"

TowerStreet_Text_WhiteTigerDefeat::
	.string "...The claw... broke.$"

TowerStreet_Text_WhiteTigerPost::
	.string "Take the stairs.\p"
	.string "The bird above sings prettier\n"
	.string "than I growl. It burns hotter,\l"
	.string "too.$"

TowerStreet_Text_StairsSealed::
	.string "The stairwell gate is locked to\n"
	.string "floor security.\p"
	.string "The floor's guardian is still\n"
	.string "standing.$"
