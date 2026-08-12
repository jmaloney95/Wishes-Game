	.equ LOCALID_HIDEOUT_OPERATIVE, 1
	.equ LOCALID_HIDEOUT_DEFECTOR, 2
	.equ LOCALID_HIDEOUT_SCENE_ONI, 5
	.equ LOCALID_HIDEOUT_SCENE_OPERATIVE, 6
	.equ LOCALID_HIDEOUT_SCENE_ZOROARK, 7
	.equ LOCALID_HIDEOUT_JET, 9

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
	@ Post-game: the whole rebellion gathers here (the jet only once Nessa
	@ has delivered it). Before the clear, all of it stays hidden.
	call_if_unset FLAG_SYS_GAME_CLEAR, RebelHideout_OnTransition_PreClear
	call_if_set FLAG_SYS_GAME_CLEAR, RebelHideout_OnTransition_PostClear
	goto_if_set FLAG_WOT_HIDEOUT_REVEAL_DONE, RebelHideout_OnTransition_ShowOperative
	setflag FLAG_HIDE_REBEL_OPERATIVE
	end
RebelHideout_OnTransition_ShowOperative::
	clearflag FLAG_HIDE_REBEL_OPERATIVE
	end
RebelHideout_OnTransition_PreClear::
	setflag FLAG_WOT_HIDE_POSTGAME_REBELS
	setflag FLAG_WOT_HIDE_POSTGAME_JET
	return
RebelHideout_OnTransition_PostClear::
	clearflag FLAG_WOT_HIDE_POSTGAME_REBELS
	call_if_set FLAG_WOT_POSTGAME_WELCOME_DONE, RebelHideout_OnTransition_ShowJet
	@ The hide flag MUST be set here, not only in the pre-clear branch: flags
	@ default to CLEAR, so a save that was already game-cleared before the jet
	@ existed never ran PreClear and spawned the jet parked on the crate.
	call_if_unset FLAG_WOT_POSTGAME_WELCOME_DONE, RebelHideout_OnTransition_HideJet
	return
RebelHideout_OnTransition_ShowJet::
	clearflag FLAG_WOT_HIDE_POSTGAME_JET
	return
RebelHideout_OnTransition_HideJet::
	setflag FLAG_WOT_HIDE_POSTGAME_JET
	return

RebelHideout_OnFrame:
	map_script_2 VAR_TEMP_1, 0, RebelHideout_EventScript_Reveal
	map_script_2 VAR_TEMP_2, 0, RebelHideout_EventScript_PostgameWelcome
	.2byte 0

@ ============================================================================
@  POST-GAME -- THE MORNING AFTER (plays on the first hideout load after the
@  credits: GameClear continues the save at the hideout cots). Nessa crosses
@  the bar, hands over the SHINY CHARM, and the rebellion's parting gift --
@  a PRIVATE JET -- swoops in and parks over the back wall at (8,3).
@  One-shot via FLAG_WOT_POSTGAME_WELCOME_DONE.
@ ============================================================================
RebelHideout_EventScript_PostgameWelcome::
	lockall
	setvar VAR_TEMP_2, 1
	goto_if_unset FLAG_SYS_GAME_CLEAR, RebelHideout_EventScript_PostgameWelcomeDone
	goto_if_set FLAG_WOT_POSTGAME_WELCOME_DONE, RebelHideout_EventScript_PostgameWelcomeDone
	@ Belt and suspenders: the post-credits continue path must have Nessa at
	@ her post no matter which load path dropped her (addobject is a no-op if
	@ she already spawned).
	clearflag FLAG_HIDE_REBEL_OPERATIVE
	addobject LOCALID_HIDEOUT_OPERATIVE
	delay 30
	applymovement LOCALID_HIDEOUT_OPERATIVE, RebelHideout_Movement_NessaApproach
	waitmovement LOCALID_HIDEOUT_OPERATIVE
	setspeaker SP_NAME_OPERATIVE
	msgbox RebelHideout_Text_PostgameCongrats, MSGBOX_DEFAULT
	closemessage
	hideportrait
	giveitem ITEM_SHINY_CHARM
	setspeaker SP_NAME_OPERATIVE
	msgbox RebelHideout_Text_PostgameOnTheHouse, MSGBOX_DEFAULT
	closemessage
	hideportrait
	delay 50
	setspeaker SP_NAME_OPERATIVE
	msgbox RebelHideout_Text_PostgameAlmostForgot, MSGBOX_DEFAULT
	closemessage
	hideportrait
	@ -- the PJ swoops in from beyond the right edge (x=17 is fully outside
	@ the 15-wide viewport; scripted movement never collision-checks, so the
	@ flight over the wall row is safe) and parks on the green crate --
	applymovement OBJ_EVENT_ID_PLAYER, RebelHideout_Movement_FaceUp
	waitmovement 0
	@ Kill any copy already standing on the crate before repositioning: a
	@ spawned object ignores setobjectxyperm (that only edits the template)
	@ and makes addobject a no-op, which is how it ended up flying FROM the
	@ crate instead of TO it.
	removeobject LOCALID_HIDEOUT_JET
	clearflag FLAG_WOT_HIDE_POSTGAME_JET
	setobjectxyperm LOCALID_HIDEOUT_JET, 17, 3
	addobject LOCALID_HIDEOUT_JET
	playse SE_M_RAZOR_WIND
	applymovement LOCALID_HIDEOUT_JET, RebelHideout_Movement_JetFlyIn
	waitmovement LOCALID_HIDEOUT_JET
	playse SE_TRUCK_UNLOAD
	delay 16
	copyobjectxytoperm LOCALID_HIDEOUT_JET
	applymovement OBJ_EVENT_ID_PLAYER, RebelHideout_Movement_FaceDown
	waitmovement 0
	setspeaker SP_NAME_OPERATIVE
	msgbox RebelHideout_Text_PostgameFly, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_WOT_POSTGAME_WELCOME_DONE
	applymovement LOCALID_HIDEOUT_OPERATIVE, RebelHideout_Movement_NessaReturn
	waitmovement LOCALID_HIDEOUT_OPERATIVE
RebelHideout_EventScript_PostgameWelcomeDone::
	releaseall
	end

RebelHideout_Movement_NessaApproach:
	walk_right
	walk_right
	walk_right
	walk_right
	face_up
	step_end

RebelHideout_Movement_NessaReturn:
	walk_left
	walk_left
	walk_left
	walk_left
	face_down
	step_end

RebelHideout_Movement_JetFlyIn:
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	step_end

RebelHideout_Movement_FaceDown:
	face_down
	step_end

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
	@ -- and the ledger to track the hunt with --
	setspeaker SP_NAME_DEFECTOR
	msgbox RebelHideout_Text_BriefingLogGift, MSGBOX_DEFAULT
	closemessage
	playfanfare MUS_OBTAIN_ITEM
	message RebelHideout_Text_ReceivedShadowLog
	waitmessage
	waitfanfare
	closemessage
	setflag FLAG_WOT_SHADOW_LOG_GET
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

@ Prof. Assistant's line advances with the hunt: purified analysis once the
@ first shrine visit lands, snag analysis once the first Shadow is caught,
@ else the standing briefing line.
RebelHideout_EventScript_Defector::
	lock
	faceplayer
	setspeaker SP_NAME_DEFECTOR
	special WotShadowLogPurifiedCount
	goto_if_ge VAR_RESULT, 1, RebelHideout_EventScript_DefectorPurified
	special WotShadowLogSnaggedCount
	goto_if_ge VAR_RESULT, 1, RebelHideout_EventScript_DefectorAnalysis
	msgbox RebelHideout_Text_Defector, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_DefectorAnalysis::
	msgbox RebelHideout_Text_DefectorAnalysis, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_DefectorPurified::
	msgbox RebelHideout_Text_DefectorPurified, MSGBOX_DEFAULT
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

@ --- post-game: the gathered rebellion --------------------------------------
RebelHideout_EventScript_PrivateJet::
	lockall
	msgbox RebelHideout_Text_PrivateJet, MSGBOX_SIGN
	releaseall
	end

RebelHideout_EventScript_PostgameFatality::
	lock
	faceplayer
	setspeaker SP_NAME_RED_FATALITY
	msgbox RebelHideout_Text_PostgameFatality, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_PostgameYiffer::
	lock
	faceplayer
	setspeaker SP_NAME_YIFFER
	msgbox RebelHideout_Text_PostgameYiffer, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_PostgameMasterGen::
	lock
	faceplayer
	setspeaker SP_NAME_MASTER_GEN
	msgbox RebelHideout_Text_PostgameMasterGen, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_PostgameMom::
	lock
	faceplayer
	setspeaker SP_NAME_MOM
	msgbox RebelHideout_Text_PostgameMom, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_PostgameMikmanc::
	lock
	faceplayer
	setspeaker SP_NAME_MIKMANC
	msgbox RebelHideout_Text_PostgameMikmanc, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_PostgameMinister::
	lock
	faceplayer
	setspeaker SP_NAME_MINISTER
	msgbox RebelHideout_Text_PostgameMinister, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_PostgameClarkson::
	lock
	faceplayer
	setspeaker SP_NAME_CLARKSON
	msgbox RebelHideout_Text_PostgameClarkson, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_PostgameEdwards::
	lock
	faceplayer
	setspeaker SP_NAME_EDWARDS
	msgbox RebelHideout_Text_PostgameEdwards, MSGBOX_DEFAULT
	release
	end

RebelHideout_EventScript_PostgameAllison::
	lock
	faceplayer
	setspeaker SP_NAME_ALLISON
	msgbox RebelHideout_Text_PostgameAllison, MSGBOX_DEFAULT
	release
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

RebelHideout_Text_DefectorAnalysis::
	.string "I analyzed your Shadow POKéMON\n"
	.string "while you rested.\p"
	.string "They deal 20 percent MORE damage…\n"
	.string "at 20 percent LESS defense.\p"
	.string "And there is a risk I cannot\n"
	.string "chart: HYPER MODE.\p"
	.string "Push one too hard and it becomes\n"
	.string "emotionally unstable in battle.\p"
	.string "Handle them gently. They were\n"
	.string "people's partners once.$"

RebelHideout_Text_DefectorPurified::
	.string "Your purified POKéMON --\n"
	.string "I ran the numbers three times.\p"
	.string "It KEPT the 20 percent attack\n"
	.string "boost. No defense loss. No\l"
	.string "HYPER MODE. No side effects.\p"
	.string "Remarkable!\p"
	.string "Catch 5 Shadows and bring them\n"
	.string "to the shrine. Every one you\l"
	.string "free weakens HIS arsenal.$"

RebelHideout_Text_BriefingLogGift::
	.string "One more thing. I kept a ledger\n"
	.string "of every Shadow he manufactured.\p"
	.string "Take it. Hunt them down.$"

RebelHideout_Text_ReceivedShadowLog::
	.string "{PLAYER} received the SHADOW LOG\n"
	.string "from PROF. ASSISTANT!$"

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

@ --- post-game text ----------------------------------------------------------
RebelHideout_Text_PostgameCongrats::
	.string "You really did it.\p"
	.string "You saved the world,\n"
	.string "JAILBREAKER.$"

RebelHideout_Text_PostgameOnTheHouse::
	.string "On the house. Go get yourself\n"
	.string "somethin' nice.$"

RebelHideout_Text_PostgameAlmostForgot::
	.string "I almost forgot --\p"
	.string "We pitched in and got you\n"
	.string "a PRIVATE JET!$"

RebelHideout_Text_PostgameFly::
	.string "Use FLY and the PJ will take\n"
	.string "you anywhere you want to go\l"
	.string "in the region.\p"
	.string "So what'll it be?\p"
	.string "STAR SUMMIT? NATIONAL PARK?\n"
	.string "Munen?$"

RebelHideout_Text_PrivateJet::
	.string "Your PRIVATE JET.\p"
	.string "Fueled, polished, and pointed\n"
	.string "at anywhere you want.$"

RebelHideout_Text_PostgameFatality::
	.string "The whole UNDERGROUND is\n"
	.string "chanting your name tonight.\p"
	.string "Rematch. Anytime. Champ.$"

RebelHideout_Text_PostgameYiffer::
	.string "Heh. Takes more than a Mutrid\n"
	.string "ambush to keep ME down.\p"
	.string "…Barely.$"

RebelHideout_Text_PostgameMasterGen::
	.string "The dojo teaches that strength\n"
	.string "exists to serve others.\p"
	.string "You are the proof, Jailbreaker.$"

RebelHideout_Text_PostgameMom::
	.string "{PLAYER}! My little world-saver!\p"
	.string "I packed snacks for the jet.\n"
	.string "Fly home soon, okay?$"

RebelHideout_Text_PostgameMikmanc::
	.string "Cell to sky, partner.\p"
	.string "Nobody will ever build a wall\n"
	.string "around this city again.$"

RebelHideout_Text_PostgameMinister::
	.string "There will be elections.\n"
	.string "Real ones.\p"
	.string "Somehow I suspect your name\n"
	.string "will be on somebody's ballot.$"

RebelHideout_Text_PostgameClarkson::
	.string "JIRACHI's readings are stable.\p"
	.string "The Shadows are waking up as\n"
	.string "themselves again -- every one\l"
	.string "you freed.\p"
	.string "Remarkable work.$"

RebelHideout_Text_PostgameEdwards::
	.string "I dropped out of the sky to\n"
	.string "break you. Twice.\p"
	.string "Instead you broke the machine.\p"
	.string "…General's honors, kid.$"

RebelHideout_Text_PostgameAllison::
	.string "The bay sings different now.\n"
	.string "Cleaner. Lighter.\p"
	.string "You did that.$"
