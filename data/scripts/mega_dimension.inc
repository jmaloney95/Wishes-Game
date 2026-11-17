@ ============================================================================
@  WISHES OF TOMORROW -- THE MEGA DIMENSION
@
@  The portals (on source maps, each spot hidden by its FLAG_HIDE_MEGA_PORTAL_*),
@  the shared map scripts of every dimension map, the Mega encounter and the
@  collapse. The logic lives in src/wot_mega_dimension.c.
@ ============================================================================

@ --- a save from before the update, already past Tradewind -------------------
@ Shown on Continue until the intro has played (WotMegaDim_SaveMissedIntro).
MegaDim_EventScript_MissedIntroRumor::
	lockall
	msgbox MegaDim_Text_MissedIntroRumor, MSGBOX_DEFAULT
	releaseall
	end

MegaDim_Text_MissedIntroRumor:
	.string "Word is going around…\p"
	.string "Something tore open in the corridor\n"
	.string "north of Tradewind Town.\p"
	.string "Fly there and walk north to see it.$"

@ --- the portal in the normal world ------------------------------------------
@ A portal stays open until its dimension collapses; going back in finds the
@ same place (shinies re-rolled) for 5 seconds off its clock.
MegaDim_EventScript_Portal::
	lock
	playse SE_M_SUPERSONIC
	special WotMegaDim_CheckPortal
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_PortalFizzles
	copyvar VAR_0x8009, VAR_RESULT
	call MegaDim_EventScript_TaunieScout
	goto_if_eq VAR_0x8009, 1, MegaDim_EventScript_PortalReenter
	msgbox MegaDim_Text_PortalPrompt, MSGBOX_YESNO
	goto MegaDim_EventScript_PortalAnswer

MegaDim_EventScript_PortalReenter::
	msgbox MegaDim_Text_PortalReenterPrompt, MSGBOX_YESNO
MegaDim_EventScript_PortalAnswer::
	goto_if_eq VAR_RESULT, NO, MegaDim_EventScript_PortalDecline
	closemessage
	playse SE_M_TELEPORT
	special StartRiftDistortion
	delay 48
	playse SE_WARP_IN
	fadescreen FADE_TO_WHITE
	special EndRiftDistortion
	delay 20
	special WotMegaDim_EnterPortal
	release
	end

MegaDim_EventScript_PortalDecline::
	release
	end

@ The far side: the map has faded in still distorted, with the
@ player standing in the exit portal (FieldCB_PortalArrival). The distortion
@ settles, and the player steps out as it clears (The step waits
@ for most of it); the partner joins after, on the next step.
@ Frames between the distortion starting to ease (~24 frames to clear) and
@ the step out (16 frames), on both sides of the portal.
	.set MEGADIM_STEP_OUT_DELAY, 12
MegaDim_EventScript_PortalArrival::
	lockall
	special EaseOutRiftDistortion
	delay MEGADIM_STEP_OUT_DELAY
	applymovement OBJ_EVENT_ID_PLAYER, MegaDim_Movement_StepOutOfPortal
	waitmovement 0
	special WotMegaDim_PortalArrivalDone
	releaseall
	end

MegaDim_Movement_StepOutOfPortal:
	walk_down
	step_end

@ Back in the real world, after the exit portal or a collapse
@ (FieldCB_PortalReturn): the map fades in without the player, the portal
@ grows open, a distorted white flash brings the player into it, and they
@ step out as the distortion clears. A collapsed portal then shrinks away.
@ (A whiteout doesn't come this way: it lands in front of the portal.)
MegaDim_EventScript_PortalReturn::
	lockall
	special WotMegaDim_GetReturnPortal
	delay 16
	playse SE_M_SUPERSONIC
	special WotMegaDim_PortalGrow
	delay 12
	playse SE_WARP_OUT
	special StartRiftDistortionFull
	@ The flash is a hardware fade: a palette fade out and back in on the same
	@ outdoor map darkens it a step at night.
	fadescreenswapbuffers FADE_TO_WHITE
	special WotMegaDim_ShowReturningPlayer
	fadescreenswapbuffers FADE_FROM_WHITE
	special EaseOutRiftDistortion
	delay MEGADIM_STEP_OUT_DELAY
	applymovement OBJ_EVENT_ID_PLAYER, MegaDim_Movement_StepOutOfPortal
	waitmovement 0
	goto_if_eq VAR_0x8005, FALSE, MegaDim_EventScript_PortalReturnDone
	delay 16
	playse SE_WARP_IN
	special WotMegaDim_PortalShrink
	removeobject VAR_0x8004
MegaDim_EventScript_PortalReturnDone::
	releaseall
	end

@ Taunie along: she names what's waiting inside.
MegaDim_EventScript_TaunieScout::
	special WotMegaDim_TaunieScout
	goto_if_eq VAR_RESULT, 0, MegaDim_EventScript_TaunieScoutEnd
	setspeaker SP_NAME_TAUNIE
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_TaunieScoutRival
	goto_if_eq VAR_RESULT, 3, MegaDim_EventScript_TaunieScoutLegend
	msgbox MegaDim_Text_TaunieScoutMega, MSGBOX_DEFAULT
	goto MegaDim_EventScript_TaunieScoutDone

MegaDim_EventScript_TaunieScoutRival::
	msgbox MegaDim_Text_TaunieScoutRival, MSGBOX_DEFAULT
	goto MegaDim_EventScript_TaunieScoutDone

MegaDim_EventScript_TaunieScoutLegend::
	msgbox MegaDim_Text_TaunieScoutLegend, MSGBOX_DEFAULT
MegaDim_EventScript_TaunieScoutDone::
	@ friendship Lv3+: the wild shinies; Lv4: the Mega's own shine
	call_if_eq VAR_0x8005, 1, MegaDim_EventScript_TaunieScoutShinyYes
	call_if_eq VAR_0x8005, 2, MegaDim_EventScript_TaunieScoutShinyNo
	call_if_eq VAR_0x8006, 1, MegaDim_EventScript_TaunieScoutMegaShiny
	setspeaker SP_NAME_NONE
MegaDim_EventScript_TaunieScoutEnd::
	return

MegaDim_EventScript_TaunieScoutShinyYes::
	msgbox MegaDim_Text_TaunieScoutShinyYes, MSGBOX_DEFAULT
	return

MegaDim_EventScript_TaunieScoutShinyNo::
	msgbox MegaDim_Text_TaunieScoutShinyNo, MSGBOX_DEFAULT
	return

MegaDim_EventScript_TaunieScoutMegaShiny::
	msgbox MegaDim_Text_TaunieScoutMegaShiny, MSGBOX_DEFAULT
	return

@ Too little time left to go back in: it collapses (WotMegaDim_CheckPortal
@ already freed it).
MegaDim_EventScript_PortalFizzles::
	msgbox MegaDim_Text_PortalFizzles, MSGBOX_DEFAULT
	closemessage
	playse SE_WARP_IN
	removeobject VAR_LAST_TALKED
	release
	end

@ --- shared map scripts of every dimension map ---------------------------------
MegaDim_OnTransition::
	special WotMegaDim_SetMegaGfx
	end

MegaDim_OnResume::
	special WotMegaDim_StartHud
	end

@ WotMegaDim_Tick raises VAR_TEMP_1 when the countdown hits zero. ON_FRAME
@ only runs while the player is free, so a battle in progress finishes first.
MegaDim_OnFrame::
	map_script_2 VAR_TEMP_1, 1, MegaDim_EventScript_Collapse
	.2byte 0

@ --- the Mega ------------------------------------------------------------------
MegaDim_EventScript_Mega::
	lock
	faceplayer
	special WotMegaDim_GetMega
	waitse
	playmoncry VAR_0x8004, CRY_MODE_ENCOUNTER
	waitmoncry
	bufferspeciesname STR_VAR_1, VAR_0x8006
	call_if_eq VAR_0x8007, FALSE, MegaDim_EventScript_MegaRadiates
	call_if_eq VAR_0x8007, TRUE, MegaDim_EventScript_LegendRadiates
	closemessage
	special WotMegaDim_SetupMegaBattle
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special WotMegaDim_EndMegaBattle
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, MegaDim_EventScript_MegaCaught
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, MegaDim_EventScript_MegaDefeated
	@ Ran away (or teleported): the Mega is still there.
	release
	end

@ Z-Shards: what a rift battle leaves behind. Wild Pokemon's flash
@ on the HUD instead (WotMegaDim_MarkWildGone), so they cost no clock time.
MegaDim_EventScript_BossShards::
	special WotZShards_GiveBoss
	msgbox MegaDim_Text_ShardsFound, MSGBOX_DEFAULT
	return

@ VAR_0x8004 = how many.
MegaDim_EventScript_GiveShards::
	special WotZShards_Give
	msgbox MegaDim_Text_ShardsFound, MSGBOX_DEFAULT
	return

MegaDim_EventScript_MegaRadiates::
	msgbox MegaDim_Text_MegaRadiates, MSGBOX_DEFAULT
	return

MegaDim_EventScript_LegendRadiates::
	msgbox MegaDim_Text_LegendRadiates, MSGBOX_DEFAULT
	return

MegaDim_EventScript_MegaCaught::
	removeobject VAR_LAST_TALKED
	special WotMegaDim_GetMega
	goto_if_eq VAR_0x8007, TRUE, MegaDim_EventScript_LegendCaught
	bufferspeciesname STR_VAR_1, VAR_0x8006
	bufferitemname STR_VAR_2, VAR_0x8005
	msgbox MegaDim_Text_CaughtWithStone, MSGBOX_DEFAULT
	call MegaDim_EventScript_BossShards
	special WotMegaDim_MarkClaimed
	goto MegaDim_EventScript_MegaGoneCollapse

@ A KO leaves the stone behind -- the first time only. Rematches (every Mega
@ already claimed) pay nothing extra. The battle may have used the special
@ vars and string buffers, so ask again.
MegaDim_EventScript_MegaDefeated::
	removeobject VAR_LAST_TALKED
	special WotMegaDim_GetMega
	bufferspeciesname STR_VAR_1, VAR_0x8006
	goto_if_eq VAR_0x8007, TRUE, MegaDim_EventScript_LegendRetreats
	goto_if_eq VAR_RESULT, TRUE, MegaDim_EventScript_MegaFaded
	msgbox MegaDim_Text_StoneLeftBehind, MSGBOX_DEFAULT
	giveitem VAR_0x8005
	call MegaDim_EventScript_BossShards
	special WotMegaDim_MarkClaimed
	goto MegaDim_EventScript_MegaGoneCollapse

MegaDim_EventScript_MegaFaded::
	msgbox MegaDim_Text_MegaFaded, MSGBOX_DEFAULT
	call MegaDim_EventScript_BossShards
	goto MegaDim_EventScript_MegaGoneCollapse

@ --- Mable's legends (Primal Groudon/Kyogre, Mega Lugia/Ho-Oh, Mega Rayquaza) ---
@ Only a catch counts. Knocked out, it retreats deeper into the rift and
@ turns up again in a later portal; the portal collapses either way.
MegaDim_EventScript_LegendCaught::
	call MegaDim_EventScript_BossShards
	special WotMegaDim_MarkClaimed
	goto_if_unset FLAG_WOT_CAUGHT_MEGA_RAYQUAZA, MegaDim_EventScript_MegaGoneCollapse
	goto_if_quest_complete QUEST_SKY_HIGH_MEGA, MegaDim_EventScript_MegaGoneCollapse
	completequest QUEST_SKY_HIGH_MEGA
	goto MegaDim_EventScript_MegaGoneCollapse

MegaDim_EventScript_LegendRetreats::
	msgbox MegaDim_Text_LegendRetreats, MSGBOX_DEFAULT
	goto MegaDim_EventScript_MegaGoneCollapse

@ --- your partner ----------------------------------------------------
@ The recruited rival follows the player inside the dimension (an NPC
@ follower) and fights beside them; talking to them gets their line.
@ Their line depends on friendship (src/data/wot_partner_lines.h).
MegaDim_EventScript_PartnerTalk::
	lock
	special WotMegaDim_BufferPartner
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_PartnerCanari
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_PartnerLida
	goto_if_eq VAR_RESULT, 3, MegaDim_EventScript_PartnerEmma
	goto_if_eq VAR_RESULT, 4, MegaDim_EventScript_PartnerTaunie
	goto_if_eq VAR_RESULT, 5, MegaDim_EventScript_PartnerTulip
	goto_if_eq VAR_RESULT, 6, MegaDim_EventScript_PartnerJacinthe
	release
	end

MegaDim_EventScript_PartnerCanari::
	setspeaker SP_NAME_CANARI
	special WotPartner_BufferFieldLine
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
	goto MegaDim_EventScript_PartnerTalkEnd

MegaDim_EventScript_PartnerLida::
	setspeaker SP_NAME_LIDA
	special WotPartner_BufferFieldLine
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
	goto MegaDim_EventScript_PartnerTalkEnd

MegaDim_EventScript_PartnerEmma::
	setspeaker SP_NAME_EMMA
	special WotPartner_BufferFieldLine
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
	goto MegaDim_EventScript_PartnerTalkEnd

MegaDim_EventScript_PartnerTaunie::
	setspeaker SP_NAME_TAUNIE
	special WotPartner_BufferFieldLine
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
	goto MegaDim_EventScript_PartnerTalkEnd

MegaDim_EventScript_PartnerTulip::
	setspeaker SP_NAME_TULIP
	special WotPartner_BufferFieldLine
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
	goto MegaDim_EventScript_PartnerTalkEnd

MegaDim_EventScript_PartnerJacinthe::
	setspeaker SP_NAME_JACINTHE
	special WotPartner_BufferFieldLine
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
MegaDim_EventScript_PartnerTalkEnd::
	closemessage
	setspeaker SP_NAME_NONE
	release
	end

@ A partner's friendship reached a new level (every 5 portal runs
@ together). They say so themselves, with their nameplate and portrait.
MegaDim_EventScript_FriendshipUp::
	lockall
	special WotMegaDim_TakeFriendshipUp
	goto_if_eq VAR_RESULT, 0, MegaDim_EventScript_FriendshipUpEnd
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_FriendshipUpCanari
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_FriendshipUpLida
	goto_if_eq VAR_RESULT, 3, MegaDim_EventScript_FriendshipUpEmma
	goto_if_eq VAR_RESULT, 4, MegaDim_EventScript_FriendshipUpTaunie
	goto_if_eq VAR_RESULT, 5, MegaDim_EventScript_FriendshipUpTulip
	setspeaker SP_NAME_JACINTHE
	goto MegaDim_EventScript_FriendshipUpSay
MegaDim_EventScript_FriendshipUpCanari::
	setspeaker SP_NAME_CANARI
	goto MegaDim_EventScript_FriendshipUpSay
MegaDim_EventScript_FriendshipUpLida::
	setspeaker SP_NAME_LIDA
	goto MegaDim_EventScript_FriendshipUpSay
MegaDim_EventScript_FriendshipUpEmma::
	setspeaker SP_NAME_EMMA
	goto MegaDim_EventScript_FriendshipUpSay
MegaDim_EventScript_FriendshipUpTaunie::
	setspeaker SP_NAME_TAUNIE
	goto MegaDim_EventScript_FriendshipUpSay
MegaDim_EventScript_FriendshipUpTulip::
	setspeaker SP_NAME_TULIP
MegaDim_EventScript_FriendshipUpSay::
	playfanfare MUS_LEVEL_UP
	msgbox MegaDim_Text_FriendshipUp, MSGBOX_DEFAULT
	waitfanfare
	closemessage
	setspeaker SP_NAME_NONE
MegaDim_EventScript_FriendshipUpEnd::
	releaseall
	end

@ A portal collapsed with a partner along (not a whiteout): the secret quest.
MegaDim_EventScript_TwoHearts::
	lockall
	special WotMegaDim_TakeTwoHearts
	goto_if_quest_complete QUEST_TWO_HEARTS, MegaDim_EventScript_TwoHeartsDone
	msgbox MegaDim_Text_TwoHearts, MSGBOX_DEFAULT
	closemessage
	completequest QUEST_TWO_HEARTS
MegaDim_EventScript_TwoHeartsDone::
	releaseall
	end

@ --- calls (WotMegaDim TryStartFieldCall, once the player is free) --------------
@ Lida, when the 8:00 portals open: the towns they opened in (one per line).
MegaDim_EventScript_PortalAlert::
	lockall
	special WotMegaDim_BufferPortalAlert
	goto_if_eq VAR_RESULT, 0, MegaDim_EventScript_CallEnd
	playse SE_POKENAV_CALL
	waitse
	setspeaker SP_NAME_LIDA
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_PortalAlert1
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_PortalAlert2
	msgbox MegaDim_Text_PortalAlert3, MSGBOX_DEFAULT
	goto MegaDim_EventScript_PortalAlertClock

MegaDim_EventScript_PortalAlert1::
	msgbox MegaDim_Text_PortalAlert1, MSGBOX_DEFAULT
	goto MegaDim_EventScript_PortalAlertClock

MegaDim_EventScript_PortalAlert2::
	msgbox MegaDim_Text_PortalAlert2, MSGBOX_DEFAULT
@ The clock line: 2:00, or Emma's longer clock when she's the partner.
MegaDim_EventScript_PortalAlertClock::
	special WotMegaDim_BufferAlertClock
	goto_if_eq VAR_RESULT, TRUE, MegaDim_EventScript_PortalAlertEmmaClock
	msgbox MegaDim_Text_PortalAlertClock, MSGBOX_DEFAULT
	goto MegaDim_EventScript_CallEnd

MegaDim_EventScript_PortalAlertEmmaClock::
	msgbox MegaDim_Text_PortalAlertEmmaClock, MSGBOX_DEFAULT
	goto MegaDim_EventScript_CallEnd

@ Mable, the day after her tests start: Primal Signatures.
MegaDim_EventScript_MablePrimalCall::
	lockall
	playse SE_POKENAV_CALL
	waitse
	setspeaker SP_NAME_MABLE
	msgbox MegaDim_Text_MablePrimalCall, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	special WotMegaDim_StartLegendQuest
	startquest QUEST_PRIMAL_SIGNATURES
	releaseall
	end

@ Mable, once all four Primal Signatures are caught: Sky-High Mega.
MegaDim_EventScript_MableRayquazaCall::
	lockall
	playse SE_POKENAV_CALL
	waitse
	setspeaker SP_NAME_MABLE
	msgbox MegaDim_Text_MableRayquazaCall, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	special WotMegaDim_StartLegendQuest
	completequest QUEST_PRIMAL_SIGNATURES
	startquest QUEST_SKY_HIGH_MEGA
	releaseall
	end

@ Emma, at 5 Cells: The Long Stay.
MegaDim_EventScript_EmmaCellCall::
	lockall
	playse SE_POKENAV_CALL
	waitse
	setspeaker SP_NAME_EMMA
	msgbox MegaDim_Text_EmmaCellCall, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	startquest QUEST_THE_LONG_STAY
	releaseall
	end

@ Mable, with Mega Rayquaza caught: it isn't over.
MegaDim_EventScript_MableDrainCall::
	lockall
	playse SE_POKENAV_CALL
	waitse
	setspeaker SP_NAME_MABLE
	msgbox MegaDim_Text_MableDrainCall, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	setvar VAR_0x8004, 1
	special WotZygarde_SetStage
	releaseall
	end

MegaDim_EventScript_CallEnd::
	closemessage
	setspeaker SP_NAME_NONE
	releaseall
	end

@ --- visible wild Pokemon ----------------------------------------------------------
@ Placed each visit by WotMegaDim SetupWildMons (species + shininess are in the
@ object's graphics id). Talk to one: its cry, then a battle with exactly that
@ Pokemon. Beaten or caught, it's gone for as long as the portal is open
@ (WotMegaDim_MarkWildGone); run away and it's still there.
MegaDim_EventScript_WildMon::
	specialvar VAR_RESULT, WotZygarde_IsTalkedCell
	goto_if_eq VAR_RESULT, TRUE, MegaDim_EventScript_Cell
	lock
	faceplayer
	special WotMegaDim_BufferTalkedMon
	waitse
	playmoncry VAR_0x8004, CRY_MODE_ENCOUNTER
	waitmoncry
	special WotMegaDim_StartOverworldMonBattle
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_WON, MegaDim_EventScript_WildMonGone
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, MegaDim_EventScript_WildMonGone
	release
	end

@ A Zygarde Cell, in a dealt slot. It costs no time: no text after
@ the first, and it buys some back. The HUD flashes "+15".
MegaDim_EventScript_Cell::
	lock
	playse SE_PIN
	special WotZygarde_TakeCell
	removeobject VAR_LAST_TALKED
	goto_if_eq VAR_RESULT, FALSE, MegaDim_EventScript_CellEnd
	msgbox MegaDim_Text_FirstCell, MSGBOX_DEFAULT
	closemessage
MegaDim_EventScript_CellEnd::
	release
	end

MegaDim_EventScript_WildMonGone::
	special WotMegaDim_MarkWildGone
	removeobject VAR_LAST_TALKED
	release
	end

@ --- the rivals ------------------------------------------------------------------
@ About one portal in five (WotMegaDim OpenPortal) a Legends Z-A character beat
@ the player to the Mega: they stand on its slot with it in their party (the
@ 3rd slot is rewritten to the portal's Mega, WotMegaDim_AdjustRivalParty).
@ One of three openers (and its matching defeat line) at random; a win hands
@ over the stone the first time (same rule as a KO) and collapses the portal,
@ a loss whites out back to it (also a collapse). The win sets their trainer
@ flag for good: they're never met in a portal again, and check in at Hotel Z
@ as a recruitable partner.
MegaDim_EventScript_Rival::
	lock
	faceplayer
	special WotMegaDim_GetRival
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_RivalCanari
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_RivalLida
	goto_if_eq VAR_RESULT, 3, MegaDim_EventScript_RivalEmma
	goto_if_eq VAR_RESULT, 4, MegaDim_EventScript_RivalTaunie
	goto_if_eq VAR_RESULT, 5, MegaDim_EventScript_RivalTulip
	goto_if_eq VAR_RESULT, 6, MegaDim_EventScript_RivalJacinthe
	release
	end

MegaDim_EventScript_RivalCanari::
	setspeaker SP_NAME_CANARI
	random 3
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_RivalCanari2
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_RivalCanari3
	msgbox MegaDim_Text_CanariIntro1, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_CANARI, MegaDim_Text_CanariDefeat1
	goto MegaDim_EventScript_RivalCanariAfter

MegaDim_EventScript_RivalCanari2::
	msgbox MegaDim_Text_CanariIntro2, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_CANARI, MegaDim_Text_CanariDefeat2
	goto MegaDim_EventScript_RivalCanariAfter

MegaDim_EventScript_RivalCanari3::
	msgbox MegaDim_Text_CanariIntro3, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_CANARI, MegaDim_Text_CanariDefeat3
	goto MegaDim_EventScript_RivalCanariAfter

MegaDim_EventScript_RivalCanariAfter::
	setspeaker SP_NAME_CANARI
	msgbox MegaDim_Text_CanariPost, MSGBOX_DEFAULT
	goto MegaDim_EventScript_RivalBeaten

MegaDim_EventScript_RivalLida::
	setspeaker SP_NAME_LIDA
	random 3
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_RivalLida2
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_RivalLida3
	msgbox MegaDim_Text_LidaIntro1, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_LIDA, MegaDim_Text_LidaDefeat1
	goto MegaDim_EventScript_RivalLidaAfter

MegaDim_EventScript_RivalLida2::
	msgbox MegaDim_Text_LidaIntro2, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_LIDA, MegaDim_Text_LidaDefeat2
	goto MegaDim_EventScript_RivalLidaAfter

MegaDim_EventScript_RivalLida3::
	msgbox MegaDim_Text_LidaIntro3, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_LIDA, MegaDim_Text_LidaDefeat3
	goto MegaDim_EventScript_RivalLidaAfter

MegaDim_EventScript_RivalLidaAfter::
	setspeaker SP_NAME_LIDA
	msgbox MegaDim_Text_LidaPost, MSGBOX_DEFAULT
	goto MegaDim_EventScript_RivalBeaten

MegaDim_EventScript_RivalEmma::
	setspeaker SP_NAME_EMMA
	random 3
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_RivalEmma2
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_RivalEmma3
	msgbox MegaDim_Text_EmmaIntro1, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_EMMA, MegaDim_Text_EmmaDefeat1
	goto MegaDim_EventScript_RivalEmmaAfter

MegaDim_EventScript_RivalEmma2::
	msgbox MegaDim_Text_EmmaIntro2, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_EMMA, MegaDim_Text_EmmaDefeat2
	goto MegaDim_EventScript_RivalEmmaAfter

MegaDim_EventScript_RivalEmma3::
	msgbox MegaDim_Text_EmmaIntro3, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_EMMA, MegaDim_Text_EmmaDefeat3
	goto MegaDim_EventScript_RivalEmmaAfter

MegaDim_EventScript_RivalEmmaAfter::
	setspeaker SP_NAME_EMMA
	msgbox MegaDim_Text_EmmaPost, MSGBOX_DEFAULT
	goto MegaDim_EventScript_RivalBeaten

MegaDim_EventScript_RivalTaunie::
	setspeaker SP_NAME_TAUNIE
	random 3
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_RivalTaunie2
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_RivalTaunie3
	msgbox MegaDim_Text_TaunieIntro1, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_TAUNIE, MegaDim_Text_TaunieDefeat1
	goto MegaDim_EventScript_RivalTaunieAfter

MegaDim_EventScript_RivalTaunie2::
	msgbox MegaDim_Text_TaunieIntro2, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_TAUNIE, MegaDim_Text_TaunieDefeat2
	goto MegaDim_EventScript_RivalTaunieAfter

MegaDim_EventScript_RivalTaunie3::
	msgbox MegaDim_Text_TaunieIntro3, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_TAUNIE, MegaDim_Text_TaunieDefeat3
	goto MegaDim_EventScript_RivalTaunieAfter

MegaDim_EventScript_RivalTaunieAfter::
	setspeaker SP_NAME_TAUNIE
	msgbox MegaDim_Text_TauniePost, MSGBOX_DEFAULT
	goto MegaDim_EventScript_RivalBeaten

MegaDim_EventScript_RivalTulip::
	setspeaker SP_NAME_TULIP
	random 3
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_RivalTulip2
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_RivalTulip3
	msgbox MegaDim_Text_TulipIntro1, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_TULIP, MegaDim_Text_TulipDefeat1
	goto MegaDim_EventScript_RivalTulipAfter

MegaDim_EventScript_RivalTulip2::
	msgbox MegaDim_Text_TulipIntro2, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_TULIP, MegaDim_Text_TulipDefeat2
	goto MegaDim_EventScript_RivalTulipAfter

MegaDim_EventScript_RivalTulip3::
	msgbox MegaDim_Text_TulipIntro3, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_TULIP, MegaDim_Text_TulipDefeat3
	goto MegaDim_EventScript_RivalTulipAfter

MegaDim_EventScript_RivalTulipAfter::
	setspeaker SP_NAME_TULIP
	msgbox MegaDim_Text_TulipPost, MSGBOX_DEFAULT
	goto MegaDim_EventScript_RivalBeaten

MegaDim_EventScript_RivalJacinthe::
	setspeaker SP_NAME_JACINTHE
	random 3
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_RivalJacinthe2
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_RivalJacinthe3
	msgbox MegaDim_Text_JacintheIntro1, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_JACINTHE, MegaDim_Text_JacintheDefeat1
	goto MegaDim_EventScript_RivalJacintheAfter

MegaDim_EventScript_RivalJacinthe2::
	msgbox MegaDim_Text_JacintheIntro2, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_JACINTHE, MegaDim_Text_JacintheDefeat2
	goto MegaDim_EventScript_RivalJacintheAfter

MegaDim_EventScript_RivalJacinthe3::
	msgbox MegaDim_Text_JacintheIntro3, MSGBOX_DEFAULT
	closemessage
	setspeaker SP_NAME_NONE
	trainerbattle_no_intro TRAINER_MEGADIM_JACINTHE, MegaDim_Text_JacintheDefeat3
	goto MegaDim_EventScript_RivalJacintheAfter

MegaDim_EventScript_RivalJacintheAfter::
	setspeaker SP_NAME_JACINTHE
	msgbox MegaDim_Text_JacinthePost, MSGBOX_DEFAULT
	goto MegaDim_EventScript_RivalBeaten

MegaDim_EventScript_RivalBeaten::
	special WotMegaDim_GetMega
	goto_if_eq VAR_RESULT, TRUE, MegaDim_EventScript_RivalLeaves
	msgbox MegaDim_Text_RivalHandsStone, MSGBOX_DEFAULT
	setspeaker SP_NAME_NONE
	giveitem VAR_0x8005
	special WotMegaDim_MarkClaimed
MegaDim_EventScript_RivalLeaves::
	closemessage
	setspeaker SP_NAME_NONE
	setvar VAR_0x8004, 8
	call MegaDim_EventScript_GiveShards
	closemessage
	playse SE_WARP_OUT
	removeobject VAR_LAST_TALKED
	goto MegaDim_EventScript_MegaGoneCollapse

MegaDim_Text_RivalHandsStone:
	.string "Here. A deal's a deal.$"

MegaDim_Text_CanariIntro1:
	.string "Ha, beat ya to it! Let's go\n"
	.string "DYN4MO! This Mega's going\p"
	.string "straight on stream!\n"
	.string "Canatics, you seeing this?!$"

MegaDim_Text_CanariDefeat1:
	.string "Ugh, that's a clip I'm\n"
	.string "deleting... GG, though.$"

MegaDim_Text_CanariIntro2:
	.string "Chat called it -- you'd show\n"
	.string "up thirty seconds too late!\p"
	.string "Wanna fight for it? Don't be\n"
	.string "shy, the Canatics love a duel!$"

MegaDim_Text_CanariDefeat2:
	.string "The Canatics are spamming\n"
	.string "your name now. Rude.$"

MegaDim_Text_CanariIntro3:
	.string "Oh hey! You're just in time\n"
	.string "for the encore. Let's go\p"
	.string "DYN4MO! One battle, live,\n"
	.string "winner takes the stone!$"

MegaDim_Text_CanariDefeat3:
	.string "Stream's over, folks!\n"
	.string "...Okay, you win this one.$"

MegaDim_Text_CanariPost:
	.string "Don't forget to like and\n"
	.string "subscribe! ...What? Habit.$"

MegaDim_Text_LidaIntro1:
	.string "Ha, beat ya to it! Team MZ\n"
	.string "always gets there first.\p"
	.string "Still want it? Prove it!$"

MegaDim_Text_LidaDefeat1:
	.string "Okay, okay, you earned it.$"

MegaDim_Text_LidaIntro2:
	.string "You're fast, {PLAYER}... but\n"
	.string "not Team-MZ fast.\p"
	.string "This Mega's ours. Unless you\n"
	.string "can take it off my hands?$"

MegaDim_Text_LidaDefeat2:
	.string "Wow. Mable's gonna want\n"
	.string "to hear about this.$"

MegaDim_Text_LidaIntro3:
	.string "Ha! Caught it before the rift\n"
	.string "even finished glowing!\p"
	.string "Come on, one round. For fun.\n"
	.string "And the stone.$"

MegaDim_Text_LidaDefeat3:
	.string "Okay, that was fun AND\n"
	.string "humiliating. Nice.$"

MegaDim_Text_LidaPost:
	.string "Team MZ could use someone\n"
	.string "like you. Just saying!$"

MegaDim_Text_EmmaIntro1:
	.string "Ha, beat ya to it. I tracked\n"
	.string "these readings here hours ago.\p"
	.string "Want the stone? Out-think me.$"

MegaDim_Text_EmmaDefeat1:
	.string "Case closed... in your favor.$"

MegaDim_Text_EmmaIntro2:
	.string "The trail was obvious, if you\n"
	.string "know where to look.\p"
	.string "Shall we settle who solved\n"
	.string "this one?$"

MegaDim_Text_EmmaDefeat2:
	.string "I missed a clue. Somewhere.\n"
	.string "Well played.$"

MegaDim_Text_EmmaIntro3:
	.string "You followed the same rift\n"
	.string "I did. Clever.\p"
	.string "But the culprit's already in\n"
	.string "custody. Care to appeal?$"

MegaDim_Text_EmmaDefeat3:
	.string "Evidence overturned.\n"
	.string "The stone's yours.$"

MegaDim_Text_EmmaPost:
	.string "Every rift leaves a trace.\n"
	.string "I'll find the next one first.$"

MegaDim_Text_TaunieIntro1:
	.string "Ha, beat ya to it!\n"
	.string "Fair's fair, though.\p"
	.string "Battle me for it!$"

MegaDim_Text_TaunieDefeat1:
	.string "Wow, you're good!$"

MegaDim_Text_TaunieIntro2:
	.string "Oh! You came for this one too?\n"
	.string "I got lucky this time.\p"
	.string "Let's see if luck holds up\n"
	.string "in a battle!$"

MegaDim_Text_TaunieDefeat2:
	.string "Nope! Luck ran out!$"

MegaDim_Text_TaunieIntro3:
	.string "I just caught it! Isn't it\n"
	.string "amazing? ...Wait, you want it?\p"
	.string "Okay, okay! Winner keeps\n"
	.string "the stone, deal?$"

MegaDim_Text_TaunieDefeat3:
	.string "Deal's a deal...\n"
	.string "I'm still proud of it!$"

MegaDim_Text_TauniePost:
	.string "Let's race to the next rift!\n"
	.string "I won't lose twice!$"

MegaDim_Text_TulipIntro1:
	.string "Oh, beat you to it, darling.\n"
	.string "Shall we see who wears\p"
	.string "this Mega better?$"

MegaDim_Text_TulipDefeat1:
	.string "Beautifully done.$"

MegaDim_Text_TulipIntro2:
	.string "A rogue Mega, such raw beauty.\n"
	.string "I simply had to have it.\p"
	.string "But you're welcome to try\n"
	.string "and take it, darling.$"

MegaDim_Text_TulipDefeat2:
	.string "Flawless. Even I can't\n"
	.string "fault that technique.$"

MegaDim_Text_TulipIntro3:
	.string "The light in here is all\n"
	.string "wrong, but the Mega? Perfect.\p"
	.string "Shall we put on a show for\n"
	.string "it, darling?$"

MegaDim_Text_TulipDefeat3:
	.string "Darling, you were radiant.\n"
	.string "Truly.$"

MegaDim_Text_TulipPost:
	.string "Beauty fades, darling.\n"
	.string "Strength like that doesn't.$"

MegaDim_Text_JacintheIntro1:
	.string "Ha, beat ya to it. A Mega\n"
	.string "this rare belongs with\p"
	.string "someone of taste... unless\n"
	.string "you show me otherwise.$"

MegaDim_Text_JacintheDefeat1:
	.string "Hmph. Very well.$"

MegaDim_Text_JacintheIntro2:
	.string "You're late. How very\n"
	.string "common of you.\p"
	.string "Still, I'll allow you one\n"
	.string "chance. Do try to impress.$"

MegaDim_Text_JacintheDefeat2:
	.string "...Impressive.\n"
	.string "Don't let it go to your head.$"

MegaDim_Text_JacintheIntro3:
	.string "This Mega deserves a\n"
	.string "proper owner. Me.\p"
	.string "Unless you'd care to make\n"
	.string "an argument for yourself?$"

MegaDim_Text_JacintheDefeat3:
	.string "A compelling argument.\n"
	.string "I concede, graciously.$"

MegaDim_Text_JacinthePost:
	.string "Next time, I shan't be so\n"
	.string "generous. Good day.$"

@ --- leaving -------------------------------------------------------------------
@ The exit portal: home, and the portal there stays open.
MegaDim_EventScript_ExitPortal::
	lock
	msgbox MegaDim_Text_ExitPrompt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MegaDim_EventScript_PortalDecline
	closemessage
	call MegaDim_EventScript_RiftOut
	special WotMegaDim_LeaveOut
	release
	end

@ The clock ran out (ON_FRAME, VAR_TEMP_1 = 1): the portal collapses.
MegaDim_EventScript_Collapse::
	lockall
	setvar VAR_TEMP_1, 2
	call MegaDim_EventScript_CollapseShake
	call MegaDim_EventScript_RiftOut
	special WotMegaDim_CollapseOut
	releaseall
	end

@ The rogue Mega was caught or KO'd, or the rival holding it beaten: the
@ portal collapses too. VAR_TEMP_1 = 2 so the clock's collapse can't follow.
MegaDim_EventScript_MegaGoneCollapse::
	closemessage
	setvar VAR_TEMP_1, 2
	call MegaDim_EventScript_CollapseShake
	call MegaDim_EventScript_RiftOut
	special WotMegaDim_CollapseOut
	release
	end

MegaDim_EventScript_CollapseShake::
	playse SE_THUNDER
	setvar VAR_0x8004, 2  @ vertical pan
	setvar VAR_0x8005, 2  @ horizontal pan
	setvar VAR_0x8006, 10 @ num shakes
	setvar VAR_0x8007, 4  @ shake delay
	special ShakeCamera
	waitstate
	msgbox MegaDim_Text_Collapsing, MSGBOX_DEFAULT
	closemessage
	playse SE_THUNDER2
	setvar VAR_0x8004, 3  @ vertical pan
	setvar VAR_0x8005, 3  @ horizontal pan
	setvar VAR_0x8006, 16 @ num shakes
	setvar VAR_0x8007, 3  @ shake delay
	special ShakeCamera
	waitstate
	return

MegaDim_EventScript_RiftOut::
	playse SE_M_SUPERSONIC
	special StartRiftDistortion
	delay 48
	playse SE_WARP_IN
	fadescreen FADE_TO_WHITE
	special EndRiftDistortion
	delay 20
	return

@ --- text ----------------------------------------------------------------------
MegaDim_Text_PortalPrompt:
	.string "A rift crackles with strange\n"
	.string "energy. Beyond it, the world looks\l"
	.string "drained of all color…\p"
	.string "Step inside?$"

MegaDim_Text_PortalReenterPrompt:
	.string "The rift is still open, but it's\n"
	.string "fading: {STR_VAR_1} left inside.\p"
	.string "Going back in costs 5 seconds.\n"
	.string "Step inside?$"

MegaDim_Text_TaunieScoutMega:
	.string "Ooh, I'm reading a Mega {STR_VAR_2}\n"
	.string "in there! Let's go get it!$"

MegaDim_Text_TaunieScoutRival:
	.string "Ooh, I'm reading a Mega {STR_VAR_2}\n"
	.string "in there…\p"
	.string "…and {STR_VAR_1} beat us to it!\n"
	.string "Gonna have to go through them.$"

MegaDim_Text_TaunieScoutShinyYes:
	.string "Ooh, and something in there's\n"
	.string "sparkling. A shiny Pokémon!$"

MegaDim_Text_TaunieScoutShinyNo:
	.string "No shiny sparkle in there\n"
	.string "this time, though.$"

MegaDim_Text_TaunieScoutMegaShiny:
	.string "Wait… the Mega itself is\n"
	.string "shimmering. It's shiny!$"

MegaDim_Text_TaunieScoutLegend:
	.string "Whoa… that's not a Mega.\n"
	.string "That's {STR_VAR_2}! Like, the real one!$"

MegaDim_Text_PortalFizzles:
	.string "The rift flickers…\n"
	.string "and fizzles out.$"

MegaDim_Text_ExitPrompt:
	.string "The rift home is still open.\n"
	.string "You can come back while it lasts.\p"
	.string "Leave the Mega Dimension?$"

MegaDim_Text_MegaRadiates:
	.string "{STR_VAR_1} is radiating\n"
	.string "Mega energy!$"

MegaDim_Text_StoneLeftBehind:
	.string "{STR_VAR_1} faded away…\n"
	.string "It left something behind!$"

MegaDim_Text_CaughtWithStone:
	.string "{STR_VAR_1} is holding its\n"
	.string "{STR_VAR_2}!$"

MegaDim_Text_MegaFaded:
	.string "{STR_VAR_1} faded away…$"

MegaDim_Text_TwoHearts:
	.string "You and your partner rode the\n"
	.string "rift all the way to its collapse.\p"
	.string "Some things are easier with\n"
	.string "someone at your side.$"

MegaDim_Text_FriendshipUp:
	.string "We make a good team, you know.\p"
	.string "Friendship rose to Lv{STR_VAR_1}!\n"
	.string "{STR_VAR_2}.{STR_VAR_3}$"

MegaDim_Text_LegendRadiates:
	.string "{STR_VAR_1} is giving off\n"
	.string "an immense power!$"

MegaDim_Text_LegendRetreats:
	.string "{STR_VAR_1} retreated deeper\n"
	.string "into the rift…\p"
	.string "It will surface again somewhere.$"

MegaDim_Text_PortalAlert1:
	.string "{PLAYER}, heads up! A new rift\n"
	.string "just opened in:\p"
	.string "{STR_VAR_1}!$"

MegaDim_Text_PortalAlert2:
	.string "{PLAYER}, heads up! New rifts\n"
	.string "just opened in:\p"
	.string "{STR_VAR_1}\n"
	.string "and {STR_VAR_2}!$"

MegaDim_Text_PortalAlert3:
	.string "{PLAYER}, heads up! New rifts\n"
	.string "just opened in:\p"
	.string "{STR_VAR_1},\n"
	.string "{STR_VAR_2}\l"
	.string "and {STR_VAR_3}!$"

MegaDim_Text_PortalAlertClock:
	.string "Two minutes inside. Go, go, go!$"

MegaDim_Text_PortalAlertEmmaClock:
	.string "Emma's with you, so that's\n"
	.string "{STR_VAR_1} inside. Go, go, go!$"

MegaDim_Text_MablePrimalCall:
	.string "{PLAYER}, it's Mable. I've run\n"
	.string "the tests on all 23 stones.\p"
	.string "And I found something else.\n"
	.string "Four power signatures in the\l"
	.string "rifts, off the charts.\p"
	.string "Primal Groudon and Primal Kyogre,\n"
	.string "in their ancient forms.\p"
	.string "And two I've never seen before.\n"
	.string "Lugia and Ho-Oh, Mega Evolved.\p"
	.string "They're in the portals now.\n"
	.string "Catch all four. Carefully.$"

MegaDim_Text_MableRayquazaCall:
	.string "{PLAYER}! Mable. All four,\n"
	.string "caught. Remarkable work.\p"
	.string "But something's chasing their\n"
	.string "energy through the rifts.\p"
	.string "A Mega Rayquaza. It's already\n"
	.string "in a portal.\p"
	.string "Bring it in before it tears\n"
	.string "the rifts wide open.$"

MegaDim_Text_FirstCell:
	.string "A green speck, alive and warm.\n"
	.string "Your Odd Pebble hums as you take it.\p"
	.string "The rift's clock slows.$"

MegaDim_Text_EmmaCellCall:
	.string "Detective Emma. Mable told me about\n"
	.string "your pebble, so I checked the books.\p"
	.string "Hotel Z takes in a lot of Z-Shards.\n"
	.string "It doesn't press many crystals.\p"
	.string "So where's the rest going?\n"
	.string "I'll keep digging. Keep this quiet.$"

MegaDim_Text_MableDrainCall:
	.string "{PLAYER}, the rifts are settling,\n"
	.string "but the gray world isn't recovering.\p"
	.string "Rayquaza only fed on it. Something\n"
	.string "else is draining it, and has been\l"
	.string "for a very long time.\p"
	.string "It's why the world is gray.\p"
	.string "Emma traced the drain. It ends at\n"
	.string "Hotel Z, under our feet.\p"
	.string "Those Cells are the only thing that\n"
	.string "ever fought back. Bring me 20, and\l"
	.string "we'll have enough to prove it.$"

MegaDim_Text_ShardsFound:
	.string "Something glitters where the fight\n"
	.string "was. Found {STR_VAR_1} Z-Shards!$"

MegaDim_Text_Collapsing:
	.string "The Mega Dimension is collapsing!$"

@ --- Rift Sanctums ---------------------------------------------------
@ After the game is beaten a barrier stands in six dimension maps. The partner
@ it belongs to, along at max friendship, opens it with their ability; behind
@ it is a sanctum: three Team Mutrid holdouts (each with a Shadow), then a
@ Shadow legend. src/wot_mega_dimension.c sSanctums holds the table.
MegaDim_EventScript_BarrierSurge::
	setvar VAR_0x8004, 0
	goto MegaDim_EventScript_Barrier

MegaDim_EventScript_BarrierLifeline::
	setvar VAR_0x8004, 1
	goto MegaDim_EventScript_Barrier

MegaDim_EventScript_BarrierTimefreeze::
	setvar VAR_0x8004, 2
	goto MegaDim_EventScript_Barrier

MegaDim_EventScript_BarrierDash::
	setvar VAR_0x8004, 3
	goto MegaDim_EventScript_Barrier

MegaDim_EventScript_BarrierIllusion::
	setvar VAR_0x8004, 4
	goto MegaDim_EventScript_Barrier

MegaDim_EventScript_BarrierBreakthrough::
	setvar VAR_0x8004, 5
MegaDim_EventScript_Barrier::
	lock
	copyvar VAR_0x8009, VAR_0x8004
	special WotSanctum_CheckBarrier
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_BarrierOpens
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
	closemessage
	release
	end

MegaDim_EventScript_BarrierOpens::
	playse SE_M_MEGA_KICK
	msgbox gWotPartnerLine, MSGBOX_DEFAULT
	closemessage
	playse SE_M_TELEPORT
	special StartRiftDistortion
	delay 32
	playse SE_M_EXPLOSION
	removeobject VAR_LAST_TALKED
	delay 16
	playse SE_WARP_IN
	fadescreen FADE_TO_WHITE
	special EndRiftDistortion
	delay 20
	copyvar VAR_0x8004, VAR_0x8009
	special WotSanctum_Enter
	release
	end

@ --- inside a sanctum --------------------------------------------------------------
Sanctum_OnTransition::
	special WotSanctum_Setup
	end

Sanctum_OnResume::
	special WotMegaDim_StartHud
	special WotSanctum_ShadeLegend
	end

Sanctum_EventScript_Exit::
	lock
	msgbox Sanctum_Text_ExitPrompt, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, MegaDim_EventScript_PortalDecline
	closemessage
	call MegaDim_EventScript_RiftOut
	special WotSanctum_Leave
	release
	end

@ The holdouts share three trainer ids across every sanctum; each sanctum
@ gives them its own team (WotMegaDim_AdjustSanctumParty) and keeps who's
@ been beaten in its own save byte.
Sanctum_EventScript_HoldoutA::
	trainerbattle_single TRAINER_SANCTUM_HOLDOUT_A, Sanctum_Text_KuroIntro, Sanctum_Text_KuroDefeat, Sanctum_EventScript_HoldoutABeaten
	end

Sanctum_EventScript_HoldoutABeaten::
	setvar VAR_0x8004, 0
	goto Sanctum_EventScript_HoldoutBeaten

Sanctum_EventScript_HoldoutB::
	trainerbattle_single TRAINER_SANCTUM_HOLDOUT_B, Sanctum_Text_YamiIntro, Sanctum_Text_YamiDefeat, Sanctum_EventScript_HoldoutBBeaten
	end

Sanctum_EventScript_HoldoutBBeaten::
	setvar VAR_0x8004, 1
	goto Sanctum_EventScript_HoldoutBeaten

Sanctum_EventScript_HoldoutC::
	trainerbattle_single TRAINER_SANCTUM_HOLDOUT_C, Sanctum_Text_OboroIntro, Sanctum_Text_OboroDefeat, Sanctum_EventScript_HoldoutCBeaten
	end

Sanctum_EventScript_HoldoutCBeaten::
	setvar VAR_0x8004, 2
Sanctum_EventScript_HoldoutBeaten::
	special WotSanctum_HoldoutBeaten
	copyvar VAR_0x8009, VAR_RESULT
	call Wot_EventScript_SnagProgress
	setvar VAR_0x8004, 5
	call MegaDim_EventScript_GiveShards
	msgbox Sanctum_Text_HoldoutFlees, MSGBOX_DEFAULT
	closemessage
	playse SE_WARP_OUT
	removeobject VAR_LAST_TALKED
	goto_if_eq VAR_0x8009, TRUE, Sanctum_EventScript_LegendAppears
	release
	end

Sanctum_EventScript_LegendAppears::
	delay 20
	playse SE_THUNDER
	setvar VAR_0x8004, 2  @ vertical pan
	setvar VAR_0x8005, 2  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 4  @ shake delay
	special ShakeCamera
	waitstate
	clearflag FLAG_TEMP_5
	addobject LOCALID_SANCTUM_LEGEND
	special WotSanctum_ShadeLegend
	msgbox Sanctum_Text_LegendStirs, MSGBOX_DEFAULT
	closemessage
	release
	end

@ The legend: only a catch counts. Knocked out or fled from, it's gone until
@ the next visit.
Sanctum_EventScript_Legend::
	lock
	faceplayer
	special WotSanctum_SetupLegendBattle
	copyvar VAR_0x8009, VAR_0x8004
	waitse
	playmoncry VAR_0x8009, CRY_MODE_ENCOUNTER
	waitmoncry
	bufferspeciesname STR_VAR_1, VAR_0x8009
	msgbox Sanctum_Text_LegendGlares, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_SYS_CTRL_OBJ_DELETE
	special BattleSetup_StartLegendaryBattle
	clearflag FLAG_SYS_CTRL_OBJ_DELETE
	special WotMegaDim_EndMegaBattle
	specialvar VAR_RESULT, GetBattleOutcome
	goto_if_eq VAR_RESULT, B_OUTCOME_CAUGHT, Sanctum_EventScript_LegendCaught
	removeobject VAR_LAST_TALKED
	setflag FLAG_TEMP_5
	bufferspeciesname STR_VAR_1, VAR_0x8009
	msgbox Sanctum_Text_LegendRetreats, MSGBOX_DEFAULT
	release
	end

Sanctum_EventScript_LegendCaught::
	special WotSanctum_LegendCaught
	removeobject VAR_LAST_TALKED
	setflag FLAG_TEMP_5
	call Wot_EventScript_SnagProgress
	setvar VAR_0x8004, 25
	call MegaDim_EventScript_GiveShards
	bufferspeciesname STR_VAR_1, VAR_0x8009
	msgbox Sanctum_Text_LegendCaught, MSGBOX_DEFAULT
	release
	end

Sanctum_Text_ExitPrompt:
	.string "The rift back to the barrier\n"
	.string "is holding steady.\p"
	.string "Leave the sanctum?$"

Sanctum_Text_KuroIntro:
	.string "KURO: Nobody told us the boss lost.\n"
	.string "Nobody told us anything!\p"
	.string "This rift is ours now. Turn around!$"

Sanctum_Text_KuroDefeat:
	.string "KURO: Fine! Take the rift!$"

Sanctum_Text_YamiIntro:
	.string "YAMI: You opened the seal?\n"
	.string "Then you can close your eyes too.\p"
	.string "The Shadows here answer to us!$"

Sanctum_Text_YamiDefeat:
	.string "YAMI: They answered to you instead…$"

Sanctum_Text_OboroIntro:
	.string "OBORO: Mutrid's last order was\n"
	.string "“hold the line.”\p"
	.string "I'm still holding it!$"

Sanctum_Text_OboroDefeat:
	.string "OBORO: The line… is gone.$"

Sanctum_Text_HoldoutFlees:
	.string "The holdout fled through a crack\n"
	.string "in the rift!$"

Sanctum_Text_LegendStirs:
	.string "With the last holdout gone, something\n"
	.string "stirs at the heart of the sanctum…$"

Sanctum_Text_LegendGlares:
	.string "A Shadow {STR_VAR_1} glares out of\n"
	.string "the dark!$"

Sanctum_Text_LegendRetreats:
	.string "The Shadow {STR_VAR_1} sank back\n"
	.string "into the rift.\p"
	.string "It may surface here again.$"

Sanctum_Text_LegendCaught:
	.string "The sanctum falls quiet.\p"
	.string "Its Shadow {STR_VAR_1} is yours now.\n"
	.string "Purify its heart in time.$"
