// Wishes of Tomorrow: what each Mega Dimension partner says at each
// friendship level. Edit these freely -- nothing else needs to change.
//
// sPartnerFieldLines: talking to them inside the dimension (Lv1-Lv4).
// sPartnerHotelLines: their greeting in Hotel Z at Lv2-Lv4 (at Lv1 they still
//                     give their pitch / "still partners?" line from
//                     data/maps/HotelZ/scripts.inc).
// Order: Canari, Lida, Emma, Taunie, Tulip, Jacinthe. One box is two lines
// of about 30 characters: \n then \l inside a box, \p starts a new box.

static const u8 *const sPartnerFieldLines[WOT_PARTNER_SAVE_COUNT][FRIENDSHIP_MAX_LEVEL] =
{
    { // Canari
        COMPOUND_STRING("Chat says we're the best duo\non the whole platform. Let's go\lDYN4MO!"),
        COMPOUND_STRING("Chat's been asking about you.\nThey call us 'the Rift Duo'!\pI didn't even pay them to!"),
        COMPOUND_STRING("Real talk, off-stream? Rifts\nused to scare me a little.\pWith you here, I forget to be\nscared. Don't tell chat."),
        COMPOUND_STRING("Camera's off for this one.\pThanks for always picking me,\n{PLAYER}. Best co-host ever."),
    },
    { // Lida
        COMPOUND_STRING("Team MZ, field unit of two.\nI like the sound of that!"),
        COMPOUND_STRING("Every run, the rifts have one\nless trick to surprise us with.\pTeam MZ is learning!"),
        COMPOUND_STRING("Lucario and I trained extra\nhard to keep up with you.\pCan you tell?"),
        COMPOUND_STRING("When Team MZ started, it was\njust me and a big dream.\pNow look at us. Thanks for\nbeing part of it, {PLAYER}."),
    },
    { // Emma
        COMPOUND_STRING("Stay close. Every rift has a\npattern, and I mean to find it."),
        COMPOUND_STRING("You favor your left when you\nscout. I've adjusted my cover\lto match."),
        COMPOUND_STRING("Case notes: partner reliable,\nreckless only when it counts.\pI find I approve."),
        COMPOUND_STRING("I used to solve everything\nalone. It was quieter.\pThis is better. Don't make me\nsay it twice."),
    },
    { // Taunie
        COMPOUND_STRING("This is so much more fun with\nthe two of us!"),
        COMPOUND_STRING("Ooh, I can read these rifts\nway better now! It's like\lwe're in sync!"),
        COMPOUND_STRING("Hehe, I packed snacks for us.\nRift snacks! They taste a\lteensy bit gray."),
        COMPOUND_STRING("You know what? You're my best\nfriend, {PLAYER}!\pThere, I said it! Let's go!"),
    },
    { // Tulip
        COMPOUND_STRING("Gray all around us, darling,\nand yet we still shine."),
        COMPOUND_STRING("Even in all this gray, you\nhave a glow about you, darling.\pThe rifts suit you."),
        COMPOUND_STRING("Gardevoir is in fine form\ntoday. She's showing off for\lyou, you know."),
        COMPOUND_STRING("Beauty is meant to be shared,\ndarling. I've shared every\lrift with you.\pThat makes them beautiful."),
    },
    { // Jacinthe
        COMPOUND_STRING("Do keep up. I'd hate to\nwin this one without you."),
        COMPOUND_STRING("Hmph. You're not entirely\nhopeless out here.\pI may even enjoy this."),
        COMPOUND_STRING("Mawile and I have grown rather\nfond of your company.\pDon't let it go to your head."),
        COMPOUND_STRING("I've been to every gala on the\ncoast. Not one compares to a\lrift at your side, {PLAYER}.\pThere. Now you know."),
    },
};

// Handing over their crystal at friendship Lv3. First pass.
static const u8 *const sPartnerCrystalLines[WOT_PARTNER_SAVE_COUNT] =
{
    COMPOUND_STRING("Okay, gift time! Off-stream, even.\pThis is my Electrium Z. Hit them\nwith something worth clipping!"),
    COMPOUND_STRING("You've got my back in there, so\nhere's something for yours.\pFightinium Z. Hit hard, {PLAYER}."),
    COMPOUND_STRING("I've been holding on to some\nevidence. I'd rather you had it.\pDarkinium Z. Use it discreetly."),
    COMPOUND_STRING("Don't get the wrong idea. A rival\nhas to be worth beating.\pDragonium Z. Now keep up!"),
    COMPOUND_STRING("You've been so patient with me.\pThis Psychium Z sings for you.\nPlease take it."),
    COMPOUND_STRING("You've proven yourself worthy of\na proper gift.\pSteelium Z. Do try to be elegant."),
};

static const u8 *const sPartnerHotelLines[WOT_PARTNER_SAVE_COUNT][FRIENDSHIP_MAX_LEVEL - 1] =
{
    { // Canari
        COMPOUND_STRING("{PLAYER}! The rift clips are\nblowing up. You're trending!\pReady for round… whatever\nnumber we're on?"),
        COMPOUND_STRING("I made you a badge for the\nstream. 'Partner of the Year'.\pOkay, it's a sticker. Still\ncounts!"),
        COMPOUND_STRING("Before we go live… you're my\nfavorite person to explore\lwith. Just so you know.\pOkay! Cameras on! DYN4MO!"),
    },
    { // Lida
        COMPOUND_STRING("Welcome back! I wrote down\nwhat we learned last run.\pMostly 'run faster'. Want to\ngo again?"),
        COMPOUND_STRING("The others keep asking how we\nmake such a good team.\pI tell them: trust, and a lot\nof portal runs!"),
        COMPOUND_STRING("Team MZ's best field unit,\nreporting for duty!\pWherever the next rift opens,\nI'm with you."),
    },
    { // Emma
        COMPOUND_STRING("Another rift, another set of\nclues. Shall we compare notes\lon the way in?"),
        COMPOUND_STRING("I closed three cases this week\non what we saw in the rifts.\pYour credit. Shall we find a\nfourth?"),
        COMPOUND_STRING("Partner. I don't use the word\nlightly.\pThe file on the rifts is still\nopen. Let's close it together."),
    },
    { // Taunie
        COMPOUND_STRING("{PLAYER}! I practiced reading\nportals all morning!\pWanna see how good I got?"),
        COMPOUND_STRING("Everyone at Hotel Z says\nwe're inseparable now. Hehe!\pThey're right, right?"),
        COMPOUND_STRING("Best friends, best partners,\nbest rift team ever!\pWhere are we going today?"),
    },
    { // Tulip
        COMPOUND_STRING("Darling! I touched up my look\njust for our next run.\pShall we make an entrance?"),
        COMPOUND_STRING("My clients ask about my secret\nlately. I tell them: a good\lpartner and a little danger."),
        COMPOUND_STRING("My dearest partner. Every\nportal with you is a runway.\pShall we?"),
    },
    { // Jacinthe
        COMPOUND_STRING("Ah, there you are. I was\nbeginning to think you'd\lforgotten me. Shall we?"),
        COMPOUND_STRING("I had my staff clear my\nschedule. For the rifts, of\lcourse. Not for you.\p…Mostly."),
        COMPOUND_STRING("My dear {PLAYER}. My doors\nare always open to you.\pNow, which rift today?"),
    },
};
