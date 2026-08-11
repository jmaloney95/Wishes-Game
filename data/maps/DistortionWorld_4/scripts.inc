DistortionWorld_4_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DistortionWorld_4_OnTransition
	.byte 0

@ See DistortionWorld_3_OnTransition: PENDING carries inversion through warps.
DistortionWorld_4_OnTransition::
	goto_if_set FLAG_DW_INVERT_PENDING, DistortionWorld_4_OnTransition_StayInverted
	clearflag FLAG_DW_INVERTED
	end
DistortionWorld_4_OnTransition_StayInverted::
	clearflag FLAG_DW_INVERT_PENDING
	setflag FLAG_DW_INVERTED
	end

@ East-edge portals link the two upside-down walkways (both inverted, so the
@ state is carried through in BOTH directions).
DistortionWorld_4_EventScript_PortalEastDown::
	lockall
	playse SE_WARP_IN
	setflag FLAG_DW_INVERT_PENDING
	warp MAP_DISTORTION_WORLD_4, 42, 34
	waitstate
	releaseall
	end

DistortionWorld_4_EventScript_PortalEastDown2::
	lockall
	playse SE_WARP_IN
	setflag FLAG_DW_INVERT_PENDING
	warp MAP_DISTORTION_WORLD_4, 42, 33
	waitstate
	releaseall
	end

DistortionWorld_4_EventScript_PortalEastUp::
	lockall
	playse SE_WARP_IN
	setflag FLAG_DW_INVERT_PENDING
	warp MAP_DISTORTION_WORLD_4, 42, 10
	waitstate
	releaseall
	end

DistortionWorld_4_EventScript_PortalEastUp2::
	lockall
	playse SE_WARP_IN
	setflag FLAG_DW_INVERT_PENDING
	warp MAP_DISTORTION_WORLD_4, 42, 9
	waitstate
	releaseall
	end

@ The Oni guarding the Rift Core (map.json: sight 2, LOOK_AROUND). Its defeat
@ flag is automatic (TRAINER_FLAGS_START + TRAINER_DISTORTION_ONI); the object
@ stays after defeat like any beaten trainer. Dialogue PLACEHOLDER (Joe).
DistortionWorld_4_EventScript_Oni::
	trainerbattle_single TRAINER_DISTORTION_ONI, DistortionWorld_4_Text_OniIntro, DistortionWorld_4_Text_OniDefeat
	msgbox DistortionWorld_4_Text_OniPost, MSGBOX_AUTOCLOSE
	end

DistortionWorld_4_Text_OniIntro::
	.string "A kept thing wears a demon's\n"
	.string "face. It grins.\p"
	.string "You want what the deep keeps?\n"
	.string "Past me, little breather.$"

DistortionWorld_4_Text_OniDefeat::
	.string "The mask cracks…\n"
	.string "the grin stays.$"

DistortionWorld_4_Text_OniPost::
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
