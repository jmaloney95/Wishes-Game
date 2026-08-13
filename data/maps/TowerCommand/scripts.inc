	.equ LOCALID_TOWER_CAPTAIN, 1

TowerCommand_MapScripts::
	.byte 0

@ ============================================================================
@  MUTRID TOWER -- COMMAND FLOOR: the final badge, held by THE CAPTAIN who
@  took the Shadow Dossier at street level (Step 2). Recognizes the player;
@  on the kept-then-stolen path he gives thanks for the research -- his
@  Shadows are its children. Lv54-55 team. Victory: FINAL BADGE
@  (FLAG_BADGE08_GET) + the MASTER BALL (canon: the finale's snag of Shadow
@  Jirachi depends on it). The lift up refuses until the badge is won.
@  All dialogue PLACEHOLDER (Joe).
@ ============================================================================

TowerCommand_EventScript_StairsDown::
	lockall
	warp MAP_TOWER_MID, 6, 4
	waitstate
	releaseall
	end

TowerCommand_EventScript_LiftUp::
	lockall
	goto_if_set FLAG_BADGE08_GET, TowerCommand_EventScript_LiftOpen
	msgbox TowerCommand_Text_LiftSealed, MSGBOX_DEFAULT
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, TowerCommand_Movement_StepBack
	waitmovement 0
	setvar VAR_TEMP_8, 0
	releaseall
	end

TowerCommand_EventScript_LiftOpen::
	@ (8,11) arrived at the map's south lip -- too low to see THE ONI at the
	@ machine. Land mid-roof; the stairs-down triggers stay as the way back.
	warp MAP_TOWER_TOP, 8, 9
	waitstate
	releaseall
	end

TowerCommand_Movement_StepBack:
	walk_down
	step_end

TowerCommand_EventScript_Captain::
	lock
	faceplayer
	goto_if_defeated TRAINER_KATHLEEN, TowerCommand_EventScript_CaptainPost
	setspeaker SP_NAME_MUTRID_CAPTAIN
	msgbox TowerCommand_Text_Captain1, MSGBOX_DEFAULT
	goto_if_unset FLAG_ACT3_RESEARCH_STOLEN, TowerCommand_EventScript_CaptainNoResearch
	msgbox TowerCommand_Text_CaptainThanks, MSGBOX_DEFAULT
	goto TowerCommand_EventScript_CaptainChallenge
TowerCommand_EventScript_CaptainNoResearch::
	msgbox TowerCommand_Text_CaptainNoResearch, MSGBOX_DEFAULT
TowerCommand_EventScript_CaptainChallenge::
	msgbox TowerCommand_Text_CaptainChallenge, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_KATHLEEN, TowerCommand_Text_CaptainDefeat
	setspeaker SP_NAME_MUTRID_CAPTAIN
	msgbox TowerCommand_Text_CaptainConcede, MSGBOX_DEFAULT
	closemessage
	playfanfare MUS_OBTAIN_BADGE
	msgbox TowerCommand_Text_ReceivedBadge, MSGBOX_DEFAULT
	waitfanfare
	setflag FLAG_BADGE08_GET
	setspeaker SP_NAME_MUTRID_CAPTAIN
	msgbox TowerCommand_Text_CaptainMasterBall, MSGBOX_DEFAULT
	giveitem ITEM_MASTER_BALL
	msgbox TowerCommand_Text_CaptainAfter, MSGBOX_DEFAULT
	release
	end

TowerCommand_EventScript_CaptainPost::
	setspeaker SP_NAME_MUTRID_CAPTAIN
	msgbox TowerCommand_Text_CaptainRematch, MSGBOX_DEFAULT
	release
	end

@ --- Text (all PLACEHOLDER (Joe)) -------------------------------------------
TowerCommand_Text_Captain1::
	.string "...The compliant citizen from\n"
	.string "the street inspection.\p"
	.string "Arms out. Bag open. You held\n"
	.string "so STILL.\p"
	.string "And now you stand on my floor\n"
	.string "wearing a bounty instead of a\l"
	.string "mask.$"

TowerCommand_Text_CaptainThanks::
	.string "I never thanked you properly.\p"
	.string "The professor's research --\n"
	.string "hand-delivered into my gloves.\p"
	.string "Every Shadow bred on the floor\n"
	.string "below us is its child.\p"
	.string "You will be fighting his\n"
	.string "life's work. And mine.$"

TowerCommand_Text_CaptainNoResearch::
	.string "You kept the professor's\n"
	.string "secrets from us once.\p"
	.string "It slowed the tide. It did not\n"
	.string "turn it.$"

TowerCommand_Text_CaptainChallenge::
	.string "The tower trusted me with its\n"
	.string "final badge.\p"
	.string "Take it from me -- or feed the\n"
	.string "machine. Those are the doors.$"

TowerCommand_Text_CaptainDefeat::
	.string "...By right of conquest,\n"
	.string "then.$"

TowerCommand_Text_CaptainConcede::
	.string "Held. Truly held.\p"
	.string "The tower keeps its own law:\n"
	.string "what beats the holder, holds.$"

TowerCommand_Text_ReceivedBadge::
	.string "{PLAYER} earned the TOWER\n"
	.string "BADGE -- the final badge!$"

TowerCommand_Text_CaptainMasterBall::
	.string "...And this. The tower's\n"
	.string "failsafe. The ball that cannot\l"
	.string "fail.\p"
	.string "I was ordered to hold it\n"
	.string "forever.\p"
	.string "I want to watch what you do\n"
	.string "with it instead.$"

TowerCommand_Text_CaptainAfter::
	.string "The lift is open.\p"
	.string "He is waiting up there beside\n"
	.string "his machine.\p"
	.string "He is always waiting.$"

TowerCommand_Text_CaptainRematch::
	.string "The badge holds. The lift is\n"
	.string "open.\p"
	.string "What waits above is no longer\n"
	.string "my law.$"

TowerCommand_Text_LiftSealed::
	.string "The executive lift reads your\n"
	.string "badge case -- and refuses.\p"
	.string "MUTRID ALPHA's final badge is\n"
	.string "the only key it honors.$"
