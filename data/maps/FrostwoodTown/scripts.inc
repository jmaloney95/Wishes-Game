FrostwoodTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, FrostwoodTown_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, FrostwoodTown_OnFrame
	.byte 0

@ Arm the Clarkson call when the player walks out of the GYM holding the EMBER
@ BADGE for the first time. Map-level (not tile-level) so it fires no matter
@ which tile the gym exit drops the player onto.
FrostwoodTown_OnTransition::
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
	showportrait PORTRAIT_CLARKSON_GENGAR
	pokenavcall FrostwoodTown_Text_ClarksonCall
	waitmessage
	closemessage
	setspeaker SP_NAME_CLARKSON
	msgbox FrostwoodTown_Text_ClarksonReminder, MSGBOX_DEFAULT
	hideportrait
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
	.string "But walk the trees now and you'll hear\nit -- dripping. The frost is letting go.\p"
	.string "Snow that old should never melt. Same\nas they say down in MUNEN.$"

FrostwoodText_GymWoman:
	.string "See that big building with the snowed-\nover roof? That's our POKEMON GYM.\p"
	.string "If you're starting out, this is where it\ngets real -- your first Badge.\p"
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
	.string "… … … … … …\n"
	.string "… … … … … Ring!\p"
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
	.string "over a call -- you'll understand \l"
	.string "when you see it.\p"
	.string "I'll be waiting. Don't keep an old man\n"
	.string "out in the cold too long!\p"
	.string "… … … … … …\n"
	.string "… … … … … Click!$"

FrostwoodTown_Text_ClarksonReminder:
	.string "PROFESSOR CLARKSON wants to meet at\n"
	.string "the summit of MOUNT MUNEN.\p"
	.string "Check the REGION MAP to work out how\n"
	.string "to climb up there.$"
