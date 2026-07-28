	.equ LOCALID_ACT3_CAPTAIN, 5
	.equ LOCALID_ARREST_OFFICER, 6
	.equ LOCALID_ARREST_OPERATIVE, 7
	.equ LOCALID_JAIL_GUARD_A, 8
	.equ LOCALID_JAIL_GUARD_B, 9

ShinTokyo_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ShinTokyo_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, ShinTokyo_OnFrame
	.byte 0

@ Scene-only NPCs (the Captain, the arrest officer, the operative) stay
@ hidden on every load; their cutscenes addobject/removeobject them.
ShinTokyo_OnTransition::
	setflag FLAG_HIDE_ACT3_CAPTAIN
	setflag FLAG_HIDE_ACT3_ARREST_NPCS
	end

@ Step 6 fires the moment the player is back on the street at red alert.
ShinTokyo_OnFrame:
	map_script_2 VAR_TEMP_3, 0, ShinTokyo_EventScript_TryArrest
	.2byte 0

@ ============================================================================
@  ACT 3, STEP 1 -- NEON DISTRICT SHELL (WoT_Act3_Canon.md)
@  Placeholder layout: Night Market tileset stands in for the neon city until
@  real art lands. Building shells (gym / black market / battle cafe / blocks)
@  are painted but their doors are signs -- interiors come later. The south
@  water is VISIBLE AND LOCKED (mirror-water tiles, collision 1): Surf is
@  unlocked by the Distortion Badge but the tides are up until Step 5/7.
@  The lone goon teaches the mask system in safety (shared GoonNod script:
@  masked = deferential, unmasked = suspicious). All text PLACEHOLDER (Joe).
@ ============================================================================

ShinTokyo_EventScript_Citizen1::
	lock
	faceplayer
	goto_if_set FLAG_ACT3_RED_ALERT, ShinTokyo_EventScript_CitizenRedAlert
	msgbox ShinTokyo_Text_Citizen1, MSGBOX_DEFAULT
	release
	end

ShinTokyo_EventScript_Citizen2::
	lock
	faceplayer
	goto_if_set FLAG_ACT3_RED_ALERT, ShinTokyo_EventScript_CitizenRedAlert
	msgbox ShinTokyo_Text_Citizen2, MSGBOX_DEFAULT
	release
	end

ShinTokyo_EventScript_Citizen3::
	lock
	faceplayer
	goto_if_set FLAG_ACT3_RED_ALERT, ShinTokyo_EventScript_CitizenRedAlert
	msgbox ShinTokyo_Text_Citizen3, MSGBOX_DEFAULT
	release
	end

ShinTokyo_EventScript_CitizenRedAlert::
	msgbox ShinTokyo_Text_CitizenRedAlert, MSGBOX_DEFAULT
	release
	end

@ At red alert every enemy engages regardless of the mask (Step 5). The full
@ hostile-patrol conversion is a later pass -- this goon at least stops
@ being fooled.
ShinTokyo_EventScript_StreetGoon::
	lock
	faceplayer
	goto_if_set FLAG_ACT3_RED_ALERT, ShinTokyo_EventScript_StreetGoonHostile
	goto OniMask_EventScript_GoonNod

ShinTokyo_EventScript_StreetGoonHostile::
	msgbox ShinTokyo_Text_GoonRedAlert, MSGBOX_DEFAULT
	release
	end

ShinTokyo_EventScript_GymDoor::
	msgbox ShinTokyo_Text_GymDoor, MSGBOX_SIGN
	end

ShinTokyo_EventScript_BlackMarketDoor::
	msgbox ShinTokyo_Text_BlackMarketDoor, MSGBOX_SIGN
	end

ShinTokyo_EventScript_CafeDoor::
	msgbox ShinTokyo_Text_CafeDoor, MSGBOX_SIGN
	end

@ Canon Step 5: at red alert the building screens swap the Leader's face for
@ the player's and a bounty. (A real conditional GRAPHIC needs art; the sign
@ text carries the beat for now.)
ShinTokyo_EventScript_PropagandaScreen::
	lockall
	goto_if_set FLAG_ACT3_RED_ALERT, ShinTokyo_EventScript_BountyScreen
	msgbox ShinTokyo_Text_PropagandaScreen, MSGBOX_SIGN
	releaseall
	end

ShinTokyo_EventScript_BountyScreen::
	msgbox ShinTokyo_Text_BountyScreen, MSGBOX_SIGN
	releaseall
	end

ShinTokyo_EventScript_TideSign::
	msgbox ShinTokyo_Text_TideSign, MSGBOX_SIGN
	end

@ --- Text (all PLACEHOLDER (Joe)) -------------------------------------------
ShinTokyo_Text_Citizen1::
	.string "Isn't the city wonderful?\p"
	.string "It's wonderful.\n"
	.string "We're told so every morning.$"

ShinTokyo_Text_Citizen2::
	.string "My Machamp got strong last month.\p"
	.string "Then it got collected.\n"
	.string "It's fine. It's fine.$"

ShinTokyo_Text_Citizen3::
	.string "People don't disappear here!\p"
	.string "They get PROMOTED.\n"
	.string "Upstairs. Forever.$"

ShinTokyo_Text_GymDoor::
	.string "VOLTEN GYM -- “THE CITY'S SPARK”\p"
	.string "The doors are powered down.\n"
	.string "(Gym interior comes later.)$"

ShinTokyo_Text_BlackMarketDoor::
	.string "The shutters hang half-open.\p"
	.string "Someone inside is watching you\n"
	.string "decide whether to knock.\p"
	.string "(Black market comes later.)$"

ShinTokyo_Text_CafeDoor::
	.string "BATTLE CAFE -- OPEN ALL NIGHT\p"
	.string "(Cafe interior comes later.)$"

ShinTokyo_Text_PropagandaScreen::
	.string "The screen towers overhead,\n"
	.string "washing the street in light.\p"
	.string "A face smiles down, ten stories\n"
	.string "tall, and does not blink.\p"
	.string "“PRODUCTIVITY IS PEACE.\n"
	.string "THE LEADER PROVIDES.”$"

ShinTokyo_Text_TideSign::
	.string "TIDE ADVISORY:\p"
	.string "Water levels raised while the\n"
	.string "tower draws power.\p"
	.string "ALL CROSSINGS CLOSED.$"

@ ============================================================================
@  STEP 2 -- THE RESEARCH INSPECTION (trigger row y=10; movable)
@  The Mutrid Captain descends for a public inspection. Officer class: sees
@  through the carved mask. KEPT path: the Shadow Dossier is taken by
@  authority, not force -- no battle. DESTROY path: nothing to find.
@  All dialogue PLACEHOLDER (Joe).
@ ============================================================================
ShinTokyo_EventScript_Inspection::
	lockall
	setvar VAR_TEMP_0, 1
	goto_if_set FLAG_ACT3_INSPECTION_DONE, ShinTokyo_EventScript_InspectionDone
	applymovement OBJ_EVENT_ID_PLAYER, ShinTokyo_Movement_PlayerFaceUp
	waitmovement 0
	addobject LOCALID_ACT3_CAPTAIN
	applymovement LOCALID_ACT3_CAPTAIN, ShinTokyo_Movement_CaptainApproach
	waitmovement 0
	setspeaker SP_NAME_MUTRID_CAPTAIN
	goto_if_unset FLAG_ONI_MASK_WORN, ShinTokyo_EventScript_InspectionHalt
	@ Officers see through the mask -- and say so.
	msgbox ShinTokyo_Text_CaptainMask, MSGBOX_DEFAULT
ShinTokyo_EventScript_InspectionHalt::
	msgbox ShinTokyo_Text_CaptainHalt, MSGBOX_DEFAULT
	checkitem ITEM_SHADOW_DOSSIER
	goto_if_eq VAR_RESULT, TRUE, ShinTokyo_EventScript_InspectionKept
	@ --- destroy path: a pat-down finds nothing. Small, quiet satisfaction.
	msgbox ShinTokyo_Text_CaptainNothing, MSGBOX_DEFAULT
	closemessage
	goto ShinTokyo_EventScript_InspectionLeave

ShinTokyo_EventScript_InspectionKept::
	msgbox ShinTokyo_Text_CaptainTakes, MSGBOX_DEFAULT
	closemessage
	playse SE_PIN
	removeitem ITEM_SHADOW_DOSSIER, 1
	msgbox ShinTokyo_Text_DossierTaken, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_ACT3_RESEARCH_STOLEN
	setspeaker SP_NAME_MUTRID_CAPTAIN
	msgbox ShinTokyo_Text_CaptainGloat, MSGBOX_DEFAULT
	closemessage
	goto ShinTokyo_EventScript_InspectionLeave

ShinTokyo_EventScript_InspectionLeave::
	applymovement LOCALID_ACT3_CAPTAIN, ShinTokyo_Movement_CaptainLeave
	waitmovement 0
	removeobject LOCALID_ACT3_CAPTAIN
	setflag FLAG_ACT3_INSPECTION_DONE
	goto_if_unset FLAG_ACT3_RESEARCH_STOLEN, ShinTokyo_EventScript_InspectionDone
	@ Gengar State C begins here (canon dialogue states).
	waitse
	playmoncry SPECIES_GENGAR, CRY_MODE_NORMAL
	waitmoncry
	setspeaker SP_NAME_GENGAR
	showportrait PORTRAIT_CLARKSON_GENGAR
	msgbox ShinTokyo_Text_GengarStateC, MSGBOX_DEFAULT
	closemessage
	hideportrait
ShinTokyo_EventScript_InspectionDone::
	releaseall
	end

@ ============================================================================
@  STEP 3 -- MINISTER'S CALL (trigger row y=16; movable). Fires only after
@  the inspection so the beats stay ordered. Opens the jail.
@ ============================================================================
ShinTokyo_EventScript_MinisterCall::
	lockall
	setvar VAR_TEMP_2, 1
	goto_if_unset FLAG_ACT3_INSPECTION_DONE, ShinTokyo_EventScript_MinisterCallSkip
	goto_if_set FLAG_ACT3_MINISTER_CALLED, ShinTokyo_EventScript_MinisterCallSkip
	pokenavcall ShinTokyo_Text_MinisterCall
	waitmessage
	closemessage
	setflag FLAG_ACT3_MINISTER_CALLED
	@ The Minister makes good on it immediately: the two sentries posted at
	@ the detention door get pulled off (they're offscreen from the trigger
	@ row, so despawning is the scatter -- the text sells it).
	setflag FLAG_HIDE_ACT3_JAIL_GUARDS
	removeobject LOCALID_JAIL_GUARD_A
	removeobject LOCALID_JAIL_GUARD_B
	msgbox ShinTokyo_Text_GuardsScatter, MSGBOX_DEFAULT
	closemessage
ShinTokyo_EventScript_MinisterCallSkip::
	releaseall
	end

@ The two sentries walling off the detention door (32,18) until the call.
ShinTokyo_EventScript_JailGuard::
	lock
	faceplayer
	msgbox ShinTokyo_Text_JailGuard, MSGBOX_DEFAULT
	release
	end

@ The detention-block plaque beside the door (31,18). The door itself is a
@ real warp at (32,18) -- physically blocked by the sentries until the
@ Minister's call scatters them.
ShinTokyo_EventScript_JailDoor::
	lockall
	goto_if_set FLAG_ACT3_MINISTER_CALLED, ShinTokyo_EventScript_JailDoorOpen
	msgbox ShinTokyo_Text_JailDoorSealed, MSGBOX_DEFAULT
	releaseall
	end

ShinTokyo_EventScript_JailDoorOpen::
	msgbox ShinTokyo_Text_JailDoorOpens, MSGBOX_DEFAULT
	releaseall
	end

@ ============================================================================
@  STEPS 5+6 -- RED ALERT AND THE ARREST (ON_FRAME, fires on the first street
@  frame after the jailbreak). The "officer" strips the carved mask -- then,
@  out of sight, the ZOROARK illusion drops and the operative walks the
@  player to the rebel hideout. Script continues INTO the hideout after the
@  warp for the arrival beat.
@ ============================================================================
ShinTokyo_EventScript_TryArrest::
	lockall
	setvar VAR_TEMP_3, 1
	goto_if_unset FLAG_ACT3_RED_ALERT, ShinTokyo_EventScript_TryArrestDone
	goto_if_set FLAG_ACT3_ARREST_DONE, ShinTokyo_EventScript_TryArrestDone
	playse SE_EXIT
	msgbox ShinTokyo_Text_RedAlertWail, MSGBOX_DEFAULT
	closemessage
	addobject LOCALID_ARREST_OFFICER
	applymovement LOCALID_ARREST_OFFICER, ShinTokyo_Movement_OfficerApproach
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, ShinTokyo_Movement_PlayerFaceUp
	waitmovement 0
	setspeaker SP_NAME_OPERATIVE
	msgbox ShinTokyo_Text_ArrestAnnounce, MSGBOX_DEFAULT
	checkitem ITEM_CARVED_MASK
	goto_if_eq VAR_RESULT, FALSE, ShinTokyo_EventScript_ArrestNoMask
	@ The Save Face payoff: she strips the mask ON SCREEN. It stays in the
	@ bag but is useless from red alert on (oni_mask_disguise.inc gate).
	closemessage
	clearflag FLAG_ONI_MASK_WORN
	special RefreshPlayerAvatarGraphicsIfOnFoot
	msgbox ShinTokyo_Text_MaskStripped, MSGBOX_DEFAULT
ShinTokyo_EventScript_ArrestNoMask::
	closemessage
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_ARREST_OFFICER
	addobject LOCALID_ARREST_OPERATIVE
	fadescreen FADE_FROM_BLACK
	waitse
	playmoncry SPECIES_ZOROARK, CRY_MODE_NORMAL
	waitmoncry
	msgbox ShinTokyo_Text_IllusionDrops, MSGBOX_DEFAULT
	setspeaker SP_NAME_OPERATIVE
	msgbox ShinTokyo_Text_OperativeReveal, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_ACT3_ARREST_DONE
	fadescreen FADE_TO_BLACK
	removeobject LOCALID_ARREST_OPERATIVE
	warp MAP_REBEL_HIDEOUT, 7, 7
	waitstate
	@ -- arrival beat, now inside the hideout --
	setspeaker SP_NAME_OPERATIVE
	msgbox ShinTokyo_Text_HideoutArrival, MSGBOX_DEFAULT
	closemessage
ShinTokyo_EventScript_TryArrestDone::
	releaseall
	end

@ --- Movement ----------------------------------------------------------------
ShinTokyo_Movement_PlayerFaceUp:
	face_up
	step_end

ShinTokyo_Movement_CaptainApproach:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

ShinTokyo_Movement_CaptainLeave:
	walk_up
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

ShinTokyo_Movement_OfficerApproach:
	walk_down
	walk_down
	walk_down
	step_end

@ --- Text (all PLACEHOLDER (Joe)) -------------------------------------------
ShinTokyo_Text_CitizenRedAlert::
	.string "Don't stand near me!\p"
	.string "Your face is on every screen\n"
	.string "in the city!$"

ShinTokyo_Text_GoonRedAlert::
	.string "That's-- the face from the\n"
	.string "bounty screens!\p"
	.string "All units! ALL UNITS!$"

ShinTokyo_Text_BountyScreen::
	.string "Your own face stares down from\n"
	.string "ten stories of glass.\p"
	.string "“WANTED: THE PRISON BREAKER.\n"
	.string "REWARD BEYOND MEASURE.”$"

ShinTokyo_Text_CaptainMask::
	.string "“Take that carnival prop off.\p"
	.string "“Masks are for the rank and\n"
	.string "file. Officers see FACES.”$"

ShinTokyo_Text_CaptainHalt::
	.string "The street empties in seconds.\p"
	.string "“Routine inspection, citizen.\n"
	.string "Arms out. Bag open.”$"

ShinTokyo_Text_CaptainNothing::
	.string "Gloved hands turn out every\n"
	.string "pocket. Twice.\p"
	.string "“...Nothing. Tch.\p"
	.string "“Move along, citizen. The city\n"
	.string "thanks you for your compliance.”$"

ShinTokyo_Text_CaptainTakes::
	.string "Gloved fingers stop on the worn\n"
	.string "cover of the SHADOW DOSSIER.\p"
	.string "“...Well. The dead professor's\n"
	.string "handwriting.\p"
	.string "“This is city property now.\n"
	.string "By my authority.”$"

ShinTokyo_Text_DossierTaken::
	.string "The SHADOW DOSSIER was taken!$"

ShinTokyo_Text_CaptainGloat::
	.string "“You carried it all this way\n"
	.string "just to hand-deliver it.\p"
	.string "“The tower will remember the\n"
	.string "favor. I won't.”$"

ShinTokyo_Text_GengarStateC::
	.string "“...It's gone.\p"
	.string "“After everything.\n"
	.string "It's in his hands now.”$"

ShinTokyo_Text_MinisterCall::
	.string "MINISTER: {PLAYER}. Don't talk,\n"
	.string "walk. Casual.\p"
	.string "MIKMANC's been made. They have\n"
	.string "him in the east block -- and he\l"
	.string "is HOLDING something they must\l"
	.string "not find.\p"
	.string "I'm in their cameras. The front\n"
	.string "door will forget you were there.\p"
	.string "Go get him out.$"

ShinTokyo_Text_JailDoorSealed::
	.string "EAST BLOCK DETENTION.\p"
	.string "Sealed. A camera swivels toward\n"
	.string "you with a soft whir.$"

ShinTokyo_Text_JailDoorOpens::
	.string "EAST BLOCK DETENTION.\p"
	.string "The camera light is dark.\n"
	.string "The bolt hangs open.$"

ShinTokyo_Text_JailGuard::
	.string "“Detention block. Official\n"
	.string "business only.\p"
	.string "“Keep walking, citizen. The\n"
	.string "city is watching you walk.”$"

ShinTokyo_Text_GuardsScatter::
	.string "Down the street, two radios\n"
	.string "crackle at once.\p"
	.string "The sentries at the detention\n"
	.string "door glance at each other--\p"
	.string "--and peel away into the rain.$"

ShinTokyo_Text_RedAlertWail::
	.string "Sirens.\p"
	.string "Every screen on the street\n"
	.string "changes at once -- to YOUR face.$"

ShinTokyo_Text_ArrestAnnounce::
	.string "“By authority of the tower:\n"
	.string "you are under arrest.\p"
	.string "“Stand down. All of you.\n"
	.string "I will escort the prisoner\l"
	.string "PERSONALLY.”\p"
	.string "The goons... defer.$"

ShinTokyo_Text_MaskStripped::
	.string "She lifts the carved face off\n"
	.string "yours in one motion.\p"
	.string "“Officers always saw through\n"
	.string "it.\p"
	.string "“Now everyone does.”$"

ShinTokyo_Text_IllusionDrops::
	.string "Two alleys later, out of every\n"
	.string "camera's reach--\p"
	.string "The officer's outline RIPPLES.\p"
	.string "A ZOROARK's illusion pours off\n"
	.string "her like rainwater.$"

ShinTokyo_Text_OperativeReveal::
	.string "A hooded young woman. Hair like\n"
	.string "a mane of shadow.\p"
	.string "“I was hunting whoever cracked\n"
	.string "the prison open.\p"
	.string "“Their manhunt found me YOU\n"
	.string "instead. Kekeke-- no. Sorry.\l"
	.string "Wrong ghost.\p"
	.string "“Come on, prison breaker.\n"
	.string "Some people need to meet you.”$"

ShinTokyo_Text_HideoutArrival::
	.string "“Down through the bones of the\n"
	.string "old city... and here.\p"
	.string "“Welcome to the rebellion.”$"
