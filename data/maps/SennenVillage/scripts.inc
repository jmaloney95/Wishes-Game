SennenVillage_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, SennenVillage_OnLoad
	.byte 0

@ Once the EMBER BADGE thaws the SHINKANSEN's gears, the goon's ice melts off the
@ tracks. We keep ONE SennenVillage map and just paint in the cleared-track tiles.
@ MUST be ON_LOAD (not ON_TRANSITION): setmetatile only sticks after the layout
@ buffer is loaded -- same as vanilla's Mirage Tower / Sootopolis door scripts.
@ The setmetatile calls below are the exact diff vs. the SennenVillage_2 layout.
SennenVillage_OnLoad::
	goto_if_set FLAG_BADGE01_GET, SennenVillage_EventScript_ClearTrain
	end

SennenVillage_EventScript_ClearTrain::
	setmetatile 14, 0, 672, 0
	setmetatile 15, 0, 672, 0
	setmetatile 8, 3, 368, 1
	setmetatile 9, 3, 368, 1
	setmetatile 10, 3, 368, 1
	setmetatile 11, 3, 368, 1
	setmetatile 12, 3, 368, 1
	setmetatile 13, 3, 368, 1
	setmetatile 14, 3, 368, 1
	setmetatile 15, 3, 368, 1
	setmetatile 16, 3, 368, 1
	setmetatile 17, 3, 368, 1
	setmetatile 18, 3, 322, 1
	setmetatile 19, 3, 71, 0
	setmetatile 20, 3, 207, 0
	setmetatile 21, 3, 207, 0
	setmetatile 22, 3, 69, 0
	setmetatile 23, 3, 198, 1
	setmetatile 24, 3, 198, 1
	setmetatile 25, 3, 71, 0
	setmetatile 26, 3, 207, 0
	setmetatile 27, 3, 207, 0
	setmetatile 28, 3, 69, 0
	setmetatile 29, 3, 322, 1
	setmetatile 30, 3, 368, 1
	setmetatile 31, 3, 368, 1
	setmetatile 32, 3, 368, 1
	setmetatile 33, 3, 368, 1
	setmetatile 34, 3, 368, 1
	setmetatile 0, 4, 651, 0
	setmetatile 1, 4, 651, 0
	setmetatile 2, 4, 651, 0
	setmetatile 3, 4, 651, 0
	setmetatile 4, 4, 651, 0
	setmetatile 5, 4, 651, 0
	setmetatile 6, 4, 651, 0
	setmetatile 7, 4, 651, 0
	setmetatile 8, 4, 651, 0
	setmetatile 9, 4, 651, 0
	setmetatile 10, 4, 651, 0
	setmetatile 11, 4, 651, 0
	setmetatile 12, 4, 651, 0
	setmetatile 13, 4, 651, 0
	setmetatile 14, 4, 651, 0
	setmetatile 15, 4, 651, 0
	setmetatile 16, 4, 651, 0
	setmetatile 17, 4, 651, 0
	setmetatile 18, 4, 651, 0
	setmetatile 19, 4, 651, 0
	setmetatile 20, 4, 651, 0
	setmetatile 21, 4, 651, 0
	setmetatile 22, 4, 651, 0
	setmetatile 23, 4, 651, 0
	setmetatile 24, 4, 651, 0
	setmetatile 25, 4, 651, 0
	setmetatile 26, 4, 651, 0
	setmetatile 27, 4, 651, 0
	setmetatile 28, 4, 651, 0
	setmetatile 29, 4, 651, 0
	setmetatile 30, 4, 651, 0
	setmetatile 31, 4, 651, 0
	setmetatile 32, 4, 651, 0
	setmetatile 33, 4, 651, 0
	setmetatile 34, 4, 651, 0
	setmetatile 0, 5, 665, 0
	setmetatile 1, 5, 665, 0
	setmetatile 2, 5, 665, 0
	setmetatile 3, 5, 665, 0
	setmetatile 4, 5, 665, 0
	setmetatile 5, 5, 665, 0
	setmetatile 6, 5, 665, 0
	setmetatile 7, 5, 665, 0
	setmetatile 8, 5, 665, 0
	setmetatile 9, 5, 665, 0
	setmetatile 10, 5, 665, 0
	setmetatile 11, 5, 665, 0
	setmetatile 12, 5, 665, 0
	setmetatile 13, 5, 665, 0
	setmetatile 14, 5, 665, 0
	setmetatile 15, 5, 665, 0
	setmetatile 16, 5, 665, 0
	setmetatile 17, 5, 665, 0
	setmetatile 18, 5, 665, 0
	setmetatile 19, 5, 665, 0
	setmetatile 20, 5, 665, 0
	setmetatile 21, 5, 665, 0
	setmetatile 22, 5, 665, 0
	setmetatile 23, 5, 665, 0
	setmetatile 24, 5, 665, 0
	setmetatile 25, 5, 665, 0
	setmetatile 26, 5, 665, 0
	setmetatile 27, 5, 665, 0
	setmetatile 28, 5, 665, 0
	setmetatile 29, 5, 665, 0
	setmetatile 30, 5, 665, 0
	setmetatile 31, 5, 665, 0
	setmetatile 32, 5, 665, 0
	setmetatile 33, 5, 665, 0
	setmetatile 34, 5, 665, 0
	setmetatile 29, 6, 669, 1
	setmetatile 30, 6, 670, 0
	setmetatile 31, 6, 678, 0
	setmetatile 32, 6, 679, 0
	setmetatile 33, 6, 678, 0
	setmetatile 34, 6, 679, 0
	setmetatile 32, 7, 650, 0
	setmetatile 33, 7, 651, 0
	setmetatile 33, 8, 665, 0
	setmetatile 33, 9, 679, 0
	setmetatile 5, 19, 289, 0
	setmetatile 6, 19, 289, 0
	setmetatile 9, 19, 290, 0
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
