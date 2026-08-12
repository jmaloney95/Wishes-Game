@ Tradewind house -- the widow who stays for the market.
TradewindHouse_MapScripts::
	.byte 0

TradewindHouse_EventScript_Widow::
	lock
	faceplayer
	msgbox TradewindHouse_Text_Widow, MSGBOX_DEFAULT
	release
	end

TradewindHouse_Text_Widow::
	.string "Everyone asks why I stay in a\n"
	.string "town like this.\p"
	.string "My husband runs a stall in the\n"
	.string "night market. Has for years.\p"
	.string "…He passed away twelve years\n"
	.string "ago, dear.\p"
	.string "His prices haven't changed once.$"
