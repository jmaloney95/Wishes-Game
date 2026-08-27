	.equ LOCALID_ISLAND_CELEBI, 1
	.equ LOCALID_ISLAND_UXIE, 5
	.equ LOCALID_ISLAND_MESPRIT, 6
	.equ LOCALID_ISLAND_AZELF, 7

	.equ LOCALID_ISLAND_CELEBI, 1

CelebiIsland_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, CelebiIsland_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, CelebiIsland_OnFrame
	.byte 0

@ The lake trio guards the shrine until the keepers' trial is done; then
@ they hold the island's ends forever and CELEBI appears at the shrine.
CelebiIsland_OnTransition::
	goto_if_set FLAG_WOT_FAIRY_TRIAL_DONE, CelebiIsland_OnTransition_Watch
	setflag FLAG_HIDE_WOT_CELEBI
	end
CelebiIsland_OnTransition_Watch::
	clearflag FLAG_HIDE_WOT_CELEBI
	setobjectxyperm LOCALID_ISLAND_UXIE, 4, 33
	setobjectxyperm LOCALID_ISLAND_MESPRIT, 25, 41
	setobjectxyperm LOCALID_ISLAND_AZELF, 46, 33
	end

@ First arrival = the first checkpoint of the Stop the Machine main quest.
CelebiIsland_OnFrame:
	map_script_2 VAR_TEMP_0, 0, CelebiIsland_EventScript_ArrivalCheckpoint
	.2byte 0

CelebiIsland_EventScript_ArrivalCheckpoint::
	lockall
	setvar VAR_TEMP_0, 1
	goto_if_set FLAG_WOT_ISLAND_CHECKPOINT, CelebiIsland_EventScript_ArrivalCheckpointDone
	questmenu QUEST_MENU_CHECK_ACTIVE, QUEST_STOP_THE_MACHINE
	goto_if_eq VAR_RESULT, FALSE, CelebiIsland_EventScript_ArrivalCheckpointDone
	setflag FLAG_WOT_ISLAND_CHECKPOINT
	questtoast QUEST_TOAST_UPDATE, QUEST_STOP_THE_MACHINE
	msgbox CelebiIsland_Text_Checkpoint, MSGBOX_DEFAULT
	closemessage
CelebiIsland_EventScript_ArrivalCheckpointDone::
	releaseall
	end

@ Surfing off the island's south edge returns to the city's tide waters.
@ North (toward the city) now crosses Route 129's open sea. The old direct
@ warp target (19,28) was STALE -- a solid city tile in the repainted layout.
CelebiIsland_EventScript_SurfToCity::
	lockall
	@ (40,37) dropped the surfer outside Route 129's playable water --
	@ (78,20) is the open sea off the island.
	warp MAP_ROUTE129, 78, 20
	waitstate
	releaseall
	end

@ Surfing off the west edge crosses to Prototype Island's waters.
@ NORTH now, not west: Prototype Island sits directly north of the island.
CelebiIsland_EventScript_SurfToPrototype::
	lockall
	warp MAP_PROTOTYPE_ISLAND, 11, 26
	waitstate
	releaseall
	end

@ ============================================================================
@  CELEBI ISLAND -- THE PURIFICATION SHRINE (WoT_Act3_Canon.md §2.2)
@  First archipelago map: a small sacred island in the raised tides. Three
@  fairy keepers test visitors; their warden holds the only bridge to the
@  shrine islet until all three accept you (FLAG_WOT_FAIRY_TRIAL_DONE).
@  CELEBI itself performs purification: snag -> battle once ("opened") ->
@  present the mon here. Purify loop: choose a party mon, route on
@  GetMonShadowState (0 no shadow / 1 sealed / 2 ready), PurifyPartyMon.
@  NOT YET WIRED TO THE WORLD -- entry comes with Step 7 (tides drop ->
@  Surf opens the south water). Reach via debug warp for testing.
@  All dialogue PLACEHOLDER (Joe).
@ ============================================================================

@ --- The keepers (sight trainers; repurposed Mossdeep orphan ids) -----------
CelebiIsland_EventScript_KeeperSuzu::
	trainerbattle_single TRAINER_HANNAH, CelebiIsland_Text_SuzuIntro, CelebiIsland_Text_SuzuDefeat
	msgbox CelebiIsland_Text_SuzuPost, MSGBOX_AUTOCLOSE
	end

CelebiIsland_EventScript_KeeperKaede::
	trainerbattle_single TRAINER_SAMANTHA, CelebiIsland_Text_KaedeIntro, CelebiIsland_Text_KaedeDefeat
	msgbox CelebiIsland_Text_KaedePost, MSGBOX_AUTOCLOSE
	end

CelebiIsland_EventScript_KeeperRan::
	trainerbattle_single TRAINER_MAURA, CelebiIsland_Text_RanIntro, CelebiIsland_Text_RanDefeat
	msgbox CelebiIsland_Text_RanPost, MSGBOX_AUTOCLOSE
	end

@ --- CELEBI: the purification -----------------------------------------------
CelebiIsland_EventScript_Celebi::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_CELEBI, CRY_MODE_NORMAL
	waitmoncry
	msgbox CelebiIsland_Text_CelebiGreet, MSGBOX_DEFAULT
	special CountShadowPartyMons
	goto_if_eq VAR_RESULT, 0, CelebiIsland_EventScript_NoShadows

CelebiIsland_EventScript_ChooseMon::
	msgbox CelebiIsland_Text_Present, MSGBOX_DEFAULT
	special ChoosePartyMon
	waitstate
	goto_if_ge VAR_0x8004, PARTY_SIZE, CelebiIsland_EventScript_Farewell
	special GetMonShadowState
	bufferpartymonnick STR_VAR_1, VAR_0x8004
	switch VAR_RESULT
	case 0, CelebiIsland_EventScript_NotShadow
	case 1, CelebiIsland_EventScript_NotOpened
	@ -- state 2: opened and ready --
	msgbox CelebiIsland_Text_Ready, MSGBOX_DEFAULT
	closemessage
	fadescreen FADE_TO_WHITE
	playfanfare MUS_HEAL
	special PurifyPartyMon
	delay 40
	fadescreen FADE_FROM_WHITE
	waitfanfare
	waitse
	playmoncry SPECIES_CELEBI, CRY_MODE_HIGH_PITCH
	waitmoncry
	msgbox CelebiIsland_Text_Purified, MSGBOX_DEFAULT
	special CountShadowPartyMons
	goto_if_eq VAR_RESULT, 0, CelebiIsland_EventScript_AllClear
	msgbox CelebiIsland_Text_Another, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, YES, CelebiIsland_EventScript_ChooseMon
	goto CelebiIsland_EventScript_Farewell

CelebiIsland_EventScript_NotShadow::
	msgbox CelebiIsland_Text_NotShadow, MSGBOX_DEFAULT
	goto CelebiIsland_EventScript_ChooseMon

CelebiIsland_EventScript_NotOpened::
	msgbox CelebiIsland_Text_NotOpened, MSGBOX_DEFAULT
	goto CelebiIsland_EventScript_ChooseMon

CelebiIsland_EventScript_NoShadows::
	msgbox CelebiIsland_Text_NoShadows, MSGBOX_DEFAULT
	release
	end

CelebiIsland_EventScript_AllClear::
	msgbox CelebiIsland_Text_AllClear, MSGBOX_DEFAULT
	release
	end

CelebiIsland_EventScript_Farewell::
	msgbox CelebiIsland_Text_Farewell, MSGBOX_DEFAULT
	release
	end

@ --- Text (all PLACEHOLDER (Joe)) -------------------------------------------
CelebiIsland_Text_SuzuIntro:
	.string "The tide let you through?\p"
	.string "Then the island is already\n"
	.string "curious about you.\p"
	.string "I am not. Prove yourself!$"

CelebiIsland_Text_SuzuDefeat:
	.string "...Gentle AND strong.$"

CelebiIsland_Text_SuzuPost:
	.string "The keepers watch the bridge.\n"
	.string "All three of us must agree.$"

CelebiIsland_Text_KaedeIntro:
	.string "The city across the water\n"
	.string "makes shadows.\p"
	.string "This island unmakes them.\n"
	.string "Show me which side you feed!$"

CelebiIsland_Text_KaedeDefeat:
	.string "Warm. Your light is warm.$"

CelebiIsland_Text_KaedePost:
	.string "Bring the poor twisted ones\n"
	.string "here. All of them.$"

CelebiIsland_Text_RanIntro:
	.string "Kekeke... you smell of the\n"
	.string "dark city.\p"
	.string "But underneath that...\n"
	.string "something older. Show me!$"

CelebiIsland_Text_RanDefeat:
	.string "Yes... THAT. That is it.$"

CelebiIsland_Text_RanPost:
	.string "The old man on the bridge is\n"
	.string "softer than he looks.$"

CelebiIsland_Text_WardenNotYet:
	.string "This bridge bears only those\n"
	.string "the keepers accept.\p"
	.string "Three tests. All three.\n"
	.string "Then the shrine will see you.$"

CelebiIsland_Text_WardenOpens:
	.string "All three keepers bow to you.\p"
	.string "Then it is not my place to\n"
	.string "stand here.\p"
	.string "Cross, child. What waits on\n"
	.string "the islet has waited long.$"

CelebiIsland_Text_CelebiGreet:
	.string "The air goes still.\p"
	.string "CELEBI hovers above the dais,\n"
	.string "and time leans in to listen.$"

CelebiIsland_Text_NoShadows:
	.string "It circles your team once,\n"
	.string "twice.\p"
	.string "No shadow clings to any of\n"
	.string "them. CELEBI trills, relieved.$"

CelebiIsland_Text_Present:
	.string "CELEBI opens its small hands,\n"
	.string "palms full of light.\p"
	.string "Present a POKéMON.$"

CelebiIsland_Text_NotShadow:
	.string "{STR_VAR_1} carries no shadow.\p"
	.string "CELEBI pats it on the head\n"
	.string "anyway.$"

CelebiIsland_Text_NotOpened:
	.string "The shadow around {STR_VAR_1}\n"
	.string "is still sealed shut.\p"
	.string "Walk with it. Fight beside it,\n"
	.string "even once. Then return.$"

CelebiIsland_Text_Ready:
	.string "CELEBI cups {STR_VAR_1}'s face\n"
	.string "in its hands.\p"
	.string "The shadow strains-- light\n"
	.string "gathers--$"

CelebiIsland_Text_Purified:
	.string "The shadow lifts!\p"
	.string "{STR_VAR_1} came back to\n"
	.string "itself!\p"
	.string "...A NATIONAL RIBBON settles\n"
	.string "quietly among its things.$"

CelebiIsland_Text_Another:
	.string "The dark still clings to\n"
	.string "another of yours.\p"
	.string "Present it as well?$"

CelebiIsland_Text_AllClear:
	.string "The last shadow on your team\n"
	.string "is gone.\p"
	.string "CELEBI spins once in the air,\n"
	.string "bright as morning.$"

CelebiIsland_Text_Farewell:
	.string "CELEBI settles back above the\n"
	.string "dais, patient as the forest.$"

CelebiIsland_Text_Checkpoint:
	.string "The island the fairies keep.\p"
	.string "The first light you have seen\n"
	.string "since the city went dark.$"

@ --- the guardians and their trial -------------------------------------------
@ Each guardian gates the trial the same way, then teaches its own half of
@ how purification actually works.
CelebiIsland_EventScript_GuardianUxie::
	lockall
	goto_if_set FLAG_WOT_FAIRY_TRIAL_DONE, CelebiIsland_EventScript_LessonUxie
	goto_if_not_defeated TRAINER_HANNAH, CelebiIsland_EventScript_GuardianBlocked
	goto_if_not_defeated TRAINER_SAMANTHA, CelebiIsland_EventScript_GuardianBlocked
	goto_if_not_defeated TRAINER_MAURA, CelebiIsland_EventScript_GuardianBlocked
	goto CelebiIsland_EventScript_TrioYield

CelebiIsland_EventScript_GuardianMesprit::
	lockall
	goto_if_set FLAG_WOT_FAIRY_TRIAL_DONE, CelebiIsland_EventScript_LessonMesprit
	goto_if_not_defeated TRAINER_HANNAH, CelebiIsland_EventScript_GuardianBlocked
	goto_if_not_defeated TRAINER_SAMANTHA, CelebiIsland_EventScript_GuardianBlocked
	goto_if_not_defeated TRAINER_MAURA, CelebiIsland_EventScript_GuardianBlocked
	goto CelebiIsland_EventScript_TrioYield

CelebiIsland_EventScript_GuardianAzelf::
	lockall
	goto_if_set FLAG_WOT_FAIRY_TRIAL_DONE, CelebiIsland_EventScript_LessonAzelf
	goto_if_not_defeated TRAINER_HANNAH, CelebiIsland_EventScript_GuardianBlocked
	goto_if_not_defeated TRAINER_SAMANTHA, CelebiIsland_EventScript_GuardianBlocked
	goto_if_not_defeated TRAINER_MAURA, CelebiIsland_EventScript_GuardianBlocked
	goto CelebiIsland_EventScript_TrioYield

CelebiIsland_EventScript_GuardianBlocked::
	waitse
	playmoncry SPECIES_UXIE, CRY_MODE_NORMAL
	waitmoncry
	msgbox CelebiIsland_Text_GuardianBlocks, MSGBOX_DEFAULT
	releaseall
	end

@ UXIE (knowledge): how to open a sealed heart.
CelebiIsland_EventScript_LessonUxie::
	waitse
	playmoncry SPECIES_UXIE, CRY_MODE_NORMAL
	waitmoncry
	msgbox CelebiIsland_Text_LessonUxie, MSGBOX_DEFAULT
	releaseall
	end

@ MESPRIT (emotion): what purification is worth.
CelebiIsland_EventScript_LessonMesprit::
	waitse
	playmoncry SPECIES_MESPRIT, CRY_MODE_NORMAL
	waitmoncry
	msgbox CelebiIsland_Text_LessonMesprit, MSGBOX_DEFAULT
	releaseall
	end

@ AZELF (willpower): wondering how far it could be pushed.
CelebiIsland_EventScript_LessonAzelf::
	waitse
	playmoncry SPECIES_AZELF, CRY_MODE_NORMAL
	waitmoncry
	msgbox CelebiIsland_Text_LessonAzelf, MSGBOX_DEFAULT
	releaseall
	end

CelebiIsland_EventScript_TrioTrial::
	lockall
	setvar VAR_TEMP_4, 1
	goto_if_set FLAG_WOT_FAIRY_TRIAL_DONE, CelebiIsland_EventScript_TrioTrialDone
	goto_if_not_defeated TRAINER_HANNAH, CelebiIsland_EventScript_TrioNotYet
	goto_if_not_defeated TRAINER_SAMANTHA, CelebiIsland_EventScript_TrioNotYet
	goto_if_not_defeated TRAINER_MAURA, CelebiIsland_EventScript_TrioNotYet
	goto CelebiIsland_EventScript_TrioYield

@ The yield + scatter + CELEBI descent. Entered either by stepping on the
@ shrine approach or by talking to a guardian once the keepers are beaten.
CelebiIsland_EventScript_TrioYield::
	waitse
	playmoncry SPECIES_UXIE, CRY_MODE_NORMAL
	waitmoncry
	playmoncry SPECIES_MESPRIT, CRY_MODE_NORMAL
	waitmoncry
	playmoncry SPECIES_AZELF, CRY_MODE_NORMAL
	waitmoncry
	msgbox CelebiIsland_Text_TrioYield, MSGBOX_DEFAULT
	closemessage
	@ --- the scatter: three guardians burst apart in three directions ---
	playse SE_FLEE
	applymovement LOCALID_ISLAND_UXIE, CelebiIsland_Movement_UxieScatterWest
	applymovement LOCALID_ISLAND_AZELF, CelebiIsland_Movement_AzelfScatterEast
	applymovement LOCALID_ISLAND_MESPRIT, CelebiIsland_Movement_MespritScatterSouth
	waitmovement LOCALID_ISLAND_UXIE
	waitmovement LOCALID_ISLAND_AZELF
	waitmovement LOCALID_ISLAND_MESPRIT
	@ relocate to their watch posts (off-camera; OnTransition re-applies)
	setobjectxyperm LOCALID_ISLAND_UXIE, 4, 33
	removeobject LOCALID_ISLAND_UXIE
	addobject LOCALID_ISLAND_UXIE
	setobjectxyperm LOCALID_ISLAND_MESPRIT, 25, 41
	removeobject LOCALID_ISLAND_MESPRIT
	addobject LOCALID_ISLAND_MESPRIT
	setobjectxyperm LOCALID_ISLAND_AZELF, 46, 33
	removeobject LOCALID_ISLAND_AZELF
	addobject LOCALID_ISLAND_AZELF
	@ --- Celebi lowers from the sky onto the altar ---
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, CelebiIsland_Movement_CameraToShrine
	waitmovement 0
	delay 16
	setflag FLAG_WOT_FAIRY_TRIAL_DONE
	clearflag FLAG_HIDE_WOT_CELEBI
	addobject LOCALID_ISLAND_CELEBI
	setobjectxy LOCALID_ISLAND_CELEBI, 25, 17
	playse SE_M_TELEPORT
	applymovement LOCALID_ISLAND_CELEBI, CelebiIsland_Movement_CelebiDescend
	waitmovement 0
	waitse
	playmoncry SPECIES_CELEBI, CRY_MODE_NORMAL
	waitmoncry
	delay 30
	applymovement LOCALID_CAMERA, CelebiIsland_Movement_CameraBackToPlayer
	waitmovement 0
	special RemoveCameraObject
	msgbox CelebiIsland_Text_TrioWatch, MSGBOX_DEFAULT
CelebiIsland_EventScript_TrioTrialDone::
	releaseall
	end

CelebiIsland_Movement_UxieScatterWest:
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	step_end

CelebiIsland_Movement_AzelfScatterEast:
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	walk_faster_right
	step_end

CelebiIsland_Movement_MespritScatterSouth:
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	step_end

CelebiIsland_Movement_CameraToShrine:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

CelebiIsland_Movement_CameraBackToPlayer:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

@ Slow float down out of the sky onto the altar tile (25,21).
CelebiIsland_Movement_CelebiDescend:
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

CelebiIsland_EventScript_TrioNotYet::
	msgbox CelebiIsland_Text_TrioNotYet, MSGBOX_DEFAULT
	releaseall
	end

CelebiIsland_Text_GuardianBlocks::
	.string "The small one hovers between\n"
	.string "you and the shrine.\p"
	.string "Its closed eyes see everything\n"
	.string "you have ever done.$"

CelebiIsland_Text_LessonUxie::
	.string "UXIE presses a thought into\n"
	.string "your head, patiently.\p"
	.string "A sealed heart cannot be\n"
	.string "reached from outside.\p"
	.string "BATTLE with the Shadow one.\n"
	.string "Stand beside it while it\l"
	.string "fights, and the seal loosens.\p"
	.string "Only then can the shrine\n"
	.string "finish the work.$"

CelebiIsland_Text_LessonMesprit::
	.string "MESPRIT turns a slow circle,\n"
	.string "delighted with itself.\p"
	.string "You fear losing the strength\n"
	.string "the shadow gave them.\p"
	.string "Do not. A purified one keeps\n"
	.string "every bit of that power --\l"
	.string "and pays none of the cost.\p"
	.string "No brittle defense. No HYPER\n"
	.string "MODE. Stronger than it was\l"
	.string "as a shadow.$"

CelebiIsland_Text_LessonAzelf::
	.string "AZELF will not settle.\p"
	.string "It has been thinking about\n"
	.string "the stones. The MEGA stones.\p"
	.string "A purified heart, it wonders,\n"
	.string "pushed through a MEGA bond...\p"
	.string "Nobody has ever done it.\n"
	.string "AZELF very much wants someone\l"
	.string "to try.$"

CelebiIsland_Text_TrioNotYet::
	.string "Three presences press against\n"
	.string "your thoughts:\p"
	.string "“The keepers. All of them.\n"
	.string "Then the shrine.”$"

CelebiIsland_Text_TrioYield::
	.string "The three lift as one.\p"
	.string "“Knowledge. Emotion. Will.\n"
	.string "The keepers vouch for yours.\p"
	.string "“The shrine is open,\n"
	.string "wish-bearer. We will hold\l"
	.string "the shores.”$"

CelebiIsland_Text_TrioWatch::
	.string "They scatter to the island's\n"
	.string "ends -- three sleepless\l"
	.string "sentinels.\p"
	.string "At the shrine... something\n"
	.string "green flickers into being.$"
