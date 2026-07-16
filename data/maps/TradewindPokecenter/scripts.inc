@ Tradewind Town Pokemon Center -- the one warm room in the veil-market.
@ Nurse pattern per OldaleTown_PokemonCenter_1F. The escalator tile has no
@ warp event on purpose (no 2F); it is inert without one.
TradewindPokecenter_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, TradewindPokecenter_OnTransition
	.byte 0

TradewindPokecenter_OnTransition::
	setrespawn HEAL_LOCATION_TRADEWIND_TOWN
	end

TradewindPokecenter_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_TRADEWIND_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end
