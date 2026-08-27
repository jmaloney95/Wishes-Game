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
	.string "What can I get ya, darling?$"

NewMapShop_Text_Farewell::
	.string "Come back down in one piece, eh?$"

NewMapShop_Text_Shopper::
	.string "I'd grab an ESCAPE ROPE\n"
	.string "if I were you.\l"
	.string "It'll save ya if ya find yourself\n"
	.string "caught in a pickle!$"
