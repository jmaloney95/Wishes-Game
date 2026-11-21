WALNUT_WOODS_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, WALNUT_WOODS_OnLoad
	.byte 0

@ Restore already-collected items / already-cut bushes to snow on map load.
WALNUT_WOODS_OnLoad::
	call_if_set FLAG_ITEM_WALNUT_WOODS_TM_BLIZZARD, WALNUT_WOODS_EventScript_SetBlizzardSnow
	call_if_set FLAG_ITEM_WALNUT_WOODS_TM_PROTECT, WALNUT_WOODS_EventScript_SetProtectSnow
	call_if_set FLAG_WALNUT_WOODS_CUT_BUSH_1, WALNUT_WOODS_EventScript_SetBush1Snow
	call_if_set FLAG_WALNUT_WOODS_CUT_BUSH_2, WALNUT_WOODS_EventScript_SetBush2Snow
	call_if_set FLAG_ITEM_WALNUT_WOODS_HM06, WALNUT_WOODS_EventScript_SetHM06Snow
	end

@ setmetatile helpers. Coordinates re-read off the map after the 2026-09-13
@ repaint: the previous set was four ROWS stale (27,21 for a ball now at
@ 27,25), so every swap would have landed on the wrong tile. All five sit
@ on the same snow layer, 0x05E.
WALNUT_WOODS_EventScript_SetBlizzardSnow::
	setmetatile 27, 25, 0x05E, FALSE
	return

WALNUT_WOODS_EventScript_SetProtectSnow::
	setmetatile 18, 17, 0x05E, FALSE
	return

WALNUT_WOODS_EventScript_SetBush1Snow::
	setmetatile 20, 28, 0x05E, FALSE
	return

WALNUT_WOODS_EventScript_SetBush2Snow::
	setmetatile 43, 28, 0x05E, FALSE
	return

WALNUT_WOODS_EventScript_SetHM06Snow::
	setmetatile 45, 28, 0x05E, FALSE
	return

@ ===========================================================================
@ Professor Clarkson - starts a how-to-battle lesson, sees the player already
@ knows, and battles them instead. Afterwards he talks about his son, then
@ runs off to the east (gone for good).
@ ===========================================================================
WALNUT_WOODS_EventScript_Clarkson::
	lock
	faceplayer
	@ Beaten but the scene never finished: skip straight to the talk after.
	goto_if_defeated TRAINER_WALNUT_CLARKSON, WALNUT_WOODS_EventScript_ClarksonAfterBattle
	setspeaker SP_NAME_CLARKSON
	showportrait PORTRAIT_CLARKSON_GENGAR
	msgbox WALNUT_WOODS_Text_ClarksonLesson, MSGBOX_DEFAULT
	closemessage
	playse SE_PIN
	applymovement VAR_LAST_TALKED, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement VAR_LAST_TALKED, Common_Movement_Delay48
	waitmovement 0
	setspeaker SP_NAME_CLARKSON
	msgbox WALNUT_WOODS_Text_ClarksonYouKnow, MSGBOX_DEFAULT
	closemessage
	hideportrait
	@ Losing is fine: the player whites out to the Blissey here, not back to
	@ the last town, and he is still standing here for another try (a loss
	@ never sets his trainer flag).
	setrespawn HEAL_LOCATION_WALNUT_WOODS
	setspeaker SP_NAME_CLARKSON
	trainerbattle_no_intro TRAINER_WALNUT_CLARKSON, WALNUT_WOODS_Text_ClarksonDefeated
WALNUT_WOODS_EventScript_ClarksonAfterBattle::
	@ The field reload after the battle turns him back to his map facing.
	applymovement VAR_LAST_TALKED, Common_Movement_FacePlayer
	waitmovement 0
	setspeaker SP_NAME_CLARKSON
	showportrait PORTRAIT_CLARKSON_GENGAR
	msgbox WALNUT_WOODS_Text_Clarkson1, MSGBOX_DEFAULT
	closemessage
	delay 45
	setspeaker SP_NAME_CLARKSON
	msgbox WALNUT_WOODS_Text_Clarkson2, MSGBOX_DEFAULT
	closemessage
	hideportrait
	@ Off down row 30. If the player is standing right below him, that is
	@ where he would step first, so he goes round them on the right instead.
	getplayerxy VAR_0x8004, VAR_0x8005
	goto_if_ne VAR_0x8004, 30, WALNUT_WOODS_EventScript_ClarksonRunStraight
	goto_if_ne VAR_0x8005, 30, WALNUT_WOODS_EventScript_ClarksonRunStraight
	applymovement VAR_LAST_TALKED, WALNUT_WOODS_Movement_ClarksonRunAround
	goto WALNUT_WOODS_EventScript_ClarksonGone

WALNUT_WOODS_EventScript_ClarksonRunStraight::
	applymovement VAR_LAST_TALKED, WALNUT_WOODS_Movement_ClarksonRunOff
WALNUT_WOODS_EventScript_ClarksonGone::
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

@ Was a red ITEM_BALL object sitting on top of the painted ball; now a tile
@ pickup like the two TMs.
WALNUT_WOODS_EventScript_HM06::
	lockall
	goto_if_set FLAG_ITEM_WALNUT_WOODS_HM06, WALNUT_WOODS_EventScript_NothingHere
	giveitem ITEM_HM06
	setflag FLAG_ITEM_WALNUT_WOODS_HM06
	call WALNUT_WOODS_EventScript_SetHM06Snow
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
@ Down one, THEN east. He stands at (30,29) and row 29 is walled from x=33,
@ so running straight east would put him through the trees (applymovement
@ never collision-checks). Row 30 is clear from x=20 to x=41, so the single
@ step down is what makes the exit legal.
@ Ten steps east reaches x=40; the screen's right edge sits around x=37 with
@ the player where they'd be standing, so he is gone before removeobject.
WALNUT_WOODS_Movement_ClarksonRunOff:
	walk_fast_down
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

@ The player is on (30,30): step right to (31,29) first, then down onto row
@ 30 beside them and east, ending at x=40 like the straight run.
WALNUT_WOODS_Movement_ClarksonRunAround:
	walk_fast_right
	walk_fast_down
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

@ ===========================================================================
@ Text
@ ===========================================================================
WALNUT_WOODS_Text_ClarksonLesson:
	.string "Glad to see you out\n"
	.string "here training!\p"
	.string "Here, let me show you how a battle\n"
	.string "works.\p"
	.string "First, you pick which POKéMON you\n"
	.string "want to fight with.\p"
	.string "Then you just bring out your\n"
	.string "POKéMON like this…$"

WALNUT_WOODS_Text_ClarksonYouKnow:
	.string "Whaaa?\p"
	.string "You already know how to battle?\p"
	.string "Well, alright then.\n"
	.string "Give me your best shot!$"

WALNUT_WOODS_Text_ClarksonDefeated:
	.string "Ha! You weren't kidding.\n"
	.string "You really do know how to battle!$"

WALNUT_WOODS_Text_Clarkson1:
	.string "I used to battle with my son out here\n"
	.string "in these very fields, back in the day…\p"
	.string "before he got involved with\n"
	.string "the wrong group of friends.\p"
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
	.string "With the EMBER BADGE, it could\n"
	.string "be CUT down.$"

WALNUT_WOODS_Text_SignWest:
	.string "WALNUT WOODS\n"
	.string "{RIGHT_ARROW} FROSTWOOD TOWN$"

WALNUT_WOODS_Text_SignEast:
	.string "FROSTWOOD TOWN {RIGHT_ARROW} WALNUT WOODS\n"
	.string "{RIGHT_ARROW} MELTING MILE$"
