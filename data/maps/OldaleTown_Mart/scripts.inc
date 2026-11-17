OldaleTown_Mart_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, OldaleTown_Mart_OnResume
	.byte 0

@ OnResume (not OnLoad): OnLoad runs before ResumeMap's ResetSpriteData,
@ which would wipe the indicator sprite before it renders.
@ The snowboarder has the "!" until Break Time's Over starts. Saves that
@ already had the pass (the old Sennen Village item ball) never start it,
@ so they skip the marker.
OldaleTown_Mart_OnResume::
	goto_if_set FLAG_GOT_EPIC_PASS, OldaleTown_Mart_OnResume_Done
	goto_if_set FLAG_REDFATALITY_HAS_EPIC_PASS, OldaleTown_Mart_OnResume_Done
	questindicator LOCALID_FROSTWOOD_BAR_SNOWBOARDER, QUEST_BREAK_TIMES_OVER
OldaleTown_Mart_OnResume_Done:
	end

OldaleTown_Mart_EventScript_Clerk::
	lock
	faceplayer
	goto OldaleTown_Mart_EventScript_Shop

@ The bar's menu card at (5,3). The bartender stands walled in behind the bar
@ with no counter tile to talk across, so the card is how the player shops.
OldaleTown_Mart_EventScript_MenuCard::
	lockall
	turnobject LOCALID_OLDALE_MART_CLERK, DIR_EAST
	goto OldaleTown_Mart_EventScript_Shop

@ The bar sells Poke Balls and pours drinks: DOMESTIC (a Potion) and CRAFT (a Super Potion).
OldaleTown_Mart_EventScript_Shop::
	message gText_HowMayIServeYou
	waitmessage
	pokemart OldaleTown_Mart_Pokemart_Drinks
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end

	.align 2
OldaleTown_Mart_Pokemart_Drinks:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_DOMESTIC
	.2byte ITEM_CRAFT
	pokemartlistend

@ The jukebox at (6,7) shuffles to a random song (src/wot_jukebox.c). It keeps
@ playing until the player leaves the bar.
OldaleTown_Mart_EventScript_Jukebox::
	lockall
	special WotJukebox_PlayRandomSong
	msgbox OldaleTown_Mart_Text_Jukebox, MSGBOX_DEFAULT
	releaseall
	end

OldaleTown_Mart_Text_Jukebox:
	.string "Shuffle!\n"
	.string "The jukebox spins up a new record.$"

OldaleTown_Mart_EventScript_Woman::
	lock
	faceplayer
	goto_if_set FLAG_ADVENTURE_STARTED, OldaleTown_Mart_EventScript_PokeBallsInStock
	msgbox OldaleTown_Mart_Text_PokeBallsAreSoldOut, MSGBOX_DEFAULT
	release
	end

OldaleTown_Mart_EventScript_PokeBallsInStock::
	msgbox OldaleTown_Mart_Text_ImGoingToBuyPokeBalls, MSGBOX_DEFAULT
	release
	end

OldaleTown_Mart_EventScript_Boy::
	msgbox OldaleTown_Mart_Text_RestoreHPWithPotion, MSGBOX_NPC
	end

@ SIERRA the skier. One easy battle; her old story is the line after.
OldaleTown_Mart_EventScript_TwigsSkier::
	trainerbattle_single TRAINER_FROSTWOOD_BAR_SKIER, OldaleTown_Mart_Text_SkierIntro, OldaleTown_Mart_Text_SkierDefeated
	msgbox OldaleTown_Mart_Text_TwigsSkier, MSGBOX_AUTOCLOSE
	end

OldaleTown_Mart_EventScript_ApresGirl::
	msgbox OldaleTown_Mart_Text_ApresGirl, MSGBOX_NPC
	end

@ ---------------------------------------------------------------------------
@ VICTOR wants a BERGMITE (Walnut Woods) to grow into an AVALUGG and impress
@ the girl at the bar. He gives his MUDKIP, at the BERGMITE's level: Water
@ resists the Frostwood Gym's Ice and Fire, and Rock Throw (Lv 9) hits all
@ four of RedFatality's team. The trade is once only;
@ TRAINER_FROSTWOOD_VICTOR_TRADED's defeated flag records it.
OldaleTown_Mart_EventScript_NervousGuy::
	lock
	faceplayer
	goto_if_defeated TRAINER_FROSTWOOD_VICTOR_TRADED, OldaleTown_Mart_EventScript_VictorAfterTrade
	msgbox OldaleTown_Mart_Text_NervousGuy, MSGBOX_DEFAULT
	ingame_trade INGAME_TRADE_VICTOR_MUDKIP, OldaleTown_Mart_Text_VictorWantBergmite, OldaleTown_Mart_EventScript_VictorDecline, OldaleTown_Mart_EventScript_VictorNotBergmite, OldaleTown_Mart_Text_VictorTraded
	settrainerflag TRAINER_FROSTWOOD_VICTOR_TRADED
	release
	end

OldaleTown_Mart_EventScript_VictorDecline::
	msgbox OldaleTown_Mart_Text_VictorDecline, MSGBOX_DEFAULT
	release
	end

OldaleTown_Mart_EventScript_VictorNotBergmite::
	msgbox OldaleTown_Mart_Text_VictorNotBergmite, MSGBOX_DEFAULT
	release
	end

OldaleTown_Mart_EventScript_VictorAfterTrade::
	msgbox OldaleTown_Mart_Text_VictorAfterTrade, MSGBOX_DEFAULT
	release
	end

@ ---------------------------------------------------------------------------
@ CHAD the snowboarder has RedFatality's EPIC PASS. Beat him and he hands it
@ over, which starts Break Time's Over; RedFatality takes it on Route 2.
@ A save that already had the pass (the old Sennen Village item ball) gets
@ the battle but no second pass.
OldaleTown_Mart_EventScript_BoardOrSki::
	goto_if_set FLAG_GOT_EPIC_PASS, OldaleTown_Mart_EventScript_SnowboarderNoPass
	goto_if_set FLAG_REDFATALITY_HAS_EPIC_PASS, OldaleTown_Mart_EventScript_SnowboarderNoPass
	trainerbattle_single TRAINER_FROSTWOOD_BAR_SNOWBOARDER, OldaleTown_Mart_Text_SnowboarderIntro, OldaleTown_Mart_Text_SnowboarderDefeated, OldaleTown_Mart_EventScript_SnowboarderGivePass
	goto OldaleTown_Mart_EventScript_SnowboarderPostBattle

OldaleTown_Mart_EventScript_SnowboarderNoPass::
	trainerbattle_single TRAINER_FROSTWOOD_BAR_SNOWBOARDER, OldaleTown_Mart_Text_SnowboarderIntroNoPass, OldaleTown_Mart_Text_SnowboarderDefeated
OldaleTown_Mart_EventScript_SnowboarderPostBattle::
	msgbox OldaleTown_Mart_Text_BoardOrSki, MSGBOX_AUTOCLOSE
	end

OldaleTown_Mart_EventScript_SnowboarderGivePass::
	lock
	applymovement VAR_LAST_TALKED, Common_Movement_FacePlayer
	waitmovement 0
	msgbox OldaleTown_Mart_Text_SnowboarderDeal, MSGBOX_DEFAULT
	giveitem ITEM_EPIC_PASS
	setflag FLAG_GOT_EPIC_PASS
	startquest QUEST_BREAK_TIMES_OVER
	msgbox OldaleTown_Mart_Text_SnowboarderSendOff, MSGBOX_DEFAULT
	release
	end

OldaleTown_Mart_EventScript_Draco::
	lock
	faceplayer
	setspeaker SP_NAME_DRACO
	msgbox OldaleTown_Mart_Text_Draco, MSGBOX_DEFAULT
	release
	end

OldaleTown_Mart_Text_Draco:
	.string "{PLAYER}? Of course you found this\n"
	.string "place.\p"
	.string "Don't get comfortable. I'm just\n"
	.string "thawing out before I take that\l"
	.string "GYM BADGE.\p"
	.string "…And no, I'm not buying you\n"
	.string "a drink.$"

OldaleTown_Mart_Text_TwigsSkier:
	.string "I was riding pipe when I caught an\n"
	.string "edge on some nasty moguls…\p"
	.string "…and landed a switch 540 in pow\n"
	.string "deeper than the Swiss Alps.\p"
	.string "Craziest thing I ever did on a\n"
	.string "pair of twigs.$"

OldaleTown_Mart_Text_ApresGirl:
	.string "I'm just here for the après.$"

OldaleTown_Mart_Text_SkierIntro:
	.string "Last one down the mountain buys the\n"
	.string "next round!\p"
	.string "…We're not on the mountain?\n"
	.string "Fine, we'll battle for it!$"

OldaleTown_Mart_Text_SkierDefeated:
	.string "Total yard sale…$"

OldaleTown_Mart_Text_NervousGuy:
	.string "I'm way too nervous to talk to her…\p"
	.string "Maybe I'd have a chance if I complete\n"
	.string "my Pokedex.\p"
	.string "…No. Too slow.\p"
	.string "Maybe if I show her my AVALUGG…\p"
	.string "…That's it!\p"
	.string "There's just one problem.\n"
	.string "I don't have an AVALUGG.$"

OldaleTown_Mart_Text_VictorWantBergmite:
	.string "Do you have a BERGMITE to trade?\n"
	.string "I'll give you my MUDKIP!$"

OldaleTown_Mart_Text_VictorDecline:
	.string "Aw… I'll just DM her on Instagram.$"

OldaleTown_Mart_Text_VictorNotBergmite:
	.string "That's not a BERGMITE!\p"
	.string "Do you even know what an AVALUGG\n"
	.string "looks like?$"

OldaleTown_Mart_Text_VictorTraded:
	.string "Yes! Give it a few years, and this\n"
	.string "little guy will be a huge AVALUGG!\p"
	.string "Take care of WINGMAN for me.\p"
	.string "He's great against ICE types.\n"
	.string "Never helped me with the ladies,\l"
	.string "though.$"

OldaleTown_Mart_Text_VictorAfterTrade:
	.string "BERGMITE evolves at level 37,\n"
	.string "right?\p"
	.string "…I'm gonna be here a while.$"

OldaleTown_Mart_Text_SnowboarderIntro:
	.string "Board or ski?\p"
	.string "There's only one right answer.\p"
	.string "…Oh, you're here for the GYM?\n"
	.string "Forget it, bro. RedFatality's on PTO.\p"
	.string "He took the GONDOLA down to the beach\n"
	.string "and left his EPIC PASS right here on\l"
	.string "the bar. Now he can't ride back up.\p"
	.string "Tell you what. Beat me, and you can\n"
	.string "take it down to him.$"

OldaleTown_Mart_Text_SnowboarderIntroNoPass:
	.string "Board or ski?\p"
	.string "There's only one right answer.\p"
	.string "Wrong answer? Let's settle it!$"

OldaleTown_Mart_Text_SnowboarderDefeated:
	.string "Gnarly! You shred, dude.$"

OldaleTown_Mart_Text_SnowboarderDeal:
	.string "A deal's a deal.$"

OldaleTown_Mart_Text_SnowboarderSendOff:
	.string "Take the GONDOLA down to the beach.\n"
	.string "He's chilling at the bottom of the\l"
	.string "lift.\p"
	.string "Tell him break time's over.$"

OldaleTown_Mart_Text_BoardOrSki:
	.string "Board or ski?\p"
	.string "There's only one right answer.$"

OldaleTown_Mart_Text_PokeBallsAreSoldOut:
	.string "The clerk says they're all sold out.\n"
	.string "I can't buy any POKé BALLS.$"

OldaleTown_Mart_Text_ImGoingToBuyPokeBalls:
	.string "I'm going to buy a bunch of POKé BALLS\n"
	.string "and catch a bunch of POKéMON!$"

OldaleTown_Mart_Text_RestoreHPWithPotion:
	.string "If a POKéMON gets hurt and loses its HP\n"
	.string "and faints, it won't be able to battle.\p"
	.string "To prevent your POKéMON from fainting,\n"
	.string "restore its HP with a POTION.$"

