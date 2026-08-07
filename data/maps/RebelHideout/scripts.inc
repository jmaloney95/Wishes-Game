	.equ LOCALID_HIDEOUT_OPERATIVE, 1
	.equ LOCALID_HIDEOUT_DEFECTOR, 2
	.equ LOCALID_HIDEOUT_SCENE_ONI, 5
	.equ LOCALID_HIDEOUT_SCENE_OPERATIVE, 6
	.equ LOCALID_HIDEOUT_SCENE_ZOROARK, 7

RebelHideout_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RebelHideout_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, RebelHideout_OnFrame
	.byte 0

@ ============================================================================
@  ACT 3, STEP 7 -- THE REBEL HIDEOUT (WoT_Act3_Canon.md)
@  The only heal + PC box in Shin-Tokyo (Blissey + Porygon). The Step-6
@  arrest DELIVERS the player here still "in custody": scripts don't survive
@  warps, so the street scene ends at the warp and the ON_FRAME scene below
@  plays the reveal -- THE ONI's outline drops, the operative and her ZOROARK
@  stand revealed -- then the defector's briefing and the main quest.
@  The static operative (object 1) stays hidden until the reveal introduces
@  her (FLAG_HIDE_REBEL_OPERATIVE, managed every transition).
@  All dialogue PLACEHOLDER (Joe).
@ ============================================================================

RebelHideout_OnTransition::
	setrespawn HEAL_LOCATION_REBEL_HIDEOUT
	setflag FLAG_HIDE_ACT3_ARREST_NPCS
	goto_if_set FLAG_WOT_HIDEOUT_REVEAL_DONE, RebelHideout_OnTransition_ShowOperative
	setflag FLAG_HIDE_REBEL_OPERATIVE
	end
RebelHideout_OnTransition_ShowOperative::
	clearflag FLAG_HIDE_REBEL_OPERATIVE
	end

RebelHideout_OnFrame:
	map_script_2 VAR_TEMP_1, 0, RebelHideout_EventScript_Reveal
	.2byte 0

RebelHideout_EventScript_Reveal::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_unset FLAG_ACT3_ARREST_DONE, RebelHideout_EventScript_RevealDone
	goto_if_set FLAG_WOT_HIDEOUT_REVEAL_DONE, RebelHideout_EventScript_RevealDone
	applymovement OBJ_EVENT_ID_PLAYER, RebelHideout_Movement_FaceUp
	waitmovement 0
	setobjectxyperm LOCALID_HIDEOUT_SCENE_ONI, 7, 7
	addobject LOCALID_HIDEOUT_SCENE_ONI
	delay 30
	setspeaker SP_NAME_THE_ONI
	msgbox RebelHideout_Text_PreReveal, MSGBOX_DEFAULT
	closemessage
	@ -- the illusion drops: operative + ZOROARK side by side --
	playse SE_M_TELEPORT
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_HIDEOUT_SCENE_ONI
	setobjectxyperm LOCALID_HIDEOUT_SCENE_OPERATIVE, 7, 7
	addobject LOCALID_HIDEOUT_SCENE_OPERATIVE
	setobjectxyperm LOCALID_HIDEOUT_SCENE_ZOROARK, 8, 7
	addobject LOCALID_HIDEOUT_SCENE_ZOROARK
	fadescreen FADE_FROM_BLACK
	waitse
	playmoncry SPECIES_ZOROARK, CRY_MODE_NORMAL
	waitmoncry
	msgbox RebelHideout_Text_IllusionDrops, MSGBOX_DEFAULT
	setspeaker SP_NAME_OPERATIVE
	msgbox RebelHideout_Text_OperativeReveal, MSGBOX_DEFAULT
	closemessage
	@ -- the defector crosses the room for the briefing --
	applymovement LOCALID_HIDEOUT_DEFECTOR, RebelHideout_Movement_DefectorApproach
	waitmovement LOCALID_HIDEOUT_DEFECTOR
	setspeaker SP_NAME_DEFECTOR
	msgbox RebelHideout_Text_Briefing1, MSGBOX_DEFAULT
	msgbox RebelHideout_Text_Briefing2, MSGBOX_DEFAULT
	msgbox RebelHideout_Text_Briefing3, MSGBOX_DEFAULT
	goto_if_unset FLAG_ACT3_RESEARCH_STOLEN, RebelHideout_EventScript_BriefingMission
	@ Kept-then-stolen path only: the operative knows whose research it was.
	setspeaker SP_NAME_OPERATIVE
	msgbox RebelHideout_Text_BriefingStolen, MSGBOX_DEFAULT
RebelHideout_EventScript_BriefingMission::
	setspeaker SP_NAME_DEFECTOR
	msgbox RebelHideout_Text_BriefingMission, MSGBOX_DEFAULT
	closemessage
	startquest QUEST_STOP_THE_MACHINE
	applymovement LOCALID_HIDEOUT_DEFECTOR, RebelHideout_Movement_DefectorReturn
	waitmovement LOCALID_HIDEOUT_DEFECTOR
	setflag FLAG_WOT_HIDEOUT_REVEAL_DONE
RebelHideout_EventScript_RevealDone::
	releaseall
	end

RebelHideout_Movement_FaceUp:
	face_up
	step_end

RebelHideout_Movement_DefectorApproach:
	walk_right
	walk_right
	walk_right
	face_right
	step_end

RebelHideout_Movement_DefectorReturn:
	walk_left
	walk_left
	walk_left
	face_right
	step_end

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
	setspeaker SP_NAME_DEFECTOR
	msgbox RebelHideout_Text_Defector, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_Zoroark::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ZOROARK, CRY_MODE_NORMAL
	waitmoncry
	msgbox RebelHideout_Text_Zoroark, MSGBOX_DEFAULT
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
RebelHideout_Text_PreReveal::
	.string "Down through the bones of the\n"
	.string "old city... and here.\p"
	.string "Out of every camera's reach.$"

RebelHideout_Text_IllusionDrops::
	.string "The ONI's outline RIPPLES --\n"
	.string "and pours away like rainwater.\p"
	.string "A hooded woman. A ZOROARK at\n"
	.string "her shoulder, grinning.$"

RebelHideout_Text_OperativeReveal::
	.string "“Kekeke. Sorry about the\n"
	.string "handcuffs routine.\p"
	.string "“Welcome to the rebellion,\n"
	.string "prison breaker.\p"
	.string "“Some people need to meet\n"
	.string "you.”$"

RebelHideout_Text_Briefing1::
	.string "The man on every screen calls\n"
	.string "himself THE ONI.\p"
	.string "I mixed his serums for three\n"
	.string "years. I know what is upstairs.\p"
	.string "He built a machine that eats\n"
	.string "souls.$"

RebelHideout_Text_Briefing2::
	.string "He needed a power source that\n"
	.string "could not die.\p"
	.string "So he tore JIRACHI -- the\n"
	.string "wish-granter, the kindest thing\l"
	.string "this region ever produced --\l"
	.string "out of the sky.\p"
	.string "And he WELDED it, awake, into\n"
	.string "the core of the tower.$"

RebelHideout_Text_Briefing3::
	.string "Its stolen wishes drive the\n"
	.string "SHADOW GENERATOR.\p"
	.string "Every SHADOW POKéMON in this\n"
	.string "city was fed through that\l"
	.string "machine -- a living soul,\l"
	.string "sealed shut, still screaming.\p"
	.string "That is what powers the lights\n"
	.string "you see outside.$"

RebelHideout_Text_BriefingStolen::
	.string "“And it got worse.\p"
	.string "“Someone let them take\n"
	.string "CLARKSON's research.\p"
	.string "“They've accelerated. Species\n"
	.string "that used to resist the process\l"
	.string "don't anymore.\p"
	.string "“Their new Shadows are...\n"
	.string "strong.”$"

RebelHideout_Text_BriefingMission::
	.string "The tides run low tonight.\p"
	.string "SURF south off Shin-Tokyo.\n"
	.string "Find the island the fairies\l"
	.string "keep -- learn to PURIFY what\l"
	.string "he has corrupted.\p"
	.string "Then climb the tower...\p"
	.string "...and rip that machine's\n"
	.string "heart out.$"

RebelHideout_Text_Operative::
	.string "Nobody follows me twice.\p"
	.string "Rest. Box what you can't carry.\n"
	.string "This is the last safe room in\l"
	.string "the city.\p"
	.string "The tides run low tonight --\n"
	.string "the water south is yours.$"

RebelHideout_Text_Defector::
	.string "Snag them. Purify them.\p"
	.string "Every Shadow you take from him\n"
	.string "is a soul he cannot burn.$"

RebelHideout_Text_Zoroark::
	.string "ZOROARK's grin does not waver.\p"
	.string "You get the feeling it enjoyed\n"
	.string "the handcuffs routine.$"

RebelHideout_Text_BlisseyOffer::
	.string "BLISSEY beams -- the only warm\n"
	.string "light in Shin-Tokyo.$"

RebelHideout_Text_BlisseyDone::
	.string "Your POKEMON are fully rested!$"

RebelHideout_Text_Porygon::
	.string "PORYGON hums -- a stolen uplink\n"
	.string "to the Box network.$"
