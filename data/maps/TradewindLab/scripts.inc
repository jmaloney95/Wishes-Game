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

@ Shady dealer -- slips you the three Kanto-starter Mega Stones, once.
TradewindLab_EventScript_ShadyDealer::
	lock
	faceplayer
	goto_if_set FLAG_GOT_TRADEWIND_MEGA_STONES, TradewindLab_EventScript_ShadyDealerDone
	msgbox TradewindLab_Text_ShadyDealer, MSGBOX_DEFAULT
	giveitem ITEM_VENUSAURITE
	giveitem ITEM_CHARIZARDITE_X
	giveitem ITEM_BLASTOISINITE
	setflag FLAG_GOT_TRADEWIND_MEGA_STONES
	msgbox TradewindLab_Text_ShadyDealerAfter, MSGBOX_DEFAULT
	release
	end

TradewindLab_EventScript_ShadyDealerDone::
	msgbox TradewindLab_Text_ShadyDealerDone, MSGBOX_DEFAULT
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

TradewindLab_Text_ShadyDealer::
	.string "Psst. Down here for the readings,\n"
	.string "or for real merchandise?\p"
	.string "Kanto's finest. Fell off a truck\n"
	.string "in another world. No questions.\p"
	.string "Here--take 'em. On the house.\n"
	.string "First one's always free.$"

TradewindLab_Text_ShadyDealerAfter::
	.string "CHARIZARD, BLASTOISE, VENUSAUR.\n"
	.string "Find the rings to wake 'em up.\p"
	.string "You were never here.$"

TradewindLab_Text_ShadyDealerDone::
	.string "That's all the free samples,\n"
	.string "friend. Beat it.$"
