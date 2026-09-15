OldaleTown_PokemonCenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, OldaleTown_PokemonCenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

OldaleTown_PokemonCenter_1F_OnTransition:
	setrespawn HEAL_LOCATION_OLDALE_TOWN
	call Common_EventScript_UpdateBrineyLocation
	end

@ WoT: this is Frostwood Town's POKéMON CENTER. The nurse stands behind the
@ counter at (8,5); the counter-front metatile (FrostwoodPC 0x2AE) is MB_COUNTER
@ so talking to the counter from (8,7)/(9,7) reaches her. The PC at (11,3) is a
@ bg_event on EventScript_PC.
OldaleTown_PokemonCenter_1F_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_OLDALE_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

OldaleTown_PokemonCenter_1F_EventScript_GoonMuk::
	msgbox OldaleTown_PokemonCenter_1F_Text_GoonMuk, MSGBOX_NPC
	end

OldaleTown_PokemonCenter_1F_EventScript_GoonBud::
	msgbox OldaleTown_PokemonCenter_1F_Text_GoonBud, MSGBOX_NPC
	end

OldaleTown_PokemonCenter_1F_EventScript_Boy::
	msgbox OldaleTown_PokemonCenter_1F_Text_GondolaToSennen, MSGBOX_NPC
	end

OldaleTown_PokemonCenter_1F_EventScript_Girl::
	msgbox OldaleTown_PokemonCenter_1F_Text_SickOfTheSnow, MSGBOX_NPC
	end

OldaleTown_PokemonCenter_1F_Text_GoonMuk:
	.string "GTFO my face.\n"
	.string "You smell like MUK.$"

OldaleTown_PokemonCenter_1F_Text_GoonBud:
	.string "Don't you have anything better\n"
	.string "to do, bud?$"

OldaleTown_PokemonCenter_1F_Text_GondolaToSennen:
	.string "If you ride the GONDOLA down the\n"
	.string "mountain, you'll end up in SENNEN.\p"
	.string "It's way warmer down there on\n"
	.string "the beachfront!$"

OldaleTown_PokemonCenter_1F_Text_SickOfTheSnow:
	.string "Sick of the snow? The GONDOLA\n"
	.string "station is up in the northeast\l"
	.string "corner of town.\p"
	.string "Ride it down to SENNEN and you'll\n"
	.string "be on the beach in no time!$"
