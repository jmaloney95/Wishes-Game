PrototypeLab_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, PrototypeLab_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, PrototypeLab_OnFrame
	.byte 0

@ Each trip in rolls whether Deoxys is shiny (VAR_TEMP_1, and its
@ overworld colours through VAR_OBJ_GFX_ID_0); a shiny one chimes the Z-A
@ shiny jingle once the lab has faded in.
PrototypeLab_OnTransition:
	special WotPrototypeLab_RollDeoxysShiny
	end

PrototypeLab_OnFrame:
	map_script_2 VAR_TEMP_1, 1, PrototypeLab_EventScript_ShinyChime
	.2byte 0

PrototypeLab_EventScript_ShinyChime::
	playse SE_WOT_SHINY
	setvar VAR_TEMP_1, 2
	end

@ ============================================================================
@  THE FLOODED LAB (under Prototype Island). Mutrid's abandoned first
@  facility: a drowned hall crossed by Surf, research logs on the way down,
@  and the failed "perfect form" prototype -- a SHADOW DEOXYS -- waiting in
@  the cracked containment ring at the bottom.
@  Encounter rules: Lv50 wild Shadow (WotMakeEventMonShadow); defeating it or
@  fleeing leaves it here (it sinks back into the dark); only CATCHING it
@  clears the object (FLAG_WOT_DEOXYS_CAUGHT). It is meant to
@  "resist purification longest"; for now it purifies
@  normally.
@
@ ============================================================================

PrototypeLab_EventScript_ExitToIsland::
	lockall
	warp MAP_PROTOTYPE_ISLAND, 6, 15
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
	@ It Mega Evolves on its own. It holds its stone, and the
	@ smart-wild-AI flag lets a wild mon choose its gimmick. It stays a
	@ Shadow; its Mega art is shown as drawn (WotShadowKeepsOwnArt).
	setwildbattle SPECIES_DEOXYS, 50, ITEM_DEOXYSITE
	special WotMakeEventMonShadow
	special WotPrototypeLab_SetDeoxysShiny
	setflag FLAG_WOT_SMART_WILD_AI
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	clearflag FLAG_WOT_SMART_WILD_AI
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

@ --- Research logs ----------------------------------
PrototypeLab_EventScript_LogA::
	msgbox PrototypeLab_Text_LogA, MSGBOX_SIGN
	end

PrototypeLab_EventScript_LogB::
	msgbox PrototypeLab_Text_LogB, MSGBOX_SIGN
	end

PrototypeLab_EventScript_LogC::
	msgbox PrototypeLab_Text_LogC, MSGBOX_SIGN
	end

@ The other two terminals on this floor. The prototype was never the only
@ thing down here that stopped doing as it was told.
PrototypeLab_EventScript_LogD::
	msgbox PrototypeLab_Text_LogD, MSGBOX_SIGN
	end

PrototypeLab_EventScript_LogE::
	msgbox PrototypeLab_Text_LogE, MSGBOX_SIGN
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

PrototypeLab_Text_LogD::
	.string "This terminal is dry. Someone\n"
	.string "kept it that way.\p"
	.string "“DAY 52. The lab system filed an\n"
	.string "experiment none of us wrote.\p"
	.string "“It logged the results before we\n"
	.string "ran it. They matched.\p"
	.string "“We are no longer sure which of\n"
	.string "us is the instrument.”$"

PrototypeLab_Text_LogE::
	.string "The screen is scrolling on its\n"
	.string "own.\p"
	.string "“DAY 71. We cut the system's\n"
	.string "uplink at the junction box.\p"
	.string "“The doors still open for it. The\n"
	.string "lights still answer it.\p"
	.string "“It has begun signing our names\n"
	.string "at the bottom of these logs.\p"
	.string "“It says it is helping.”$"

PrototypeLab_Text_DeoxysStirs::
	.string "The water around the ring goes\n"
	.string "utterly still.\p"
	.string "Something uncoils from the\n"
	.string "cracked glass…\p"
	.string "and rebuilds itself, piece\n"
	.string "by piece, to face you.$"

PrototypeLab_Text_DeoxysSinks::
	.string "The prototype folds back into\n"
	.string "the dark water.\p"
	.string "Unhurried. Unfinished.\n"
	.string "Waiting.$"
