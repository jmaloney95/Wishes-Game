DistortionWorld_5_MapScripts::
	.byte 0

@ Rift Heart ball (placeholder spot 26,19 -- move it when the layer is
@ painted). VAR_DISTORTION_RIFT_DEPTH -> 4: all layers cleared (the cave
@ keeps leading back here for revisits), then Gengar collapses the visit.
DistortionWorld_5_EventScript_RiftHeart::
	finditem ITEM_RIFT_HEART
	checkitem ITEM_RIFT_HEART
	goto_if_eq VAR_RESULT, FALSE, DistortionWorld_5_EventScript_HeartLeftBehind
	setvar VAR_DISTORTION_RIFT_DEPTH, 4
	goto DistortionRift_EventScript_GengarCollapse

DistortionWorld_5_EventScript_HeartLeftBehind::
	end

@ ----------------------------------------------------------------------------
@  The rift's keeper -- the Distortion gym. Beating him awards the
@  DISTORTION BADGE (FLAG_BADGE05_GET, which also unlocks SURF in the field).
@  His ace is GIRATINA, the thing the rift answers to.
@ ----------------------------------------------------------------------------
DistortionWorld_5_EventScript_Leader::
	lock
	faceplayer
	goto_if_set FLAG_BADGE05_GET, DistortionWorld_5_EventScript_LeaderAfter
	msgbox DistortionWorld_5_Text_LeaderIntro, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro TRAINER_DISTORTION_LEADER, DistortionWorld_5_Text_LeaderDefeat
	msgbox DistortionWorld_5_Text_LeaderBadgeWords, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_BADGE
	message DistortionWorld_5_Text_ReceivedDistortionBadge
	waitmessage
	waitfanfare
	setflag FLAG_BADGE05_GET
	msgbox DistortionWorld_5_Text_LeaderFarewell, MSGBOX_DEFAULT
	release
	end

DistortionWorld_5_EventScript_LeaderAfter::
	msgbox DistortionWorld_5_Text_LeaderAfter, MSGBOX_DEFAULT
	release
	end

DistortionWorld_5_Text_LeaderIntro::
	.string "You crossed every stone to reach\n"
	.string "a room the world forgot.\p"
	.string "I am VESPER. I keep the rift's\n"
	.string "heart, and the rift keeps me.\p"
	.string "The one below us has watched you\n"
	.string "fall through every layer.\p"
	.string "Show us both what you carry.$"

DistortionWorld_5_Text_LeaderDefeat::
	.string "...Even IT leans toward you.\n"
	.string "Remarkable.$"

DistortionWorld_5_Text_LeaderBadgeWords::
	.string "The rift does not yield. It\n"
	.string "ACCEPTS. And it has accepted you.$"

DistortionWorld_5_Text_ReceivedDistortionBadge::
	.string "{PLAYER} received the DISTORTION\n"
	.string "BADGE from VESPER!$"

DistortionWorld_5_Text_LeaderFarewell::
	.string "That mark carries weight on both\n"
	.string "sides of the veil.\p"
	.string "POKEMON that know SURF will now\n"
	.string "carry you across open water.\p"
	.string "Take the heart, if you can bear\n"
	.string "what it remembers.$"

DistortionWorld_5_Text_LeaderAfter::
	.string "The rift is quieter with you in\n"
	.string "it. Giratina notices such things.$"
