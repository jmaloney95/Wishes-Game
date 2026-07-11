NationalParkAct2_MapScripts::
	.byte 0

@ ============================================================================
@  NATIONAL PARK -- ACT 2 (occupied)
@  The oni group holds the park. Fog, empty beach, ghosts loose near the
@  grave. Goons seal every land exit; only the gravesite is left open --
@  "the master said the grave stays open."
@  Entry/exit: the silent boatman ferries to/from Tradewind Town.
@  The gravesite is a thin place: with the CATALPA BOW it crosses to the
@  Distortion World (Prof. Clarkson).
@  Blocker goons reuse OniMask_EventScript_GoonNod (mask-aware).
@  Trainer goons use the Carved Mask disguise pattern (checkitem first).
@ ============================================================================

@ --------------------------------------------------------------------------
@  Silent boatman (return trip). He never speaks.
@ --------------------------------------------------------------------------
NationalParkAct2_EventScript_Boatman::
	lock
	faceplayer
	msgbox NationalParkAct2_Text_BoatmanOffer, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq NationalParkAct2_EventScript_BoatmanStay
	closemessage
	fadescreen FADE_TO_BLACK
	playse SE_SHIP
	delay 30
	warp MAP_TRADEWIND_TOWN, 18, 27
	waitstate
	release
	end

NationalParkAct2_EventScript_BoatmanStay::
	msgbox NationalParkAct2_Text_BoatmanStay, MSGBOX_DEFAULT
	release
	end

@ --------------------------------------------------------------------------
@  Gravesite -- the thin place. The sign is flavor + a hint; the CROSSING
@  happens when the player USES the CATALPA BOW from the bag nearby
@  (ItemUseOutOfBattle_CatalpaBow in src/item_use.c runs
@   NationalParkAct2_EventScript_CatalpaBowCrossing).
@ --------------------------------------------------------------------------
NationalParkAct2_EventScript_Gravesite::
	lockall
	goto_if_set FLAG_ACT2_MET_CLARKSON_DISTORTION, NationalParkAct2_EventScript_GraveSealed
	checkitem ITEM_CATALPA_BOW
	goto_if_eq VAR_RESULT, FALSE, NationalParkAct2_EventScript_GraveNoBow
	msgbox NationalParkAct2_Text_GraveHasBow, MSGBOX_DEFAULT
	releaseall
	end

NationalParkAct2_EventScript_GraveNoBow::
	msgbox NationalParkAct2_Text_GraveNoBow, MSGBOX_DEFAULT
	releaseall
	end

NationalParkAct2_EventScript_GraveSealed::
	msgbox NationalParkAct2_Text_GraveSealed, MSGBOX_DEFAULT
	releaseall
	end

@ Entry point for USING the Catalpa Bow near the grave (src/item_use.c).
NationalParkAct2_EventScript_CatalpaBowCrossing::
	lockall
	goto_if_set FLAG_ACT2_MET_CLARKSON_DISTORTION, NationalParkAct2_EventScript_BowQuiet
	goto_if_set FLAG_ACT2_GRAVE_PORTAL_OPENED, NationalParkAct2_EventScript_BowCrossQuick
	msgbox NationalParkAct2_Text_GraveFirstCrossing, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_ACT2_GRAVE_PORTAL_OPENED
	compare VAR_QUEST_FATHERS_WISH, 3
	goto_if_ne NationalParkAct2_EventScript_BowDoCross
	setvar VAR_QUEST_FATHERS_WISH, 100
	completequest QUEST_GRANDFATHERS_WISH
	closemessage
	goto NationalParkAct2_EventScript_BowDoCross

NationalParkAct2_EventScript_BowCrossQuick::
	msgbox NationalParkAct2_Text_GraveQuickCrossing, MSGBOX_DEFAULT
	closemessage
	goto NationalParkAct2_EventScript_BowDoCross

NationalParkAct2_EventScript_BowDoCross::
	setvar VAR_0x8004, 1  @ vertical pan
	setvar VAR_0x8005, 1  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 4  @ shake delay
	special ShakeCamera
	waitstate
	@ Reality warps: the double flash shows the world mid-distortion before it
	@ dissolves for good (see field_specials.c).
	playse SE_M_SUPERSONIC
	special StartRiftDistortion
	delay 48
	playse SE_WARP_IN
	fadescreen FADE_TO_WHITE
	delay 16
	fadescreen FADE_FROM_WHITE
	delay 8
	fadescreen FADE_TO_WHITE
	delay 16
	special EndRiftDistortion
	warp MAP_DISTORTION_WORLD, 15, 24
	waitstate
	releaseall
	end

NationalParkAct2_EventScript_BowQuiet::
	msgbox NationalParkAct2_Text_BowQuiet, MSGBOX_DEFAULT
	releaseall
	end

@ --------------------------------------------------------------------------
@  Goons
@ --------------------------------------------------------------------------
NationalParkAct2_EventScript_GoonEast::
	checkitem ITEM_CARVED_MASK
	goto_if_eq VAR_RESULT, TRUE, OniMask_EventScript_GoonFooled
	trainerbattle_single TRAINER_ACT2_PARK_GOON_EAST, NationalParkAct2_Text_GoonEastIntro, NationalParkAct2_Text_GoonEastDefeat
	msgbox NationalParkAct2_Text_GoonEastPostBattle, MSGBOX_AUTOCLOSE
	end

NationalParkAct2_EventScript_GoonWest::
	checkitem ITEM_CARVED_MASK
	goto_if_eq VAR_RESULT, TRUE, OniMask_EventScript_GoonFooled
	trainerbattle_single TRAINER_ACT2_PARK_GOON_WEST, NationalParkAct2_Text_GoonWestIntro, NationalParkAct2_Text_GoonWestDefeat
	msgbox NationalParkAct2_Text_GoonWestPostBattle, MSGBOX_AUTOCLOSE
	end

NationalParkAct2_EventScript_GraveWatcher::
	lock
	faceplayer
	msgbox NationalParkAct2_Text_GraveWatcher, MSGBOX_DEFAULT
	release
	end

@ --------------------------------------------------------------------------
@  Ghosts -- the park's dead are restless now
@ --------------------------------------------------------------------------
NationalParkAct2_EventScript_Misdreavus::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_MISDREAVUS, CRY_MODE_NORMAL
	msgbox NationalParkAct2_Text_Misdreavus, MSGBOX_DEFAULT
	waitmoncry
	release
	end

NationalParkAct2_EventScript_Haunter::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_HAUNTER, CRY_MODE_NORMAL
	msgbox NationalParkAct2_Text_Haunter, MSGBOX_DEFAULT
	waitmoncry
	release
	end

NationalParkAct2_EventScript_Duskull::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_DUSKULL, CRY_MODE_NORMAL
	msgbox NationalParkAct2_Text_Duskull, MSGBOX_DEFAULT
	waitmoncry
	release
	end

@ ============================================================================
@  Text
@ ============================================================================
NationalParkAct2_Text_BoatmanOffer::
	.string "The boatman says nothing.\p"
	.string "He only turns his lantern toward\n"
	.string "the far shore, and waits.\p"
	.string "Return to Tradewind Town?$"

NationalParkAct2_Text_BoatmanStay::
	.string "The boatman settles back against\n"
	.string "the tiller.\p"
	.string "He does not look at the park.\n"
	.string "He has not looked at it once.$"

NationalParkAct2_Text_GraveHasBow::
	.string "The grave marker faces the distant\n"
	.string "summit, same as it always has.\p"
	.string "The air above the stone RIPPLES...\n"
	.string "and at your hip, the Catalpa Bow\l"
	.string "stirs against its cord, wanting\l"
	.string "to be USED.$"

NationalParkAct2_Text_GraveSealed::
	.string "The grave marker faces the distant\n"
	.string "summit.\p"
	.string "The air above the stone is still\n"
	.string "now. Entirely, restfully still.\p"
	.string "Whatever was open here has closed,\n"
	.string "and taken its grin with it.$"

NationalParkAct2_Text_BowQuiet::
	.string "{PLAYER} raised the Catalpa Bow...\p"
	.string "The string does not stir.\n"
	.string "The seam has closed for good.$"

NationalParkAct2_Text_GraveNoBow::
	.string "The grave marker faces the distant\n"
	.string "summit, same as it always has.\p"
	.string "'For those who climb and do not\n"
	.string "return. The mountain remembers.'\p"
	.string "The air above the stone RIPPLES,\n"
	.string "like heat, though the fog is cold.\p"
	.string "Something here is open a crack...\n"
	.string "but your empty hands can't find\l"
	.string "the seam.$"

NationalParkAct2_Text_GraveFirstCrossing::
	.string "The air above the stone ripples.\p"
	.string "The Catalpa Bow trembles at your\n"
	.string "hip -- then hums, one low note,\l"
	.string "though nothing touched the string.\p"
	.string "The ripple widens. The fog leans\n"
	.string "IN, like breath drawn through a\l"
	.string "keyhole.\p"
	.string "The seam between here and THERE\n"
	.string "opens before you.$"

NationalParkAct2_Text_QuestComplete::
	.string "Quest complete:\n"
	.string "A FATHER'S FINAL WISH!\p"
	.string "Somewhere behind the wind, an old\n"
	.string "man's breath finally settles.$"

NationalParkAct2_Text_GraveQuickCrossing::
	.string "The bow hums its one low note.\n"
	.string "The seam remembers you.$"

NationalParkAct2_Text_GoonEastIntro::
	.string "The beach is CLOSED. The park is\n"
	.string "CLOSED. Everything is closed!\p"
	.string "How did you even get in here?!$"

NationalParkAct2_Text_GoonEastDefeat::
	.string "The boat... nobody watches\n"
	.string "the WATER...$"

NationalParkAct2_Text_GoonEastPostBattle::
	.string "Go on then, wander the fog.\n"
	.string "The dead have the run of the place\l"
	.string "anyway.$"

NationalParkAct2_Text_GoonWestIntro::
	.string "No entry past this point. The\n"
	.string "master's orders were EXACT:\p"
	.string "every road sealed... except the\n"
	.string "grave. Hey -- you're not one of\l"
	.string "ours!$"

NationalParkAct2_Text_GoonWestDefeat::
	.string "Fine! FINE! Take it up with\n"
	.string "what's waiting past the grave!$"

NationalParkAct2_Text_GoonWestPostBattle::
	.string "Why leave the grave open, you ask?\p"
	.string "Because the master says something\n"
	.string "on the OTHER side owes him a debt.$"

NationalParkAct2_Text_GraveWatcher::
	.string "...You feel it too, don't you.\p"
	.string "The gravestone doesn't cast fog\n"
	.string "like the rest. It DRINKS it.\p"
	.string "The master said the grave stays\n"
	.string "open. For HIM.\p"
	.string "I don't ask which him.$"

NationalParkAct2_Text_Misdreavus::
	.string "A Misdreavus circles the grave\n"
	.string "path, keening softly.\p"
	.string "It sounds almost like a name.$"

NationalParkAct2_Text_Haunter::
	.string "A Haunter hangs in the fog,\n"
	.string "grinning at nothing.\p"
	.string "Nothing grins back. Probably.$"

NationalParkAct2_Text_Duskull::
	.string "A Duskull drifts between the\n"
	.string "grave markers, slow as a lantern\l"
	.string "on a still night.\p"
	.string "It is keeping a path clear.\n"
	.string "For whom?$"
