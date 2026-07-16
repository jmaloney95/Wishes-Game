@ Tradewind empty apartment (east door). Empty -- except for the corner you
@ can talk to. Invisible object event (gfx 0 + MOVEMENT_TYPE_INVISIBLE,
@ CeladonCity_Condominiums pattern): a voice with no body.
TradewindApartment_2_MapScripts::
	.byte 0

TradewindApartment_2_EventScript_Presence::
	lock
	playse SE_ICE_CRACK
	msgbox TradewindApartment_2_Text_Presence, MSGBOX_DEFAULT
	release
	end

TradewindApartment_2_Text_Presence::
	.string "The room is empty.\p"
	.string "…\p"
	.string "The air by the corner is colder\n"
	.string "than the rest.\p"
	.string "“You can have this one. I never\n"
	.string "liked the view.”$"
