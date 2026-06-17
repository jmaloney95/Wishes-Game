WALNUT_WOODS_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, WALNUT_WOODS_OnLoad
	.byte 0

@ Restore already-collected items / already-cut bushes to snow on map load.
WALNUT_WOODS_OnLoad::
	call_if_set FLAG_ITEM_WALNUT_WOODS_TM_BLIZZARD, WALNUT_WOODS_EventScript_SetBlizzardSnow
	call_if_set FLAG_ITEM_WALNUT_WOODS_TM_PROTECT, WALNUT_WOODS_EventScript_SetProtectSnow
	call_if_set FLAG_WALNUT_WOODS_CUT_BUSH_1, WALNUT_WOODS_EventScript_SetBush1Snow
	call_if_set FLAG_WALNUT_WOODS_CUT_BUSH_2, WALNUT_WOODS_EventScript_SetBush2Snow
	end

@ setmetatile helpers (snow ground that matches each spot's surroundings)
WALNUT_WOODS_EventScript_SetBlizzardSnow::
	setmetatile 23, 21, 94, FALSE
	return

WALNUT_WOODS_EventScript_SetProtectSnow::
	setmetatile 14, 13, 537, FALSE
	return

WALNUT_WOODS_EventScript_SetBush1Snow::
	setmetatile 16, 24, 94, FALSE
	return

WALNUT_WOODS_EventScript_SetBush2Snow::
	setmetatile 40, 26, 94, FALSE
	return

@ ===========================================================================
@ Professor Clarkson - pep talk, then runs off to the east (gone for good).
@ ===========================================================================
WALNUT_WOODS_EventScript_Clarkson::
	lock
	faceplayer
	msgbox WALNUT_WOODS_Text_Clarkson1, MSGBOX_DEFAULT
	closemessage
	delay 45
	msgbox WALNUT_WOODS_Text_Clarkson2, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, WALNUT_WOODS_Movement_ClarksonRunOff
	waitmovement 0
	setflag FLAG_HIDE_WALNUT_CLARKSON
	removeobject VAR_LAST_TALKED
	release
	end

@ ===========================================================================
@ Porygon - opens the Pokemon Storage System (move/withdraw/deposit).
@ ===========================================================================
WALNUT_WOODS_EventScript_Porygon::
	lock
	faceplayer
	playse SE_PC_ON
	msgbox WALNUT_WOODS_Text_Porygon, MSGBOX_DEFAULT
	playse SE_PC_LOGIN
	special ShowPokemonStorageSystemPC
	release
	end

@ ===========================================================================
@ Chansey - full heal like a Pokemon Center, and sets the whiteout respawn.
@ ===========================================================================
WALNUT_WOODS_EventScript_Blissey::
	lock
	faceplayer
	msgbox WALNUT_WOODS_Text_ChanseyOffer, MSGBOX_DEFAULT
	closemessage
	special HealPlayerParty
	playfanfare MUS_HEAL
	waitfanfare
	setrespawn HEAL_LOCATION_WALNUT_WOODS
	msgbox WALNUT_WOODS_Text_ChanseyDone, MSGBOX_DEFAULT
	release
	end

@ ===========================================================================
@ Item pickups - the pokeball is a TILE in the map; grabbing it turns the
@ tile into snow (no sprite object).
@ ===========================================================================
WALNUT_WOODS_EventScript_TMBlizzard::
	lockall
	goto_if_set FLAG_ITEM_WALNUT_WOODS_TM_BLIZZARD, WALNUT_WOODS_EventScript_NothingHere
	giveitem ITEM_TM_BLIZZARD
	setflag FLAG_ITEM_WALNUT_WOODS_TM_BLIZZARD
	call WALNUT_WOODS_EventScript_SetBlizzardSnow
	special DrawWholeMapView
	releaseall
	end

WALNUT_WOODS_EventScript_TMProtect::
	lockall
	goto_if_set FLAG_ITEM_WALNUT_WOODS_TM_PROTECT, WALNUT_WOODS_EventScript_NothingHere
	giveitem ITEM_TM_PROTECT
	setflag FLAG_ITEM_WALNUT_WOODS_TM_PROTECT
	call WALNUT_WOODS_EventScript_SetProtectSnow
	special DrawWholeMapView
	releaseall
	end

WALNUT_WOODS_EventScript_NothingHere::
	releaseall
	end

@ ===========================================================================
@ Cuttable bushes - the bush is a TILE in the map; cutting it turns the tile
@ into snow (no sprite object).
@ ===========================================================================
WALNUT_WOODS_EventScript_CutBush1::
	lockall
	checkfieldmove FIELD_MOVE_CUT, TRUE
	goto_if_eq VAR_RESULT, PARTY_SIZE, WALNUT_WOODS_EventScript_BushCantCut
	msgbox WALNUT_WOODS_Text_CutBush, MSGBOX_DEFAULT
	closemessage
	waitse
	playse SE_M_CUT
	delay 20
	setflag FLAG_WALNUT_WOODS_CUT_BUSH_1
	call WALNUT_WOODS_EventScript_SetBush1Snow
	special DrawWholeMapView
	releaseall
	end

WALNUT_WOODS_EventScript_CutBush2::
	lockall
	checkfieldmove FIELD_MOVE_CUT, TRUE
	goto_if_eq VAR_RESULT, PARTY_SIZE, WALNUT_WOODS_EventScript_BushCantCut
	msgbox WALNUT_WOODS_Text_CutBush, MSGBOX_DEFAULT
	closemessage
	waitse
	playse SE_M_CUT
	delay 20
	setflag FLAG_WALNUT_WOODS_CUT_BUSH_2
	call WALNUT_WOODS_EventScript_SetBush2Snow
	special DrawWholeMapView
	releaseall
	end

WALNUT_WOODS_EventScript_BushCantCut::
	msgbox WALNUT_WOODS_Text_BushCantCut, MSGBOX_DEFAULT
	releaseall
	end

@ ===========================================================================
@ Signs
@ ===========================================================================
WALNUT_WOODS_EventScript_SignWest::
	msgbox WALNUT_WOODS_Text_SignWest, MSGBOX_SIGN
	end

WALNUT_WOODS_EventScript_SignEast::
	msgbox WALNUT_WOODS_Text_SignEast, MSGBOX_SIGN
	end

@ ===========================================================================
@ Movement
@ ===========================================================================
WALNUT_WOODS_Movement_ClarksonRunOff:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

@ ===========================================================================
@ Text
@ ===========================================================================
WALNUT_WOODS_Text_Clarkson1:
	.string "PROF. CLARKSON: Glad to see you out\n"
	.string "here training!\p"
	.string "I used to battle with my son out here\n"
	.string "in these very fields, back in the day…\p"
	.string "before I moved on to bigger and\n"
	.string "better things.\p"
	.string "You know… you remind me of him.$"

WALNUT_WOODS_Text_Clarkson2:
	.string "…\p"
	.string "Best not to linger on the past!\p"
	.string "Godspeed, {PLAYER}!$"

WALNUT_WOODS_Text_Porygon:
	.string "A PORYGON hums and fires up the\n"
	.string "POKéMON Storage System.$"

WALNUT_WOODS_Text_ChanseyOffer:
	.string "CHANSEY: Blissey!\p"
	.string "It offers to tend to your tired\n"
	.string "POKéMON.$"

WALNUT_WOODS_Text_ChanseyDone:
	.string "Blissey, blis-sey!\p"
	.string "Your POKéMON are brimming with\n"
	.string "health once more!$"

WALNUT_WOODS_Text_CutBush:
	.string "A thick, snow-laden bush blocks the\n"
	.string "way.\p"
	.string "{PLAYER} used CUT!$"

WALNUT_WOODS_Text_BushCantCut:
	.string "A thick, snow-laden bush.\p"
	.string "It looks like it could be cut down\n"
	.string "by the right move.$"

WALNUT_WOODS_Text_SignWest:
	.string "WALNUT WOODS\n"
	.string "{RIGHT_ARROW} FROSTWOOD TOWN$"

WALNUT_WOODS_Text_SignEast:
	.string "FROSTWOOD TOWN {RIGHT_ARROW} WALNUT WOODS\n"
	.string "{RIGHT_ARROW} MELTING MILE$"
