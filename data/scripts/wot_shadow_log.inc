@ ============================================================================
@  WoT: the SHADOW LOG (START -> LOG). A read-only ledger of every Shadow
@  Pokemon obtainable in the game -- snagged and purified state per species.
@  The whole UI lives in C (WotShowShadowLog, src/wot_shadow_log.c); this
@  wrapper just parks the script context on it, same shape as the HMs menu.
@ ============================================================================
WotShadowLog_EventScript_Open::
	lockall
	special WotShowShadowLog
	waitstate
	releaseall
	end
