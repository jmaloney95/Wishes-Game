LilycoveCity_DepartmentStore_4F_MapScripts::
	.byte 0

LilycoveCity_DepartmentStore_4F_EventScript_Gentleman::
	msgbox LilycoveCity_DepartmentStore_4F_Text_AttackOrDefenseTM, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_Woman::
	msgbox LilycoveCity_DepartmentStore_4F_Text_FiftyDifferentTMs, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_Youngster::
	msgbox LilycoveCity_DepartmentStore_4F_Text_PokemonOnlyHaveFourMoves, MSGBOX_NPC
	end

LilycoveCity_DepartmentStore_4F_EventScript_ClerkLeft::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_4F_Pokemart_AttackTMs
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_4F_Pokemart_AttackTMs:
	.2byte ITEM_TM_FIRE_BLAST
	.2byte ITEM_TM_THUNDER
	.2byte ITEM_TM_BLIZZARD
	.2byte ITEM_TM_HYPER_BEAM
	.2byte ITEM_AM_GUTS
	.2byte ITEM_AM_TECHNICIAN
	.2byte ITEM_AM_MOXIE
	.2byte ITEM_AM_ADAPTABILITY
	.2byte ITEM_AM_SHEER_FORCE
	.2byte ITEM_AM_TOUGH_CLAWS
	.2byte ITEM_AM_IRON_FIST
	.2byte ITEM_AM_TINTED_LENS
	.2byte ITEM_AM_SKILL_LINK
	.2byte ITEM_AM_PRANKSTER
	.2byte ITEM_AM_SPEED_BOOST
	pokemartlistend

LilycoveCity_DepartmentStore_4F_EventScript_ClerkRight::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart LilycoveCity_DepartmentStore_4F_Pokemart_DefenseTMs
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
LilycoveCity_DepartmentStore_4F_Pokemart_DefenseTMs:
	.2byte ITEM_TM_PROTECT
	.2byte ITEM_TM_SAFEGUARD
	.2byte ITEM_TM_REFLECT
	.2byte ITEM_TM_LIGHT_SCREEN
	.2byte ITEM_AM_INTIMIDATE
	.2byte ITEM_AM_THICK_FAT
	.2byte ITEM_AM_STURDY
	.2byte ITEM_AM_LEVITATE
	.2byte ITEM_AM_UNAWARE
	.2byte ITEM_AM_SERENE_GRACE
	.2byte ITEM_AM_POISON_HEAL
	.2byte ITEM_AM_MULTISCALE
	.2byte ITEM_AM_MAGIC_GUARD
	.2byte ITEM_AM_REGENERATOR
	.2byte ITEM_AM_MAGIC_BOUNCE
	pokemartlistend

LilycoveCity_DepartmentStore_4F_Text_AttackOrDefenseTM:
	.string "Hmm…\p"
	.string "An attacking move…\n"
	.string "Or a defensive move…\p"
	.string "It's no easy matter to decide which TM\n"
	.string "moves should be taught to POKéMON…$"

LilycoveCity_DepartmentStore_4F_Text_FiftyDifferentTMs:
	.string "There are so many different kinds of\n"
	.string "TM moves.\p"
	.string "A catalog I read said there are fifty\n"
	.string "different kinds.$"

LilycoveCity_DepartmentStore_4F_Text_PokemonOnlyHaveFourMoves:
	.string "I'd like to get all the different TMs,\n"
	.string "but a POKéMON learns only four moves.$"

@ Wishes of Tomorrow: a shopper explains the new ABILITY MACHINE discs.
LilycoveCity_DepartmentStore_4F_EventScript_AMExplainer::
	lock
	faceplayer
	msgbox LilycoveCity_DepartmentStore_4F_Text_AMExplainer, MSGBOX_DEFAULT
	release
	end

LilycoveCity_DepartmentStore_4F_Text_AMExplainer:
	.string "See those AM discs? ABILITY\n"
	.string "MACHINES, the clerks call them.\p"
	.string "Use one on a POKéMON and its\n"
	.string "Ability is rewritten to whatever\l"
	.string "the disc holds.\p"
	.string "For keeps, mind you. No swapping\n"
	.string "back to what it was born with.\p"
	.string "Choose the mon to match, and\n"
	.string "it's like a whole new POKéMON.$"

