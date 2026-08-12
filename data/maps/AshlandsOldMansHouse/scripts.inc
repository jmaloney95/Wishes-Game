AshlandsOldMansHouse_MapScripts::
	.byte 0

@ ============================================================================
@  THE OLD PROPHET'S HOUSE (Ashlands) -- opened when the old man vanishes
@  after the FENTANYL quest. Holds the CATALPA BOW (item ball) that opens
@  the gravesite crossing in the Act 2 National Park.
@  Three generations in one room: the prophet, Prof. Clarkson, and the boy
@  who became Team Mutrid's masked leader.
@ ============================================================================
@ The Catalpa Bow item ball: quest stage 3 on pickup.
AshlandsOldMansHouse_EventScript_BowBall::
	finditem ITEM_CATALPA_BOW
	goto_if_eq VAR_RESULT, FALSE, AshlandsOldMansHouse_EventScript_BowBallEnd
	compare VAR_QUEST_FATHERS_WISH, 2
	goto_if_ne AshlandsOldMansHouse_EventScript_BowBallEnd
	setvar VAR_QUEST_FATHERS_WISH, 3
	startquest QUEST_GRANDFATHERS_WISH
AshlandsOldMansHouse_EventScript_BowBallEnd::
	end

AshlandsOldMansHouse_EventScript_FamilyShrine::
	msgbox AshlandsOldMansHouse_Text_FamilyShrine, MSGBOX_SIGN
	end

AshlandsOldMansHouse_EventScript_Letters::
	msgbox AshlandsOldMansHouse_Text_Letters, MSGBOX_SIGN
	end

@ ============================================================================
@  Text
@ ============================================================================
AshlandsOldMansHouse_Text_FamilyShrine::
	.string "A small household shrine, kept\n"
	.string "spotless amid the ash.\p"
	.string "Three photographs stand in a row.\p"
	.string "An old man in prophet's robes,\n"
	.string "stern before the volcano.\p"
	.string "A young man with a field ledger\n"
	.string "and an embarrassed smile --\l"
	.string "unmistakably Prof. CLARKSON.\p"
	.string "And a boy holding a festival\n"
	.string "mask.\p"
	.string "The boy's face has been scratched\n"
	.string "out. By a fingernail, over many\l"
	.string "years.$"

AshlandsOldMansHouse_Text_Letters::
	.string "A bundle of letters, tied with\n"
	.string "prayer-cord. All the same hand.\p"
	.string "“Father -- the mountain is not a\n"
	.string "god. It is a MECHANISM. I can\l"
	.string "prove it. -- C.”\p"
	.string "“Father -- answer me. The boy\n"
	.string "listens to you. Tell him the\l"
	.string "rites are METAPHOR. -- C.”\p"
	.string "“Father. He has stopped writing\n"
	.string "to me. Watch him. -- C.”\p"
	.string "The last letter is unopened.\n"
	.string "The seal is unbroken.$"
