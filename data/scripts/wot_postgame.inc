@ ============================================================================
@  WoT post-game: once the credits have rolled, the occupation is over. Every
@  Mutrid object outside Shin Tokyo carries FLAG_WOT_HIDE_POSTGAME_MUTRIDS and
@  is gone. Shin Tokyo keeps its three hold-out patrols (ShinTokyo_OnTransition
@  _PostGame), and the Mega Dimension keeps its goons: that is where the rest
@  of Team Mutrid fled after the Oni fell.
@
@  Call this from the ON_TRANSITION of each map with such objects. It runs on
@  every visit, so saves that cleared the game before this existed are covered
@  too. Never give this flag to an object a script removes with removeobject:
@  that would set it mid-game and empty every map at once.
@ ============================================================================
Common_EventScript_WotPostGameMutridsGone::
	goto_if_unset FLAG_SYS_GAME_CLEAR, Common_EventScript_WotPostGameMutridsGoneEnd
	setflag FLAG_WOT_HIDE_POSTGAME_MUTRIDS
Common_EventScript_WotPostGameMutridsGoneEnd::
	return
