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

Route_2_EventScript_SwimmerAllison::
	trainerbattle_single TRAINER_ROUTE2_SWIMMER_ALLISON, Route_2_Text_AllisonIntro, Route_2_Text_AllisonDefeat
	msgbox Route_2_Text_AllisonPost, MSGBOX_AUTOCLOSE
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
	.string "The thaw made the water just right…\n"
	.string "and you made the view even better!$"

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
