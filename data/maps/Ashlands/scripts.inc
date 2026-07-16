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
	compare VAR_QUEST_FATHERS_WISH, 0
	goto_if_ne Ashlands_EventScript_OldManQuestKnown
	closemessage
	setvar VAR_QUEST_FATHERS_WISH, 1
	bufferstring STR_VAR_1, Quest_Text_FathersFinalWishTitle
	call Quest_EventScript_NotifyStarted
Ashlands_EventScript_OldManQuestKnown::
	release
	end

@ Local ID 3 = the old man. After the lore he vanishes into ash
@ (FLAG_HIDE_ASHLANDS_OLD_MAN), unblocking the door to his house,
@ where the CATALPA BOW waits.
Ashlands_EventScript_OldManReceive::
	msgbox Ashlands_Text_OldManTakeItem, MSGBOX_DEFAULT
	removeitem ITEM_FENTANYL
	setflag FLAG_FENTANYL_GIVEN
	msgbox Ashlands_Text_OldManLore, MSGBOX_DEFAULT
	msgbox Ashlands_Text_OldManBequest, MSGBOX_DEFAULT
	closemessage
	playse SE_WARP_IN
	fadescreen FADE_TO_BLACK
	removeobject 3
	setflag FLAG_HIDE_ASHLANDS_OLD_MAN
	fadescreen FADE_FROM_BLACK
	msgbox Ashlands_Text_OldManGone, MSGBOX_DEFAULT
	closemessage
	setvar VAR_QUEST_FATHERS_WISH, 2
	bufferstring STR_VAR_1, Quest_Text_FathersFinalWishTitle
	call Quest_EventScript_NotifyUpdated
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

Ashlands_Text_OldManLore::
	.string "Listen, while I can.\p"
	.string "TEAM MUTRID worships ruin.\n"
	.string "Their leader: my GRANDSON.\p"
	.string "I taught him this mountain's\n"
	.string "rites. His father, the same.\p"
	.string "My son, Professor CLARKSON,\n"
	.string "climbed to STUDY JIRACHI.\p"
	.string "His boy came to SEIZE it.\n"
	.string "The Undying Wish.\p"
	.string "His one desire: burn it all.\p"
	.string "You saw him cast his own\n"
	.string "father into the fire.\p"
	.string "The hand that did it,\n"
	.string "I taught its every rite.\p"
	.string "This ash is his wish made real.\p"
	.string "You may be the last soul\n"
	.string "who can stop my family.$"

Ashlands_Text_OldManAfter::
	.string "Go on, child… before there's\n"
	.string "nothing left to save.$"

Ashlands_Text_OldManBequest::
	.string "The medicine bought minutes,\n"
	.string "not years. Let them count.\p"
	.string "In my house hangs a CATALPA\n"
	.string "bow, strung with prayer-cord.\p"
	.string "A rite: play it at a grave,\n"
	.string "so the dead aren't alone.\p"
	.string "My son rests at the park\n"
	.string "across the water.\p"
	.string "Play the rite for him --\n"
	.string "his own blood never will.\p"
	.string "And tell his stone…\p"
	.string "his father believes him now.$"
Ashlands_Text_OldManGone:
	.string "…\p"
	.string "Where the old man lay there is\n"
	.string "only a settling of fine ash,\l"
	.string "already scattering on the wind.\p"
	.string "The door of his house stands\n"
	.string "unbarred.$"

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

@ ----------------------------------------------------------------------------
@  West seam -> Ashlands_2. This boundary CANNOT be a map connection: Ashlands
@  uses the General primary tileset and Ashlands_2 uses NightMarket, and
@  connection crossings never reload the primary tileset or its animation
@  callback (only the secondary), so General's tiles + animations bleed into
@  every NightMarket map in the chain ("skewed textures"). A warp does a full
@  load, which swaps the primary cleanly.
@ ----------------------------------------------------------------------------
Ashlands_EventScript_SeamWest5::
	warp MAP_ASHLANDS_2, 24, 5
	waitstate
	end

Ashlands_EventScript_SeamWest6::
	warp MAP_ASHLANDS_2, 24, 6
	waitstate
	end

Ashlands_EventScript_SeamWest7::
	warp MAP_ASHLANDS_2, 24, 7
	waitstate
	end

Ashlands_EventScript_SeamWest8::
	warp MAP_ASHLANDS_2, 24, 8
	waitstate
	end
