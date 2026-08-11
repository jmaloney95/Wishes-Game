	.equ LOCALID_TOWER_JIRACHI, 1
	.equ LOCALID_TOWER_ONI, 2
	.equ LOCALID_TOWER_JET, 3
	.equ LOCALID_TOWER_RAYQUAZA, 4

TowerTop_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, TowerTop_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, TowerTop_OnLoad
	map_script MAP_SCRIPT_ON_RESUME, TowerTop_OnResume
	map_script MAP_SCRIPT_ON_FRAME_TABLE, TowerTop_OnFrame
	.byte 0

@ Shadow Jirachi floats in the machine's glass. ON_RESUME (not ON_FRAME) so
@ the bob also resumes after a battle return; the task no-ops itself once the
@ object is gone, and re-arming it while it runs is a no-op.
TowerTop_OnResume::
	setvar VAR_0x8004, LOCALID_TOWER_JIRACHI
	special WotStartObjectBob
	end

@ The jet + released legendaries only ever exist mid-cutscene.
TowerTop_OnTransition::
	setflag FLAG_WOT_TOWER_LEGENDS_AWAY
	end

@ Once Shadow Jirachi is caught the machine stays broken forever.
TowerTop_OnLoad::
	call_if_set FLAG_WOT_TOWER_JIRACHI_GONE, TowerTop_EventScript_SetBrokenMachine
	end

TowerTop_EventScript_SetBrokenMachine::
	setmetatile 7, 1, 0x3C8, TRUE
	setmetatile 8, 2, 0x3F2, TRUE
	setmetatile 7, 3, 0x3C8, TRUE
	return

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
	@ "...or prepare to die." -- the final battle, right here.
	call TowerTop_EventScript_OniBattle
TowerTop_EventScript_OniSceneDone::
	releaseall
	end

@ Shared by the intro scene and the rematch talk (after a whiteout). Returns
@ only on victory -- then the whole escape beat plays.
TowerTop_EventScript_OniBattle::
	trainerbattle_no_intro TRAINER_WALLY_VR_5, TowerTop_Text_OniDefeat
	@ the storm's ambient SE would fight the scene audio (12ch or not)
	special WotSuppressWeatherSE
	setspeaker SP_NAME_THE_ONI
	msgbox TowerTop_Text_OniFlee, MSGBOX_DEFAULT
	closemessage
	@ Pan to the roofline FIRST -- the back edge (13,2) is off-camera from
	@ anywhere the player can stand, so without this the boarding happens
	@ where nobody can see it.
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, TowerTop_Movement_CameraUpToRoof
	waitmovement 0
	delay 12
	@ out the back: right 5, up 4 -> (13,2)
	applymovement LOCALID_TOWER_ONI, TowerTop_Movement_OniRunOut
	waitmovement LOCALID_TOWER_ONI
	delay 16
	@ the jet screams across the roofline and swallows him
	clearflag FLAG_WOT_TOWER_LEGENDS_AWAY
	addobject LOCALID_TOWER_JET
	playse SE_M_RAZOR_WIND
	applymovement LOCALID_TOWER_JET, TowerTop_Movement_JetSwoopIn
	waitmovement LOCALID_TOWER_JET
	setflag FLAG_WOT_ONI_DEFEATED
	removeobject LOCALID_TOWER_ONI
	applymovement LOCALID_TOWER_JET, TowerTop_Movement_JetSwoopOut
	waitmovement LOCALID_TOWER_JET
	removeobject LOCALID_TOWER_JET
	setflag FLAG_WOT_TOWER_LEGENDS_AWAY
	delay 24
	applymovement LOCALID_CAMERA, TowerTop_Movement_CameraBackDown
	waitmovement 0
	special RemoveCameraObject
	return

TowerTop_EventScript_StairsDown::
	lockall
	warp MAP_TOWER_COMMAND, 7, 5
	waitstate
	releaseall
	end

@ Talking to him again after the scene (i.e. after a whiteout): straight back
@ into the fight.
TowerTop_EventScript_Oni::
	lock
	faceplayer
	setspeaker SP_NAME_THE_ONI
	msgbox TowerTop_Text_OniAgain, MSGBOX_DEFAULT
	closemessage
	call TowerTop_EventScript_OniBattle
	release
	end

TowerTop_EventScript_Jirachi::
	lockall
	goto_if_set FLAG_WOT_ONI_DEFEATED, TowerTop_EventScript_JirachiFree
	waitse
	playmoncry SPECIES_JIRACHI, CRY_MODE_WEAK
	waitmoncry
	msgbox TowerTop_Text_JirachiLook, MSGBOX_DEFAULT
	releaseall
	end

@ The Oni is gone; the wish-granter hangs unguarded in the dying machine.
TowerTop_EventScript_JirachiFree::
	waitse
	playmoncry SPECIES_JIRACHI, CRY_MODE_ENCOUNTER
	waitmoncry
	msgbox TowerTop_Text_JirachiUnbound, MSGBOX_DEFAULT
	closemessage
	setwildbattle SPECIES_JIRACHI, 70
	special WotMakeEventMonShadow
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, TowerTop_EventScript_Overload
	releaseall
	end

@ ============================================================================
@  THE OVERLOAD -- Shadow Jirachi is caught, the machine loses its heart.
@  Shakes, breaking tiles (0x3C8/0x3F2), the captive legendaries burst free
@  and scatter, then the credits. GameClear saves via the Hall of Fame and
@  chains into CB2_StartCreditsSequence; Continue resumes at the hideout.
@ ============================================================================
TowerTop_EventScript_Overload::
	setflag FLAG_WOT_TOWER_JIRACHI_GONE
	special WotSuppressWeatherSE
	msgbox TowerTop_Text_MachineOverload, MSGBOX_DEFAULT
	closemessage
	@ Pan up: the machine's upper housing (rows 1-3) is off-camera from the
	@ Jirachi tile, and that is exactly where it tears itself apart.
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, TowerTop_Movement_CameraUpToMachine
	waitmovement 0
	delay 20
	@ -- the housing bursts open, one panel at a time --
	playse SE_BREAKABLE_DOOR
	setmetatile 7, 1, 0x3C8, TRUE
	special DrawWholeMapView
	delay 22
	playse SE_MUD_BALL
	setmetatile 8, 2, 0x3F2, TRUE
	special DrawWholeMapView
	delay 22
	playse SE_BREAKABLE_DOOR
	setmetatile 7, 3, 0x3C8, TRUE
	special DrawWholeMapView
	delay 26
	@ -- and then the whole core lets go --
	playse SE_M_EXPLOSION
	setvar VAR_0x8004, 8  @ vertical pan
	setvar VAR_0x8005, 3  @ horizontal pan
	setvar VAR_0x8006, 12 @ num shakes
	setvar VAR_0x8007, 3  @ shake delay
	special WotShakeCameraSilent
	waitstate
	delay 20
	@ -- the greatest of the caged things tears free --
	clearflag FLAG_WOT_TOWER_LEGENDS_AWAY
	addobject LOCALID_TOWER_RAYQUAZA
	waitse
	playmoncry SPECIES_RAYQUAZA, CRY_MODE_ENCOUNTER
	waitmoncry
	delay 20
	applymovement LOCALID_TOWER_RAYQUAZA, TowerTop_Movement_RayquazaScatter
	waitmovement LOCALID_TOWER_RAYQUAZA
	removeobject LOCALID_TOWER_RAYQUAZA
	setflag FLAG_WOT_TOWER_LEGENDS_AWAY
	removeobject LOCALID_TOWER_JIRACHI
	@ ...a beat of empty sky...
	delay 70
	applymovement LOCALID_CAMERA, TowerTop_Movement_CameraBackFromMachine
	waitmovement 0
	special RemoveCameraObject
	msgbox TowerTop_Text_TheEnd, MSGBOX_DEFAULT
	closemessage
	@ -- confetti over the freed city, then the credits --
	special WotStartFieldConfetti
	playse SE_APPLAUSE
	delay 180
	fadescreenspeed FADE_TO_BLACK, 8
	special WotStopFieldConfetti
	special GameClear
	releaseall
	end

TowerTop_Movement_OniFacePlayer:
	face_down
	step_end

TowerTop_Movement_OniFaceMachine:
	face_up
	step_end

@ From (8,6): right 5, up 4 -> (13,2), the back edge of the roof.
TowerTop_Movement_OniRunOut:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

@ Jet template (15,2): two tiles left puts it over the Oni at (13,2).
TowerTop_Movement_JetSwoopIn:
	walk_faster_left
	walk_faster_left
	step_end

TowerTop_Movement_JetSwoopOut:
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	step_end

@ Rayquaza climbs straight up out of the broken tower into open sky.
TowerTop_Movement_RayquazaScatter:
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	step_end

@ Camera work. The player can only ever stand around y=6-11, so both the
@ jet boarding (y=2) and the machine housing (y=1-3) need a lift.
TowerTop_Movement_CameraUpToRoof:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

TowerTop_Movement_CameraBackDown:
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	step_end

TowerTop_Movement_CameraUpToMachine:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

TowerTop_Movement_CameraBackFromMachine:
	walk_fast_down
	walk_fast_down
	walk_fast_down
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

TowerTop_Text_OniDefeat::
	.string "...Imperfect.\p"
	.string "How are you all so strong\n"
	.string "while staying so WEAK?$"

TowerTop_Text_OniFlee::
	.string "Keep the tower. Keep the city.\p"
	.string "A perfect world doesn't need\n"
	.string "a throne. Only a seed.\p"
	.string "We will finish this, {PLAYER}.$"

TowerTop_Text_JirachiUnbound::
	.string "With the ONI gone, the seal is\n"
	.string "thin as breath.\p"
	.string "SHADOW JIRACHI turns. For the\n"
	.string "first time, it looks AT you.\p"
	.string "It has one wish left.\n"
	.string "It wishes to be caught.$"

TowerTop_Text_MachineOverload::
	.string "The machine SCREAMS.\p"
	.string "A thousand stolen wishes have\n"
	.string "nowhere left to go!$"

TowerTop_Text_TheEnd::
	.string "The generator splits open like\n"
	.string "a seed pod.\p"
	.string "Wings. Claws. Light. Everything\n"
	.string "the tower ever swallowed pours\l"
	.string "back into the world.\p"
	.string "The storm over SHIN-TOKYO\n"
	.string "breaks. Somewhere below, the\l"
	.string "Rebellion is cheering.\p"
	.string "Every wish is its own again.$"
