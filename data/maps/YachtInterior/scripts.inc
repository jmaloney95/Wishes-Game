@ ============================================================================
@  THE HIJACKED YACHT (Route 2 pier)
@  Reward for the fisherman's questline: he hands over the YACHT KEY, which
@  finally turns the Route 2 "Hijack the yacht?" sign from a joke into a door.
@  Below deck the previous owner left three Hoenn starters in their balls --
@  the player may take exactly ONE (FLAG_WOT_YACHT_STARTER_TAKEN hides all
@  three the moment one is claimed).
@  PLACEHOLDER ROOM -- Joe decorates the layout in porymap.
@ ============================================================================

YachtInterior_MapScripts::
	map_script MAP_SCRIPT_ON_FRAME_TABLE, YachtInterior_OnFrame
	.byte 0

YachtInterior_OnFrame:
	map_script_2 VAR_TEMP_1, 0, YachtInterior_EventScript_Disembark
	.2byte 0

@ Step onto the hatch tile to go back ashore beside the sign.
YachtInterior_EventScript_Disembark::
	lockall
	setvar VAR_TEMP_1, 1
	fadescreen FADE_TO_BLACK
	warp MAP_ROUTE_2, 34, 28
	waitstate
	releaseall
	end

YachtInterior_EventScript_BallTreecko::
	lockall
	msgbox YachtInterior_Text_BallTreecko, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, YachtInterior_EventScript_LeaveIt
	givemon SPECIES_TREECKO, 5
	goto YachtInterior_EventScript_Took

YachtInterior_EventScript_BallTorchic::
	lockall
	msgbox YachtInterior_Text_BallTorchic, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, YachtInterior_EventScript_LeaveIt
	givemon SPECIES_TORCHIC, 5
	goto YachtInterior_EventScript_Took

YachtInterior_EventScript_BallMudkip::
	lockall
	msgbox YachtInterior_Text_BallMudkip, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, YachtInterior_EventScript_LeaveIt
	givemon SPECIES_MUDKIP, 5
	goto YachtInterior_EventScript_Took

@ One only. The other two balls vanish with the flag on the next map load,
@ and are removed immediately so the choice reads as final.
YachtInterior_EventScript_Took::
	goto_if_eq VAR_RESULT, MON_CANT_GIVE, YachtInterior_EventScript_NoRoom
	playfanfare MUS_OBTAIN_ITEM
	message YachtInterior_Text_Took
	waitmessage
	waitfanfare
	closemessage
	setflag FLAG_WOT_YACHT_STARTER_TAKEN
	removeobject 1
	removeobject 2
	removeobject 3
	msgbox YachtInterior_Text_AfterTook, MSGBOX_DEFAULT
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
YachtInterior_Text_BallTreecko::
	.string "A ball rests in a cradle of\n"
	.string "green velvet.\p"
	.string "Something inside it is very,\n"
	.string "very still.\p"
	.string "Take this one?$"

YachtInterior_Text_BallTorchic::
	.string "This ball is warm to the touch.\p"
	.string "Whatever is in there has been\n"
	.string "waiting a long time.\p"
	.string "Take this one?$"

YachtInterior_Text_BallMudkip::
	.string "Condensation beads on the third\n"
	.string "ball and runs off the cradle.\p"
	.string "Take this one?$"

YachtInterior_Text_Took::
	.string "{PLAYER} took the ball!$"

YachtInterior_Text_AfterTook::
	.string "The other two cradles fold\n"
	.string "themselves shut.\p"
	.string "One was all the yacht was ever\n"
	.string "going to give you.$"

YachtInterior_Text_NoRoom::
	.string "Your party and your boxes are\n"
	.string "both full.\p"
	.string "The cradle stays shut.$"

YachtInterior_Text_LeaveIt::
	.string "You leave the ball in its\n"
	.string "cradle.$"
