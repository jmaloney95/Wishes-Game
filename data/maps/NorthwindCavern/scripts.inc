NorthwindCavern_MapScripts::
	.byte 0

@ ============================================================================
@  NORTHWIND CAVERN -- the north wind sleeps at the back of the burial
@  tunnel. SUICUNE waits here (Deoxys pattern: it only leaves when CAUGHT;
@  defeat or flight and it simply reforms on the wind).
@ ============================================================================
NorthwindCavern_EventScript_Suicune::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_SUICUNE, CRY_MODE_ENCOUNTER
	waitmoncry
	delay 30
	msgbox NorthwindCavern_Text_SuicuneStirs, MSGBOX_DEFAULT
	closemessage
	setwildbattle SPECIES_SUICUNE, 35
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, NorthwindCavern_EventScript_SuicuneGone
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, NorthwindCavern_EventScript_SuicuneGone
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, NorthwindCavern_EventScript_SuicuneGone
	@ -- caught: the north wind goes with you --
	setflag FLAG_WOT_SUICUNE_CAUGHT
	removeobject VAR_LAST_TALKED
	release
	end

NorthwindCavern_EventScript_SuicuneGone::
	msgbox NorthwindCavern_Text_SuicuneScatters, MSGBOX_DEFAULT
	release
	end

NorthwindCavern_Text_SuicuneStirs::
	.string "The cold in the chamber gathers\n"
	.string "into a shape...\p"
	.string "SUICUNE lowers its head.\n"
	.string "The north wind has been waiting.$"

NorthwindCavern_Text_SuicuneScatters::
	.string "The shape breaks apart into\n"
	.string "frost and wind...\p"
	.string "The cold settles back over the\n"
	.string "chamber, patient as ever.$"
