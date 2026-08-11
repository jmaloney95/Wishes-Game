Dojo_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Dojo_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Dojo_OnFrame
	.byte 0

@ The safehouse is the checkpoint for the whole Sennen assault: whiteouts in
@ SennenAct2 (which also setrespawns this) and in here come back to the cots
@ beside the Blissey healer.
Dojo_OnTransition::
	setrespawn HEAL_LOCATION_DOJO
	end

Dojo_OnFrame:
	map_script_2 VAR_TEMP_0, 0, Dojo_EventScript_RebellionIntro
	.2byte 0

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

Dojo_Movement_CameraPanUp:
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

Dojo_Movement_CameraPanDown:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
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
	.string "noise and dark--\p"
	.string "--that's me, love.\n"
	.string "That's me.$"

Dojo_Text_YifferGift1::
	.string "Big day tomorrow. You eaten?\p"
	.string "Didn't think so. A fighter runs\n"
	.string "on scraps and spite.\p"
	.string "Here -- take mine.$"

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
	.string " -- hit harder, {PLAYER}.\p"
	.string "For all of us.$"

Dojo_Text_MikmancAfter::
	.string "I'm not scared.\p"
	.string "Okay. I'm a LITTLE scared.$"
