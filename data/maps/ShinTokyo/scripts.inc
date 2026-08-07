	.equ LOCALID_ACT3_CAPTAIN, 5
	.equ LOCALID_ARREST_OFFICER, 6
	.equ LOCALID_ARREST_OPERATIVE, 7
	.equ LOCALID_JAIL_GUARD_A, 8
	.equ LOCALID_JAIL_GUARD_B, 9
	.equ LOCALID_ARREST_ZOROARK, 10
	.equ LOCALID_ARREST_GOON_A, 11
	.equ LOCALID_ARREST_GOON_B, 12
	.equ LOCALID_ARREST_GOON_C, 13

ShinTokyo_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ShinTokyo_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, ShinTokyo_OnLoad
	.byte 0

@ Tile edits must run in ON_LOAD -- ON_TRANSITION fires before the map layout
@ is loaded, so a setmetatile there is thrown away (the fence never opened).
ShinTokyo_OnLoad::
	call_if_set FLAG_ACT3_RED_ALERT, ShinTokyo_OnLoad_RedAlert
	end

@ the rebels kick a panel out of the lattice fence: the broken-panel
@ metatile with DOOR behavior lands on the hideout warp at (16,12).
ShinTokyo_OnLoad_RedAlert::
	setmetatile 16, 12, 0x1F0, FALSE
	return

@ Scene-only NPCs (the Captain, the arrest cast) stay hidden on every load;
@ their cutscenes addobject/removeobject them. Red alert reveals the Shadow
@ patrols.
@ NOTE: there is deliberately NO layout swap here any more. A second copied
@ layout went stale the moment the map was repainted in Porymap (the city
@ reverted to the pre-edit version after the jailbreak). ONE layout --
@ LAYOUT_SHIN_TOKYO, the one Porymap edits -- is the source of truth. The
@ tide beat is gated in script instead (see ShinTokyo_EventScript_SurfToIsland).
ShinTokyo_OnTransition::
	setflag FLAG_HIDE_ACT3_CAPTAIN
	setflag FLAG_HIDE_ACT3_ARREST_NPCS
	goto_if_set FLAG_ACT3_RED_ALERT, ShinTokyo_OnTransition_RedAlert
	setflag FLAG_HIDE_ACT3_PATROLS
	@ jumbotron: the Leader smiles down until the city turns on you
	clearflag FLAG_HIDE_WOT_LEADER_SCREEN
	setflag FLAG_HIDE_WOT_WANTED_POSTER
	end
ShinTokyo_OnTransition_RedAlert::
	clearflag FLAG_HIDE_ACT3_PATROLS
	@ The patrols re-form every time the city is re-entered: defeat flags
	@ clear here, and their teams re-roll from the pool at battle start
	@ (WotTryRandomizeShadowPatrol in battle_main.c).
	cleartrainerflag TRAINER_NATE
	cleartrainerflag TRAINER_MACEY
	cleartrainerflag TRAINER_CLIFFORD
	@ jumbotron swaps to the bounty
	setflag FLAG_HIDE_WOT_LEADER_SCREEN
	clearflag FLAG_HIDE_WOT_WANTED_POSTER
	end

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

@ --- Red-alert Shadow patrols (the city as snag grounds, v1) ----------------
@ Sight trainers fielding snaggable Shadow mons; they appear the moment the
@ red alert is up (FLAG_HIDE_ACT3_PATROLS cleared in ON_TRANSITION).
ShinTokyo_EventScript_Patrol1::
	trainerbattle_single TRAINER_NATE, ShinTokyo_Text_Patrol1Intro, ShinTokyo_Text_Patrol1Defeat
	msgbox ShinTokyo_Text_PatrolPost, MSGBOX_AUTOCLOSE
	call Wot_EventScript_SnagProgress
	end

ShinTokyo_EventScript_Patrol2::
	trainerbattle_single TRAINER_MACEY, ShinTokyo_Text_Patrol2Intro, ShinTokyo_Text_Patrol2Defeat
	msgbox ShinTokyo_Text_PatrolPost, MSGBOX_AUTOCLOSE
	call Wot_EventScript_SnagProgress
	end

ShinTokyo_EventScript_Patrol3::
	trainerbattle_single TRAINER_CLIFFORD, ShinTokyo_Text_Patrol3Intro, ShinTokyo_Text_Patrol3Defeat
	msgbox ShinTokyo_Text_PatrolPost, MSGBOX_AUTOCLOSE
	call Wot_EventScript_SnagProgress
	end

@ Surf route: crossing the southern edge carries the player to Celebi Island
@ (trigger row -- plain warps don't fire on ordinary water tiles). The TIDE
@ is the gate: until the red alert (Step 5) the crossing is closed, so the
@ player can paddle the harbour but not leave.
ShinTokyo_EventScript_SurfToIsland::
	lockall
	goto_if_unset FLAG_ACT3_RED_ALERT, ShinTokyo_EventScript_TideTooHigh
	warp MAP_CELEBI_ISLAND, 10, 43
	waitstate
	releaseall
	end

ShinTokyo_EventScript_TideTooHigh::
	msgbox ShinTokyo_Text_TideTooHigh, MSGBOX_DEFAULT
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, ShinTokyo_Movement_StepBackUp
	waitmovement 0
	setvar VAR_TEMP_7, 0
	releaseall
	end

ShinTokyo_Movement_StepBackUp:
	walk_up
	step_end

@ The tower's executive entrance (door tile 24,4). Sealed until the shrine
@ trial is complete -- the rebellion's plan needs purification first.
ShinTokyo_EventScript_TowerDoor::
	lockall
	goto_if_set FLAG_WOT_FAIRY_TRIAL_DONE, ShinTokyo_EventScript_TowerDoorOpen
	msgbox ShinTokyo_Text_TowerSealed, MSGBOX_DEFAULT
	closemessage
	applymovement OBJ_EVENT_ID_PLAYER, ShinTokyo_Movement_StepBackDown
	waitmovement 0
	setvar VAR_TEMP_9, 0
	releaseall
	end

ShinTokyo_EventScript_TowerDoorOpen::
	warp MAP_TOWER_STREET, 4, 8
	waitstate
	releaseall
	end

ShinTokyo_Movement_StepBackDown:
	walk_down
	step_end

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

ShinTokyo_Text_TideTooHigh::
	.string "The current out past the break-\n"
	.string "water is a wall of black water.\p"
	.string "Nothing crosses this while the\n"
	.string "tower is drawing power.$"

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
	startquest SIDE_QUEST_MINISTERS_FAVOR
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
@  STEPS 5+6 -- RED ALERT AND THE ARREST (coord-trigger ring around the alley
@  pocket: the player gets a few free steps outside the prison, then any way
@  out of the pocket springs the scene). Choreography-first staging:
@  3 goons converge and surround the player wherever they stand (positions
@  computed from getplayerxy), THE ONI arrives from the north, claims the
@  escort personally, the goons defer and run offscreen -- then a cut, a
@  marched walk to the hideout door with the player in tow, and the ZOROARK
@  illusion drops with the operative and her ZOROARK side by side.
@  Script continues INTO the hideout for the briefing (Shadow Generator lore
@  + the Stop the Machine main quest).
@ ============================================================================
ShinTokyo_EventScript_TryArrest::
	lockall
	setvar VAR_TEMP_3, 1
	goto_if_unset FLAG_ACT3_RED_ALERT, ShinTokyo_EventScript_TryArrestDone
	goto_if_set FLAG_ACT3_ARREST_DONE, ShinTokyo_EventScript_TryArrestDone
	playse SE_EXIT
	msgbox ShinTokyo_Text_RedAlertWail, MSGBOX_DEFAULT
	closemessage
	@ -- the surround: three goons converge on the player's actual tile --
	getplayerxy VAR_0x8004, VAR_0x8005
	copyvar VAR_0x8006, VAR_0x8004
	subvar VAR_0x8006, 3
	setobjectxyperm LOCALID_ARREST_GOON_A, VAR_0x8006, VAR_0x8005
	addobject LOCALID_ARREST_GOON_A
	copyvar VAR_0x8006, VAR_0x8004
	addvar VAR_0x8006, 3
	setobjectxyperm LOCALID_ARREST_GOON_B, VAR_0x8006, VAR_0x8005
	addobject LOCALID_ARREST_GOON_B
	copyvar VAR_0x8007, VAR_0x8005
	addvar VAR_0x8007, 3
	setobjectxyperm LOCALID_ARREST_GOON_C, VAR_0x8004, VAR_0x8007
	addobject LOCALID_ARREST_GOON_C
	applymovement LOCALID_ARREST_GOON_A, ShinTokyo_Movement_GoonConvergeRight
	applymovement LOCALID_ARREST_GOON_B, ShinTokyo_Movement_GoonConvergeLeft
	applymovement LOCALID_ARREST_GOON_C, ShinTokyo_Movement_GoonConvergeUp
	waitmovement LOCALID_ARREST_GOON_A
	waitmovement LOCALID_ARREST_GOON_B
	waitmovement LOCALID_ARREST_GOON_C
	setspeaker SP_NAME_GOON
	msgbox ShinTokyo_Text_GoonsBoxIn, MSGBOX_DEFAULT
	closemessage
	@ -- THE ONI arrives from the north --
	copyvar VAR_0x8007, VAR_0x8005
	subvar VAR_0x8007, 4
	setobjectxyperm LOCALID_ARREST_OFFICER, VAR_0x8004, VAR_0x8007
	addobject LOCALID_ARREST_OFFICER
	applymovement LOCALID_ARREST_OFFICER, ShinTokyo_Movement_OniApproach
	waitmovement LOCALID_ARREST_OFFICER
	applymovement OBJ_EVENT_ID_PLAYER, ShinTokyo_Movement_PlayerFaceUp
	waitmovement 0
	setspeaker SP_NAME_THE_ONI
	msgbox ShinTokyo_Text_ArrestAnnounce, MSGBOX_DEFAULT
	closemessage
	@ -- the goons defer and run offscreen --
	applymovement LOCALID_ARREST_GOON_A, ShinTokyo_Movement_GoonDeferRunLeft
	applymovement LOCALID_ARREST_GOON_B, ShinTokyo_Movement_GoonDeferRunRight
	applymovement LOCALID_ARREST_GOON_C, ShinTokyo_Movement_GoonDeferRunDown
	waitmovement LOCALID_ARREST_GOON_A
	waitmovement LOCALID_ARREST_GOON_B
	waitmovement LOCALID_ARREST_GOON_C
	removeobject LOCALID_ARREST_GOON_A
	removeobject LOCALID_ARREST_GOON_B
	removeobject LOCALID_ARREST_GOON_C
	@ -- the Save Face payoff, compact: the mask comes off on screen --
	checkitem ITEM_CARVED_MASK
	goto_if_eq VAR_RESULT, FALSE, ShinTokyo_EventScript_ArrestNoMask
	setspeaker SP_NAME_THE_ONI
	msgbox ShinTokyo_Text_MaskStripped, MSGBOX_DEFAULT
	closemessage
	playse SE_PIN
	clearflag FLAG_ONI_MASK_WORN
	special RefreshPlayerAvatarGraphicsIfOnFoot
	delay 20
ShinTokyo_EventScript_ArrestNoMask::
	@ -- cut: THE ONI marches the player to the hideout. Scripts do NOT
	@ survive warps, so this scene ENDS at the warp with the flag already
	@ set; the illusion-drop reveal + briefing replay as the RebelHideout
	@ ON_FRAME scene (gated on FLAG_WOT_HIDEOUT_REVEAL_DONE).
	fadescreen FADE_TO_BLACK
	msgbox ShinTokyo_Text_EscortMarch, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_ACT3_ARREST_DONE
	warp MAP_REBEL_HIDEOUT, 7, 8
	waitstate
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

ShinTokyo_Movement_GoonConvergeRight:
	walk_fast_right
	walk_fast_right
	step_end

ShinTokyo_Movement_GoonConvergeLeft:
	walk_fast_left
	walk_fast_left
	step_end

ShinTokyo_Movement_GoonConvergeUp:
	walk_fast_up
	walk_fast_up
	step_end

ShinTokyo_Movement_OniApproach:
	walk_down
	walk_down
	walk_down
	step_end

ShinTokyo_Movement_GoonDeferRunLeft:
	face_up
	delay_16
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	walk_fast_left
	step_end

ShinTokyo_Movement_GoonDeferRunRight:
	face_up
	delay_16
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

ShinTokyo_Movement_GoonDeferRunDown:
	delay_16
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
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

ShinTokyo_Text_Patrol1Intro::
	.string "Prison breaker spotted!\p"
	.string "This one's soul is ALREADY\n"
	.string "sold. Take the bounty!$"

ShinTokyo_Text_Patrol1Defeat::
	.string "The bounty... slipped...$"

ShinTokyo_Text_Patrol2Intro::
	.string "You hear it too, right?\p"
	.string "How quiet they are now.\n"
	.string "The machine likes them QUIET.$"

ShinTokyo_Text_Patrol2Defeat::
	.string "So loud... why are you so\n"
	.string "LOUD...$"

ShinTokyo_Text_Patrol3Intro::
	.string "Curfew, citizen.\p"
	.string "Forever.$"

ShinTokyo_Text_Patrol3Defeat::
	.string "Impossible. They don't even\n"
	.string "feel pain anymore...$"

ShinTokyo_Text_PatrolPost::
	.string "The patroller stares at the\n"
	.string "empty BALL on their belt.$"

ShinTokyo_Text_TowerSealed::
	.string "MUTRID TOWER -- EXECUTIVE\n"
	.string "ENTRANCE.\p"
	.string "Sealed. Whatever waits above\n"
	.string "unmakes anything it touches.\p"
	.string "Without the shrine's power to\n"
	.string "undo it, this door is a grave.$"

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

ShinTokyo_Text_GoonsBoxIn::
	.string "Bounty target boxed in!\p"
	.string "Nobody touch the prize --\n"
	.string "hold for command!$"

ShinTokyo_Text_ArrestAnnounce::
	.string "By authority of the tower:\n"
	.string "you are under arrest.\p"
	.string "I will escort the prisoner\n"
	.string "PERSONALLY.$"

ShinTokyo_Text_MaskStripped::
	.string "That carnival prop.\n"
	.string "Off.$"

ShinTokyo_Text_EscortMarch::
	.string "The ONI marches you through\n"
	.string "the neon rain.\p"
	.string "Nobody on the street meets\n"
	.string "your eyes.$"

@ --- The briefing (all PLACEHOLDER (Joe)) -----------------------------------

@ ============================================================================
@  THE BLACK MARKET (alleyway) -- three vendors moving Tradewind department
@  store stock at street prices, plus everything Mutrid outlawed.
@  All dialogue PLACEHOLDER (Joe).
@ ============================================================================
ShinTokyo_EventScript_BlackMarketAM::
	lock
	faceplayer
	message ShinTokyo_Text_BlackMarketAM
	waitmessage
	pokemart ShinTokyo_Pokemart_BlackMarketAMs
	msgbox ShinTokyo_Text_BlackMarketBye, MSGBOX_DEFAULT
	release
	end

	.align 2
ShinTokyo_Pokemart_BlackMarketAMs:
	.2byte ITEM_AM_INTIMIDATE
	.2byte ITEM_AM_GUTS
	.2byte ITEM_AM_TECHNICIAN
	.2byte ITEM_AM_MOXIE
	.2byte ITEM_AM_ADAPTABILITY
	.2byte ITEM_AM_SHEER_FORCE
	.2byte ITEM_AM_TOUGH_CLAWS
	.2byte ITEM_AM_IRON_FIST
	.2byte ITEM_AM_TINTED_LENS
	.2byte ITEM_AM_SKILL_LINK
	.2byte ITEM_AM_THICK_FAT
	.2byte ITEM_AM_STURDY
	.2byte ITEM_AM_LEVITATE
	.2byte ITEM_AM_UNAWARE
	.2byte ITEM_AM_SERENE_GRACE
	.2byte ITEM_AM_PRANKSTER
	.2byte ITEM_AM_POISON_HEAL
	.2byte ITEM_AM_MULTISCALE
	.2byte ITEM_AM_MAGIC_GUARD
	.2byte ITEM_AM_REGENERATOR
	.2byte ITEM_AM_SPEED_BOOST
	.2byte ITEM_AM_MAGIC_BOUNCE
	pokemartlistend

ShinTokyo_EventScript_BlackMarketPaw::
	lock
	faceplayer
	message ShinTokyo_Text_BlackMarketPaw
	waitmessage
	pokemart ShinTokyo_Pokemart_BlackMarketPaws
	msgbox ShinTokyo_Text_BlackMarketBye, MSGBOX_DEFAULT
	release
	end

	.align 2
ShinTokyo_Pokemart_BlackMarketPaws:
	.2byte ITEM_MANKEYS_PAW
	.2byte ITEM_PRIMEAPES_PAW
	.2byte ITEM_ANNIHILAPES_PAW
	pokemartlistend

ShinTokyo_EventScript_BlackMarketBalls::
	lock
	faceplayer
	message ShinTokyo_Text_BlackMarketBalls
	waitmessage
	pokemart ShinTokyo_Pokemart_BlackMarketBalls
	msgbox ShinTokyo_Text_BlackMarketBye, MSGBOX_DEFAULT
	release
	end

	.align 2
ShinTokyo_Pokemart_BlackMarketBalls:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_ULTRA_BALL
	.2byte ITEM_PREMIER_BALL
	.2byte ITEM_HEAL_BALL
	.2byte ITEM_NET_BALL
	.2byte ITEM_NEST_BALL
	.2byte ITEM_DIVE_BALL
	.2byte ITEM_DUSK_BALL
	.2byte ITEM_TIMER_BALL
	.2byte ITEM_QUICK_BALL
	.2byte ITEM_REPEAT_BALL
	.2byte ITEM_LUXURY_BALL
	.2byte ITEM_LEVEL_BALL
	.2byte ITEM_LURE_BALL
	.2byte ITEM_MOON_BALL
	.2byte ITEM_FRIEND_BALL
	.2byte ITEM_LOVE_BALL
	.2byte ITEM_FAST_BALL
	.2byte ITEM_HEAVY_BALL
	.2byte ITEM_DREAM_BALL
	.2byte ITEM_SAFARI_BALL
	.2byte ITEM_SPORT_BALL
	.2byte ITEM_BEAST_BALL
	.2byte ITEM_CHERISH_BALL
	pokemartlistend

@ --- hidden valuables -------------------------------------------------------
ShinTokyo_EventScript_ItemAlley::
	finditem ITEM_BIG_NUGGET
	end

ShinTokyo_EventScript_ItemPlaza::
	finditem ITEM_PEARL_STRING
	end

ShinTokyo_Text_BlackMarketAM::
	.string "Ability Machines. Lab-grade.\n"
	.string "Fell off a Mutrid convoy.\p"
	.string "No refunds. No receipts.\n"
	.string "No questions.$"

ShinTokyo_Text_BlackMarketPaw::
	.string "Careful with these.\p"
	.string "A paw grants exactly what you\n"
	.string "ask. That's the problem.$"

ShinTokyo_Text_BlackMarketBalls::
	.string "Every ball ever made. Don't ask\n"
	.string "how I got the rare ones.\p"
	.string "Mutrid outlawed catching.\n"
	.string "I outlawed listening.$"

ShinTokyo_Text_BlackMarketBye::
	.string "You were never here.$"

@ --- the jumbotron objects (sign interactions reuse the screen texts) -------
ShinTokyo_EventScript_LeaderScreen::
	msgbox ShinTokyo_Text_PropagandaScreen, MSGBOX_SIGN
	end

ShinTokyo_EventScript_WantedScreen::
	msgbox ShinTokyo_Text_BountyScreen, MSGBOX_SIGN
	end

@ --- THE MINISTER (in the flesh, east side) ---------------------------------
ShinTokyo_EventScript_Minister::
	lock
	faceplayer
	setspeaker SP_NAME_MINISTER
	questmenu QUEST_MENU_CHECK_ACTIVE, SIDE_QUEST_MINISTERS_FAVOR
	goto_if_eq VAR_RESULT, TRUE, ShinTokyo_EventScript_MinisterSettle
	msgbox ShinTokyo_Text_MinisterIdle, MSGBOX_DEFAULT
	release
	end

ShinTokyo_EventScript_MinisterSettle::
	goto_if_unset FLAG_ACT3_MIKMANC_FREED, ShinTokyo_EventScript_MinisterWaiting
	msgbox ShinTokyo_Text_MinisterSettle, MSGBOX_DEFAULT
	closemessage
	completequest SIDE_QUEST_MINISTERS_FAVOR
	giveitem ITEM_BIG_NUGGET
	msgbox ShinTokyo_Text_MinisterAfter, MSGBOX_DEFAULT
	release
	end

ShinTokyo_EventScript_MinisterWaiting::
	msgbox ShinTokyo_Text_MinisterWaiting, MSGBOX_DEFAULT
	release
	end

@ --- city Pokemon ------------------------------------------------------------
ShinTokyo_EventScript_MonWeezing::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_WEEZING_GALAR, CRY_MODE_NORMAL
	waitmoncry
	msgbox ShinTokyo_Text_MonWeezing, MSGBOX_DEFAULT
	release
	end

ShinTokyo_EventScript_MonKoffing::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_KOFFING, CRY_MODE_NORMAL
	waitmoncry
	msgbox ShinTokyo_Text_MonKoffing, MSGBOX_DEFAULT
	release
	end

ShinTokyo_EventScript_MonGrimer::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_GRIMER, CRY_MODE_NORMAL
	waitmoncry
	msgbox ShinTokyo_Text_MonGrimer, MSGBOX_DEFAULT
	release
	end

ShinTokyo_EventScript_MonRevavroom::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_REVAVROOM, CRY_MODE_NORMAL
	waitmoncry
	msgbox ShinTokyo_Text_MonRevavroom, MSGBOX_DEFAULT
	release
	end

ShinTokyo_EventScript_SmallPoster::
	msgbox ShinTokyo_Text_SmallPoster, MSGBOX_SIGN
	end

ShinTokyo_Text_MinisterIdle::
	.string "Ah. A citizen.\p"
	.string "Enjoy the city. It counts\n"
	.string "everything you enjoy.$"

ShinTokyo_Text_MinisterSettle::
	.string "The prisoner walked free.\n"
	.string "One phone call. Mine.\p"
	.string "I never forget a favor,\n"
	.string "{PLAYER}.\p"
	.string "More importantly--\n"
	.string "neither do you. Now we're\l"
	.string "both invested.$"

ShinTokyo_Text_MinisterAfter::
	.string "Walk carefully.\p"
	.string "The tower watches its\n"
	.string "investments closest of all.$"

ShinTokyo_Text_MonWeezing::
	.string "WEEZING drinks from the\n"
	.string "factory vents.\p"
	.string "It looks healthier than\n"
	.string "anyone else on this street.$"

ShinTokyo_Text_MonKoffing::
	.string "KOFFING bobs along the alley,\n"
	.string "perfectly at home.$"

ShinTokyo_Text_MonGrimer::
	.string "GRIMER oozes out of a storm\n"
	.string "drain.\p"
	.string "It waves. Politely.$"

ShinTokyo_Text_MonRevavroom::
	.string "REVAVROOM idles by the curb,\n"
	.string "revving at passing cars.\p"
	.string "It has no driver.\n"
	.string "It needs no driver.$"

ShinTokyo_Text_SmallPoster::
	.string "WANTED: the terrorist\n"
	.string "“{PLAYER}”.\p"
	.string "Reward: 1,000,000.\n"
	.string "Approach with caution.$"

ShinTokyo_Text_MinisterWaiting::
	.string "The cell I opened is still\n"
	.string "occupied, {PLAYER}.\p"
	.string "Finish the job.$"
