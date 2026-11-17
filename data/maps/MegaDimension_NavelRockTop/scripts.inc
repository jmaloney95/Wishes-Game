@ The Mega Dimension copy of NavelRock_Top for Mable's Mega Ho-Oh: its layout,
@ none of its events. The legend waits at the top of the climb; the step
@ two tiles below it starts the battle, as the original's trigger did.

MegaDimension_NavelRockTop_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MegaDim_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, MegaDim_OnResume
	map_script MAP_SCRIPT_ON_FRAME_TABLE, MegaDim_OnFrame
	.byte 0

MegaDimension_NavelRockTop_EventScript_Boss::
	setvar VAR_LAST_TALKED, LOCALID_MEGADIM_NAVELTOP_BOSS
	goto MegaDim_EventScript_Mega
