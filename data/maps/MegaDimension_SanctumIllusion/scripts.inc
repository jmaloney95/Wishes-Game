@ A Rift Sanctum: reached through a partner's barrier in the Mega
@ Dimension. Everything here is shared (data/scripts/mega_dimension.inc); the
@ objects are placed in the map and can be moved in porymap.

MegaDimension_SanctumIllusion_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Sanctum_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, Sanctum_OnResume
	map_script MAP_SCRIPT_ON_FRAME_TABLE, MegaDim_OnFrame
	.byte 0
