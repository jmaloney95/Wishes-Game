@ ============================================================================
@  Mystery Codes: each Mega card carries a code, and this NPC turns it into
@  the card's Pokemon, once per save (src/wot_mystery_code.c). The same NPC
@  stands in Tradewind Town's Pokemon Center and in Shin Tokyo's.
@ ============================================================================
WotMysteryCode_EventScript_Npc::
	lock
	faceplayer
	goto_if_unset FLAG_SYS_POKEMON_GET, WotMysteryCode_EventScript_NoPokemon
	msgbox WotMysteryCode_Text_HaveCode, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, WotMysteryCode_EventScript_Bye
	msgbox WotMysteryCode_Text_EnterIt, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	special WotMysteryCode_OpenEntry
	lock
	faceplayer
	special WotMysteryCode_Check
	switch VAR_RESULT
	case WOT_MYSTERY_CODE_EMPTY, WotMysteryCode_EventScript_Bye
	case WOT_MYSTERY_CODE_NO_MATCH, WotMysteryCode_EventScript_NoMatch
	case WOT_MYSTERY_CODE_USED, WotMysteryCode_EventScript_Used
	@ the pulled Mega in the portrait box (with the shiny sparkle for a shiny card)
	showmonpic VAR_0x8004, 10, 3, VAR_0x8007
	call_if_eq VAR_0x8006, FALSE, WotMysteryCode_EventScript_NicePull
	call_if_eq VAR_0x8006, TRUE, WotMysteryCode_EventScript_EpicPull
	special WotMysteryCode_Give
	goto_if_eq VAR_RESULT, MON_CANT_GIVE, WotMysteryCode_EventScript_NoRoom
	playfanfare MUS_OBTAIN_ITEM
	message WotMysteryCode_Text_Received
	waitmessage
	waitfanfare
	hidemonpic
	goto_if_eq VAR_RESULT, MON_GIVEN_TO_PC, WotMysteryCode_EventScript_SentToPC
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, WotMysteryCode_EventScript_Done
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	lock
	faceplayer
	goto WotMysteryCode_EventScript_Done

WotMysteryCode_EventScript_SentToPC::
	msgbox gText_NicknameThisPokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, WotMysteryCode_EventScript_TransferredToPC
	call Common_EventScript_NameReceivedBoxMon
WotMysteryCode_EventScript_TransferredToPC::
	call Common_EventScript_TransferredToPC
WotMysteryCode_EventScript_Done::
	msgbox WotMysteryCode_Text_TakeCare, MSGBOX_DEFAULT
	release
	end

@ The card's rarity on the site: rare and secret cards are an epic pull.
WotMysteryCode_EventScript_NicePull::
	msgbox WotMysteryCode_Text_NicePull, MSGBOX_DEFAULT
	return

WotMysteryCode_EventScript_EpicPull::
	msgbox WotMysteryCode_Text_EpicPull, MSGBOX_DEFAULT
	return

WotMysteryCode_EventScript_NoMatch::
	msgbox WotMysteryCode_Text_NoMatch, MSGBOX_DEFAULT
	release
	end

WotMysteryCode_EventScript_Used::
	msgbox WotMysteryCode_Text_Used, MSGBOX_DEFAULT
	release
	end

@ Party and boxes full: nothing is given and the code stays unused.
WotMysteryCode_EventScript_NoRoom::
	hidemonpic
	msgbox WotMysteryCode_Text_NoRoom, MSGBOX_DEFAULT
	release
	end

WotMysteryCode_EventScript_NoPokemon::
	msgbox WotMysteryCode_Text_NoPokemon, MSGBOX_DEFAULT
	release
	end

WotMysteryCode_EventScript_Bye::
	msgbox WotMysteryCode_Text_Bye, MSGBOX_DEFAULT
	release
	end

WotMysteryCode_Text_HaveCode:
	.string "Hi there! Do you have a\n"
	.string "Mystery Code?$"

WotMysteryCode_Text_EnterIt:
	.string "Please enter your code.$"

WotMysteryCode_Text_NicePull:
	.string "Nice pull!$"

WotMysteryCode_Text_EpicPull:
	.string "Epic pull!$"

WotMysteryCode_Text_Received:
	.string "{PLAYER} received {STR_VAR_1}!$"

WotMysteryCode_Text_TakeCare:
	.string "Take good care of it!$"

WotMysteryCode_Text_NoMatch:
	.string "Sorry, that code doesn't match\n"
	.string "any gift.\p"
	.string "Please check it and try again.$"

WotMysteryCode_Text_Used:
	.string "You've already received\n"
	.string "{STR_VAR_1} with that code.$"

WotMysteryCode_Text_NoRoom:
	.string "There's no room for it in your\n"
	.string "party or your boxes.\p"
	.string "Make some room and come back.\n"
	.string "Your code will still work.$"

WotMysteryCode_Text_NoPokemon:
	.string "Hi there! Come back once you\n"
	.string "have a Pokémon of your own.$"

WotMysteryCode_Text_Bye:
	.string "All right. Come back anytime!$"
