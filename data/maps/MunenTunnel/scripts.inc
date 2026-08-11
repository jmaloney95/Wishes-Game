MunenTunnel_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, MunenTunnel_OnTransition
	.byte 0

@ Whiteouts in the tunnel respawn at the NewMap inn; Escape Rope / Dig
@ surface at the tunnel mouth on NewMap (warp 10).
MunenTunnel_OnTransition::
	setrespawn HEAL_LOCATION_NEW_MAP_INN
	setescapewarp MAP_NEW_MAP, 10, 0, 0
	end

@ ============================================================================
@  MUNEN TUNNEL -- old burial tunnel of the mountain dead, desecrated by the
@  oni group as a staging camp before their climb. Optional lore dungeon.
@  Front half: tomb (graves, Dusclops warden). Left room: abandoned camp.
@  Top room: deepest chamber -- straggler grunt + the apprentice's carved mask.
@ ============================================================================

@ --------------------------------------------------------------------------
@  Item balls
@ --------------------------------------------------------------------------
MunenTunnel_EventScript_ItemNugget::
	finditem ITEM_NUGGET
	end

MunenTunnel_EventScript_ItemMaxEther::
	finditem ITEM_MAX_ETHER
	end

MunenTunnel_EventScript_ItemTMShadowBall::
	finditem ITEM_TM30
	end

MunenTunnel_EventScript_ItemTMThunderbolt::
	finditem ITEM_TM24
	end

@ The mask stays out of reach while Captain McCormick holds the chamber.
MunenTunnel_EventScript_ItemCarvedMask::
	goto_if_unset FLAG_POMPEII_GENERAL_RETREATED, MunenTunnel_EventScript_MaskGuarded
	finditem ITEM_CARVED_MASK
	end

MunenTunnel_EventScript_MaskGuarded::
	msgbox MunenTunnel_Text_MaskGuarded, MSGBOX_SIGN
	end

@ --------------------------------------------------------------------------
@  The tunnel gauntlet -- Master Gen's quest. Two goons hold the halls and
@  CAPTAIN McCORMICK holds the mask chamber. Basic teams, Lv16-20.
@ --------------------------------------------------------------------------
MunenTunnel_EventScript_GoonA::
	trainerbattle_single TRAINER_TUNNEL_GOON_A, MunenTunnel_Text_GoonAIntro, MunenTunnel_Text_GoonADefeat
	msgbox MunenTunnel_Text_GoonAPost, MSGBOX_AUTOCLOSE
	end

MunenTunnel_EventScript_GoonB::
	trainerbattle_single TRAINER_TUNNEL_GOON_B, MunenTunnel_Text_GoonBIntro, MunenTunnel_Text_GoonBDefeat
	msgbox MunenTunnel_Text_GoonBPost, MSGBOX_AUTOCLOSE
	end

MunenTunnel_EventScript_General::
	trainerbattle_single TRAINER_TUNNEL_CAPTAIN, MunenTunnel_Text_GeneralIntro, MunenTunnel_Text_GeneralDefeat, MunenTunnel_EventScript_GeneralRetreat
	end

MunenTunnel_EventScript_GeneralRetreat::
	lockall
	setspeaker SP_NAME_TUNNEL_CAPTAIN
	msgbox MunenTunnel_Text_GeneralRetreat, MSGBOX_DEFAULT
	closemessage
	playse SE_EXIT
	removeobject VAR_LAST_TALKED
	setflag FLAG_POMPEII_GENERAL_RETREATED
	releaseall
	end

@ --------------------------------------------------------------------------
@  Dusclops -- the tomb's warden. It does not attack. It only watches.
@ --------------------------------------------------------------------------
MunenTunnel_EventScript_Houndoom::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_HOUNDOOM, CRY_MODE_NORMAL
	msgbox MunenTunnel_Text_Houndoom, MSGBOX_DEFAULT
	waitmoncry
	release
	end

MunenTunnel_EventScript_Sableye::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_SABLEYE, CRY_MODE_NORMAL
	msgbox MunenTunnel_Text_Sableye, MSGBOX_DEFAULT
	waitmoncry
	release
	end

MunenTunnel_EventScript_Mightyena::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_MIGHTYENA, CRY_MODE_NORMAL
	msgbox MunenTunnel_Text_Mightyena, MSGBOX_DEFAULT
	waitmoncry
	release
	end

MunenTunnel_EventScript_Shuppet::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_SHUPPET, CRY_MODE_NORMAL
	msgbox MunenTunnel_Text_Shuppet, MSGBOX_DEFAULT
	waitmoncry
	release
	end

@ --------------------------------------------------------------------------
@  Straggler grunt -- left behind to guard the deepest chamber.
@  Local ID 5. Flees after losing; hidden by FLAG_HIDE_MUNEN_TUNNEL_GRUNT.
@ --------------------------------------------------------------------------
MunenTunnel_EventScript_Grunt::
	trainerbattle_single TRAINER_MUNEN_TUNNEL_GRUNT, MunenTunnel_Text_GruntIntro, MunenTunnel_Text_GruntDefeat, MunenTunnel_EventScript_GruntFlee
	end

MunenTunnel_EventScript_GruntFlee::
	lockall
	msgbox MunenTunnel_Text_GruntFlee, MSGBOX_DEFAULT
	closemessage
	applymovement 5, MunenTunnel_Movement_GruntFlee
	waitmovement 0
	removeobject 5
	setflag FLAG_HIDE_MUNEN_TUNNEL_GRUNT
	releaseall
	end

MunenTunnel_Movement_GruntFlee:
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_down
	walk_fast_down
	step_end

@ --------------------------------------------------------------------------
@  Tomb reading material
@ --------------------------------------------------------------------------
MunenTunnel_EventScript_GraveDefaced::
	msgbox MunenTunnel_Text_GraveDefaced, MSGBOX_SIGN
	end

MunenTunnel_EventScript_GraveOfferings::
	msgbox MunenTunnel_Text_GraveOfferings, MSGBOX_SIGN
	end

MunenTunnel_EventScript_WallInscription::
	msgbox MunenTunnel_Text_WallInscription, MSGBOX_SIGN
	end

MunenTunnel_EventScript_CampOrders::
	msgbox MunenTunnel_Text_CampOrders, MSGBOX_SIGN
	end

MunenTunnel_EventScript_UnsentLetter::
	msgbox MunenTunnel_Text_UnsentLetter, MSGBOX_SIGN
	end

@ ============================================================================
@  Text
@ ============================================================================
MunenTunnel_Text_Houndoom::
	.string "A HOUNDOOM prowls between the\n"
	.string "graves, hackles raised.\p"
	.string "Its eyes catch the torchlight--\n"
	.string "it is watching you back.$"

MunenTunnel_Text_MaskGuarded::
	.string "A carved mask lies in the dust,\p"
	.string "but the CAPTAIN's boot is planted\n"
	.string "an inch from it, and his eyes are\l"
	.string "planted on you.$"

MunenTunnel_Text_GoonAIntro::
	.string "Oi! The tomb's closed,\n"
	.string "Turn around.\p"
	.string "...You're not turning around.$"

MunenTunnel_Text_GoonADefeat::
	.string "The CAPTAIN's gonna have my mask\n"
	.string "for this...$"

MunenTunnel_Text_GoonAPost::
	.string "Go on then. The deeper caverns are\n"
	.string "the CAPTAIN's problem, not mine.$"

MunenTunnel_Text_GoonBIntro::
	.string "You beat the gate guard? Ha.\p"
	.string "He was the WEAK one.$"

MunenTunnel_Text_GoonBDefeat::
	.string "Fine, fine! So was I!$"

MunenTunnel_Text_GoonBPost::
	.string "The CAPTAIN doesn't lose. He'll\n"
	.string "bury you with the rest of the\l"
	.string "tomb's residents.$"

MunenTunnel_Text_GeneralIntro::
	.string "Hold.\p"
	.string "CAPTAIN McCORMICK. I hold this tomb\n"
	.string "until the WISH has been claimed.\p"
	.string "Two of mine, beaten by a child\n"
	.string "playing archaeologist.\p"
	.string "The apprentice's little mask?\n"
	.string "It stays in the dirt where he\l"
	.string "threw it -- and so will you.$"

MunenTunnel_Text_GeneralDefeat::
	.string "...Company -- withdraw.$"

MunenTunnel_Text_GeneralRetreat::
	.string "A tomb is a poor hill to die on.\p"
	.string "Take the dead man's toy, then.\n"
	.string "The mountain above is already\l"
	.string "ours. And the Leader does not\l"
	.string "withdraw.$"

MunenTunnel_Text_GruntIntro::
	.string "Hm? A villager?\p"
	.string "No... doesn't matter. The ritual has\n"
	.string "begun. Everyone else has climbed.\p"
	.string "My orders are simple -- NOBODY\n"
	.string "disturbs what we left down here!$"

MunenTunnel_Text_GruntDefeat::
	.string "Agh! I wasn't left here because\n"
	.string "I was the STRONG one...$"

MunenTunnel_Text_GruntFlee::
	.string "You don't get it! The Summit Gate\n"
	.string "only opens for the masked!\p"
	.string "The mountain keeps what people\n"
	.string "regret -- and the master's going\l"
	.string "to give it all BACK!\p"
	.string "Laugh if you want! You'll see!\n"
	.string "Up top... everyone will see!$"

MunenTunnel_Text_GraveDefaced::
	.string "An old grave marker. The names\n"
	.string "were worn away generations ago.\p"
	.string "Fresh chisel marks scar the stone.\n"
	.string "Someone carved a horned face here,\l"
	.string "and they did it recently.$"

MunenTunnel_Text_GraveOfferings::
	.string "You can just yeet off these cliffs,\n"
	.string "don't bother backtracking.$"

MunenTunnel_Text_WallInscription::
	.string "OPERATION: WISH\p"
	.string "PHASE 1: Obtain JIRACHI, the\n"
	.string "DYING WISH.\p"
	.string "PHASE 2: \p"
	.string "(REDACTED)$"
MunenTunnel_Text_CampOrders::
	.string "Bedrolls, cold ash, empty crates...\n"
	.string "a scrap of paper is pinned under\l"
	.string "a rock.\p"
	.string "Move only at night. The village\n"
	.string "must not count our numbers.\p"
	.string "Masks stay ON. The Summit Gate\n"
	.string "opens for no bare face.\p"
	.string "When the bell sounds twice, burn\n"
	.string "this camp and climb.    -- CPT. M$"

MunenTunnel_Text_UnsentLetter::
	.string "A letter, folded with great care.\n"
	.string "It was never sent.\p"
	.string "“Master Gen --\p"
	.string "“I'm sorry about the mask. I know\n"
	.string "it wasn't mine to take.\p"
	.string "“But the man with the face of an Oni\n"
	.string "says Father is waiting past the\l"
	.string "Summit Gate.\p"
	.string "“He says The WISH \n"
	.string "we regret, and the master can\l"
	.string "give it back.\p"
	.string "“I have to see him again. Even\n"
	.string "once. Please don't follow me.\p"
	.string "“                    -- Ryoko”$"

MunenTunnel_Text_Sableye::
	.string "A SABLEYE picks at the tomb\n"
	.string "wall, gnawing loose gems.\p"
	.string "Its jeweled eyes flash once...\n"
	.string "and it grins.$"

MunenTunnel_Text_Mightyena::
	.string "A MIGHTYENA circles the dark\n"
	.string "with its nose low.\p"
	.string "A growl rolls out of it, soft\n"
	.string "and patient.$"

MunenTunnel_Text_Shuppet::
	.string "A SHUPPET drifts along the\n"
	.string "ceiling, trailing cold air.\p"
	.string "It turns to face you without\n"
	.string "turning at all.$"
