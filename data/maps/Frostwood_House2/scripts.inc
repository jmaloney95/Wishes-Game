@ ============================================================================
@  The Frostwood gym leader's family home, AFTER Toasty Time. The girls are
@  home with their AZURILL and everyone's happy. The door into OldaleTown_House2
@  is redirected here once FLAG_MELTINGMILE_PATH_CLEARED is set
@  (WotRedirectWarp in field_control_avatar.c); before that, House2 is used.
@ ============================================================================
Frostwood_House2_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Frostwood_House2_OnResume
	.byte 0

@ ON_RESUME, not ON_LOAD: the task needs the objects spawned, and menus reset the
@ task pool, so it is re-armed every time the map resumes (a no-op if running).
Frostwood_House2_OnResume::
	setvar VAR_0x8004, (1 << LOCALID_FW_HOUSE2_MOM) | (1 << LOCALID_FW_HOUSE2_GIRL_1) | (1 << LOCALID_FW_HOUSE2_GIRL_2) | (1 << LOCALID_FW_HOUSE2_AZURILL_1) | (1 << LOCALID_FW_HOUSE2_AZURILL_2)
	setvar VAR_0x8005, (1 << LOCALID_FW_HOUSE2_AZURILL_1) | (1 << LOCALID_FW_HOUSE2_AZURILL_2)
	special WotStartFamilyHearts
	end

Frostwood_House2_EventScript_Mom::
	lock
	faceplayer
	msgbox Frostwood_House2_Text_Mom, MSGBOX_DEFAULT
	release
	end

Frostwood_House2_EventScript_Dad::
	lock
	faceplayer
	goto_if_set FLAG_TOASTY_TIME_RARE_CANDY, Frostwood_House2_EventScript_DadAfter
	msgbox Frostwood_House2_Text_DadThanks, MSGBOX_DEFAULT
	giveitem ITEM_RARE_CANDY
	@ giveitem clobbers VAR_RESULT; its success flag is VAR_0x8007, and it
	@ already printed the bag-full message if there was no room.
	goto_if_eq VAR_0x8007, FALSE, Frostwood_House2_EventScript_DadBagFull
	setflag FLAG_TOASTY_TIME_RARE_CANDY
	msgbox Frostwood_House2_Text_DadAfterGift, MSGBOX_DEFAULT
Frostwood_House2_EventScript_DadBagFull::
	release
	end

Frostwood_House2_EventScript_DadAfter::
	msgbox Frostwood_House2_Text_DadAfter, MSGBOX_DEFAULT
	release
	end

Frostwood_House2_EventScript_Girl1::
	lock
	faceplayer
	msgbox Frostwood_House2_Text_Girl1, MSGBOX_DEFAULT
	release
	end

Frostwood_House2_EventScript_Girl2::
	lock
	faceplayer
	msgbox Frostwood_House2_Text_Girl2, MSGBOX_DEFAULT
	release
	end

Frostwood_House2_Text_Mom:
	.string "Both my girls home, warm and fed,\n"
	.string "and their AZURILL won't stop bouncing!\p"
	.string "Thank you for bringing them back\n"
	.string "to us.$"

Frostwood_House2_Text_DadThanks:
	.string "There you are! You actually got\n"
	.string "those two to come in from the snow.\p"
	.string "Supper was still warm, even!\n"
	.string "Thank you for your help.\p"
	.string "Here -- a little something for\n"
	.string "your trouble.$"

Frostwood_House2_Text_DadAfterGift:
	.string "Give it to a POKéMON you're proud of.\n"
	.string "Our girls sure made us proud today.$"

Frostwood_House2_Text_DadAfter:
	.string "A full house and a hot supper.\n"
	.string "Doesn't get better than that.$"

Frostwood_House2_Text_Girl1:
	.string "The cocoa worked. It always works.\p"
	.string "We'll get back to training\n"
	.string "tomorrow!$"

Frostwood_House2_Text_Girl2:
	.string "Our AZURILL are so happy to be\n"
	.string "warm again!\p"
	.string "One day we'll be as strong as\n"
	.string "our big brother.$"
