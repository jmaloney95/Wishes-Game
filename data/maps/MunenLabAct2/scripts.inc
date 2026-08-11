MunenLabAct2_MapScripts::
	.byte 0

@ ============================================================================
@  CLARKSON'S LABORATORY -- ACT 2 (occupied Munen)
@  Per Gengar-Clarkson's guidance, the player comes here to destroy the
@  research. DRACO got here first (local ID 1, top center, guarding the
@  desk). Battle him (starter-based variant, like Torii; earlyrival battle:
@  WIN OR LOSE the story continues, party is healed after) -- either way he
@  has already memorized the research and flees to SHIN-TOKYO to sell it to
@  the Dark Lord.
@  The ORIGINAL document remains: the desk offers a physical choice --
@  Keep the Research (ITEM_SHADOW_DOSSIER, readable from the bag) or
@  Destroy the Research (no item; the lore is learned along the way, so
@  nothing is missable). B on the menu defers the choice.
@ ============================================================================

@ Trigger row (y=8, VAR_TEMP_1 == 0). Also guards against refire after flee.
MunenLabAct2_EventScript_DracoTrigger::
	setvar VAR_TEMP_1, 1
	goto_if_set FLAG_ACT2_LAB_DRACO_FLED, MunenLabAct2_EventScript_TriggerDone
	lockall
	playbgm MUS_DP_VS_GALACTIC, FALSE
	playse SE_PIN
	applymovement 1, MunenLabAct2_Movement_Exclaim
	waitmovement 0
	@ Pan the camera up to Draco at the desk (player is on the y=8 trigger row).
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, MunenLabAct2_Movement_CameraPanUp
	waitmovement 0
	setspeaker SP_NAME_DRACO
	msgbox MunenLabAct2_Text_DracoIntro1, MSGBOX_DEFAULT
	msgbox MunenLabAct2_Text_DracoIntro2, MSGBOX_DEFAULT
	closemessage
	@ Pan back to the player before the battle (never remove a panned camera!).
	applymovement LOCALID_CAMERA, MunenLabAct2_Movement_CameraPanDown
	waitmovement 0
	special RemoveCameraObject
	goto_if_eq VAR_STARTER_CHOICE, 1, MunenLabAct2_EventScript_BattleAurorus
	goto_if_eq VAR_STARTER_CHOICE, 2, MunenLabAct2_EventScript_BattleArmaldo
	goto MunenLabAct2_EventScript_BattleTyrantrum

MunenLabAct2_EventScript_TriggerDone::
	end

MunenLabAct2_EventScript_BattleAurorus::
	trainerbattle_earlyrival TRAINER_DRACO_LAB_AURORUS, RIVAL_BATTLE_HEAL_AFTER, MunenLabAct2_Text_DracoDefeat, MunenLabAct2_Text_DracoVictory
	goto MunenLabAct2_EventScript_DracoFlees

MunenLabAct2_EventScript_BattleArmaldo::
	trainerbattle_earlyrival TRAINER_DRACO_LAB_ARMALDO, RIVAL_BATTLE_HEAL_AFTER, MunenLabAct2_Text_DracoDefeat, MunenLabAct2_Text_DracoVictory
	goto MunenLabAct2_EventScript_DracoFlees

MunenLabAct2_EventScript_BattleTyrantrum::
	trainerbattle_earlyrival TRAINER_DRACO_LAB_TYRANTRUM, RIVAL_BATTLE_HEAL_AFTER, MunenLabAct2_Text_DracoDefeat, MunenLabAct2_Text_DracoVictory
	goto MunenLabAct2_EventScript_DracoFlees

MunenLabAct2_EventScript_DracoFlees::
	setspeaker SP_NAME_DRACO
	msgbox MunenLabAct2_Text_DracoFlee, MSGBOX_DEFAULT
	closemessage
	applymovement 1, MunenLabAct2_Movement_DracoFlee
	waitmovement 0
	playse SE_EXIT
	removeobject 1
	setflag FLAG_ACT2_LAB_DRACO_FLED
	fadedefaultbgm
	msgbox MunenLabAct2_Text_AfterFlee, MSGBOX_DEFAULT
	@ Gengar-Clarkson insists the research be dealt with before leaving.
	@ While FLAG_ACT2_RESEARCH_RESOLVED is unset, the exit row (y=11) is gated.
	setspeaker SP_NAME_GENGAR
	showportrait PORTRAIT_CLARKSON_GENGAR
	msgbox MunenLabAct2_Text_GengarGate, MSGBOX_DEFAULT
	closemessage
	hideportrait
	releaseall
	end

@ Talking to Draco directly (before stepping on the trigger row).
MunenLabAct2_EventScript_Draco::
	lock
	faceplayer
	setvar VAR_TEMP_1, 1
	playbgm MUS_DP_VS_GALACTIC, FALSE
	setspeaker SP_NAME_DRACO
	msgbox MunenLabAct2_Text_DracoIntro1, MSGBOX_DEFAULT
	msgbox MunenLabAct2_Text_DracoIntro2, MSGBOX_DEFAULT
	closemessage
	goto_if_eq VAR_STARTER_CHOICE, 1, MunenLabAct2_EventScript_BattleAurorus
	goto_if_eq VAR_STARTER_CHOICE, 2, MunenLabAct2_EventScript_BattleArmaldo
	goto MunenLabAct2_EventScript_BattleTyrantrum

@ --------------------------------------------------------------------------
@  The desk: the physical choice.
@ --------------------------------------------------------------------------
MunenLabAct2_EventScript_ResearchDesk::
	lockall
	goto_if_set FLAG_ACT2_RESEARCH_RESOLVED, MunenLabAct2_EventScript_DeskResolved
	goto_if_unset FLAG_ACT2_LAB_DRACO_FLED, MunenLabAct2_EventScript_DeskBlocked
	msgbox MunenLabAct2_Text_DeskChoice, MSGBOX_DEFAULT
	multichoice 20, 8, MULTI_KEEP_DESTROY_RESEARCH, FALSE
	switch VAR_RESULT
	case 0, MunenLabAct2_EventScript_KeepResearch
	case 1, MunenLabAct2_EventScript_DestroyResearch
	@ B button: decide later
	msgbox MunenLabAct2_Text_DeskDeferred, MSGBOX_DEFAULT
	releaseall
	end

MunenLabAct2_EventScript_KeepResearch::
	giveitem ITEM_SHADOW_DOSSIER
	setflag FLAG_ACT2_RESEARCH_RESOLVED
	msgbox MunenLabAct2_Text_KeptResearch, MSGBOX_DEFAULT
	releaseall
	end

MunenLabAct2_EventScript_DestroyResearch::
	msgbox MunenLabAct2_Text_DestroyResearch1, MSGBOX_DEFAULT
	closemessage
	playse SE_M_FLAME_WHEEL
	fadescreen FADE_TO_WHITE
	delay 20
	fadescreen FADE_FROM_WHITE
	setflag FLAG_ACT2_RESEARCH_RESOLVED
	msgbox MunenLabAct2_Text_DestroyResearch2, MSGBOX_DEFAULT
	releaseall
	end

MunenLabAct2_EventScript_DeskBlocked::
	msgbox MunenLabAct2_Text_DeskBlocked, MSGBOX_DEFAULT
	releaseall
	end

MunenLabAct2_EventScript_DeskResolved::
	checkitem ITEM_SHADOW_DOSSIER
	goto_if_eq VAR_RESULT, TRUE, MunenLabAct2_EventScript_DeskEmptyKept
	msgbox MunenLabAct2_Text_DeskAshes, MSGBOX_DEFAULT
	releaseall
	end

MunenLabAct2_EventScript_DeskEmptyKept::
	msgbox MunenLabAct2_Text_DeskEmptyKept, MSGBOX_DEFAULT
	releaseall
	end

@ --------------------------------------------------------------------------
@  Exit gate: Gengar won't let the player walk out into occupied Munen until
@  the research choice is made -- and the desk is blocked until Draco is
@  beaten, so the battle is mandatory before the village opens up. Trigger
@  row at y=11 (in front of the y=12 warps), fires on VAR_TEMP_2 == 0 every
@  step; inert once the research is resolved, so it never re-arms.
@ --------------------------------------------------------------------------
MunenLabAct2_EventScript_ExitGate::
	goto_if_set FLAG_ACT2_RESEARCH_RESOLVED, MunenLabAct2_EventScript_ExitGateInert
	lockall
	setspeaker SP_NAME_GENGAR
	showportrait PORTRAIT_CLARKSON_GENGAR
	msgbox MunenLabAct2_Text_GengarGateNudge, MSGBOX_DEFAULT
	closemessage
	hideportrait
	applymovement OBJ_EVENT_ID_PLAYER, MunenLabAct2_Movement_GateStepBack
	waitmovement 0
	releaseall
	end

MunenLabAct2_EventScript_ExitGateInert::
	end

MunenLabAct2_Movement_GateStepBack:
	walk_up
	step_end

@ Using the kept dossier from the bag (dispatched from src/item_use.c):
@ Read (repeatable) / Destroy (confirmed, removes the item) / Cancel.
@ Destroying leaves the canonical Act 3 state -- FLAG_ACT2_RESEARCH_RESOLVED
@ set, ITEM_SHADOW_DOSSIER absent -- identical to destroying it at the desk.
MunenLabAct2_EventScript_DossierBagMenu::
	lockall
	multichoice 20, 6, MULTI_READ_DESTROY_RESEARCH, FALSE
	switch VAR_RESULT
	case 0, MunenLabAct2_EventScript_ReadDossier
	case 1, MunenLabAct2_EventScript_BagDestroyConfirm
	releaseall
	end

MunenLabAct2_EventScript_BagDestroyConfirm::
	msgbox MunenLabAct2_Text_BagDestroyConfirm, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_ne MunenLabAct2_EventScript_BagDestroyDeclined
	closemessage
	playse SE_M_FLAME_WHEEL
	fadescreen FADE_TO_WHITE
	delay 20
	fadescreen FADE_FROM_WHITE
	removeitem ITEM_SHADOW_DOSSIER
	msgbox MunenLabAct2_Text_BagDestroyed, MSGBOX_DEFAULT
	releaseall
	end

MunenLabAct2_EventScript_BagDestroyDeclined::
	msgbox MunenLabAct2_Text_BagDestroyDeclined, MSGBOX_DEFAULT
	releaseall
	end

@ Reading the kept dossier (from the bag menu above).
MunenLabAct2_EventScript_ReadDossier::
	lockall
	msgbox MunenLabAct2_Text_Dossier1, MSGBOX_DEFAULT
	msgbox MunenLabAct2_Text_Dossier2, MSGBOX_DEFAULT
	msgbox MunenLabAct2_Text_Dossier3, MSGBOX_DEFAULT
	releaseall
	end

@ --------------------------------------------------------------------------
@  Flavor
@ --------------------------------------------------------------------------
MunenLabAct2_EventScript_RansackedShelves::
	msgbox MunenLabAct2_Text_RansackedShelves, MSGBOX_SIGN
	end

MunenLabAct2_EventScript_DeadMachines::
	msgbox MunenLabAct2_Text_DeadMachines, MSGBOX_SIGN
	end

MunenLabAct2_Movement_CameraPanUp:
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

MunenLabAct2_Movement_CameraPanDown:
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

MunenLabAct2_Movement_Exclaim:
	emote_exclamation_mark
	delay_16
	step_end

@ Draco bolts NORTH out the top of the lab (he starts at 6,2; three fast
@ steps put him past the map edge before removeobject).
MunenLabAct2_Movement_DracoFlee:
	face_up
	delay_8
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

@ ============================================================================
@  Text
@ ============================================================================
MunenLabAct2_Text_DracoIntro1::
	.string "Every door in this village nailed\n"
	.string "shut, every road cut...\p"
	.string "...and still YOU turn up.\n"
	.string "Of course you do, {PLAYER}.$"

MunenLabAct2_Text_DracoIntro2::
	.string "Do you know what this is?\p"
	.string "CLARKSON'S proof. The mountain is\n"
	.string "a MACHINE -- and machines can be\l"
	.string "owned.\p"
	.string "The Dark Lord in Shin-Tokyo pays\n"
	.string "for knowledge like this. Pays in\l"
	.string "ways you can't imagine.\p"
	.string "You want the professor's papers?\n"
	.string "Then take them from me.$"

MunenLabAct2_Text_DracoDefeat::
	.string "Tch...! Enough. ENOUGH!$"

MunenLabAct2_Text_DracoVictory::
	.string "Still soft, {PLAYER}.\n"
	.string "The Dark Lord will eat you alive.$"

MunenLabAct2_Text_DracoFlee::
	.string "It doesn't matter, you know.\p"
	.string "I've already read every\l"
	.string "page. The words are already\l"
	.string "mine... and soon they'll be HIS.\p"
	.string "Shin-Tokyo's lights are calling,\n"
	.string "{PLAYER}. Try to keep up...$"

MunenLabAct2_Text_AfterFlee::
	.string "On the desk, Clarkson's original\n"
	.string "research lies where he left it.$"

MunenLabAct2_Text_GengarGate::
	.string "Wait.. You must DESTROY the\n"
	.string "Research$"

MunenLabAct2_Text_GengarGateNudge::
	.string "Wait.. You must DESTROY the\n"
	.string "Research$"

MunenLabAct2_Text_DeskBlocked::
	.string "The desk is buried in papers and\n"
	.string "instruments.\p"
	.string "Draco stands between you and all\n"
	.string "of it.$"

MunenLabAct2_Text_DeskChoice::
	.string "Clarkson's research sits on the\n"
	.string "desk. Years of work. One ghost's\l"
	.string "last regret.\p"
	.string "He asked you to destroy it.\n"
	.string "The choice, though, is yours.$"

MunenLabAct2_Text_KeptResearch::
	.string "{PLAYER} tucked the research away.\p"
	.string "It can be read from the KEY ITEMS\n"
	.string "pocket...$"

MunenLabAct2_Text_DestroyResearch1::
	.string "{PLAYER} gathered every page into\n"
	.string "the old lab burner...\p"
	.string "...and lit it.$"

MunenLabAct2_Text_DestroyResearch2::
	.string "The pages catch quickly.\p"
	.string "A life's work becomes light, then\n"
	.string "warmth, then ash.\p"
	.string "Somewhere -- maybe in your bag,\n"
	.string "you could swear something grins,\l"
	.string "and lets out a long-held breath.$"

MunenLabAct2_Text_DeskDeferred::
	.string "The research waits on the desk.\p"
	.string "It isn't going anywhere.\n"
	.string "That's rather the problem.$"

MunenLabAct2_Text_DeskAshes::
	.string "Only ash remains on the desk,\n"
	.string "still faintly warm.\p"
	.string "The lab feels lighter.$"

MunenLabAct2_Text_DeskEmptyKept::
	.string "The desk is bare.\p"
	.string "The research rides in your bag\n"
	.string "now, heavy as a held breath.$"

MunenLabAct2_Text_BagDestroyConfirm::
	.string "Destroy Clarkson's research?\p"
	.string "This cannot be undone.$"

@ PLACEHOLDER (Joe): aftermath of destroying the research from the bag.
MunenLabAct2_Text_BagDestroyed::
	.string "{PLAYER} fed the pages to a small,\n"
	.string "careful fire.\p"
	.string "A life's work becomes light, then\n"
	.string "warmth, then ash.\p"
	.string "Somewhere close by, something\n"
	.string "grins, and lets out a long-held\l"
	.string "breath.$"

MunenLabAct2_Text_BagDestroyDeclined::
	.string "{PLAYER} tucked the research away\n"
	.string "again.$"

MunenLabAct2_Text_Dossier1::
	.string "{PLAYER} opened the Shadow\n"
	.string "Dossier...\p"
	.string "“FIELD NOTES, VOL. IX -- FINAL.\n"
	.string "-- E. CLARKSON\p"
	.string "“The mountain is not a god. It is\n"
	.string "a MECHANISM. It keeps what grief\l"
	.string "cannot release -- and what is\l"
	.string "kept can be RESHAPED.$"

MunenLabAct2_Text_Dossier2::
	.string "“A heart can be closed the way a\n"
	.string "door is closed.\p"
	.string "“I have seen the specimens. I\n"
	.string "will call them what they are:\l"
	.string "SHADOW POKEMON. Pokemon whose\l"
	.string "hearts have been shut --\l"
	.string "artificially. Deliberately.\p"
	.string "“And JIRACHI... the Undying Wish\n"
	.string "is force without shape. Shaped\l"
	.string "through the Summit Gate, one wish\l"
	.string "could close ten thousand hearts\l"
	.string "at once.\p"
	.string "“An army that feels nothing,\n"
	.string "wished into being overnight.$"

MunenLabAct2_Text_Dossier3::
	.string "“There is a countermeasure. I\n"
	.string "believe it, though I cannot yet\l"
	.string "prove it.\p"
	.string "“CELEBI. The forest's spirit of\n"
	.string "time itself.\p"
	.string "“What was closed IN time can be\n"
	.string "reopened BY it. Purification will\l"
	.string "demand exactly what the shadows\l"
	.string "lack: time. Memory. Care.\p"
	.string "“If you are reading this and I am\n"
	.string "gone -- the Gate must NOT be\l"
	.string "given a wish.”$"

MunenLabAct2_Text_RansackedShelves::
	.string "The shelves have been swept bare\n"
	.string "in one long armful.\p"
	.string "Whoever searched here knew\n"
	.string "exactly what they were looking\l"
	.string "for -- and didn't care what broke.$"

MunenLabAct2_Text_DeadMachines::
	.string "The lab machines are dark.\p"
	.string "Snow has drifted in through a\n"
	.string "cracked window and settled on the\l"
	.string "keys, unmelted.\p"
	.string "No one has warmed this room in a\n"
	.string "long time.$"
