NewMapInn_MapScripts::
	.byte 0

@ ============================================================================
@  PILGRIM'S REST -- free bed before the climb. Heals + sets respawn.
@  NOTE: HealPlayerParty is synchronous -- do NOT follow with waitstate.
@ ============================================================================
NewMapInn_EventScript_Innkeeper::
	lock
	faceplayer
	msgbox NewMapInn_Text_Greeting, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq NewMapInn_EventScript_Decline
	closemessage
	fadescreen FADE_TO_BLACK
	special HealPlayerParty
	fadescreen FADE_FROM_BLACK
	playfanfare MUS_HEAL
	waitfanfare
	setrespawn HEAL_LOCATION_NEW_MAP_INN
	msgbox NewMapInn_Text_Rested, MSGBOX_DEFAULT
	release
	end

NewMapInn_EventScript_Decline::
	msgbox NewMapInn_Text_Decline, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  Text
@ ============================================================================
NewMapInn_Text_Greeting::
	.string "Welcome to the Pilgrim's Rest.\p"
	.string "Once, every bed was full the night\n"
	.string "before a climb. Pilgrims, all of\l"
	.string "them, waiting on the dawn bell.\p"
	.string "Now it's just empty beds and me.\p"
	.string "You look half frozen, dear.\n"
	.string "Will you rest a while?$"

NewMapInn_Text_Rested::
	.string "There. Warm again, the lot of you.\p"
	.string "If the mountain turns you back,\n"
	.string "you come straight here, hear me?\p"
	.string "This door doesn't lock. Not for\n"
	.string "anyone coming DOWN.$"

NewMapInn_Text_Decline::
	.string "Suit yourself, dear.\p"
	.string "The kettle stays warm and the beds\n"
	.string "stay made. That much I can do.$"
