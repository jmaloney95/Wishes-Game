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
    SP_NAME_COLONEL_EDWARDS,
    SP_NAME_COUNT
};

// Namebox name-text color tier, assigned per speaker in
// src/data/speaker_names.h. Speakers set from a raw text pointer
// (including auto-named trainers) always use SPEAKER_TIER_DEFAULT.
enum SpeakerNameTier {
    SPEAKER_TIER_DEFAULT = 0, // random NPCs and trainers (LASS, FISHERMAN, ...)
    SPEAKER_TIER_MAIN,        // main story cast
    SPEAKER_TIER_QUEST,       // quest-giver characters
};

#endif // GUARD_CONSTANTS_SPEAKER_NAMES_H
