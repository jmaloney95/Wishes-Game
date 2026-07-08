MunenVillageAct2_MapScripts::
	.byte 0

@ ============================================================================
@  MUNEN VILLAGE -- ACT 2 (overrun by Team Mutrid)
@  Reached ONLY via the Gengar ferry (Catalpa Bow, post-Distortion scene).
@  This is the point of no return: house doors are barred by Mutrid, EXCEPT
@  Clarkson's LABORATORY at (38,18) -> MAP_MUNEN_LAB_ACT2 (warp 0), where
@  Draco waits with the stolen research. Arrival point is (9,14).
@  Still to come: arrival cutscene, villager fates, boss encounter.
@ ============================================================================

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
	.string "“BY ORDER OF TEAM MUTRID --\p"
	.string "“This village and all it remembers\n"
	.string "now serve the Wish.\p"
	.string "“Remain indoors. The mountain will\n"
	.string "take what it is owed.”$"
