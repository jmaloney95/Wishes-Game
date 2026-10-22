@ The Mega Dimension copy of MarineCave_End for Mable's Primal Kyogre: its layout,
@ none of its events. The legend stands where the original did; the old
@ trigger in front of it now starts the legend battle.

MegaDimension_MarineCave_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MegaDim_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, MegaDim_OnResume
	map_script MAP_SCRIPT_ON_FRAME_TABLE, MegaDim_OnFrame
	.byte 0

MegaDimension_MarineCave_EventScript_Boss::
	setvar VAR_LAST_TALKED, LOCALID_MEGADIM_MARINECAVE_BOSS
	goto MegaDim_EventScript_Mega
