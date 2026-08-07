PrototypeIsland_MapScripts::
	.byte 0

@ ============================================================================
@  PROTOTYPE ISLAND (WoT_Act3_Canon.md archipelago table): the half-sunk
@  remains of Mutrid's FIRST laboratory -- before Munen, before the tower.
@  A lone hermit tells the lore and warns against going below; the flooded
@  lab (MAP_PROTOTYPE_LAB) waits underneath with the failed "perfect form"
@  prototype. Surf-hop wiring: Celebi Island west edge <-> here (east edge).
@  All dialogue PLACEHOLDER (Joe).
@ ============================================================================

PrototypeIsland_EventScript_Hermit::
	lock
	faceplayer
	msgbox PrototypeIsland_Text_Hermit1, MSGBOX_DEFAULT
	msgbox PrototypeIsland_Text_Hermit2, MSGBOX_DEFAULT
	msgbox PrototypeIsland_Text_Hermit3, MSGBOX_DEFAULT
	release
	end

@ The dark doorway down into the flooded lab.
PrototypeIsland_EventScript_EnterLab::
	lockall
	msgbox PrototypeIsland_Text_Doorway, MSGBOX_DEFAULT
	closemessage
	playse SE_DOOR
	warp MAP_PROTOTYPE_LAB, 9, 2
	waitstate
	releaseall
	end

PrototypeIsland_EventScript_SurfToCelebi::
	lockall
	warp MAP_CELEBI_ISLAND, 1, 36
	waitstate
	releaseall
	end

@ --- Text (all PLACEHOLDER (Joe)) -------------------------------------------
PrototypeIsland_Text_Hermit1::
	.string "Hoh? A visitor. The tide must\n"
	.string "be feeling generous.\p"
	.string "You're standing on the roof of\n"
	.string "the FIRST lab, kid.\p"
	.string "Before Munen. Before the tower.\n"
	.string "Mutrid started HERE.$"

PrototypeIsland_Text_Hermit2::
	.string "They wanted a “perfect form”.\p"
	.string "A POKéMON built from scratch --\n"
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
