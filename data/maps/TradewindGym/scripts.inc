@ Tradewind Gym hall -- the indoor shrine of the Lantern Gym. The trial itself
@ is held OUTSIDE under the lanterns (Madam Tsuji and her procession stand by
@ the door); this hall is where the flames are tended.
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
	.string "Looking for the LANTERN GYM's\n"
	.string "trial? Then turn around.\p"
	.string "MADAM TSUJI holds her trial out\n"
	.string "under the open lanterns, where\l"
	.string "the spirits can watch.\p"
	.string "In here, we only keep the flames\n"
	.string "fed. All one hundred and eight\l"
	.string "of them.$"

TradewindGym_Text_Meditant::
	.string "…Each lantern outside holds a\n"
	.string "name. When one gutters, we\l"
	.string "relight it within the hour.\p"
	.string "We missed one, once.\p"
	.string "We do not talk about the\n"
	.string "hundred and ninth lantern.$"
