@ The Drained Hotel: Hotel Z's layout, gray, behind the rift at the
@ desk. MAPSEC_MEGA_DIMENSION for the gray and the battle void, but no portal
@ session: no clock and no HUD, and a whiteout goes to the last Pokemon Center
@ (WotMegaDim_TrySetWhiteoutWarp), with the rift still open to try again.
MegaDimension_DrainedHotel_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DrainedHotel_OnTransition
	.byte 0

DrainedHotel_OnTransition::
	special WotZygarde_SetupDrainedHotel
	end

DrainedHotel_EventScript_Exit::
	lock
	msgbox DrainedHotel_Text_ExitPrompt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, DrainedHotel_EventScript_ExitNo
	closemessage
	call MegaDim_EventScript_RiftOut
	warp MAP_HOTEL_Z, 2, 3
	waitstate
	release
	end

DrainedHotel_EventScript_ExitNo::
	closemessage
	release
	end

@ Mega Floette. Only a win moves things on; it can't be caught.
DrainedHotel_EventScript_Floette::
	lock
	setspeaker SP_NAME_AZ
	msgbox DrainedHotel_Text_AZWaits, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	special WotZygarde_SetupFloetteBattle
	copyvar VAR_0x8009, VAR_0x8004
	waitse
	playmoncry VAR_0x8009, CRY_MODE_ENCOUNTER
	waitmoncry
	msgbox DrainedHotel_Text_FloetteRises, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_WOT_NO_CATCHING
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	clearflag FLAG_WOT_NO_CATCHING
	special WotMegaDim_EndMegaBattle
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, DrainedHotel_EventScript_FloetteFalls
	release
	end

@ The dog comes through, the pebble breaks, and Zygarde finishes it.
DrainedHotel_EventScript_FloetteFalls::
	clearflag FLAG_TEMP_4
	addobject LOCALID_DRAINED_DOG
	waitse
	playmoncry SPECIES_ZYGARDE_10_AURA_BREAK, CRY_MODE_ENCOUNTER
	waitmoncry
	msgbox DrainedHotel_Text_PebbleCracks, MSGBOX_DEFAULT
	closemessage
	removeitem ITEM_ODD_PEBBLE
	playse SE_THUNDER
	setvar VAR_0x8004, 2
	setvar VAR_0x8005, 2
	setvar VAR_0x8006, 12
	setvar VAR_0x8007, 4
	special ShakeCamera
	waitstate
	fadescreenswapbuffers FADE_TO_WHITE
	removeobject LOCALID_DRAINED_DOG
	removeobject LOCALID_DRAINED_MACHINE
	setflag FLAG_TEMP_4
	setflag FLAG_TEMP_3
	clearflag FLAG_TEMP_5
	addobject LOCALID_DRAINED_ZYGARDE
	fadescreenswapbuffers FADE_FROM_WHITE
	waitse
	playmoncry SPECIES_ZYGARDE_COMPLETE, CRY_MODE_ENCOUNTER
	waitmoncry
	msgbox DrainedHotel_Text_Sealed, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_AZ
	msgbox DrainedHotel_Text_AZEnd, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	playse SE_WARP_OUT
	removeobject LOCALID_DRAINED_AZ
	setflag FLAG_TEMP_2
	removeobject LOCALID_DRAINED_FLOETTE
	setflag FLAG_TEMP_1
	setvar VAR_0x8004, 3
	special WotZygarde_SetStage
	msgbox DrainedHotel_Text_ZygardeWaits, MSGBOX_DEFAULT
	closemessage
	release
	end

@ Complete Zygarde, holding Zygardite. Only a catch counts; knocked out, it's
@ gone until the next visit.
DrainedHotel_EventScript_Zygarde::
	lock
	faceplayer
	special WotZygarde_SetupZygardeBattle
	copyvar VAR_0x8009, VAR_0x8004
	waitse
	playmoncry VAR_0x8009, CRY_MODE_ENCOUNTER
	waitmoncry
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special WotMegaDim_EndMegaBattle
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, DrainedHotel_EventScript_ZygardeCaught
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, DrainedHotel_EventScript_ZygardeKOd
	release
	end

DrainedHotel_EventScript_ZygardeKOd::
	removeobject VAR_LAST_TALKED
	setflag FLAG_TEMP_5
	msgbox DrainedHotel_Text_ZygardeRetreats, MSGBOX_DEFAULT
	closemessage
	release
	end

DrainedHotel_EventScript_ZygardeCaught::
	removeobject VAR_LAST_TALKED
	setflag FLAG_TEMP_5
	setflag FLAG_WOT_CAUGHT_ZYGARDE
	completequest QUEST_THE_LONG_STAY
	msgbox DrainedHotel_Text_ZygardeCaught, MSGBOX_DEFAULT
	closemessage
	release
	end

DrainedHotel_Text_ExitPrompt:
	.string "Go back through the rift?$"

DrainedHotel_Text_AZWaits:
	.string "You came. Of course you did.\p"
	.string "Look at her. Three thousand years,\n"
	.string "and she has never once looked at me\l"
	.string "the way she used to.\p"
	.string "She only needs a little more.$"

DrainedHotel_Text_FloetteRises:
	.string "The machine howls, and the Floette\n"
	.string "rises out of it, burning white!$"

DrainedHotel_Text_PebbleCracks:
	.string "The dog walks past you, straight\n"
	.string "into the light.\p"
	.string "Your Odd Pebble is hot. It splits.\p"
	.string "Every Cell you carried is moving.$"

DrainedHotel_Text_Sealed:
	.string "Zygarde, whole at last, closes over\n"
	.string "the machine like a wound healing.\p"
	.string "It's quiet.$"

DrainedHotel_Text_AZEnd:
	.string "Ah. There you are.\p"
	.string "All this time, you were only ever\n"
	.string "trying to leave.\p"
	.string "Go on, then. I'm sorry I kept you.$"

DrainedHotel_Text_ZygardeWaits:
	.string "AZ is gone.\p"
	.string "Zygarde hasn't moved. It's looking\n"
	.string "at you.$"

DrainedHotel_Text_ZygardeRetreats:
	.string "Zygarde sinks away into the floor.\n"
	.string "It'll be back when you are.$"

DrainedHotel_Text_ZygardeCaught:
	.string "Mable should hear about this.$"
