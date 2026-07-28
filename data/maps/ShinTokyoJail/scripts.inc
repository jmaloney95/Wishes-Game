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

ShinTokyoJail_OnTransition::
	call_if_set FLAG_ACT3_JAIL_REVEAL_SEEN, ShinTokyoJail_OnTransition_Dark
	goto_if_set FLAG_ACT3_MIKMANC_FREED, ShinTokyoJail_OnTransition_CellCheckpoint
	setrespawn HEAL_LOCATION_SHIN_TOKYO_JAIL_ENTRY
	end
ShinTokyoJail_OnTransition_CellCheckpoint::
	setrespawn HEAL_LOCATION_SHIN_TOKYO_JAIL_CELL
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
@ CANON CONFLICT (flagged for Joe): Act 2's Dojo Edwards already gives
@ ITEM_SNAG_MACHINE -- if the player has it, Mikmanc acknowledges instead.
ShinTokyoJail_EventScript_Mikmanc::
	lock
	faceplayer
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_Cell1, MSGBOX_DEFAULT
	checkitem ITEM_SNAG_MACHINE
	goto_if_eq VAR_RESULT, TRUE, ShinTokyoJail_EventScript_AlreadyHasSnag
	msgbox ShinTokyoJail_Text_CellHandover, MSGBOX_DEFAULT
	giveitem ITEM_SNAG_MACHINE
	goto ShinTokyoJail_EventScript_CellAfter

ShinTokyoJail_EventScript_AlreadyHasSnag::
	msgbox ShinTokyoJail_Text_CellSpare, MSGBOX_DEFAULT

ShinTokyoJail_EventScript_CellAfter::
	setspeaker SP_NAME_MIKMANC
	msgbox ShinTokyoJail_Text_Cell2, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_ACT3_MIKMANC_FREED
	setrespawn HEAL_LOCATION_SHIN_TOKYO_JAIL_CELL
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
	setflag FLAG_ACT3_RED_ALERT
	playse SE_EXIT
	fadescreen FADE_TO_BLACK
	warp MAP_SHIN_TOKYO, 6, 19
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
	.string "block in slow, bored arcs.\p"
	.string "Memorize it. All of it.$"

ShinTokyoJail_Text_Reveal1::
	.string "The door seals behind you.\p"
	.string "The lights die with it.$"

ShinTokyoJail_Text_Reveal2::
	.string "One flicker of emergency power --\n"
	.string "one snapshot of the room.\p"
	.string "Then nothing but the weak ring\n"
	.string "of your own lamp.\p"
	.string "The patrols are still out there.\n"
	.string "Move between them.$"

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
	.string "...{PLAYER}? You actually came.\p"
	.string "Kekeke -- sorry. Been in the dark\n"
	.string "so long I'm talking like the\l"
	.string "ghost.\p"
	.string "They tore this place apart\n"
	.string "looking for it. Never thought\l"
	.string "to search ME twice.$"

ShinTokyoJail_Text_CellHandover::
	.string "Hold out your arm.\p"
	.string "This is everything the rebellion\n"
	.string "has left. Don't lose it.$"

ShinTokyoJail_Text_CellSpare::
	.string "EDWARDS got you one already?\p"
	.string "Good. Then mine stays hidden as\n"
	.string "the backup it was meant to be.$"

ShinTokyoJail_Text_Cell2::
	.string "The west service run. Second\n"
	.string "exit, fresh goons.\p"
	.string "I'm right behind you -- we fight\n"
	.string "as one until you're out.$"

ShinTokyoJail_Text_FollowerTalk::
	.string "Right behind you.\p"
	.string "The service run first -- then\n"
	.string "the night air.$"

ShinTokyoJail_Text_GateSealed::
	.string "A reinforced shutter.\p"
	.string "It won't budge -- the release is\n"
	.string "inside the cell block.$"

ShinTokyoJail_Text_Squad1Mikmanc::
	.string "Squad ahead! Back to back,\n"
	.string "{PLAYER} -- like the gym days!$"

ShinTokyoJail_Text_Squad1Lose::
	.string "The block is sealed. You were\n"
	.string "never here.$"

ShinTokyoJail_Text_Squad1Won::
	.string "MIKMANC: One more squad between\n"
	.string "us and the night air. Go!$"

ShinTokyoJail_Text_Squad2Mikmanc::
	.string "Last one. Make it loud enough\n"
	.string "and they'll chase US, not the\l"
	.string "others!$"

ShinTokyoJail_Text_Squad2Lose::
	.string "Lights out, prison breaker.$"

ShinTokyoJail_Text_Squad2Won::
	.string "MIKMANC: The run's clear!\n"
	.string "Door at the end -- MOVE!$"

ShinTokyoJail_Text_SquadTalk::
	.string "The squad stands lamp-lit in the\n"
	.string "narrow run, listening.$"

ShinTokyoJail_Text_ExitSealed::
	.string "Bolted from the outside.$"

ShinTokyoJail_Text_MikmancFlee::
	.string "This is where we split.\p"
	.string "Alarms in thirty seconds. I go\n"
	.string "dark, you go LOUD.\p"
	.string "Thank you, {PLAYER}. For the\n"
	.string "light.$"
