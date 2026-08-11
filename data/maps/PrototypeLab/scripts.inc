PrototypeLab_MapScripts::
	.byte 0

@ ============================================================================
@  THE FLOODED LAB (under Prototype Island). Mutrid's abandoned first
@  facility: a drowned hall crossed by Surf, research logs on the way down,
@  and the failed "perfect form" prototype -- a SHADOW DEOXYS -- waiting in
@  the cracked containment ring at the bottom.
@  Encounter rules: Lv50 wild Shadow (WotMakeEventMonShadow); defeating it or
@  fleeing leaves it here (it sinks back into the dark); only CATCHING it
@  clears the object (FLAG_WOT_DEOXYS_CAUGHT). Canon note: it is meant to
@  "resist purification longest" -- special-case at the shrine is a later
@  co-director call; for now it purifies normally.
@  All dialogue PLACEHOLDER (Joe).
@ ============================================================================

PrototypeLab_EventScript_ExitToIsland::
	lockall
	warp MAP_PROTOTYPE_ISLAND, 8, 5
	waitstate
	releaseall
	end

PrototypeLab_EventScript_Deoxys::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_DEOXYS, CRY_MODE_ENCOUNTER
	waitmoncry
	delay 30
	msgbox PrototypeLab_Text_DeoxysStirs, MSGBOX_DEFAULT
	closemessage
	setwildbattle SPECIES_DEOXYS, 50
	special WotMakeEventMonShadow
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, PrototypeLab_EventScript_DeoxysDown
	goto_if_eq VAR_RESULT, B_OUTCOME_RAN, PrototypeLab_EventScript_DeoxysDown
	goto_if_eq VAR_RESULT, B_OUTCOME_PLAYER_TELEPORTED, PrototypeLab_EventScript_DeoxysDown
	@ -- caught: the prototype leaves with you --
	setflag FLAG_WOT_DEOXYS_CAUGHT
	removeobject VAR_LAST_TALKED
	release
	end

PrototypeLab_EventScript_DeoxysDown::
	msgbox PrototypeLab_Text_DeoxysSinks, MSGBOX_DEFAULT
	release
	end

@ --- Research logs (all PLACEHOLDER (Joe)) ----------------------------------
PrototypeLab_EventScript_LogA::
	msgbox PrototypeLab_Text_LogA, MSGBOX_SIGN
	end

PrototypeLab_EventScript_LogB::
	msgbox PrototypeLab_Text_LogB, MSGBOX_SIGN
	end

PrototypeLab_EventScript_LogC::
	msgbox PrototypeLab_Text_LogC, MSGBOX_SIGN
	end

PrototypeLab_Text_LogA::
	.string "A waterlogged terminal, still\n"
	.string "faintly alive.\p"
	.string "“DAY 4. The director calls it\n"
	.string "PROJECT PERFECT FORM.\p"
	.string "“A vessel with no soul of its\n"
	.string "own. Nothing to seal. Nothing\l"
	.string "to resist us.”$"

PrototypeLab_Text_LogB::
	.string "“DAY 61. It rebuilt its own\n"
	.string "arms today.\p"
	.string "“Not the shape we grew. A shape\n"
	.string "it PREFERRED.\p"
	.string "“The director is thrilled.\n"
	.string "The rest of us have stopped\l"
	.string "sleeping here.”$"

PrototypeLab_Text_LogC::
	.string "“DAY 77. Containment is\n"
	.string "cracked. It is not angry.\p"
	.string "“That is the worst part. It\n"
	.string "just WATCHES us, and changes.\p"
	.string "“We flood the shafts tonight.\n"
	.string "Forgive us. Forgive me.”$"

PrototypeLab_Text_DeoxysStirs::
	.string "The water around the ring goes\n"
	.string "utterly still.\p"
	.string "Something uncoils from the\n"
	.string "cracked glass --\p"
	.string "-- and rebuilds itself, piece\n"
	.string "by piece, to face you.$"

PrototypeLab_Text_DeoxysSinks::
	.string "The prototype folds back into\n"
	.string "the dark water.\p"
	.string "Unhurried. Unfinished.\n"
	.string "Waiting.$"
