Ashlands_MapScripts::
	.byte 0

@==============================================================================
@ THE DYING OLD MAN (lying in the ash) -- fetch quest for FENTANYL. Bring it and
@ he reveals who burned the world: TEAM MUTRID, and what they did with JIRACHI.
@==============================================================================
Ashlands_EventScript_OldMan::
	lock
	faceplayer
	goto_if_set FLAG_FENTANYL_GIVEN, Ashlands_EventScript_OldManAfter
	checkitem ITEM_FENTANYL
	compare VAR_RESULT, TRUE
	goto_if_eq Ashlands_EventScript_OldManReceive
	msgbox Ashlands_Text_OldManQuest, MSGBOX_DEFAULT
	release
	end

Ashlands_EventScript_OldManReceive::
	msgbox Ashlands_Text_OldManTakeItem, MSGBOX_DEFAULT
	removeitem ITEM_FENTANYL
	setflag FLAG_FENTANYL_GIVEN
	msgbox Ashlands_Text_OldManLore, MSGBOX_DEFAULT
	release
	end

Ashlands_EventScript_OldManAfter::
	msgbox Ashlands_Text_OldManAfter, MSGBOX_DEFAULT
	release
	end

@==============================================================================
@ THE SAGE -- raving about the prophecy. Clearly lost his mind.
@==============================================================================
Ashlands_EventScript_Sage::
	lock
	faceplayer
	msgbox Ashlands_Text_Sage, MSGBOX_DEFAULT
	release
	end

@==============================================================================
@ TEXT
@==============================================================================
Ashlands_Text_OldManQuest:
	.string "…hhh… you there. Living one.\p"
	.string "My body's giving out. The ash got\n"
	.string "into my lungs days ago.\p"
	.string "There's medicine in an old supply\n"
	.string "ball, west of here -- FENTANYL.\p"
	.string "Bring it to me… and I'll tell you\n"
	.string "what did this to us.$"

Ashlands_Text_OldManTakeItem:
	.string "{PLAYER} handed over the FENTANYL.\p"
	.string "…Ahh. The pain… it lets go.\n"
	.string "Bless you, child.$"

Ashlands_Text_OldManLore:
	.string "Now listen, while I still can.\p"
	.string "TEAM MUTRID. A doomsday cult, old\n"
	.string "as the volcano they pray to.\p"
	.string "They worship ruin -- believe the\n"
	.string "world is rotten and begs to burn.\p"
	.string "Their leader is a masked devil. The\n"
	.string "old prophet's own blood, they say.\p"
	.string "Days ago they climbed the mountain\n"
	.string "and seized JIRACHI -- the Undying\l"
	.string "Wish, that grants any desire.\p"
	.string "And their desire… is the end of\n"
	.string "everything. Look around you.\p"
	.string "You were up on that summit, weren't\n"
	.string "you? You saw their faces.\p"
	.string "Then you may be the only soul alive\n"
	.string "who can still stop them.$"

Ashlands_Text_OldManAfter:
	.string "Go on, child… before there's\n"
	.string "nothing left to save.$"

Ashlands_Text_Sage:
	.string "It's fulfilled! The prophecy,\n"
	.string "FULFILLED!\p"
	.string "The star fell, the wish was spoken,\n"
	.string "and the sky turned to ash! Hahaha!\p"
	.string "Don't you SEE? We asked for this.\n"
	.string "We BEGGED the wish to end us.\p"
	.string "…Are you the dreamer, or the dream?\n"
	.string "I can never tell anymore.\p"
	.string "The ash tastes like snow on my\n"
	.string "tongue. Isn't it beautiful?$"
