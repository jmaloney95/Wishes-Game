TradewindTown_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, TradewindTown_OnTransition
	.byte 0

@ Whiteout respawn: Tradewind has no Pokemon Center, so without this a faint
@ here sent the player all the way back to a pre-Star-Summit Act 1 spawn. Making
@ the town its own heal location keeps the playthrough local.
TradewindTown_OnTransition::
	setrespawn HEAL_LOCATION_TRADEWIND_TOWN
	end

@ ============================================================================
@  B1 - Curio Merchant "Curios of the Crossing"
@  One impossible thing at a time: the stall offers curio #N of 7
@  (VAR_TRADEWIND_CURIO_STOCK) at 1,000 each. Buying one unlocks the next;
@  each can only ever be bought once. At 7, the stall is sold out for good.
@ ============================================================================
Tradewind_EventScript_CurioMerchant::
	lock
	faceplayer
	goto_if_eq VAR_TRADEWIND_CURIO_STOCK, 7, Tradewind_EventScript_CurioSoldOut
	msgbox Tradewind_Text_CurioGreeting, MSGBOX_DEFAULT
	switch VAR_TRADEWIND_CURIO_STOCK
	case 0, Tradewind_EventScript_CurioOffer0
	case 1, Tradewind_EventScript_CurioOffer1
	case 2, Tradewind_EventScript_CurioOffer2
	case 3, Tradewind_EventScript_CurioOffer3
	case 4, Tradewind_EventScript_CurioOffer4
	case 5, Tradewind_EventScript_CurioOffer5
	case 6, Tradewind_EventScript_CurioOffer6
	release
	end

Tradewind_EventScript_CurioOffer0::
	setorcopyvar VAR_0x8008, ITEM_BOTTLED_WARMTH
	goto Tradewind_EventScript_CurioOffer
Tradewind_EventScript_CurioOffer1::
	setorcopyvar VAR_0x8008, ITEM_BORROWED_YEARS
	goto Tradewind_EventScript_CurioOffer
Tradewind_EventScript_CurioOffer2::
	setorcopyvar VAR_0x8008, ITEM_UNSPOKEN_LAST_WORDS
	goto Tradewind_EventScript_CurioOffer
Tradewind_EventScript_CurioOffer3::
	setorcopyvar VAR_0x8008, ITEM_A_STRANGERS_NAME
	goto Tradewind_EventScript_CurioOffer
Tradewind_EventScript_CurioOffer4::
	setorcopyvar VAR_0x8008, ITEM_FOLDED_GOODBYE
	goto Tradewind_EventScript_CurioOffer
Tradewind_EventScript_CurioOffer5::
	setorcopyvar VAR_0x8008, ITEM_ONE_GOOD_DREAM
	goto Tradewind_EventScript_CurioOffer
Tradewind_EventScript_CurioOffer6::
	setorcopyvar VAR_0x8008, ITEM_THE_LONG_WAY_HOME
	goto Tradewind_EventScript_CurioOffer

Tradewind_EventScript_CurioOffer::
	bufferitemname STR_VAR_1, VAR_0x8008
	showmoneybox 0, 0
	msgbox Tradewind_Text_CurioOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Tradewind_EventScript_CurioDecline
	checkmoney 1000
	goto_if_eq VAR_RESULT, FALSE, Tradewind_EventScript_CurioNoMoney
	giveitem VAR_0x8008
	goto_if_eq VAR_RESULT, FALSE, Tradewind_EventScript_CurioNoRoom
	removemoney 1000
	updatemoneybox
	addvar VAR_TRADEWIND_CURIO_STOCK, 1
	msgbox Tradewind_Text_CurioBought, MSGBOX_DEFAULT
	hidemoneybox
	release
	end

Tradewind_EventScript_CurioDecline::
	hidemoneybox
	msgbox Tradewind_Text_CurioFarewell, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_CurioNoMoney::
	hidemoneybox
	msgbox Tradewind_Text_CurioNoMoney, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_CurioNoRoom::
	hidemoneybox
	msgbox Tradewind_Text_CurioNoRoom, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_CurioSoldOut::
	msgbox Tradewind_Text_CurioSoldOut, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  B2 - The Pawnbroker of Bonds (Mega-stone exchange)
@  Hidden until the Lantern Badge. Transmutes a curio into its Mega Stone.
@ ============================================================================
Tradewind_EventScript_Pawnbroker::
	lock
	faceplayer
	goto_if_unset FLAG_BADGE_LANTERN, Tradewind_EventScript_PawnbrokerClosed
	msgbox Tradewind_Text_PawnbrokerOffer, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Tradewind_EventScript_PawnDone
	checkitem ITEM_BOTTLED_WARMTH
	goto_if_eq VAR_RESULT, TRUE, Tradewind_EventScript_PawnBottledWarmth
	checkitem ITEM_BORROWED_YEARS
	goto_if_eq VAR_RESULT, TRUE, Tradewind_EventScript_PawnBorrowedYears
	checkitem ITEM_UNSPOKEN_LAST_WORDS
	goto_if_eq VAR_RESULT, TRUE, Tradewind_EventScript_PawnUnspoken
	checkitem ITEM_A_STRANGERS_NAME
	goto_if_eq VAR_RESULT, TRUE, Tradewind_EventScript_PawnStranger
	checkitem ITEM_FOLDED_GOODBYE
	goto_if_eq VAR_RESULT, TRUE, Tradewind_EventScript_PawnFolded
	checkitem ITEM_ONE_GOOD_DREAM
	goto_if_eq VAR_RESULT, TRUE, Tradewind_EventScript_PawnDream
	checkitem ITEM_THE_LONG_WAY_HOME
	goto_if_eq VAR_RESULT, TRUE, Tradewind_EventScript_PawnLongWay
	msgbox Tradewind_Text_PawnNoCurio, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_PawnBottledWarmth::
	removeitem ITEM_BOTTLED_WARMTH
	giveitem ITEM_HOUNDOOMINITE
	goto Tradewind_EventScript_PawnRevealed
Tradewind_EventScript_PawnBorrowedYears::
	removeitem ITEM_BORROWED_YEARS
	giveitem ITEM_BANETTITE
	goto Tradewind_EventScript_PawnRevealed
Tradewind_EventScript_PawnUnspoken::
	removeitem ITEM_UNSPOKEN_LAST_WORDS
	giveitem ITEM_SABLENITE
	goto Tradewind_EventScript_PawnRevealed
Tradewind_EventScript_PawnStranger::
	removeitem ITEM_A_STRANGERS_NAME
	giveitem ITEM_ABSOLITE
	goto Tradewind_EventScript_PawnRevealed
Tradewind_EventScript_PawnFolded::
	removeitem ITEM_FOLDED_GOODBYE
	giveitem ITEM_GARDEVOIRITE
	goto Tradewind_EventScript_PawnRevealed
Tradewind_EventScript_PawnDream::
	removeitem ITEM_ONE_GOOD_DREAM
	giveitem ITEM_MAWILITE
	goto Tradewind_EventScript_PawnRevealed
Tradewind_EventScript_PawnLongWay::
	removeitem ITEM_THE_LONG_WAY_HOME
	giveitem ITEM_BLAZIKENITE
	goto Tradewind_EventScript_PawnRevealed

Tradewind_EventScript_PawnRevealed::
	msgbox Tradewind_Text_StoneRevealed, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_PawnbrokerClosed::
	msgbox Tradewind_Text_PawnbrokerClosed, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_PawnDone::
	msgbox Tradewind_Text_PawnFarewell, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  G - The Lantern Gym (open-air, optional)
@  Three gym trainers, then Madam Tsuji. Victory = Lantern Badge + Mega Ring
@  + Gengarite, and the Pawnbroker stall opens (curio -> Mega Stone trade).
@ ============================================================================
Tradewind_EventScript_GymGoro::
	lock
	faceplayer
	goto_if_defeated TRAINER_TRADEWIND_GORO, Tradewind_EventScript_GoroDefeated
	trainerbattle_no_intro TRAINER_TRADEWIND_GORO, Tradewind_Text_GoroDefeat
	msgbox Tradewind_Text_GoroPostBattle, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_GoroDefeated::
	msgbox Tradewind_Text_GoroPostBattle, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_GymLanternBearer::
	lock
	faceplayer
	goto_if_defeated TRAINER_TRADEWIND_LANTERN_BEARER, Tradewind_EventScript_LanternBearerDefeated
	trainerbattle_no_intro TRAINER_TRADEWIND_LANTERN_BEARER, Tradewind_Text_LanternBearerDefeat
	msgbox Tradewind_Text_LanternBearerPostBattle, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_LanternBearerDefeated::
	msgbox Tradewind_Text_LanternBearerPostBattle, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_GymApprentice::
	lock
	faceplayer
	goto_if_defeated TRAINER_TSUJI_APPRENTICE, Tradewind_EventScript_ApprenticeDefeated
	trainerbattle_no_intro TRAINER_TSUJI_APPRENTICE, Tradewind_Text_ApprenticeDefeat
	msgbox Tradewind_Text_ApprenticePostBattle, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_ApprenticeDefeated::
	msgbox Tradewind_Text_ApprenticePostBattle, MSGBOX_DEFAULT
	release
	end

@ --- Madam Tsuji -------------------------------------------------------------
Tradewind_EventScript_MadamTsuji::
	lock
	faceplayer
	goto_if_set FLAG_BADGE_LANTERN, Tradewind_EventScript_TsujiPostBadge
	goto_if_not_defeated TRAINER_TRADEWIND_GORO, Tradewind_EventScript_TsujiNotReady
	goto_if_not_defeated TRAINER_TRADEWIND_LANTERN_BEARER, Tradewind_EventScript_TsujiNotReady
	goto_if_not_defeated TRAINER_TSUJI_APPRENTICE, Tradewind_EventScript_TsujiNotReady
	setspeaker SP_NAME_MADAM_TSUJI
	msgbox Tradewind_Text_TsujiMegaIntro, MSGBOX_DEFAULT
	trainerbattle_no_intro TRAINER_MADAM_TSUJI, Tradewind_Text_TsujiDefeat
	setspeaker SP_NAME_MADAM_TSUJI
	msgbox Tradewind_Text_TsujiBadgeSpeech, MSGBOX_DEFAULT
	setflag FLAG_BADGE_LANTERN
	setspeaker SP_NAME_NONE
	giveitem ITEM_MEGA_RING
	giveitem ITEM_GENGARITE
	clearflag FLAG_HIDE_TRADEWIND_PAWNBROKER
	setspeaker SP_NAME_MADAM_TSUJI
	msgbox Tradewind_Text_TsujiPawnbrokerHint, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_TsujiNotReady::
	setspeaker SP_NAME_MADAM_TSUJI
	msgbox Tradewind_Text_TsujiNotReady, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_TsujiPostBadge::
	setspeaker SP_NAME_MADAM_TSUJI
	msgbox Tradewind_Text_TsujiPostBadge, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  A - Ambiance spirits (dialogue-only veil-market flavor)
@ ============================================================================
Tradewind_EventScript_DontEatHere::
	lock
	faceplayer
	msgbox Tradewind_Text_DontEatHere, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_FaeTouchedHint::
	lock
	faceplayer
	msgbox Tradewind_Text_FaeTouchedHint, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_AfterlifeRealtor::
	lock
	faceplayer
	msgbox Tradewind_Text_AfterlifeRealtor, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_SpiritShopper1::
	lock
	faceplayer
	msgbox Tradewind_Text_SpiritShopper1, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_SpiritShopper2::
	lock
	faceplayer
	msgbox Tradewind_Text_SpiritShopper2, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_MaskedMerchant::
	lock
	faceplayer
	msgbox Tradewind_Text_MaskedMerchant, MSGBOX_DEFAULT
	release
	end

@ Locked doors (bg_event signs on the shut storefronts)
Tradewind_EventScript_LockedDoor::
	msgbox Tradewind_Text_LockedDoor, MSGBOX_SIGN
	end

@ Foreshadows Tsuji's Mega Ring + Gengarite reward
Tradewind_EventScript_MegaHint::
	lock
	faceplayer
	msgbox Tradewind_Text_MegaHint, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  Ghosts of the night market -- wild spirits drifting the stalls and the
@  black mirror-water on the town's east edge, plus two human(?) residents.
@ ============================================================================
Tradewind_EventScript_GhostDrifloon::
	lock
	faceplayer
	playmoncry SPECIES_DRIFLOON, CRY_MODE_NORMAL
	waitmoncry
	msgbox Tradewind_Text_GhostDrifloon, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_GhostLampent::
	lock
	faceplayer
	playmoncry SPECIES_LAMPENT, CRY_MODE_NORMAL
	waitmoncry
	msgbox Tradewind_Text_GhostLampent, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_GhostGastly::
	lock
	faceplayer
	playmoncry SPECIES_GASTLY, CRY_MODE_NORMAL
	waitmoncry
	msgbox Tradewind_Text_GhostGastly, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_GhostDuskull::
	lock
	faceplayer
	playmoncry SPECIES_DUSKULL, CRY_MODE_NORMAL
	waitmoncry
	msgbox Tradewind_Text_GhostDuskull, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_GhostLitwick::
	lock
	faceplayer
	playmoncry SPECIES_LITWICK, CRY_MODE_NORMAL
	waitmoncry
	msgbox Tradewind_Text_GhostLitwick, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_GhostShuppet::
	lock
	faceplayer
	playmoncry SPECIES_SHUPPET, CRY_MODE_NORMAL
	waitmoncry
	msgbox Tradewind_Text_GhostShuppet, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_GhostMisdreavus::
	lock
	faceplayer
	playmoncry SPECIES_MISDREAVUS, CRY_MODE_NORMAL
	waitmoncry
	msgbox Tradewind_Text_GhostMisdreavus, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_GhostChanneler::
	lock
	faceplayer
	msgbox Tradewind_Text_GhostChanneler, MSGBOX_DEFAULT
	release
	end

Tradewind_EventScript_GhostDancer::
	lock
	faceplayer
	msgbox Tradewind_Text_GhostDancer, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  Street trainers (eerie night-market, <=Lv20)
@ ============================================================================
Tradewind_EventScript_TrainerSable::
	trainerbattle_single TRAINER_TRADEWIND_SABLE, Tradewind_Text_SableIntro, Tradewind_Text_SableDefeat
	msgbox Tradewind_Text_SablePostBattle, MSGBOX_AUTOCLOSE
	end

Tradewind_EventScript_TrainerRoshan::
	trainerbattle_single TRAINER_TRADEWIND_ROSHAN, Tradewind_Text_RoshanIntro, Tradewind_Text_RoshanDefeat
	msgbox Tradewind_Text_RoshanPostBattle, MSGBOX_AUTOCLOSE
	end

Tradewind_EventScript_TrainerPiper::
	trainerbattle_single TRAINER_TRADEWIND_PIPER, Tradewind_Text_PiperIntro, Tradewind_Text_PiperDefeat
	msgbox Tradewind_Text_PiperPostBattle, MSGBOX_AUTOCLOSE
	end

@ ============================================================================
@  Text
@ ============================================================================
Tradewind_Text_CurioGreeting::
	.string "Curios of the Crossing.\p"
	.string "The cloth holds one impossible\n"
	.string "thing at a time. Each finds only\l"
	.string "one keeper. Ever.$"

Tradewind_Text_CurioOffer::
	.string "Today the cloth offers...\n"
	.string "{STR_VAR_1}.\p"
	.string "A thousand coin. One only.\n"
	.string "Will you keep it?$"

Tradewind_Text_CurioBought::
	.string "It is yours to keep now.\p"
	.string "Come back tomorrow-minded.\n"
	.string "The cloth will hold something\l"
	.string "new.$"

Tradewind_Text_CurioNoMoney::
	.string "The cloth does not haggle.\n"
	.string "A thousand coin, or it waits.$"

Tradewind_Text_CurioNoRoom::
	.string "Your bag is full.\p"
	.string "Impossible things still take up\n"
	.string "room. Make some, then return.$"

@ PLACEHOLDER (Joe): sold-out line once all seven curios are gone.
Tradewind_Text_CurioSoldOut::
	.string "The cloth lies bare.\p"
	.string "Seven impossible things, seven\n"
	.string "keepers. The crossing has nothing\l"
	.string "left to sell you...\p"
	.string "...except the advice to hold\n"
	.string "tight to what you carry.$"

Tradewind_Text_CurioFarewell::
	.string "The crossing keeps its\n"
	.string "hours. Come back anytime.$"

Tradewind_Text_PawnbrokerOffer::
	.string "Ahh. You carry a curio, and\n"
	.string "the Badge to spend it.\p"
	.string "Hand one over. What it truly\n"
	.string "is... shows only now.$"

Tradewind_Text_StoneRevealed::
	.string "There. A bond, made solid.\n"
	.string "It was always this, waiting.$"

Tradewind_Text_PawnNoCurio::
	.string "No curio, no exchange.\n"
	.string "The Curio Merchant is down\l"
	.string "the way.$"

Tradewind_Text_PawnbrokerClosed::
	.string "This stall isn't lit for you.\n"
	.string "Not yet. Earn the Lantern\l"
	.string "first, then we'll talk.$"

Tradewind_Text_PawnFarewell::
	.string "Bonds keep. Come back when\n"
	.string "you're ready to spend one.$"

Tradewind_Text_GoroDefeat::
	.string "Gahh! Full stomach,\n"
	.string "empty victory...$"

Tradewind_Text_GoroPostBattle::
	.string "Rule one of the market:\n"
	.string "never eat what's offered.\p"
	.string "Rule two: never battle\n"
	.string "Madam Tsuji hungry.$"

Tradewind_Text_LanternBearerDefeat::
	.string "The fox-fire... gutters\n"
	.string "out...$"

Tradewind_Text_LanternBearerPostBattle::
	.string "Follow a lantern in this\n"
	.string "market and you'll walk in\l"
	.string "circles till morning.\p"
	.string "Madam Tsuji lights her own.$"

Tradewind_Text_ApprenticeDefeat::
	.string "Appraised... and found\n"
	.string "wanting. Me, not you.$"

Tradewind_Text_ApprenticePostBattle::
	.string "I appraise what crosses\n"
	.string "this market. Trinket or\l"
	.string "treasure, I can tell.\p"
	.string "You? Treasure. Madam Tsuji\n"
	.string "will want a look at you.$"

Tradewind_Text_TsujiNotReady::
	.string "Madam Tsuji sees no one\n"
	.string "unproven.\p"
	.string "Three of mine keep this\n"
	.string "market. Best them first,\l"
	.string "then we'll speak.$"

Tradewind_Text_TsujiMegaIntro::
	.string "So. The one my three\n"
	.string "couldn't stop.\p"
	.string "I am Tsuji. I deal in bonds\n"
	.string "- the kind between a\l"
	.string "trainer and their POKéMON.\p"
	.string "Wound tight enough, a bond\n"
	.string "becomes a stone. Struck in\l"
	.string "battle, the stone becomes\l"
	.string "power beyond evolution.\p"
	.string "They call it MEGA EVOLUTION.\n"
	.string "Watch closely. It will cost\l"
	.string "you the match.$"

Tradewind_Text_TsujiDefeat::
	.string "...The bond broke my way\n"
	.string "this time. How curious.$"

Tradewind_Text_TsujiBadgeSpeech::
	.string "Take the LANTERN BADGE.\n"
	.string "You've earned its light.\p"
	.string "And take these - a MEGA RING,\n"
	.string "and a stone to start you off.\p"
	.string "A bond wound tight enough\n"
	.string "becomes a stone. Now you\l"
	.string "may strike the spark\l"
	.string "yourself.$"

Tradewind_Text_TsujiPawnbrokerHint::
	.string "One more thing. The\n"
	.string "Pawnbroker's stall is lit\l"
	.string "for you now.\p"
	.string "Those curios the market\n"
	.string "sells? Bring him one and\l"
	.string "see what it truly is.$"

Tradewind_Text_TsujiPostBadge::
	.string "The LANTERN BADGE suits\n"
	.string "you.\p"
	.string "Feed your curios to the\n"
	.string "Pawnbroker. Every bond\l"
	.string "wants to be known.$"

Tradewind_Text_DontEatHere::
	.string "Don't eat. Whatever you do,\n"
	.string "don't eat here.\p"
	.string "You won't get hungry\n"
	.string "again... and you won't leave.$"

Tradewind_Text_FaeTouchedHint::
	.string "Careful up top. Down here,\n"
	.string "only the fae-touched land a\l"
	.string "true blow.\p"
	.string "Bring nothing fae, and\n"
	.string "you'll bring nothing at all.$"

Tradewind_Text_AfterlifeRealtor::
	.string "Plots! Fine plots! Eternal\n"
	.string "rest, water view...\p"
	.string "walking distance to the gym.\n"
	.string "Won't last. Nothing does.$"

Tradewind_Text_SpiritShopper1::
	.string "...cold hands. Warm soup.\n"
	.string "Cold hands...$"

Tradewind_Text_SpiritShopper2::
	.string "I only came for the one\n"
	.string "thing. I forget what.$"

Tradewind_Text_MaskedMerchant::
	.string "Behind the mask? Another\n"
	.string "mask, friend. Buy a smile.\l"
	.string "Only slightly used.$"

@ ============================================================================
@  THE SILENT BOATMAN (Act 2) -- ferries to the occupied National Park.
@  He never speaks. Object: OBJ_EVENT_GFX_SAILOR at the waterline (18,28),
@  Landing on the park side: (16,38), in front of the return boatman (16,37).
@  best-guess coords -- nudge in Porymap. Arrival back from the park lands
@  at (18,27), directly above him.
@  NOT story-gated yet: he ferries whenever asked. Gate with an Act 2 flag
@  (object "flag" field or a goto_if_unset) once the act transition exists.
@ ============================================================================
Tradewind_EventScript_Boatman::
	lock
	faceplayer
	msgbox Tradewind_Text_BoatmanOffer, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Tradewind_EventScript_BoatmanStay
	closemessage
	fadescreen FADE_TO_BLACK
	playse SE_SHIP
	delay 30
	warp MAP_NATIONAL_PARK_ACT2, 16, 38
	waitstate
	release
	end

Tradewind_EventScript_BoatmanStay::
	msgbox Tradewind_Text_BoatmanStay, MSGBOX_DEFAULT
	release
	end

Tradewind_Text_BoatmanOffer::
	.string "The boatman does not greet you.\p"
	.string "He looks at you the way deep water\n"
	.string "looks at a dropped coin, and tips\l"
	.string "his lantern toward the fog bank\l"
	.string "across the strait.\p"
	.string "Beneath it, faintly: the shape of\n"
	.string "the National Park ferry pier.\p"
	.string "Board the boat?$"

Tradewind_Text_BoatmanStay::
	.string "The boatman returns his gaze to\n"
	.string "the water.\p"
	.string "The lantern stays lit.\n"
	.string "The offer stays open.$"

Tradewind_Text_LockedDoor::
	.string "It's locked…$"

Tradewind_Text_GhostDrifloon::
	.string "DRIFLOON drifts against the wind,\n"
	.string "string dangling like an offer.\p"
	.string "Don't take its hand.$"

Tradewind_Text_GhostLampent::
	.string "LAMPENT hangs perfectly still\n"
	.string "over the black water.\p"
	.string "Its flame reflects where there\n"
	.string "is nothing to reflect.$"

Tradewind_Text_GhostGastly::
	.string "GASTLY grins at your reflection\n"
	.string "instead of at you.$"

Tradewind_Text_GhostDuskull::
	.string "DUSKULL's one eye slides from\n"
	.string "socket to socket…\p"
	.string "It is counting the shoppers.\n"
	.string "It counts one too many.$"

Tradewind_Text_GhostLitwick::
	.string "LITWICK's flame bends toward\n"
	.string "you like a beckoning finger.\p"
	.string "The market lanterns bend with it.$"

Tradewind_Text_GhostShuppet::
	.string "SHUPPET nestles among the masks\n"
	.string "on the stall.\p"
	.string "One of the masks is breathing.$"

Tradewind_Text_GhostMisdreavus::
	.string "MISDREAVUS hums a lullaby.\p"
	.string "You've heard it before. You\n"
	.string "can't remember where. It smiles.$"

Tradewind_Text_GhostChanneler::
	.string "The dead barter here, same as\n"
	.string "the living. Look at the vendors'\l"
	.string "feet, if you doubt me.\p"
	.string "The ones that touch the ground\n"
	.string "are the customers.$"

Tradewind_Text_GhostDancer::
	.string "I danced at this market's first\n"
	.string "lantern-lighting.\p"
	.string "That was… oh, a hundred and\n"
	.string "forty years ago.\p"
	.string "…Why are you looking at me\n"
	.string "like that?$"

Tradewind_Text_MegaHint::
	.string "You've seen MADAM TSUJI's ring?\n"
	.string "The stone set in it isn't a jewel.\p"
	.string "They say when her lantern burns\n"
	.string "low, she touches it, and her\l"
	.string "partner becomes something ELSE.\p"
	.string "Mega Evolution, the old-timers\n"
	.string "call it. She'd never show it to\l"
	.string "a stranger.\p"
	.string "Earn her badge, though… and who\n"
	.string "knows what she'll pass on.$"

Tradewind_Text_SableIntro::
	.string "The lanterns whisper to my\n"
	.string "darlings. Right now they're\l"
	.string "whispering about YOU.$"

Tradewind_Text_SableDefeat::
	.string "Shh… they've gone quiet.$"

Tradewind_Text_SablePostBattle::
	.string "My darlings only play with the\n"
	.string "living they LIKE. Feel honored.$"

Tradewind_Text_RoshanIntro::
	.string "Half the shoppers in this market\n"
	.string "cast no shadow. Care to guess\l"
	.string "which half you're in?$"

Tradewind_Text_RoshanDefeat::
	.string "Ah. The half that hits back.$"

Tradewind_Text_RoshanPostBattle::
	.string "Don't buy anything the cold\n"
	.string "vendors sell. Their change is\l"
	.string "always a little TOO old.$"

Tradewind_Text_PiperIntro::
	.string "Grandma says never whistle in\n"
	.string "the market after dark.\p"
	.string "I whistle ALL the time!$"

Tradewind_Text_PiperDefeat::
	.string "Eep! Something whistled BACK!$"

Tradewind_Text_PiperPostBattle::
	.string "SABLEYE followed me home from\n"
	.string "the gem stall. Or maybe I\l"
	.string "followed IT home. It's fuzzy.$"
