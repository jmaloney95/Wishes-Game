DistortionWorld_MapScripts::
	.byte 0

@ ============================================================================
@  DISTORTION WORLD -- the far side of the gravesite seam.
@  Where the mountain KEEPS what people cannot let go.
@  CANON: Prof. Clarkson DIED at Star Summit. His spirit lives on inside a
@  wild GENGAR ("ghosts collect ghosts"). The Catalpa Bow is the one string
@  that sounds on both sides of the veil -- it lets the living hear him.
@  The NPC here is the GENGAR (local ID 1), speaking as Clarkson.
@  Scene: full confession -> Gengar JOINS THE PARTY -> the Distortion World
@  collapses (screen shake) -> player is thrown back in front of the
@  gravesite (47,19). Afterward the seam is closed for good
@  (FLAG_ACT2_MET_CLARKSON_DISTORTION; also sets FLAG_NATIONAL_PARK_GRAVE_SECRET
@  so the Act 1 gravesite reveals his note).
@  If the party AND boxes are full, the scene politely refuses and can be
@  retried (nothing is set).
@ ============================================================================
DistortionWorld_EventScript_Clarkson::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_GENGAR, CRY_MODE_NORMAL
	waitmoncry
	setspeaker SP_NAME_GENGAR
	showportrait PORTRAIT_CLARKSON_GENGAR
	msgbox DistortionWorld_Text_GengarClarkson1, MSGBOX_DEFAULT
	msgbox DistortionWorld_Text_GengarClarkson2, MSGBOX_DEFAULT
	msgbox DistortionWorld_Text_GengarClarkson3, MSGBOX_DEFAULT
	closemessage
	hideportrait
	givemon SPECIES_GENGAR, 40, shinyMode=SHINY_MODE_ALWAYS
	compare VAR_RESULT, MON_CANT_GIVE
	goto_if_eq DistortionWorld_EventScript_NoRoom
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	compare VAR_RESULT, MON_GIVEN_TO_PC
	goto_if_eq DistortionWorld_EventScript_JoinedPC
	msgbox DistortionWorld_Text_GengarJoined, MSGBOX_DEFAULT
	goto DistortionWorld_EventScript_Collapse

DistortionWorld_EventScript_JoinedPC::
	msgbox DistortionWorld_Text_GengarJoinedPC, MSGBOX_DEFAULT
	goto DistortionWorld_EventScript_Collapse

DistortionWorld_EventScript_Collapse::
	closemessage
	@ Gengar offers to send the player home before he fades and the world
	@ collapses (the warp back to National Park below IS that teleport).
	setspeaker SP_NAME_GENGAR
	showportrait PORTRAIT_CLARKSON_GENGAR
	msgbox DistortionWorld_Text_GengarTeleportHome, MSGBOX_DEFAULT
	closemessage
	hideportrait
	setflag FLAG_ACT2_MET_CLARKSON_DISTORTION
	setflag FLAG_NATIONAL_PARK_GRAVE_SECRET
	removeobject 1
	playse SE_THUNDER
	setvar VAR_0x8004, 2  @ vertical pan
	setvar VAR_0x8005, 2  @ horizontal pan
	setvar VAR_0x8006, 10 @ num shakes
	setvar VAR_0x8007, 4  @ shake delay
	special ShakeCamera
	waitstate
	msgbox DistortionWorld_Text_Collapse1, MSGBOX_DEFAULT
	closemessage
	playse SE_THUNDER2
	setvar VAR_0x8004, 3  @ vertical pan
	setvar VAR_0x8005, 3  @ horizontal pan
	setvar VAR_0x8006, 16 @ num shakes
	setvar VAR_0x8007, 3  @ shake delay
	special ShakeCamera
	waitstate
	msgbox DistortionWorld_Text_Collapse2, MSGBOX_DEFAULT
	closemessage
	playse SE_WARP_IN
	fadescreen FADE_TO_WHITE
	delay 40
	warp MAP_NATIONAL_PARK_ACT2, 47, 19
	waitstate
	release
	end

DistortionWorld_EventScript_NoRoom::
	msgbox DistortionWorld_Text_NoRoom, MSGBOX_DEFAULT
	release
	end

DistortionWorld_EventScript_Rift::
	lockall
	msgbox DistortionWorld_Text_Rift, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq DistortionWorld_EventScript_RiftStay
	closemessage
	playse SE_WARP_IN
	fadescreen FADE_TO_BLACK
	delay 30
	warp MAP_NATIONAL_PARK_ACT2, 47, 19
	waitstate
	releaseall
	end

DistortionWorld_EventScript_RiftStay::
	closemessage
	releaseall
	end

DistortionWorld_EventScript_FloatingStones::
	msgbox DistortionWorld_Text_FloatingStones, MSGBOX_SIGN
	end

DistortionWorld_EventScript_KeptThings::
	msgbox DistortionWorld_Text_KeptThings, MSGBOX_SIGN
	end

@ ----------------------------------------------------------------------------
@  Floating-rock crossing at x25: lower island (y13) <-> rock (y10) <-> cave
@  island (y7). The gaps are 2 tiles, beyond a vanilla ledge hop, so facing
@  the gap and pressing A skips the player across in two quick arcs
@  (bg_events at (25,8/9/11/12) gate on facing direction).
@ ----------------------------------------------------------------------------
DistortionWorld_EventScript_HopNorth::
	lockall
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, DistortionWorld_Movement_HopNorth
	waitmovement 0
	releaseall
	end

DistortionWorld_EventScript_HopSouth::
	lockall
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, DistortionWorld_Movement_HopSouth
	waitmovement 0
	releaseall
	end

DistortionWorld_Movement_HopNorth:
	jump_2_up
	jump_up
	step_end

DistortionWorld_Movement_HopSouth:
	jump_2_down
	jump_down
	step_end

@ ============================================================================
@  Text
@ ============================================================================
DistortionWorld_Text_GengarClarkson1::
	.string "Kekeke... don't reach for a Poke\n"
	.string "Ball just yet, {PLAYER}.\p"
	.string "It's me. Or what remains of me.\n"
	.string "Let me say it plainly,\n"
	.string "Professor Clarkson died on Star\n"
	.string "Summit.\p"
	.string "What fell into the lava was me.\n"
	.string "All of me.$"

DistortionWorld_Text_GengarClarkson2::
	.string "GENGAR found my spirit\n"
	.string "drifting loose in the seams.\p"
	.string "The grin took some getting used to.\n"
	.string "And you hear my words only\n"
	.string "because of that bow you carry.\p"
	.string "Catalpa wood. The\n"
	.string "one string that sounds on BOTH\l"
	.string "sides of the veil.$"

DistortionWorld_Text_GengarClarkson3::
	.string "Listen. I can't hold\n"
	.string "the rift open forever.\p"
	.string "My research still sits in my\n"
	.string "lab. My notes on JIRACHI\l"
	.string "and SHADOW pokemon.\p"
	.string "{PLAYER}... you can't let\n"
	.string "Team Mutrid find it.\p"
	.string "You must DESTROY my research.\n"
	.string "Burn every page.\p"
	.string "We must see this through together.\p"
	.string "Take us with you, {PLAYER}.$"

DistortionWorld_Text_GengarJoined::
	.string "GENGAR -- and everything riding\n"
	.string "within it -- joined your party!$"

DistortionWorld_Text_GengarJoinedPC::
	.string "GENGAR -- and everything riding\n"
	.string "within it -- was sent to the PC.\p"
	.string "Box logistics. Even death has\n"
	.string "paperwork, kekeke.$"

DistortionWorld_Text_NoRoom::
	.string "...Your party and your boxes are\n"
	.string "FULL, {PLAYER}.\p"
	.string "“Even a ghost needs a seat.\p"
	.string "Make room, then come back for\n"
	.string "me. I have waited this long.$"

DistortionWorld_Text_GengarTeleportHome::
	.string "This place won't hold together\n"
	.string "much longer -- and neither will I.\p"
	.string "But I have just enough left in me\n"
	.string "for one last trick.\p"
	.string "Let me send you home, {PLAYER}.\n"
	.string "Back to the living world, where you\l"
	.string "belong.$"

DistortionWorld_Text_Collapse1::
	.string "Ah. There it is.\p"
	.string "It seems I was the nail holding\n"
	.string "this room shut, {PLAYER}.\p"
	.string "Hold on to me!$"

DistortionWorld_Text_Collapse2::
	.string "The floating stones crack apart\p"
	.string "The Distortion World folds\n"
	.string "inward--$"

DistortionWorld_Text_Rift::
	.string "The seam hangs in the air here,\n"
	.string "a thin bright wound in the fog.\p"
	.string "Through it: the gravesite, the\n"
	.string "park, the cold ordinary world.\p"
	.string "Step back through?$"

DistortionWorld_Text_FloatingStones::
	.string "Grave-stones drift here, unmoored,\n"
	.string "turning slowly end over end.\p"
	.string "The names on them are all\n"
	.string "perfectly legible.\p"
	.string "That is somehow worse.$"

DistortionWorld_Text_KeptThings::
	.string "Small things are gathered in the\n"
	.string "hollow of a floating stone:\p"
	.string "a child's shoe. A dry brush. A\n"
	.string "letter, folded so many times it\l"
	.string "can never be opened again.\p"
	.string "Kept things. Every one of them\n"
	.string "somebody's regret.$"
