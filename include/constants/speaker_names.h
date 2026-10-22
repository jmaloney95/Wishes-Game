#ifndef GUARD_CONSTANTS_SPEAKER_NAMES_H
#define GUARD_CONSTANTS_SPEAKER_NAMES_H

enum SpeakerNames {
    SP_NAME_NONE = 0,
    SP_NAME_MOM,
    SP_NAME_PLAYER,
    // Wishes of Tomorrow cast
    SP_NAME_CLARKSON,
    SP_NAME_HARU,
    SP_NAME_MASTER_GEN,
    SP_NAME_MADAM_TSUJI,
    SP_NAME_MUTRID_LEADER,
    SP_NAME_RED_FATALITY,
    SP_NAME_DRACO,
    SP_NAME_COBRA,
    SP_NAME_MIKMANC,
    SP_NAME_MINISTER,
    SP_NAME_YIFFER,
    SP_NAME_TUNNEL_CAPTAIN,
    SP_NAME_GOON,
    SP_NAME_ORACLE,
    SP_NAME_OLD_WOMAN,
    SP_NAME_ALLISON,
    SP_NAME_GENGAR,
    SP_NAME_RYOKO,
    SP_NAME_MUTRID_CAPTAIN,
    SP_NAME_OPERATIVE,
    SP_NAME_THE_ONI,
    SP_NAME_DEFECTOR,
    SP_NAME_EDWARDS,
    // The Mega Dimension (Legends Z-A cast)
    SP_NAME_LIDA,
    SP_NAME_CANARI,
    SP_NAME_EMMA,
    SP_NAME_TAUNIE,
    SP_NAME_TULIP,
    SP_NAME_JACINTHE,
    SP_NAME_MABLE,
    SP_NAME_COUNT
};

// Namebox style (plate colour + name colour), assigned per speaker in
// src/data/speaker_names.h; the colours are sNameboxStyles in
// src/field_name_box.c. Speakers set from a raw text pointer (including
// auto-named trainers) always use SPEAKER_TIER_DEFAULT.
enum SpeakerNameTier {
    SPEAKER_TIER_DEFAULT = 0, // blue-grey plate, near-white name: everyone without a class
    SPEAKER_TIER_VILLAIN,     // red plate, white name: Team Mutrid (Draco, Leader ???, goons)
    SPEAKER_TIER_QUEST,       // green name: quest-giver characters
    SPEAKER_TIER_GHOST,       // purple name: GENGAR (Clarkson's ghost)
    // The Legends Z-A cast (Mega Dimension DLC): their own class, always on a
    // dark blue plate, each with their own name colour.
    SPEAKER_TIER_ZA,          // white name: Emma, Taunie, Tulip
    SPEAKER_TIER_ZA_YELLOW,   // Canari
    SPEAKER_TIER_ZA_BLUE,     // Lida (light blue)
    SPEAKER_TIER_ZA_PURPLE,   // Jacinthe
    SPEAKER_TIER_ZA_GREEN,    // Researcher Mable
    SPEAKER_TIER_COUNT
};

#endif // GUARD_CONSTANTS_SPEAKER_NAMES_H
