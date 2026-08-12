	.set LOCALID_MUNEN_LAKE_FISHERMAN, 1

MunenLake_MapScripts::
	.byte 0

MunenLake_EventScript_Fisherman::
	lock
	faceplayer
	msgbox MunenLake_Text_Fisherman, MSGBOX_DEFAULT
	giveitem ITEM_SUPER_ROD
	goto_if_eq VAR_RESULT, FALSE, MunenLake_EventScript_FishermanNoRoom
	setflag FLAG_MUNEN_LAKE_GOT_SUPER_ROD
	msgbox MunenLake_Text_FishermanFarewell, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_MUNEN_LAKE_FISHERMAN, MunenLake_Movement_FishermanRunOff
	waitmovement 0
	removeobject LOCALID_MUNEN_LAKE_FISHERMAN
	release
	end

MunenLake_EventScript_FishermanNoRoom::
	msgbox MunenLake_Text_FishermanNoRoom, MSGBOX_DEFAULT
	release
	end

MunenLake_Movement_FishermanRunOff:
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	step_end

MunenLake_Text_Fisherman:
	.string "Holy fuck that guy just fell\n"
	.string "through the fucking ice!\p"
	.string "Nah, fuck this.\n"
	.string "I quit this sport.\p"
	.string "Here, take this!$"

MunenLake_Text_FishermanFarewell:
	.string "I'm not built for this, man.$"

MunenLake_Text_FishermanNoRoom:
	.string "Your bag's full to bursting.\p"
	.string "Make some room and find me before\n"
	.string "I head off!$"

MunenLake_EventScript_Cameraman::
	lock
	faceplayer
	msgbox MunenLake_Text_Cameraman, MSGBOX_DEFAULT
	release
	end

MunenLake_Text_Cameraman:
	.string "Shhh! Keep it down, kid.\p"
	.string "Word is there's an enormous RED BEAST\n"
	.string "sleeping under this frozen lake.\p"
	.string "The second it busts up through the ice,\n"
	.string "I'll get the shot of a lifetime --\l"
	.string "front page of the FROSTWOOD news!\p"
	.string "...Any minute now.\p"
	.string "...Annny minute now.$"
