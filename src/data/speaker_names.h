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
    [SP_NAME_TUNNEL_CAPTAIN]  = COMPOUND_STRING("CAPTAIN KURODA"),
    [SP_NAME_GOON]            = COMPOUND_STRING("GOON"),
    [SP_NAME_ORACLE]          = COMPOUND_STRING("THE ORACLE"),
    [SP_NAME_OLD_WOMAN]       = COMPOUND_STRING("OLD WOMAN"),
    [SP_NAME_ALLISON]         = COMPOUND_STRING("ALLISON"),
    [SP_NAME_GENGAR]          = COMPOUND_STRING("GENGAR"),
    [SP_NAME_RYOKO]           = COMPOUND_STRING("CAPTAIN RYOKO"),
    [SP_NAME_MUTRID_CAPTAIN]  = COMPOUND_STRING("THE CAPTAIN"),
    [SP_NAME_OPERATIVE]       = COMPOUND_STRING("???"),
};

// Name color tier per speaker (see enum SpeakerNameTier). Anyone not listed
// here - including all auto-named trainers - renders with the default colors.
// Red is reserved for Team Mutrid; quest givers are green.
const u8 gSpeakerNameTiers[SP_NAME_COUNT] =
{
    [SP_NAME_MASTER_GEN]    = SPEAKER_TIER_QUEST,
    [SP_NAME_MUTRID_LEADER] = SPEAKER_TIER_VILLAIN,
    [SP_NAME_DRACO]         = SPEAKER_TIER_VILLAIN,
    [SP_NAME_GOON]          = SPEAKER_TIER_VILLAIN,
    [SP_NAME_GENGAR]        = SPEAKER_TIER_GHOST,
    [SP_NAME_RYOKO]         = SPEAKER_TIER_VILLAIN,
    [SP_NAME_TUNNEL_CAPTAIN] = SPEAKER_TIER_VILLAIN,
    [SP_NAME_MUTRID_CAPTAIN] = SPEAKER_TIER_VILLAIN,
};
