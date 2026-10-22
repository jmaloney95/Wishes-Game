const u8 *const gSpeakerNamesTable[SP_NAME_COUNT] =
{
    [SP_NAME_MOM]             = COMPOUND_STRING("MOM"),
    [SP_NAME_PLAYER]          = COMPOUND_STRING("{PLAYER}"),
    // Wishes of Tomorrow cast. Trainer spellings match src/data/trainers.party
    // so scripted plates look identical to auto-named battle plates.
    [SP_NAME_CLARKSON]        = COMPOUND_STRING("PROFESSOR CLARKSON"),
    [SP_NAME_HARU]            = COMPOUND_STRING("HARU"),
    [SP_NAME_MASTER_GEN]      = COMPOUND_STRING("MASTER GEN"),
    [SP_NAME_MADAM_TSUJI]     = COMPOUND_STRING("MADAME TSUJI"),
    [SP_NAME_MUTRID_LEADER]   = COMPOUND_STRING("??? LEADER"),
    [SP_NAME_RED_FATALITY]    = COMPOUND_STRING("RedFatality"),
    [SP_NAME_DRACO]           = COMPOUND_STRING("DRACO"),
    [SP_NAME_COBRA]           = COMPOUND_STRING("COBRA"),
    [SP_NAME_MIKMANC]         = COMPOUND_STRING("MikManc"),
    [SP_NAME_MINISTER]        = COMPOUND_STRING("Minister"),
    [SP_NAME_YIFFER]          = COMPOUND_STRING("Yiffer"),
    [SP_NAME_TUNNEL_CAPTAIN]  = COMPOUND_STRING("CAPTAIN McCORMICK"),
    [SP_NAME_GOON]            = COMPOUND_STRING("GOON"),
    [SP_NAME_ORACLE]          = COMPOUND_STRING("THE ORACLE"),
    [SP_NAME_OLD_WOMAN]       = COMPOUND_STRING("OLD WOMAN"),
    [SP_NAME_ALLISON]         = COMPOUND_STRING("SIREN ALLISON"),
    [SP_NAME_GENGAR]          = COMPOUND_STRING("GENGAR"),
    [SP_NAME_RYOKO]           = COMPOUND_STRING("CAPTAIN RYOKO"),
    [SP_NAME_MUTRID_CAPTAIN]  = COMPOUND_STRING("MUTRID ALPHA"),
    [SP_NAME_OPERATIVE]       = COMPOUND_STRING("NESSA"),
    [SP_NAME_THE_ONI]         = COMPOUND_STRING("THE ONI"),
    [SP_NAME_DEFECTOR]        = COMPOUND_STRING("Prof. Assistant"),
    [SP_NAME_EDWARDS]         = COMPOUND_STRING("GENERAL EDWARDS"),
    [SP_NAME_LIDA]            = COMPOUND_STRING("LIDA"),
    [SP_NAME_CANARI]          = COMPOUND_STRING("CANARI"),
    [SP_NAME_EMMA]            = COMPOUND_STRING("EMMA"),
    [SP_NAME_TAUNIE]          = COMPOUND_STRING("TAUNIE"),
    [SP_NAME_TULIP]           = COMPOUND_STRING("TULIP"),
    [SP_NAME_JACINTHE]        = COMPOUND_STRING("JACINTHE"),
    [SP_NAME_MABLE]           = COMPOUND_STRING("RESEARCHER MABLE"),
};

// Name color tier per speaker (see enum SpeakerNameTier). Anyone not listed
// here - including all auto-named trainers - renders with the default colors.
// Red is reserved for Team Mutrid; quest givers are green.
const u8 gSpeakerNameTiers[SP_NAME_COUNT] =
{
    [SP_NAME_MASTER_GEN]    = SPEAKER_TIER_QUEST,
    [SP_NAME_ALLISON]       = SPEAKER_TIER_QUEST,
    [SP_NAME_MUTRID_LEADER] = SPEAKER_TIER_VILLAIN,
    [SP_NAME_DRACO]         = SPEAKER_TIER_VILLAIN,
    [SP_NAME_GOON]          = SPEAKER_TIER_VILLAIN,
    [SP_NAME_GENGAR]        = SPEAKER_TIER_GHOST,
    [SP_NAME_RYOKO]         = SPEAKER_TIER_VILLAIN,
    [SP_NAME_TUNNEL_CAPTAIN] = SPEAKER_TIER_VILLAIN,
    [SP_NAME_THE_ONI]       = SPEAKER_TIER_VILLAIN,
    [SP_NAME_DEFECTOR]      = SPEAKER_TIER_QUEST,
    [SP_NAME_MUTRID_CAPTAIN] = SPEAKER_TIER_VILLAIN,
    // The Legends Z-A cast: dark blue plates (their own class).
    [SP_NAME_LIDA]          = SPEAKER_TIER_ZA_BLUE,
    [SP_NAME_CANARI]        = SPEAKER_TIER_ZA_YELLOW,
    [SP_NAME_EMMA]          = SPEAKER_TIER_ZA,
    [SP_NAME_TAUNIE]        = SPEAKER_TIER_ZA,
    [SP_NAME_TULIP]         = SPEAKER_TIER_ZA,
    [SP_NAME_JACINTHE]      = SPEAKER_TIER_ZA_PURPLE,
    [SP_NAME_MABLE]         = SPEAKER_TIER_ZA_GREEN,  // green: who to report back to
};
