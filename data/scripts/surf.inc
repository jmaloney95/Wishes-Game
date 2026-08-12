EventScript_UseSurf::
	checkfieldmove FIELD_MOVE_SURF
	goto_if_eq VAR_RESULT, PARTY_SIZE, EventScript_CantUseSurfYet
	setfieldeffectargument 0, VAR_RESULT
	lockall
	msgbox gText_WantToUseSurf, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EventScript_ReleaseUseSurf
	closemessage
	dofieldeffect FLDEFF_USE_SURF
EventScript_ReleaseUseSurf::
	releaseall
EventScript_EndUseSurf::
	end

@ WoT: surfing is a badge-unlocked trainer ability, so "nothing happens" is a
@ confusing silent failure -- say why instead.
EventScript_CantUseSurfYet::
	lockall
	msgbox EventScript_Text_CantSurfYet, MSGBOX_DEFAULT
	releaseall
	end

EventScript_Text_CantSurfYet::
	.string "The water is deep and fast.\p"
	.string "No badge you carry gives you\n"
	.string "the right to cross it yet.$"

EventScript_CurrentTooFast::
	lockall
	checkfieldmove FIELD_MOVE_SURF
	goto_if_eq VAR_RESULT, FALSE, EventScript_CurrentTooFastEndMsg
	playse SE_SELECT
	msgbox Text_CurrentTooFast
EventScript_CurrentTooFastEndMsg::
	releaseall
	end
