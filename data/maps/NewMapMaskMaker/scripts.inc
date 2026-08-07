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
	.string "...The oni mask is a warding face.\n"
	.string "You wear it so the dead mistake\l"
	.string "you for their own, and let you\l"
	.string "pass in peace.\p"
	.string "Those climbers wearing my designs\n"
	.string "have twisted them into something\l"
	.string "else. Something hungry.\p"
	.string "And my apprentice, RYOKO went with them.\p"
	.string "He took the mask he'd carved for\n"
	.string "this year's festival.\p"
	.string "I saw them enter the OLD TOMB at\n"
	.string "the NORTH end of the village.\p"
	.string "I'm too old to venture in.\n"
	.string "If anything of his is still down\l"
	.string "there... would you bring it to me?$"

NewMapMaskMaker_Text_GenReminder::
	.string "The old tomb tunnel, north side of\n"
	.string "the village, past the warning sign.\p"
	.string "The door's seal is already open.\p"
	.string "Whatever RYOKO left down\n"
	.string "there... I'd know it.\l"
	.string "I taught him.$"

NewMapMaskMaker_Text_GenMaskFound::
	.string "That mask... give it here. Let me\n"
	.string "see it in the light.\p"
	.string "...Yes. This is his carving.\n"
	.string "You fought your way past their\l"
	.string "goons for this.\p"
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
	.string "POKEMON that know ROCK SMASH may\n"
	.string "use it in the field.\p"
	.string "The path up the mountain is\n"
	.string "choked with rubble. You'll need\l"
	.string "it.$"

NewMapMaskMaker_Text_GenKeepIt::
	.string "And keep the mask. I mean it.\p"
	.string "Put it on before they lay eyes on\n"
	.string "you, and the ONI will take you for\l"
	.string "one of their own.\p"
	.string "Oh -- and I've cleared the crates\n"
	.string "off RYOKO'S door. His house is yours\l"
	.string "to look through. Perhaps you'll\l"
	.string "understand him better than I did.$"

NewMapMaskMaker_Text_GenAfter::
	.string "The dawn bell rings twice before\n"
	.string "Doomsday.$"

NewMapMaskMaker_Text_MaskWall::
	.string "Festival masks line the wall.\n"
	.string "Fox. Crane. Moon hare. Thunder cat.\p"
	.string "At the center, one empty hook.$"

NewMapMaskMaker_Text_Workbench::
	.string "A workbench worn smooth by forty\n"
	.string "years of hands.$"
