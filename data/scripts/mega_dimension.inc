@ ============================================================================
@  WISHES OF TOMORROW -- THE MEGA DIMENSION
@
@  The portals (on source maps, each spot hidden by its FLAG_HIDE_MEGA_PORTAL_*),
@  the shared map scripts of every dimension map, the Mega encounter and the
@  collapse. The logic lives in src/wot_mega_dimension.c.
@ ============================================================================

@ --- the portal in the normal world ------------------------------------------
@ A portal stays open until its dimension collapses; going back in finds the
@ same place (shinies re-rolled) for 5 seconds off its clock.
MegaDim_EventScript_Portal::
	lock
	playse SE_M_SUPERSONIC
	special WotMegaDim_CheckPortal
	goto_if_eq VAR_RESULT, 2, MegaDim_EventScript_PortalFizzles
	goto_if_eq VAR_RESULT, 1, MegaDim_EventScript_PortalReenter
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
	special WotMegaDim_MarkClaimed
	goto MegaDim_EventScript_MegaGoneCollapse

MegaDim_EventScript_MegaFaded::
	msgbox MegaDim_Text_MegaFaded, MSGBOX_DEFAULT
	goto MegaDim_EventScript_MegaGoneCollapse

@ --- Mable's legends (Primal Groudon/Kyogre, Mega Rayquaza) ----------------------
@ Only a catch counts. Knocked out, it retreats deeper into the rift and
@ turns up again in a later portal; the portal collapses either way.
MegaDim_EventScript_LegendCaught::
	special WotMegaDim_MarkClaimed
	goto_if_unset FLAG_WOT_CAUGHT_MEGA_RAYQUAZA, MegaDim_EventScript_MegaGoneCollapse
	goto_if_quest_complete QUEST_SKY_HIGH_MEGA, MegaDim_EventScript_MegaGoneCollapse
	completequest QUEST_SKY_HIGH_MEGA
	goto MegaDim_EventScript_MegaGoneCollapse

MegaDim_EventScript_LegendRetreats::
	msgbox MegaDim_Text_LegendRetreats, MSGBOX_DEFAULT
	goto MegaDim_EventScript_MegaGoneCollapse

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
	goto MegaDim_EventScript_CallEnd

MegaDim_EventScript_PortalAlert1::
	msgbox MegaDim_Text_PortalAlert1, MSGBOX_DEFAULT
	goto MegaDim_EventScript_CallEnd

MegaDim_EventScript_PortalAlert2::
	msgbox MegaDim_Text_PortalAlert2, MSGBOX_DEFAULT
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

@ Mable, once both Primals are caught: Sky-High Mega.
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

MegaDim_EventScript_WildMonGone::
	special WotMegaDim_MarkWildGone
	removeobject VAR_LAST_TALKED
	release
	end

@ --- the rivals ------------------------------------------------------------------
@ About one portal in five (WotMegaDim OpenPortal) a Legends Z-A character beat
@ the player to the Mega: they stand on its slot with it in their party (the
@ 5th slot is rewritten to the portal's Mega, WotMegaDim_AdjustRivalParty).
@ Fightable every trip in, with one of three openers (and its matching defeat
@ line) at random; a win hands over the stone the first time (same rule as a
@ KO) and collapses the portal, a loss whites out back to it (also a collapse).
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
	cleartrainerflag TRAINER_MEGADIM_CANARI
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
	cleartrainerflag TRAINER_MEGADIM_LIDA
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
	cleartrainerflag TRAINER_MEGADIM_EMMA
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
	cleartrainerflag TRAINER_MEGADIM_TAUNIE
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
	cleartrainerflag TRAINER_MEGADIM_TULIP
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
	cleartrainerflag TRAINER_MEGADIM_JACINTHE
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
	.string "{STR_VAR_1}!\p"
	.string "Two minutes inside. Go, go, go!$"

MegaDim_Text_PortalAlert2:
	.string "{PLAYER}, heads up! New rifts\n"
	.string "just opened in:\p"
	.string "{STR_VAR_1}\n"
	.string "and {STR_VAR_2}!\p"
	.string "Two minutes inside. Go, go, go!$"

MegaDim_Text_PortalAlert3:
	.string "{PLAYER}, heads up! New rifts\n"
	.string "just opened in:\p"
	.string "{STR_VAR_1},\n"
	.string "{STR_VAR_2}\l"
	.string "and {STR_VAR_3}!\p"
	.string "Two minutes inside. Go, go, go!$"

MegaDim_Text_MablePrimalCall:
	.string "{PLAYER}, it's Mable. I've run\n"
	.string "the tests on all 19 stones.\p"
	.string "And I found something else.\n"
	.string "Two power signatures in the\l"
	.string "rifts, off the charts.\p"
	.string "Primal Pokémon. Groudon and\n"
	.string "Kyogre, in their ancient forms.\p"
	.string "They're in the portals now.\n"
	.string "Catch them both. Carefully.$"

MegaDim_Text_MableRayquazaCall:
	.string "{PLAYER}! Mable. Both Primals,\n"
	.string "caught. Remarkable work.\p"
	.string "But something's chasing their\n"
	.string "energy through the rifts.\p"
	.string "A Mega Rayquaza. It's already\n"
	.string "in a portal.\p"
	.string "Bring it in before it tears\n"
	.string "the rifts wide open.$"

MegaDim_Text_Collapsing:
	.string "The Mega Dimension is collapsing!$"
