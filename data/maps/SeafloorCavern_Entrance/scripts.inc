SeafloorCavern_Entrance_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, SeafloorCavern_Entrance_OnResume
	map_script MAP_SCRIPT_ON_TRANSITION, SeafloorCavern_Entrance_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SeafloorCavern_Entrance_OnFrame
	.byte 0

SeafloorCavern_Entrance_OnResume:
	setdivewarp MAP_UNDERWATER_SEAFLOOR_CAVERN, 6, 5
	setescapewarp MAP_UNDERWATER_SEAFLOOR_CAVERN, 6, 5
	call_if_set FLAG_SYS_CTRL_OBJ_DELETE, SeafloorCavern_Entrance_EventScript_TryRemoveCaughtGyarados
	end

@ When the Red Gyarados is caught, the field reloads with FLAG_SYS_CTRL_OBJ_DELETE
@ still set. Remove it permanently in that case.
SeafloorCavern_Entrance_EventScript_TryRemoveCaughtGyarados::
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, Common_EventScript_NopReturn
	setflag FLAG_SEAFLOOR_GYARADOS_GONE
	setflag FLAG_SEAFLOOR_GYARADOS_CAUGHT
	setflag FLAG_HIDE_SEAFLOOR_RED_GYARADOS
	setflag FLAG_HIDE_SEAFLOOR_FISHERMAN
	removeobject LOCALID_SEAFLOOR_CAVERN_ENTRANCE_GYARADOS
	removeobject LOCALID_SEAFLOOR_CAVERN_ENTRANCE_FISHERMAN
	return

@ Decide the Gyarados visibility, and arm the entry cutscene on first visit.
@ VAR_TEMP_1 resets to 0 on every map load; we set it to 1 here when the
@ ritual still needs to play, and the OnFrame table fires on VAR_TEMP_1 == 1.
SeafloorCavern_Entrance_OnTransition:
	goto_if_set FLAG_SEAFLOOR_GYARADOS_GONE, SeafloorCavern_Entrance_EventScript_HideGyaradosOnLoad
	goto_if_set FLAG_SEAFLOOR_RITUAL_SEEN, SeafloorCavern_Entrance_EventScript_ShowGyaradosOnLoad
	@ ritual not played yet -> hide Gyarados and arm the cutscene
	setflag FLAG_HIDE_SEAFLOOR_RED_GYARADOS
	setvar VAR_TEMP_1, 1
	end

SeafloorCavern_Entrance_EventScript_HideGyaradosOnLoad:
	setflag FLAG_HIDE_SEAFLOOR_RED_GYARADOS
	end

SeafloorCavern_Entrance_EventScript_ShowGyaradosOnLoad:
	clearflag FLAG_HIDE_SEAFLOOR_RED_GYARADOS
	end

SeafloorCavern_Entrance_OnFrame:
	map_script_2 VAR_TEMP_1, 1, SeafloorCavern_Entrance_EventScript_RitualCutscene
	.2byte 0

@ ===========================================================================
@ Entry cutscene: pan to the fisherman mid-ritual, the Red Gyarados surfaces,
@ the fisherman gasps in awe.
@ ===========================================================================
SeafloorCavern_Entrance_EventScript_RitualCutscene::
	lockall
	delay 30
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, SeafloorCavern_Entrance_Movement_CameraToFisherman
	waitmovement 0
	delay 30
	applymovement LOCALID_SEAFLOOR_CAVERN_ENTRANCE_FISHERMAN, Common_Movement_FaceDown
	waitmovement 0
	msgbox SeafloorCavern_Entrance_Text_RitualMumble, MSGBOX_DEFAULT
	closemessage
	delay 30
	@ reveal the Gyarados in the water
	clearflag FLAG_HIDE_SEAFLOOR_RED_GYARADOS
	addobject LOCALID_SEAFLOOR_CAVERN_ENTRANCE_GYARADOS
	applymovement LOCALID_CAMERA, SeafloorCavern_Entrance_Movement_CameraToGyarados
	waitmovement 0
	playse SE_M_DIVE
	delay 20
	waitse
	playmoncry SPECIES_GYARADOS, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	delay 30
	@ back to the fisherman, who gasps
	applymovement LOCALID_CAMERA, SeafloorCavern_Entrance_Movement_CameraToFishermanFromGyarados
	waitmovement 0
	delay 10
	playse SE_PIN
	applymovement LOCALID_SEAFLOOR_CAVERN_ENTRANCE_FISHERMAN, Common_Movement_ExclamationMark
	waitmovement 0
	delay 20
	msgbox SeafloorCavern_Entrance_Text_FishermanGasp, MSGBOX_DEFAULT
	closemessage
	delay 10
	applymovement LOCALID_CAMERA, SeafloorCavern_Entrance_Movement_CameraToPlayer
	waitmovement 0
	special RemoveCameraObject
	setflag FLAG_SEAFLOOR_RITUAL_SEEN
	setvar VAR_TEMP_1, 0
	releaseall
	end

@ ===========================================================================
@ The Red Gyarados - static legendary-style battle.
@ KO'd or caught -> gone forever. Run/lost -> stays for a rematch.
@ ===========================================================================
SeafloorCavern_Entrance_EventScript_RedGyarados::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_GYARADOS, CRY_MODE_ENCOUNTER
	delay 40
	waitmoncry
	setvar VAR_LAST_TALKED, LOCALID_SEAFLOOR_CAVERN_ENTRANCE_GYARADOS
	setwildbattle SPECIES_GYARADOS, 20
	special SetSeafloorGyaradosShiny
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, SeafloorCavern_Entrance_EventScript_GyaradosDefeated
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, SeafloorCavern_Entrance_EventScript_GyaradosCaught
	@ ran away, lost, or teleported -> Gyarados remains, can be rebattled
	release
	end

SeafloorCavern_Entrance_EventScript_GyaradosDefeated::
	setflag FLAG_SEAFLOOR_GYARADOS_GONE
	setflag FLAG_HIDE_SEAFLOOR_RED_GYARADOS
	setflag FLAG_HIDE_SEAFLOOR_FISHERMAN
	removeobject LOCALID_SEAFLOOR_CAVERN_ENTRANCE_FISHERMAN
	goto Common_EventScript_RemoveStaticPokemon
	end

SeafloorCavern_Entrance_EventScript_GyaradosCaught::
	setflag FLAG_SEAFLOOR_GYARADOS_GONE
	setflag FLAG_SEAFLOOR_GYARADOS_CAUGHT
	setflag FLAG_HIDE_SEAFLOOR_RED_GYARADOS
	setflag FLAG_HIDE_SEAFLOOR_FISHERMAN
	removeobject LOCALID_SEAFLOOR_CAVERN_ENTRANCE_FISHERMAN
	release
	end

@ ===========================================================================
@ The fisherman - chickens out and hands over his 5 Ultra Balls (one-time).
@ ===========================================================================
SeafloorCavern_Entrance_EventScript_Fisherman::
	lock
	faceplayer
	goto_if_set FLAG_SEAFLOOR_FISHERMAN_GAVE_BALLS, SeafloorCavern_Entrance_EventScript_FishermanAfter
	msgbox SeafloorCavern_Entrance_Text_FishermanChickensOut, MSGBOX_DEFAULT
	giveitem ITEM_ULTRA_BALL, 5
	setflag FLAG_SEAFLOOR_FISHERMAN_GAVE_BALLS
	msgbox SeafloorCavern_Entrance_Text_FishermanGoodLuck, MSGBOX_DEFAULT
	release
	end

SeafloorCavern_Entrance_EventScript_FishermanAfter::
	msgbox SeafloorCavern_Entrance_Text_FishermanAfter, MSGBOX_DEFAULT
	release
	end

@ ===========================================================================
@ Camera movements (camera starts on the player at the 10,1 entrance)
@ ===========================================================================
SeafloorCavern_Entrance_Movement_CameraToFisherman:
	walk_left
	walk_left
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

SeafloorCavern_Entrance_Movement_CameraToGyarados:
	walk_right
	walk_right
	walk_right
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

SeafloorCavern_Entrance_Movement_CameraToFishermanFromGyarados:
	walk_left
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

SeafloorCavern_Entrance_Movement_CameraToPlayer:
	walk_right
	walk_right
	walk_right
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

@ ===========================================================================
@ Text
@ ===========================================================================
SeafloorCavern_Entrance_Text_RitualMumble:
	.string "Fisherman: …O great gods of the deep…\n"
	.string "…hear my humble plea…\p"
	.string "Grant unto me the ULTIMATE CATCH,\n"
	.string "the crimson serpent of legend…\p"
	.string "…by tide and scale, I beg of thee…$"

SeafloorCavern_Entrance_Text_FishermanGasp:
	.string "Fisherman: Wha-?!\p"
	.string "I-It… it WORKED?!\n"
	.string "The waters… they're churning red!\p"
	.string "The crimson serpent… it's REAL!$"

SeafloorCavern_Entrance_Text_FishermanChickensOut:
	.string "Fisherman: You see it too, right?!\n"
	.string "That… that's a RED GYARADOS!\p"
	.string "I prayed for it my whole life, but…\n"
	.string "now that it's here, I- I can't!\p"
	.string "Those eyes! That roar! I'd be lucky\n"
	.string "to keep my arms, let alone catch it!\p"
	.string "You- you take these. Please.\n"
	.string "I can't bear to look at it!$"

SeafloorCavern_Entrance_Text_FishermanGoodLuck:
	.string "Fisherman: ULTRA BALLS. Five of 'em.\n"
	.string "Saved 'em for this very moment…\p"
	.string "and I'm too chicken to throw a one.\p"
	.string "Go on. Land the catch I never could.$"

SeafloorCavern_Entrance_Text_FishermanAfter:
	.string "Fisherman: That thing still gives me\n"
	.string "the shivers…\p"
	.string "Show it who's boss for me, would you?$"
