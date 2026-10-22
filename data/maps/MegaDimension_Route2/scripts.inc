@ The Mega Dimension copy of Route 2: Route 2's layout, none of its events.
@ Everything here is shared Mega Dimension behaviour (data/scripts/mega_dimension.inc);
@ a new dimension map only needs this same header.

MegaDimension_Route2_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MegaDim_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, MegaDim_OnResume
	map_script MAP_SCRIPT_ON_FRAME_TABLE, MegaDim_OnFrame
	.byte 0
