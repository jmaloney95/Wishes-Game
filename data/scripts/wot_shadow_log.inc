@ ============================================================================
@  WoT: the SHADOW LOG (START -> LOG). A read-only ledger of every Shadow
@  Pokemon obtainable in the game -- snagged and purified state per species.
@  The whole UI lives in C (WotShowShadowLog, src/wot_shadow_log.c); this
@  wrapper just parks the script context on it, same shape as the HMs menu.
@ ============================================================================
WotShadowLog_EventScript_Open::
	lockall
	@ LOG holds the Mega Log too, once the rifts have opened. With
	@ only one of the two unlocked, it goes straight there.
	goto_if_unset FLAG_WOT_MEGA_DOOMSDAY_INTRO, WotShadowLog_EventScript_OpenShadow
	goto_if_unset FLAG_WOT_SHADOW_LOG_GET, WotShadowLog_EventScript_OpenMega
	dynmultichoice 0, 0, FALSE, 2, 0, DYN_MULTICHOICE_CB_NONE, WotShadowLog_Text_ShadowLog, WotShadowLog_Text_MegaLog
	goto_if_eq VAR_RESULT, 0, WotShadowLog_EventScript_OpenShadow
	goto_if_eq VAR_RESULT, 1, WotShadowLog_EventScript_OpenMega
	releaseall
	end

WotShadowLog_EventScript_OpenShadow:
	special WotShowShadowLog
	waitstate
	releaseall
	end

WotShadowLog_EventScript_OpenMega:
	fadescreen FADE_TO_BLACK
	special WotMegaLog_Open
	releaseall
	end

WotShadowLog_Text_ShadowLog:
	.string "SHADOW LOG$"

WotShadowLog_Text_MegaLog:
	.string "MEGA LOG$"
