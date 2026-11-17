@ Tradewind house -- the widow who stays for the market.
TradewindHouse_MapScripts::
	.byte 0

TradewindHouse_EventScript_Widow::
	lock
	faceplayer
	msgbox TradewindHouse_Text_WidowOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, TradewindHouse_EventScript_WidowStory
	msgbox TradewindHouse_Text_WidowWhich, MSGBOX_DEFAULT
	closemessage
	special ChoosePartyMon
	goto_if_ge VAR_0x8004, PARTY_SIZE, TradewindHouse_EventScript_WidowStory
	@ How every one of them grows: src/wot_evo_advice.c.
	special WotEvoAdvice_Prepare
	goto_if_eq VAR_RESULT, 0xFF, TradewindHouse_EventScript_WidowEgg
	goto_if_eq VAR_RESULT, 0, TradewindHouse_EventScript_WidowFinal
	copyvar VAR_0x8006, VAR_RESULT
	setvar VAR_0x8005, 0
TradewindHouse_EventScript_WidowAdvice::
	special WotEvoAdvice_Get
	msgbox gStringVar4, MSGBOX_DEFAULT
	addvar VAR_0x8005, 1
	goto_if_lt VAR_0x8005, VAR_0x8006, TradewindHouse_EventScript_WidowAdvice
	msgbox TradewindHouse_Text_WidowDone, MSGBOX_DEFAULT
	release
	end

TradewindHouse_EventScript_WidowFinal::
	msgbox TradewindHouse_Text_WidowFinal, MSGBOX_DEFAULT
	release
	end

TradewindHouse_EventScript_WidowEgg::
	msgbox TradewindHouse_Text_WidowEgg, MSGBOX_DEFAULT
	release
	end

TradewindHouse_EventScript_WidowStory::
	msgbox TradewindHouse_Text_Widow, MSGBOX_DEFAULT
	release
	end

TradewindHouse_Text_WidowOffer:
	.string "My husband could look at any\n"
	.string "Pokémon and tell you how it grows.\p"
	.string "Some of it rubbed off on me.\n"
	.string "Shall I take a look at one, dear?$"

TradewindHouse_Text_WidowWhich:
	.string "Which one?$"

TradewindHouse_Text_WidowDone:
	.string "That's all there is to it.\n"
	.string "He'd have liked you, dear.$"

TradewindHouse_Text_WidowFinal:
	.string "{STR_VAR_1}? Oh, it won't change\n"
	.string "any further, dear.\p"
	.string "It's as grown as it'll ever be.$"

TradewindHouse_Text_WidowEgg:
	.string "An Egg! Let it hatch first,\n"
	.string "then bring it back to me.$"

TradewindHouse_Text_Widow::
	.string "Everyone asks why I stay in a\n"
	.string "town like this.\p"
	.string "My husband runs a stall in the\n"
	.string "night market. Has for years.\p"
	.string "…He passed away twelve years\n"
	.string "ago, dear.\p"
	.string "His prices haven't changed once.$"
