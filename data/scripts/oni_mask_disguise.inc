@ ============================================================================
@  ONI MASK DISGUISE (Act 2)
@  The Carved Mask (ITEM_CARVED_MASK, from the Munen Tunnel quest) is a USABLE
@  key item. Selecting "Use" toggles FLAG_ONI_MASK_WORN. While worn, the masked
@  oni goons mistake the player for one of their own -- in fact for a superior,
@  and defer to them ("Right this way, sir").
@
@  HOW TO USE ON A GOON:
@   - AMBIENT / TALK goon: point the object's script at OniMask_EventScript_GoonNod
@     (or GoonFooled), or add this at the top of the goon's own talk script:
@         goto_if_set FLAG_ONI_MASK_WORN, OniMask_EventScript_GoonMasked
@   - SIGHT/BATTLE goon (trainerbattle_single as the FIRST command): do NOT put
@     any command before trainerbattle -- the sight system reads the script at
@     a fixed offset and a preceding command corrupts the post-battle pointer.
@     Gate those with a coord/OnResume check or make them talk-only instead.
@ ============================================================================

@ Use the Carved Mask key item -> toggle the disguise on/off.
EventScript_UseOniMask::
	lockall
	goto_if_set FLAG_ONI_MASK_WORN, EventScript_TakeOffOniMask
	setflag FLAG_ONI_MASK_WORN
	msgbox OniMask_Text_PutOn, MSGBOX_DEFAULT
	releaseall
	end

EventScript_TakeOffOniMask::
	clearflag FLAG_ONI_MASK_WORN
	msgbox OniMask_Text_TakeOff, MSGBOX_DEFAULT
	releaseall
	end

@ Shared: a rank-and-file goon takes the masked player for a ranking officer.
OniMask_EventScript_GoonMasked::
	msgbox OniMask_Text_GoonMasked, MSGBOX_DEFAULT
	release
	end

@ Talk goon that would normally challenge you: masked -> deferential, no fight.
OniMask_EventScript_GoonFooled::
	lock
	faceplayer
	goto_if_set FLAG_ONI_MASK_WORN, OniMask_EventScript_GoonMasked
	msgbox OniMask_Text_GoonSuspicious, MSGBOX_DEFAULT
	release
	end

@ Ambient goon (no battle), mask-aware flavor.
OniMask_EventScript_GoonNod::
	lock
	faceplayer
	goto_if_set FLAG_ONI_MASK_WORN, OniMask_EventScript_GoonMasked
	msgbox OniMask_Text_GoonSuspicious, MSGBOX_DEFAULT
	release
	end

@ Kept for compatibility with existing object-event references.
OniMask_EventScript_GoonNodMasked::
	msgbox OniMask_Text_GoonMasked, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  Text
@ ============================================================================
OniMask_Text_PutOn::
	.string "{PLAYER} slipped on the Carved Mask.\p"
	.string "Its snarling oni face swallows your\n"
	.string "own features whole.\p"
	.string "To the masked ones, you just might\n"
	.string "pass for one of their own now.$"

OniMask_Text_TakeOff::
	.string "{PLAYER} took off the Carved Mask.\p"
	.string "The cold air bites at your bare\n"
	.string "face again.$"

OniMask_Text_GoonMasked::
	.string "The masked figure snaps upright the\n"
	.string "instant it sees your carved face.\p"
	.string "“S-sir! Forgive me, I didn't see\n"
	.string "you coming!\p"
	.string "“Everything's locked down tight, just\n"
	.string "as you ordered. Not a soul in or out.\p"
	.string "“Right this way, sir -- the others\n"
	.string "are expecting you up ahead.”$"

OniMask_Text_GoonSuspicious::
	.string "...\p"
	.string "Eyes glitter behind the mask's\n"
	.string "carved sockets, tracking you.\p"
	.string "“Bare faces don't belong here.\n"
	.string "Turn around.”$"
