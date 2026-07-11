DistortionWorld_3_MapScripts::
	.byte 0

@ Rift Shard ball (placeholder spot 29,15 -- move it when the layer is
@ painted). Taking it deepens the rift: VAR_DISTORTION_RIFT_DEPTH -> 2, so
@ the DistortionWorld_2 cave re-aims at DistortionWorld_4, and Gengar
@ collapses this visit the same way the first Distortion World ended.
@ Std_FindItem clobbers VAR_RESULT internally, so success is re-checked with
@ checkitem; if the bag was somehow full the ball stays and nothing advances.
DistortionWorld_3_EventScript_RiftShard::
	finditem ITEM_RIFT_SHARD
	checkitem ITEM_RIFT_SHARD
	goto_if_eq VAR_RESULT, FALSE, DistortionWorld_3_EventScript_ShardLeftBehind
	setvar VAR_DISTORTION_RIFT_DEPTH, 2
	goto DistortionRift_EventScript_GengarCollapse

DistortionWorld_3_EventScript_ShardLeftBehind::
	end

@ The rift's first keeper -- a stranger who has been down here too long.
DistortionWorld_3_EventScript_Warden::
	trainerbattle_single TRAINER_RIFT_WARDEN, DistortionWorld_3_Text_WardenIntro, DistortionWorld_3_Text_WardenDefeat
	msgbox DistortionWorld_3_Text_WardenPost, MSGBOX_AUTOCLOSE
	end

DistortionWorld_3_Text_WardenIntro::
	.string "You walk like the living.\p"
	.string "The rift keeps what cannot be let\n"
	.string "go. I stayed to learn what it\l"
	.string "kept me for.\p"
	.string "Show me your grip on the world.$"

DistortionWorld_3_Text_WardenDefeat::
	.string "...A firm grip. Firmer than mine.$"

DistortionWorld_3_Text_WardenPost::
	.string "Deeper down, the stones drift\n"
	.string "further apart. Jump like you mean\l"
	.string "it -- hesitation is a long fall.$"
