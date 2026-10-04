@ ============================================================================
@  THE HIJACKED YACHT (Route 2 pier)
@  Reward for the fisherman's questline: he hands over the YACHT KEY, which
@  finally turns the Route 2 "Hijack the yacht?" sign from a joke into a door.
@  Below deck the owner -- the person designing the next generation of
@  POKEMON -- left a field-test case with three prototypes: BROWT, POMBON and
@  GECQUA, each previewed with its sprite the way the Munen starters are. The
@  player may take exactly ONE (FLAG_WOT_YACHT_STARTER_TAKEN hides all three
@  the moment one is claimed). Their notes on the desk sketch where the three
@  evolve: sky, caves, deep sea.
@
@  In and out are ordinary warps. Aboard: Route 2's warp 2 on the gangway
@  (40,17), an arrow warp -- stand on it and press UP -- which lands on
@  the hatch here at (5,7). It is locked without the YACHT KEY by
@  WotGetLockedDoorScript (field_control_avatar.c). Ashore: the hatch tiles
@  (5,7)/(6,7) are south arrow warps back to that gangway.
@ ============================================================================

YachtInterior_MapScripts::
	.byte 0

@ The owner's working notes, on the desk at (1,4).
YachtInterior_EventScript_DeskNotes::
	msgbox YachtInterior_Text_DeskNotes, MSGBOX_SIGN
	end

@ VAR_0x8004 carries the chosen ball's local id to _Took, so that ball leaves
@ its cradle the moment it is taken and the other two go when the lid shuts.
@ It is set after givemon, so nothing between here and _Took can clobber it.
YachtInterior_EventScript_BallBrowt::
	lockall
	showmonpic SPECIES_BROWT, 10, 3
	msgbox YachtInterior_Text_BallBrowt, MSGBOX_YESNO
	hidemonpic
	goto_if_eq VAR_RESULT, NO, YachtInterior_EventScript_LeaveIt
	bufferspeciesname STR_VAR_1, SPECIES_BROWT
	givemon SPECIES_BROWT, 5
	setvar VAR_0x8004, LOCALID_YACHT_BALL_BROWT
	goto YachtInterior_EventScript_Took

YachtInterior_EventScript_BallPombon::
	lockall
	showmonpic SPECIES_POMBON, 10, 3
	msgbox YachtInterior_Text_BallPombon, MSGBOX_YESNO
	hidemonpic
	goto_if_eq VAR_RESULT, NO, YachtInterior_EventScript_LeaveIt
	bufferspeciesname STR_VAR_1, SPECIES_POMBON
	givemon SPECIES_POMBON, 5
	setvar VAR_0x8004, LOCALID_YACHT_BALL_POMBON
	goto YachtInterior_EventScript_Took

YachtInterior_EventScript_BallGecqua::
	lockall
	showmonpic SPECIES_GECQUA, 10, 3
	msgbox YachtInterior_Text_BallGecqua, MSGBOX_YESNO
	hidemonpic
	goto_if_eq VAR_RESULT, NO, YachtInterior_EventScript_LeaveIt
	bufferspeciesname STR_VAR_1, SPECIES_GECQUA
	givemon SPECIES_GECQUA, 5
	setvar VAR_0x8004, LOCALID_YACHT_BALL_GECQUA
	goto YachtInterior_EventScript_Took

@ One only. The flag goes on first so the other two can never be claimed.
@ The chosen ball leaves with the player; the note under it explains why the
@ other two stay, and they vanish as the lid goes down.
YachtInterior_EventScript_Took::
	goto_if_eq VAR_RESULT, MON_CANT_GIVE, YachtInterior_EventScript_NoRoom
	setflag FLAG_WOT_YACHT_STARTER_TAKEN
	removeobject VAR_0x8004
	playfanfare MUS_OBTAIN_ITEM
	message YachtInterior_Text_Took
	waitmessage
	waitfanfare
	msgbox YachtInterior_Text_AfterTook, MSGBOX_DEFAULT
	closemessage
	playse SE_CLICK
	removeobject LOCALID_YACHT_BALL_BROWT
	removeobject LOCALID_YACHT_BALL_POMBON
	removeobject LOCALID_YACHT_BALL_GECQUA
	waitse
	releaseall
	end

YachtInterior_EventScript_NoRoom::
	msgbox YachtInterior_Text_NoRoom, MSGBOX_DEFAULT
	releaseall
	end

YachtInterior_EventScript_LeaveIt::
	msgbox YachtInterior_Text_LeaveIt, MSGBOX_DEFAULT
	releaseall
	end

@ --- Text ------------------------------------------------------------------
YachtInterior_Text_BallBrowt::
	.string "There's a handwritten tag taped\n"
	.string "to this ball.\p"
	.string "“BROWT. Gets restless indoors.\n"
	.string "Crack a window for it, please.”\p"
	.string "Take BROWT with you?$"

YachtInterior_Text_BallPombon::
	.string "This ball's tag is singed around\n"
	.string "the edges.\p"
	.string "“POMBON. Runs warm. Do NOT let\n"
	.string "it nap on the charts again.”\p"
	.string "Take POMBON with you?$"

YachtInterior_Text_BallGecqua::
	.string "The ink on this tag has run,\n"
	.string "like it got splashed.\p"
	.string "“GECQUA. Needs water every day.\n"
	.string "Sulks if you forget.”\p"
	.string "Take GECQUA with you?$"

YachtInterior_Text_Took::
	.string "{PLAYER} received the {STR_VAR_1}!$"

YachtInterior_Text_AfterTook::
	.string "There's a sticky note on the\n"
	.string "inside of the lid.\p"
	.string "“Field test kit. ONE per trainer,\n"
	.string "please! The rest are spoken for.”\p"
	.string "You close the lid on the other two.$"

YachtInterior_Text_NoRoom::
	.string "Your party's full, and so are\n"
	.string "your boxes.\p"
	.string "You set the ball back down.$"

YachtInterior_Text_LeaveIt::
	.string "You put the ball back in its\n"
	.string "cradle.$"

YachtInterior_Text_DeskNotes::
	.string "Papers cover the desk, all in the\n"
	.string "same cramped handwriting.\p"
	.string "“GEN X STARTERS\n"
	.string "Evolution notes, draft 11.\p"
	.string "“The three in the case are doing\n"
	.string "great. Better than their designer.\p"
	.string "“The brief says this generation\n"
	.string "is about going further. Up, down,\l"
	.string "under. So what if the starters\l"
	.string "ARE the expedition?\p"
	.string "“BROWT: the sky. I think its final\n"
	.string "stage flies where the air gets too\l"
	.string "thin for anyone else. It already\l"
	.string "spends all day staring at clouds.\p"
	.string "“POMBON: caves. Its flame becomes\n"
	.string "a lantern. It goes down first and\l"
	.string "lights the way for everyone else.\p"
	.string "“GECQUA: the deep sea. A real\n"
	.string "diver, built for the pressure,\l"
	.string "with markings that glow down\l"
	.string "where the light gives up.\p"
	.string "“Can't settle their final types.\n"
	.string "Field test first, then decide.\p"
	.string "“...I should really get some\n"
	.string "sleep.”$"
