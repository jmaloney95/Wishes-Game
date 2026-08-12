NewMapApprenticeHouse_MapScripts::
	.byte 0

@ ============================================================================
@  ITSUKI'S HOUSE -- boarded up until FLAG_NEWMAP_APPRENTICE_DOOR_OPEN is set
@  (Master Gen clears the crates after the mask is returned).
@  Optional epilogue lore: WHY a village boy joins the oni climb.
@ ============================================================================
NewMapApprenticeHouse_EventScript_ItemRareCandy::
	finditem ITEM_RARE_CANDY
	end

NewMapApprenticeHouse_EventScript_Diary::
	msgbox NewMapApprenticeHouse_Text_Diary, MSGBOX_SIGN
	end

NewMapApprenticeHouse_EventScript_Worktable::
	msgbox NewMapApprenticeHouse_Text_Worktable, MSGBOX_SIGN
	end

NewMapApprenticeHouse_EventScript_Hearth::
	msgbox NewMapApprenticeHouse_Text_Hearth, MSGBOX_SIGN
	end

@ ============================================================================
@  Text
@ ============================================================================
NewMapApprenticeHouse_Text_Diary::
	.string "A diary, left open on the desk.\p"
	.string "“...heard it again on the north\n"
	.string "wind. Father's voice. It said my\l"
	.string "name the old way, like he did.\p"
	.string "“Master Gen says grief plays the\n"
	.string "ear like a flute. Maybe.\p"
	.string "“...A man in a horned mask was\n"
	.string "waiting by the old tomb tonight.\l"
	.string "He didn't laugh at me.\p"
	.string "“He said the mountain keeps what\n"
	.string "we regret. Every voice, every\l"
	.string "face. Kept, not gone.\p"
	.string "“He said: the masked may climb,\n"
	.string "and the Summit Gate gives back.\p"
	.string "“The bell rings twice tomorrow.”\p"
	.string "The remaining pages are blank.$"

NewMapApprenticeHouse_Text_Worktable::
	.string "Wood shavings blanket a small\n"
	.string "worktable.\p"
	.string "Practice masks hang in a row above\n"
	.string "it -- each one better than the\l"
	.string "last, forty tries or more.\p"
	.string "The final hook is empty.$"

NewMapApprenticeHouse_Text_Hearth::
	.string "The hearth is cold.\p"
	.string "On the mantle sits a framed\n"
	.string "photograph: a fisherman with a\l"
	.string "wide grin, one hand resting on a\l"
	.string "small boy's head.\p"
	.string "The glass has been wiped clean of\n"
	.string "dust. Recently. Often.$"
