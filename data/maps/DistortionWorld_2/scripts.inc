DistortionWorld_2_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, DistortionWorld_2_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, DistortionWorld_2_OnFrame
	.byte 0

@ ----------------------------------------------------------------------------
@  Floating-stone crossings (shared by every rift layer): face the gap and
@  press A to skip across. 1-wide gaps are a single arc; 2-wide gaps are the
@  double bounce-skip. bg_events in each layer's map.json pick the direction.
@ ----------------------------------------------------------------------------
DistortionRift_EventScript_HopEast1::
	lockall
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, DistortionRift_Movement_HopEast1
	waitmovement 0
	releaseall
	end

DistortionRift_EventScript_HopEast2::
	lockall
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, DistortionRift_Movement_HopEast2
	waitmovement 0
	releaseall
	end

DistortionRift_EventScript_HopWest1::
	lockall
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, DistortionRift_Movement_HopWest1
	waitmovement 0
	releaseall
	end

DistortionRift_EventScript_HopWest2::
	lockall
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, DistortionRift_Movement_HopWest2
	waitmovement 0
	releaseall
	end

DistortionRift_Movement_HopEast1:
	jump_2_right
	step_end

DistortionRift_Movement_HopEast2:
	jump_2_right
	jump_right
	step_end

DistortionRift_Movement_HopWest1:
	jump_2_left
	step_end

DistortionRift_Movement_HopWest2:
	jump_2_left
	jump_left
	step_end

DistortionRift_EventScript_HopNorth1::
	lockall
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, DistortionRift_Movement_HopNorth1
	waitmovement 0
	releaseall
	end

DistortionRift_EventScript_HopNorth2::
	lockall
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, DistortionRift_Movement_HopNorth2
	waitmovement 0
	releaseall
	end

DistortionRift_EventScript_HopSouth1::
	lockall
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, DistortionRift_Movement_HopSouth1
	waitmovement 0
	releaseall
	end

DistortionRift_EventScript_HopSouth2::
	lockall
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, DistortionRift_Movement_HopSouth2
	waitmovement 0
	releaseall
	end

DistortionRift_Movement_HopNorth1:
	jump_2_up
	step_end

DistortionRift_Movement_HopNorth2:
	jump_2_up
	jump_up
	step_end

DistortionRift_Movement_HopSouth1:
	jump_2_down
	step_end

DistortionRift_Movement_HopSouth2:
	jump_2_down
	jump_down
	step_end

@ ============================================================================
@  DISTORTION WORLD -- RIFT LAYERS (Act 2, post-collapse returns)
@  Every Catalpa Bow ferry trip after the Gengar-Clarkson scene lands HERE
@  (catalpa_bow.inc), never in the original MAP_DISTORTION_WORLD.
@  VAR_DISTORTION_RIFT_DEPTH drives the whole feature:
@    0 = first return not seen yet -> ON_FRAME island-reveal cutscene below
@    1 = reveal seen               -> cave (25,5) leads to DistortionWorld_3
@    2 = Rift Shard taken          -> cave leads to DistortionWorld_4
@    3 = Rift Core taken           -> cave leads to DistortionWorld_5
@    4 = Rift Heart taken          -> cave keeps leading to DistortionWorld_5
@  The cave warp event at (25,5) is MAP_DYNAMIC; ON_TRANSITION re-aims it on
@  every map entry, so save/reload and elevator scripts elsewhere can never
@  leave it stale. Each relic pickup (see the layer maps' scripts.inc) bumps
@  the depth and jumps to DistortionRift_EventScript_GengarCollapse below.
@ ============================================================================

@ Warp-id form (single arg = the destination map's warp index 0), so the
@ arrival point tracks each layer's entrance warp wherever it is moved in
@ Porymap -- no coordinates to keep in sync.
DistortionWorld_2_OnTransition::
	goto_if_eq VAR_DISTORTION_RIFT_DEPTH, 2, DistortionWorld_2_OnTransition_CaveToLayer4
	goto_if_ge VAR_DISTORTION_RIFT_DEPTH, 3, DistortionWorld_2_OnTransition_CaveToLayer5
	setdynamicwarp MAP_DISTORTION_WORLD_3, 0
	end

DistortionWorld_2_OnTransition_CaveToLayer4::
	setdynamicwarp MAP_DISTORTION_WORLD_4, 0
	end

DistortionWorld_2_OnTransition_CaveToLayer5::
	setdynamicwarp MAP_DISTORTION_WORLD_5, 0
	end

DistortionWorld_2_OnFrame:
	map_script_2 VAR_DISTORTION_RIFT_DEPTH, 0, DistortionWorld_2_EventScript_IslandReveal
	.2byte 0

@ First ferry return: Gengar points out the island that has risen across the
@ void at x25 (the hop crossing), then the camera pans up the column to the
@ cave the stepping-path leads to. The ferry always lands the player on
@ (15,24); the island's heart is (25,11) [+10 east, -13 north], the cave
@ mouth is (25,5) [6 further north], and the return walks the full way back.
@ If another entrance into DistortionWorld_2 is ever added, gate this scene
@ on the arrival point or re-aim the camera.
@ FLAG_SAFE_FOLLOWER_MOVEMENT guard: same as the park tour (Route_2_2) --
@ without it every camera applymovement also hides the follower and
@ waitmovement 0 can hang the script.
DistortionWorld_2_EventScript_IslandReveal::
	lockall
	setflag FLAG_SAFE_FOLLOWER_MOVEMENT
	waitse
	playmoncry SPECIES_GENGAR, CRY_MODE_NORMAL
	waitmoncry
	setspeaker SP_NAME_GENGAR
	msgbox DistortionWorld_2_Text_SeamRemembers, MSGBOX_DEFAULT
	closemessage
	special SpawnCameraObject
	fadescreen FADE_TO_BLACK
	applymovement LOCALID_CAMERA, DistortionWorld_2_Movement_CamToIsland
	waitmovement 0
	fadescreen FADE_FROM_BLACK
	msgbox DistortionWorld_2_Text_IslandRose, MSGBOX_DEFAULT
	closemessage
	@ Slow visible pan up the x25 column: island heart (25,11) -> the cave
	@ mouth (25,5) that the stepping-path leads to.
	applymovement LOCALID_CAMERA, DistortionWorld_2_Movement_CamPanUpToCave
	msgbox DistortionWorld_2_Text_CaveAbove, MSGBOX_DEFAULT
	waitmovement 0
	closemessage
	fadescreen FADE_TO_BLACK
	@ Walk the camera back onto the player BEFORE removing it, or the player
	@ is left steering the camera.
	applymovement LOCALID_CAMERA, DistortionWorld_2_Movement_CamReturn
	waitmovement 0
	special RemoveCameraObject
	fadescreen FADE_FROM_BLACK
	setvar VAR_DISTORTION_RIFT_DEPTH, 1
	clearflag FLAG_SAFE_FOLLOWER_MOVEMENT
	releaseall
	end

@ Shared rift-collapse eject, used by the relic pickups in layers 3/4/5.
@ Set VAR_DISTORTION_RIFT_DEPTH to the NEW depth before jumping here.
@ Mirrors the original DistortionWorld_EventScript_Collapse beat for beat:
@ thunder + two escalating ShakeCamera passes, white-out, and the player is
@ thrown back in front of the gravesite (47,19).
DistortionRift_EventScript_GengarCollapse::
	lockall
	waitse
	playmoncry SPECIES_GENGAR, CRY_MODE_NORMAL
	waitmoncry
	setspeaker SP_NAME_GENGAR
	showportrait PORTRAIT_CLARKSON_GENGAR
	msgbox DistortionRift_Text_CantHoldIt, MSGBOX_DEFAULT
	closemessage
	hideportrait
	playse SE_THUNDER
	setvar VAR_0x8004, 2  @ vertical pan
	setvar VAR_0x8005, 2  @ horizontal pan
	setvar VAR_0x8006, 10 @ num shakes
	setvar VAR_0x8007, 4  @ shake delay
	special ShakeCamera
	waitstate
	msgbox DistortionRift_Text_RiftFolds, MSGBOX_DEFAULT
	closemessage
	playse SE_THUNDER2
	setvar VAR_0x8004, 3  @ vertical pan
	setvar VAR_0x8005, 3  @ horizontal pan
	setvar VAR_0x8006, 16 @ num shakes
	setvar VAR_0x8007, 3  @ shake delay
	special ShakeCamera
	waitstate
	@ Reality warps as the rift gives way (see field_specials.c).
	playse SE_M_SUPERSONIC
	special StartRiftDistortion
	delay 48
	playse SE_WARP_IN
	fadescreen FADE_TO_WHITE
	special EndRiftDistortion
	delay 40
	warp MAP_NATIONAL_PARK_ACT2, 47, 19
	waitstate
	releaseall
	end

@ ============================================================================
@  Text -- all lines PLACEHOLDER (Joe)
@ ============================================================================
DistortionWorld_2_Text_SeamRemembers::
	.string "GENGAR pours out of its Poke Ball\n"
	.string "like smoke.\p"
	.string "“Kekeke. Welcome back, {PLAYER}.\n"
	.string "The seam remembers you.\p"
	.string "But look -- it is not\n"
	.string "quite as we left it…$"

DistortionWorld_2_Text_IslandRose::
	.string "There. A stepping-path across\n"
	.string "the void…$"

DistortionWorld_2_Text_CaveAbove::
	.string "…straight to that cave above.\p"
	.string "The rift is inviting us deeper.\n"
	.string "Kekeke. It would be rude\l"
	.string "to refuse.$"

DistortionRift_Text_CantHoldIt::
	.string "{PLAYER}! The rift is closing in--\n"
	.string "I can't hold it open.\l"
	.string "Hold on to me!$"

DistortionRift_Text_RiftFolds::
	.string "The Distortion World folds\n"
	.string "inward--$"

@ Ferry pad (15,24) -> island heart (25,11): east 10, north 13, moved under a
@ fade with walk_faster (hidden reposition, park-tour style). Then a slow
@ VISIBLE pan 6 north to the cave mouth (25,5). The return (19 south,
@ 10 west, hidden) is the exact inverse of the combined trip so the camera
@ lands back on the player.
DistortionWorld_2_Movement_CamToIsland:
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	walk_faster_up
	step_end

DistortionWorld_2_Movement_CamPanUpToCave:
	walk_slow_up
	walk_slow_up
	walk_slow_up
	walk_slow_up
	walk_slow_up
	walk_slow_up
	step_end

DistortionWorld_2_Movement_CamReturn:
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	step_end
