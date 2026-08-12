@ ============================================================================
@  WoT: the HMs submenu (START -> HMS).
@  HMs are the trainer's own abilities -- unlocked by badges, never taught.
@  Cut / Rock Smash / Strength / Surf / Waterfall / Dive / Flash are used by
@  interacting with the world (trees, rocks, boulders, water edges...), so
@  this menu lists them for reference. FLY has no tile to walk into, so it is
@  USED from here: pick it and the region map opens.
@  Only unlocked moves are pushed; the selection ids stay fixed per move so
@  the switch below doesn't shift as the list grows.
@ ============================================================================

	.equ WOT_HM_CUT, 0
	.equ WOT_HM_FLASH, 1
	.equ WOT_HM_ROCK_SMASH, 2
	.equ WOT_HM_STRENGTH, 3
	.equ WOT_HM_SURF, 4
	.equ WOT_HM_FLY, 5
	.equ WOT_HM_DIVE, 6
	.equ WOT_HM_WATERFALL, 7
	.equ WOT_HM_CANCEL, 8

WotHMs_EventScript_List::
	lockall
	setvar VAR_TEMP_A, 0
	setvar VAR_0x8004, FIELD_MOVE_CUT
	special WotIsHMUnlocked
	call_if_eq VAR_RESULT, TRUE, WotHMs_EventScript_PushCut
	setvar VAR_0x8004, FIELD_MOVE_FLASH
	special WotIsHMUnlocked
	call_if_eq VAR_RESULT, TRUE, WotHMs_EventScript_PushFlash
	setvar VAR_0x8004, FIELD_MOVE_ROCK_SMASH
	special WotIsHMUnlocked
	call_if_eq VAR_RESULT, TRUE, WotHMs_EventScript_PushRockSmash
	setvar VAR_0x8004, FIELD_MOVE_STRENGTH
	special WotIsHMUnlocked
	call_if_eq VAR_RESULT, TRUE, WotHMs_EventScript_PushStrength
	setvar VAR_0x8004, FIELD_MOVE_SURF
	special WotIsHMUnlocked
	call_if_eq VAR_RESULT, TRUE, WotHMs_EventScript_PushSurf
	setvar VAR_0x8004, FIELD_MOVE_FLY
	special WotIsHMUnlocked
	call_if_eq VAR_RESULT, TRUE, WotHMs_EventScript_PushFly
	setvar VAR_0x8004, FIELD_MOVE_DIVE
	special WotIsHMUnlocked
	call_if_eq VAR_RESULT, TRUE, WotHMs_EventScript_PushDive
	setvar VAR_0x8004, FIELD_MOVE_WATERFALL
	special WotIsHMUnlocked
	call_if_eq VAR_RESULT, TRUE, WotHMs_EventScript_PushWaterfall
	goto_if_eq VAR_TEMP_A, 0, WotHMs_EventScript_None
	dynmultipush WotHMs_Text_Cancel, WOT_HM_CANCEL
	dynmultistack 0, 0, FALSE, 6, FALSE, 0, NULL
	switch VAR_RESULT
	case WOT_HM_FLY, WotHMs_EventScript_UseFly
	case WOT_HM_CUT, WotHMs_EventScript_InfoCut
	case WOT_HM_FLASH, WotHMs_EventScript_InfoFlash
	case WOT_HM_ROCK_SMASH, WotHMs_EventScript_InfoRockSmash
	case WOT_HM_STRENGTH, WotHMs_EventScript_InfoStrength
	case WOT_HM_SURF, WotHMs_EventScript_InfoSurf
	case WOT_HM_DIVE, WotHMs_EventScript_InfoDive
	case WOT_HM_WATERFALL, WotHMs_EventScript_InfoWaterfall
	goto WotHMs_EventScript_Close

@ --- list builders (VAR_TEMP_A counts what was pushed) ----------------------
WotHMs_EventScript_PushCut::
	dynmultipush WotHMs_Text_Cut, WOT_HM_CUT
	addvar VAR_TEMP_A, 1
	return

WotHMs_EventScript_PushFlash::
	dynmultipush WotHMs_Text_Flash, WOT_HM_FLASH
	addvar VAR_TEMP_A, 1
	return

WotHMs_EventScript_PushRockSmash::
	dynmultipush WotHMs_Text_RockSmash, WOT_HM_ROCK_SMASH
	addvar VAR_TEMP_A, 1
	return

WotHMs_EventScript_PushStrength::
	dynmultipush WotHMs_Text_Strength, WOT_HM_STRENGTH
	addvar VAR_TEMP_A, 1
	return

WotHMs_EventScript_PushSurf::
	dynmultipush WotHMs_Text_Surf, WOT_HM_SURF
	addvar VAR_TEMP_A, 1
	return

WotHMs_EventScript_PushFly::
	dynmultipush WotHMs_Text_Fly, WOT_HM_FLY
	addvar VAR_TEMP_A, 1
	return

WotHMs_EventScript_PushDive::
	dynmultipush WotHMs_Text_Dive, WOT_HM_DIVE
	addvar VAR_TEMP_A, 1
	return

WotHMs_EventScript_PushWaterfall::
	dynmultipush WotHMs_Text_Waterfall, WOT_HM_WATERFALL
	addvar VAR_TEMP_A, 1
	return

@ --- FLY: the one HM used from this menu ------------------------------------
WotHMs_EventScript_UseFly::
	closemessage
	releaseall
	special WotOpenFlyMap
	end

@ --- the rest are used by walking into the world ----------------------------
WotHMs_EventScript_InfoCut::
	msgbox WotHMs_Text_InfoCut, MSGBOX_DEFAULT
	goto WotHMs_EventScript_Close

WotHMs_EventScript_InfoFlash::
	msgbox WotHMs_Text_InfoFlash, MSGBOX_DEFAULT
	goto WotHMs_EventScript_Close

WotHMs_EventScript_InfoRockSmash::
	msgbox WotHMs_Text_InfoRockSmash, MSGBOX_DEFAULT
	goto WotHMs_EventScript_Close

WotHMs_EventScript_InfoStrength::
	msgbox WotHMs_Text_InfoStrength, MSGBOX_DEFAULT
	goto WotHMs_EventScript_Close

WotHMs_EventScript_InfoSurf::
	msgbox WotHMs_Text_InfoSurf, MSGBOX_DEFAULT
	goto WotHMs_EventScript_Close

WotHMs_EventScript_InfoDive::
	msgbox WotHMs_Text_InfoDive, MSGBOX_DEFAULT
	goto WotHMs_EventScript_Close

WotHMs_EventScript_InfoWaterfall::
	msgbox WotHMs_Text_InfoWaterfall, MSGBOX_DEFAULT
	goto WotHMs_EventScript_Close

WotHMs_EventScript_None::
	msgbox WotHMs_Text_None, MSGBOX_DEFAULT
WotHMs_EventScript_Close::
	closemessage
	releaseall
	end

@ --- menu entries ----------------------------------------------------------
WotHMs_Text_Cut::
	.string "CUT$"

WotHMs_Text_Flash::
	.string "FLASH$"

WotHMs_Text_RockSmash::
	.string "ROCK SMASH$"

WotHMs_Text_Strength::
	.string "STRENGTH$"

WotHMs_Text_Surf::
	.string "SURF$"

WotHMs_Text_Fly::
	.string "FLY$"

WotHMs_Text_Dive::
	.string "DIVE$"

WotHMs_Text_Waterfall::
	.string "WATERFALL$"

WotHMs_Text_Cancel::
	.string "CANCEL$"

@ --- descriptions ----------------------------------------------------------
WotHMs_Text_InfoCut::
	.string "CUT: step up to a thin tree\n"
	.string "and it comes down.$"

WotHMs_Text_InfoFlash::
	.string "FLASH: dark caves light\n"
	.string "themselves as you enter.$"

WotHMs_Text_InfoRockSmash::
	.string "ROCK SMASH: walk into a\n"
	.string "cracked rock to break it.$"

WotHMs_Text_InfoStrength::
	.string "STRENGTH: push boulders\n"
	.string "wherever you meet them.$"

WotHMs_Text_InfoSurf::
	.string "SURF: step to the water's\n"
	.string "edge and ride out.$"

WotHMs_Text_InfoDive::
	.string "DIVE: use it on deep water\n"
	.string "to go under.$"

WotHMs_Text_InfoWaterfall::
	.string "WATERFALL: ride straight up\n"
	.string "the falls.$"

WotHMs_Text_None::
	.string "No field moves unlocked yet.\p"
	.string "Badges awaken them.$"
