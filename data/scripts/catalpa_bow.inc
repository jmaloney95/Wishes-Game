@ ============================================================================
@  CATALPA BOW -- GENGAR FERRY (Act 2)
@  After the Distortion World scene, using the Catalpa Bow anywhere in the
@  field lets Gengar-Clarkson offer passage home to Munen Village -- now
@  overrun by Team Mutrid. This is the ACT 2 POINT OF NO RETURN: the player
@  is warned, can decline freely (train, finish the Tradewind gym), and can
@  use the bow again later when ready.
@  Entry points are dispatched from ItemUseOutOfBattle_CatalpaBow
@  (src/item_use.c): Ferry when Gengar is in the party, NoGengar otherwise.
@ ============================================================================
EventScript_CatalpaBowGengarFerry::
	lockall
	waitse
	playmoncry SPECIES_GENGAR, CRY_MODE_NORMAL
	waitmoncry
	goto_if_set FLAG_ACT2_CROSSED_TO_MUNEN, EventScript_CatalpaBowFerryAgain
	msgbox Text_GengarFerryOfferFirst, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_CatalpaBowFerryDecline
	setflag FLAG_ACT2_CROSSED_TO_MUNEN
	goto EventScript_CatalpaBowFerryGo

EventScript_CatalpaBowFerryAgain::
	msgbox Text_GengarFerryOfferAgain, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq EventScript_CatalpaBowFerryDecline
	goto EventScript_CatalpaBowFerryGo

EventScript_CatalpaBowFerryGo::
	closemessage
	playse SE_WARP_IN
	fadescreen FADE_TO_BLACK
	delay 20
	setvar VAR_0x8004, 1  @ vertical pan
	setvar VAR_0x8005, 1  @ horizontal pan
	setvar VAR_0x8006, 6  @ num shakes
	setvar VAR_0x8007, 4  @ shake delay
	special ShakeCamera
	waitstate
	warp MAP_MUNEN_VILLAGE_ACT2, 9, 15
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
Text_GengarFerryOfferFirst::
	.string "The bow hums -- and GENGAR pours\n"
	.string "out of its Poke Ball like smoke.\p"
	.string "“Thinking of home, {PLAYER}?\p"
	.string "“The roads to Munen are cut. But\n"
	.string "the dead were walking between\l"
	.string "these mountains long before there\l"
	.string "were roads.\p"
	.string "“I can carry you there, the way\n"
	.string "ghosts travel.\p"
	.string "“But hear me first: Munen has\n"
	.string "FALLEN. My son's people hold your\l"
	.string "village.\p"
	.string "“Once we cross, the way back is\n"
	.string "THEIRS, not ours. Finish whatever\l"
	.string "business you have out here --\l"
	.string "training, that lantern gym --\l"
	.string "BEFORE you say yes.\p"
	.string "“Well? Shall we go home?”$"

Text_GengarFerryOfferAgain::
	.string "GENGAR pours out of its Poke Ball\n"
	.string "like smoke.\p"
	.string "“The ghost-road to Munen is open,\n"
	.string "{PLAYER}. Shall we?”$"

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
