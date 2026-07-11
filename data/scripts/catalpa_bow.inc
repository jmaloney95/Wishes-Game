@ ============================================================================
@  CATALPA BOW -- GENGAR GHOST-ROADS (Act 2)
@  After the Distortion World scene, using the Catalpa Bow anywhere in the
@  field opens Gengar-Clarkson's travel menu from the very first use:
@  Clarkson's Lab or the Distortion World. There is deliberately NO direct
@  ride to Munen Village -- occupied Munen is only reached by walking out of
@  the lab, and the lab's exit gate holds until Draco is beaten and the
@  research choice is made.
@  Entry points are dispatched from ItemUseOutOfBattle_CatalpaBow
@  (src/item_use.c): Ferry when Gengar is in the party, NoGengar otherwise.
@ ============================================================================
EventScript_CatalpaBowGengarFerry::
	lockall
	waitse
	playmoncry SPECIES_GENGAR, CRY_MODE_NORMAL
	waitmoncry
	setspeaker SP_NAME_GENGAR
	msgbox Text_GengarTravelMenu, MSGBOX_DEFAULT
	multichoice 0, 0, MULTI_CATALPA_DESTINATIONS, FALSE
	switch VAR_RESULT
	case 0, EventScript_CatalpaBowToLab
	case 1, EventScript_CatalpaBowToDistortion
	@ CANCEL / B button
	goto EventScript_CatalpaBowFerryDecline

EventScript_CatalpaBowToLab::
	setflag FLAG_ACT2_CROSSED_TO_MUNEN
	closemessage
	playse SE_WARP_IN
	fadescreen FADE_TO_BLACK
	delay 20
	@ Land one tile ABOVE the exit-gate trigger row (y=11): coord triggers only
	@ fire on stepping onto a tile, so arriving on the row itself would let the
	@ player slip out the door without the gate ever firing.
	warp MAP_MUNEN_LAB_ACT2, 6, 10
	waitstate
	releaseall
	end

EventScript_CatalpaBowToDistortion::
	closemessage
	@ Reality warps: mosaic pulse + camera wobble (field_specials.c) ramps in,
	@ the white fade dissolves the distorted world, and cleanup happens while
	@ the screen is blank.
	playse SE_M_SUPERSONIC
	special StartRiftDistortion
	delay 48
	playse SE_WARP_IN
	fadescreen FADE_TO_WHITE
	special EndRiftDistortion
	delay 20
	@ Ferry-era trips land in DistortionWorld_2 -- the reformed room where the
	@ island has risen across the x25 void (first arrival plays the reveal;
	@ see DistortionWorld_2/scripts.inc). The original MAP_DISTORTION_WORLD is
	@ only ever seen once, during the Gengar-Clarkson scene.
	warp MAP_DISTORTION_WORLD_2, 15, 24
	waitstate
	releaseall
	end

EventScript_CatalpaBowFerryDecline::
	msgbox Text_GengarFerryDecline, MSGBOX_DEFAULT
	releaseall
	end

@ Scene done, but Gengar-Clarkson was left in the PC.
EventScript_CatalpaBowNoGengar::
	lockall
	msgbox Text_CatalpaBowNoGengar, MSGBOX_DEFAULT
	releaseall
	end

@ ============================================================================
@  Text
@ ============================================================================
Text_GengarTravelMenu::
	.string "GENGAR pours out of its Poke Ball\n"
	.string "like smoke.\p"
	.string "“The ghost-roads are open,\n"
	.string "{PLAYER}. Where to?”$"

Text_GengarFerryDecline::
	.string "“Kekeke. Wise. Sharpen everything\n"
	.string "first.\p"
	.string "“The bow knows where I am.\n"
	.string "Call when you're ready.”\p"
	.string "GENGAR seeps back into its\n"
	.string "Poke Ball.$"

Text_CatalpaBowNoGengar::
	.string "{PLAYER} raised the Catalpa Bow.\p"
	.string "The string hums... and hums...\n"
	.string "but nothing answers.\p"
	.string "The ghost it calls for is shut\n"
	.string "away in a PC Box somewhere,\l"
	.string "sulking.$"
