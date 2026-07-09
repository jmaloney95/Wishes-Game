MunenTunnel_MapScripts::
	.byte 0

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

MunenTunnel_EventScript_ItemCarvedMask::
	finditem ITEM_CARVED_MASK
	end

@ --------------------------------------------------------------------------
@  Dusclops -- the tomb's warden. It does not attack. It only watches.
@ --------------------------------------------------------------------------
MunenTunnel_EventScript_Dusclops::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_DUSCLOPS, CRY_MODE_NORMAL
	msgbox MunenTunnel_Text_Dusclops, MSGBOX_DEFAULT
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
MunenTunnel_Text_Dusclops::
	.string "A Dusclops stands among the graves,\n"
	.string "still as the stone around it.\p"
	.string "Its single eye follows you...\n"
	.string "then drifts back into the dark.\p"
	.string "It seems to be keeping watch over\n"
	.string "something. Or someone.$"

MunenTunnel_Text_GruntIntro::
	.string "Hm? A villager?\p"
	.string "No... doesn't matter. The rite has\n"
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
	.string "A stone basin for offerings lies\n"
	.string "overturned by heavy boots.\p"
	.string "Dried flowers are scattered and\n"
	.string "trampled. None were spared.$"

MunenTunnel_Text_WallInscription::
	.string "Words are cut deep into the rock\n"
	.string "wall, in an old, old hand:\p"
	.string "“HERE SLEEP THE DEAD OF MUNEN.\n"
	.string "MOURN THEM. DO NOT WAKE THEM.\p"
	.string "WHAT YOU REGRET, THE MOUNTAIN\n"
	.string "KEEPS. LET IT KEEP YOURS.”$"

MunenTunnel_Text_CampOrders::
	.string "Bedrolls, cold ash, empty crates...\n"
	.string "a scrap of paper is pinned under\l"
	.string "a rock.\p"
	.string "“Move only at night. The village\n"
	.string "must not count our numbers.\p"
	.string "“Masks stay ON. The Summit Gate\n"
	.string "opens for no bare face.\p"
	.string "“When the bell sounds twice, burn\n"
	.string "this camp and climb.    -- K”$"

MunenTunnel_Text_UnsentLetter::
	.string "A letter, folded with great care.\n"
	.string "It was never sent.\p"
	.string "“Master Gen --\p"
	.string "“I'm sorry about the mask. I know\n"
	.string "it wasn't mine to take.\p"
	.string "“But the man in the horned mask\n"
	.string "says Father is waiting past the\l"
	.string "Summit Gate.\p"
	.string "“He says the mountain keeps what\n"
	.string "we regret, and the master can\l"
	.string "give it back.\p"
	.string "“I have to see him again. Even\n"
	.string "once. Please don't follow me.\p"
	.string "“                    -- Itsuki”$"
