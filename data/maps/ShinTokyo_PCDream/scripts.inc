@ ============================================================================
@  THE GIRATINA DREAM -- Shin Tokyo's blue Pokemon Center, adrift in the dark.
@  Reached only through Tradewind_PC's exit while FLAG_WOT_GIRATINA_DREAM_ARMED
@  is set (the nurse whose words came out blank). The building is the real
@  one's 5x5 block, copied out of the Shin Tokyo layout; everything else is a
@  blank metatile over the tileset's black backdrop.
@  Walking out into the dark (the trigger row at y=8) wakes the thing the
@  Distortion World answers to: a Lv40 SHADOW GIRATINA. Only CATCHING it ends
@  the dream for good (a KO counts the same as running). A KO, a run or a
@  whiteout wakes the player at the nurse with the real exit back; talking to
@  her again re-arms it. The PC's door here wakes them the same way.
@  State flags are documented in constants/flags.h.
@
@ ============================================================================

ShinTokyo_PCDream_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ShinTokyo_PCDream_OnTransition
	.byte 0

ShinTokyo_PCDream_OnTransition::
	@ A whiteout in here comes back to the nurse, not the hideout cots.
	setrespawn HEAL_LOCATION_SHIN_TOKYO_PC
	setflag FLAG_TEMP_HIDE_FOLLOWER
	@ Giratina only exists inside its scene (temp hide flags clear on load).
	setflag FLAG_TEMP_1
	end

ShinTokyo_PCDream_EventScript_Giratina::
	lockall
	setvar VAR_TEMP_1, 1
	fadeoutbgm 4
	delay 30
	playse SE_THUNDER
	setvar VAR_0x8004, 2  @ vertical pan
	setvar VAR_0x8005, 2  @ horizontal pan
	setvar VAR_0x8006, 12 @ num shakes
	setvar VAR_0x8007, 4  @ shake delay
	special ShakeCamera
	waitstate
	msgbox ShinTokyo_PCDream_Text_Rift, MSGBOX_DEFAULT
	closemessage
	@ It surfaces four tiles below wherever the player stopped, so the
	@ approach always plays out on screen (the mirage pattern).
	getplayerxy VAR_0x8004, VAR_0x8005
	addvar VAR_0x8005, 4
	clearflag FLAG_TEMP_1
	addobject LOCALID_DREAM_GIRATINA
	setobjectxy LOCALID_DREAM_GIRATINA, VAR_0x8004, VAR_0x8005
	applymovement LOCALID_DREAM_GIRATINA, ShinTokyo_PCDream_Movement_Rise
	waitmovement 0
	playse SE_PIN
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_ExclamationMark
	waitmovement 0
	waitse
	playmoncry SPECIES_GIRATINA, CRY_MODE_ENCOUNTER
	setvar VAR_0x8004, 3
	setvar VAR_0x8005, 3
	setvar VAR_0x8006, 16
	setvar VAR_0x8007, 3
	special ShakeCamera
	waitstate
	waitmoncry
	msgbox ShinTokyo_PCDream_Text_GiratinaRises, MSGBOX_DEFAULT
	closemessage
	@ FIGHTING is set BEFORE the battle so Tradewind_PC's ON_TRANSITION can
	@ tell a whiteout from the door; the whiteout heal then ends the dream.
	setflag FLAG_WOT_GIRATINA_DREAM_FIGHTING
	setwildbattle SPECIES_GIRATINA, 40
	special WotMakeEventMonShadow
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, ShinTokyo_PCDream_EventScript_Caught
	@ -- KO, ran, teleported: back in front of the nurse, awake. The exit is
	@ the real city again; her next heal re-arms the dream. A whiteout never
	@ gets here: Tradewind_PC_EventScript_DreamWhiteOutHeal does the same. --
	clearflag FLAG_WOT_GIRATINA_DREAM_FIGHTING
	clearflag FLAG_WOT_GIRATINA_DREAM_ARMED
	msgbox ShinTokyo_PCDream_Text_Recedes, MSGBOX_DEFAULT
	closemessage
	warpwhitefade MAP_TRADEWIND_PC, 7, 4
	waitstate
	releaseall
	end

ShinTokyo_PCDream_EventScript_Caught::
	@ Set before the warp: script contexts die at warps.
	setflag FLAG_WOT_GIRATINA_DREAM_CAUGHT
	msgbox ShinTokyo_PCDream_Text_Waking, MSGBOX_DEFAULT
	closemessage
	warpwhitefade MAP_TRADEWIND_PC, 7, 4
	waitstate
	releaseall
	end

ShinTokyo_PCDream_Movement_Rise:
	walk_slow_up
	walk_slow_up
	step_end

ShinTokyo_PCDream_Text_Rift::
	.string "The ground under you folds the\n"
	.string "wrong way.\p"
	.string "You know this feeling. The bottom\n"
	.string "of the rift felt exactly like this.$"

ShinTokyo_PCDream_Text_GiratinaRises::
	.string "Something vast uncoils out of\n"
	.string "the dark…\p"
	.string "and its shadow is the wrong\n"
	.string "colour.$"

ShinTokyo_PCDream_Text_Recedes::
	.string "Giratina sinks back into the\n"
	.string "dark. It isn't finished with you.$"

ShinTokyo_PCDream_Text_Waking::
	.string "The dark peels away like\n"
	.string "wet paper…$"
