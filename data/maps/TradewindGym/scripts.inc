@ Tradewind Gym -- the Lantern Gym hall (Dewford gym layout). Madam Tsuji and
@ her procession hold the trial inside: Goro, the Lantern Bearer, and the
@ Apprentice stand along the winding path, Tsuji waits at the far shrine.
@ Their battle scripts live in TradewindTown/scripts.inc (labels are global);
@ Tsuji requires all three defeated before she will fight.
TradewindGym_MapScripts::
	.byte 0

TradewindGym_EventScript_Acolyte::
	lock
	faceplayer
	msgbox TradewindGym_Text_Acolyte, MSGBOX_DEFAULT
	release
	end

TradewindGym_EventScript_Meditant::
	lock
	faceplayer
	msgbox TradewindGym_Text_Meditant, MSGBOX_DEFAULT
	release
	end

TradewindGym_Text_Acolyte::
	.string "Welcome to the LANTERN GYM,\n"
	.string "challenger.\p"
	.string "Three keepers tend the flames\n"
	.string "along the path: the fist, the\l"
	.string "lantern, and the apprentice.\p"
	.string "Douse all three, and MADAM TSUJI\n"
	.string "will grant you her trial at the\l"
	.string "far shrine.\p"
	.string "Mind the dark between the lamps.\n"
	.string "It minds you back.$"

TradewindGym_Text_Meditant::
	.string "…Each lantern in this hall holds\n"
	.string "a name. When one gutters, we\l"
	.string "relight it within the hour.\p"
	.string "We missed one, once.\p"
	.string "We do not talk about the\n"
	.string "hundred and ninth lantern.$"
