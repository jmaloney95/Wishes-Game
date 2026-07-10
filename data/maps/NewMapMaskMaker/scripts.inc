NewMapMaskMaker_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, NewMapMaskMaker_OnResume
	.byte 0

NewMapMaskMaker_OnResume::
	questindicator 1, SIDE_QUEST_SAVE_FACE  @ Master Gen, until Save Face is picked up
	end

@ ============================================================================
@  MASTER GEN, THE MASK-MAKER -- quest giver for the Munen Tunnel.
@  His apprentice Itsuki stole a festival mask and joined the oni group.
@  States:
@    1. First talk          -> sets FLAG_NEWMAP_TUNNEL_RUMOR (quest open)
@    2. Quest open, no mask -> reminder
@    3. Player has mask     -> reward, sets FLAG_NEWMAP_MASK_RETURNED and
@                              FLAG_NEWMAP_APPRENTICE_DOOR_OPEN (clears the
@                              crates from Itsuki's door). Player KEEPS the
@                              mask -- in Act 2 it works as a DISGUISE: oni
@                              goons take a masked player for one of their
@                              own and skip the battle (see
@                              data/scripts/oni_mask_disguise.inc).
@    4. Done                -> closing words
@ ============================================================================
NewMapMaskMaker_EventScript_Gen::
	lock
	faceplayer
	setspeaker SP_NAME_MASTER_GEN
	goto_if_set FLAG_NEWMAP_MASK_RETURNED, NewMapMaskMaker_EventScript_GenAfter
	checkitem ITEM_CARVED_MASK
	goto_if_eq VAR_RESULT, TRUE, NewMapMaskMaker_EventScript_GenMaskFound
	goto_if_set FLAG_NEWMAP_TUNNEL_RUMOR, NewMapMaskMaker_EventScript_GenReminder
	msgbox NewMapMaskMaker_Text_GenIntro, MSGBOX_DEFAULT
	setflag FLAG_NEWMAP_TUNNEL_RUMOR
	@ Gen has the slab hauled off the tunnel mouth (the rock object in town
	@ hides once this is set).
	setflag FLAG_POMPEII_TUNNEL_ROCK_CLEARED
	startquest SIDE_QUEST_SAVE_FACE
	release
	end

NewMapMaskMaker_EventScript_GenReminder::
	msgbox NewMapMaskMaker_Text_GenReminder, MSGBOX_DEFAULT
	release
	end

NewMapMaskMaker_EventScript_GenMaskFound::
	msgbox NewMapMaskMaker_Text_GenMaskFound, MSGBOX_DEFAULT
	playfanfare MUS_OBTAIN_BADGE
	message NewMapMaskMaker_Text_ReceivedFlintBadge
	waitmessage
	waitfanfare
	setflag FLAG_BADGE03_GET
	msgbox NewMapMaskMaker_Text_GenBadgeWords, MSGBOX_DEFAULT
	msgbox NewMapMaskMaker_Text_GenKeepIt, MSGBOX_DEFAULT
	setflag FLAG_NEWMAP_MASK_RETURNED
	setflag FLAG_NEWMAP_APPRENTICE_DOOR_OPEN
	@ completequest also unlocks the quest for players who found the mask
	@ before ever hearing Gen's request.
	completequest SIDE_QUEST_SAVE_FACE
	release
	end

NewMapMaskMaker_EventScript_GenAfter::
	msgbox NewMapMaskMaker_Text_GenAfter, MSGBOX_DEFAULT
	release
	end

@ --------------------------------------------------------------------------
@  Furniture
@ --------------------------------------------------------------------------
NewMapMaskMaker_EventScript_MaskWall::
	msgbox NewMapMaskMaker_Text_MaskWall, MSGBOX_SIGN
	end

NewMapMaskMaker_EventScript_Workbench::
	msgbox NewMapMaskMaker_Text_Workbench, MSGBOX_SIGN
	end

@ ============================================================================
@  Text
@ ============================================================================
NewMapMaskMaker_Text_GenIntro::
	.string "...Forty years I've carved for the\n"
	.string "mountain festival. Fox, crane, oni.\p"
	.string "The oni mask is a WARDING face.\n"
	.string "You wear it so the dead mistake\l"
	.string "you for their own, and let you\l"
	.string "pass in peace.\p"
	.string "Those climbers wearing my designs\n"
	.string "have twisted them into something\l"
	.string "else. Something hungry.\p"
	.string "And my apprentice, Itsuki...\n"
	.string "he went with them.\p"
	.string "He took the mask he'd carved for\n"
	.string "this year's festival. His first.\l"
	.string "His best.\p"
	.string "They camped in the old tomb tunnel\n"
	.string "west of the village before they\l"
	.string "climbed. The sealed one.\p"
	.string "I'm too old for tombs. If anything\n"
	.string "of my boy is still down there...\l"
	.string "would you bring it to me?\p"
	.string "The village keeps a slab over the\n"
	.string "tunnel mouth so nothing wanders\l"
	.string "out. Or in.\p"
	.string "I'll have it rolled aside before\n"
	.string "you reach it. Go carefully --\l"
	.string "THEY left more than footprints.$"

NewMapMaskMaker_Text_GenReminder::
	.string "The old tomb tunnel, west side of\n"
	.string "the village, past the warning sign.\p"
	.string "The seal on the door is already\n"
	.string "broken. THEY saw to that.\p"
	.string "Whatever my apprentice left down\n"
	.string "there... I'd know it by the\l"
	.string "knife-work. I taught him, after all.$"

NewMapMaskMaker_Text_GenMaskFound::
	.string "That mask... give it here. Let me\n"
	.string "see it in the light.\p"
	.string "...Yes. This is Itsuki's carving.\n"
	.string "This cut here -- he holds the knife\l"
	.string "wrong, no matter how I scold him.\p"
	.string "He left his festival mask behind\n"
	.string "and climbed in one of THEIRS.\p"
	.string "...Then he means to pass the Summit\n"
	.string "Gate as one of the dead.\p"
	.string "You fought your way past that\n"
	.string "self-styled GENERAL for this.\p"
	.string "Then you've earned what I've kept\n"
	.string "in the workbench drawer forty\l"
	.string "years.$"

NewMapMaskMaker_Text_ReceivedFlintBadge::
	.string "{PLAYER} received the FLINT BADGE\n"
	.string "from MASTER GEN!$"

NewMapMaskMaker_Text_GenBadgeWords::
	.string "The FLINT BADGE. From my league\n"
	.string "days, before these hands turned\l"
	.string "to carving.\p"
	.string "It carries the league's writ:\n"
	.string "POKEMON that know ROCK SMASH may\l"
	.string "use it in the field.\p"
	.string "The path up the mountain is\n"
	.string "choked with rubble. You'll need\l"
	.string "it.$"

NewMapMaskMaker_Text_GenKeepIt::
	.string "And keep the mask. I mean it.\p"
	.string "A warding face, carved honest, is\n"
	.string "a passport among the masked.\p"
	.string "Put it on before they lay eyes on\n"
	.string "you, and the oni will take you for\l"
	.string "one of their own.\p"
	.string "No challenge. No questions. They\n"
	.string "don't speak on the climb anyway.\p"
	.string "And when you find Itsuki up there,\n"
	.string "show him his own honest work...\l"
	.string "before that mountain tells him\l"
	.string "what he is.\p"
	.string "Oh -- and I've cleared the crates\n"
	.string "off his door. His house is yours\l"
	.string "to look through. Perhaps you'll\l"
	.string "understand him better than I did.$"

NewMapMaskMaker_Text_GenAfter::
	.string "The dawn bell rings twice before a\n"
	.string "festival. It rang twice this\l"
	.string "morning, and I carved nothing.\p"
	.string "Bring my boy down the mountain,\n"
	.string "trainer. Masked or bare-faced,\l"
	.string "I don't care which.$"

NewMapMaskMaker_Text_MaskWall::
	.string "Festival masks line the wall.\n"
	.string "Fox. Crane. Moon hare. Thunder cat.\p"
	.string "At the center, one empty hook.\p"
	.string "The dust says its mask hung there\n"
	.string "a long, long time.$"

NewMapMaskMaker_Text_Workbench::
	.string "A workbench worn smooth by forty\n"
	.string "years of hands.\p"
	.string "A half-carved oni mask stares up\n"
	.string "from the vise, one eye still\l"
	.string "trapped in the wood.\p"
	.string "The chisel beside it is dated and\n"
	.string "labeled: “ITSUKI'S FIRST.”$"
