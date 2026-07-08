@
@ Pokemon Wishes of Tomorrow -- Munen Village (Munen_village_2)
@ Opening cutscene, Mom, Professor Clarkson, ice-block starter selection.
@

Munen_village_2_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Munen_village_2_OnLoad
	map_script MAP_SCRIPT_ON_RESUME, Munen_village_2_OnResume
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Munen_village_2_OnFrame
	.byte 0

@ OnResume (not OnLoad): the "!" sprite must be created after ResumeMap's
@ ResetSpriteData, which OnLoad runs before.
Munen_village_2_OnResume::
	questindicator 7, SIDE_QUEST_ONE_THAT_GOT_AWAY  @ Fisherman's son
	end

@ Runs every time the map loads, before player can see anything.
@ Default state: cutscene-only NPCs (Haru, Professor Clarkson) are hidden.
@ Their respective scripts clearflag + addobject to spawn them when their
@ trigger fires, then setflag + removeobject again when they leave.
Munen_village_2_OnLoad::
	setflag FLAG_HIDE_RIVAL_HARU
	setflag FLAG_HIDE_PROFESSOR
	@ TEMP (testing): give the Town Map so the region map is viewable from the
	@ bag. We use the field Town Map (clean viewer) instead of the PokeNav,
	@ because the PokeNav's city-zoom feature glitches on our custom flyable
	@ towns (which have no city-zoom art). ITEM_TOWN_MAP is a key item, so this
	@ additem is idempotent. Remove when map access is gated properly.
	additem ITEM_TOWN_MAP, 1
	@ Reunited fisherman father: shown only after the Red Gyarados is resolved.
	call_if_set FLAG_SEAFLOOR_GYARADOS_GONE, Munen_village_2_EventScript_ShowFishermanDad
	call_if_unset FLAG_SEAFLOOR_GYARADOS_GONE, Munen_village_2_EventScript_HideFishermanDad
	return

Munen_village_2_EventScript_ShowFishermanDad::
	clearflag FLAG_HIDE_MUNEN_FISHERMAN_DAD
	return

Munen_village_2_EventScript_HideFishermanDad::
	setflag FLAG_HIDE_MUNEN_FISHERMAN_DAD
	return

Munen_village_2_OnFrame:
	map_script_2 VAR_MUNEN_INTRO_STATE, 0, Munen_village_2_EventScript_Intro
	.2byte 0

Munen_village_2_EventScript_Intro::
	lockall
	setvar VAR_MUNEN_INTRO_STATE, 1
	fadescreen FADE_TO_BLACK         @ go dark for narration
	delay 30
	playbgm MUS_RG_LAVENDER, FALSE
	playse SE_PIN
	delay 30
	msgbox MunenText_Intro1, MSGBOX_DEFAULT
	closemessage
	msgbox MunenText_Intro2, MSGBOX_DEFAULT
	closemessage
	delay 20
	msgbox MunenText_Intro3, MSGBOX_DEFAULT
	closemessage
	msgbox MunenText_Intro4, MSGBOX_DEFAULT
	closemessage
	delay 30
	@ Release the lockall BEFORE the warp so the warp's own LockPlayerFieldControls
	@ takes over cleanly. Otherwise lockall residue blocks player input on the new map
	@ (warps and signs become unresponsive).
	releaseall
	@ Same-map warp triggers pokeemerald's standard warp-exit fade-in for a clean reveal.
	@ VAR_MUNEN_INTRO_STATE is already 1, so the intro won't re-fire.
	warp MAP_MUNEN_VILLAGE_2, 9, 14
	waitstate
	end

Munen_village_2_EventScript_Mom::
	lock
	faceplayer
	setspeaker SP_NAME_MOM
	goto_if_unset FLAG_TALKED_TO_MOM, Munen_village_2_EventScript_MomFirst
	msgbox MunenText_MomAgain, MSGBOX_DEFAULT
	release
	end

Munen_village_2_EventScript_MomFirst::
	msgbox MunenText_MomFirst, MSGBOX_DEFAULT
	setflag FLAG_TALKED_TO_MOM
	release
	end

@ Legacy in-village Clarkson NPC (not currently placed in map.json since the
@ Professor encounter happens at the north exit). Kept here so we can re-add
@ him to map.json later without recreating the script.
Munen_village_2_EventScript_Clarkson::
	lock
	faceplayer
	setspeaker SP_NAME_CLARKSON
	showportrait PORTRAIT_CLARKSON_GENGAR
	goto_if_set FLAG_RECEIVED_STARTER, Munen_village_2_EventScript_ClarksonAfterStarter
	msgbox MunenText_Clarkson, MSGBOX_DEFAULT
	setflag FLAG_MET_HINOKI
	hideportrait
	release
	end

Munen_village_2_EventScript_ClarksonAfterStarter::
	msgbox MunenText_ClarksonAfter, MSGBOX_DEFAULT
	hideportrait
	release
	end

Munen_village_2_EventScript_IceTyrunt::
	lock
	faceplayer
	msgbox MunenText_IceTyrunt, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Munen_village_2_EventScript_StarterRefuse
	setvar VAR_STARTER_CHOICE, 1
	givemon SPECIES_TYRUNT, 5
	bufferspeciesname STR_VAR_1, SPECIES_TYRUNT
	goto Munen_village_2_EventScript_StarterReceived

Munen_village_2_EventScript_IceAmaura::
	lock
	faceplayer
	msgbox MunenText_IceAmaura, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Munen_village_2_EventScript_StarterRefuse
	setvar VAR_STARTER_CHOICE, 2
	givemon SPECIES_AMAURA, 5
	bufferspeciesname STR_VAR_1, SPECIES_AMAURA
	goto Munen_village_2_EventScript_StarterReceived

Munen_village_2_EventScript_IceAnorith::
	lock
	faceplayer
	msgbox MunenText_IceAnorith, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Munen_village_2_EventScript_StarterRefuse
	setvar VAR_STARTER_CHOICE, 3
	givemon SPECIES_ANORITH, 5
	bufferspeciesname STR_VAR_1, SPECIES_ANORITH
	goto Munen_village_2_EventScript_StarterReceived

Munen_village_2_EventScript_StarterRefuse::
	release
	end

@ Visible "received the X!" feedback + flavor text, then chains into the
@ rival battle. Items (Poke Balls / Potions) are handled by the Professor
@ when the player visits the lab later, NOT here.
@ NOTE: LOCALIDs are 1-indexed by position in map.json object_events:
@   1 Mom, 2 IceTyrunt, 3 IceAmaura, 4 IceAnorith, 5 VillageKid,
@   6 OldLady, 7 Fisherman, 8 Caretaker, 9 Haru.
Munen_village_2_EventScript_StarterReceived::
	setflag FLAG_RECEIVED_STARTER
	setflag FLAG_SYS_POKEMON_GET     @ unlocks the POKEMON option in the start menu
	setflag FLAG_SYS_QUEST_MENU_GET  @ unlocks the QUESTS option in the start menu
	playfanfare MUS_OBTAIN_ITEM
	waitfanfare
	msgbox MunenText_GotStarter, MSGBOX_DEFAULT
	msgbox MunenText_StarterChosen, MSGBOX_DEFAULT
	removeobject 2                   @ ice block Tyrunt
	removeobject 3                   @ ice block Amaura
	removeobject 4                   @ ice block Anorith
	@ Chain straight into the rival cutscene -- still under lock.
	goto Munen_village_2_EventScript_HaruIntro
	end

@==============================
@ Signs (bg_events)
@==============================
Munen_village_2_EventScript_JoeHouseSign::
	msgbox MunenText_JoeHouseSign, MSGBOX_SIGN
	end

Munen_village_2_EventScript_MaysHouseSign::
	msgbox MunenText_MaysHouseSign, MSGBOX_SIGN
	end

Munen_village_2_EventScript_LabSign::
	msgbox MunenText_LabSign, MSGBOX_SIGN
	end

Munen_village_2_EventScript_ForeclosureSign::
	msgbox MunenText_ForeclosureSign, MSGBOX_SIGN
	end

Munen_village_2_EventScript_RivalHouseSign::
	msgbox MunenText_RivalHouseSign, MSGBOX_SIGN
	end

@==============================
@ NPCs
@==============================
@ LOCALID assignments (1-indexed by position in map.json object_events):
@   1 Mom
@   2 IceTyrunt
@   3 IceAmaura
@   4 IceAnorith
@   5 VillageKid       (in starter's removeobject range -- intentional: "kid runs off")
@   6 Old Lady         (matches existing removeobject 6 below)
@   7 Fisherman
@   8 Caretaker
@
@ All NPC sprites are vanilla pokeemerald range (< 100) to avoid the FRLG-
@ extended-range crash. Movement types are FACE_* / LOOK_AROUND, not WANDER.

Munen_village_2_EventScript_VillageKid::
	lock
	faceplayer
	msgbox MunenText_VillageKid, MSGBOX_DEFAULT
	release
	end

Munen_village_2_EventScript_OldLady::
	setspeaker SP_NAME_OLD_WOMAN
	lock
	faceplayer
	msgbox MunenText_OldLady, MSGBOX_DEFAULT
	closemessage
	setflag FLAG_OLD_LADY_SPOKE
	removeobject 6
	release
	end

Munen_village_2_EventScript_Fisherman::
	lock
	faceplayer
	goto_if_set FLAG_SEAFLOOR_GYARADOS_GONE, Munen_village_2_EventScript_FishermanReunited
	msgbox MunenText_Fisherman, MSGBOX_DEFAULT
	questmenu QUEST_MENU_CHECK_UNLOCKED, SIDE_QUEST_ONE_THAT_GOT_AWAY
	call_if_eq VAR_RESULT, FALSE, Munen_village_2_EventScript_StartGotAwayQuest
	release
	end

Munen_village_2_EventScript_StartGotAwayQuest::
	startquest SIDE_QUEST_ONE_THAT_GOT_AWAY
	return

Munen_village_2_EventScript_FishermanReunited::
	msgbox Munen_village_2_Text_SonReunited, MSGBOX_DEFAULT
	release
	end

@ ===========================================================================
@ The father, reunited from SeafloorCavern. Recognizes the player and asks
@ for the Red Gyarados. Yes -> trades it for TM Earthquake. No -> keep it.
@ If the player KO'd it (has none), he gives the TM as thanks anyway.
@ ===========================================================================
Munen_village_2_EventScript_FishermanDad::
	lock
	faceplayer
	goto_if_set FLAG_MUNEN_FISHERMAN_REWARDED, Munen_village_2_EventScript_FishermanDadAfter
	msgbox Munen_village_2_Text_DadRecognize, MSGBOX_DEFAULT
	call_if_quest_active SIDE_QUEST_ONE_THAT_GOT_AWAY, Munen_village_2_EventScript_CompleteGotAwayQuest
	specialvar VAR_RESULT, CheckPlayerHasRedGyarados
	goto_if_eq VAR_RESULT, FALSE, Munen_village_2_EventScript_FishermanDadGift
	msgbox Munen_village_2_Text_DadAsk, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Munen_village_2_EventScript_FishermanDadDeclined
	@ YES -> attempt to take the Gyarados
	specialvar VAR_RESULT, TakeRedGyaradosFromPlayer
	goto_if_eq VAR_RESULT, FALSE, Munen_village_2_EventScript_FishermanDadLastMon
	msgbox Munen_village_2_Text_DadThanks, MSGBOX_DEFAULT
	giveitem ITEM_TM_EARTHQUAKE, 1
	setflag FLAG_MUNEN_FISHERMAN_REWARDED
	release
	end

Munen_village_2_EventScript_CompleteGotAwayQuest::
	completequest SIDE_QUEST_ONE_THAT_GOT_AWAY
	return

@ Player declined the trade -- keep the Gyarados, can return later.
Munen_village_2_EventScript_FishermanDadDeclined::
	msgbox Munen_village_2_Text_DadDeclined, MSGBOX_DEFAULT
	release
	end

@ The Red Gyarados is the player's only Pokemon -- refuse the trade.
Munen_village_2_EventScript_FishermanDadLastMon::
	msgbox Munen_village_2_Text_DadLastMon, MSGBOX_DEFAULT
	release
	end

@ Player KO'd it (or no longer has it) -- reward the TM as thanks.
Munen_village_2_EventScript_FishermanDadGift::
	msgbox Munen_village_2_Text_DadGift, MSGBOX_DEFAULT
	giveitem ITEM_TM_EARTHQUAKE, 1
	setflag FLAG_MUNEN_FISHERMAN_REWARDED
	release
	end

Munen_village_2_EventScript_FishermanDadAfter::
	msgbox Munen_village_2_Text_DadAfter, MSGBOX_DEFAULT
	release
	end

Munen_village_2_EventScript_Caretaker::
	lock
	faceplayer
	msgbox MunenText_Caretaker, MSGBOX_DEFAULT
	release
	end

@==============================
@ Professor Clarkson -- north-exit encounter (hidden NPC, spawned by the scene)
@==============================
Munen_village_2_EventScript_ProfessorDummy::
	end

@==============================
@ North-exit handler -- ONE coord trigger per top-edge tile (22..27, y=1).
@ IMPORTANT: the engine only fires a SINGLE coord_event per tile, so the
@ Professor encounter and the progress gate MUST live in the same script.
@ (They used to be two overlapping coord_events sharing these tiles; only one
@ ever fired, which soft-locked the exit.) Check order:
@   1. no starter yet           -> block
@   2. haven't faced the rival  -> block
@   3. starter + rival done, but Professor hasn't handed over supplies
@                               -> run the Professor encounter (sets the flag)
@   4. all three done           -> fall through; player walks out to the warp
@==============================
Munen_village_2_EventScript_ExitNorth::
	goto_if_unset FLAG_RECEIVED_STARTER, Munen_village_2_EventScript_NorthGateBlock
	goto_if_unset FLAG_BATTLED_HARU, Munen_village_2_EventScript_NorthGateBlock
	goto_if_unset FLAG_PROFESSOR_GAVE_ITEMS, Munen_village_2_EventScript_ProfessorScene
	@ All three gates cleared -- just let the player walk north. Munen now CONNECTS
	@ up to MeltingMile (seamless connection), so no warp is needed here anymore.
	end

Munen_village_2_EventScript_NorthGateBlock::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, Munen_village_2_Movement_GatePushBack
	waitmovement 0
	msgbox MunenText_NorthGate, MSGBOX_DEFAULT
	closemessage
	releaseall
	end

Munen_village_2_Movement_GatePushBack:
	walk_down
	step_end

Munen_village_2_EventScript_ProfessorScene::
	lockall
	@ Push the player back south so there's space for the encounter.
	applymovement OBJ_EVENT_ID_PLAYER, Munen_village_2_Movement_PlayerBackUp
	waitmovement 0
	@ Spawn Clarkson at his template position (24, 0) -- the top edge.
	clearflag FLAG_HIDE_PROFESSOR
	addobject 10
	delay 15
	@ Clarkson walks down to meet the player.
	applymovement 10, Munen_village_2_Movement_ProfessorApproach
	waitmovement 0
	setspeaker SP_NAME_CLARKSON
	showportrait PORTRAIT_CLARKSON_GENGAR
	msgbox MunenText_ProfessorHello, MSGBOX_DEFAULT
	closemessage
	@ He stops speaking to hand over supplies: drop both plate and portrait,
	@ then bring them back when he speaks again. giveitem shows its own UI.
	hideportrait
	setspeaker SP_NAME_NONE
	giveitem ITEM_POKE_BALL, 5
	giveitem ITEM_POTION, 2
	setspeaker SP_NAME_CLARKSON
	showportrait PORTRAIT_CLARKSON_GENGAR
	msgbox MunenText_ProfessorAfterItems, MSGBOX_DEFAULT
	closemessage
	hideportrait
	startquest QUEST_WAKING_MOUNTAIN
	@ Clarkson walks back north and exits the camera view.
	applymovement 10, Munen_village_2_Movement_ProfessorLeave
	waitmovement 0
	@ Cleanup: mark complete, re-hide, remove from active list.
	setflag FLAG_PROFESSOR_GAVE_ITEMS
	setflag FLAG_HIDE_PROFESSOR
	removeobject 10
	releaseall
	end

@==============================
@ Rival Haru -- appears after starter is chosen, runs in, talks, leaves.
@==============================
@ Dummy script for the hidden rival -- shouldn't ever fire because the
@ object is invisible until HaruIntro spawns him.
Munen_village_2_EventScript_HaruDummy::
	end

Munen_village_2_EventScript_HaruIntro::
	@ Already under lock from StarterReceived. Spawn the rival.
	@ Haru's template position in map.json is (25, 9) -- a few tiles south
	@ of the ice blocks, comfortably inside the player's visible camera
	@ window so he renders as soon as addobject runs.
	clearflag FLAG_HIDE_RIVAL_HARU
	addobject 9
	setflag FLAG_MET_RIVAL_HARU
	@ Mark the rival as "faced" up front so LOSING the battle (which whites the
	@ player out before EventScript_HaruAfterBattle can run) doesn't permanently
	@ block the north exit. Win or lose, this counts as having battled the rival.
	setflag FLAG_BATTLED_HARU
	delay 20
	@ "Hey wait!" shout (still visible at spawn, sets the moment up)
	setspeaker SP_NAME_DRACO
	msgbox MunenText_HaruShout, MSGBOX_DEFAULT
	closemessage
	@ Haru runs up to the player (4 fast steps north from (25,9) to (25,5)).
	applymovement 9, Munen_village_2_Movement_HaruApproach
	waitmovement 0
	@ Player turns to face Haru (south).
	applymovement OBJ_EVENT_ID_PLAYER, Munen_village_2_Movement_PlayerFaceDown
	waitmovement 0
	setspeaker SP_NAME_DRACO
	msgbox MunenText_HaruIntro, MSGBOX_DEFAULT
	closemessage
	@ Keep his plate red through the battle-start / defeat text too.
	setspeaker SP_NAME_DRACO
	@ Branch into the correct counter-pick battle based on player's starter.
	compare VAR_STARTER_CHOICE, 1
	goto_if_eq Munen_village_2_EventScript_HaruBattleVsTyrunt
	compare VAR_STARTER_CHOICE, 2
	goto_if_eq Munen_village_2_EventScript_HaruBattleVsAmaura
	compare VAR_STARTER_CHOICE, 3
	goto_if_eq Munen_village_2_EventScript_HaruBattleVsAnorith
	@ Fallback (shouldn't be reachable) -- skip battle, go to farewell.
	goto Munen_village_2_EventScript_HaruAfterBattle

@ Player chose Tyrunt -> Haru fights with Amaura.
Munen_village_2_EventScript_HaruBattleVsTyrunt::
	trainerbattle_single TRAINER_HARU_HAS_AMAURA, MunenText_HaruBattleStart, MunenText_HaruDefeated, Munen_village_2_EventScript_HaruAfterBattle
	end

@ Player chose Amaura -> Haru fights with Anorith.
Munen_village_2_EventScript_HaruBattleVsAmaura::
	trainerbattle_single TRAINER_HARU_HAS_ANORITH, MunenText_HaruBattleStart, MunenText_HaruDefeated, Munen_village_2_EventScript_HaruAfterBattle
	end

@ Player chose Anorith -> Haru fights with Tyrunt.
Munen_village_2_EventScript_HaruBattleVsAnorith::
	trainerbattle_single TRAINER_HARU_HAS_TYRUNT, MunenText_HaruBattleStart, MunenText_HaruDefeated, Munen_village_2_EventScript_HaruAfterBattle
	end

@ Shared post-battle: farewell + walk off + cleanup.
@ Runs whether Haru beat us in trainerbattle or not (well, mostly the latter;
@ on player loss the game whites out before this script ever resumes).
Munen_village_2_EventScript_HaruAfterBattle::
	lockall
	setspeaker SP_NAME_DRACO
	msgbox MunenText_HaruFarewell, MSGBOX_DEFAULT
	closemessage
	@ Haru walks back the way he came, then off-stage.
	applymovement 9, Munen_village_2_Movement_HaruLeave
	waitmovement 0
	@ Mark him battled and re-hide; on next map load he stays gone.
	setflag FLAG_BATTLED_HARU
	setflag FLAG_HIDE_RIVAL_HARU
	removeobject 9
	releaseall
	end

@==============================
@ Movement sequences
@==============================
@ Haru runs north from (25,8) to (25,5) -- three fast steps.
@ Player typically stands at (X, 4) facing the ice block at (X, 3), so Haru
@ ending at (25, 5) puts him just south of the player for face-to-face dialogue.
@ walk_fast_* is the standard "running" speed used for rivals in pokeemerald.
Munen_village_2_Movement_HaruApproach:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	face_up
	step_end

@ Haru runs back south off-stage after the battle, returning the way he came
@ and continuing a few extra tiles so he exits the camera view cleanly.
Munen_village_2_Movement_HaruLeave:
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	step_end

Munen_village_2_Movement_PlayerFaceDown:
	face_down
	step_end

@ Professor Clarkson north-exit encounter movements.
@ Trigger fires when player walks onto (22..26, 1). After the script pushes
@ player back to y=3, Clarkson spawns at (24, 0) and walks down to (24, 2).
Munen_village_2_Movement_PlayerBackUp:
	walk_down
	walk_down
	face_up
	step_end

Munen_village_2_Movement_ProfessorApproach:
	walk_down
	walk_down
	face_down
	step_end

Munen_village_2_Movement_ProfessorLeave:
	walk_up
	walk_up
	step_end

MunenText_Intro1:
	.string "Mt. Munen.\nOne thousand years ago.$"

MunenText_Intro2:
	.string "They say a star fell upon this mountain.\pThey say it granted a wish.\pThey say it ended the world.$"

MunenText_Intro3:
	.string "Those who survived sealed it away.\pThey sealed the star.\nThey sealed what came with it.\pThey called it the Undying Wish...\pAnd they prayed it would never wake again.$"

MunenText_Intro4:
	.string "It has been sleeping for one thousand\nyears.\pToday, something disturbs the seal.$"

MunenText_MomFirst:
	.string "Good morning, sleepyhead.\pProfessor Clarkson sent word. \nHe wants to see you at his lab.\pMind the cold. The mist's come down\nearly.$"

MunenText_MomAgain:
	.string "The Professor's waiting at his\nlab, dear. Wrap up warm.$"

MunenText_Clarkson:
	.string "There you are.\pI study what this mountain remembers --\nits old ice, its stories.\pDeep in the frozen lake I found three\nof them. Sleeping in the ice.\pI revived them. I shouldn't have.\pGo on -- choose one.\nLet it choose you back.$"

MunenText_ClarksonAfter:
	.string "It already trusts you. Good.\pYou're holding part of an answer\nthis mountain has kept for a long time.$"

MunenText_IceTyrunt:
	.string "Inside the ice, something stirs.\pIt regards you as if it already\nrules here.\pTake this one?$"

MunenText_IceAmaura:
	.string "Inside the ice, something stirs.\pThe cold of centuries still clings\nto it.\pTake this one?$"

MunenText_IceAnorith:
	.string "Inside the ice, something stirs.\pIt twitches, alert, pure\nsurvival instinct.\pTake this one?$"

MunenText_StarterChosen:
	.string "The ice cracks.\pIt breathes for the first time in a\nthousand years and chooses to\lbreathe beside you.$"

MunenText_GotStarter:
	.string "{PLAYER} received the {STR_VAR_1}!$"

MunenText_OldLady:
	.string "The snow on the mountain\nis melting, child.\pThis snow should never melt. Never.\pDeath is coming to Munen.\pI can feel it in the wind.$"

MunenText_VillageKid:
	.string "Did you hear? Professor\nClarkson found something at the \ltop of the mountain!\pWhat could it be?\pI wanna know but my dad says to stay \nclose to home. No fair!$"

MunenText_Fisherman:
	.string "My dad went out to ice-fish\non Lake Munen days ago...\pHe still hasn't come home.\pThe ice is thin now and half the lake's\nopen water already. It isn't safe.\pI keep watching the far shore, but\nI can't reach it from here.\pIf you ever find a way across that\nwater... please. Look for him.$"

MunenText_Caretaker:
	.string "The festival is cancelled this year.\pThe Professor's orders. He says the\nmountain isn't safe right now.\pFirst time we've missed it \nsince I was born.$"

@ Rival Haru dialogue
MunenText_HaruShout:
	.string "???: HEY! Wait up!$"

MunenText_HaruIntro:
	.string "So that's where they went.\pPROFESSOR CLARKSON said he'd left\nsomething for me too! I'm not letting\lyou walk off with all of it.\pI'm taking what's left.\pConsider this a warning,\n{PLAYER}.$"

MunenText_HaruFarewell:
	.string "Next time we meet --\nwe're settling this.\pSee you on the road.$"

@ Professor Clarkson north-exit encounter
MunenText_ProfessorHello:
	.string "Hold up, {PLAYER}!\pYou can't go wandering off the mountain\nempty-handed.\pHere -- I came to give you these\nbefore you set out.$"

MunenText_ProfessorAfterItems:
	.string "The Poke Balls are for catching\nwild Pokemon -- you'll find a few along\lthe northern trail.\pThe Potions are for when your partner\nneeds patching up.\pNow then. Be careful out there.\pAnd write to your mother.$"

@ North-exit gate text (shown until starter + rival + professor are done)
MunenText_NorthGate:
	.string "It's not safe to head up the mountain\nyet.\pProfessor Clarkson asked to see you\nbefore you set out -- and you'll want\la partner at your side first.$"

@ Rival Haru in-battle text
MunenText_HaruBattleStart:
	.string "Let's see what that thing\ncan do!$"

MunenText_HaruDefeated:
	.string "...Heh. Not bad.\pI'll be better next time.$"

@==============================
@ Sign text
@==============================
MunenText_JoeHouseSign:
	.string "{PLAYER}'S HOUSE$"

MunenText_MaysHouseSign:
	.string "DRACO'S HOUSE$"

MunenText_LabSign:
	.string "PROFESSOR CLARKSON'S LABORATORY\pIf you can read this from outside,\nhe's probably busy with research.$"

MunenText_ForeclosureSign:
	.string "FORECLOSURE NOTICE\pThis property has been seized.\pThe family who lived here left suddenly.\pNo one in Munen will say where.$"

MunenText_RivalHouseSign:
	.string "THE NUT DUNGEON$"

@ --- Reunited fisherman (father + son) voice lines ---
Munen_village_2_Text_SonReunited:
	.string "My old man's finally home!\pHe came back babbling about a RED\nGYARADOS and the trainer who faced it.\pI thought he'd lost it in that cave…\nguess he was telling the truth.\pThank you for bringing him back to us.$"

Munen_village_2_Text_DadRecognize:
	.string "Wait… THERE he is!\pSon, that's the guy! That's the one\nwho took on the RED GYARADOS!\pI'd know that face anywhere. I haven't\nstopped shaking since that cave.$"

Munen_village_2_Text_DadAsk:
	.string "So it's true… you really landed it.\pListen… I know it's a lot to ask,\nbut that GYARADOS was my life's dream.\pWould you… would you be willing to\ngive it to me?$"

Munen_village_2_Text_DadDeclined:
	.string "Heh… figured I'd ask.\pThat's an incredible reward.\nYou deserve to keep it.$"

Munen_village_2_Text_DadThanks:
	.string "You… you'd really part with it?\pI don't have much, but a fisherman\nalways keeps something for the big one.\pPlease - take this. I want you to have it.$"

Munen_village_2_Text_DadGift:
	.string "You faced it down all the same…\nthat's more guts than I'll ever have.\pA catch like that wasn't meant for a\ncoward like me. But you earned this.\pTake it. I insist.$"

Munen_village_2_Text_DadLastMon:
	.string "Hold on now - that's your only\nPOKéMON! I can't take that from you.\pHang onto it. Come find me if you\never change your mind.$"

Munen_village_2_Text_DadAfter:
	.string "Every morning I watch the water and\nthink about that day.\pThank you, {PLAYER}. Truly.$"
