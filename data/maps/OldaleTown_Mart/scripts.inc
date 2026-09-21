OldaleTown_Mart_MapScripts::
	.byte 0

OldaleTown_Mart_EventScript_Clerk::
	lock
	faceplayer
	goto OldaleTown_Mart_EventScript_Shop

@ The bar's menu card at (5,3). The bartender stands walled in behind the bar
@ with no counter tile to talk across, so the card is how the player shops.
OldaleTown_Mart_EventScript_MenuCard::
	lockall
	turnobject LOCALID_OLDALE_MART_CLERK, DIR_EAST
	goto OldaleTown_Mart_EventScript_Shop

@ The bar sells Poke Balls and pours drinks: DOMESTIC (a Potion) and CRAFT (a Super Potion).
OldaleTown_Mart_EventScript_Shop::
	message gText_HowMayIServeYou
	waitmessage
	pokemart OldaleTown_Mart_Pokemart_Drinks
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
OldaleTown_Mart_Pokemart_Drinks:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_DOMESTIC
	.2byte ITEM_CRAFT
	pokemartlistend

OldaleTown_Mart_EventScript_Woman::
	lock
	faceplayer
	goto_if_set FLAG_ADVENTURE_STARTED, OldaleTown_Mart_EventScript_PokeBallsInStock
	msgbox OldaleTown_Mart_Text_PokeBallsAreSoldOut, MSGBOX_DEFAULT
	release
	end

OldaleTown_Mart_EventScript_PokeBallsInStock::
	msgbox OldaleTown_Mart_Text_ImGoingToBuyPokeBalls, MSGBOX_DEFAULT
	release
	end

OldaleTown_Mart_EventScript_Boy::
	msgbox OldaleTown_Mart_Text_RestoreHPWithPotion, MSGBOX_NPC
	end

OldaleTown_Mart_EventScript_TwigsSkier::
	msgbox OldaleTown_Mart_Text_TwigsSkier, MSGBOX_NPC
	end

OldaleTown_Mart_EventScript_ApresGirl::
	msgbox OldaleTown_Mart_Text_ApresGirl, MSGBOX_NPC
	end

OldaleTown_Mart_EventScript_NervousGuy::
	msgbox OldaleTown_Mart_Text_NervousGuy, MSGBOX_NPC
	end

OldaleTown_Mart_EventScript_BoardOrSki::
	msgbox OldaleTown_Mart_Text_BoardOrSki, MSGBOX_NPC
	end

OldaleTown_Mart_EventScript_Draco::
	lock
	faceplayer
	setspeaker SP_NAME_DRACO
	msgbox OldaleTown_Mart_Text_Draco, MSGBOX_DEFAULT
	release
	end

OldaleTown_Mart_Text_Draco:
	.string "{PLAYER}? Of course you found this\n"
	.string "place.\p"
	.string "Don't get comfortable. I'm just\n"
	.string "thawing out before I take that\l"
	.string "GYM BADGE.\p"
	.string "…And no, I'm not buying you\n"
	.string "a drink.$"

OldaleTown_Mart_Text_TwigsSkier:
	.string "I was riding pipe when I caught an\n"
	.string "edge on some nasty moguls…\p"
	.string "…and landed a switch 540 in pow\n"
	.string "deeper than the Swiss Alps.\p"
	.string "Craziest thing I ever did on a\n"
	.string "pair of twigs.$"

OldaleTown_Mart_Text_ApresGirl:
	.string "I'm just here for the après.$"

OldaleTown_Mart_Text_NervousGuy:
	.string "I'm way too nervous to talk to her…\p"
	.string "I'll just DM her on Instagram.$"

OldaleTown_Mart_Text_BoardOrSki:
	.string "Board or ski?\p"
	.string "There's only one right answer.$"

OldaleTown_Mart_Text_PokeBallsAreSoldOut:
	.string "The clerk says they're all sold out.\n"
	.string "I can't buy any POKé BALLS.$"

OldaleTown_Mart_Text_ImGoingToBuyPokeBalls:
	.string "I'm going to buy a bunch of POKé BALLS\n"
	.string "and catch a bunch of POKéMON!$"

OldaleTown_Mart_Text_RestoreHPWithPotion:
	.string "If a POKéMON gets hurt and loses its HP\n"
	.string "and faints, it won't be able to battle.\p"
	.string "To prevent your POKéMON from fainting,\n"
	.string "restore its HP with a POTION.$"

