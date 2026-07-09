SennenVillage_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, SennenVillage_OnLoad
	.byte 0

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
	setmetatile 8, 9, 368, 1
	setmetatile 9, 9, 368, 1
	setmetatile 10, 9, 368, 1
	setmetatile 11, 9, 368, 1
	setmetatile 12, 9, 368, 1
	setmetatile 13, 9, 368, 1
	setmetatile 14, 9, 368, 1
	setmetatile 15, 9, 368, 1
	setmetatile 16, 9, 368, 1
	setmetatile 17, 9, 368, 1
	setmetatile 18, 9, 322, 1
	setmetatile 19, 9, 71, 0
	setmetatile 20, 9, 207, 0
	setmetatile 21, 9, 207, 0
	setmetatile 22, 9, 69, 0
	setmetatile 23, 9, 198, 1
	setmetatile 24, 9, 198, 1
	setmetatile 25, 9, 71, 0
	setmetatile 26, 9, 207, 0
	setmetatile 27, 9, 207, 0
	setmetatile 28, 9, 69, 0
	setmetatile 29, 9, 322, 1
	setmetatile 30, 9, 368, 1
	setmetatile 31, 9, 368, 1
	setmetatile 32, 9, 368, 1
	setmetatile 33, 9, 368, 1
	setmetatile 34, 9, 368, 1
	setmetatile 0, 10, 651, 0
	setmetatile 1, 10, 651, 0
	setmetatile 2, 10, 651, 0
	setmetatile 3, 10, 651, 0
	setmetatile 4, 10, 651, 0
	setmetatile 5, 10, 651, 0
	setmetatile 6, 10, 651, 0
	setmetatile 7, 10, 651, 0
	setmetatile 8, 10, 651, 0
	setmetatile 9, 10, 651, 0
	setmetatile 10, 10, 651, 0
	setmetatile 11, 10, 651, 0
	setmetatile 12, 10, 651, 0
	setmetatile 13, 10, 651, 0
	setmetatile 14, 10, 651, 0
	setmetatile 15, 10, 651, 0
	setmetatile 16, 10, 651, 0
	setmetatile 17, 10, 651, 0
	setmetatile 18, 10, 651, 0
	setmetatile 19, 10, 651, 0
	setmetatile 20, 10, 651, 0
	setmetatile 21, 10, 651, 0
	setmetatile 22, 10, 651, 0
	setmetatile 23, 10, 651, 0
	setmetatile 24, 10, 651, 0
	setmetatile 25, 10, 651, 0
	setmetatile 26, 10, 651, 0
	setmetatile 27, 10, 651, 0
	setmetatile 28, 10, 651, 0
	setmetatile 29, 10, 651, 0
	setmetatile 30, 10, 651, 0
	setmetatile 31, 10, 651, 0
	setmetatile 32, 10, 651, 0
	setmetatile 33, 10, 651, 0
	setmetatile 34, 10, 651, 0
	setmetatile 0, 11, 665, 0
	setmetatile 1, 11, 665, 0
	setmetatile 2, 11, 665, 0
	setmetatile 3, 11, 665, 0
	setmetatile 4, 11, 665, 0
	setmetatile 5, 11, 665, 0
	setmetatile 6, 11, 665, 0
	setmetatile 7, 11, 665, 0
	setmetatile 8, 11, 665, 0
	setmetatile 9, 11, 665, 0
	setmetatile 10, 11, 665, 0
	setmetatile 11, 11, 665, 0
	setmetatile 12, 11, 665, 0
	setmetatile 13, 11, 665, 0
	setmetatile 14, 11, 665, 0
	setmetatile 15, 11, 665, 0
	setmetatile 16, 11, 665, 0
	setmetatile 17, 11, 665, 0
	setmetatile 18, 11, 665, 0
	setmetatile 19, 11, 665, 0
	setmetatile 20, 11, 665, 0
	setmetatile 21, 11, 665, 0
	setmetatile 22, 11, 665, 0
	setmetatile 23, 11, 665, 0
	setmetatile 24, 11, 665, 0
	setmetatile 25, 11, 665, 0
	setmetatile 26, 11, 665, 0
	setmetatile 27, 11, 665, 0
	setmetatile 28, 11, 665, 0
	setmetatile 29, 11, 665, 0
	setmetatile 30, 11, 665, 0
	setmetatile 31, 11, 665, 0
	setmetatile 32, 11, 665, 0
	setmetatile 33, 11, 665, 0
	setmetatile 34, 11, 665, 0
	setmetatile 29, 12, 669, 1
	setmetatile 30, 12, 670, 0
	setmetatile 31, 12, 678, 0
	setmetatile 32, 12, 679, 0
	setmetatile 33, 12, 678, 0
	setmetatile 34, 12, 679, 0
	setmetatile 32, 13, 650, 0
	setmetatile 33, 13, 651, 0
	setmetatile 33, 14, 665, 0
	setmetatile 33, 15, 679, 0
	end

SennenVillage_EventScript_LineSign::
	lockall
	msgbox SennenVillage_Text_LineSign, MSGBOX_DEFAULT
	releaseall
	end

SennenVillage_Text_LineSign:
	.string "SENNEN LINE\p"
	.string "Northbound:  SENNEN -> SHIN-TOKYO\p"
	.string "NOTICE: the SHINKANSEN bullet train\n"
	.string "is CLOSED until further notice.$"
