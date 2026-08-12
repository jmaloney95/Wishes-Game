@ ============================================================================
@  THE PLAYER'S HOME -- ACT 2 (occupied Munen)
@  Entry cutscene (ON_FRAME, once): two Mutrid goons are trying to arrest
@  Mom. She fights BOTH off (ladle-fu), they flee -- then she sees the
@  player, alive, and after healing the party she leads them to the
@  safehouse (MAP_DOJO), clearing FLAG_HIDE_DOJO_REBELS on the way.
@  Local IDs: 1 = Mom, 2 = Goon A (M), 3 = Goon B (F).
@  All three objects are hidden by FLAG_ACT2_HOME_CUTSCENE_DONE afterward;
@  revisiting shows the empty, wrecked kitchen.
@ ============================================================================
MunenHomeAct2_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, MunenHomeAct2_OnFrame
	.byte 0

MunenHomeAct2_OnFrame:
	map_script_2 VAR_TEMP_0, 0, MunenHomeAct2_EventScript_Cutscene
	.2byte 0

MunenHomeAct2_EventScript_Cutscene::
	setvar VAR_TEMP_0, 1
	goto_if_set FLAG_ACT2_HOME_CUTSCENE_DONE, MunenHomeAct2_EventScript_CutsceneEnd
	lockall
	playbgm MUS_DP_VS_GALACTIC, FALSE
	@ Pan the camera up-left so Mom and the goons are in frame
	@ (player enters at the door, 8,8; the scuffle is around 5,3).
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, MunenHomeAct2_Movement_CameraPanUp
	waitmovement 0
	setspeaker SP_NAME_GOON
	msgbox MunenHomeAct2_Text_GoonThreat, MSGBOX_DEFAULT
	setspeaker SP_NAME_MOM
	msgbox MunenHomeAct2_Text_MomRefuse, MSGBOX_DEFAULT
	setspeaker SP_NAME_GOON
	msgbox MunenHomeAct2_Text_GoonGrabHer, MSGBOX_DEFAULT
	closemessage
	applymovement 2, MunenHomeAct2_Movement_GoonLunge
	applymovement 3, MunenHomeAct2_Movement_GoonLunge
	waitmovement 0
	playse SE_M_DOUBLE_SLAP
	applymovement 1, MunenHomeAct2_Movement_MomSwing
	applymovement 2, MunenHomeAct2_Movement_GoonReel
	waitmovement 0
	playse SE_M_DOUBLE_SLAP
	applymovement 1, MunenHomeAct2_Movement_MomSwing
	applymovement 3, MunenHomeAct2_Movement_GoonReel
	waitmovement 0
	msgbox MunenHomeAct2_Text_Wham, MSGBOX_DEFAULT
	setspeaker SP_NAME_GOON
	msgbox MunenHomeAct2_Text_GoonFlee, MSGBOX_DEFAULT
	closemessage
	applymovement 2, MunenHomeAct2_Movement_GoonAFlee
	applymovement 3, MunenHomeAct2_Movement_GoonBFlee
	waitmovement 0
	playse SE_EXIT
	removeobject 2
	removeobject 3
	fadedefaultbgm
	delay 30
	@ Pan back to the player before removing the camera.
	applymovement LOCALID_CAMERA, MunenHomeAct2_Movement_CameraPanDown
	waitmovement 0
	special RemoveCameraObject
	applymovement 1, MunenHomeAct2_Movement_MomNotice
	waitmovement 0
	setspeaker SP_NAME_MOM
	msgbox MunenHomeAct2_Text_MomGasp, MSGBOX_DEFAULT
	closemessage
	applymovement 1, MunenHomeAct2_Movement_MomApproach
	applymovement LOCALID_PLAYER, MunenHomeAct2_Movement_PlayerFaceUp
	waitmovement 0
	setspeaker SP_NAME_MOM
	msgbox MunenHomeAct2_Text_MomReunion, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	special HealPlayerParty
	fadescreen FADE_FROM_BLACK
	playfanfare MUS_HEAL
	waitfanfare
	setspeaker SP_NAME_MOM
	msgbox MunenHomeAct2_Text_MomSafehouse, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_ACT2_HOME_CUTSCENE_DONE
	clearflag FLAG_HIDE_DOJO_REBELS
	fadescreen FADE_TO_BLACK
	delay 20
	warp MAP_DOJO, 21, 33
	waitstate
	releaseall
	end

MunenHomeAct2_EventScript_CutsceneEnd::
	end

@ Talking to Mom directly mid-scene isn't possible (cutscene runs on entry);
@ this is a safety fallback.
MunenHomeAct2_EventScript_Mom::
	lock
	faceplayer
	msgbox MunenHomeAct2_Text_MomSafehouse, MSGBOX_DEFAULT
	release
	end

MunenHomeAct2_EventScript_OverturnedTable::
	msgbox MunenHomeAct2_Text_OverturnedTable, MSGBOX_SIGN
	end

@ --------------------------------------------------------------------------
@  Movement
@ --------------------------------------------------------------------------
MunenHomeAct2_Movement_CameraPanUp:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_left
	walk_left
	step_end

MunenHomeAct2_Movement_CameraPanDown:
	walk_right
	walk_right
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MunenHomeAct2_Movement_GoonLunge:
	walk_in_place_fast_up
	delay_8
	step_end

MunenHomeAct2_Movement_MomSwing:
	walk_in_place_fast_down
	delay_4
	step_end

MunenHomeAct2_Movement_GoonReel:
	walk_fast_down
	emote_exclamation_mark
	step_end

MunenHomeAct2_Movement_GoonAFlee:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_down
	step_end

MunenHomeAct2_Movement_GoonBFlee:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_down
	step_end

MunenHomeAct2_Movement_MomNotice:
	emote_exclamation_mark
	delay_16
	step_end

MunenHomeAct2_Movement_MomApproach:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_right
	walk_right
	walk_right
	face_down
	step_end

MunenHomeAct2_Movement_PlayerFaceUp:
	face_up
	step_end

@ ============================================================================
@  Text
@ ============================================================================
MunenHomeAct2_Text_GoonThreat::
	.string "GOON: Last chance, woman. Every\n"
	.string "roof in Munen signs the ledger,\l"
	.string "or every roof in Munen BURNS.\p"
	.string "You're coming with us.$"

MunenHomeAct2_Text_MomRefuse::
	.string "MOM: This is my kitchen.\p"
	.string "You are wearing MASKS in my\n"
	.string "kitchen, and you are DRIPPING\l"
	.string "on my clean floor.\p"
	.string "Out.$"

MunenHomeAct2_Text_GoonGrabHer::
	.string "GOON: ...Grab her.$"

MunenHomeAct2_Text_Wham::
	.string "WHAM! THWACK!\p"
	.string "Mom's ladle moves faster than\n"
	.string "either goon can follow.$"

MunenHomeAct2_Text_GoonFlee::
	.string "GOON: She's RABID! The Wish isn't\n"
	.string "paying me enough for THIS!$"

MunenHomeAct2_Text_MomGasp::
	.string "MOM: And STAY out!\p"
	.string "Honestly, the NERVE of--\p"
	.string "...\p"
	.string "MOM: ...{PLAYER}?$"

MunenHomeAct2_Text_MomReunion::
	.string "MOM: {PLAYER}!! Oh-- oh, look at\n"
	.string "you. LOOK at you.\p"
	.string "They told us no one came down\n"
	.string "from that mountain. They stood in\l"
	.string "my doorway and told me my child\l"
	.string "was GONE.\p"
	.string "...You're thinner. And you've\n"
	.string "been fighting.\p"
	.string "Not another word -- not HERE.\n"
	.string "The walls have masks these days.\p"
	.string "Your Pokemon first. Come here,\n"
	.string "all of you.$"

MunenHomeAct2_Text_MomSafehouse::
	.string "MOM: There are still people in\n"
	.string "Munen who haven't knelt, love.\p"
	.string "We meet where Mutrid doesn't\n"
	.string "look: the old dojo.\p"
	.string "Stay close to me, keep your hood\n"
	.string "up, and walk like you're bored.\p"
	.string "Quickly, now.$"

MunenHomeAct2_Text_OverturnedTable::
	.string "The kitchen table is overturned.\p"
	.string "Dinner for one -- long cold --\n"
	.string "is spread across the floorboards.\p"
	.string "She set a place anyway. She\n"
	.string "always sets a place.$"
