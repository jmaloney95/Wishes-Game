	.set LOCALID_STARSUMMIT_JIRACHI, 1
	.set LOCALID_STARSUMMIT_PROF, 2
	.set LOCALID_STARSUMMIT_GOON, 3
	.set LOCALID_STARSUMMIT_GOON_TOP1, 4
	.set LOCALID_STARSUMMIT_GOON_TOP2, 5

StarSummit_MapScripts::
	.byte 0

@==============================================================================
@ ARRIVAL CUTSCENE -- fires at the top of the stairs (triggers @15-16,y18).
@  1) Pan up to JIRACHI; it cries (sound + on-screen text).
@  2) Drop to PROF. CLARKSON, fallen before a masked goon.
@  3) Clarkson warns -> shove -> second warning -> "Goodbye, Father" -> final
@     shove into the lava; he sizzles and is GONE for good.
@  4) The goon walks down to the player and challenges: the impossible battle
@     (Typhlosion + Feraligatr Lv50). Win OR lose, JIRACHI is taken.
@==============================================================================
StarSummit_EventScript_IntroCutscene::
	lockall
	goto_if_set FLAG_STARSUMMIT_INTRO_SEEN, StarSummit_EventScript_IntroDone
	delay 20
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, StarSummit_Movement_CameraToJirachi
	waitmovement 0
	delay 20
	@ JIRACHI cries out -- sound AND text on screen
	waitse
	playmoncry SPECIES_JIRACHI, CRY_MODE_ENCOUNTER
	msgbox StarSummit_Text_JirachiCry, MSGBOX_DEFAULT
	waitmoncry
	closemessage
	delay 15
	@ drop to Clarkson + the masked goon
	applymovement LOCALID_CAMERA, StarSummit_Movement_CameraToProf
	waitmovement 0
	delay 20
	setspeaker SP_NAME_CLARKSON
	msgbox StarSummit_Text_ClarksonLine1, MSGBOX_DEFAULT
	closemessage
	delay 10
	@ first shove -- one tile toward the edge
	applymovement LOCALID_STARSUMMIT_GOON, StarSummit_Movement_GoonStepRight
	applymovement LOCALID_STARSUMMIT_PROF, StarSummit_Movement_ProfPush
	waitmovement 0
	delay 20
	setspeaker SP_NAME_CLARKSON
	msgbox StarSummit_Text_ClarksonLine2, MSGBOX_DEFAULT
	closemessage
	delay 10
	@ "Goodbye, Father" -> final shove into the lava
	setspeaker SP_NAME_MUTRID_LEADER
	msgbox StarSummit_Text_GoonGoodbye, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_STARSUMMIT_GOON, StarSummit_Movement_GoonStepRight
	applymovement LOCALID_STARSUMMIT_PROF, StarSummit_Movement_ProfIntoLava
	waitmovement 0
	playse SE_LAVARIDGE_FALL_WARP
	delay 28
	@ Clarkson is gone for GOOD -- hide flag BEFORE removeobject so the engine
	@ can't respawn him when the camera moves.
	setflag FLAG_HIDE_STARSUMMIT_PROF
	removeobject LOCALID_STARSUMMIT_PROF
	delay 30
	@ camera settles back on the player, then the masked figure descends ON them
	applymovement LOCALID_CAMERA, StarSummit_Movement_CameraToPlayer
	waitmovement 0
	special RemoveCameraObject
	delay 10
	applymovement LOCALID_STARSUMMIT_GOON, StarSummit_Movement_GoonApproachPlayer
	waitmovement 0
	delay 12
	msgbox StarSummit_Text_GoonChallenge, MSGBOX_DEFAULT
	closemessage
	@ Lock the story beat in NOW.
	setflag FLAG_STARSUMMIT_INTRO_SEEN
	setflag FLAG_JIRACHI_CAPTURED
	@ ===== THE IMPOSSIBLE BATTLE =====
	@ B_FLAG_NO_WHITEOUT keeps control on a LOSS (the intended outcome). Win OR
	@ lose, the script continues below.
	setflag B_FLAG_NO_WHITEOUT
	trainerbattle_no_intro TRAINER_STARSUMMIT_BOSS, StarSummit_Text_GoonDefeat
	clearflag B_FLAG_NO_WHITEOUT
	@ party is likely fainted -- revive it so the scene (and the next city) work
	special HealPlayerParty
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, StarSummit_EventScript_BossWon
	msgbox StarSummit_Text_GoonAfterLoss, MSGBOX_DEFAULT
	goto StarSummit_EventScript_BossAftermath

StarSummit_EventScript_BossWon::
	msgbox StarSummit_Text_GoonAfterWin, MSGBOX_DEFAULT
	goto StarSummit_EventScript_BossAftermath

@ The two masked figures cross the bridge to flank JIRACHI (14,6)/(15,6); JIRACHI
@ cries; the screen fades to WHITE (the capture). Then the Act II title card.
StarSummit_EventScript_BossAftermath::
	closemessage
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, StarSummit_Movement_CameraToTop
	waitmovement 0
	delay 40
	@ the two masked figures cross the bridge (slow) to flank JIRACHI (14,6)/(15,6)
	applymovement LOCALID_STARSUMMIT_GOON_TOP1, StarSummit_Movement_GoonCrossBridge
	applymovement LOCALID_STARSUMMIT_GOON_TOP2, StarSummit_Movement_GoonCrossBridge
	waitmovement 0
	delay 50
	@ JIRACHI cries, then disappears
	waitse
	playmoncry SPECIES_JIRACHI, CRY_MODE_ENCOUNTER
	waitmoncry
	delay 40
	playse SE_M_TELEPORT
	setflag FLAG_HIDE_STARSUMMIT_JIRACHI
	removeobject LOCALID_STARSUMMIT_JIRACHI
	delay 60
	@ the goons walk back across the bridge (slow)
	applymovement LOCALID_STARSUMMIT_GOON_TOP1, StarSummit_Movement_GoonCrossBridgeBack
	applymovement LOCALID_STARSUMMIT_GOON_TOP2, StarSummit_Movement_GoonCrossBridgeBack
	waitmovement 0
	delay 50
	@ slow fade to WHITE -- the capture. Tidy camera under cover of white.
	@ (speed is the per-step frame delay -- raise it again for an even slower fade.)
	fadescreenspeed FADE_TO_WHITE, 12
	special RemoveCameraObject
	@ long, heavy hold before the act break
	delay 150
	@ a beat... then the Act II title
	msgbox StarSummit_Text_PreTitle, MSGBOX_DEFAULT
	closemessage
	delay 40
	@ Act II title on the white screen. NOTE: script-only version (framed message
	@ box). For a true fade-IN title card, see the follow-up.
	msgbox StarSummit_Text_ActII, MSGBOX_DEFAULT
	closemessage
	delay 30
	@ Act II opens: the player comes to in the eerie HOT SPRING.
	warp MAP_HOT_SPRING, 15, 13
	waitstate
	releaseall
	end

StarSummit_EventScript_IntroDone::
	releaseall
	end

@==============================================================================
@ MOVEMENTS
@==============================================================================
@ From the y18 trigger: up 11 -> Jirachi (y7); down 5 -> Prof (y12); down 6 -> player.
StarSummit_Movement_CameraToJirachi:
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

StarSummit_Movement_CameraToProf:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

StarSummit_Movement_CameraToPlayer:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

@ Capture cutscene: from the player(y18) back up to Jirachi (y7).
StarSummit_Movement_CameraToTop:
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

@ Goon (15,12) steps right one tile per shove (->16, ->17).
StarSummit_Movement_GoonStepRight:
	walk_right
	step_end

@ Clarkson driven right: push 1 16->17 (turns to face the goon); push 2 drives him
@ TWO tiles 17->19 so he's clearly out over the lava before he vanishes.
StarSummit_Movement_ProfPush:
	walk_fast_right
	face_left
	step_end

StarSummit_Movement_ProfIntoLava:
	walk_fast_right
	walk_fast_right
	step_end

@ Goon walks DOWN from (17,12) to (15,17), right in front of the player, facing them.
@ (Tweak in Porymap if the player's stop tile differs.)
StarSummit_Movement_GoonApproachPlayer:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	face_down
	step_end

@ Top goons (7,6)/(8,6) cross the bridge 7 tiles right (SLOW) to flank JIRACHI at (14,6)/(15,6).
StarSummit_Movement_GoonCrossBridge:
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	step_end

@ ...then back the way they came (14,6)/(15,6) -> (7,6)/(8,6), SLOW.
StarSummit_Movement_GoonCrossBridgeBack:
	walk_slow_left
	walk_slow_left
	walk_slow_left
	walk_slow_left
	walk_slow_left
	walk_slow_left
	walk_slow_left
	step_end

@==============================================================================
@ TEXT
@==============================================================================
StarSummit_Text_JirachiCry:
	.string "JIRACHI's cry rings out across\n"
	.string "the summit!$"

StarSummit_Text_ClarksonLine1:
	.string "{PLAYER}…\p"
	.string "I should never have told you to\n"
	.string "come here.$"

StarSummit_Text_ClarksonLine2:
	.string "{PLAYER}…\p"
	.string "You can't let JIRACHI's power\n"
	.string "fall into enemy hands!$"

StarSummit_Text_GoonGoodbye:
	.string "…Goodbye, FATHER.$"

StarSummit_Text_GoonChallenge:
	.string "He begged you to stay away.\n"
	.string "You should have listened.\p"
	.string "Now you'll watch me take it all.$"

StarSummit_Text_GoonDefeat:
	.string "…Tch. You're strong.\n"
	.string "But it changes nothing.$"

StarSummit_Text_GoonAfterWin:
	.string "Strong. But the WISH was\n"
	.string "never yours to protect.$"

StarSummit_Text_GoonAfterLoss:
	.string "Pathetic. Stay down.\n"
	.string "The WISH is ours now.$"

@ TODO: replace with the line Joe wants here (placeholder = a silent beat).
StarSummit_Text_PreTitle:
	.string "…$"

StarSummit_Text_ActII:
	.string "Act II: Doomsday$"
