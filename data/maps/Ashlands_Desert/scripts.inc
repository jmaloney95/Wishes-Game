.set LOCALID_DESERT_MIRAGE, 1

Ashlands_Desert_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Ashlands_Desert_OnTransition
	.byte 0

Ashlands_Desert_OnTransition::
	setflag FLAG_HIDE_ASHLANDS_MIRAGE
	end

@ --- west seam -> Ashlands_2 (east side comes in via the map connection) -----
Ashlands_Desert_EventScript_SeamWest5::
	warp MAP_ASHLANDS_2, 24, 5
	waitstate
	end

Ashlands_Desert_EventScript_SeamWest6::
	warp MAP_ASHLANDS_2, 24, 6
	waitstate
	end

Ashlands_Desert_EventScript_SeamWest7::
	warp MAP_ASHLANDS_2, 24, 7
	waitstate
	end

Ashlands_Desert_EventScript_SeamWest8::
	warp MAP_ASHLANDS_2, 24, 8
	waitstate
	end

@ ============================================================================
@  THE MIRAGE -- crossing the middle of the maze (trigger column x=20), a
@  shape rips across the heat-haze: SHADOW JIRACHI, gone before it can be
@  believed. One-time foreshadowing beat. The actor spawns relative to the
@  player so it always crosses THEIR screen (applymovement ignores walls).
@ ============================================================================
Ashlands_Desert_EventScript_Mirage::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_set FLAG_WOT_MIRAGE_SEEN, Ashlands_Desert_EventScript_MirageDone
	setflag FLAG_WOT_MIRAGE_SEEN
	delay 30
	getplayerxy VAR_0x8004, VAR_0x8005
	subvar VAR_0x8004, 8
	subvar VAR_0x8005, 2
	clearflag FLAG_HIDE_ASHLANDS_MIRAGE
	addobject LOCALID_DESERT_MIRAGE
	setobjectxy LOCALID_DESERT_MIRAGE, VAR_0x8004, VAR_0x8005
	delay 8
	waitse
	playmoncry SPECIES_JIRACHI, CRY_MODE_ENCOUNTER
	applymovement LOCALID_DESERT_MIRAGE, Ashlands_Desert_Movement_MirageGlide
	waitmovement 0
	waitmoncry
	removeobject LOCALID_DESERT_MIRAGE
	setflag FLAG_HIDE_ASHLANDS_MIRAGE
	delay 24
	msgbox Ashlands_Desert_Text_Mirage, MSGBOX_DEFAULT
	closemessage
Ashlands_Desert_EventScript_MirageDone::
	releaseall
	end

Ashlands_Desert_Movement_MirageGlide:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

Ashlands_Desert_Text_Mirage::
	.string "Wait, was that...\p"
	.string "...\p"
	.string "Couldn't be...could it?$"
