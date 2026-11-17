@ ===========================================================================
@ Trailer capture (debug menu only, see src/wot_trailer.c)
@ ===========================================================================
@ The National Park guide's tour, as if it happened in the Mega Dimension:
@ the map in black and white, the guide, the player, every Pokemon and the
@ message box in colour. The camera moves and the guide's lines are the real
@ cutscene's (data/maps/Route_2_2/scripts.inc), so the shots match it; that
@ script is not touched. The only saved bit written is the guide's own
@ FLAG_SAFE_FOLLOWER_MOVEMENT guard, set and cleared as her script does.
@
@ Shot list, as recorded for the trailer (walk_slow = 32 frames a tile):
@   the player and the guide on the dirt, ~2.5 s
@   cut: meadow, pan east 16 tiles (Gible in the grass)
@   cut: shore, pan south 18 tiles (Sandile, Marill, the tuber, swimmers)
@   cut: the memorial, still (Blissey, Porygon, Haunter, Duskull)

@ VAR_0x8004 = WOT_TRAILER_PARK_* (include/wot_trailer.h).
@ R + START > Scripts > "Trailer: grey park": no message boxes, hands-free.
Debug_EventScript_TrailerPark::
	setvar VAR_0x8004, 0 @ WOT_TRAILER_PARK_CLIP
	goto Debug_EventScript_TrailerParkGo

@ R + START > Scripts > "Trailer: grey park+text": the guide's lines, A to
@ advance, as in the game.
Debug_EventScript_TrailerParkText::
	setvar VAR_0x8004, 1 @ WOT_TRAILER_PARK_TEXT
	goto Debug_EventScript_TrailerParkGo

@ R + START > Scripts > "Trailer: park portal": hands-free like "grey park",
@ but the park is empty (no guide, no swimmers, no Pokemon, no follower) and a
@ portal swirls on the shore where Marill wanders, mid-screen in the south pan.
Debug_EventScript_TrailerParkPortal::
	setvar VAR_0x8004, 2 @ WOT_TRAILER_PARK_PORTAL
Debug_EventScript_TrailerParkGo::
	special WotTrailer_StartParkTour
	goto_if_eq VAR_RESULT, FALSE, Debug_EventScript_TrailerParkOff
	waitstate
	end

Debug_EventScript_TrailerParkOff::
	releaseall
	end

@ Started by wot_trailer.c once the park has faded in, as if the player had
@ just talked to the guide (VAR_LAST_TALKED = her local id).
WotTrailer_EventScript_ParkTour::
	lockall
	setflag FLAG_SAFE_FOLLOWER_MOVEMENT
	turnobject LOCALID_PLAYER, DIR_EAST
	turnobject VAR_LAST_TALKED, DIR_WEST
	goto WotTrailer_EventScript_ParkTourShots

@ The empty park: the same shots and timing, the player alone on the dirt.
WotTrailer_EventScript_ParkTourEmpty::
	lockall
	setflag FLAG_SAFE_FOLLOWER_MOVEMENT
	turnobject LOCALID_PLAYER, DIR_EAST
WotTrailer_EventScript_ParkTourShots:
	delay 150
	special SpawnCameraObject
	call WotTrailer_EventScript_CutToShot1
	applymovement LOCALID_CAMERA, Route_2_2_Movement_PanEast
	waitmovement 0
	call WotTrailer_EventScript_CutToShot2
	applymovement LOCALID_CAMERA, Route_2_2_Movement_PanSouth
	waitmovement 0
	call WotTrailer_EventScript_CutToShot3
	delay 240
	goto WotTrailer_EventScript_ParkTourEnd

WotTrailer_EventScript_ParkTourText::
	lockall
	setflag FLAG_SAFE_FOLLOWER_MOVEMENT
	turnobject LOCALID_PLAYER, DIR_EAST
	turnobject VAR_LAST_TALKED, DIR_WEST
	msgbox Route_2_2_Text_GuideWelcome, MSGBOX_DEFAULT
	closemessage
	special SpawnCameraObject
	call WotTrailer_EventScript_CutToShot1
	applymovement LOCALID_CAMERA, Route_2_2_Movement_PanEast
	msgbox Route_2_2_Text_GuideShot1, MSGBOX_DEFAULT
	waitmovement 0
	closemessage
	call WotTrailer_EventScript_CutToShot2
	applymovement LOCALID_CAMERA, Route_2_2_Movement_PanSouth
	msgbox Route_2_2_Text_GuideShot2, MSGBOX_DEFAULT
	waitmovement 0
	closemessage
	call WotTrailer_EventScript_CutToShot3
	msgbox Route_2_2_Text_GuideShot3, MSGBOX_DEFAULT
	closemessage
	goto WotTrailer_EventScript_ParkTourEnd

@ Cuts are hardware fades (swapbuffers), so the palettes are never rebuilt.
WotTrailer_EventScript_CutToShot1::
	fadescreenswapbuffers FADE_TO_BLACK
	applymovement LOCALID_CAMERA, Route_2_2_Movement_CamToShot1
	waitmovement 0
	fadescreenswapbuffers FADE_FROM_BLACK
	return

WotTrailer_EventScript_CutToShot2::
	fadescreenswapbuffers FADE_TO_BLACK
	applymovement LOCALID_CAMERA, Route_2_2_Movement_CamToShot2
	waitmovement 0
	fadescreenswapbuffers FADE_FROM_BLACK
	return

WotTrailer_EventScript_CutToShot3::
	fadescreenswapbuffers FADE_TO_BLACK
	applymovement LOCALID_CAMERA, Route_2_2_Movement_CamToShot3
	waitmovement 0
	fadescreenswapbuffers FADE_FROM_BLACK
	return

@ Camera back on the player, then the park reloads in colour where it started.
@ The script ends at the warp.
WotTrailer_EventScript_ParkTourEnd::
	fadescreenswapbuffers FADE_TO_BLACK
	applymovement LOCALID_CAMERA, Route_2_2_Movement_CamReturn
	waitmovement 0
	special RemoveCameraObject
	clearflag FLAG_SAFE_FOLLOWER_MOVEMENT
	special WotTrailer_EndParkTour
	waitstate
	end
