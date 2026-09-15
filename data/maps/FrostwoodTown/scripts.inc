FrostwoodTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, FrostwoodTown_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, FrostwoodTown_OnFrame
	.byte 0

@ Arm the Clarkson call when the player walks out of the GYM holding the EMBER
@ BADGE for the first time. Map-level (not tile-level) so it fires no matter
@ which tile the gym exit drops the player onto.
FrostwoodTown_OnTransition::
	@ Whiteout respawn: being in town makes Frostwood the last heal location, so
	@ fainting near here no longer throws the player back to an Act 1 spawn.
	setrespawn HEAL_LOCATION_FROSTWOOD_TOWN
	@ Clarkson's post-Ember-Badge call. (Route 2 now sits between Frostwood and Sennen,
	@ so Frostwood's north exit is a plain warp to Route_2 -- the old dynamic Sennen warp
	@ is gone; the train swap is handled at Sennen instead.)
	goto_if_unset FLAG_DEFEATED_RUSTBORO_GYM, FrostwoodTown_OnTransition_Done
	goto_if_set FLAG_GOT_CLARKSON_MTMUNEN_CALL, FrostwoodTown_OnTransition_Done
	setvar VAR_FROSTWOOD_STATE, 1
FrostwoodTown_OnTransition_Done:
	end

FrostwoodTown_OnFrame:
	map_script_2 VAR_FROSTWOOD_STATE, 1, FrostwoodTown_EventScript_ClarksonCall
	.2byte 0

FrostwoodTown_EventScript_ClarksonCall::
	lockall
	delay 20
	playse SE_PIN
	pokenavcall FrostwoodTown_Text_ClarksonCall
	waitmessage
	closemessage
	setspeaker SP_NAME_CLARKSON
	msgbox FrostwoodTown_Text_ClarksonReminder, MSGBOX_DEFAULT
	setflag FLAG_GOT_CLARKSON_MTMUNEN_CALL
	setvar VAR_FROSTWOOD_STATE, 0
	releaseall
	end

FrostwoodTown_EventScript_OldMan::
	lock
	faceplayer
	msgbox FrostwoodText_OldMan, MSGBOX_DEFAULT
	release
	end

FrostwoodTown_EventScript_GymWoman::
	lock
	faceplayer
	msgbox FrostwoodText_GymWoman, MSGBOX_DEFAULT
	release
	end

FrostwoodTown_EventScript_Kid::
	lock
	faceplayer
	msgbox FrostwoodText_Kid, MSGBOX_DEFAULT
	release
	end

FrostwoodTown_EventScript_Worried::
	lock
	faceplayer
	msgbox FrostwoodText_Worried, MSGBOX_DEFAULT
	release
	end

FrostwoodTown_EventScript_MartLady::
	lock
	faceplayer
	msgbox FrostwoodText_MartLady, MSGBOX_DEFAULT
	release
	end

FrostwoodText_OldMan:
	.string "Welcome to FROSTWOOD TOWN, traveler.\p"
	.string "These woods have worn the same white\ncoat for a thousand winters.\p"
	.string "But walk the trees now and you'll hear\nit, dripping. The frost is letting go.\p"
	.string "Snow that old should never melt. Same\nas they say down in MUNEN.$"

FrostwoodText_GymWoman:
	.string "See that big building with the snowed-\nover roof? That's our POKEMON GYM.\p"
	.string "If you're starting out, this is where it\ngets real, your first Badge.\p"
	.string "Warm up at the POKEMON CENTER first.\nThe LEADER won't go easy on you.$"

FrostwoodText_Kid:
	.string "A trainer came through ahead of you!\nSpiky hair, real full of himself.\p"
	.string "DRACO, he said his name was. Bragged\nhe'd take the Gym Badge before anyone.\p"
	.string "Are you gonna race him? You should!$"

FrostwoodText_Worried:
	.string "You feel it too, don't you. The cold's\ngoing soft.\p"
	.string "My grandmother told stories of a wish\nsealed in the mountain, and the winter\lput here to keep it sleeping.\p"
	.string "If the winter dies... I don't want to\nthink about what wakes up.$"

FrostwoodText_MartLady:
	.string "The POKE MART just got a fresh delivery\nof POTIONS and POKE BALLS.\p"
	.string "Stock up before you head up the\nmountain trail. It's no place to be\lcaught short.$"

FrostwoodTown_Text_ClarksonCall:
	.string "………………\n"
	.string "…………… Ring!\p"
	.string "CLARKSON: {PLAYER}! It's PROFESSOR\n"
	.string "CLARKSON. Word travels fast up here,\l"
	.string "they tell me you took \l"
	.string "the EMBER BADGE already?\p"
	.string "Ha! I knew there was something about\n"
	.string "you the moment we met.\p"
	.string "That's faster growth than I've seen\n"
	.string "in a trainer in years… and that's\l"
	.string "exactly why I'm calling.\p"
	.string "I wouldn't ask if it weren't important,\n"
	.string "but I need your help with something\l"
	.string "only you can do now.\p"
	.string "Climb to the very top of MOUNT MUNEN\n"
	.string "and meet me at the summit.\p"
	.string "There's something up there I have to\n"
	.string "show you with my own hands. Not \l"
	.string "over a call, you'll understand \l"
	.string "when you see it.\p"
	.string "I'll be waiting. Don't keep an old man\n"
	.string "out in the cold too long!\p"
	.string "………………\n"
	.string "…………… Click!$"

FrostwoodTown_Text_ClarksonReminder:
	.string "The PROFESSOR wants to meet at\n"
	.string "the summit of MOUNT MUNEN.\p"
	.string "Check the REGION MAP to work out how\n"
	.string "to climb up there.$"

@ ---------------------------------------------------------------------------
@ Gondola down to the valley station (Route112_CableCarStation, whose door opens
@ onto Route_2). The station's two door tiles (45,9)/(46,9) are coord triggers,
@ not warps, so walking in asks first; the ride back from the valley warps onto
@ (45,9), and the non-animated door steps the player out onto (45,10).
FrostwoodTown_EventScript_GondolaDoor::
	lockall
	msgbox FrostwoodTown_Text_TakeGondola, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, FrostwoodTown_EventScript_GondolaDecline
	closemessage
	hidefollower
	playse SE_EXIT
	applymovement LOCALID_PLAYER, FrostwoodTown_Movement_BoardGondola
	waitmovement 0
	delay 16
	@ From the door the car hangs half off the right edge of the screen, so pan over.
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, FrostwoodTown_Movement_CameraToGondola
	waitmovement 0
	playbgm MUS_CABLE_CAR, FALSE
	delay 30
	@ Slides down the cable until it is off screen (waits for it).
	setvar VAR_0x8004, LOCALID_FROSTWOOD_GONDOLA
	special WotGondolaDepart
	delay 20
	fadescreen FADE_TO_BLACK
	special RemoveCameraObject
	setvar VAR_CABLE_CAR_STATION_STATE, 2
	incrementgamestat GAME_STAT_RODE_CABLE_CAR
	warpsilent MAP_ROUTE112_CABLE_CAR_STATION, 6, 4
	waitstate
	releaseall
	end

FrostwoodTown_EventScript_GondolaDecline::
	closemessage
	applymovement LOCALID_PLAYER, FrostwoodTown_Movement_StepOffGondolaDoor
	waitmovement 0
	releaseall
	end

FrostwoodTown_Movement_BoardGondola:
	set_invisible
	step_end

FrostwoodTown_Movement_CameraToGondola:
	walk_right
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

FrostwoodTown_Movement_StepOffGondolaDoor:
	walk_down
	step_end

FrostwoodTown_Text_TakeGondola:
	.string "Would you like to take the GONDOLA?$"

@ ---------------------------------------------------------------------------
@ Frostwood Gym is locked until RedFatality gets his EPIC PASS on Route 2
@ (FLAG_REDFATALITY_HAS_EPIC_PASS). The door is an animated door, so there is no
@ tile event to catch: WotGetLockedDoorScript (field_control_avatar.c) runs
@ GymLocked in place of the door warp. The note itself is a bg_event on (41,9).
FrostwoodTown_EventScript_GymLocked::
	lockall
	playse SE_WALL_HIT
	msgbox FrostwoodTown_Text_GymNote, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, FrostwoodTown_Movement_StepBackFromGym
	waitmovement 0
	releaseall
	end

FrostwoodTown_EventScript_GymNote::
	goto_if_set FLAG_REDFATALITY_HAS_EPIC_PASS, FrostwoodTown_EventScript_GymNoteGone
	msgbox FrostwoodTown_Text_GymNote, MSGBOX_SIGN
FrostwoodTown_EventScript_GymNoteGone::
	end

FrostwoodTown_Movement_StepBackFromGym:
	walk_down
	step_end

FrostwoodTown_Text_GymNote:
	.string "There's a note posted on the door.\p"
	.string "“FROSTWOOD's GYM LEADER\n"
	.string "is out on PTO.”$"
