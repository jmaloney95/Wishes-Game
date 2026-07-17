.equ LOCALID_SA2_RYOKO,       2
.equ LOCALID_SA2_YIFFER,      3
.equ LOCALID_SA2_MACHAMP,     4
.equ LOCALID_SA2_ALAKAZAM,    5
.equ LOCALID_SA2_GOON_MID,    6
.equ LOCALID_SA2_GEN,         7
.equ LOCALID_SA2_CHANDELURE,  8
.equ LOCALID_SA2_HONCHKROW,   9
.equ LOCALID_SA2_GOON_WEST,  10
.equ LOCALID_SA2_REDFATALITY,11
.equ LOCALID_SA2_RAICHU,     12
.equ LOCALID_SA2_HYDREIGON,  13

SennenAct2_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SennenAct2_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SennenAct2_OnFrame
	.byte 0

SennenAct2_OnTransition::
	goto_if_unset FLAG_SENNEN_ACT2_INTRO_SEEN, SennenAct2_OnTransition_End
	setvar VAR_TEMP_1, 1
SennenAct2_OnTransition_End::
	end

SennenAct2_OnFrame:
	map_script_2 VAR_TEMP_1, 0, SennenAct2_EventScript_AssaultCutscene
	.2byte 0

@ ============================================================================
@  SENNEN STATION -- THE ASSAULT (Act 2 finale)
@  The Rebellion hits the station so the player can board the Shinkansen.
@  Live battles rage on the sides (rebel + their mon vs goon + their mon,
@  ambient) -- the rebels are getting CRUSHED, but that was always the plan:
@  every mask in the region is looking at THEM, not at the player.
@  Two Mutrid CAPTAINS (medium battles, trainers 885/886) guard the final
@  approach to the train door at (32,12).
@  The door is a bg event: SHINKANSEN KEY swipes it open, clears
@  FLAG_HIDE_SHINKANSEN_ACT2_GOONS (act-2 passengers appear inside) and
@  warps aboard MAP_SHINKANSEN_INTERIOR (8,3).
@  NOTE: no exit warps -- you don't leave an assault. Entry wiring from the
@  safehouse + propaganda/map dressing are handled by the user in Porymap.
@  All coords are best-guess vs LAYOUT_SENNEN_ACT2 -- nudge freely.
@ ============================================================================

@ ============================================================================
@  THE STATION ASSAULT -- entry cutscene (plays once, on first arrival).
@  Camera tours three live battles left-to-right:
@   WEST (x=7 column):  Master Gen + Chandelure vs goon + Honchkrow -> Gen WINS
@   MID  (y=22 row):    Yiffer + Machamp vs Alakazam + goon -> Machamp KO'd,
@                       Yiffer knocked back and gone (Clarkson-death SE)
@   EAST (y=20 row):    RedFatality + Raichu vs Ryoko's Hydreigon -> Raichu
@                       KO'd, RedFatality knocked back and gone
@  Then Ryoko recalls Hydreigon, marches to the player, and attacks.
@  Death-scene gotcha (from StarSummit): set the object's hide FLAG before
@  removeobject, or the engine respawns it when the camera moves.
@ ============================================================================
SennenAct2_EventScript_AssaultCutscene::
	lockall
	setvar VAR_TEMP_1, 1
	setflag FLAG_SAFE_FOLLOWER_MOVEMENT
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_FaceUp
	waitmovement 0
	delay 30
	special SpawnCameraObject
	@ ---- pan to the WEST battle (camera lands ~8,23) ----
	applymovement LOCALID_CAMERA, SennenAct2_Movement_CameraToWest
	waitmovement 0
	delay 16
	@ Honchkrow strikes first
	waitse
	playmoncry SPECIES_HONCHKROW, CRY_MODE_NORMAL
	applymovement LOCALID_SA2_HONCHKROW, SennenAct2_Movement_LungeUp
	waitmovement 0
	applymovement LOCALID_SA2_CHANDELURE, SennenAct2_Movement_FlinchDown
	waitmovement 0
	waitmoncry
	setspeaker SP_NAME_MASTER_GEN
	msgbox SennenAct2_Text_GenBattleCry, MSGBOX_DEFAULT
	closemessage
	@ Chandelure answers -- and ends it
	waitse
	playmoncry SPECIES_CHANDELURE, CRY_MODE_ENCOUNTER
	playse SE_M_FLAMETHROWER
	applymovement LOCALID_SA2_CHANDELURE, SennenAct2_Movement_LungeDown
	waitmovement 0
	setvar VAR_0x8004, 1
	setvar VAR_0x8005, 1
	setvar VAR_0x8006, 4
	setvar VAR_0x8007, 5
	special ShakeCamera
	waitstate
	waitse
	playmoncry SPECIES_HONCHKROW, CRY_MODE_FAINT
	waitmoncry
	playse SE_M_TELEPORT
	setflag FLAG_SENNEN_ACT2_B1_FALLEN
	removeobject LOCALID_SA2_HONCHKROW
	delay 12
	@ the west goon breaks and runs
	setspeaker SP_NAME_GOON
	msgbox SennenAct2_Text_GoonRout, MSGBOX_DEFAULT
	closemessage
	applymovement LOCALID_SA2_GOON_WEST, SennenAct2_Movement_GoonRout
	waitmovement 0
	setflag FLAG_SENNEN_ACT2_GOONS_CLEARED
	removeobject LOCALID_SA2_GOON_WEST
	delay 20
	@ ---- pan to the MID battle (camera lands ~14,22) ----
	applymovement LOCALID_CAMERA, SennenAct2_Movement_CameraToMid
	waitmovement 0
	delay 16
	@ Machamp presses the attack
	waitse
	playmoncry SPECIES_MACHAMP, CRY_MODE_NORMAL
	applymovement LOCALID_SA2_MACHAMP, SennenAct2_Movement_LungeRight
	waitmovement 0
	applymovement LOCALID_SA2_ALAKAZAM, SennenAct2_Movement_FlinchLeft
	waitmovement 0
	waitmoncry
	setspeaker SP_NAME_YIFFER
	msgbox SennenAct2_Text_YifferBattleCry, MSGBOX_DEFAULT
	closemessage
	@ Alakazam answers -- Machamp drops
	waitse
	playmoncry SPECIES_ALAKAZAM, CRY_MODE_ENCOUNTER
	playse SE_M_PSYBEAM
	applymovement LOCALID_SA2_ALAKAZAM, SennenAct2_Movement_LungeLeft
	waitmovement 0
	setvar VAR_0x8004, 1
	setvar VAR_0x8005, 1
	setvar VAR_0x8006, 4
	setvar VAR_0x8007, 5
	special ShakeCamera
	waitstate
	waitse
	playmoncry SPECIES_MACHAMP, CRY_MODE_FAINT
	waitmoncry
	playse SE_M_TELEPORT
	setflag FLAG_SENNEN_ACT2_B2_FALLEN
	removeobject LOCALID_SA2_MACHAMP
	delay 12
	@ Yiffer takes the recoil -- knocked back a square, then gone
	applymovement LOCALID_SA2_YIFFER, SennenAct2_Movement_KnockedBack
	waitmovement 0
	delay 10
	playse SE_LAVARIDGE_FALL_WARP
	delay 28
	removeobject LOCALID_SA2_YIFFER
	delay 20
	@ ---- pan to the EAST battle (camera lands ~19,21) ----
	applymovement LOCALID_CAMERA, SennenAct2_Movement_CameraToEast
	waitmovement 0
	delay 16
	setspeaker SP_NAME_RED_FATALITY
	msgbox SennenAct2_Text_RedFatalityBattleCry, MSGBOX_DEFAULT
	closemessage
	@ Raichu gives everything it has
	waitse
	playmoncry SPECIES_RAICHU, CRY_MODE_NORMAL
	playse SE_THUNDER
	applymovement LOCALID_SA2_RAICHU, SennenAct2_Movement_LungeRight
	waitmovement 0
	applymovement LOCALID_SA2_HYDREIGON, SennenAct2_Movement_FlinchLeft
	waitmovement 0
	waitmoncry
	@ Hydreigon answers -- Raichu drops
	waitse
	playmoncry SPECIES_HYDREIGON, CRY_MODE_ENCOUNTER
	playse SE_M_DRAGON_RAGE
	applymovement LOCALID_SA2_HYDREIGON, SennenAct2_Movement_LungeLeft
	waitmovement 0
	setvar VAR_0x8004, 2
	setvar VAR_0x8005, 2
	setvar VAR_0x8006, 6
	setvar VAR_0x8007, 4
	special ShakeCamera
	waitstate
	waitse
	playmoncry SPECIES_RAICHU, CRY_MODE_FAINT
	waitmoncry
	playse SE_M_TELEPORT
	setflag FLAG_SENNEN_ACT2_B3_FALLEN
	removeobject LOCALID_SA2_RAICHU
	delay 12
	@ RedFatality falls the same way Yiffer did
	applymovement LOCALID_SA2_REDFATALITY, SennenAct2_Movement_KnockedBack
	waitmovement 0
	delay 10
	playse SE_LAVARIDGE_FALL_WARP
	delay 28
	removeobject LOCALID_SA2_REDFATALITY
	delay 24
	@ Ryoko recalls Hydreigon and turns on the player
	playse SE_BALL
	removeobject LOCALID_SA2_HYDREIGON
	delay 16
	applymovement LOCALID_SA2_RYOKO, SennenAct2_Movement_RyokoExclaim
	waitmovement 0
	@ ---- camera returns to the player; Ryoko marches down ----
	applymovement LOCALID_CAMERA, SennenAct2_Movement_CameraToPlayer
	waitmovement 0
	special RemoveCameraObject
	applymovement LOCALID_SA2_RYOKO, SennenAct2_Movement_RyokoApproach
	waitmovement 0
	setspeaker SP_NAME_RYOKO
	msgbox SennenAct2_Text_RyokoChallenge, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_SENNEN_ACT2_INTRO_SEEN
	clearflag FLAG_SAFE_FOLLOWER_MOVEMENT
	trainerbattle_no_intro TRAINER_SENNEN_CAPTAIN_RYOKO, SennenAct2_Text_RyokoDefeat
	setspeaker SP_NAME_RYOKO
	msgbox SennenAct2_Text_RyokoPostBattle, MSGBOX_DEFAULT
	releaseall
	end

@ --------------------------------------------------------------------------
@  Cutscene movement
@ --------------------------------------------------------------------------
SennenAct2_Movement_CameraToWest:
	walk_up
	walk_up
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_up
	walk_up
	walk_up
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

SennenAct2_Movement_CameraToMid:
	walk_right
	walk_right
	walk_right
	walk_up
	walk_right
	walk_right
	walk_right
	step_end

SennenAct2_Movement_CameraToEast:
	walk_right
	walk_right
	walk_right
	walk_up
	walk_right
	walk_right
	step_end

SennenAct2_Movement_CameraToPlayer:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	step_end

SennenAct2_Movement_LungeUp:
	jump_in_place_up
	step_end

SennenAct2_Movement_LungeDown:
	jump_in_place_down
	step_end

SennenAct2_Movement_LungeLeft:
	jump_in_place_left
	step_end

SennenAct2_Movement_LungeRight:
	jump_in_place_right
	step_end

SennenAct2_Movement_FlinchDown:
	walk_in_place_fast_down
	step_end

SennenAct2_Movement_FlinchLeft:
	walk_in_place_fast_left
	step_end

SennenAct2_Movement_GoonRout:
	face_down
	delay_8
	walk_fast_down
	walk_fast_down
	step_end

SennenAct2_Movement_KnockedBack:
	walk_fast_left
	face_right
	step_end

SennenAct2_Movement_RyokoExclaim:
	emote_exclamation_mark
	delay_16
	face_down
	step_end

SennenAct2_Movement_RyokoApproach:
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_left
	walk_left
	walk_left
	walk_left
	walk_left
	walk_down
	face_down
	step_end

@ --------------------------------------------------------------------------
@  The captains
@ --------------------------------------------------------------------------
SennenAct2_EventScript_CaptainKannon::
	trainerbattle_single TRAINER_SENNEN_CAPTAIN_KANNON, SennenAct2_Text_KannonIntro, SennenAct2_Text_KannonDefeat
	msgbox SennenAct2_Text_KannonPostBattle, MSGBOX_AUTOCLOSE
	end

SennenAct2_EventScript_CaptainRyoko::
	trainerbattle_single TRAINER_SENNEN_CAPTAIN_RYOKO, SennenAct2_Text_RyokoIntro, SennenAct2_Text_RyokoDefeat
	msgbox SennenAct2_Text_RyokoPostBattle, MSGBOX_AUTOCLOSE
	end

@ --------------------------------------------------------------------------
@  The train door (bg event at 32,12)
@ --------------------------------------------------------------------------
SennenAct2_EventScript_TrainDoor::
	lockall
	checkitem ITEM_SHINKANSEN_KEY
	goto_if_eq VAR_RESULT, FALSE, SennenAct2_EventScript_DoorLocked
	msgbox SennenAct2_Text_DoorSwipe, MSGBOX_DEFAULT
	closemessage
	playse SE_PC_ON
	delay 20
	playse SE_DOOR
	msgbox SennenAct2_Text_DoorOpens, MSGBOX_DEFAULT
	closemessage
	clearflag FLAG_HIDE_SHINKANSEN_ACT2_GOONS
	fadescreen FADE_TO_BLACK
	delay 20
	warp MAP_SHINKANSEN_INTERIOR, 8, 3
	waitstate
	releaseall
	end

SennenAct2_EventScript_DoorLocked::
	msgbox SennenAct2_Text_DoorLocked, MSGBOX_DEFAULT
	releaseall
	end

@ --------------------------------------------------------------------------
@  The battlefield -- rebels holding the line (ambient)
@ --------------------------------------------------------------------------
SennenAct2_EventScript_RebelFighter1::
	lock
	faceplayer
	msgbox SennenAct2_Text_RebelFighter1, MSGBOX_DEFAULT
	release
	end

SennenAct2_EventScript_RebelFighter2::
	lock
	faceplayer
	msgbox SennenAct2_Text_RebelFighter2, MSGBOX_DEFAULT
	release
	end

SennenAct2_EventScript_RebelFighter3::
	lock
	faceplayer
	msgbox SennenAct2_Text_RebelFighter3, MSGBOX_DEFAULT
	release
	end

SennenAct2_EventScript_GoonFighter1::
	lock
	faceplayer
	msgbox SennenAct2_Text_GoonFighter1, MSGBOX_DEFAULT
	release
	end

SennenAct2_EventScript_GoonFighter2::
	lock
	faceplayer
	msgbox SennenAct2_Text_GoonFighter2, MSGBOX_DEFAULT
	release
	end

SennenAct2_EventScript_GoonFighter3::
	lock
	faceplayer
	msgbox SennenAct2_Text_GoonFighter3, MSGBOX_DEFAULT
	release
	end

SennenAct2_EventScript_RebelMon1::
	lock
	waitse
	playmoncry SPECIES_MACHAMP, CRY_MODE_WEAK
	msgbox SennenAct2_Text_RebelMon1, MSGBOX_DEFAULT
	waitmoncry
	release
	end

SennenAct2_EventScript_RebelMon2::
	lock
	waitse
	playmoncry SPECIES_CHANDELURE, CRY_MODE_WEAK
	msgbox SennenAct2_Text_RebelMon2, MSGBOX_DEFAULT
	waitmoncry
	release
	end

SennenAct2_EventScript_RebelMon3::
	lock
	waitse
	playmoncry SPECIES_RAICHU, CRY_MODE_WEAK
	msgbox SennenAct2_Text_RebelMon3, MSGBOX_DEFAULT
	waitmoncry
	release
	end

SennenAct2_EventScript_GoonMon1::
	lock
	waitse
	playmoncry SPECIES_HOUNDOOM, CRY_MODE_NORMAL
	msgbox SennenAct2_Text_GoonMon1, MSGBOX_DEFAULT
	waitmoncry
	release
	end

SennenAct2_EventScript_GoonMon2::
	lock
	waitse
	playmoncry SPECIES_GOLBAT, CRY_MODE_NORMAL
	msgbox SennenAct2_Text_GoonMon2, MSGBOX_DEFAULT
	waitmoncry
	release
	end

SennenAct2_EventScript_GoonMon3::
	lock
	waitse
	playmoncry SPECIES_WEEZING, CRY_MODE_NORMAL
	msgbox SennenAct2_Text_GoonMon3, MSGBOX_DEFAULT
	waitmoncry
	release
	end

@ ============================================================================
@  Text
@ ============================================================================
SennenAct2_Text_KannonIntro::
	.string "CAPTAIN KANNON. Nobody boards.\p"
	.string "The rabble out there can bleed on\n"
	.string "the cobbles all they like.\p"
	.string "The PLATFORM is mine.$"

SennenAct2_Text_KannonDefeat::
	.string "The platform... was mine...$"

SennenAct2_Text_KannonPostBattle::
	.string "You're the one they're bleeding\n"
	.string "for. I see it now. Too late.$"

SennenAct2_Text_RyokoIntro::
	.string "CAPTAIN RYOKO. Hm. The little\n"
	.string "ghost the reports keep dying\l"
	.string "over.\p"
	.string "The Wish wants you kneeling.\n"
	.string "I'll settle for flattened.$"

SennenAct2_Text_RyokoDefeat::
	.string "Flattened... me...$"

SennenAct2_Text_RyokoPostBattle::
	.string "Go, then. Board.\p"
	.string "Shin-Tokyo doesn't spit anyone\n"
	.string "back out.$"

SennenAct2_Text_DoorLocked::
	.string "The Shinkansen's door is sealed.\n"
	.string "A card reader blinks red, red,\l"
	.string "red.\p"
	.string "It wants a key that Mutrid\n"
	.string "doesn't have.$"

SennenAct2_Text_DoorSwipe::
	.string "{PLAYER} swiped the SHINKANSEN\n"
	.string "KEY.\p"
	.string "Somewhere in Sennen, a vending\n"
	.string "machine begins looping camera\l"
	.string "footage.\p"
	.string "The reader blinks green.$"

SennenAct2_Text_DoorOpens::
	.string "The door hisses open.\p"
	.string "Behind you: thunder, smoke, and\n"
	.string "every voice you love, shouting.\p"
	.string "You don't look back.\n"
	.string "You look FORWARD.$"

SennenAct2_Text_RebelFighter1::
	.string "REBEL: Two more! Come ON, big\n"
	.string "guy, stay up, stay UP--\p"
	.string "{PLAYER}!! Eyes off us!\n"
	.string "The DOOR! GO!$"

SennenAct2_Text_RebelFighter2::
	.string "REBEL: Third wave. There's always\n"
	.string "a third wave, isn't there.\p"
	.string "We're the loudest thing in\n"
	.string "Sennen, exactly like Minister\l"
	.string "drew it up.\p"
	.string "Every second you stand here, you\n"
	.string "waste what we're PAYING for. RUN!$"

SennenAct2_Text_RebelFighter3::
	.string "REBEL: Ha... haha... my poor\n"
	.string "Raichu's down to fumes and I\l"
	.string "can't feel my arm.\p"
	.string "Best day of my life. We're worth\n"
	.string "SOMETHING today.\p"
	.string "Board that train, kid. Make it\n"
	.string "worth EVERYTHING.$"

SennenAct2_Text_GoonFighter1::
	.string "GOON: Stay out of this, brat --\n"
	.string "we'll get to you after we mop up\l"
	.string "these station rats!$"

SennenAct2_Text_GoonFighter2::
	.string "GOON: Wave after wave of them!\n"
	.string "Where do these rebels keep\l"
	.string "COMING from?!\p"
	.string "...wait. Why would they all-- oh\n"
	.string "no. OH NO. IT'S A DIVERSION--$"

SennenAct2_Text_GoonMon1::
	.string "Houndoom's jaws glow like a\n"
	.string "furnace door.\p"
	.string "The Machamp facing it is bleeding\n"
	.string "from three arms... and grinning\l"
	.string "with everything else.$"

SennenAct2_Text_GoonMon2::
	.string "The Golbat wheels and dives,\n"
	.string "shrieking.\p"
	.string "The Chandelure's flames have\n"
	.string "gone small and blue -- almost\l"
	.string "out. Almost.$"

SennenAct2_Text_GoonMon3::
	.string "The Weezing floods the platform\n"
	.string "with smog.\p"
	.string "Through it, the Raichu keeps\n"
	.string "sparking. Dim... but sparking.$"

SennenAct2_Text_RebelMon1::
	.string "Machamp plants all four fists and\n"
	.string "REFUSES to fall.$"

SennenAct2_Text_RebelMon2::
	.string "Chandelure's song is thin and\n"
	.string "guttering.\p"
	.string "It burns anyway.$"

SennenAct2_Text_RebelMon3::
	.string "Raichu drags itself back up.\n"
	.string "Again.$"

SennenAct2_Text_GenBattleCry::
	.string "Hold the line!\p"
	.string "CHANDELURE -- light them a\n"
	.string "lesson they'll keep!$"

SennenAct2_Text_GoonRout::
	.string "The station belongs to the\n"
	.string "WISH-- agh! Fall back!$"

SennenAct2_Text_YifferBattleCry::
	.string "MACHAMP doesn't kneel!\n"
	.string "And neither do I!$"

SennenAct2_Text_RedFatalityBattleCry::
	.string "RAICHU, one more! Everything\n"
	.string "you've got left!$"

SennenAct2_Text_RyokoChallenge::
	.string "Hm. You watched three heroes\n"
	.string "break, little ghost.\p"
	.string "The Wish wants you kneeling.\n"
	.string "Come. Let's make it four.$"

SennenAct2_Text_GoonFighter3::
	.string "GOON: The captains hold the\n"
	.string "platform. NOBODY reaches that\l"
	.string "train.\p"
	.string "...Right? Somebody tell me I'm\n"
	.string "right.$"
