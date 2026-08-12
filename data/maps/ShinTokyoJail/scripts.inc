	.equ LOCALID_JAIL_MIKMANC, 4

ShinTokyoJail_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ShinTokyoJail_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, ShinTokyoJail_OnFrame
	.byte 0

@ ============================================================================
@  ACT 3, STEP 4 -- THE DARK JAILBREAK (WoT_Act3_Canon.md)
@  Staging of the canon set-piece:
@   - Darkness v2 = cave-style flash darkness (level 7: black screen, ~1.5
@     tile light ring around the player). First entry loads LIT -- the block
@     and its patrols are visible -- then the entry scene kills the lights.
@     Every later load re-darkens via ON_TRANSITION (SetDefaultFlashLevel
@     zeroes the level on each warp because this map is not a "cave").
@     Still TODO from the full canon spec: guard-carried light sprites +
@     radius-based detection. Patrols are sight trainers: getting spotted
@     means a hard fight; losing IS the canon "whiteout -> checkpoint" loop.
@   - Checkpoints via heal locations: whiteout respawns at the map entry
@     until Mikmanc is freed, then at the cell block (ON_TRANSITION below).
@   - Phase 3 escort: Mikmanc walks BEHIND the player (follower NPC system)
@     from the cell to the maintenance exit, and fights beside them as
@     PARTNER_MIKMANC (multi_2_vs_1) against the two service-run squads.
@  All dialogue PLACEHOLDER (Joe).
@ ============================================================================

@ Darkness only holds until the break-out is discovered: once Mikmanc is free
@ the block is at RED ALERT with its lights forced back on.
ShinTokyoJail_OnTransition::
	goto_if_set FLAG_ACT3_MIKMANC_FREED, ShinTokyoJail_OnTransition_CellCheckpoint
	call_if_set FLAG_ACT3_JAIL_REVEAL_SEEN, ShinTokyoJail_OnTransition_Dark
	setrespawn HEAL_LOCATION_SHIN_TOKYO_JAIL_ENTRY
	end
ShinTokyoJail_OnTransition_CellCheckpoint::
	@ Once the city is at red alert, a jail re-entry must NOT re-arm the
	@ stale cell checkpoint (that overwrote the hideout respawn and caused
	@ the whiteout-into-guards loop). Hideout is the checkpoint from then on.
	goto_if_set FLAG_ACT3_RED_ALERT, ShinTokyoJail_OnTransition_HideoutCheckpoint
	setrespawn HEAL_LOCATION_SHIN_TOKYO_JAIL_CELL
	end
ShinTokyoJail_OnTransition_HideoutCheckpoint::
	setrespawn HEAL_LOCATION_REBEL_HIDEOUT
	end
@ The scanline window itself is armed later in the map load
@ (InitCurrentFlashLevelScanlineEffect) -- setting the level here is enough.
ShinTokyoJail_OnTransition_Dark::
	setflashlevel 7
	return

ShinTokyoJail_OnFrame:
	map_script_2 VAR_TEMP_1, 0, ShinTokyoJail_EventScript_EntryReveal
	.2byte 0

@ Canon: the block is seen ONCE in full light -- one mental snapshot to plan
@ from -- then the lights die for good. v2: the map loads lit, the scene
@ blacks out the screen, arms the flash-level-7 scanline ring (via the
@ WotInitFlashScanlineEffect special) and fades back into the darkness, with
@ one white emergency-power flicker as the snapshot beat.
ShinTokyoJail_EventScript_EntryReveal::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_set FLAG_ACT3_JAIL_REVEAL_SEEN, ShinTokyoJail_EventScript_EntryRevealDone
	setflag FLAG_ACT3_JAIL_REVEAL_SEEN
	@ Repurposed vanilla trainer ids must start undefeated (their old gyms
	@ could never set the flags, but this is free insurance).
	cleartrainerflag TRAINER_GRUNT_UNUSED
	cleartrainerflag TRAINER_BRENDAN_PLACEHOLDER
	cleartrainerflag TRAINER_MAY_PLACEHOLDER
	cleartrainerflag TRAINER_PRESTON
	cleartrainerflag TRAINER_VIRGIL
	@ Same insurance for the ids repurposed downstream of the jailbreak:
	@ the red-alert patrols and the Celebi Island keepers.
	cleartrainerflag TRAINER_NATE
	cleartrainerflag TRAINER_MACEY
	cleartrainerflag TRAINER_CLIFFORD
	cleartrainerflag TRAINER_HANNAH
	cleartrainerflag TRAINER_SAMANTHA
	cleartrainerflag TRAINER_MAURA
	cleartrainerflag TRAINER_BLAKE
	cleartrainerflag TRAINER_KAYLA
	cleartrainerflag TRAINER_SYLVIA
	cleartrainerflag TRAINER_NICHOLAS
	cleartrainerflag TRAINER_KATHLEEN
	@ A beat under the working lights: the room and its patrols, visible.
	delay 40
	msgbox ShinTokyoJail_Text_Reveal0, MSGBOX_DEFAULT
	closemessage
	delay 30
	playse SE_PC_OFF
	msgbox ShinTokyoJail_Text_Reveal1, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_BLACK
	setflashlevel 7
	special WotInitFlashScanlineEffect
	delay 20
	fadescreen FADE_FROM_BLACK
	delay 20
	@ One flicker of emergency power -- the snapshot.
	fadescreen FADE_TO_WHITE
	delay 12
	fadescreen FADE_FROM_WHITE
	delay 8
	msgbox ShinTokyoJail_Text_Reveal2, MSGBOX_DEFAULT
	closemessage
ShinTokyoJail_EventScript_EntryRevealDone::
	releaseall
	end

@ --- Phase 1: patrols ------------------------------------------------------
ShinTokyoJail_EventScript_GuardA::
	trainerbattle_single TRAINER_GRUNT_UNUSED, ShinTokyoJail_Text_GuardAIntro, ShinTokyoJail_Text_GuardADefeat
	msgbox ShinTokyoJail_Text_GuardAPost, MSGBOX_AUTOCLOSE
	end

ShinTokyoJail_EventScript_GuardB::
	trainerbattle_single TRAINER_BRENDAN_PLACEHOLDER, ShinTokyoJail_Text_GuardBIntro, ShinTokyoJail_Text_GuardBDefeat
	msgbox ShinTokyoJail_Text_GuardBPost, MSGBOX_AUTOCLOSE
	end

ShinTokyoJail_EventScript_GuardC::
	trainerbattle_single TRAINER_MAY_PLACEHOLDER, ShinTokyoJail_Text_GuardCIntro, ShinTokyoJail_Text_GuardCDefeat
	msgbox ShinTokyoJail_Text_GuardCPost, MSGBOX_AUTOCLOSE
	end

@ --- Phase 2: the cell -----------------------------------------------------
@ Canon: the Snag Machine is handed over HERE so it can never be lost.
@ Mikmanc STOLE it from Mutrid's own lab on his way into the cell -- their
@ weapon, turned around on them. Handover first, then the explainer.
ShinTokyoJail_EventScript_Mikmanc::
	lock
	faceplayer
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_Cell1, MSGBOX_DEFAULT
	checkitem ITEM_SNAG_MACHINE
	goto_if_eq VAR_RESULT, TRUE, ShinTokyoJail_EventScript_AlreadyHasSnag
	msgbox ShinTokyoJail_Text_CellHandover, MSGBOX_DEFAULT
	hideportrait
	giveitem ITEM_SNAG_MACHINE
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_CellExplain, MSGBOX_DEFAULT
	goto ShinTokyoJail_EventScript_CellAfter

ShinTokyoJail_EventScript_AlreadyHasSnag::
	msgbox ShinTokyoJail_Text_CellSpare, MSGBOX_DEFAULT

ShinTokyoJail_EventScript_CellAfter::
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_Cell2, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_ACT3_MIKMANC_FREED
	setrespawn HEAL_LOCATION_SHIN_TOKYO_JAIL_CELL
	@ -- THE BLOCK WAKES UP --
	@ Emptied cell = discovered. The lights slam back on, the way you came
	@ seals itself, and the klaxon starts. (Music pass later: Joe wants a
	@ Metal-Gear-style track here -- add MUS_* + `playbgm` once imported.)
	delay 20
	playse SE_BANG
	delay 25
	@ lights up: the flash darkness animates away and stays off (the
	@ ON_TRANSITION dark branch is skipped from here on)
	animateflash 0
	setflashlevel 0
	msgbox ShinTokyoJail_Text_RedAlert1, MSGBOX_DEFAULT
	closemessage
	playse SE_SLIDING_DOOR
	delay 30
	special WotRedAlertFlash
	waitstate
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_RedAlert2, MSGBOX_DEFAULT
	closemessage
	startquest SIDE_QUEST_HARNESS_DARKNESS
	@ The escort proper: Mikmanc falls in behind the player (DPP-style
	@ follower NPC). Flags 0 = no bike/surf/fly while escorting, and the
	@ follower SURVIVES a whiteout -- the cell-checkpoint respawn keeps him
	@ at your back. The squad doubles partner him battle-side via
	@ PARTNER_MIKMANC, so no battlePartner is set here.
	setfollowernpc LOCALID_JAIL_MIKMANC, 0, ShinTokyoJail_EventScript_MikmancFollowerTalk
	release
	end

ShinTokyoJail_EventScript_MikmancFollowerTalk::
	lock
	faceplayer
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_FollowerTalk, MSGBOX_DEFAULT
	release
	end

@ --- The shiv inmate (cell beside Mikmanc's) --------------------------------
@ One gift, then he bolts down the block and is gone for good.
ShinTokyoJail_EventScript_Inmate::
	lock
	faceplayer
	msgbox ShinTokyoJail_Text_Inmate1, MSGBOX_DEFAULT
	giveitem ITEM_QUICK_CLAW
	msgbox ShinTokyoJail_Text_Inmate2, MSGBOX_DEFAULT
	closemessage
	applymovement VAR_LAST_TALKED, ShinTokyoJail_Movement_InmateFlee
	waitmovement 0
	playse SE_EXIT
	setflag FLAG_WOT_INMATE_GONE
	removeobject VAR_LAST_TALKED
	release
	end

ShinTokyoJail_Movement_InmateFlee:
	walk_down
	walk_down
	walk_right
	walk_fast_down
	walk_fast_down
	step_end

@ --- Service gate between the cell block and the west run ------------------
ShinTokyoJail_EventScript_ServiceGate::
	lockall
	setvar VAR_TEMP_4, 1
	goto_if_set FLAG_ACT3_MIKMANC_FREED, ShinTokyoJail_EventScript_ServiceGateOpen
	msgbox ShinTokyoJail_Text_GateSealed, MSGBOX_DEFAULT
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, ShinTokyoJail_Movement_StepBackUp
	waitmovement 0
	setvar VAR_TEMP_4, 0
ShinTokyoJail_EventScript_ServiceGateOpen::
	releaseall
	end

ShinTokyoJail_Movement_StepBackUp:
	walk_up
	step_end

@ --- Phase 3: escort doubles (partner Mikmanc) -----------------------------
ShinTokyoJail_EventScript_Squad1::
	lockall
	setvar VAR_TEMP_5, 1
	goto_if_defeated TRAINER_PRESTON, ShinTokyoJail_EventScript_SquadPass
	goto_if_unset FLAG_ACT3_MIKMANC_FREED, ShinTokyoJail_EventScript_SquadPass
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_Squad1Mikmanc, MSGBOX_DEFAULT
	closemessage
ShinTokyoJail_EventScript_Squad1Choose::
	@ The half-party selection is MANDATORY before any partner multi --
	@ multi_2_vs_1 reduces gPlayerParty to the selected 3 (the partner's team
	@ borrows slots 3-5), so skipping it fields an empty Lv0 side.
	special SavePlayerParty
	fadescreen FADE_TO_BLACK
	special ChooseHalfPartyForBattle
	goto_if_ne VAR_RESULT, 0, ShinTokyoJail_EventScript_Squad1Battle
	special LoadPlayerParty
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_SquadMustPick, MSGBOX_DEFAULT
	closemessage
	goto ShinTokyoJail_EventScript_Squad1Choose
ShinTokyoJail_EventScript_Squad1Battle::
	multi_2_vs_1 TRAINER_PRESTON, ShinTokyoJail_Text_Squad1Lose, PARTNER_MIKMANC
	switch VAR_RESULT
	case 1, ShinTokyoJail_EventScript_Squad1Won
	fadescreen FADE_TO_BLACK
	@ (registered waitstate=1 -- the whiteout takes over from here)
	special SetCB2WhiteOut
	releaseall
	end

ShinTokyoJail_EventScript_Squad1Won::
	settrainerflag TRAINER_PRESTON
	msgbox ShinTokyoJail_Text_Squad1Won, MSGBOX_DEFAULT
	call Wot_EventScript_SnagProgress
	releaseall
	end

ShinTokyoJail_EventScript_Squad2::
	lockall
	setvar VAR_TEMP_6, 1
	goto_if_defeated TRAINER_VIRGIL, ShinTokyoJail_EventScript_SquadPass
	goto_if_unset FLAG_ACT3_MIKMANC_FREED, ShinTokyoJail_EventScript_SquadPass
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_Squad2Mikmanc, MSGBOX_DEFAULT
	closemessage
ShinTokyoJail_EventScript_Squad2Choose::
	special SavePlayerParty
	fadescreen FADE_TO_BLACK
	special ChooseHalfPartyForBattle
	goto_if_ne VAR_RESULT, 0, ShinTokyoJail_EventScript_Squad2Battle
	special LoadPlayerParty
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_SquadMustPick, MSGBOX_DEFAULT
	closemessage
	goto ShinTokyoJail_EventScript_Squad2Choose
ShinTokyoJail_EventScript_Squad2Battle::
	multi_2_vs_1 TRAINER_VIRGIL, ShinTokyoJail_Text_Squad2Lose, PARTNER_MIKMANC
	switch VAR_RESULT
	case 1, ShinTokyoJail_EventScript_Squad2Won
	fadescreen FADE_TO_BLACK
	@ (registered waitstate=1 -- the whiteout takes over from here)
	special SetCB2WhiteOut
	releaseall
	end

ShinTokyoJail_EventScript_Squad2Won::
	settrainerflag TRAINER_VIRGIL
	msgbox ShinTokyoJail_Text_Squad2Won, MSGBOX_DEFAULT
	call Wot_EventScript_SnagProgress
	releaseall
	end

ShinTokyoJail_EventScript_SquadPass::
	releaseall
	end

@ The posted squad members are talk-only bodies; their battles fire from the
@ trigger rows so Mikmanc can join as partner.
ShinTokyoJail_EventScript_Squad1Talk::
	msgbox ShinTokyoJail_Text_SquadTalk, MSGBOX_NPC
	end

ShinTokyoJail_EventScript_Squad2Talk::
	msgbox ShinTokyoJail_Text_SquadTalk, MSGBOX_NPC
	end

@ --- Doors -----------------------------------------------------------------
@ (The front door is a plain map warp at (12,19) back to the district; the
@  old scripted front door was retired with it.)

@ Red alert seals the front door: from the moment Mikmanc is out, the only way
@ on is forward through the west service run. The bounce sits on the approach
@ tile (12,18) rather than the warp tile itself, so the warp can never fire.
ShinTokyoJail_EventScript_FrontDoorBlock::
	lockall
	setvar VAR_TEMP_9, 1
	goto_if_unset FLAG_ACT3_MIKMANC_FREED, ShinTokyoJail_EventScript_FrontDoorBlockDone
	playse SE_WALL_HIT
	msgbox ShinTokyoJail_Text_FrontDoorSealed, MSGBOX_DEFAULT
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, ShinTokyoJail_Movement_StepBackUp
	waitmovement 0
ShinTokyoJail_EventScript_FrontDoorBlockDone::
	setvar VAR_TEMP_9, 0
	releaseall
	end

ShinTokyoJail_EventScript_MaintenanceExit::
	lockall
	goto_if_set FLAG_ACT3_MIKMANC_FREED, ShinTokyoJail_EventScript_Escape
	msgbox ShinTokyoJail_Text_ExitSealed, MSGBOX_DEFAULT
	releaseall
	end

@ Stepping out the maintenance run ends the heist: Mikmanc splits off to
@ safety, and the city goes to RED ALERT (Step 5 -- permanent).
ShinTokyoJail_EventScript_Escape::
	checkfollowernpc
	goto_if_eq VAR_RESULT, FALSE, ShinTokyoJail_EventScript_EscapeNoFollower
	facefollowernpc
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_MikmancFlee, MSGBOX_DEFAULT
	closemessage
	@ He runs back up the service corridor -- two steps and the darkness
	@ swallows him (the light ring only reaches about a tile and a half).
	applymovement OBJ_EVENT_ID_NPC_FOLLOWER, ShinTokyoJail_Movement_MikmancFlee
	waitmovement OBJ_EVENT_ID_NPC_FOLLOWER
	destroyfollowernpc
	goto ShinTokyoJail_EventScript_EscapeGo

ShinTokyoJail_EventScript_EscapeNoFollower::
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_MikmancFlee, MSGBOX_DEFAULT
	closemessage

ShinTokyoJail_EventScript_EscapeGo::
	@ Clearing the jail (first successful exit) locks in the JAILBREAKER
	@ milestone -- the Minister on the docks pays out from here.
	call_if_unset FLAG_ACT3_RED_ALERT, ShinTokyoJail_EventScript_JailbreakerClear
	setflag FLAG_ACT3_RED_ALERT
	playse SE_EXIT
	fadescreen FADE_TO_BLACK
	warp MAP_SHIN_TOKYO, 28, 41
	waitstate
	releaseall
	end

ShinTokyoJail_Movement_MikmancFlee:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

@ --- Text (all PLACEHOLDER (Joe)) -------------------------------------------
ShinTokyoJail_Text_Reveal0::
	.string "Fluorescent light, humming.\p"
	.string "Three patrols sweep the cell\n"
	.string "block.$"

ShinTokyoJail_Text_Reveal1::
	.string "The door seals behind you.\p"
	.string "The lights die with it.$"

ShinTokyoJail_Text_Reveal2::
	.string "One flash of emergency power --\n"
	.string "then nothing.\p"
	.string "Only a ring of light around\n"
	.string "your feet.$"

ShinTokyoJail_Text_GuardAIntro::
	.string "A light snaps onto your face.\p"
	.string "Movement in C-block!$"

ShinTokyoJail_Text_GuardADefeat::
	.string "Report... report it in...$"

ShinTokyoJail_Text_GuardAPost::
	.string "The lamp rolls on the floor,\n"
	.string "spinning shadows.$"

ShinTokyoJail_Text_GuardBIntro::
	.string "Who cut the lights back here?!\p"
	.string "...You're no technician.$"

ShinTokyoJail_Text_GuardBDefeat::
	.string "The dark took my aim...$"

ShinTokyoJail_Text_GuardBPost::
	.string "She fumbles for a radio that\n"
	.string "isn't there anymore.$"

ShinTokyoJail_Text_GuardCIntro::
	.string "Gate post. Nobody passes.\p"
	.string "NOBODY.$"

ShinTokyoJail_Text_GuardCDefeat::
	.string "...The gate... is open...$"

ShinTokyoJail_Text_GuardCPost::
	.string "He slumps against the cell-block\n"
	.string "gate he was guarding.$"

ShinTokyoJail_Text_Cell1::
	.string "...{PLAYER}? Thank Arceus,\n"
	.string "I don't have to murder that guy\l"
	.string "in the next cell.\p"
	.string "Oh wait! -- Check this out.\n"
	.string "I rolled a nat 20 and swiped\l"
	.string "this from a Mutrid Officer.$"

ShinTokyoJail_Text_CellHandover::
	.string "Hold out your arm.$"

ShinTokyoJail_Text_CellExplain::
	.string "The SNAG MACHINE. Mutrid tech.\n"
	.string "Top of the line.\p"
	.string "They fight with\n"
	.string "SHADOW POKéMON -- stolen souls,\l"
	.string "sealed shut.\p"
	.string "With that on your arm, any\n"
	.string "BALL you throw can rip a\l"
	.string "SHADOW right off their team.\p"
	.string "It only answers to Shadows.\n"
	.string "Ordinary trainers are safe.$"

ShinTokyoJail_Text_CellSpare::
	.string "Huh. You've already got one\n"
	.string "on your arm.\p"
	.string "Nat 1 for my big reveal.$"

ShinTokyoJail_Text_Cell2::
	.string "Now. Let's head to the West\n"
	.string "service exit, I've got a plan.\p"
	.string "I'm right behind you -- we fight\n"
	.string "as one.$"

ShinTokyoJail_Text_FollowerTalk::
	.string "Right behind you.\p"
	.string "Let's double team these goons.\n"
	.string "...Oof. You get what I mean.$"

ShinTokyoJail_Text_Inmate1::
	.string "Psst. You're breaking out?\p"
	.string "I made a shiv to help us\n"
	.string "escape. Here -- take one.$"

ShinTokyoJail_Text_Inmate2::
	.string "Strikes first, every time.\n"
	.string "Good luck, kid. I'm GONE.$"

ShinTokyoJail_Text_RedAlert1::
	.string "Somewhere above, a bolt SLAMS.$"

ShinTokyoJail_Text_RedAlert2::
	.string "We've tripped the alarms.\p"
	.string "The front door is armed, {PLAYER}.\n"
	.string "West service run -- GO!$"

ShinTokyoJail_Text_FrontDoorSealed::
	.string "The front door is a slab of\n"
	.string "blast steel now.\p"
	.string "Nothing leaves the way it came\n"
	.string "in.$"

ShinTokyoJail_Text_GateSealed::
	.string "A reinforced shutter.\p"
	.string "It won't budge -- the release is\n"
	.string "inside the cell block.$"

ShinTokyoJail_Text_SquadMustPick::
	.string "No time to hesitate!\p"
	.string "Pick your three -- I'll cover\n"
	.string "the rest!$"

ShinTokyoJail_Text_Squad1Mikmanc::
	.string "Squad ahead -- and they've got\n"
	.string "Shadows with them!\p"
	.string "Here's our chance, {PLAYER}!\p"
	.string "Use the SNAG MACHINE -- capture\n"
	.string "their SHADOW POKéMON!$"

ShinTokyoJail_Text_Squad1Lose::
	.string "The block is sealed. You were\n"
	.string "never here.$"

ShinTokyoJail_Text_Squad1Won::
	.string "One more squad between\n"
	.string "us and the night air. Go!$"

ShinTokyoJail_Text_Squad2Mikmanc::
	.string "Last one. I'm going plus-Ultra.$"

ShinTokyoJail_Text_Squad2Lose::
	.string "Lights out, prison breaker.$"

ShinTokyoJail_Text_Squad2Won::
	.string "The exit's clear!$"

ShinTokyoJail_Text_SquadTalk::
	.string "The squad stands lamp-lit in the\n"
	.string "narrow run, listening.$"

ShinTokyoJail_Text_ExitSealed::
	.string "Bolted from the outside.$"

ShinTokyoJail_Text_MikmancFlee::
	.string "This is where we split.\p"
	.string "Pints on me when we get home.\n"
	.string "Thanks for the escort,\n"
	.string "{PLAYER}.$"

ShinTokyoJail_EventScript_JailbreakerClear::
	questmenu QUEST_MENU_SET_REWARD, SIDE_QUEST_MINISTERS_FAVOR
	questtoast QUEST_TOAST_UPDATE, SIDE_QUEST_MINISTERS_FAVOR
	return
