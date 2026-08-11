	.equ LOCALID_ARENA_ANNOUNCER, 1
	.equ LOCALID_ARENA_DOORMAN, 9
	.equ LOCALID_ARENA_LOSER, 13
	.equ LOCALID_ARENA_VOLTEN, 15
	.equ LOCALID_ARENA_CHALLENGER_1, 16
	.equ LOCALID_ARENA_CHALLENGER_2, 17

UndergroundArena_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, UndergroundArena_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, UndergroundArena_OnFrame
	.byte 0

@ ============================================================================
@  THE UNDERGROUND ARENA -- Shin-Tokyo's illegal fight circuit, and the only
@  place left in the city where trainers still battle. Mutrid outlawed it
@  above ground, so the whole scene moved into the tunnels.
@  Beating the three-bout tournament (two challengers, then the champion
@  VOLTEN) awards the UNDERGROUND BADGE -- which IS FLAG_BADGE06_GET, so it
@  also unlocks FLY in the field (IsFieldMoveUnlocked_Fly keys on that badge)
@  -- plus the LIFE ORB. The gauntlet is one locked scene: no healing between
@  bouts. All dialogue PLACEHOLDER (Joe).
@ ============================================================================

UndergroundArena_OnTransition::
	@ The two gauntlet challengers are scene-only.
	setflag FLAG_HIDE_ARENA_CHALLENGERS
	@ Sprites for the map's VAR_0 / VAR_1 objects: the champion in the ring
	@ and the fighter he just beat. Swap these two ids to change their look
	@ -- no script edits needed.
	setvar VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_WATTSON
	setvar VAR_OBJ_GFX_ID_1, OBJ_EVENT_GFX_CAMPER
	end

UndergroundArena_OnFrame:
	map_script_2 VAR_TEMP_1, 0, UndergroundArena_EventScript_TitleBout
	.2byte 0

@ --- Entry cutscene: a title bout, live ------------------------------------
UndergroundArena_EventScript_TitleBout::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_set FLAG_WOT_ARENA_INTRO_SEEN, UndergroundArena_EventScript_TitleBoutDone
	setflag FLAG_WOT_ARENA_INTRO_SEEN
	delay 20
	@ mid-battle: the crowd is already screaming
	playse SE_M_SWAGGER
	applymovement LOCALID_ARENA_VOLTEN, UndergroundArena_Movement_FaceLeft
	waitmovement LOCALID_ARENA_VOLTEN
	msgbox UndergroundArena_Text_Intro1, MSGBOX_DEFAULT
	closemessage
	delay 20
	playse SE_M_THUNDERBOLT
	applymovement LOCALID_ARENA_LOSER, UndergroundArena_Movement_LoserReels
	waitmovement LOCALID_ARENA_LOSER
	delay 20
	applymovement LOCALID_ARENA_LOSER, UndergroundArena_Movement_LoserOut
	waitmovement LOCALID_ARENA_LOSER
	removeobject LOCALID_ARENA_LOSER
	@ the announcer calls it
	applymovement LOCALID_ARENA_ANNOUNCER, UndergroundArena_Movement_Emote
	waitmovement LOCALID_ARENA_ANNOUNCER
	msgbox UndergroundArena_Text_Intro2, MSGBOX_DEFAULT
	closemessage
	playse SE_APPLAUSE
	delay 30
	msgbox UndergroundArena_Text_Intro3, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ARENA_VOLTEN, UndergroundArena_Movement_FaceDown
	waitmovement LOCALID_ARENA_VOLTEN
UndergroundArena_EventScript_TitleBoutDone::
	releaseall
	end

@ --- The announcer: sign-up desk -------------------------------------------
UndergroundArena_EventScript_Announcer::
	lock
	faceplayer
	goto_if_set FLAG_WOT_ARENA_WON, UndergroundArena_EventScript_AnnouncerChamp
	goto_if_set FLAG_WOT_ARENA_ENTERED, UndergroundArena_EventScript_AnnouncerWaiting
	msgbox UndergroundArena_Text_AnnouncerOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, UndergroundArena_EventScript_AnnouncerDecline
	closemessage
	@ "WAIT -- we have a last-minute entry!"
	applymovement LOCALID_ARENA_ANNOUNCER, UndergroundArena_Movement_Emote
	waitmovement LOCALID_ARENA_ANNOUNCER
	msgbox UndergroundArena_Text_AnnouncerEntry, MSGBOX_DEFAULT
	closemessage
	playse SE_APPLAUSE
	delay 60
	setflag FLAG_WOT_ARENA_ENTERED
	msgbox UndergroundArena_Text_AnnouncerToRing, MSGBOX_DEFAULT
	release
	end

UndergroundArena_EventScript_AnnouncerDecline::
	msgbox UndergroundArena_Text_AnnouncerDecline, MSGBOX_DEFAULT
	release
	end

UndergroundArena_EventScript_AnnouncerWaiting::
	msgbox UndergroundArena_Text_AnnouncerToRing, MSGBOX_DEFAULT
	release
	end

UndergroundArena_EventScript_AnnouncerChamp::
	msgbox UndergroundArena_Text_AnnouncerChamp, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  THE TOURNAMENT (ring trigger at 4,6)
@  One unbroken locked scene: each challenger walks to (13,6), fights, and is
@  replaced immediately -- the player never regains control, so there is no
@  healing between bouts. Losing whites out and the run resets.
@ ============================================================================
UndergroundArena_EventScript_Tournament::
	lockall
	setvar VAR_TEMP_2, 1
	goto_if_unset FLAG_WOT_ARENA_ENTERED, UndergroundArena_EventScript_TournamentSkip
	goto_if_set FLAG_WOT_ARENA_WON, UndergroundArena_EventScript_TournamentSkip
	@ Repeat runs must start clean (a loss leaves the defeat flags set).
	cleartrainerflag TRAINER_DUDLEY
	cleartrainerflag TRAINER_KAYLEE
	cleartrainerflag TRAINER_TERRY
	applymovement OBJ_EVENT_ID_PLAYER, UndergroundArena_Movement_FaceRight
	waitmovement 0
	msgbox UndergroundArena_Text_RoundOne, MSGBOX_DEFAULT
	closemessage
	playse SE_APPLAUSE
	delay 40
	@ -- BOUT 1 --
	addobject LOCALID_ARENA_CHALLENGER_1
	applymovement LOCALID_ARENA_CHALLENGER_1, UndergroundArena_Movement_ChallengerIn
	waitmovement LOCALID_ARENA_CHALLENGER_1
	msgbox UndergroundArena_Text_Challenger1, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro TRAINER_DUDLEY, UndergroundArena_Text_Challenger1Lose
	removeobject LOCALID_ARENA_CHALLENGER_1
	msgbox UndergroundArena_Text_RoundTwo, MSGBOX_DEFAULT
	closemessage
	playse SE_APPLAUSE
	delay 40
	@ -- BOUT 2 --
	addobject LOCALID_ARENA_CHALLENGER_2
	applymovement LOCALID_ARENA_CHALLENGER_2, UndergroundArena_Movement_ChallengerIn
	waitmovement LOCALID_ARENA_CHALLENGER_2
	msgbox UndergroundArena_Text_Challenger2, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro TRAINER_KAYLEE, UndergroundArena_Text_Challenger2Lose
	removeobject LOCALID_ARENA_CHALLENGER_2
	msgbox UndergroundArena_Text_FinalRound, MSGBOX_DEFAULT
	closemessage
	playse SE_APPLAUSE
	delay 50
	@ -- THE CHAMPION --
	applymovement LOCALID_ARENA_VOLTEN, UndergroundArena_Movement_VoltenToRing
	waitmovement LOCALID_ARENA_VOLTEN
	msgbox UndergroundArena_Text_VoltenIntro, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro TRAINER_TERRY, UndergroundArena_Text_VoltenLose
	msgbox UndergroundArena_Text_VoltenBeaten, MSGBOX_DEFAULT
	closemessage
	playse SE_APPLAUSE
	delay 40
	msgbox UndergroundArena_Text_NewChamp, MSGBOX_DEFAULT
	closemessage
	@ -- the badge --
	setflag FLAG_WOT_ARENA_WON
	playfanfare MUS_OBTAIN_BADGE
	msgbox UndergroundArena_Text_ReceivedBadge, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_BADGE06_GET
	msgbox UndergroundArena_Text_VoltenFly, MSGBOX_DEFAULT
	closemessage
	giveitem ITEM_LIFE_ORB
	msgbox UndergroundArena_Text_VoltenAfter, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_ARENA_VOLTEN, UndergroundArena_Movement_VoltenBack
	waitmovement LOCALID_ARENA_VOLTEN
UndergroundArena_EventScript_TournamentSkip::
	setvar VAR_TEMP_2, 0
	releaseall
	end

@ --- Volten, off the clock --------------------------------------------------
UndergroundArena_EventScript_Volten::
	lock
	faceplayer
	goto_if_set FLAG_WOT_ARENA_WON, UndergroundArena_EventScript_VoltenPost
	msgbox UndergroundArena_Text_VoltenIdle, MSGBOX_DEFAULT
	release
	end

UndergroundArena_EventScript_VoltenPost::
	msgbox UndergroundArena_Text_VoltenPost, MSGBOX_DEFAULT
	release
	end

@ --- The crowd --------------------------------------------------------------
UndergroundArena_EventScript_CrowdBanned::
	msgbox UndergroundArena_Text_CrowdBanned, MSGBOX_NPC
	end

UndergroundArena_EventScript_CrowdFightClub::
	msgbox UndergroundArena_Text_CrowdFightClub, MSGBOX_NPC
	end

UndergroundArena_EventScript_CrowdVolten::
	msgbox UndergroundArena_Text_CrowdVolten, MSGBOX_NPC
	end

UndergroundArena_EventScript_CrowdBetting::
	msgbox UndergroundArena_Text_CrowdBetting, MSGBOX_NPC
	end

UndergroundArena_EventScript_CrowdKid::
	msgbox UndergroundArena_Text_CrowdKid, MSGBOX_NPC
	end

UndergroundArena_EventScript_CrowdNoisy::
	msgbox UndergroundArena_Text_CrowdNoisy, MSGBOX_NPC
	end

UndergroundArena_EventScript_Doorman::
	lock
	faceplayer
	msgbox UndergroundArena_Text_Doorman, MSGBOX_DEFAULT
	release
	end

@ --- Movement ---------------------------------------------------------------
UndergroundArena_Movement_FaceLeft:
	face_left
	step_end

UndergroundArena_Movement_FaceRight:
	face_right
	step_end

UndergroundArena_Movement_FaceDown:
	face_down
	step_end

UndergroundArena_Movement_Emote:
	emote_exclamation_mark
	delay_16
	step_end

UndergroundArena_Movement_LoserReels:
	face_right
	delay_16
	walk_left
	emote_x
	delay_16
	step_end

UndergroundArena_Movement_LoserOut:
	walk_fast_left
	walk_fast_left
	walk_fast_up
	step_end

@ Challengers come in from the right-hand tunnel to the mark at (13,6).
UndergroundArena_Movement_ChallengerIn:
	walk_left
	walk_left
	walk_left
	walk_left
	face_left
	step_end

UndergroundArena_Movement_VoltenToRing:
	walk_right
	walk_right
	face_left
	step_end

UndergroundArena_Movement_VoltenBack:
	walk_left
	walk_left
	face_down
	step_end

@ --- Text (all PLACEHOLDER (Joe)) -------------------------------------------
UndergroundArena_Text_Intro1::
	.string "The tunnel opens into noise.\p"
	.string "Below, under a ring of stolen\n"
	.string "floodlights, two trainers are\l"
	.string "still going at it.$"

UndergroundArena_Text_Intro2::
	.string "ANNOUNCER: --AND THAT'S THE\n"
	.string "MATCH!\p"
	.string "The challenger is DOWN, folks!\n"
	.string "What a run they had!$"

UndergroundArena_Text_Intro3::
	.string "ANNOUNCER: STILL your undisputed\n"
	.string "champion of the UNDERGROUND\l"
	.string "ARENA --\p"
	.string "V O L T E N !\p"
	.string "The tunnel shakes. Somewhere\n"
	.string "above, Mutrid hears nothing.$"

UndergroundArena_Text_AnnouncerOffer::
	.string "You're not here to watch, are\n"
	.string "you. I know that face.\p"
	.string "Three bouts, back to back. No\n"
	.string "breaks, no potions between --\l"
	.string "you go in with what you've got.\p"
	.string "Want in?$"

UndergroundArena_Text_AnnouncerDecline::
	.string "Smart. Come back when your\n"
	.string "team's ready.$"

UndergroundArena_Text_AnnouncerEntry::
	.string "ANNOUNCER: WAIT! WAIT --\p"
	.string "WE HAVE A LAST-MINUTE ENTRY!\p"
	.string "{PLAYER}!!$"

UndergroundArena_Text_AnnouncerToRing::
	.string "The crowd is on its feet.\p"
	.string "Step onto the mark when you're\n"
	.string "ready, kid. Ring's yours.$"

UndergroundArena_Text_AnnouncerChamp::
	.string "Ladies and gentlemen: the\n"
	.string "champion is in the building.\p"
	.string "He winks at you.$"

UndergroundArena_Text_RoundOne::
	.string "ANNOUNCER: ROUND ONE!\p"
	.string "Give it up for the challenger!$"

UndergroundArena_Text_Challenger1::
	.string "Do your worst.$"

UndergroundArena_Text_Challenger1Lose::
	.string "...You did your worst.$"

UndergroundArena_Text_RoundTwo::
	.string "ANNOUNCER: ROUND TWO -- no rest,\n"
	.string "that's the underground rule!$"

UndergroundArena_Text_Challenger2::
	.string "You're still standing?\p"
	.string "Let's fix that.$"

UndergroundArena_Text_Challenger2Lose::
	.string "Tch. Still standing.$"

UndergroundArena_Text_FinalRound::
	.string "ANNOUNCER: FINAL ROUND!\p"
	.string "You know who's next.\n"
	.string "You've ALL known who's next!$"

UndergroundArena_Text_VoltenIntro::
	.string "Two down. On no rest.\p"
	.string "They shut the gyms, kid. Took\n"
	.string "the badges, took the lights.\p"
	.string "So we dug a hole and kept\n"
	.string "battling in it.\p"
	.string "Show me it was worth digging.$"

UndergroundArena_Text_VoltenLose::
	.string "HA! There it is!$"

UndergroundArena_Text_VoltenBeaten::
	.string "Beaten. Down here. By a kid\n"
	.string "with a bounty on their face.\p"
	.string "Best night this pit's ever had.$"

UndergroundArena_Text_NewChamp::
	.string "ANNOUNCER: NEW CHAMPION!\n"
	.string "NEW CHAMPION!!\p"
	.string "The tunnel goes absolutely\n"
	.string "feral.$"

UndergroundArena_Text_ReceivedBadge::
	.string "{PLAYER} earned the\n"
	.string "UNDERGROUND BADGE!$"

UndergroundArena_Text_VoltenFly::
	.string "That badge isn't official.\p"
	.string "It's better. Every flier in the\n"
	.string "resistance answers to it.\p"
	.string "Whistle from anywhere and\n"
	.string "they'll come carry you.$"

UndergroundArena_Text_VoltenAfter::
	.string "Take this too.\p"
	.string "A LIFE ORB. It costs you to\n"
	.string "swing that hard.\p"
	.string "Down here, everything does.$"

UndergroundArena_Text_VoltenIdle::
	.string "Champion of a hole in the\n"
	.string "ground.\p"
	.string "Still counts.$"

UndergroundArena_Text_VoltenPost::
	.string "Ring's yours whenever you want\n"
	.string "it back, champ.\p"
	.string "I'll be here. Somebody has to\n"
	.string "keep the lights on.$"

UndergroundArena_Text_CrowdBanned::
	.string "Mutrid shut battling down all\n"
	.string "over Shin-Tokyo.\p"
	.string "Said it was “unproductive.”\p"
	.string "So we hold the tournament down\n"
	.string "here instead.$"

UndergroundArena_Text_CrowdFightClub::
	.string "The first rule of fight club\n"
	.string "is we don't talk about fight\l"
	.string "club.\p"
	.string "...The second rule is I've\n"
	.string "never been good at rule one.$"

UndergroundArena_Text_CrowdVolten::
	.string "VOLTEN's held that ring for\n"
	.string "two hundred nights.\p"
	.string "Nobody's even taken him to a\n"
	.string "last mon.$"

UndergroundArena_Text_CrowdBetting::
	.string "I've got three weeks' rations\n"
	.string "riding on the champ.\p"
	.string "Don't you dare.$"

UndergroundArena_Text_CrowdKid::
	.string "When I grow up I'm gonna battle\n"
	.string "in the ring!\p"
	.string "Mom says the ring is illegal.\n"
	.string "Mom's HERE, though.$"

UndergroundArena_Text_CrowdNoisy::
	.string "Keep your voice down up top.\p"
	.string "Down here? SCREAM.$"

UndergroundArena_Text_Doorman::
	.string "Password's changed twice this\n"
	.string "week.\p"
	.string "You got in, so I guess somebody\n"
	.string "likes you.$"
