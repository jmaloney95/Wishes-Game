@ Tradewind secret basement laboratory -- beneath the "empty" east apartment.
@ Reached via the hidden floor hatch at (1,2) of TradewindApartment_2; the
@ stairs at (17,1) lead back up.
TradewindLab_MapScripts::
	.byte 0

TradewindLab_EventScript_Scientist::
	lock
	faceplayer
	msgbox TradewindLab_Text_Scientist, MSGBOX_DEFAULT
	release
	end

TradewindLab_Text_Scientist::
	.string "…How did you find the hatch?\p"
	.string "No matter. You're here, and the\n"
	.string "instruments don't care who reads\l"
	.string "them.\p"
	.string "I measure the veil. Every year\n"
	.string "the readings up in the market get\l"
	.string "a little thinner.\p"
	.string "The town above us isn't becoming\n"
	.string "haunted, friend.\p"
	.string "It's becoming the other side.$"
