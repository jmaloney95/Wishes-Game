SennenAct2_MapScripts::
	.byte 0

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

SennenAct2_Text_GoonFighter3::
	.string "GOON: The captains hold the\n"
	.string "platform. NOBODY reaches that\l"
	.string "train.\p"
	.string "...Right? Somebody tell me I'm\n"
	.string "right.$"
