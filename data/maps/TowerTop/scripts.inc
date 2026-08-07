	.equ LOCALID_TOWER_JIRACHI, 1
	.equ LOCALID_TOWER_ONI, 2

TowerTop_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, TowerTop_OnFrame
	.byte 0

@ ============================================================================
@  MUTRID TOWER -- THE SUMMIT (shell). The Shadow Core: the machine, powered
@  by SHADOW JIRACHI welded into its heart, and THE ONI beside it. Entering
@  plays his scene once (FLAG_WOT_ONI_MET): pure evil, finishing what his
@  father could not -- the perfect world. NO BATTLE YET -- Joe will author
@  his team in detail (Pic "Mutrid Leader" already exists for it).
@  Jirachi OW gfx = SPECIES(JIRACHI) placeholder until the custom Shadow
@  Jirachi sprites are imported. All dialogue PLACEHOLDER (Joe).
@ ============================================================================

TowerTop_OnFrame:
	map_script_2 VAR_TEMP_1, 0, TowerTop_EventScript_OniScene
	.2byte 0

TowerTop_EventScript_OniScene::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_set FLAG_WOT_ONI_MET, TowerTop_EventScript_OniSceneDone
	setflag FLAG_WOT_ONI_MET
	delay 60
	@ The machine's hum; JIRACHI's faint, exhausted cry inside the glass.
	waitse
	playmoncry SPECIES_JIRACHI, CRY_MODE_WEAK
	waitmoncry
	delay 30
	applymovement LOCALID_TOWER_ONI, TowerTop_Movement_OniFacePlayer
	waitmovement LOCALID_TOWER_ONI
	setspeaker SP_NAME_THE_ONI
	msgbox TowerTop_Text_Oni1, MSGBOX_DEFAULT
	msgbox TowerTop_Text_Oni2, MSGBOX_DEFAULT
	closemessage
	waitse
	playmoncry SPECIES_JIRACHI, CRY_MODE_WEAK
	waitmoncry
	setspeaker SP_NAME_THE_ONI
	msgbox TowerTop_Text_Oni3, MSGBOX_DEFAULT
	msgbox TowerTop_Text_Oni4, MSGBOX_DEFAULT
	msgbox TowerTop_Text_Oni5, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_TOWER_ONI, TowerTop_Movement_OniFaceMachine
	waitmovement LOCALID_TOWER_ONI
TowerTop_EventScript_OniSceneDone::
	releaseall
	end

TowerTop_EventScript_StairsDown::
	lockall
	warp MAP_TOWER_COMMAND, 7, 5
	waitstate
	releaseall
	end

@ Talking to him again after the scene. The battle comes later (Joe).
TowerTop_EventScript_Oni::
	lock
	faceplayer
	setspeaker SP_NAME_THE_ONI
	msgbox TowerTop_Text_OniAgain, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_TOWER_ONI, TowerTop_Movement_OniFaceMachine
	waitmovement LOCALID_TOWER_ONI
	release
	end

TowerTop_EventScript_Jirachi::
	lockall
	waitse
	playmoncry SPECIES_JIRACHI, CRY_MODE_WEAK
	waitmoncry
	msgbox TowerTop_Text_JirachiLook, MSGBOX_DEFAULT
	releaseall
	end

TowerTop_Movement_OniFacePlayer:
	face_down
	step_end

TowerTop_Movement_OniFaceMachine:
	face_up
	step_end

@ --- Text (all PLACEHOLDER (Joe)) -------------------------------------------
TowerTop_Text_Oni1::
	.string "You climbed all this way.\p"
	.string "Through my city. Past my\n"
	.string "captain. Up my tower.\p"
	.string "Good. Perfection should be\n"
	.string "witnessed before it is shared.$"

TowerTop_Text_Oni2::
	.string "My father dreamed of a better\n"
	.string "world. He flinched. He died\l"
	.string "small.\p"
	.string "I do not flinch.\p"
	.string "I am completing what he could\n"
	.string "not: the PERFECT world.$"

TowerTop_Text_Oni3::
	.string "Listen to it. The wish-granter\n"
	.string "itself, wired into my machine.\p"
	.string "Every wish it screams becomes\n"
	.string "another perfected soul.\p"
	.string "It wanted to grant wishes.\n"
	.string "I simply... never let it stop.$"

TowerTop_Text_Oni4::
	.string "POKéMON are imperfect.\p"
	.string "They hesitate. They love. They\n"
	.string "have WEAKNESSES.\p"
	.string "Shadows do not hesitate.\n"
	.string "Shadows do not weep.\p"
	.string "Shadows are STRONG.$"

TowerTop_Text_Oni5::
	.string "So choose, {PLAYER}.\p"
	.string "Come to the dark side...\p"
	.string "...or prepare to die.$"

TowerTop_Text_OniAgain::
	.string "Still breathing. Still\n"
	.string "choosing slowly.\p"
	.string "The machine is patient,\n"
	.string "{PLAYER}. I am less so.$"

TowerTop_Text_JirachiLook::
	.string "Inside the glass, JIRACHI's\n"
	.string "eyes are barely open.\p"
	.string "A thousand stolen wishes press\n"
	.string "against the seal.\p"
	.string "Not one of them is its own.$"
