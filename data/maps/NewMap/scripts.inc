NewMap_MapScripts::
	.byte 0

@ ============================================================================
@  Villagers - the pre-dawn oni sighting (foreshadow the climb up Mt. Munen)
@ ============================================================================
NewMap_EventScript_Watchman::
	lock
	faceplayer
	msgbox NewMap_Text_Watchman, MSGBOX_DEFAULT
	release
	end

NewMap_EventScript_WorriedWoman::
	lock
	faceplayer
	msgbox NewMap_Text_WorriedWoman, MSGBOX_DEFAULT
	release
	end

NewMap_EventScript_OldMan::
	lock
	faceplayer
	msgbox NewMap_Text_OldMan, MSGBOX_DEFAULT
	release
	end

NewMap_EventScript_Kid::
	lock
	faceplayer
	goto_if_set FLAG_NEWMAP_TUNNEL_RUMOR, NewMap_EventScript_KidDare
	msgbox NewMap_Text_Kid, MSGBOX_DEFAULT
	release
	end

NewMap_EventScript_KidDare::
	msgbox NewMap_Text_KidDare, MSGBOX_DEFAULT
	release
	end

NewMap_EventScript_Skeptic::
	lock
	faceplayer
	msgbox NewMap_Text_Skeptic, MSGBOX_DEFAULT
	release
	end

NewMap_EventScript_MountainHint::
	lock
	faceplayer
	msgbox NewMap_Text_MountainHint, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  Tame Pokemon ambiance (cry when spoken to)
@ ============================================================================
NewMap_EventScript_Poochyena::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_POOCHYENA, CRY_MODE_NORMAL
	msgbox NewMap_Text_Poochyena, MSGBOX_DEFAULT
	waitmoncry
	release
	end

NewMap_EventScript_Skitty::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_SKITTY, CRY_MODE_NORMAL
	msgbox NewMap_Text_Skitty, MSGBOX_DEFAULT
	waitmoncry
	release
	end

NewMap_EventScript_Wingull::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_WINGULL, CRY_MODE_NORMAL
	msgbox NewMap_Text_Wingull, MSGBOX_DEFAULT
	waitmoncry
	release
	end

@ ============================================================================
@  Munen Tunnel entrance + Itsuki's boarded-up house
@ ============================================================================
NewMap_EventScript_TunnelSign::
	msgbox NewMap_Text_TunnelSign, MSGBOX_SIGN
	end

NewMap_EventScript_ApprenticeDoorCrates::
	lock
	msgbox NewMap_Text_ApprenticeDoorCrates, MSGBOX_DEFAULT
	release
	end

@ ============================================================================
@  Text
@ ============================================================================
NewMap_Text_Watchman::
	.string "Before dawn, a line of them came\n"
	.string "through the gate. Oni masks, every\l"
	.string "one. Not a single word between them.\p"
	.string "They went straight up to STAR SUMMIT\n"
	.string "and didn't look back. Gave me chills.$"

NewMap_Text_WorriedWoman::
	.string "You saw the masked ones too?\p"
	.string "What business do folk in demon masks\n"
	.string "have at the summit...$"

NewMap_Text_OldMan::
	.string "Oni masks, before the sun.\p"
	.string "In my grandfather's day, that meant\n"
	.string "one of two things, a festival\p"
	.string "or a reckoning. And I hear no drums.$"

NewMap_Text_Kid::
	.string "I SAW them! Big scary red masks!\p"
	.string "They ran up the mountain path super\n"
	.string "fast, like they weren't even cold!\l"
	.string "I wanna know what's up there!$"

NewMap_Text_Skeptic::
	.string "The masked lot? Pff. Festival troupe,\n"
	.string "rehearsing early. Has to be.\p"
	.string "...They left awful quiet, though.\n"
	.string "And nobody's come back down.$"

NewMap_Text_MountainHint::
	.string "Thinking of following them up?\p"
	.string "It's dangerous up there.\n"
	.string "High level Pokemon prey under the fog.$"

NewMap_Text_Poochyena::
	.string "The town's Poochyena stands stiff by\n"
	.string "the door, ears pricked toward the peak.$"

NewMap_Text_Skitty::
	.string "A well-fed Skitty dozes in a warm\n"
	.string "patch of sun, not a care in the world.$"

NewMap_Text_Wingull::
	.string "A Wingull preens on the rooftop,\n"
	.string "watching the mountain road.$"

NewMap_Text_KidDare::
	.string "You talked to old man Gen, huh?\p"
	.string "You're really going into the tomb\n"
	.string "tunnel?! For REAL?!\p"
	.string "I dared Touya to touch the door\n"
	.string "once and he cried for a WEEK.\p"
	.string "...I saw something shiny just\n"
	.string "inside, y'know. Never went back\l"
	.string "for it. You can have it. Probably.$"

NewMap_Text_TunnelSign::
	.string "MUNEN TUNNEL\p"
	.string "Resting place of the mountain\n"
	.string "dead. Sealed by order of the\l"
	.string "village elders.\p"
	.string "DO NOT WAKE THEM.\p"
	.string "...The seal beneath the sign is\n"
	.string "torn. The door stands open.$"

NewMap_Text_ApprenticeDoorCrates::
	.string "Heavy crates are stacked against\n"
	.string "the door, nailed shut.\p"
	.string "A carpenter's mark on the lid:\n"
	.string "“GEN.”\p"
	.string "Someone wanted this house left\n"
	.string "alone.$"

@ ============================================================================
@  POMPEII NIGHT MARKET (Spirited Away veil-market direction)
@  Ready-to-attach labels -- no objects are placed yet. In Porymap, add the
@  object and point its script at one of these:
@    NewMap_EventScript_LanternLighter  (OBJ_EVENT_GFX_OLD_WOMAN, FACE_UP)
@    NewMap_EventScript_FoodStall       (OBJ_EVENT_GFX_COOK or POKEFAN_M, behind a counter)
@    NewMap_EventScript_TrinketStall    (OBJ_EVENT_GFX_WOMAN_5 or GENTLEMAN)
@    NewMap_EventScript_KimonoDancer    (OBJ_EVENT_GFX_KIMONO_GIRL, WANDER or spin)
@    NewMap_EventScript_MaskedPatron    (OBJ_EVENT_GFX_MAGMA_MEMBER_M, FACE_DOWN)
@    NewMap_EventScript_MarketSign      (bg_event sign)
@  PALETTE NOTE: 3 mon species (Poochyena/Skitty/Wingull) already live on this
@  map -- prefer standard-palette human NPCs here, and verify nothing turns
@  invisible after adding gfx (see palette-exhaustion notes).
@ ============================================================================
NewMap_EventScript_LanternLighter::
	lock
	faceplayer
	msgbox NewMap_Text_LanternLighter, MSGBOX_DEFAULT
	release
	end

NewMap_EventScript_FoodStall::
	lock
	faceplayer
	message NewMap_Text_FoodStallGreeting
	waitmessage
	pokemart NewMap_FoodStallMart
	msgbox NewMap_Text_FoodStallFarewell, MSGBOX_DEFAULT
	release
	end

	.align 2
NewMap_FoodStallMart:
	.2byte ITEM_MOOMOO_MILK
	.2byte ITEM_LEMONADE
	.2byte ITEM_LAVA_COOKIE
	.2byte ITEM_RAGE_CANDY_BAR
	.2byte ITEM_CASTELIACONE
	pokemartlistend

NewMap_EventScript_TrinketStall::
	lock
	faceplayer
	message NewMap_Text_TrinketStallGreeting
	waitmessage
	pokemart NewMap_TrinketStallMart
	msgbox NewMap_Text_TrinketStallFarewell, MSGBOX_DEFAULT
	release
	end

	.align 2
NewMap_TrinketStallMart:
	.2byte ITEM_CLEANSE_TAG
	.2byte ITEM_SPELL_TAG
	.2byte ITEM_SMOKE_BALL
	.2byte ITEM_DUSK_BALL
	pokemartlistend

NewMap_EventScript_KimonoDancer::
	lock
	faceplayer
	msgbox NewMap_Text_KimonoDancer, MSGBOX_DEFAULT
	release
	end

@ Masked figure browsing the market. Reacts to the Carved Mask -- ties the
@ Act 2 disguise into town before the player ever needs it.
NewMap_EventScript_MaskedPatron::
	lock
	faceplayer
	checkitem ITEM_CARVED_MASK
	goto_if_eq VAR_RESULT, TRUE, NewMap_EventScript_MaskedPatronNod
	msgbox NewMap_Text_MaskedPatron, MSGBOX_DEFAULT
	release
	end

NewMap_EventScript_MaskedPatronNod::
	msgbox NewMap_Text_MaskedPatronNod, MSGBOX_DEFAULT
	release
	end

NewMap_EventScript_MarketSign::
	msgbox NewMap_Text_MarketSign, MSGBOX_SIGN
	end

@ The slab over the Munen Tunnel mouth. Master Gen has it rolled aside when
@ he gives the Save Face quest (FLAG_POMPEII_TUNNEL_ROCK_CLEARED hides it).
NewMap_EventScript_TunnelRock::
	msgbox NewMap_Text_TunnelRock, MSGBOX_SIGN
	end

NewMap_Text_TunnelRock::
	.string "A great stone slab seals the old\n"
	.string "tunnel mouth. It would take half\l"
	.string "the village to move it.\p"
	.string "Fresh chisel marks ring its edge --\n"
	.string "the MASK-MAKER's work, by the look\l"
	.string "of the cuts.$"

NewMap_Text_LanternLighter::
	.string "Every lantern lit before full dark,\n"
	.string "that's the rule. Every single one.\p"
	.string "Not for the pilgrims, dear.\n"
	.string "For the OTHER customers.\p"
	.string "The ones who only come out once\n"
	.string "the paper glows red.$"

NewMap_Text_FoodStallGreeting::
	.string "Hot! Sweet! Fresh off the coals!\p"
	.string "Eat well before the climb, or the\n"
	.string "mountain eats YOU. Ha!$"

NewMap_Text_FoodStallFarewell::
	.string "Don't leave food at the tunnel\n"
	.string "mouth. It ATTRACTS things.$"

NewMap_Text_TrinketStallGreeting::
	.string "Charms and wards, traveler.\p"
	.string "Sold more this week than in the\n"
	.string "last ten years. Wonder why.$"

NewMap_Text_TrinketStallFarewell::
	.string "Keep the Cleanse Tag on the\n"
	.string "OUTSIDE of your bag. It has to\l"
	.string "see them coming.$"

NewMap_Text_KimonoDancer::
	.string "The festival needs a dancer, so\n"
	.string "I practice. Masked ones or no\l"
	.string "masked ones.\p"
	.string "When the drums finally sound,\n"
	.string "Pompeii will remember what this\l"
	.string "market is FOR.$"

NewMap_Text_MaskedPatron::
	.string "A figure in an oni mask stands\n"
	.string "utterly still among the stalls.\p"
	.string "It does not browse. It does not\n"
	.string "speak.\p"
	.string "It is counting the lanterns.$"

NewMap_Text_MaskedPatronNod::
	.string "The masked figure's gaze drops to\n"
	.string "the carved face at your belt.\p"
	.string "It inclines its head, slowly, and\n"
	.string "returns to counting lanterns.\p"
	.string "You have been mistaken for\n"
	.string "someone. Or something.$"

NewMap_Text_MarketSign::
	.string "POMPEII NIGHT MARKET\p"
	.string "“Lit lanterns welcome two kinds\n"
	.string "of guests.\p"
	.string "“Please do not stare at the\n"
	.string "second kind.”$"
