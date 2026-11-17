PrototypeIsland_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, PrototypeIsland_OnLoad
	map_script MAP_SCRIPT_ON_TRANSITION, PrototypeIsland_OnTransition
	.byte 0

@ ============================================================================
@  PROTOTYPE ISLAND: the half-sunk
@  remains of Mutrid's FIRST laboratory -- before Munen, before the tower.
@  A lone hermit tells the lore and warns against going below; the flooded
@  lab (MAP_PROTOTYPE_LAB) waits underneath with the failed "perfect form"
@  prototype. Surf-hop wiring: Celebi Island west edge <-> here (east edge).
@
@ ============================================================================

PrototypeIsland_EventScript_Hermit::
	lock
	faceplayer
	msgbox PrototypeIsland_Text_Hermit1, MSGBOX_DEFAULT
	msgbox PrototypeIsland_Text_Hermit2, MSGBOX_DEFAULT
	msgbox PrototypeIsland_Text_Hermit3, MSGBOX_DEFAULT
	release
	end

@ ----------------------------------------------------------------------------
@  The lab gate and the far pier's X switch.
@  The stairwell down sits behind a shutter at (5-7,13-14), the New Mauville
@  door tiles. It stays shut until the X switch on the far pier (31,5) is
@  stepped on, which sets FLAG_WOT_PROTOTYPE_LAB_UNLOCKED. The switch is
@  one-way. ON_LOAD redraws both swaps on every load (warp, Continue, after a
@  battle) and re-shuts the gate, solid, while the flag is clear.
@  A Mutrid guard holds the pier. Once he's beaten he leaves for good.
@ ----------------------------------------------------------------------------
PrototypeIsland_OnLoad:
	call_if_set FLAG_WOT_PROTOTYPE_LAB_UNLOCKED, PrototypeIsland_EventScript_SetLabOpen
	call_if_unset FLAG_WOT_PROTOTYPE_LAB_UNLOCKED, PrototypeIsland_EventScript_SetLabShut
	end

PrototypeIsland_OnTransition:
	call_if_defeated TRAINER_PROTOTYPE_ISLAND_GUARD, PrototypeIsland_EventScript_HideGuard
	end

PrototypeIsland_EventScript_HideGuard::
	setflag FLAG_TEMP_1
	return

PrototypeIsland_EventScript_SetLabShut::
	setmetatile 5, 13, METATILE_Facility_NewMauvilleDoor_Closed_Tile0, TRUE
	setmetatile 6, 13, METATILE_Facility_NewMauvilleDoor_Closed_Tile1, TRUE
	setmetatile 7, 13, METATILE_Facility_NewMauvilleDoor_Closed_Tile2, TRUE
	setmetatile 5, 14, METATILE_Facility_NewMauvilleDoor_Closed_Tile3, TRUE
	setmetatile 6, 14, METATILE_Facility_NewMauvilleDoor_Closed_Tile4, TRUE
	setmetatile 7, 14, METATILE_Facility_NewMauvilleDoor_Closed_Tile5, TRUE
	return

PrototypeIsland_EventScript_SetLabOpen::
	setmetatile 31, 5, 0x2E2, FALSE  @ the switch's red X (unpressed: 0x2DA)
	setmetatile 5, 13, METATILE_Facility_NewMauvilleDoor_Open_Tile0, TRUE
	setmetatile 6, 13, METATILE_Facility_NewMauvilleDoor_Open_Tile1, TRUE
	setmetatile 7, 13, METATILE_Facility_NewMauvilleDoor_Open_Tile2, TRUE
	setmetatile 5, 14, METATILE_Facility_NewMauvilleDoor_Open_Tile3, TRUE
	setmetatile 6, 14, METATILE_Facility_NewMauvilleDoor_Open_Tile4, FALSE
	setmetatile 7, 14, METATILE_Facility_NewMauvilleDoor_Open_Tile5, TRUE
	setvar VAR_TEMP_8, 1  @ a pressed switch never fires again
	return

@ Facing the shut gate. (Open, the doorway's trigger below takes over.)
PrototypeIsland_EventScript_LabGate::
	goto_if_set FLAG_WOT_PROTOTYPE_LAB_UNLOCKED, PrototypeIsland_EventScript_LabGateOpen
	lockall
	applymovement LOCALID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	msgbox PrototypeIsland_Text_GateLocked, MSGBOX_DEFAULT
	releaseall
	end

PrototypeIsland_EventScript_LabGateOpen::
	end

@ The dark doorway down into the flooded lab.
PrototypeIsland_EventScript_EnterLab::
	lockall
	@ the shut gate is solid, so this is only a safety net
	goto_if_unset FLAG_WOT_PROTOTYPE_LAB_UNLOCKED, PrototypeIsland_EventScript_EnterLabLocked
	msgbox PrototypeIsland_Text_Doorway, MSGBOX_DEFAULT
	closemessage
	playse SE_DOOR
	warp MAP_PROTOTYPE_LAB, 16, 3
	waitstate
	releaseall
	end

PrototypeIsland_EventScript_EnterLabLocked::
	msgbox PrototypeIsland_Text_GateLocked, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_PLAYER, Common_Movement_WalkDown
	waitmovement 0
	releaseall
	end

@ The X switch.
PrototypeIsland_EventScript_LabSwitch::
	goto_if_set FLAG_WOT_PROTOTYPE_LAB_UNLOCKED, PrototypeIsland_EventScript_LabSwitchSpent
	lockall
	@ got round him (along the lower railing, or up from the water)
	goto_if_not_defeated TRAINER_PROTOTYPE_ISLAND_GUARD, PrototypeIsland_EventScript_GuardCatchesYou
PrototypeIsland_EventScript_PressLabSwitch::
	playse SE_SWITCH
	call PrototypeIsland_EventScript_SetLabOpen
	special DrawWholeMapView
	setflag FLAG_WOT_PROTOTYPE_LAB_UNLOCKED
	waitse
	msgbox PrototypeIsland_Text_SwitchClicks, MSGBOX_DEFAULT
	closemessage
	playse SE_M_STRENGTH
	setvar VAR_0x8004, 1  @ vertical pan
	setvar VAR_0x8005, 1  @ horizontal pan
	setvar VAR_0x8006, 8  @ num shakes
	setvar VAR_0x8007, 5  @ shake delay
	special ShakeCamera
	waitstate
	msgbox PrototypeIsland_Text_GateGrindsOpen, MSGBOX_DEFAULT
	releaseall
	end

PrototypeIsland_EventScript_LabSwitchSpent::
	setvar VAR_TEMP_8, 1
	end

@ The pier guard: a Mutrid goon with two Shadow Pokemon.
PrototypeIsland_EventScript_Guard::
	trainerbattle_single TRAINER_PROTOTYPE_ISLAND_GUARD, PrototypeIsland_Text_GuardIntro, PrototypeIsland_Text_GuardDefeat, PrototypeIsland_EventScript_GuardDefeated
	@ (he leaves right after the battle, so this is only reached if he didn't)
	msgbox PrototypeIsland_Text_GuardLeaves, MSGBOX_AUTOCLOSE
	end

PrototypeIsland_EventScript_GuardDefeated::
	call PrototypeIsland_EventScript_GuardLeaves
	release
	end

PrototypeIsland_EventScript_GuardCatchesYou::
	playse SE_PIN
	applymovement LOCALID_PROTOTYPE_ISLAND_GUARD, Common_Movement_ExclamationMark
	waitmovement 0
	applymovement LOCALID_PROTOTYPE_ISLAND_GUARD, Common_Movement_FacePlayer
	waitmovement 0
	msgbox PrototypeIsland_Text_GuardCatchesYou, MSGBOX_DEFAULT
	closemessage
	trainerbattle_no_intro TRAINER_PROTOTYPE_ISLAND_GUARD, PrototypeIsland_Text_GuardDefeat
	call PrototypeIsland_EventScript_GuardLeaves
	goto PrototypeIsland_EventScript_PressLabSwitch

@ He clears off the one-tile pier, so he can't stand between you and the
@ switch. His hide flag is FLAG_TEMP_1; ON_TRANSITION sets it on later visits.
PrototypeIsland_EventScript_GuardLeaves::
	msgbox PrototypeIsland_Text_GuardLeaves, MSGBOX_DEFAULT
	closemessage
	fadescreenswapbuffers FADE_TO_BLACK
	removeobject LOCALID_PROTOTYPE_ISLAND_GUARD
	fadescreenswapbuffers FADE_FROM_BLACK
	call Wot_EventScript_SnagProgress
	return

@ The archipelago chain, north to south:
@   Ryuden Island <-> Route 130 <-> PROTOTYPE ISLAND <-> Celebi Island
@ (and Celebi Island <-> Route 129 <-> Shin-Tokyo below that).
@ NOTE: the old south trigger row sat at y=28 on a 28-tall map -- out of
@ bounds, so surfing south did nothing. It lives on y=27 now.
PrototypeIsland_EventScript_SurfToCelebi::
	lockall
	warp MAP_CELEBI_ISLAND, 25, 1
	waitstate
	releaseall
	end

PrototypeIsland_EventScript_SurfToRoute130::
	lockall
	@ the open channel; (42,31) fell outside the water after Route 130 was
	@ repainted, which put the player off the map
	warp MAP_ROUTE130, 54, 12
	waitstate
	releaseall
	end

@ --- Text -------------------------------------------
PrototypeIsland_Text_Hermit1::
	.string "Hoh? A visitor. The tide must\n"
	.string "be feeling generous.\p"
	.string "You're standing on the roof of\n"
	.string "the FIRST lab, kid.\p"
	.string "Before Munen. Before the tower.\n"
	.string "Mutrid started HERE.$"

PrototypeIsland_Text_Hermit2::
	.string "They wanted a “perfect form”.\p"
	.string "A POKéMON built from scratch…\n"
	.string "no soul to seal, nothing to\l"
	.string "resist them.\p"
	.string "What woke up down there was\n"
	.string "not perfect.\p"
	.string "It was WRONG.$"

PrototypeIsland_Text_Hermit3::
	.string "They flooded the shafts and ran\n"
	.string "in a single night.\p"
	.string "Don't go below, kid. I mean it.\p"
	.string "The water didn't kill it.$"

PrototypeIsland_Text_Doorway::
	.string "A stairwell drops into black\n"
	.string "water.\p"
	.string "Somewhere far below, something\n"
	.string "hums against the flood.$"

PrototypeIsland_Text_GateLocked::
	.string "A steel shutter seals the\n"
	.string "stairwell. It's locked.\p"
	.string "There's no keyhole… only a\n"
	.string "cable running out to sea.$"

PrototypeIsland_Text_SwitchClicks::
	.string "Click!\p"
	.string "The X sinks into the floor\n"
	.string "and glows red.$"

PrototypeIsland_Text_GateGrindsOpen::
	.string "Somewhere across the water,\n"
	.string "something heavy grinds open.$"

PrototypeIsland_Text_GuardIntro::
	.string "Hey! This pier's off-limits.\p"
	.string "That switch opens the old lab,\n"
	.string "and the old lab stays SHUT.\p"
	.string "Orders from the top!$"

PrototypeIsland_Text_GuardDefeat::
	.string "They don't pay me enough\n"
	.string "for this…$"

PrototypeIsland_Text_GuardCatchesYou::
	.string "Hey! Hands off that switch!$"

PrototypeIsland_Text_GuardLeaves::
	.string "Fine. Press your switch.\p"
	.string "Whatever woke up down there…\n"
	.string "it can have you.$"
