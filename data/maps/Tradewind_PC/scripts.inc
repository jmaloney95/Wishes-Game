@ ============================================================================
@  SHIN TOKYO'S BLUE POKEMON CENTER -- and the Giratina dream.
@  Players met a bug here: the nurse's words came out blank. It is the door
@  into a dream now. Healing here leaves her box blank and ARMS the dream:
@  the exit opens onto ShinTokyo_PCDream (this building alone in the dark),
@  where Shadow Giratina waits. Catching it wakes the player back in here
@  with the counter empty; the exit after that is the real city again.
@  State flags are documented in constants/flags.h. The exits are
@  MAP_DYNAMIC, aimed on every load (transition AND resume, so a save made in
@  here never walks out through a stale dynamic warp from Hotel Z).
@
@ ============================================================================
	.set SHINTOKYO_PC_DOOR_WARP, 6  @ Shin Tokyo's warp onto this door

Tradewind_PC_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Tradewind_PC_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, Tradewind_PC_EventScript_AimExit
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Tradewind_PC_OnFrame
	.byte 0

Tradewind_PC_OnTransition::
	goto_if_unset FLAG_WOT_GIRATINA_DREAM_ARMED, Tradewind_PC_EventScript_AimExit
	goto_if_set FLAG_WOT_GIRATINA_DREAM_CAUGHT, Tradewind_PC_EventScript_Woke
	goto_if_set FLAG_WOT_GIRATINA_DREAM_FIGHTING, Tradewind_PC_EventScript_StillDreaming
	@ Armed with neither: the only way in is the dream's own door, so the
	@ player walked away from Giratina. They wake; the next heal re-arms it.
	clearflag FLAG_WOT_GIRATINA_DREAM_ARMED
	goto Tradewind_PC_EventScript_AimExit

@ Back from a whiteout (a KO or a run already disarmed, in the dream's script):
@ ARMED stays set just long enough for EventScript_AfterWhiteOutHeal to pick
@ the blank nurse, and Tradewind_PC_EventScript_DreamWhiteOutHeal clears it.
Tradewind_PC_EventScript_StillDreaming::
	clearflag FLAG_WOT_GIRATINA_DREAM_FIGHTING
	goto Tradewind_PC_EventScript_AimExit

@ Caught: the counter is empty this one visit (temp flag), and the dream is over.
Tradewind_PC_EventScript_Woke::
	clearflag FLAG_WOT_GIRATINA_DREAM_ARMED
	clearflag FLAG_WOT_GIRATINA_DREAM_FIGHTING
	setflag FLAG_TEMP_1
	setvar VAR_TEMP_1, 1
	goto Tradewind_PC_EventScript_AimExit

Tradewind_PC_EventScript_AimExit::
	goto_if_set FLAG_WOT_GIRATINA_DREAM_ARMED, Tradewind_PC_EventScript_AimDream
	setdynamicwarp MAP_SHIN_TOKYO, SHINTOKYO_PC_DOOR_WARP
	end

Tradewind_PC_EventScript_AimDream::
	setdynamicwarp MAP_SHIN_TOKYO_PC_DREAM, 0
	end

Tradewind_PC_OnFrame:
	map_script_2 VAR_TEMP_1, 1, Tradewind_PC_EventScript_WakeUp
	.2byte 0

Tradewind_PC_EventScript_WakeUp::
	lockall
	setvar VAR_TEMP_1, 2
	delay 30
	msgbox Tradewind_PC_Text_WakeUp, MSGBOX_DEFAULT
	releaseall
	end

@ Until Giratina is caught, every heal here is the blank one, and it arms
@ the dream. Afterwards she is an ordinary nurse.
Tradewind_PC_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_SHINTOKYO_PC_NURSE
	goto_if_set FLAG_WOT_GIRATINA_DREAM_CAUGHT, Tradewind_PC_EventScript_NurseNormal
	lock
	faceplayer
	msgbox Tradewind_PC_Text_Blank, MSGBOX_DEFAULT
	incrementgamestat GAME_STAT_USED_POKECENTER
	call EventScript_PkmnCenterNurse_TakeAndHealPkmn
	msgbox Tradewind_PC_Text_Blank, MSGBOX_DEFAULT
	applymovement VAR_0x800B, Movement_PkmnCenterNurse_Bow
	waitmovement 0
	msgbox Tradewind_PC_Text_Blank, MSGBOX_DEFAULT
	setflag FLAG_WOT_GIRATINA_DREAM_ARMED
	setdynamicwarp MAP_SHIN_TOKYO_PC_DREAM, 0
	release
	end

Tradewind_PC_EventScript_NurseNormal::
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

@ A whiteout inside the dream lands here (HEAL_LOCATION_SHIN_TOKYO_PC):
@ EventScript_AfterWhiteOutHeal branches in while ARMED is set (FIGHTING is
@ already cleared by ON_TRANSITION, which runs first), so the nurse who
@ heals you is the blank one too. The dream ends here: the exit is the real
@ city until she is spoken to again.
Tradewind_PC_EventScript_DreamWhiteOutHeal::
	lockall
	clearflag FLAG_WOT_GIRATINA_DREAM_ARMED
	clearflag FLAG_WOT_GIRATINA_DREAM_FIGHTING
	setdynamicwarp MAP_SHIN_TOKYO, SHINTOKYO_PC_DOOR_WARP
	msgbox Tradewind_PC_Text_Blank, MSGBOX_DEFAULT
	call EventScript_PkmnCenterNurse_TakeAndHealPkmn
	msgbox Tradewind_PC_Text_Blank, MSGBOX_DEFAULT
	applymovement VAR_LAST_TALKED, Movement_PkmnCenterNurse_Bow
	waitmovement 0
	fadedefaultbgm
	releaseall
	end

@ Deliberately empty: the box opens, waits for A, and says nothing.
Tradewind_PC_Text_Blank::
	.string "$"

Tradewind_PC_Text_WakeUp::
	.string "…The counter is empty.\p"
	.string "The lights hum like nothing\n"
	.string "happened.\p"
	.string "But there's a Poké Ball on your\n"
	.string "belt you don't remember throwing.$"
