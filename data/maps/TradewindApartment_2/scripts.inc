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

@ Hidden floor hatch (coord_event at 1,2; VAR_TEMP_0 gate). Arriving from the
@ lab lands ON this tile without firing it -- coord triggers only fire when
@ STEPPING onto the tile.
TradewindApartment_2_EventScript_Hatch::
	lockall
	playse SE_DOOR
	msgbox TradewindApartment_2_Text_Hatch, MSGBOX_DEFAULT
	closemessage
	warp MAP_TRADEWIND_LAB, 16, 2
	waitstate
	releaseall
	end

TradewindApartment_2_Text_Hatch::
	.string "The floorboard gives way…\n"
	.string "A hatch! Stairs lead down.$"
