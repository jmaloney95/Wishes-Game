DistortionWorld_4_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DistortionWorld_4_OnTransition
	.byte 0

@ See DistortionWorld_3_OnTransition: PENDING carries inversion through warps.
DistortionWorld_4_OnTransition::
	setrespawn HEAL_LOCATION_NATIONAL_PARK_ACT2
	goto_if_set FLAG_DW_INVERT_PENDING, DistortionWorld_4_OnTransition_StayInverted
	clearflag FLAG_DW_INVERTED
	end
DistortionWorld_4_OnTransition_StayInverted::
	clearflag FLAG_DW_INVERT_PENDING
	setflag FLAG_DW_INVERTED
	end

@ East-edge portals link the two upside-down walkways (both inverted, so the
@ state is carried through in BOTH directions). The walkways are rows 10-11
@ and 34-35; each portal row lands on the matching row of the other walkway,
@ one tile in from the x42 trigger column. A script warp always lands facing
@ south, so landing ON x42 let the player step straight down into the other
@ portal and bounce back (and the old y33 / y9 landings were solid tiles).
DistortionWorld_4_EventScript_PortalEastDown::
	lockall
	playse SE_WARP_IN
	setflag FLAG_DW_INVERT_PENDING
	warp MAP_DISTORTION_WORLD_4, 41, 34
	waitstate
	releaseall
	end

DistortionWorld_4_EventScript_PortalEastDown2::
	lockall
	playse SE_WARP_IN
	setflag FLAG_DW_INVERT_PENDING
	warp MAP_DISTORTION_WORLD_4, 41, 35
	waitstate
	releaseall
	end

DistortionWorld_4_EventScript_PortalEastUp::
	lockall
	playse SE_WARP_IN
	setflag FLAG_DW_INVERT_PENDING
	warp MAP_DISTORTION_WORLD_4, 41, 11
	waitstate
	releaseall
	end

DistortionWorld_4_EventScript_PortalEastUp2::
	lockall
	playse SE_WARP_IN
	setflag FLAG_DW_INVERT_PENDING
	warp MAP_DISTORTION_WORLD_4, 41, 10
	waitstate
	releaseall
	end

@ The rift trainer guarding the Rift Core: a Hex Maniac the rift kept, like the
@ DW_3 Warden (NOT the Oni). map.json: sight 2, FACE_DOWN. Its defeat
@ flag is automatic (TRAINER_FLAGS_START + TRAINER_RIFT_CORE_GUARD). When it
@ spots the player it walks down onto (27,14) -- the only tile the Core can be
@ taken from -- and the engine's trainer approach also moves its saved template
@ there (OverrideTemplateCoordsForObjectEvent), so it would respawn on the path
@ after scrolling off-screen or loading a save. So after the battle, and on any
@ later talk (saves made while it stood there), it walks back to its spawn
@ spot and the template is put back too. If the player is standing on that
@ spot it stays put (talk to it from another side). A warp into the map
@ reloads the template from map.json, so that path was always fine.
@ DW4_GUARD_HOME_X/Y must match the trainer's x/y in map.json.
	.set DW4_GUARD_HOME_X, 27
	.set DW4_GUARD_HOME_Y, 13
DistortionWorld_4_EventScript_Guard::
	trainerbattle_single TRAINER_RIFT_CORE_GUARD, DistortionWorld_4_Text_GuardIntro, DistortionWorld_4_Text_GuardDefeat, DistortionWorld_4_EventScript_GuardGoHome
	msgbox DistortionWorld_4_Text_GuardPost, MSGBOX_DEFAULT
	closemessage
DistortionWorld_4_EventScript_GuardGoHome::
	getplayerxy VAR_TEMP_1, VAR_TEMP_2
	goto_if_ne VAR_TEMP_1, DW4_GUARD_HOME_X, DistortionWorld_4_EventScript_GuardGoHomeStep
	goto_if_ne VAR_TEMP_2, DW4_GUARD_HOME_Y, DistortionWorld_4_EventScript_GuardGoHomeStep
	releaseall
	end
DistortionWorld_4_EventScript_GuardGoHomeStep::
	getobjectxy LOCALID_DISTORTION_WORLD_4_GUARD, CURRENT_POSITION, VAR_TEMP_1, VAR_TEMP_2
	goto_if_gt VAR_TEMP_2, DW4_GUARD_HOME_Y, DistortionWorld_4_EventScript_GuardStepUp
	goto_if_lt VAR_TEMP_2, DW4_GUARD_HOME_Y, DistortionWorld_4_EventScript_GuardStepDown
	goto_if_gt VAR_TEMP_1, DW4_GUARD_HOME_X, DistortionWorld_4_EventScript_GuardStepLeft
	goto_if_lt VAR_TEMP_1, DW4_GUARD_HOME_X, DistortionWorld_4_EventScript_GuardStepRight
	setobjectxyperm LOCALID_DISTORTION_WORLD_4_GUARD, DW4_GUARD_HOME_X, DW4_GUARD_HOME_Y
	turnobject LOCALID_DISTORTION_WORLD_4_GUARD, DIR_SOUTH
	releaseall
	end

DistortionWorld_4_EventScript_GuardStepUp::
	applymovement LOCALID_DISTORTION_WORLD_4_GUARD, Common_Movement_WalkUp
	waitmovement 0
	goto DistortionWorld_4_EventScript_GuardGoHomeStep

DistortionWorld_4_EventScript_GuardStepDown::
	applymovement LOCALID_DISTORTION_WORLD_4_GUARD, Common_Movement_WalkDown
	waitmovement 0
	goto DistortionWorld_4_EventScript_GuardGoHomeStep

DistortionWorld_4_EventScript_GuardStepLeft::
	applymovement LOCALID_DISTORTION_WORLD_4_GUARD, Common_Movement_WalkLeft
	waitmovement 0
	goto DistortionWorld_4_EventScript_GuardGoHomeStep

DistortionWorld_4_EventScript_GuardStepRight::
	applymovement LOCALID_DISTORTION_WORLD_4_GUARD, Common_Movement_WalkRight
	waitmovement 0
	goto DistortionWorld_4_EventScript_GuardGoHomeStep

DistortionWorld_4_Text_GuardIntro::
	.string "Another one, walking in from\n"
	.string "the living side.\p"
	.string "The deep kept me. It keeps\n"
	.string "everything, in the end.\p"
	.string "Down here, what should hurt you\n"
	.string "heals you, and what you trust\l"
	.string "turns on you.\p"
	.string "You want what the deep keeps?\n"
	.string "Past me, little breather.$"

DistortionWorld_4_Text_GuardDefeat::
	.string "The deep loosens its grip…\n"
	.string "for now.$"

DistortionWorld_4_Text_GuardPost::
	.string "Take it, then.\p"
	.string "The deep keeps what it wants\n"
	.string "anyway.$"

@ Rift Core ball (placeholder spot 23,18 -- move it when the layer is
@ painted). VAR_DISTORTION_RIFT_DEPTH -> 3: the DistortionWorld_2 cave
@ re-aims at DistortionWorld_5, then Gengar collapses the visit.
DistortionWorld_4_EventScript_RiftCore::
	finditem ITEM_RIFT_CORE
	checkitem ITEM_RIFT_CORE
	goto_if_eq VAR_RESULT, FALSE, DistortionWorld_4_EventScript_CoreLeftBehind
	setvar VAR_DISTORTION_RIFT_DEPTH, 3
	goto DistortionRift_EventScript_GengarCollapse

DistortionWorld_4_EventScript_CoreLeftBehind::
	end
