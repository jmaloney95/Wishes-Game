RebelHideout_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RebelHideout_OnTransition
	.byte 0

@ ============================================================================
@  ACT 3, STEP 7 (STUB) -- THE REBEL HIDEOUT (WoT_Act3_Canon.md)
@  Safe-hub shell reached by the Step 6 arrest escort. Per canon this is the
@  ONLY heal + PC box in Shin-Tokyo (Blissey + Porygon, cloned from the
@  Walnut Woods pattern). The defector's shadow-science lesson, the
@  archipelago map handover, and the purification loop are the NEXT
@  implementation pass -- their NPCs stand here as stubs.
@  All dialogue PLACEHOLDER (Joe).
@ ============================================================================

RebelHideout_OnTransition::
	setrespawn HEAL_LOCATION_REBEL_HIDEOUT
	end

RebelHideout_EventScript_Operative::
	lock
	faceplayer
	setspeaker SP_NAME_OPERATIVE
	msgbox RebelHideout_Text_Operative, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_Defector::
	lock
	faceplayer
	msgbox RebelHideout_Text_Defector, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_Blissey::
	lock
	faceplayer
	msgbox RebelHideout_Text_BlisseyOffer, MSGBOX_DEFAULT
	closemessage
	special HealPlayerParty
	playfanfare MUS_HEAL
	waitfanfare
	setrespawn HEAL_LOCATION_REBEL_HIDEOUT
	msgbox RebelHideout_Text_BlisseyDone, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_Porygon::
	lock
	faceplayer
	playse SE_PC_ON
	msgbox RebelHideout_Text_Porygon, MSGBOX_DEFAULT
	playse SE_PC_LOGIN
	special ShowPokemonStorageSystemPC
	release
	end

RebelHideout_EventScript_ExitDoor::
	lockall
	closemessage
	fadescreen FADE_TO_BLACK
	warp MAP_SHIN_TOKYO, 16, 15
	waitstate
	releaseall
	end

@ --- Text (all PLACEHOLDER (Joe)) -------------------------------------------
RebelHideout_Text_Operative::
	.string "Nobody follows me twice.\p"
	.string "Rest. Box what you can't carry.\n"
	.string "This is the last safe room in\l"
	.string "the city.\p"
	.string "When you're ready, we talk about\n"
	.string "what they're doing upstairs.$"

RebelHideout_Text_Defector::
	.string "You're the one from the prison\n"
	.string "feeds. I used to work for HIM.\p"
	.string "...For his father, really.\n"
	.string "I'll explain what a Shadow\l"
	.string "POKEMON is. Soon.\p"
	.string "(The purification loop is the\n"
	.string "next implementation pass.)$"

RebelHideout_Text_BlisseyOffer::
	.string "BLISSEY beams -- the only warm\n"
	.string "light in Shin-Tokyo.$"

RebelHideout_Text_BlisseyDone::
	.string "Your POKEMON are fully rested!$"

RebelHideout_Text_Porygon::
	.string "PORYGON hums -- a stolen uplink\n"
	.string "to the Box network.$"
