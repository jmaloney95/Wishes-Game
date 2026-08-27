Route_2_MapScripts::
	.byte 0

@ -- Route 2 grinding trainers (weak, pre-Gym-1) --

Route_2_EventScript_Fisherman::
	trainerbattle_single TRAINER_ROUTE2_FISHERMAN, Route_2_Text_FishermanIntro, Route_2_Text_FishermanDefeat
	msgbox Route_2_Text_FishermanPost, MSGBOX_AUTOCLOSE
	end

Route_2_EventScript_Hiker::
	trainerbattle_single TRAINER_ROUTE2_HIKER, Route_2_Text_HikerIntro, Route_2_Text_HikerDefeat
	msgbox Route_2_Text_HikerPost, MSGBOX_AUTOCLOSE
	end

Route_2_EventScript_Camper::
	trainerbattle_single TRAINER_ROUTE2_CAMPER, Route_2_Text_CamperIntro, Route_2_Text_CamperDefeat
	msgbox Route_2_Text_CamperPost, MSGBOX_AUTOCLOSE
	end

Route_2_Text_FishermanIntro:
	.string "The fish ain't bitin' today…\n"
	.string "Maybe YOU'LL give me a nibble!$"

Route_2_Text_FishermanDefeat:
	.string "Reeled in and tossed back…$"

Route_2_Text_FishermanPost:
	.string "The big ones are further up the\n"
	.string "SENNEN line. If it ever\l"
	.string "reopens, that is.$"

Route_2_Text_HikerIntro:
	.string "These slopes'll toughen you up\n"
	.string "before you face FROSTWOOD's gym!$"

Route_2_Text_HikerDefeat:
	.string "Whew! You've got strong legs!$"

Route_2_Text_HikerPost:
	.string "Train hard out here. That gym\n"
	.string "leader doesn't go easy on rookies.$"

Route_2_Text_CamperIntro:
	.string "You caught me with a pitched tent.\n"
	.string "Get ready for my finisher!$"

Route_2_Text_CamperDefeat:
	.string "Ahhh I give uuup!$"

Route_2_Text_CamperPost:
	.string "Plenty of wild POKéMON around here\n"
	.string "if you're looking to level up.$"

@ -- Ambient wandering wild POKéMON (cry on talk, no battle) --

Route_2_EventScript_WildZigzagoon::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ZIGZAGOON, 0
	msgbox Route_2_Text_WildZigzagoon, MSGBOX_DEFAULT
	waitmoncry
	release
	end

Route_2_EventScript_WildPoochyena::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_POOCHYENA, 0
	msgbox Route_2_Text_WildPoochyena, MSGBOX_DEFAULT
	waitmoncry
	release
	end

Route_2_EventScript_WildWingull::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_WINGULL, 0
	msgbox Route_2_Text_WildWingull, MSGBOX_DEFAULT
	waitmoncry
	release
	end

Route_2_EventScript_WildPsyduck::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_PSYDUCK, 0
	msgbox Route_2_Text_WildPsyduck, MSGBOX_DEFAULT
	waitmoncry
	release
	end

Route_2_Text_WildZigzagoon:
	.string "A ZIGZAGOON is zig-zagging through\n"
	.string "the underbrush!$"

Route_2_Text_WildPoochyena:
	.string "A POOCHYENA eyes you warily, then\n"
	.string "trots off down the trail.$"

Route_2_Text_WildWingull:
	.string "A WINGULL wheels overhead and lands\n"
	.string "nearby with a cry!$"

Route_2_Text_WildPsyduck:
	.string "Psyyyyy\n"
	.string "duckkkk$"

@ SIREN ALLISON: one-liner through the plate/portrait, boss card, battle.
@ The trainer flag is shared across acts, so the card fires exactly once --
@ at whichever of her encounters the player reaches first.
Route_2_EventScript_SwimmerAllison::
	goto_if_defeated TRAINER_ROUTE2_SWIMMER_ALLISON, Route_2_EventScript_AllisonAfter
	lockall
	setspeaker SP_NAME_ALLISON
	msgbox Route_2_Text_AllisonIntro, MSGBOX_DEFAULT
	closemessage
	bossintro BOSS_INTRO_ALLISON
	trainerbattle_no_intro TRAINER_ROUTE2_SWIMMER_ALLISON, Route_2_Text_AllisonDefeat
	setspeaker SP_NAME_ALLISON
	msgbox Route_2_Text_AllisonPost, MSGBOX_DEFAULT
	releaseall
	end

Route_2_EventScript_AllisonAfter::
	lock
	faceplayer
	setspeaker SP_NAME_ALLISON
	msgbox Route_2_Text_AllisonPost, MSGBOX_DEFAULT
	release
	end

Route_2_EventScript_SwimmerSkye::
	trainerbattle_single TRAINER_ROUTE2_SWIMMER_SKYE, Route_2_Text_SkyeIntro, Route_2_Text_SkyeDefeat
	msgbox Route_2_Text_SkyePost, MSGBOX_AUTOCLOSE
	end

Route_2_EventScript_SwimmerRosa::
	trainerbattle_single TRAINER_ROUTE2_SWIMMER_ROSA, Route_2_Text_RosaIntro, Route_2_Text_RosaDefeat
	msgbox Route_2_Text_RosaPost, MSGBOX_AUTOCLOSE
	end

Route_2_EventScript_SwimmerMarina::
	trainerbattle_single TRAINER_ROUTE2_SWIMMER_MARINA, Route_2_Text_MarinaIntro, Route_2_Text_MarinaDefeat
	msgbox Route_2_Text_MarinaPost, MSGBOX_AUTOCLOSE
	end

Route_2_Text_AllisonIntro:
	.string "Its UV 9…\n"
	.string "and you're blocking my rays.$"

Route_2_Text_AllisonDefeat:
	.string "Swept off my feet… and I'm not\n"
	.string "even standing!$"

Route_2_Text_AllisonPost:
	.string "My LUVDISC took one look at you and\n"
	.string "forgot how to swim. Same, honestly.$"

Route_2_Text_SkyeIntro:
	.string "I was floating here waiting for a\n"
	.string "shooting star… you'll do nicely!$"

Route_2_Text_SkyeDefeat:
	.string "Falling for you was the easy part!$"

Route_2_Text_SkyePost:
	.string "They say the SENNEN LINE runs again.\n"
	.string "Take me somewhere warm sometime?$"

Route_2_Text_RosaIntro:
	.string "A thousand years frozen, and YOU'RE\n"
	.string "what finally melts the ice? Unfair!$"

Route_2_Text_RosaDefeat:
	.string "Be still, my CORSOLA heart!$"

Route_2_Text_RosaPost:
	.string "If you're climbing STAR SUMMIT, wish\n"
	.string "for me, okay? Just a little wish.$"

Route_2_Text_MarinaIntro:
	.string "Careful, cutie. The current's strong\n"
	.string "and so am I!$"

Route_2_Text_MarinaDefeat:
	.string "Oof… you sank my whole strategy!$"

Route_2_Text_MarinaPost:
	.string "Strong AND charming? The Pro-\n"
	.string "fessor's lucky to have you up there.$"

@ ---------------------------------------------------------------------------
@  Beach crowd -- the blizzard is one town south; here it is towel weather.
@ ---------------------------------------------------------------------------
Route_2_EventScript_YachtSign::
	lockall
	msgbox Route_2_Text_YachtSign, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Route_2_EventScript_YachtSignDone
	@ The joke finally pays off: the fisherman's key starts her.
	checkitem ITEM_YACHT_KEY
	goto_if_eq VAR_RESULT, TRUE, Route_2_EventScript_YachtBoard
	msgbox Route_2_Text_YachtFailed, MSGBOX_DEFAULT
Route_2_EventScript_YachtSignDone::
	releaseall
	end

Route_2_EventScript_YachtBoard::
	msgbox Route_2_Text_YachtStarts, MSGBOX_DEFAULT
	closemessage
	playse SE_DOOR
	fadescreen FADE_TO_BLACK
	warp MAP_YACHT_INTERIOR, 6, 7
	waitstate
	releaseall
	end

Route_2_EventScript_TuberSandy::
	trainerbattle_single TRAINER_CINDY_2, Route_2_Text_SandyIntro, Route_2_Text_SandyDefeat
	msgbox Route_2_Text_SandyPost, MSGBOX_AUTOCLOSE
	end

Route_2_EventScript_TuberCoco::
	trainerbattle_single TRAINER_LUCAS_2, Route_2_Text_CocoIntro, Route_2_Text_CocoDefeat
	msgbox Route_2_Text_CocoPost, MSGBOX_AUTOCLOSE
	end

Route_2_EventScript_ExpertPete::
	trainerbattle_single TRAINER_MIKE_1, Route_2_Text_PeteIntro, Route_2_Text_PeteDefeat
	msgbox Route_2_Text_PetePost, MSGBOX_AUTOCLOSE
	end

Route_2_EventScript_BeachBeauty::
	lock
	faceplayer
	msgbox Route_2_Text_BeachBeauty, MSGBOX_DEFAULT
	release
	end

Route_2_EventScript_BeachSailor::
	lock
	faceplayer
	msgbox Route_2_Text_BeachSailor, MSGBOX_DEFAULT
	release
	end

Route_2_EventScript_BeachOldMan::
	lock
	faceplayer
	msgbox Route_2_Text_BeachOldMan, MSGBOX_DEFAULT
	release
	end

Route_2_Text_YachtSign::
	.string "A gleaming yacht bobs at the\n"
	.string "pier, keys in the ignition.\p"
	.string "Hijack the yacht?$"

Route_2_Text_YachtFailed::
	.string "Failed!$"

Route_2_Text_YachtStarts::
	.string "You try the fisherman's key.\p"
	.string "The engine catches on the\n"
	.string "first turn.\p"
	.string "...Nobody comes running.$"

Route_2_Text_SandyIntro::
	.string "I heard that one town south\n"
	.string "it's a whiteout blizzard.$"

Route_2_Text_SandyDefeat::
	.string "Whatever, ya flatfoot.!$"

Route_2_Text_SandyPost::
	.string "Snowman or sandcastle?\n"
	.string "I can't decide!$"

Route_2_Text_CocoIntro::
	.string "Prime beach weather every single\n"
	.string "day! How is that even fair?$"

Route_2_Text_CocoDefeat::
	.string "Wipeout...$"

Route_2_Text_CocoPost::
	.string "They say you can see the storm\n"
	.string "wall from the south fence. Brrr.$"

Route_2_Text_PeteIntro::
	.string "Personally? I prefer the cold.$"

Route_2_Text_PeteDefeat::
	.string "I am sweating. That is new.$"

Route_2_Text_PetePost::
	.string "The moment I save up, I am\n"
	.string "moving to Frostwood for good.$"

Route_2_Text_BeachBeauty::
	.string "Sunscreen on the left arm,\n"
	.string "frostbite cream on the right.$"

Route_2_Text_BeachSailor::
	.string "Get a load of the cheeks on her!\n"
	.string "Arceus, have Mercy!$"

Route_2_Text_BeachOldMan::
	.string "Some say it is climate change,\n"
	.string "but legend says JIRACHI\p"
	.string "will rise again.$"

Route_2_EventScript_TwinsReinaVanessa::
	trainerbattle_double TRAINER_RED, Route_2_Text_TwinsIntro, Route_2_Text_TwinsDefeat, Route_2_Text_TwinsNotEnough
	msgbox Route_2_Text_TwinsPost, MSGBOX_AUTOCLOSE
	end

Route_2_Text_TwinsIntro::
	.string "REINA: We do everything as a\n"
	.string "pair!\p"
	.string "VANESSA: Even our FINIZEN\n"
	.string "match! Double battle time!$"

Route_2_Text_TwinsDefeat::
	.string "Even our synchronized splash\n"
	.string "failed!$"

Route_2_Text_TwinsNotEnough::
	.string "You only have one POKeMON?\n"
	.string "Come back with a pair!$"

Route_2_Text_TwinsPost::
	.string "Matching partners, matching\n"
	.string "losses... Still cute though!$"
