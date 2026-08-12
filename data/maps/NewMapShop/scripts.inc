NewMapShop_MapScripts::
	.byte 0

@ ============================================================================
@  CLIMBING SUPPLY SHOP -- last stock-up before Mt. Munen.
@ ============================================================================
NewMapShop_EventScript_Clerk::
	lock
	faceplayer
	message NewMapShop_Text_Greeting
	waitmessage
	pokemart NewMapShop_Pokemart
	msgbox NewMapShop_Text_Farewell, MSGBOX_DEFAULT
	release
	end

	.align 2
NewMapShop_Pokemart:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_AWAKENING
	.2byte ITEM_BURN_HEAL
	.2byte ITEM_REPEL
	.2byte ITEM_SUPER_REPEL
	.2byte ITEM_ESCAPE_ROPE
	pokemartlistend

NewMapShop_EventScript_Shopper::
	lock
	faceplayer
	msgbox NewMapShop_Text_Shopper, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  Text
@ ============================================================================
NewMapShop_Text_Greeting::
	.string "Welcome!\p"
	.string "Heading up? Then buy double.\n"
	.string "STAR SUMMIT doesn't do refunds.$"

NewMapShop_Text_Farewell::
	.string "Mind the cold past the torii.\n"
	.string "Come back down in one piece, eh?$"

NewMapShop_Text_Shopper::
	.string "I was here when he opened up shop\n"
	.string "this morning. The shelves were\l"
	.string "already half bare!\p"
	.string "He says the masked ones came in the\n"
	.string "night and bought out every coil of\l"
	.string "rope and drop of lantern oil.\p"
	.string "Paid in old coins, he says. COLD\n"
	.string "ones. Straight out of the ground.$"
