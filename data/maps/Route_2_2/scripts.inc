Route_2_2_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Route_2_2_OnResume
	.byte 0

Route_2_2_OnResume::
	questindicator 12, SIDE_QUEST_EIGHT_WAYS  @ Eevee breeder
	questindicator 20, SIDE_QUEST_HEAVY_METAL @ Scyther collector
	flagindicator 21, FLAG_NATIONAL_PARK_CUT_GIVEN @ Cut tutor
	end

@ ===========================================================================
@ Park guide - welcomes the player and describes the wildlife
@ ===========================================================================
Route_2_2_EventScript_ParkGuide::
	lockall
	@ Keep the follower Pokemon out of the camera choreography: without this,
	@ every applymovement on the camera also force-hides the follower and every
	@ `waitmovement 0` waits on the follower's enter-ball movement too -- which
	@ can hang the script (permanent black screen / stuck controls) at the end
	@ of the tour. Same guard the engine's std sign scripts use.
	setflag FLAG_SAFE_FOLLOWER_MOVEMENT
	faceplayer
	goto_if_set FLAG_NATIONAL_PARK_TOUR_DONE, Route_2_2_EventScript_ParkGuideAgain
	msgbox Route_2_2_Text_GuideWelcome, MSGBOX_DEFAULT
	closemessage
	special SpawnCameraObject
	@ Cut to shot 1: pan east along y54 (x25 -> x35)
	fadescreen FADE_TO_BLACK
	applymovement LOCALID_CAMERA, Route_2_2_Movement_CamToShot1
	waitmovement 0
	fadescreen FADE_FROM_BLACK
	applymovement LOCALID_CAMERA, Route_2_2_Movement_PanEast
	msgbox Route_2_2_Text_GuideShot1, MSGBOX_DEFAULT
	waitmovement 0
	closemessage
	@ Jump cut to shot 2: pan south along x51 (y51 -> y61)
	fadescreen FADE_TO_BLACK
	applymovement LOCALID_CAMERA, Route_2_2_Movement_CamToShot2
	waitmovement 0
	fadescreen FADE_FROM_BLACK
	applymovement LOCALID_CAMERA, Route_2_2_Movement_PanSouth
	msgbox Route_2_2_Text_GuideShot2, MSGBOX_DEFAULT
	waitmovement 0
	closemessage
	@ Cut to final still at (47,19)
	fadescreen FADE_TO_BLACK
	applymovement LOCALID_CAMERA, Route_2_2_Movement_CamToShot3
	waitmovement 0
	fadescreen FADE_FROM_BLACK
	msgbox Route_2_2_Text_GuideShot3, MSGBOX_DEFAULT
	closemessage
	@ Return control to the player
	fadescreen FADE_TO_BLACK
	@ Walk the camera back onto the player BEFORE removing it, or the player is
	@ left steering the camera. (net forward pan was east 18 / north 47)
	applymovement LOCALID_CAMERA, Route_2_2_Movement_CamReturn
	waitmovement 0
	special RemoveCameraObject
	fadescreen FADE_FROM_BLACK
	setflag FLAG_NATIONAL_PARK_TOUR_DONE
	clearflag FLAG_SAFE_FOLLOWER_MOVEMENT
	releaseall
	end

@ After the first tour she just gives a short greeting (no camera demo).
Route_2_2_EventScript_ParkGuideAgain::
	msgbox Route_2_2_Text_GuideAgain, MSGBOX_DEFAULT
	clearflag FLAG_SAFE_FOLLOWER_MOVEMENT
	releaseall
	end

@ Camera spawns on the player (29,66). Hidden moves use walk_faster (under a
@ fade); visible pans use walk_slow for a cinematic feel.
Route_2_2_Movement_CamToShot1:        @ 29,66 -> 25,54  (west 4, north 12)
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
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

Route_2_2_Movement_PanEast:           @ 25,54 -> 41,54  (east 16)
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	walk_slow_right
	step_end

Route_2_2_Movement_CamToShot2:        @ 41,54 -> 51,51  (east 10, north 3)
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
	step_end

Route_2_2_Movement_PanSouth:          @ 51,51 -> 51,69  (south 18)
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	walk_slow_down
	step_end

Route_2_2_Movement_CamToShot3:        @ 51,69 -> 47,19  (west 4, north 50)
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
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

@ ===========================================================================
@ Item ball - TM Earthquake
@ ===========================================================================
Route_2_2_EventScript_TMEarthquake::
	lockall
	giveitem ITEM_TM_EARTHQUAKE
	setflag FLAG_ITEM_NATIONAL_PARK_TM_EARTHQUAKE
	removeobject VAR_LAST_TALKED
	releaseall
	end

@ ===========================================================================
@ Trainers (reuse existing parties, park-themed dialogue)
@ ===========================================================================
@ SIREN ALLISON: one-liner through the plate/portrait, boss card, battle.
@ The trainer flag is shared across acts, so the card fires exactly once --
@ at whichever of her encounters the player reaches first.
Route_2_2_EventScript_SwimmerAllison::
	goto_if_defeated TRAINER_ROUTE2_SWIMMER_ALLISON, Route_2_2_EventScript_AllisonAfter
	lockall
	setspeaker SP_NAME_ALLISON
	msgbox Route_2_2_Text_AllisonIntro, MSGBOX_DEFAULT
	closemessage
	@ Clear the SE bed so the banner shimmer is not fighting the surf.
	special WotSuppressWeatherSE
	special WotStopSceneSE
	bossintro BOSS_INTRO_ALLISON
	trainerbattle_no_intro TRAINER_ROUTE2_SWIMMER_ALLISON, Route_2_2_Text_AllisonDefeat
	setspeaker SP_NAME_ALLISON
	msgbox Route_2_2_Text_AllisonPost, MSGBOX_DEFAULT
	releaseall
	end

Route_2_2_EventScript_AllisonAfter::
	lock
	faceplayer
	setspeaker SP_NAME_ALLISON
	msgbox Route_2_2_Text_AllisonPost, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_SwimmerMarina::
	trainerbattle_single TRAINER_ROUTE2_SWIMMER_MARINA, Route_2_2_Text_MarinaIntro, Route_2_2_Text_MarinaDefeat
	msgbox Route_2_2_Text_MarinaPost, MSGBOX_AUTOCLOSE
	end

@ ===========================================================================
@ Friendly beachgoers
@ ===========================================================================
Route_2_2_EventScript_BeachGoer1::
	lock
	faceplayer
	msgbox Route_2_2_Text_BeachGoer1, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_BeachGoer2::
	lock
	faceplayer
	msgbox Route_2_2_Text_BeachGoer2, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_Picnicker::
	lock
	faceplayer
	msgbox Route_2_2_Text_Picnicker, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_Tuber::
	lock
	faceplayer
	msgbox Route_2_2_Text_Tuber, MSGBOX_DEFAULT
	release
	end

@ Eevee breeder - sells a (secretly shiny) baby Eevee for 10,000, one time
Route_2_2_EventScript_Beauty::
Route_2_2_EventScript_EeveeBreeder::
	lock
	faceplayer
	goto_if_set FLAG_NATIONAL_PARK_EEVEE_BREEDER, Route_2_2_EventScript_BreederAfter
	msgbox Route_2_2_Text_BreederIntro, MSGBOX_DEFAULT
	questmenu QUEST_MENU_CHECK_UNLOCKED, SIDE_QUEST_EIGHT_WAYS
	call_if_eq VAR_RESULT, FALSE, Route_2_2_EventScript_StartEightWaysQuest
	msgbox Route_2_2_Text_BreederOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route_2_2_EventScript_BreederDecline
	checkmoney 10000
	goto_if_eq VAR_RESULT, FALSE, Route_2_2_EventScript_BreederNoMoney
	removemoney 10000
	givemon SPECIES_EEVEE, 5, shinyMode=SHINY_MODE_ALWAYS
	goto_if_eq VAR_RESULT, MON_CANT_GIVE, Route_2_2_EventScript_BreederNoRoom
	playfanfare MUS_OBTAIN_ITEM
	msgbox Route_2_2_Text_BreederGive, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_NATIONAL_PARK_EEVEE_BREEDER
	completequest SIDE_QUEST_EIGHT_WAYS
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC, Route_2_2_EventScript_BreederToPC
	release
	end

Route_2_2_EventScript_StartEightWaysQuest::
	startquest SIDE_QUEST_EIGHT_WAYS
	return

Route_2_2_EventScript_BreederToPC::
	msgbox Route_2_2_Text_BreederPC, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_BreederNoRoom::
	addmoney 10000
	msgbox Route_2_2_Text_BreederNoRoom, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_BreederNoMoney::
	msgbox Route_2_2_Text_BreederNoMoney, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_BreederDecline::
	msgbox Route_2_2_Text_BreederDecline, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_BreederAfter::
	msgbox Route_2_2_Text_BreederAfter, MSGBOX_DEFAULT
	release
	end

@ ===========================================================================
@ Ghosts drifting near the graves
@ ===========================================================================
Route_2_2_EventScript_Misdreavus::
	lock
	faceplayer
	msgbox Route_2_2_Text_Misdreavus, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_Haunter::
	lock
	faceplayer
	msgbox Route_2_2_Text_Haunter, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_Gastly::
	lock
	faceplayer
	msgbox Route_2_2_Text_Gastly, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_Duskull::
	lock
	faceplayer
	msgbox Route_2_2_Text_Duskull, MSGBOX_DEFAULT
	release
	end

@ ===========================================================================
@ Cute companion Pokemon wandering the park
@ ===========================================================================
Route_2_2_EventScript_PetMarill::
	lock
	faceplayer
	msgbox Route_2_2_Text_PetMarill, MSGBOX_DEFAULT
	release
	end

@ Frolicking Gible (object at 34,49). Dual label so it works whether the map
@ event points at the old or new name.
Route_2_2_EventScript_PetPikachu::
Route_2_2_EventScript_Gible::
	lock
	faceplayer
	msgbox Route_2_2_Text_Gible, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_PetEevee::
	lock
	faceplayer
	msgbox Route_2_2_Text_PetEevee, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_Wingull::
	lock
	faceplayer
	msgbox Route_2_2_Text_Wingull, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_Psyduck::
Route_2_2_EventScript_Sandile::
	lock
	faceplayer
	msgbox Route_2_2_Text_Sandile, MSGBOX_DEFAULT
	release
	end

@ ===========================================================================
@ Text
@ ===========================================================================
Route_2_2_Text_GuideWelcome:
	.string "Welcome to the NATIONAL PARK!\p"
	.string "First time here? Here, let me show\n"
	.string "you around our little paradise.$"

Route_2_2_Text_GuideShot1:
	.string "Off to the west, our sunny meadows\n"
	.string "teem with GRASS POKéMON of every\l"
	.string "shape and color.$"

Route_2_2_Text_GuideShot2:
	.string "Follow the path down to the eastern\n"
	.string "shore, where WATER POKéMON splash\l"
	.string "and play in the shallows.$"

Route_2_2_Text_GuideShot3:
	.string "And up north stands the old\n"
	.string "memorial…\p"
	.string "They say gentle SPIRITS still drift\n"
	.string "among the graves. Do be respectful\l"
	.string "if you wander up that way.$"

Route_2_2_Text_GuideAgain:
	.string "Enjoy the NATIONAL PARK!\p"
	.string "GRASS types to the west, the WATER\n"
	.string "and the shore to the east.$"

Route_2_2_Text_AllisonIntro:
	.string "Prepare to face my team of\n"
	.string "BUNNIES.$"

Route_2_2_Text_AllisonDefeat:
	.string "At least i'm getting a tan!$"

Route_2_2_Text_AllisonPost:
	.string "I need to catch more bunnies!$"

Route_2_2_Text_MarinaIntro:
	.string "My POKéMON have been itching for\n"
	.string "a battle. You're up!$"

Route_2_2_Text_MarinaDefeat:
	.string "That was a blast! Good game.$"

Route_2_2_Text_MarinaPost:
	.string "If you fish from the shore,\n"
	.string "you'll find all kinds of POKéMON.$"

Route_2_2_Text_BeachGoer1:
	.string "I just floated around all afternoon.\n"
	.string "This park is paradise!$"

Route_2_2_Text_BeachGoer2:
	.string "My buddies and I drove out here just\n"
	.string "to let our POKéMON play in the sand.$"

Route_2_2_Text_Picnicker:
	.string "I want to finish this Krabby Patty,\n"
	.string "but i'm just so fucking full..\n"
	.string "Oh well.$"

Route_2_2_Text_Tuber:
	.string "Look, look! My MARILL learned to\n"
	.string "float on its tail! So cute!$"

Route_2_2_Text_BreederIntro:
	.string "Why hello, dear! Aren't you a\n"
	.string "well-mannered trainer.\p"
	.string "I raise EEVEE here in the park. I\n"
	.string "brush their coats till they gleam.\p"
	.string "True beauty begins with good\n"
	.string "grooming, you know.$"

Route_2_2_Text_BreederOffer:
	.string "One of my EEVEE just had the most\n"
	.string "darling little litter.\p"
	.string "I could let one of the babies go to\n"
	.string "a kind trainer like you…\p"
	.string "Say, for 10,000? What do you think?$"

Route_2_2_Text_BreederGive:
	.string "Wonderful! Here is your little one.\p"
	.string "Brush it every day and it will grow\n"
	.string "up simply gorgeous!$"

Route_2_2_Text_BreederPC:
	.string "Oh my, your team is full, so I sent\n"
	.string "the little EEVEE to your PC.$"

Route_2_2_Text_BreederNoMoney:
	.string "Oh dear, you're a little short.\p"
	.string "Come back when you've saved up\n"
	.string "10,000.$"

Route_2_2_Text_BreederNoRoom:
	.string "Goodness, you've no room for it,\n"
	.string "not even in your PC!\p"
	.string "Here's your money back. Do come\n"
	.string "again, dear.$"

Route_2_2_Text_BreederDecline:
	.string "No? That's quite all right.\p"
	.string "An EEVEE should only go to someone\n"
	.string "who truly wants one.$"

Route_2_2_Text_BreederAfter:
	.string "How is your little EEVEE getting on?\p"
	.string "Keep that coat brushed and it'll\n"
	.string "shine like a star!$"

Route_2_2_Text_Misdreavus:
	.string "MISDREAVUS: Misss…\p"
	.string "It giggles, drifts closer, then\n"
	.string "darts away into the mist.$"

Route_2_2_Text_Haunter:
	.string "A HAUNTER hovers over the graves,\n"
	.string "watching you with a wide grin.$"

Route_2_2_Text_Gastly:
	.string "A GASTLY swirls past in a cloud of\n"
	.string "purple haze. A chill runs through\l"
	.string "you.$"

Route_2_2_Text_Duskull:
	.string "DUSKULL floats silently among the\n"
	.string "headstones, as if standing guard.$"

Route_2_2_Text_PetMarill:
	.string "A MARILL bounces happily along the\n"
	.string "water's edge.$"

Route_2_2_Text_Gible:
	.string "A GIBLE frolics through the tall\n"
	.string "weeds, snapping playfully at\l"
	.string "drifting petals.$"

Route_2_2_Text_PetEevee:
	.string "An EEVEE rolls in the warm sand,\n"
	.string "then blinks up at you.$"

Route_2_2_Text_Wingull:
	.string "A WINGULL wheels overhead, riding\n"
	.string "the sea breeze.$"

Route_2_2_Text_Sandile:
	.string "A SANDILE lazes in the warm sand,\n"
	.string "eyes just above the surface.$"

@ ===========================================================================
@ Added beachgoer trainers (Eddie, Finn) + Metal Coat trade NPC
@ ===========================================================================
Route_2_2_EventScript_TuberEddie::
	trainerbattle_single TRAINER_NATIONAL_PARK_TUBER_EDDIE, Route_2_2_Text_EddieIntro, Route_2_2_Text_EddieDefeat
	msgbox Route_2_2_Text_EddiePost, MSGBOX_AUTOCLOSE
	end

Route_2_2_EventScript_SwimmerFinn::
	trainerbattle_single TRAINER_NATIONAL_PARK_SWIMMER_FINN, Route_2_2_Text_FinnIntro, Route_2_2_Text_FinnDefeat
	msgbox Route_2_2_Text_FinnPost, MSGBOX_AUTOCLOSE
	end

@ Repeatable: he takes any Scyther brought to him, every time. The starter is
@ given BEFORE the Scyther is taken so a full party+boxes can never eat the
@ player's Scyther without a reward.
Route_2_2_EventScript_MetalCoatGuy::
	lock
	faceplayer
	msgbox Route_2_2_Text_MetalCoatIntro, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Route_2_2_EventScript_MetalCoatDecline
	setvar VAR_0x8004, SPECIES_SCYTHER
	specialvar VAR_RESULT, DoesPlayerPartyContainSpecies
	compare VAR_RESULT, FALSE
	goto_if_eq Route_2_2_EventScript_MetalCoatNoScyther
	random 3
	compare VAR_RESULT, 0
	goto_if_eq Route_2_2_EventScript_MetalCoatBulbasaur
	compare VAR_RESULT, 1
	goto_if_eq Route_2_2_EventScript_MetalCoatCharmander
	goto Route_2_2_EventScript_MetalCoatSquirtle

Route_2_2_EventScript_MetalCoatBulbasaur::
	givemon SPECIES_BULBASAUR, 5
	compare VAR_RESULT, MON_CANT_GIVE
	goto_if_eq Route_2_2_EventScript_MetalCoatNoRoom
	bufferspeciesname STR_VAR_1, SPECIES_BULBASAUR
	goto Route_2_2_EventScript_MetalCoatGrant

Route_2_2_EventScript_MetalCoatCharmander::
	givemon SPECIES_CHARMANDER, 5
	compare VAR_RESULT, MON_CANT_GIVE
	goto_if_eq Route_2_2_EventScript_MetalCoatNoRoom
	bufferspeciesname STR_VAR_1, SPECIES_CHARMANDER
	goto Route_2_2_EventScript_MetalCoatGrant

Route_2_2_EventScript_MetalCoatSquirtle::
	givemon SPECIES_SQUIRTLE, 5
	compare VAR_RESULT, MON_CANT_GIVE
	goto_if_eq Route_2_2_EventScript_MetalCoatNoRoom
	bufferspeciesname STR_VAR_1, SPECIES_SQUIRTLE
	goto Route_2_2_EventScript_MetalCoatGrant

Route_2_2_EventScript_MetalCoatGrant::
	setvar VAR_0x8004, SPECIES_SCYTHER
	special TakeFirstMonOfSpecies
	msgbox Route_2_2_Text_MetalCoatThanks, MSGBOX_DEFAULT
	questmenu QUEST_MENU_CHECK_COMPLETE, SIDE_QUEST_HEAVY_METAL
	call_if_eq VAR_RESULT, FALSE, Route_2_2_EventScript_CompleteHeavyMetalQuest
	release
	end

Route_2_2_EventScript_CompleteHeavyMetalQuest::
	completequest SIDE_QUEST_HEAVY_METAL
	return

Route_2_2_EventScript_MetalCoatNoRoom::
	msgbox Route_2_2_Text_MetalCoatNoRoom, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_MetalCoatNoScyther::
	msgbox Route_2_2_Text_MetalCoatNoScyther, MSGBOX_DEFAULT
	questmenu QUEST_MENU_CHECK_UNLOCKED, SIDE_QUEST_HEAVY_METAL
	call_if_eq VAR_RESULT, FALSE, Route_2_2_EventScript_StartHeavyMetalQuest
	release
	end

Route_2_2_EventScript_MetalCoatDecline::
	msgbox Route_2_2_Text_MetalCoatDeclineMsg, MSGBOX_DEFAULT
	questmenu QUEST_MENU_CHECK_UNLOCKED, SIDE_QUEST_HEAVY_METAL
	call_if_eq VAR_RESULT, FALSE, Route_2_2_EventScript_StartHeavyMetalQuest
	release
	end

Route_2_2_EventScript_StartHeavyMetalQuest::
	startquest SIDE_QUEST_HEAVY_METAL
	return

Route_2_2_EventScript_MetalCoatDone::
	msgbox Route_2_2_Text_MetalCoatAfter, MSGBOX_DEFAULT
	release
	end

Route_2_2_Text_EddieIntro:
	.string "I built the biggest sandcastle ever\n"
	.string "and something MOVED into it!$"

Route_2_2_Text_EddieDefeat:
	.string "Aww, my sandcastle crew lost!$"

Route_2_2_Text_EddiePost:
	.string "My SANDYGAST guards this beach.\n"
	.string "Careful it doesn't pull you in!$"

Route_2_2_Text_FinnIntro:
	.string "The tide pools out here hide all\n"
	.string "sorts of strange creatures!$"

Route_2_2_Text_FinnDefeat:
	.string "You really know your sea life!$"

Route_2_2_Text_FinnPost:
	.string "A MAREANIE hides where you least\n"
	.string "expect it. Watch your step!$"

Route_2_2_Text_MetalCoatIntro:
	.string "Check it out - I dug a shiny\n"
	.string "METAL COAT out of the sand!\p"
	.string "They say a SCYTHER changes when\n"
	.string "it holds one... I'd love to see it.\p"
	.string "Got a SCYTHER you'd part with?\n"
	.string "I'll give you a little buddy for it!$"

Route_2_2_Text_MetalCoatDeclineMsg:
	.string "No? That's okay. Come back if you\n"
	.string "change your mind!$"

Route_2_2_Text_MetalCoatNoScyther:
	.string "Hmm, no SCYTHER on you right now.\n"
	.string "Come find me when you've got one!$"

Route_2_2_Text_MetalCoatThanks:
	.string "Whoa, a real SCYTHER! Incredible!\p"
	.string "Here, take this little one as my\n"
	.string "thanks - it's a surprise who!\p"
	.string "You received a {STR_VAR_1}!$"

Route_2_2_Text_MetalCoatNoRoom:
	.string "Whoa, whoa - your party and boxes\n"
	.string "are stuffed full!\p"
	.string "Make room for my thank-you gift,\n"
	.string "then come find me again.$"

Route_2_2_Text_MetalCoatAfter:
	.string "That SCYTHER and my METAL COAT\n"
	.string "are gonna be great pals. Thanks!$"

@ ===========================================================================
@ Cut tutor - gives HM01 (Cut) and points to the Walnut Woods shortcut
@ ===========================================================================
Route_2_2_EventScript_CutTutor::
	lock
	faceplayer
	goto_if_set FLAG_NATIONAL_PARK_CUT_GIVEN, Route_2_2_EventScript_CutTutorAfter
	msgbox Route_2_2_Text_CutTutorIntro, MSGBOX_DEFAULT
	giveitem ITEM_HM01
	goto_if_eq VAR_RESULT, FALSE, Route_2_2_EventScript_CutTutorBagFull
	setflag FLAG_NATIONAL_PARK_CUT_GIVEN
	msgbox Route_2_2_Text_CutTutorShortcut, MSGBOX_DEFAULT
	release
	end

Route_2_2_EventScript_CutTutorBagFull::
	release
	end

Route_2_2_EventScript_CutTutorAfter::
	msgbox Route_2_2_Text_CutTutorAfter, MSGBOX_DEFAULT
	release
	end

Route_2_2_Text_CutTutorIntro:
	.string "Off to WALNUT WOODS? There's a\n"
	.string "shortcut nearby, but a big tree\l"
	.string "blocks the way.\p"
	.string "Here - take this!$"

Route_2_2_Text_CutTutorShortcut:
	.string "Teach CUT to one of your team,\n"
	.string "then slice that tree down.\p"
	.string "It's a quick shortcut straight\n"
	.string "back to WALNUT WOODS. Give it a\l"
	.string "try!$"

Route_2_2_Text_CutTutorAfter:
	.string "Cut that tree down for a quick\n"
	.string "shortcut to WALNUT WOODS.$"

@ ===========================================================================
@ Trail signs - the north path out of the park climbs to Mount Munen
@ ===========================================================================
Route_2_2_EventScript_MountMunenSign1::
	msgbox Route_2_2_Text_MountMunenSign1, MSGBOX_SIGN
	end

Route_2_2_EventScript_MountMunenSign2::
	msgbox Route_2_2_Text_MountMunenSign2, MSGBOX_SIGN
	end

Route_2_2_Text_MountMunenSign1:
	.string "NATIONAL PARK TRAILHEAD\p"
	.string "{UP_ARROW} NORTH: MOUNT MUNEN\n"
	.string "Steep path. Dress warm.$"

Route_2_2_Text_MountMunenSign2:
	.string "{UP_ARROW} NORTH: MOUNT MUNEN\n"
	.string "MUNEN VILLAGE via the mountain trail$"

@ ===========================================================================
@ Gravesite - foreshadows the summit now; hidden Prof. Clarkson note after flag
@ ===========================================================================
Route_2_2_EventScript_Gravesite::
	lockall
	goto_if_set FLAG_NATIONAL_PARK_GRAVE_SECRET, Route_2_2_EventScript_GravesiteSecret
	msgbox Route_2_2_Text_Gravesite, MSGBOX_DEFAULT
	releaseall
	end

Route_2_2_EventScript_GravesiteSecret::
	msgbox Route_2_2_Text_GravesiteClarkson, MSGBOX_DEFAULT
	releaseall
	end

Route_2_2_Text_Gravesite:
	.string "A solemn grave marker faces the\n"
	.string "distant summit.\p"
	.string "'For those who climb and do not\n"
	.string "return. The mountain\l"
	.string "remembers.'\p"
	.string "A cold wind drifts down from the\n"
	.string "peak.$"

Route_2_2_Text_GravesiteClarkson:
	.string "A note is wedged behind the\n"
	.string "marker, freshly folded.\p"
	.string "'If you're reading this, I made\n"
	.string "it down alive.\p"
	.string "Don't trust what you saw at the\n"
	.string "summit. Keep climbing.\p"
	.string "- Prof. Clarkson'$"

Route_2_2_Movement_CamReturn:         @ ghosts (47,19) -> player spawn  (west 18, south 47)
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
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
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
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	step_end
