Route2LifeguardShack_MapScripts::
	.byte 0

Route2LifeguardShack_EventScript_Lifeguard::
	lock
	faceplayer
	goto_if_set FLAG_ROUTE2_GOT_SOFT_SAND, Route2LifeguardShack_EventScript_LifeguardAgain
	msgbox Route2LifeguardShack_Text_Lifeguard, MSGBOX_DEFAULT
	giveitem ITEM_SOFT_SAND
	setflag FLAG_ROUTE2_GOT_SOFT_SAND
	release
	end

Route2LifeguardShack_EventScript_LifeguardAgain::
	msgbox Route2LifeguardShack_Text_LifeguardAgain, MSGBOX_DEFAULT
	release
	end

Route2LifeguardShack_Text_Lifeguard::
	.string "Hey! You're not supposed to be\n"
	.string "in here.\p"
	.string "...Or maybe I'm not supposed\n"
	.string "to be here.\p"
	.string "Now I'm confused.\p"
	.string "Here you go!$"

Route2LifeguardShack_Text_LifeguardAgain::
	.string "I'm still not sure which of us\n"
	.string "is supposed to be here.$"
