MunenVillageAct2_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MunenVillageAct2_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, MunenVillageAct2_OnLoad
	.byte 0

@ Post-game the goons are gone, which opens the north road to Melting Mile.
MunenVillageAct2_OnTransition:
	call Common_EventScript_WotPostGameMutridsGone
	end

@ ============================================================================
@  MUNEN VILLAGE -- ACT 2 (overrun by Team Mutrid)
@  The layout is a clone of the Act 1 village (MunenVillage), so both acts
@  share one geometry. The player arrives by walking out of Clarkson's
@  LABORATORY (lab door 45,21 = warp 0 <-> MAP_MUNEN_LAB_ACT2), after the
@  Draco battle and the research choice; the Gengar ferry only runs to the
@  lab. HOME (16,19 = warp 1 <-> MAP_MUNEN_HOME_ACT2) plays Mom's cutscene
@  and she takes the player on to the Dojo. Every other way out is held by
@  Mutrid: a goon on Draco's doorstep (31,20) and three across the north
@  road (34..36,4); there is no map connection to Melting Mile in this act.
@ ============================================================================

@ The painted item balls and Rock Smash boulders came across with the cloned
@ layout. They share Act 1's flags and scripts, so anything taken or smashed
@ in Act 1 stays gone here and anything left can still be had.
MunenVillageAct2_OnLoad::
	call_if_set FLAG_MUNEN_VILLAGE_BOULDER_1, MunenVillage_EventScript_ClearBoulder1
	call_if_set FLAG_MUNEN_VILLAGE_BOULDER_2, MunenVillage_EventScript_ClearBoulder2
	call_if_set FLAG_MUNEN_VILLAGE_BOULDER_3, MunenVillage_EventScript_ClearBoulder3
	call_if_set FLAG_MUNEN_VILLAGE_BOULDER_4, MunenVillage_EventScript_ClearBoulder4
	call_if_set FLAG_MUNEN_VILLAGE_BOULDER_5, MunenVillage_EventScript_ClearBoulder5
	call_if_set FLAG_ITEM_MUNEN_VILLAGE_TM_DIG, MunenVillage_EventScript_ClearItemTMDig
	call_if_set FLAG_ITEM_MUNEN_VILLAGE_SACRED_ASH, MunenVillage_EventScript_ClearItemSacredAsh
	call_if_set FLAG_ITEM_MUNEN_VILLAGE_LIGHT_BALL, MunenVillage_EventScript_ClearItemLightBall
	end

MunenVillageAct2_EventScript_GoonSquare::
	lock
	faceplayer
	setspeaker SP_NAME_GOON
	goto_if_set FLAG_ONI_MASK_WORN, OniMask_EventScript_GoonMasked
	msgbox MunenVillageAct2_Text_GoonSquare, MSGBOX_DEFAULT
	release
	end

MunenVillageAct2_EventScript_GoonSquareMasked::
	msgbox MunenVillageAct2_Text_GoonSquareMasked, MSGBOX_DEFAULT
	release
	end

MunenVillageAct2_EventScript_GoonPatrol::
	lock
	faceplayer
	setspeaker SP_NAME_GOON
	goto_if_set FLAG_ONI_MASK_WORN, OniMask_EventScript_GoonMasked
	msgbox MunenVillageAct2_Text_GoonPatrol, MSGBOX_DEFAULT
	release
	end

MunenVillageAct2_EventScript_OccupationNotice::
	msgbox MunenVillageAct2_Text_OccupationNotice, MSGBOX_SIGN
	end

@ ============================================================================
@  Text
@ ============================================================================
MunenVillageAct2_Text_GoonSquare::
	.string "Village's under Mutrid management\n"
	.string "now. Doors stay barred, locals\l"
	.string "stay indoors, and YOU...\p"
	.string "...wait. How did you even get in?\n"
	.string "Every road is cut!$"

MunenVillageAct2_Text_GoonSquareMasked::
	.string "The goon glances at the carved\n"
	.string "face at your belt and straightens\l"
	.string "up.\p"
	.string "“Inspection, is it? Everything's\n"
	.string "in order here. Doors barred,\l"
	.string "locals quiet. Honest.”$"

MunenVillageAct2_Text_GoonPatrol::
	.string "Funny little village. Snow on\n"
	.string "every roof, bells on every door.\p"
	.string "The master grew up here, you know.\n"
	.string "You'd think he'd be SENTIMENTAL\l"
	.string "about it.\p"
	.string "...You'd think.$"

MunenVillageAct2_Text_OccupationNotice::
	.string "A notice is nailed over the old\n"
	.string "village sign, ink still wet:\p"
	.string "“BY ORDER OF TEAM MUTRID…\p"
	.string "“This village and all it remembers\n"
	.string "now serve the Wish.\p"
	.string "“Remain indoors. The mountain will\n"
	.string "take what it is owed.”$"
