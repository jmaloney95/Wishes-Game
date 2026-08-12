HotSpring_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, HotSpring_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, HotSpring_OnFrame
	.byte 0

@ Any tile flagged MB_CRACKED_FLOOR_HOLE drops the player to the pool center.
HotSpring_OnTransition::
	setholewarp MAP_HOT_SPRING, 15, 13
	end

HotSpring_OnFrame::
	map_script_2 VAR_HOT_SPRING_STATE, 0, HotSpring_EventScript_Act2Opening
	.2byte 0

@ Act 2 opens here: the player comes to in the hot spring with Clarkson's
@ final request still ringing in their ears.
HotSpring_EventScript_Act2Opening::
	lockall
	setvar VAR_HOT_SPRING_STATE, 1
	startquest QUEST_LIFES_WORK
	releaseall
	end

@==============================================================================
@ THE HOT SPRING -- where the player comes to after the summit. The bathers are
@ the dead, the dying, and the dreaming. Eerie, vacant, dreamlike. (Placeholder
@ sprites -- swap for custom zombie/skeleton OW sprites later.)
@==============================================================================

@ A body sprawled at the water's edge -- still talking, somehow.
HotSpring_EventScript_Body::
	lock
	faceplayer
	msgbox HotSpring_Text_Body, MSGBOX_DEFAULT
	release
	end

@ A gaunt woman who watches the steam.
HotSpring_EventScript_Maniac::
	lock
	faceplayer
	msgbox HotSpring_Text_Maniac, MSGBOX_DEFAULT
	release
	end

@ An old bather with vacant eyes.
HotSpring_EventScript_Bather::
	lock
	faceplayer
	msgbox HotSpring_Text_Bather, MSGBOX_DEFAULT
	release
	end

HotSpring_Text_Body:
	.string "…so warm…\p"
	.string "I'll rest here a little longer.\n"
	.string "Just a little longer…\p"
	.string "You should stay in. The water takes\n"
	.string "the cold away. Takes everything\l"
	.string "away.$"

HotSpring_Text_Maniac:
	.string "Shhh. Don't wake the others.\p"
	.string "They're only sleeping. We're all\n"
	.string "only sleeping now.\p"
	.string "The steam carries the voices of\n"
	.string "everyone the spring has swallowed.\p"
	.string "…Can't you hear them?$"

HotSpring_Text_Bather:
	.string "…Have we met?\p"
	.string "In a dream, maybe. Or before the\n"
	.string "ash came down.\p"
	.string "It's so hard to remember which\n"
	.string "world is the real one anymore.$"
