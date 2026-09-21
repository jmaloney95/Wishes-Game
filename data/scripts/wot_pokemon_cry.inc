@ ============================================================================
@  WoT: talking to an overworld Pokemon plays its cry -- no commentary.
@  Point an object's script here, or leave it NULL: GetInteractedObjectEventScript
@  falls back to this for any Pokemon object without a script of its own.
@ ============================================================================
Common_EventScript_WotPokemonCry::
	lock
	faceplayer
	special WotBufferLastTalkedSpecies
	goto_if_eq VAR_0x8004, SPECIES_NONE, Common_EventScript_WotPokemonCryDone
	playmoncry VAR_0x8004, CRY_MODE_NORMAL
	waitmoncry
Common_EventScript_WotPokemonCryDone::
	release
	end
