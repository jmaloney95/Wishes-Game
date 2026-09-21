SennenVillage_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, SennenVillage_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, SennenVillage_OnTransition
	.byte 0

@ Whiteout respawn: make the train-hub village its own heal location (setrespawn
@ only writes save data, so it is safe in ON_TRANSITION -- no warp involved).
SennenVillage_OnTransition::
	setrespawn HEAL_LOCATION_SENNEN_VILLAGE
	end

@ Single-map design: SennenVillage is the train-present base map. Once the EMBER
@ BADGE (FLAG_BADGE01_GET) is earned, ON_LOAD repaints the cleared-track tiles
@ (the exact diff vs the SennenVillage_2 reference). MUST be ON_LOAD, not
@ ON_TRANSITION -- setmetatile only sticks after the layout buffer loads, and
@ warping during ON_TRANSITION crashes ("leaving script while a warp is in
@ progress"). No map/warp swap needed: same map, same warps, tiles just change.
SennenVillage_OnLoad::
	goto_if_set FLAG_BADGE01_GET, SennenVillage_EventScript_ClearTrain
	end

SennenVillage_EventScript_ClearTrain::
	setmetatile 13, 10, 368, 1
	setmetatile 14, 10, 368, 1
	setmetatile 15, 10, 368, 1
	setmetatile 16, 10, 368, 1
	setmetatile 17, 10, 368, 1
	setmetatile 18, 10, 368, 1
	setmetatile 19, 10, 368, 1
	setmetatile 20, 10, 368, 1
	setmetatile 21, 10, 368, 1
	setmetatile 22, 10, 368, 1
	setmetatile 23, 10, 322, 1
	setmetatile 24, 10, 71, 0
	setmetatile 25, 10, 207, 0
	setmetatile 26, 10, 207, 0
	setmetatile 27, 10, 69, 0
	setmetatile 28, 10, 198, 1
	setmetatile 29, 10, 198, 1
	setmetatile 30, 10, 71, 0
	setmetatile 31, 10, 207, 0
	setmetatile 32, 10, 207, 0
	setmetatile 33, 10, 69, 0
	setmetatile 34, 10, 322, 1
	setmetatile 35, 10, 368, 1
	setmetatile 36, 10, 368, 1
	setmetatile 37, 10, 368, 1
	setmetatile 38, 10, 368, 1
	setmetatile 39, 10, 368, 1
	setmetatile 40, 10, 368, 1
	setmetatile 41, 10, 368, 1
	setmetatile 42, 10, 368, 1
	setmetatile 43, 10, 368, 1
	setmetatile 44, 10, 368, 1
	setmetatile 45, 10, 368, 1
	setmetatile 46, 10, 368, 1
	setmetatile 47, 10, 368, 1
	setmetatile 48, 10, 368, 1
	setmetatile 49, 10, 368, 1
	setmetatile 12, 11, 651, 1
	setmetatile 13, 11, 651, 1
	setmetatile 14, 11, 651, 1
	setmetatile 15, 11, 651, 1
	setmetatile 16, 11, 651, 1
	setmetatile 17, 11, 651, 1
	setmetatile 18, 11, 651, 0
	setmetatile 19, 11, 651, 0
	setmetatile 20, 11, 651, 0
	setmetatile 21, 11, 651, 0
	setmetatile 22, 11, 651, 0
	setmetatile 23, 11, 651, 0
	setmetatile 24, 11, 651, 0
	setmetatile 25, 11, 651, 0
	setmetatile 26, 11, 651, 0
	setmetatile 27, 11, 651, 0
	setmetatile 28, 11, 651, 0
	setmetatile 29, 11, 651, 0
	setmetatile 30, 11, 651, 0
	setmetatile 31, 11, 651, 0
	setmetatile 32, 11, 651, 0
	setmetatile 33, 11, 651, 0
	setmetatile 34, 11, 651, 0
	setmetatile 35, 11, 651, 0
	setmetatile 36, 11, 651, 0
	setmetatile 37, 11, 651, 0
	setmetatile 38, 11, 651, 0
	setmetatile 39, 11, 651, 1
	setmetatile 40, 11, 651, 1
	setmetatile 41, 11, 651, 1
	setmetatile 42, 11, 651, 1
	setmetatile 43, 11, 651, 1
	setmetatile 44, 11, 651, 1
	setmetatile 45, 11, 651, 1
	setmetatile 46, 11, 651, 1
	setmetatile 47, 11, 651, 1
	setmetatile 48, 11, 651, 1
	setmetatile 49, 11, 651, 1
	setmetatile 12, 12, 665, 1
	setmetatile 13, 12, 665, 1
	setmetatile 14, 12, 665, 1
	setmetatile 15, 12, 665, 1
	setmetatile 16, 12, 665, 1
	setmetatile 17, 12, 665, 1
	setmetatile 18, 12, 665, 0
	setmetatile 19, 12, 665, 0
	setmetatile 20, 12, 665, 0
	setmetatile 21, 12, 665, 0
	setmetatile 22, 12, 665, 0
	setmetatile 23, 12, 665, 0
	setmetatile 24, 12, 665, 0
	setmetatile 25, 12, 665, 0
	setmetatile 26, 12, 665, 0
	setmetatile 27, 12, 665, 0
	setmetatile 28, 12, 665, 0
	setmetatile 29, 12, 665, 0
	setmetatile 30, 12, 665, 0
	setmetatile 31, 12, 665, 0
	setmetatile 32, 12, 665, 0
	setmetatile 33, 12, 665, 0
	setmetatile 34, 12, 665, 0
	setmetatile 35, 12, 665, 0
	setmetatile 36, 12, 665, 0
	setmetatile 37, 12, 665, 0
	setmetatile 38, 12, 665, 0
	setmetatile 39, 12, 665, 1
	setmetatile 40, 12, 665, 1
	setmetatile 41, 12, 665, 1
	setmetatile 42, 12, 665, 1
	setmetatile 43, 12, 665, 1
	setmetatile 44, 12, 665, 1
	setmetatile 45, 12, 665, 1
	setmetatile 46, 12, 665, 1
	setmetatile 47, 12, 665, 1
	setmetatile 48, 12, 665, 1
	setmetatile 49, 12, 665, 1
	setmetatile 34, 13, 669, 1
	setmetatile 35, 13, 670, 0
	setmetatile 36, 13, 678, 0
	setmetatile 37, 13, 679, 0
	setmetatile 38, 13, 678, 0
	setmetatile 39, 13, 678, 1
	setmetatile 40, 13, 678, 1
	setmetatile 41, 13, 678, 1
	setmetatile 42, 13, 678, 1
	setmetatile 43, 13, 678, 1
	setmetatile 44, 13, 678, 1
	setmetatile 45, 13, 678, 1
	setmetatile 46, 13, 678, 1
	setmetatile 47, 13, 678, 1
	setmetatile 48, 13, 678, 1
	setmetatile 49, 13, 678, 1
	setmetatile 37, 14, 650, 0
	setmetatile 38, 14, 651, 0
	setmetatile 39, 14, 651, 1
	setmetatile 40, 14, 651, 1
	setmetatile 41, 14, 651, 1
	setmetatile 42, 14, 651, 1
	setmetatile 43, 14, 651, 1
	setmetatile 44, 14, 651, 1
	setmetatile 45, 14, 651, 1
	setmetatile 46, 14, 651, 1
	setmetatile 47, 14, 651, 1
	setmetatile 48, 14, 651, 1
	setmetatile 49, 14, 651, 1
	setmetatile 39, 15, 664, 1
	setmetatile 40, 15, 664, 1
	setmetatile 41, 15, 664, 1
	setmetatile 42, 15, 664, 1
	setmetatile 43, 15, 664, 1
	setmetatile 44, 15, 664, 1
	setmetatile 45, 15, 664, 1
	setmetatile 46, 15, 664, 1
	setmetatile 47, 15, 664, 1
	setmetatile 48, 15, 664, 1
	setmetatile 49, 15, 664, 1
	end

SennenVillage_EventScript_LineSign::
	lockall
	msgbox SennenVillage_Text_LineSign, MSGBOX_DEFAULT
	releaseall
	end

SennenVillage_Text_LineSign:
	.string "SENNEN LINE\p"
	.string "Northbound: SENNEN -> SHIN-TOKYO\p"
	.string "NOTICE: the SHINKANSEN bullet train\n"
	.string "is CLOSED until further notice.$"
