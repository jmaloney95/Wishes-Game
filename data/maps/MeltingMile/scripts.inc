@ LOCALIDs (1-indexed by object_events order in map.json):
@   1 Gentleman, 2 Girl_3 (right), 3 Girl_2 (left),
@   4 Azurill (left, x23), 5 Azurill (right, x24), 6 Apricorn tree, 7 Pikachu
	.set LOCALID_MM_GIRL_R, 2
	.set LOCALID_MM_GIRL_L, 3
	.set LOCALID_MM_AZURILL_L, 4
	.set LOCALID_MM_AZURILL_R, 5

MeltingMile_MapScripts::
	.byte 0

@==============================
@ Gentleman -- the thaw is drawing in pokemon that don't belong this far north.
@ A wild PIKACHU object stands next to him as a visual hint.
@==============================
MeltingMile_EventScript_Gentleman::
	lock
	faceplayer
	msgbox MeltingMileText_Gentleman, MSGBOX_DEFAULT
	release
	end

MeltingMile_EventScript_Pikachu::
	lock
	faceplayer
	playmoncry SPECIES_PIKACHU, CRY_MODE_ENCOUNTER
	msgbox MeltingMileText_Pikachu, MSGBOX_DEFAULT
	waitmoncry
	release
	end

@==============================
@ Two girls "training" their AZURILL with endless Splash, blocking the path to
@ Lake Munen. Talking to any of the four runs the gag. They won't budge until
@ the player has the path item (placeholder ITEM_SS_TICKET -- swap later).
@==============================
MeltingMile_EventScript_SplashGirls::
	lockall
	checkitem ITEM_HOT_COCOA          @ their dad's cocoa lures them home (from FrostwoodTown house)
	goto_if_eq VAR_RESULT, TRUE, MeltingMile_EventScript_GirlsLeave
	@ ---- Round 1 ----
	msgbox MeltingMileText_UseSplash, MSGBOX_DEFAULT
	applymovement LOCALID_MM_AZURILL_L, MeltingMile_Movement_SplashRight
	waitmovement 0
	msgbox MeltingMileText_UsedSplash, MSGBOX_DEFAULT
	closemessage
	msgbox MeltingMileText_UseSplash, MSGBOX_DEFAULT
	applymovement LOCALID_MM_AZURILL_R, MeltingMile_Movement_SplashLeft
	waitmovement 0
	msgbox MeltingMileText_UsedSplash, MSGBOX_DEFAULT
	closemessage
	@ ---- Round 2 ----
	msgbox MeltingMileText_UseSplash, MSGBOX_DEFAULT
	applymovement LOCALID_MM_AZURILL_L, MeltingMile_Movement_SplashRight
	waitmovement 0
	msgbox MeltingMileText_UsedSplash, MSGBOX_DEFAULT
	closemessage
	msgbox MeltingMileText_UseSplash, MSGBOX_DEFAULT
	applymovement LOCALID_MM_AZURILL_R, MeltingMile_Movement_SplashLeft
	waitmovement 0
	msgbox MeltingMileText_UsedSplash, MSGBOX_DEFAULT
	closemessage
	@ ---- Out of PP ----
	msgbox MeltingMileText_NoPP, MSGBOX_DEFAULT
	closemessage
	@ ---- Everyone is baffled (animated ? over all four) ----
	applymovement LOCALID_MM_GIRL_L, MeltingMile_Movement_Question
	applymovement LOCALID_MM_GIRL_R, MeltingMile_Movement_Question
	applymovement LOCALID_MM_AZURILL_L, MeltingMile_Movement_Question
	applymovement LOCALID_MM_AZURILL_R, MeltingMile_Movement_Question
	waitmovement 0
	releaseall
	end

@ Player has the path item -> the girls finally give up the spot and clear off.
MeltingMile_EventScript_GirlsLeave::
	msgbox MeltingMileText_GirlsLeave, MSGBOX_DEFAULT
	closemessage
	removeitem ITEM_HOT_COCOA          @ the cocoa is handed over
	applymovement LOCALID_MM_GIRL_L, MeltingMile_Movement_StepAside
	applymovement LOCALID_MM_GIRL_R, MeltingMile_Movement_StepAside
	applymovement LOCALID_MM_AZURILL_L, MeltingMile_Movement_StepAside
	applymovement LOCALID_MM_AZURILL_R, MeltingMile_Movement_StepAside
	waitmovement 0
	setflag FLAG_MELTINGMILE_PATH_CLEARED
	removeobject LOCALID_MM_GIRL_L
	removeobject LOCALID_MM_GIRL_R
	removeobject LOCALID_MM_AZURILL_L
	removeobject LOCALID_MM_AZURILL_R
	releaseall
	end

@==============================
@ Movements
@==============================
MeltingMile_Movement_SplashRight:
	jump_in_place_right
	step_end

MeltingMile_Movement_SplashLeft:
	jump_in_place_left
	step_end

MeltingMile_Movement_Question:
	emote_question_mark
	delay_16
	step_end

MeltingMile_Movement_StepAside:
	walk_down
	walk_down
	step_end

@==============================
@ Text
@==============================
MeltingMileText_Gentleman:
	.string "The thaw's stirring up something\nstrange on this trail.\p"
	.string "All my long years, the only POKEMON I\never met in these snows were ICE types.\p"
	.string "But now? Creatures I don't even\nrecognize are wandering up from the\llowlands where it's gone soft.\p"
	.string "Why, just this morning -- something small\nand yellow, sparks hopping off its\lcheeks. Never seen the like up here!$"

MeltingMileText_Pikachu:
	.string "The little yellow POKEMON notices you.\nIts cheeks crackle.\p"
	.string "It doesn't run... it tilts its head,\ncurious. Almost like it wants to come\lalong.$"

MeltingMileText_UseSplash:
	.string "AZURILL, use SPLASH!$"

MeltingMileText_UsedSplash:
	.string "AZURILL used SPLASH!$"

MeltingMileText_NoPP:
	.string "AZURILL has no PP left for this move!$"

MeltingMileText_GirlsLeave:
	.string "Wait... is that HOT COCOA?\pDad sent you, didn't he. Ugh, fiiine,\nwe're coming!\pCome on, AZURILL -- last one home's\na slowpoke!$"
