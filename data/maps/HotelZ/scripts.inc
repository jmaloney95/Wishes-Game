@ ============================================================================
@  HOTEL Z -- Team MZ's base (placeholder interior: a copy of the Slateport
@  Pokemon Fan Club layout with its raised stage flattened, LAYOUT_HOTEL_Z).
@  Two ways in, both once the Tradewind intro has played: Tradewind's
@  (23,17) building and the double door in Shin Tokyo (12-13,17). The exits
@  use the dynamic warp, so you leave the way you came: Shin Tokyo is far later
@  in the story than Tradewind, so a crossing would skip most of the game.
@  The first time the player walks in, Mable briefs them on rogue Mega
@  Evolution: Mega Doomsday completes and Two Minutes Is a Long Time starts
@  (bring her the stones of all 23 rogue Megas; she keeps a tally). Then her
@  calls start Primal Signatures and Sky-High Mega (data/scripts/mega_dimension.inc).
@ ============================================================================
HotelZ_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, HotelZ_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, HotelZ_OnFrame
	.byte 0

HotelZ_OnTransition::
	special WotHotelZ_SetExit
	special WotHotelZ_ShowRivals
	special WotZygarde_SetupDesk
	call_if_unset FLAG_WOT_MEGA_DOOMSDAY_INTRO, HotelZ_EventScript_HideTeam
	call_if_set FLAG_WOT_MEGA_DOOMSDAY_INTRO, HotelZ_EventScript_ShowTeam
	end

HotelZ_EventScript_HideTeam::
	setflag FLAG_HIDE_HOTEL_Z_TEAM
	return

HotelZ_EventScript_ShowTeam::
	clearflag FLAG_HIDE_HOTEL_Z_TEAM
	return

HotelZ_OnFrame:
	map_script_2 VAR_TEMP_1, 0, HotelZ_EventScript_Briefing
	.2byte 0

HotelZ_EventScript_Briefing::
	lockall
	setvar VAR_TEMP_1, 1
	goto_if_set FLAG_WOT_HOTEL_Z_MET, HotelZ_EventScript_BriefingDone
	goto_if_unset FLAG_WOT_MEGA_DOOMSDAY_INTRO, HotelZ_EventScript_BriefingDone
	applymovement LOCALID_HOTEL_Z_LIDA, Common_Movement_ExclamationMark
	waitmovement 0
	@ Whoever is talking hops (Lida, Canari) or steps up (Mable), so the
	@ player learns who's who; the nameplate colours match.
	applymovement LOCALID_HOTEL_Z_LIDA, HotelZ_Movement_Hop
	waitmovement 0
	setspeaker SP_NAME_LIDA
	msgbox HotelZ_Text_LidaWelcome, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_HOTEL_Z_CANARI, HotelZ_Movement_Hop
	waitmovement 0
	setspeaker SP_NAME_CANARI
	msgbox HotelZ_Text_CanariWelcome, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_HOTEL_Z_MABLE, Common_Movement_WalkDown
	waitmovement 0
	setspeaker SP_NAME_MABLE
	msgbox HotelZ_Text_MableResearch, MSGBOX_DEFAULT
	msgbox HotelZ_Text_MableOffer, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_HOTEL_Z_MABLE, HotelZ_Movement_MableStepBack
	waitmovement 0
	applymovement LOCALID_HOTEL_Z_LIDA, HotelZ_Movement_Hop
	waitmovement 0
	setspeaker SP_NAME_LIDA
	msgbox HotelZ_Text_LidaLongTime, MSGBOX_DEFAULT
	msgbox HotelZ_Text_LidaDoors, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	setflag FLAG_WOT_HOTEL_Z_MET
	completequest QUEST_MEGA_DOOMSDAY
	startquest QUEST_TWO_MINUTES
HotelZ_EventScript_BriefingDone::
	specialvar VAR_RESULT, WotZygarde_PebbleDue
	goto_if_eq VAR_RESULT, TRUE, HotelZ_EventScript_DogAttacks
	releaseall
	end

@ --- the Zygarde arc --------------------------------------------------
@ 3 stones in: the dog is between you and the door, and it wants a fight.
HotelZ_EventScript_DogAttacks::
	waitse
	playmoncry SPECIES_ZYGARDE_10_AURA_BREAK, CRY_MODE_ENCOUNTER
	waitmoncry
	applymovement LOCALID_HOTEL_Z_DOG, Common_Movement_ExclamationMark
	waitmovement 0
	msgbox HotelZ_Text_DogBlocks, MSGBOX_DEFAULT
	closemessage
HotelZ_EventScript_DogBattle::
	special WotZygarde_SetupDogBattle
	setflag FLAG_WOT_NO_CATCHING
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	clearflag FLAG_WOT_NO_CATCHING
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_ne VAR_RESULT, B_OUTCOME_WON, HotelZ_EventScript_DogBattle
	msgbox HotelZ_Text_DogDrops, MSGBOX_DEFAULT
	closemessage
	giveitem ITEM_ODD_PEBBLE
	startquest QUEST_ORDER_IN_THE_RIFTS
	setspeaker SP_NAME_AZ
	msgbox HotelZ_Text_AZSeesPebble, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	fadescreenswapbuffers FADE_TO_BLACK
	removeobject LOCALID_HOTEL_Z_DOG
	setflag FLAG_TEMP_6
	fadescreenswapbuffers FADE_FROM_BLACK
	releaseall
	end

HotelZ_EventScript_Dog::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_ZYGARDE_10_AURA_BREAK, CRY_MODE_NORMAL
	waitmoncry
	specialvar VAR_RESULT, WotZygarde_DogCounts
	goto_if_eq VAR_RESULT, TRUE, HotelZ_EventScript_DogCounts
	msgbox HotelZ_Text_DogStares, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_AZ
	msgbox HotelZ_Text_AZOnDog, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	release
	end

HotelZ_EventScript_DogCounts::
	msgbox HotelZ_Text_DogCounts, MSGBOX_DEFAULT
	closemessage
	release
	end

@ 20 Cells, after Mable's call. What he did, and that you helped.
HotelZ_EventScript_AZReveal::
	completequest QUEST_ORDER_IN_THE_RIFTS
	setspeaker SP_NAME_AZ
	msgbox HotelZ_Text_AZReveal1, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	waitse
	playmoncry SPECIES_ZYGARDE_10_AURA_BREAK, CRY_MODE_ENCOUNTER
	waitmoncry
	applymovement LOCALID_HOTEL_Z_DOG, Common_Movement_ExclamationMark
	waitmovement 0
	setspeaker SP_NAME_AZ
	msgbox HotelZ_Text_AZReveal2, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_HOTEL_Z_AZ, HotelZ_Text_AZDefeat
	setspeaker SP_NAME_AZ
	msgbox HotelZ_Text_AZLeaves, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	playse SE_THUNDER
	setvar VAR_0x8004, 2
	setvar VAR_0x8005, 2
	setvar VAR_0x8006, 8
	setvar VAR_0x8007, 4
	special ShakeCamera
	waitstate
	setvar VAR_0x8004, 2
	special WotZygarde_SetStage
	removeobject LOCALID_HOTEL_Z_AZ
	setflag FLAG_TEMP_5
	playse SE_WARP_OUT
	removeobject LOCALID_HOTEL_Z_DOG
	setflag FLAG_TEMP_6
	clearflag FLAG_TEMP_7
	addobject LOCALID_HOTEL_Z_RIFT
	msgbox HotelZ_Text_RiftOpens, MSGBOX_DEFAULT
	closemessage
	release
	end

@ The rift where AZ stood.
HotelZ_EventScript_DeskRift::
	lock
	msgbox HotelZ_Text_RiftPrompt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, HotelZ_EventScript_DeskRiftNo
	closemessage
	call MegaDim_EventScript_RiftOut
	warp MAP_MEGA_DIMENSION_DRAINED_HOTEL, 6, 9
	waitstate
	release
	end

HotelZ_EventScript_DeskRiftNo::
	closemessage
	release
	end

@ With AZ gone, the desk runs on trust.
HotelZ_EventScript_DeskNote::
	msgbox HotelZ_Text_DeskNote, MSGBOX_DEFAULT
	closemessage
	special WotZShop_Open
	release
	end

HotelZ_EventScript_MableLine1::
	msgbox HotelZ_Text_MableCells, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableClose

HotelZ_EventScript_MableLine2::
	msgbox HotelZ_Text_MableDrain, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableClose

HotelZ_EventScript_MableLine3::
	msgbox HotelZ_Text_MableAfterAZ, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableClose

HotelZ_EventScript_MableLine4::
	msgbox HotelZ_Text_MableEnoughCells, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableClose

HotelZ_EventScript_MableGiveCube::
	setspeaker SP_NAME_MABLE
	msgbox HotelZ_Text_MableCube, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	giveitem ITEM_ZYGARDE_CUBE
	givemon SPECIES_FLOETTE_ETERNAL, 50
	playfanfare MUS_OBTAIN_ITEM
	msgbox HotelZ_Text_FloetteJoins, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	setspeaker SP_NAME_MABLE
	msgbox HotelZ_Text_MableCubeAfter, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_Text_DogBlocks:
	.string "AZ's dog has planted itself between\n"
	.string "you and the door.\p"
	.string "It isn't playing.$"

HotelZ_Text_DogDrops:
	.string "The dog shakes itself off and looks\n"
	.string "at you for a long moment.\p"
	.string "It drops something at your feet.$"

HotelZ_Text_AZSeesPebble:
	.string "A pebble. How curious. May I…\p"
	.string "No. Forgive me. Do keep that away\n"
	.string "from my desk, if you would.$"

HotelZ_Text_DogStares:
	.string "The dog isn't looking at you.\n"
	.string "It's watching the front desk.$"

HotelZ_Text_AZOnDog:
	.string "He came with the building.\n"
	.string "Or perhaps I came with him.$"

HotelZ_Text_DogCounts:
	.string "The dog sniffs your Odd Pebble.\p"
	.string "{STR_VAR_1} Cells. It seems to want {STR_VAR_2}.$"

HotelZ_Text_AZReveal1:
	.string "You've brought it to my desk after\n"
	.string "all. All those little green pieces.\p"
	.string "Let me save the detective her time.\n"
	.string "It was always me.\p"
	.string "Three thousand years ago I lost my\n"
	.string "Floette, and I built a machine to\l"
	.string "bring her back. It needs life.\p"
	.string "That gray place had plenty, once.$"

HotelZ_Text_AZReveal2:
	.string "Who did you think those shards were\n"
	.string "from? What did you think I did with\l"
	.string "every one you handed me?\p"
	.string "You've been a wonderful guest.\p"
	.string "One world, for one life. I would\n"
	.string "do it again. Stand aside.$"

HotelZ_Text_AZDefeat:
	.string "Yes. You would be the one.$"

HotelZ_Text_AZLeaves:
	.string "It changes nothing. She is nearly\n"
	.string "whole. She only needs a little more.$"

HotelZ_Text_RiftOpens:
	.string "A rift has torn open behind the desk.\n"
	.string "The dog went in after him.$"

HotelZ_Text_RiftPrompt:
	.string "The rift behind the desk.\n"
	.string "Go through?$"

HotelZ_Text_DeskNote:
	.string "There's a note on the desk.\p"
	.string "“Back soon. Leave your shards,\n"
	.string "take what you've paid for. Taunie.”$"

HotelZ_Text_MableCells:
	.string "Those Cells of yours? I've run the\n"
	.string "numbers. They're what closes rifts.\p"
	.string "That clock isn't the rift dying.\n"
	.string "Something is sealing it.$"

HotelZ_Text_MableDrain:
	.string "You've found {STR_VAR_1} Cells. Bring it\n"
	.string "to {STR_VAR_2}, then we'll know for sure.$"

HotelZ_Text_MableEnoughCells:
	.string "{STR_VAR_1} Cells. That's enough to be\n"
	.string "sure, and they all point one way.\p"
	.string "At the front desk. At AZ.\n"
	.string "Go and talk to him.$"

HotelZ_Text_MableAfterAZ:
	.string "Nothing regrows overnight, but the\n"
	.string "gray world has stopped losing color.$"

HotelZ_Text_MableCube:
	.string "So Zygarde chose you. Take this. It\n"
	.string "keeps all its pieces in one place.\p"
	.string "And someone came back through that\n"
	.string "rift with you. She won't leave.$"

HotelZ_Text_FloetteJoins:
	.string "The Eternal Flower Floette\n"
	.string "joined you!$"

HotelZ_Text_MableCubeAfter:
	.string "I don't think she's waiting for him.\n"
	.string "I think she's done waiting.$"


HotelZ_Movement_Hop:
	jump_in_place_down
	step_end

HotelZ_Movement_MableStepBack:
	walk_up
	face_down
	step_end

@ Mable keeps a tally of the rogue Megas' stones (the player keeps the
@ stones). Only the dimension's own claims count. At 23 she runs her tests;
@ after that she has a line for each stage of her quest chain.
HotelZ_EventScript_Mable::
	lock
	faceplayer
	setspeaker SP_NAME_MABLE
	specialvar VAR_RESULT, WotMegaDim_MableReport
	buffernumberstring STR_VAR_1, VAR_0x8004
	goto_if_eq VAR_RESULT, 3, HotelZ_EventScript_MableStage
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_MableAllStones
	goto_if_eq VAR_RESULT, 1, HotelZ_EventScript_MableRecorded
	goto_if_eq VAR_0x8004, 0, HotelZ_EventScript_MableWaiting
	msgbox HotelZ_Text_MableTally, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableWaiting::
	msgbox HotelZ_Text_MableWaiting, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableRecorded::
	call_if_eq VAR_0x8004, 1, HotelZ_EventScript_MableFirstSample
	msgbox HotelZ_Text_MableRecorded, MSGBOX_DEFAULT
	checkitem ITEM_Z_POWER_RING
	call_if_eq VAR_RESULT, TRUE, HotelZ_EventScript_MableNewCrystals
	closemessage
	questtoast QUEST_TOAST_UPDATE, QUEST_TWO_MINUTES
	goto HotelZ_EventScript_MableEnd

@ The rogue's Z-Move, explained, and the player's own way in.
HotelZ_EventScript_MableGiveRing::
	setspeaker SP_NAME_MABLE
	msgbox HotelZ_Text_MableRing, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	giveitem ITEM_Z_POWER_RING
	giveitem ITEM_NORMALIUM_Z
	giveitem ITEM_Z_SHARD, 10
	setspeaker SP_NAME_MABLE
	msgbox HotelZ_Text_MableRingAfter, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableGiveCore::
	setspeaker SP_NAME_MABLE
	msgbox HotelZ_Text_MableCore, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	giveitem ITEM_ROGUE_CORE
	setspeaker SP_NAME_MABLE
	msgbox HotelZ_Text_MableCoreAfter, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

@ AZ's desk. The counter takes a talk too (bg events in map.json).
HotelZ_EventScript_AZ::
	lock
	specialvar VAR_RESULT, WotZygarde_GetStage
	goto_if_ge VAR_RESULT, 2, HotelZ_EventScript_DeskNote
	specialvar VAR_RESULT, WotZygarde_RevealDue
	goto_if_eq VAR_RESULT, TRUE, HotelZ_EventScript_AZReveal
	setspeaker SP_NAME_AZ
	checkitem ITEM_Z_POWER_RING
	goto_if_eq VAR_RESULT, FALSE, HotelZ_EventScript_AZNoRing
	msgbox HotelZ_Text_AZWelcome, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	special WotZShop_Open
	setspeaker SP_NAME_AZ
	msgbox HotelZ_Text_AZBye, MSGBOX_DEFAULT
	goto HotelZ_EventScript_AZEnd

HotelZ_EventScript_AZNoRing::
	msgbox HotelZ_Text_AZNoRing, MSGBOX_DEFAULT
HotelZ_EventScript_AZEnd::
	closemessage
	setspeaker SP_NAME_NONE
	release
	end

HotelZ_EventScript_MableFirstSample::
	msgbox HotelZ_Text_MableSample, MSGBOX_DEFAULT
	return

HotelZ_EventScript_MableAllStones::
	msgbox HotelZ_Text_MableAllStones, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	goto_if_quest_complete QUEST_TWO_MINUTES, HotelZ_EventScript_MableEnd
	completequest QUEST_TWO_MINUTES
	goto HotelZ_EventScript_MableEnd

@ VAR_0x8005 = her stage (WotMegaDim MABLE_STAGE_*).
HotelZ_EventScript_MableStage::
	goto_if_eq VAR_0x8005, 1, HotelZ_EventScript_MableTesting
	goto_if_eq VAR_0x8005, 2, HotelZ_EventScript_MablePrimals
	goto_if_eq VAR_0x8005, 3, HotelZ_EventScript_MableTesting
	goto_if_eq VAR_0x8005, 4, HotelZ_EventScript_MableRayquaza
	msgbox HotelZ_Text_MableAfter, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableTesting::
	msgbox HotelZ_Text_MableTesting, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MablePrimals::
	msgbox HotelZ_Text_MablePrimals, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableRayquaza::
	msgbox HotelZ_Text_MableRayquaza, MSGBOX_DEFAULT
	goto HotelZ_EventScript_MableEnd

HotelZ_EventScript_MableEnd::
	specialvar VAR_RESULT, WotMegaDim_MableGiftDue
	goto_if_eq VAR_RESULT, 1, HotelZ_EventScript_MableGiveRing
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_MableGiveCore
	goto_if_eq VAR_RESULT, 3, HotelZ_EventScript_MableGiveCube
	specialvar VAR_RESULT, WotZygarde_MableLine
	goto_if_eq VAR_RESULT, 1, HotelZ_EventScript_MableLine1
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_MableLine2
	goto_if_eq VAR_RESULT, 3, HotelZ_EventScript_MableLine3
	goto_if_eq VAR_RESULT, 4, HotelZ_EventScript_MableLine4
HotelZ_EventScript_MableClose::
	closemessage
	setspeaker SP_NAME_NONE
	release
	end

@ --- the Z-A rivals as partners -------------------------------------
@ A rival beaten in the Mega Dimension is unlocked: they check in here (Emma,
@ Taunie, Tulip, Jacinthe appear; Lida and Canari were here all along) and can
@ be asked to join your portal runs. One partner at a time: asking another
@ swaps them, asking your partner again lets you go solo.
@ WotMegaDim_GetRivalState: VAR_RESULT 0 not beaten, 1 recruitable, 2 partner.

HotelZ_EventScript_Canari::
	lock
	faceplayer
	setspeaker SP_NAME_CANARI
	setvar VAR_0x8004, 1
	special WotMegaDim_GetRivalState
	special WotPartner_BufferHotelLine
	goto_if_eq VAR_RESULT, 0xFF, HotelZ_EventScript_CanariFriendLine
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_CanariPartner
	goto_if_eq VAR_RESULT, 0, HotelZ_EventScript_CanariIdle
	msgbox HotelZ_Text_CanariPitch, MSGBOX_DEFAULT
	goto HotelZ_EventScript_CanariCard

HotelZ_EventScript_CanariPartner::
	msgbox HotelZ_Text_CanariReady, MSGBOX_DEFAULT
	goto HotelZ_EventScript_CanariCard

HotelZ_EventScript_CanariFriendLine::
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
HotelZ_EventScript_CanariCard::
	setvar VAR_0x8004, 1
	call HotelZ_EventScript_OpenPartnerCard
	setspeaker SP_NAME_CANARI
	goto_if_eq VAR_RESULT, 1, HotelZ_EventScript_CanariCardJoins
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_CanariCardSolo
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_CanariCardJoins::
	msgbox HotelZ_Text_CanariJoins, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_CanariCardSolo::
	msgbox HotelZ_Text_CanariSolo, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_CanariIdle::
	msgbox HotelZ_Text_CanariIdle, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_Lida::
	lock
	faceplayer
	setspeaker SP_NAME_LIDA
	setvar VAR_0x8004, 2
	special WotMegaDim_GetRivalState
	special WotPartner_BufferHotelLine
	goto_if_eq VAR_RESULT, 0xFF, HotelZ_EventScript_LidaFriendLine
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_LidaPartner
	goto_if_eq VAR_RESULT, 0, HotelZ_EventScript_LidaIdle
	msgbox HotelZ_Text_LidaPitch, MSGBOX_DEFAULT
	goto HotelZ_EventScript_LidaCard

HotelZ_EventScript_LidaPartner::
	msgbox HotelZ_Text_LidaReady, MSGBOX_DEFAULT
	goto HotelZ_EventScript_LidaCard

HotelZ_EventScript_LidaFriendLine::
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
HotelZ_EventScript_LidaCard::
	setvar VAR_0x8004, 2
	call HotelZ_EventScript_OpenPartnerCard
	setspeaker SP_NAME_LIDA
	goto_if_eq VAR_RESULT, 1, HotelZ_EventScript_LidaCardJoins
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_LidaCardSolo
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_LidaCardJoins::
	msgbox HotelZ_Text_LidaJoins, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_LidaCardSolo::
	msgbox HotelZ_Text_LidaSolo, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_LidaIdle::
	msgbox HotelZ_Text_LidaIdle, MSGBOX_DEFAULT
	msgbox HotelZ_Text_LidaDoors, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_Emma::
	lock
	faceplayer
	setspeaker SP_NAME_EMMA
	setvar VAR_0x8004, 3
	special WotMegaDim_GetRivalState
	special WotPartner_BufferHotelLine
	goto_if_eq VAR_RESULT, 0xFF, HotelZ_EventScript_EmmaFriendLine
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_EmmaPartner
	msgbox HotelZ_Text_EmmaPitch, MSGBOX_DEFAULT
	goto HotelZ_EventScript_EmmaCard

HotelZ_EventScript_EmmaPartner::
	msgbox HotelZ_Text_EmmaReady, MSGBOX_DEFAULT
	goto HotelZ_EventScript_EmmaCard

HotelZ_EventScript_EmmaFriendLine::
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
HotelZ_EventScript_EmmaCard::
	setvar VAR_0x8004, 3
	call HotelZ_EventScript_OpenPartnerCard
	setspeaker SP_NAME_EMMA
	goto_if_eq VAR_RESULT, 1, HotelZ_EventScript_EmmaCardJoins
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_EmmaCardSolo
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_EmmaCardJoins::
	msgbox HotelZ_Text_EmmaJoins, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_EmmaCardSolo::
	msgbox HotelZ_Text_EmmaSolo, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_Taunie::
	lock
	faceplayer
	setspeaker SP_NAME_TAUNIE
	setvar VAR_0x8004, 4
	special WotMegaDim_GetRivalState
	special WotPartner_BufferHotelLine
	goto_if_eq VAR_RESULT, 0xFF, HotelZ_EventScript_TaunieFriendLine
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_TauniePartner
	msgbox HotelZ_Text_TauniePitch, MSGBOX_DEFAULT
	goto HotelZ_EventScript_TaunieCard

HotelZ_EventScript_TauniePartner::
	msgbox HotelZ_Text_TaunieReady, MSGBOX_DEFAULT
	goto HotelZ_EventScript_TaunieCard

HotelZ_EventScript_TaunieFriendLine::
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
HotelZ_EventScript_TaunieCard::
	setvar VAR_0x8004, 4
	call HotelZ_EventScript_OpenPartnerCard
	setspeaker SP_NAME_TAUNIE
	goto_if_eq VAR_RESULT, 1, HotelZ_EventScript_TaunieCardJoins
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_TaunieCardSolo
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_TaunieCardJoins::
	msgbox HotelZ_Text_TaunieJoins, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_TaunieCardSolo::
	msgbox HotelZ_Text_TaunieSolo, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_Tulip::
	lock
	faceplayer
	setspeaker SP_NAME_TULIP
	setvar VAR_0x8004, 5
	special WotMegaDim_GetRivalState
	special WotPartner_BufferHotelLine
	goto_if_eq VAR_RESULT, 0xFF, HotelZ_EventScript_TulipFriendLine
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_TulipPartner
	msgbox HotelZ_Text_TulipPitch, MSGBOX_DEFAULT
	goto HotelZ_EventScript_TulipCard

HotelZ_EventScript_TulipPartner::
	msgbox HotelZ_Text_TulipReady, MSGBOX_DEFAULT
	goto HotelZ_EventScript_TulipCard

HotelZ_EventScript_TulipFriendLine::
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
HotelZ_EventScript_TulipCard::
	setvar VAR_0x8004, 5
	call HotelZ_EventScript_OpenPartnerCard
	setspeaker SP_NAME_TULIP
	goto_if_eq VAR_RESULT, 1, HotelZ_EventScript_TulipCardJoins
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_TulipCardSolo
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_TulipCardJoins::
	msgbox HotelZ_Text_TulipJoins, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_TulipCardSolo::
	msgbox HotelZ_Text_TulipSolo, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_Jacinthe::
	lock
	faceplayer
	setspeaker SP_NAME_JACINTHE
	setvar VAR_0x8004, 6
	special WotMegaDim_GetRivalState
	special WotPartner_BufferHotelLine
	goto_if_eq VAR_RESULT, 0xFF, HotelZ_EventScript_JacintheFriendLine
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_JacinthePartner
	msgbox HotelZ_Text_JacinthePitch, MSGBOX_DEFAULT
	goto HotelZ_EventScript_JacintheCard

HotelZ_EventScript_JacinthePartner::
	msgbox HotelZ_Text_JacintheReady, MSGBOX_DEFAULT
	goto HotelZ_EventScript_JacintheCard

HotelZ_EventScript_JacintheFriendLine::
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
HotelZ_EventScript_JacintheCard::
	setvar VAR_0x8004, 6
	call HotelZ_EventScript_OpenPartnerCard
	setspeaker SP_NAME_JACINTHE
	goto_if_eq VAR_RESULT, 1, HotelZ_EventScript_JacintheCardJoins
	goto_if_eq VAR_RESULT, 2, HotelZ_EventScript_JacintheCardSolo
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_JacintheCardJoins::
	msgbox HotelZ_Text_JacintheJoins, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

HotelZ_EventScript_JacintheCardSolo::
	msgbox HotelZ_Text_JacintheSolo, MSGBOX_DEFAULT
	goto HotelZ_EventScript_RivalEnd

@ The partner card (wot_partner_card.c) stands in for the
@ yes/no. VAR_0x8004 = the rival; VAR_RESULT = 0 closed, 1 invited, 2 solo.
@ VAR_0x8004 = the rival. At friendship Lv3 they hand over their crystal first
@.
HotelZ_EventScript_OpenPartnerCard::
	copyvar VAR_0x8009, VAR_0x8004
	special WotPartner_TakeCrystalGift
	goto_if_eq VAR_RESULT, ITEM_NONE, HotelZ_EventScript_OpenPartnerCardNow
	copyvar VAR_0x8005, VAR_RESULT
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	giveitem VAR_0x8005
	copyvar VAR_0x8004, VAR_0x8009
HotelZ_EventScript_OpenPartnerCardNow::
	closemessage
	setspeaker SP_NAME_NONE
	fadescreen FADE_TO_BLACK
	special WotPartnerCard_Open
	waitstate
	return

HotelZ_EventScript_RivalEnd::
	closemessage
	setspeaker SP_NAME_NONE
	release
	end

HotelZ_Text_LidaWelcome:
	.string "{PLAYER}! You actually came!\p"
	.string "Welcome to Hotel Z. Officially\n"
	.string "it's a hotel. Unofficially…\l"
	.string "it's Team MZ's base.$"

HotelZ_Text_CanariWelcome:
	.string "Unofficially-officially. We have\n"
	.string "a logo and everything.$"

HotelZ_Text_MableResearch:
	.string "So you're the one who stood up\n"
	.string "to The Oni. I'm Mable, Team MZ's\l"
	.string "researcher. Report back to me.\p"
	.string "I study Mega Evolution. Lately,\n"
	.string "the kind that's gone wrong.\p"
	.string "Rogue Megas. Pokémon locked in\n"
	.string "Mega form, with no Trainer and\l"
	.string "no Mega Ring holding them.\p"
	.string "They're being pulled through\n"
	.string "rifts, from a gray mirror of\l"
	.string "our world. A Mega Dimension.$"

HotelZ_Text_MableOffer:
	.string "I can hold a rift open for two\n"
	.string "minutes. Not a second more.\p"
	.string "Go in, find the rogue Mega and\n"
	.string "neutralize it. Bring me back its\l"
	.string "Mega Stone as a sample.\p"
	.string "There are 23 of them out there.\n"
	.string "I need a reading from each one.\p"
	.string "The rifts open at 8:00 every\n"
	.string "morning. Watch for the light.$"

HotelZ_Text_LidaLongTime:
	.string "Two minutes is a long time…\n"
	.string "if you're fast!$"

HotelZ_Text_MableRing:
	.string "That burst of power the rogue threw\n"
	.string "at you? Z-Power. It's old Alolan\l"
	.string "science, and it shouldn't be here.\p"
	.string "No Pokémon should manage a Mega\n"
	.string "and a Z-Move together. Rogues do.\p"
	.string "You deserve a fair fight. Wear this,\n"
	.string "and hold a crystal on a Pokémon.$"

HotelZ_Text_MableRingAfter:
	.string "Those Z-Shards are what rogues shed\n"
	.string "when they burn Z-Power.\p"
	.string "AZ at the front desk can press them\n"
	.string "into crystals. Each rogue you bring\l"
	.string "in tells him which ones to make.$"

HotelZ_EventScript_MableNewCrystals::
	msgbox HotelZ_Text_MableNewCrystals, MSGBOX_DEFAULT
	return

HotelZ_Text_MableNewCrystals:
	.string "These readings should help AZ too.\n"
	.string "Ask at the desk what's new.$"

HotelZ_Text_MableCore:
	.string "Mega Rayquaza's rift left this.\n"
	.string "It's the heart of what rogues do.\p"
	.string "Carry it, and your Mega can throw\n"
	.string "a Z-Move the way they do.$"

HotelZ_Text_MableCoreAfter:
	.string "No crystal needed, in or out of a\n"
	.string "rift. Be careful with it.$"

HotelZ_Text_AZWelcome:
	.string "Welcome to Hotel Z.\n"
	.string "I keep the desk.\p"
	.string "Bring me those shards, and I will\n"
	.string "press them into crystals for you.$"

HotelZ_Text_AZBye:
	.string "Three thousand years, and I still\n"
	.string "enjoy a guest. Come again.$"

HotelZ_Text_AZNoRing:
	.string "Welcome to Hotel Z.\n"
	.string "I keep the desk.\p"
	.string "Speak with Mable when you bring back\n"
	.string "your first stone. I can help after.$"

HotelZ_Text_MableSample:
	.string "Is that… a rogue Mega's stone?\n"
	.string "You actually did it.\p"
	.string "This is exactly the sample I\n"
	.string "needed. Thank you, {PLAYER}.\p"
	.string "I'll keep a record of every rift\n"
	.string "Pokémon you bring in: the Mega Log.\p"
	.string "You'll find it under LOG\n"
	.string "in your menu.$"

HotelZ_Text_MableRecorded:
	.string "Let me take a reading… there.\n"
	.string "Keep the stone, it's yours.\p"
	.string "Recorded! That's {STR_VAR_1}\n"
	.string "of 23.$"

HotelZ_Text_MableTally:
	.string "Your tally so far: {STR_VAR_1} of 23.\n"
	.string "Find a rift and keep going!$"

HotelZ_Text_MableAllStones:
	.string "That's the last one. 23 of 23.\n"
	.string "Every rogue Mega's stone.\p"
	.string "Now I can run the tests.\n"
	.string "Give me a day. I'll call you.$"

HotelZ_Text_MableTesting:
	.string "The tests are still running.\n"
	.string "I'll call you the moment I\l"
	.string "have something.$"

HotelZ_Text_MablePrimals:
	.string "Primal Groudon, Primal Kyogre,\n"
	.string "Mega Lugia and Mega Ho-Oh.\p"
	.string "They turn up in the portals.\n"
	.string "Catch all four, {PLAYER}.\l"
	.string "A knockout won't do it.$"

HotelZ_Text_MableRayquaza:
	.string "Mega Rayquaza is in a portal.\n"
	.string "Catch it before it tears the\l"
	.string "rifts wide open.$"

HotelZ_Text_MableWaiting:
	.string "Find a rift, take down the rogue\n"
	.string "Mega and bring me its stone.\l"
	.string "Two minutes. Go!$"

HotelZ_Text_MableAfter:
	.string "The rifts are settling. Thanks\n"
	.string "to you, {PLAYER}, Team MZ has\l"
	.string "every reading I could want.$"

HotelZ_Text_LidaDoors:
	.string "Oh, and Hotel Z is wherever you\n"
	.string "need it to be. Look for the sign!\p"
	.string "There's a door in Tradewind and one\n"
	.string "in Shin Tokyo, but you always walk\l"
	.string "out the door you came in.$"

HotelZ_Text_LidaIdle:
	.string "Team MZ's always short on hands.\n"
	.string "Glad you're on board!$"

HotelZ_Text_CanariIdle:
	.string "Don't mind the camera. Okay,\n"
	.string "maybe mind it a little.$"

HotelZ_Text_CanariPitch:
	.string "Hey, {PLAYER}! You beat me fair\n"
	.string "and square, and the Canatics\l"
	.string "LOVED it.\p"
	.string "Wanna team up for portal runs?\n"
	.string "Double the content!$"

HotelZ_Text_CanariJoins:
	.string "Let's go DYN4MO! I'll be right\n"
	.string "behind you in the rifts.$"

HotelZ_Text_CanariReady:
	.string "Partners! Still good to go…\n"
	.string "Or wanna go solo for a while?$"

HotelZ_Text_CanariSolo:
	.string "Aw, okay. Holler when you need\n"
	.string "a co-host!$"

HotelZ_Text_LidaPitch:
	.string "Okay, you beat me. Team MZ\n"
	.string "takes notes.\p"
	.string "How about it, {PLAYER}? Want a\n"
	.string "partner on your portal runs?$"

HotelZ_Text_LidaJoins:
	.string "Team MZ field unit, go!\n"
	.string "I'll follow you in.$"

HotelZ_Text_LidaReady:
	.string "We're partners! …Unless you\n"
	.string "want to go it alone for a bit?$"

HotelZ_Text_LidaSolo:
	.string "Got it. The door's always open!$"

HotelZ_Text_EmmaPitch:
	.string "You out-thought me in that\n"
	.string "rift. I don't say that often.\p"
	.string "Two minds on one case…\n"
	.string "Shall I join your portal runs?$"

HotelZ_Text_EmmaJoins:
	.string "Then it's settled. I'll take\n"
	.string "notes; you take the lead.$"

HotelZ_Text_EmmaReady:
	.string "Our partnership stands. …Or\n"
	.string "do you prefer to work alone?$"

HotelZ_Text_EmmaSolo:
	.string "Understood. Call on me when\n"
	.string "the trail goes cold.$"

HotelZ_Text_TauniePitch:
	.string "That battle was amazing!\n"
	.string "I've been thinking…\p"
	.string "Can I come along on your\n"
	.string "portal runs? Pretty please?$"

HotelZ_Text_TaunieJoins:
	.string "Yay! I'll stick right\n"
	.string "behind you!$"

HotelZ_Text_TaunieReady:
	.string "Best partners ever! …Wait,\n"
	.string "do you want to go alone?$"

HotelZ_Text_TaunieSolo:
	.string "Okay! I'll be right here when\n"
	.string "you want me back!$"

HotelZ_Text_TulipPitch:
	.string "Darling, you were radiant in\n"
	.string "that rift.\p"
	.string "Wouldn't the two of us look\n"
	.string "marvelous out there together?$"

HotelZ_Text_TulipJoins:
	.string "Marvelous. Lead the way,\n"
	.string "darling.$"

HotelZ_Text_TulipReady:
	.string "Still your partner, darling.\n"
	.string "…Or would you rather shine alone?$"

HotelZ_Text_TulipSolo:
	.string "As you wish. Do come back\n"
	.string "for me.$"

HotelZ_Text_JacinthePitch:
	.string "You bested me. I concede it\n"
	.string "graciously.\p"
	.string "I suppose I could lend you my\n"
	.string "talents in the rifts. Well?$"

HotelZ_Text_JacintheJoins:
	.string "Splendid. Do try to keep pace.$"

HotelZ_Text_JacintheReady:
	.string "We are partners, are we not?\n"
	.string "…Or must you go alone?$"

HotelZ_Text_JacintheSolo:
	.string "Very well. I shan't wait\n"
	.string "forever.$"
