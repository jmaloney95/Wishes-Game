	.equ LOCALID_DOJO_MOM, 2

@ The arrival walk. DOJO_ARRIVAL_Y is where MunenHomeAct2 warps the player in
@ (keep the two in step); Mom waits one tile ahead of them.
	.equ DOJO_PATH_X,      14
	.equ DOJO_ARRIVAL_Y,   25
	.equ DOJO_MOM_START_Y, DOJO_ARRIVAL_Y - 1

Dojo_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Dojo_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Dojo_OnFrame
	.byte 0

@ The safehouse is the checkpoint for the whole Sennen assault: whiteouts in
@ SennenAct2 (which also setrespawns this) and in here come back to the cots
@ beside the Blissey healer.
Dojo_OnTransition::
	setrespawn HEAL_LOCATION_DOJO
	@ On the walk-in from Munen, Mom starts at the garden gate instead of at
	@ her post so she can lead the player up; the walk puts her template back.
	@ FLAG_ACT2_SAFEHOUSE_INTRO_DONE covers both halves of that first visit.
	goto_if_set FLAG_ACT2_SAFEHOUSE_INTRO_DONE, Dojo_OnTransition_End
	goto_if_set FLAG_HIDE_DOJO_REBELS, Dojo_OnTransition_End
	goto_if_unset FLAG_ACT2_HOME_CUTSCENE_DONE, Dojo_OnTransition_End
	setobjectxyperm LOCALID_DOJO_MOM, DOJO_PATH_X, DOJO_MOM_START_Y
	setvar VAR_TEMP_1, 1
Dojo_OnTransition_End::
	end

Dojo_OnFrame:
	map_script_2 VAR_TEMP_1, 1, Dojo_EventScript_Arrival
	map_script_2 VAR_TEMP_0, 0, Dojo_EventScript_RebellionIntro
	.2byte 0

@ ============================================================================
@  ARRIVAL (once): Mom walks the player up the garden path from the gate to
@  the temple steps, then takes her post, and the Rebellion intro follows on.
@  Both walks are the same 13 steps, so Mom stays exactly one tile ahead.
@ ============================================================================
Dojo_EventScript_Arrival::
	setvar VAR_TEMP_1, 0
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, Dojo_Movement_PlayerLookUp
	waitmovement 0
	setspeaker SP_NAME_MOM
	msgbox Dojo_Text_MomThisWay, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_DOJO_MOM, Dojo_Movement_UpThePath
	applymovement OBJ_EVENT_ID_PLAYER, Dojo_Movement_UpThePath
	waitmovement 0
	applymovement LOCALID_DOJO_MOM, Dojo_Movement_MomToPost
	waitmovement 0
	@ put her template back, or she would respawn at the gate on every visit
	setobjectxyperm LOCALID_DOJO_MOM, 12, 10
	delay 20
	goto Dojo_EventScript_RebellionIntro
	end

Dojo_Movement_PlayerLookUp:
	face_up
	step_end

Dojo_Movement_UpThePath:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

@ (14,11) -> her post at (12,10), facing the room
Dojo_Movement_MomToPost:
	walk_left
	walk_left
	walk_up
	face_down
	step_end

Dojo_Text_MomThisWay:
	.string "This way. Up the path, past the\n"
	.string "blossoms.\p"
	.string "Stay close to me.$"

@ ============================================================================
@  THE SAFEHOUSE (Act 2) -- the old dojo, where the Rebellion gathers.
@  Rebel objects are hidden by FLAG_HIDE_DOJO_REBELS (set at new game,
@  cleared by Mom in the MunenHomeAct2 cutscene).
@  ROSTER (the Frostwood Gym crew + Mom):
@    (Gen. Edwards removed 2026-07-28 -- the SNAG MACHINE now comes from
@     Mikmanc during the Act 3 jailbreak; Edwards is saved for later.)
@    REDFATALITY (GORDIE gfx)   -> TM13 ICE BEAM, a fighter's parting gift
@    MINISTER (SCOTT gfx)       -> IT expert: the SHINKANSEN KEY + he will
@                                  handle the NETWORK BREACH on assault day
@    YIFFER (HIKER gfx)          -> LEFTOVERS + keep-going encouragement
@    MIKMANC (YOUNGSTER gfx)    -> LIFE ORB + a word of courage
@    MOM                        -> the Soothe Bell + LEFTOVERS (because Mom)
@  INTRO CUTSCENE (once, ON_FRAME): camera pans up to the group, Mom:
@  "Welcome to the Rebellion." They will hold Sennen -- and some will die
@  for it -- so the player can board the Shinkansen to Shin-Tokyo.
@ ============================================================================
Dojo_EventScript_RebellionIntro::
	setvar VAR_TEMP_0, 1
	goto_if_set FLAG_HIDE_DOJO_REBELS, Dojo_EventScript_RebellionIntroEnd
	goto_if_set FLAG_ACT2_SAFEHOUSE_INTRO_DONE, Dojo_EventScript_RebellionIntroEnd
	lockall
	playbgm MUS_HG_BELL_TOWER, FALSE
	delay 30
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, Dojo_Movement_CameraPanUp
	waitmovement 0
	msgbox Dojo_Text_IntroWelcome, MSGBOX_DEFAULT
	msgbox Dojo_Text_IntroPlan, MSGBOX_DEFAULT
	msgbox Dojo_Text_IntroCost, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_CAMERA, Dojo_Movement_CameraPanDown
	waitmovement 0
	special RemoveCameraObject
	setflag FLAG_ACT2_SAFEHOUSE_INTRO_DONE
	fadedefaultbgm
	releaseall
	end

Dojo_EventScript_RebellionIntroEnd::
	end

@ The pan was written for an entrance that no longer exists: from the temple
@ steps the group is four tiles up, and twelve would run off the top.
Dojo_Movement_CameraPanUp:
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

Dojo_Movement_CameraPanDown:
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

@ --------------------------------------------------------------------------
@  Members
@ --------------------------------------------------------------------------
Dojo_EventScript_RebelMom::
	lock
	faceplayer
	goto_if_set FLAG_ACT2_REBEL_GIFT_MOM, Dojo_EventScript_RebelMomAfter
	msgbox Dojo_Text_MomGift1, MSGBOX_DEFAULT
	giveitem ITEM_SOOTHE_BELL
	setflag FLAG_ACT2_REBEL_GIFT_MOM
	msgbox Dojo_Text_MomGift2, MSGBOX_DEFAULT
	release
	end

Dojo_EventScript_RebelMomAfter::
	msgbox Dojo_Text_MomAfter, MSGBOX_DEFAULT
	release
	end

@ (Gen. Edwards was removed from the safehouse roster 2026-07-28: the SNAG
@ MACHINE is now delivered solely by Mikmanc in the Act 3 jailbreak, per
@ WoT_Act3_Canon.md Step 4. Edwards is being saved for a later appearance.)

Dojo_EventScript_RebelRedFatality::
	lock
	faceplayer
	setspeaker SP_NAME_RED_FATALITY
	goto_if_set FLAG_ACT2_REBEL_GIFT_REDFATALITY, Dojo_EventScript_RebelRedFatalityAfter
	msgbox Dojo_Text_RedFatalityGift1, MSGBOX_DEFAULT
	giveitem ITEM_TM13
	setflag FLAG_ACT2_REBEL_GIFT_REDFATALITY
	msgbox Dojo_Text_RedFatalityGift2, MSGBOX_DEFAULT
	release
	end

Dojo_EventScript_RebelRedFatalityAfter::
	msgbox Dojo_Text_RedFatalityAfter, MSGBOX_DEFAULT
	release
	end

Dojo_EventScript_RebelMinister::
	lock
	faceplayer
	setspeaker SP_NAME_MINISTER
	checkitem ITEM_SHINKANSEN_KEY
	goto_if_eq VAR_RESULT, TRUE, Dojo_EventScript_RebelMinisterAfter
	msgbox Dojo_Text_MinisterGift1, MSGBOX_DEFAULT
	giveitem ITEM_SHINKANSEN_KEY
	msgbox Dojo_Text_MinisterGift2, MSGBOX_DEFAULT
	release
	end

Dojo_EventScript_RebelMinisterAfter::
	msgbox Dojo_Text_MinisterAfter, MSGBOX_DEFAULT
	release
	end

Dojo_EventScript_RebelMikmanc::
	lock
	faceplayer
	setspeaker SP_NAME_MIKMANC
	goto_if_set FLAG_ACT2_REBEL_GIFT_MIKMANC, Dojo_EventScript_RebelMikmancAfter
	msgbox Dojo_Text_MikmancGift1, MSGBOX_DEFAULT
	giveitem ITEM_LIFE_ORB
	setflag FLAG_ACT2_REBEL_GIFT_MIKMANC
	msgbox Dojo_Text_MikmancGift2, MSGBOX_DEFAULT
	release
	end

Dojo_EventScript_RebelMikmancAfter::
	msgbox Dojo_Text_MikmancAfter, MSGBOX_DEFAULT
	release
	end

Dojo_EventScript_RebelYiffer::
	lock
	faceplayer
	setspeaker SP_NAME_YIFFER
	goto_if_set FLAG_ACT2_REBEL_GIFT_YIFFER, Dojo_EventScript_RebelYifferAfter
	msgbox Dojo_Text_YifferGift1, MSGBOX_DEFAULT
	giveitem ITEM_LEFTOVERS
	setflag FLAG_ACT2_REBEL_GIFT_YIFFER
	msgbox Dojo_Text_YifferGift2, MSGBOX_DEFAULT
	release
	end

Dojo_EventScript_RebelYifferAfter::
	msgbox Dojo_Text_YifferAfter, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  Text
@ ============================================================================
Dojo_Text_IntroWelcome::
	.string "Welcome to the Rebellion,\n"
	.string "{PLAYER}.$"

Dojo_Text_IntroBrief::
	.string "Mutrid has taken over the\n"
	.string "region. they're building a\l"
	.string "machine that corrupts pokemon\l"
	.string "and brainwashes people.\p"
	.string "We need to stop them.\p"
	.string "The plan is simple, love.\n"
	.string "We fight them at Sennen and\l"
	.string "take back the station.\p"
	.string "You'll board the SHINKANSEN and\n"
	.string "travel to SHIN-TOKYO, Mutrid's\l"
	.string "evil HQ.\p"
	.string "Find a way to stop the machine.$"

Dojo_Text_IntroPlan::
	.string "Trainers from\n"
	.string "every corner of this region.\p"
	.string "And every one of them is here\n"
	.string "because of what YOU did.\p"
	.string "You came down from the mountain\n"
	.string "when no one comes down. You\l"
	.string "crossed the veil and came BACK.\p"
	.string "If anyone alive can walk into\n"
	.string "Shin-Tokyo and stop that machine,\l"
	.string "it's you.$"

Dojo_Text_IntroCost::
	.string "{PLAYER}. Look at me.\p"
	.string "Some of the people in this room\n"
	.string "will not make it.\p"
	.string "They know it. They chose it.\n"
	.string "So when that train pulls away,\l"
	.string "you don't look back.\p"
	.string "You look FORWARD.\n"
	.string "And you END this.$"

Dojo_Text_MomGift1::
	.string "MOM: Before the fighting starts,\n"
	.string "I have something for you.\p"
	.string "It hung over your crib, this\n"
	.string "little bell. Your father tied it\l"
	.string "there the day you were born.\p"
	.string "He said the sound would keep bad\n"
	.string "dreams from settling on you.$"

Dojo_Text_MomGift2::
	.string "MOM: Tie it to your bag. Or to a\n"
	.string "Pokemon that frets.\p"
	.string "And when you hear it ring,\n"
	.string "somewhere out there in all that\l"
	.string "noise and dark…\p"
	.string "that's me, love.\n"
	.string "That's me.$"

Dojo_Text_YifferGift1::
	.string "Big day tomorrow. You eaten?\p"
	.string "Didn't think so. A fighter runs\n"
	.string "on scraps and spite.\p"
	.string "Here, take mine.$"

Dojo_Text_YifferGift2::
	.string "LEFTOVERS. Best\n"
	.string "item ever made. Your partner just\l"
	.string "keeps going... bite by bite.\p"
	.string "See you on the platform.$"

Dojo_Text_YifferAfter::
	.string "Chew slow, breathe slower.\p"
	.string "Spite's the real meal.$"

Dojo_Text_MomAfter::
	.string "MOM: You can do it sweetie.$"

Dojo_Text_RedFatalityGift1::
	.string "Hey, {PLAYER}.\n"
	.string "Remember me? Frostwood.\p"
	.string "You walked into my gym with cold\n"
	.string "feet and walked out with my\l"
	.string "badge. Gym's frozen shut now.$"

Dojo_Text_RedFatalityGift2::
	.string "TM13. ICE BEAM.\n"
	.string "My signature.\p"
	.string "It's a parting gift.\n"
	.string "I'll see you on the other side,\n"
	.string "dog.$"

Dojo_Text_RedFatalityAfter::
	.string "See you on the other side, slugger.$"

Dojo_Text_MinisterGift1::
	.string "Everyone thinks a rebellion is\n"
	.string "muscle. It's not. It's UPTIME.\p"
	.string "Mutrid runs the whole line off\n"
	.string "one network: cameras, door locks,\l"
	.string "track signals, the lot.\p"
	.string "I've been inside their system for\n"
	.string "days. They still think I'm a\l"
	.string "vending machine in Sennen.$"

Dojo_Text_MinisterGift2::
	.string "That's the SHINKANSEN train\n"
	.string "keycard. Cloned it off their own\l"
	.string "sysadmin.\p"
	.string "Here's how tomorrow goes:\p"
	.string "The moment we hit the platform, I\n"
	.string "breach their network. Cameras\l"
	.string "loop. Door locks forget you\l"
	.string "exist. Track signals go green,\l"
	.string "all the way east.\p"
	.string "You swipe, you board, you're a\n"
	.string "GHOST on their own rail.\p"
	.string "The breach is my job. The train\n"
	.string "is yours.$"

Dojo_Text_MinisterAfter::
	.string "Tomorrow everything goes dark.$"

Dojo_Text_MikmancGift1::
	.string "{PLAYER}!! It's me,\n"
	.string "MikManc! From the gym!\p"
	.string "I've got something for you.$"

Dojo_Text_MikmancGift2::
	.string "LIFE ORB. It costs a\n"
	.string "little to hit a LOT harder.\p"
	.string "...When you make it out there\n"
	.string ", hit harder, {PLAYER}.\p"
	.string "For all of us.$"

Dojo_Text_MikmancAfter::
	.string "I'm not scared.\p"
	.string "Okay. I'm a LITTLE scared.$"
