OldaleTown_House2_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, OldaleTown_House2_OnResume
	.byte 0

@ OnResume (not OnLoad): OnLoad runs before ResumeMap's ResetSpriteData,
@ which would wipe the indicator sprite before it renders.
OldaleTown_House2_OnResume::
	questindicator 2, SIDE_QUEST_TOASTY_TIME  @ Dad, until the cocoa quest is picked up
	end

@ Frostwood Town gym leader's family home (reuses the OldaleTown_House2 interior).
@ Woman = proud mother of the GYM LEADER. Man = father who sends the player to fetch
@ the two daughters (the AZURILL trainers blocking the lake path on MeltingMile) for
@ dinner, handing over the HOT COCOA that lures them home.
OldaleTown_House2_EventScript_Woman::
	lock
	faceplayer
	msgbox OldaleTown_House2_Text_MomProud, MSGBOX_DEFAULT
	release
	end

OldaleTown_House2_EventScript_Man::
	lock
	faceplayer
	goto_if_set FLAG_GOT_HOT_COCOA, OldaleTown_House2_EventScript_DadReminder
	msgbox OldaleTown_House2_Text_DadAsk, MSGBOX_DEFAULT
	giveitem ITEM_HOT_COCOA
	setflag FLAG_GOT_HOT_COCOA
	startquest SIDE_QUEST_TOASTY_TIME
	msgbox OldaleTown_House2_Text_DadAfterGive, MSGBOX_DEFAULT
	release
	end

OldaleTown_House2_EventScript_DadReminder::
	msgbox OldaleTown_House2_Text_DadReminder, MSGBOX_DEFAULT
	release
	end

OldaleTown_House2_Text_MomProud:
	.string "That's my boy running the GYM here in\n"
	.string "FROSTWOOD -- a real GYM LEADER!\p"
	.string "I always knew he had it in him. His\n"
	.string "little sisters worship him, you know.\p"
	.string "They're out on the trail every day,\n"
	.string "training their AZURILL, dreaming of\l"
	.string "being just as strong someday.$"

OldaleTown_House2_Text_DadAsk:
	.string "Oh! A visitor. Perfect timing -- maybe\n"
	.string "you can help an old man out.\p"
	.string "My two girls are up the trail toward the\n"
	.string "lake AGAIN, drilling that SPLASH over\l"
	.string "and over. Supper's nearly stone cold.\p"
	.string "Here -- take them this HOT COCOA.\n"
	.string "Those two will never come in from the\l"
	.string "snow without it.$"

OldaleTown_House2_Text_DadAfterGive:
	.string "Tell them their father says it's time\n"
	.string "to come home and eat. They'll grumble,\l"
	.string "but the cocoa always wins.$"

OldaleTown_House2_Text_DadReminder:
	.string "Still freezing out there, those two.\p"
	.string "Bring them that HOT COCOA -- they'll\n"
	.string "follow you home, you'll see.$"
