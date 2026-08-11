.equ LOCALID_SA2_EDWARDS,     1
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
.equ LOCALID_SA2_GARCHOMP,   13
.equ LOCALID_SA2_JET,        15
.equ LOCALID_SA2_IMPACT,     16
.equ LOCALID_SA2_FALLER,     17

SennenAct2_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SennenAct2_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, SennenAct2_OnFrame
	.byte 0

@ Edwards only exists on the gate AFTER his jet drop-in: pre-battle he is
@ hidden (AWAY flag) and arrives via the ambush trigger; once defeated he
@ stands at his template post like any NPC. The jet/faller/impact props are
@ re-hidden on every map load.
SennenAct2_OnTransition::
	setrespawn HEAL_LOCATION_DOJO
	setflag FLAG_WOT_SA2_AIRSHOW
	setflag FLAG_WOT_SA2_FALLER
	goto_if_defeated TRAINER_SENNEN_CAPTAIN_KANNON, SennenAct2_OnTransition_EdwardsDone
	setflag FLAG_WOT_SA2_EDWARDS_AWAY
	goto SennenAct2_OnTransition_Intro
SennenAct2_OnTransition_EdwardsDone::
	clearflag FLAG_WOT_SA2_EDWARDS_AWAY
	setvar VAR_TEMP_2, 1
SennenAct2_OnTransition_Intro::
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
	@ Free the follower's object slot: 14 templates + player + camera is
	@ exactly OBJECT_EVENTS_COUNT; a follower mon starves one actor's respawn.
	special RemoveFollowingPokemon
	@ Same storm-audio mute as the Edwards ambush -- scene SEs own the mix.
	special WotSuppressWeatherSE
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
	@ Own flag! Setting the shared GOONS_CLEARED here blocked Garchomp's
	@ respawn after the west pan culled him -- the "sometimes no Garchomp" bug.
	setflag FLAG_WOT_SA2_GOON_WEST_FLED
	removeobject LOCALID_SA2_GOON_WEST
	delay 20
	@ ---- pan to the MID battle (camera lands ~14,22) ----
	applymovement LOCALID_CAMERA, SennenAct2_Movement_CameraToMid
	waitmovement 0
	delay 16
	@ Machamp presses the attack
	waitse
	playmoncry SPECIES_MACHAMP, CRY_MODE_NORMAL
	applymovement LOCALID_SA2_MACHAMP, SennenAct2_Movement_LungeDown
	waitmovement 0
	applymovement LOCALID_SA2_ALAKAZAM, SennenAct2_Movement_FlinchUp
	waitmovement 0
	waitmoncry
	setspeaker SP_NAME_YIFFER
	msgbox SennenAct2_Text_YifferBattleCry, MSGBOX_DEFAULT
	closemessage
	@ Alakazam answers -- Machamp drops
	waitse
	playmoncry SPECIES_ALAKAZAM, CRY_MODE_ENCOUNTER
	playse SE_M_PSYBEAM
	applymovement LOCALID_SA2_ALAKAZAM, SennenAct2_Movement_LungeUp
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
	applymovement LOCALID_SA2_YIFFER, SennenAct2_Movement_KnockedIntoSea
	waitmovement 0
	playse SE_M_SURF
	delay 24
	removeobject LOCALID_SA2_YIFFER
	delay 20
	@ ---- pan to the EAST battle (camera lands ~19,21) ----
	@ Insurance: if any east actor was culled during the west/mid pans,
	@ force-respawn it before the camera arrives (no-ops when present).
	addobject LOCALID_SA2_REDFATALITY
	addobject LOCALID_SA2_RAICHU
	addobject LOCALID_SA2_GARCHOMP
	addobject LOCALID_SA2_RYOKO
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
	applymovement LOCALID_SA2_RAICHU, SennenAct2_Movement_LungeDown
	waitmovement 0
	applymovement LOCALID_SA2_GARCHOMP, SennenAct2_Movement_FlinchUp
	waitmovement 0
	waitmoncry
	@ Hydreigon answers -- Raichu drops
	waitse
	playmoncry SPECIES_GARCHOMP, CRY_MODE_ENCOUNTER
	playse SE_M_DRAGON_RAGE
	applymovement LOCALID_SA2_GARCHOMP, SennenAct2_Movement_LungeUp
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
	applymovement LOCALID_SA2_REDFATALITY, SennenAct2_Movement_KnockedIntoSea
	waitmovement 0
	playse SE_M_SURF
	delay 24
	removeobject LOCALID_SA2_REDFATALITY
	delay 24
	@ ---- camera returns to the player FIRST; the platform is tight enough
	@ now that Ryoko's whole reaction plays in view from here ----
	applymovement LOCALID_CAMERA, SennenAct2_Movement_CameraBackToPlayer
	waitmovement 0
	special RemoveCameraObject
	@ Ryoko recalls his partner and whirls on the player
	playse SE_BALL
	setflag FLAG_WOT_SA2_GARCHOMP_AWAY
	removeobject LOCALID_SA2_GARCHOMP
	delay 16
	applymovement LOCALID_SA2_RYOKO, SennenAct2_Movement_RyokoExclaim
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
@ (16,22) -> (6,16): one smooth diagonal sweep, then a short slide west.
SennenAct2_Movement_CameraToWest:
	walk_diag_northwest
	walk_diag_northwest
	walk_diag_northwest
	walk_diag_northwest
	walk_diag_northwest
	walk_diag_northwest
	walk_left
	walk_left
	walk_left
	walk_left
	step_end

@ (6,16) -> (12,14): drift up-right, then east along the platform.
SennenAct2_Movement_CameraToMid:
	walk_diag_northeast
	walk_diag_northeast
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

@ (12,14) -> (17,15): glide east with a slight settle down.
SennenAct2_Movement_CameraToEast:
	walk_diag_southeast
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

@ (17,15) -> (16,22): glide back down to the player.
SennenAct2_Movement_CameraBackToPlayer:
	walk_diag_southwest
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
	walk_down
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

SennenAct2_Movement_FlinchUp:
	walk_in_place_fast_up
	step_end

SennenAct2_Movement_FlinchLeft:
	walk_in_place_fast_left
	step_end

@ Panicked scramble right-and-down toward the dojo entrance the player came
@ in through (the old straight-down route ran him through the map wall).
SennenAct2_Movement_GoonRout:
	face_right
	delay_8
	walk_fast_right
	walk_fast_right
	walk_fast_down
	walk_fast_right
	walk_fast_down
	walk_fast_right
	walk_fast_down
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	walk_fast_right
	step_end

@ Blown straight backwards -- north off the cliff and into the sea.
SennenAct2_Movement_KnockedIntoSea:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	face_down
	step_end

SennenAct2_Movement_RyokoExclaim:
	emote_exclamation_mark
	delay_16
	face_down
	step_end

@ (19,20) -> (17,28): straight down the column, then cuts in to face the
@ player at arm's length.
@ --------------------------------------------------------------------------
@  The captains
@ --------------------------------------------------------------------------
@ ============================================================================
@  GENERAL EDWARDS -- the airshow ambush.
@  The gate corridor (x21-22, y5-12) forces the player over the trigger row
@  (y10, both columns; VAR_TEMP_2 arms it, on_transition kills it once he is
@  defeated). Beats: player startles -> camera pans to the roofline -> the
@  WOT_JET screams in from the right (elevation 4 = OAM priority 1, so it
@  draws OVER the station roof's layer-0 top tiles -- the bridge trick) ->
@  Edwards bails out (elevation-4 FALLER actor, visible the whole drop) ->
@  lands as the crouched WOT_EDWARDS_IMPACT frame, screen shakes -> camera
@  home -> boss card slams (battle theme starts THERE via the primed
@  boss-intro path) -> he rises as his normal sprite, walks up, one line,
@  battle. Losing leaves the trainer flag unset, so the whole flyby replays
@  on the rematch approach.
@ ============================================================================
SennenAct2_EventScript_EdwardsAmbush::
	lockall
	setvar VAR_TEMP_2, 1
	@ Kill the downpour's ambient loop + claps: it hogs the sound channels
	@ and mangled every scene SE and the card. Weather visuals continue; the
	@ storm audio auto-returns on the next map load / battle return.
	special WotSuppressWeatherSE
	applymovement OBJ_EVENT_ID_PLAYER, SennenAct2_Movement_PlayerNoticeJet
	waitmovement 0
	special SpawnCameraObject
	applymovement LOCALID_CAMERA, SennenAct2_Movement_CameraUpToSky
	waitmovement 0
	@ the flyby
	clearflag FLAG_WOT_SA2_AIRSHOW
	addobject LOCALID_SA2_JET
	playse SE_M_RAZOR_WIND
	applymovement LOCALID_SA2_JET, SennenAct2_Movement_JetIn
	waitmovement 0
	@ Edwards bails out; the jet does not even slow down. The fall whistle
	@ leads the drop by a beat and rings through it (the landing boom is what
	@ cuts it off -- naturally).
	applymovement LOCALID_SA2_JET, SennenAct2_Movement_JetOut
	clearflag FLAG_WOT_SA2_FALLER
	addobject LOCALID_SA2_FALLER
	playse SE_FALL
	delay 8
	applymovement LOCALID_SA2_FALLER, SennenAct2_Movement_EdwardsFall
	waitmovement 0
	@ the superhero landing: the deep looping quake rumble (the sound Joe
	@ liked) under a massive shake -- and the boss theme DETONATES off the
	@ impact while he is still crouched
	setflag FLAG_WOT_SA2_FALLER
	removeobject LOCALID_SA2_FALLER
	addobject LOCALID_SA2_IMPACT
	playse SE_M_EARTHQUAKE
	setvar VAR_0x8004, 9  @ vertical pan (MASSIVE)
	setvar VAR_0x8005, 3  @ horizontal pan
	setvar VAR_0x8006, 10 @ num shakes
	setvar VAR_0x8007, 3  @ shake delay
	special WotShakeCameraSilent  @ ShakeCamera plays its own thud SE -- it was eating the rumble
	waitstate
	setvar VAR_0x8004, MUS_DP_VS_UXIE_MESPRIT_AZELF
	special WotPrimeBossTheme
	delay 35
	@ rumble out on cue (SE_M_EARTHQUAKE loops forever otherwise); theme carries
	special WotStopSceneSE
	applymovement LOCALID_CAMERA, SennenAct2_Movement_CameraBackDown
	waitmovement 0
	special RemoveCameraObject
	setflag FLAG_WOT_SA2_AIRSHOW
	removeobject LOCALID_SA2_JET
	@ up from the crouch, straight at you -- theme blazing
	clearflag FLAG_WOT_SA2_EDWARDS_AWAY
	addobject LOCALID_SA2_EDWARDS
	setobjectxy LOCALID_SA2_EDWARDS, 21, 7
	removeobject LOCALID_SA2_IMPACT
	applymovement LOCALID_SA2_EDWARDS, SennenAct2_Movement_EdwardsAdvance
	waitmovement 0
	setspeaker SP_NAME_EDWARDS
	msgbox SennenAct2_Text_KannonIntro, MSGBOX_DEFAULT
	closemessage
	@ card LAST (Joe's revised order): visual + slam only -- primed means it
	@ neither fades nor restarts the already-running theme, just ducks it
	@ under the slam whoosh. Belt-and-braces: re-kill storm audio + flush all
	@ SE players so the shimmer rings over a clean bed.
	special WotSuppressWeatherSE
	special WotStopSceneSE
	bossintro BOSS_INTRO_EDWARDS
	trainerbattle_no_intro TRAINER_SENNEN_CAPTAIN_KANNON, SennenAct2_Text_KannonDefeat
	setspeaker SP_NAME_EDWARDS
	msgbox SennenAct2_Text_KannonPostBattle, MSGBOX_DEFAULT
	releaseall
	end

SennenAct2_Movement_PlayerNoticeJet:
	face_up
	emote_exclamation_mark
	step_end

SennenAct2_Movement_CameraUpToSky:
	walk_up
	walk_up
	walk_up
	walk_up
	step_end

SennenAct2_Movement_JetIn:
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	step_end

SennenAct2_Movement_JetOut:
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	walk_faster_left
	step_end

@ walk_fast, not walk_faster: the drop needs enough air time for the SE_FALL
@ whistle to read (32 frames vs 16).
SennenAct2_Movement_EdwardsFall:
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	step_end

SennenAct2_Movement_CameraBackDown:
	walk_down
	walk_down
	walk_down
	walk_down
	step_end

SennenAct2_Movement_EdwardsAdvance:
	walk_down
	walk_down
	face_player
	step_end

@ The platform gate: one line through the namebox/portrait, then the boss
@ intro card slams, then the battle (no_intro -- the line WAS the intro).
@ Defeated = plain chat, the card never re-fires. (Pre-defeat this script is
@ unreachable -- Edwards is hidden until the ambush above puts him there.)
SennenAct2_EventScript_CaptainKannon::
	goto_if_defeated TRAINER_SENNEN_CAPTAIN_KANNON, SennenAct2_EventScript_EdwardsAfter
	lockall
	setspeaker SP_NAME_EDWARDS
	msgbox SennenAct2_Text_KannonIntro, MSGBOX_DEFAULT
	closemessage
	bossintro BOSS_INTRO_EDWARDS
	trainerbattle_no_intro TRAINER_SENNEN_CAPTAIN_KANNON, SennenAct2_Text_KannonDefeat
	setspeaker SP_NAME_EDWARDS
	msgbox SennenAct2_Text_KannonPostBattle, MSGBOX_DEFAULT
	releaseall
	end

SennenAct2_EventScript_EdwardsAfter::
	lock
	faceplayer
	setspeaker SP_NAME_EDWARDS
	msgbox SennenAct2_Text_KannonPostBattle, MSGBOX_DEFAULT
	release
	end

SennenAct2_EventScript_CaptainRyoko::
	trainerbattle_single TRAINER_SENNEN_CAPTAIN_RYOKO, SennenAct2_Text_RyokoIntro, SennenAct2_Text_RyokoDefeat
	msgbox SennenAct2_Text_RyokoPostBattle, MSGBOX_AUTOCLOSE
	end

@ (The old scripted train door was retired: boarding is now a plain map warp
@  at (19,15), and the keycard gate lives in the Act-2 car itself --
@  ShinkansenInterior_Act2_EventScript_NoKeyEject.)

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

@ The elite whose Mega Alakazam broke Machamp and put Yiffer in the sea.
@ Optional talk battle (TRAINER_LEAF repurposed -- THE POOL IS NOW EMPTY).
SennenAct2_EventScript_GoonFighter1::
	trainerbattle_single TRAINER_LEAF, SennenAct2_Text_VossIntro, SennenAct2_Text_VossDefeat
	setspeaker SP_NAME_GOON
	msgbox SennenAct2_Text_VossPostBattle, MSGBOX_AUTOCLOSE
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
	.string "STAND DOWN.\p"
	.string "Hands where I can see them.\p"
	.string "That is a direct order!\n"
	.string "Use of deadly force is authorized.$"

SennenAct2_Text_KannonDefeat::
	.string "Impressive, Private...$"

SennenAct2_Text_KannonPostBattle::
	.string "You weren't worth the ammunition\n"
	.string "anyway.$"

SennenAct2_Text_RyokoIntro::
	.string "Hm. You watched your friends\n"
	.string "die today.\p"
	.string "The Oni wants you kneeling.$"

SennenAct2_Text_RyokoDefeat::
	.string "Soiled... it...$"

SennenAct2_Text_RyokoPostBattle::
	.string "Go, then. Board.\p"
	.string "You'll never survive the\n"
	.string "TOWER OF SHIN-TOKYO.$"

SennenAct2_Text_RebelFighter1::
	.string "Two more! Come ON, big\n"
	.string "guy, stay up, stay UP--\p"
	.string "{PLAYER} Get to\n"
	.string "The DOOR! GO!$"

SennenAct2_Text_RebelFighter2::
	.string "Third wave. There's always\n"
	.string "a third wave, isn't there.$"

SennenAct2_Text_RebelFighter3::
	.string "Board that train, kid. Make it\n"
	.string "worth EVERYTHING.$"

SennenAct2_Text_VossIntro::
	.string "The karate man? Hmph.\n"
	.string "Just a minor casualty.\p"
	.string "You'll manage.\p"
	.string "What… you want VENGEANCE?\n"
	.string "Adorable. Come collect it.$"

SennenAct2_Text_VossDefeat::
	.string "Tch. A statistical\n"
	.string "anomaly.$"

SennenAct2_Text_VossPostBattle::
	.string "Vengeance won't buy your\n"
	.string "friend a pulse.$"

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
	.string "CHANDELURE light them up!$"

SennenAct2_Text_GoonRout::
	.string "The station belongs to\n"
	.string "MUTRID-- agh! Fall back!$"

SennenAct2_Text_YifferBattleCry::
	.string "MACHAMP doesn't kneel to THE ONI!\n"
	.string "And neither will I!$"

SennenAct2_Text_RedFatalityBattleCry::
	.string "RAICHU, one more! Everything\n"
	.string "you've got left!$"

SennenAct2_Text_RyokoChallenge::
	.string "Siding with that old MASTER GEN?\n"
	.string "His teachings are expired.\p"
	.string "It's useless listening to elders.\n"
	.string "Lemme teach you a thing or two.$"

SennenAct2_Text_GoonFighter3::
	.string "The GENERAL holds the\n"
	.string "platform. NOBODY reaches that\l"
	.string "train.\p"
	.string "...Right? Somebody tell me I'm\n"
	.string "right.$"
