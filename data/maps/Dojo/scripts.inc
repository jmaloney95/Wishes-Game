Dojo_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Dojo_OnFrame
	.byte 0

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
	.string "MOM: Welcome to the Rebellion,\n"
	.string "{PLAYER}.$"

Dojo_Text_IntroPlan::
	.string "MOM: Look at them. Trainers from\n"
	.string "every corner of this region --\l"
	.string "Frostwood, the Ashlands, towns\l"
	.string "whose names Mutrid already burned\l"
	.string "off the map.\p"
	.string "And every one of them is here\n"
	.string "because of what YOU did.\p"
	.string "You came down from the mountain\n"
	.string "when no one comes down. You\l"
	.string "crossed the veil and came BACK.\p"
	.string "If anyone alive can walk into\n"
	.string "Shin-Tokyo and stop that machine,\l"
	.string "it's you. It was always going to\l"
	.string "be you.\p"
	.string "The plan is simple, love. We\n"
	.string "fight through SENNEN. We take the\l"
	.string "station. Every mask in the region\l"
	.string "comes running to stop us...\p"
	.string "...and while they're looking at\n"
	.string "US, you board the SHINKANSEN.\l"
	.string "Alone. Straight into their\l"
	.string "throat.$"

Dojo_Text_IntroCost::
	.string "MOM: {PLAYER}. Look at me.\p"
	.string "Some of the people in this room\n"
	.string "will not see the morning after.\p"
	.string "They know it. They chose it.\n"
	.string "Every single one of them chose\l"
	.string "it, with their eyes open.\p"
	.string "So when that train pulls away,\n"
	.string "you don't look back at us.\p"
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
	.string "YIFFER: Yo. Yiffer. Frostwood\n"
	.string "gym. The hiker you flattened\l"
	.string "between the ice puzzles.\p"
	.string "I'm not a planner like Minister\n"
	.string "or a legend like the boss. But I\l"
	.string "know what a long march needs.$"

Dojo_Text_YifferGift2::
	.string "YIFFER: LEFTOVERS. Best trail\n"
	.string "item ever made. Your partner just\l"
	.string "keeps going... bite by bite.\p"
	.string "That's all any of us have to do\n"
	.string "tomorrow, {PLAYER}. Keep going.\l"
	.string "Bite by bite.\p"
	.string "See you on the platform.$"

Dojo_Text_YifferAfter::
	.string "YIFFER: Ate my own leftovers\n"
	.string "already. No regrets.\p"
	.string "Keep going, {PLAYER}.\n"
	.string "Bite by bite.$"

Dojo_Text_MomAfter::
	.string "MOM: Warm socks, healed Pokemon,\n"
	.string "and no heroics before breakfast.\p"
	.string "...I know, I know. Go on.\n"
	.string "Let me look at you one more time.$"

Dojo_Text_RedFatalityGift1::
	.string "REDFATALITY: Hey, {PLAYER}.\n"
	.string "Remember me? Frostwood.\p"
	.string "You walked into my gym with cold\n"
	.string "feet and walked out with my\l"
	.string "badge. Best loss I ever took.\p"
	.string "Gym's frozen SHUT now. Mutrid\n"
	.string "runs coolant lines through my\l"
	.string "battlefield.\p"
	.string "So I've got no gym, no gate to\n"
	.string "keep... and one thing left worth\l"
	.string "giving.$"

Dojo_Text_RedFatalityGift2::
	.string "REDFATALITY: TM13. ICE BEAM.\n"
	.string "My signature.\p"
	.string "A fighter's parting gift, kid.\n"
	.string "When you're on that train and it\l"
	.string "all goes loud--\p"
	.string "--aim like Frostwood is watching.\n"
	.string "Because we will be.$"

Dojo_Text_RedFatalityAfter::
	.string "REDFATALITY: Tomorrow I hold a\n"
	.string "platform instead of a gym.\p"
	.string "Same job, really. Nobody gets\n"
	.string "past me without earning it.$"

Dojo_Text_MinisterGift1::
	.string "MINISTER: Remember me, {PLAYER}?\n"
	.string "Frostwood gym. Back row. The one\l"
	.string "with the laptop.\p"
	.string "Everyone thinks a rebellion is\n"
	.string "muscle. It's not. It's UPTIME.\p"
	.string "Mutrid runs the whole line off\n"
	.string "one network: cameras, door locks,\l"
	.string "track signals, the lot.\p"
	.string "I've been inside their system for\n"
	.string "nine days. They still think I'm a\l"
	.string "vending machine in Sennen.$"

Dojo_Text_MinisterGift2::
	.string "MINISTER: That's the driver's\n"
	.string "keycard. Cloned it off their own\l"
	.string "sysadmin. The only copy Mutrid\l"
	.string "doesn't control.\p"
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
	.string "MINISTER: Nine days in their\n"
	.string "network and nobody noticed.\p"
	.string "Tomorrow they notice.\n"
	.string "Tomorrow EVERYTHING goes dark.$"

Dojo_Text_MikmancGift1::
	.string "MIKMANC: {PLAYER}!! It's me,\n"
	.string "MikManc! From the gym!\p"
	.string "You beat me in like four turns\n"
	.string "and I've told EVERYONE it was\l"
	.string "nine.\p"
	.string "I don't have a train key or army\n"
	.string "tech or a famous TM. I just have\l"
	.string "this. Saved up for it my whole\l"
	.string "certification.$"

Dojo_Text_MikmancGift2::
	.string "MIKMANC: LIFE ORB. It costs a\n"
	.string "little to hit a LOT harder.\p"
	.string "Kind of like tomorrow, right?\n"
	.string "It costs a little. It hits a lot\l"
	.string "harder.\p"
	.string "...When you're out there past\n"
	.string "where we can follow -- hit\l"
	.string "harder, {PLAYER}.\p"
	.string "For all of us.$"

Dojo_Text_MikmancAfter::
	.string "MIKMANC: I'm not scared.\p"
	.string "Okay. I'm a LITTLE scared.\n"
	.string "But I'm still going to be on\l"
	.string "that platform tomorrow.\p"
	.string "Four turns, {PLAYER}. Tell them\n"
	.string "it took nine.$"
