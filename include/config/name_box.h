#ifndef GUARD_CONFIG_NAME_BOX_H
#define GUARD_CONFIG_NAME_BOX_H

#define OW_FLAG_SUPPRESS_NAME_BOX   FLAG_SUPPRESS_SPEAKER_NAME  // If this flag is set, any namebox (whether its from a macro or a code) will not show up until this flag is unset.

// Namebox Speaker configs
#define OW_NAME_BOX_USE_DYNAMIC_WIDTH TRUE  // When TRUE, the namebox window can use different width depending on the length of the speaker's name.
#define OW_NAME_BOX_NPC_TRAINER       TRUE  // When TRUE, any approaching NPC trainers will have a namebox shown automagically. The name will be taken from their trainer data.
#define OW_NAME_BOX_DEFAULT_WIDTH     12    // Maximum width of what OW_NAME_BOX_USE_DYNAMIC_WIDTH can set. Also the default width when the config above is set to FALSE (or the dynamic width exceeds this value). 12 fits "PROFESSOR CLARKSON".
#define OW_NAME_BOX_DEFAULT_HEIGHT    2     // Maximum height of the namebox window.

// Text colors of Namebox. The numbers corresponds to the palette index.
// The BG color is not provided as it always needs to be 0.
// WoT's dark message_box palette inverts vanilla: 2 = near-white text, 3 = grey shadow.
#define OW_NAME_BOX_FOREGROUND_COLOR  2
#define OW_NAME_BOX_SHADOW_COLOR      3

// Per-tier name colors (message_box palette indexes). Which speaker gets
// which tier is data: gSpeakerNameTiers in src/data/speaker_names.h.
// Auto-named trainers and raw text-pointer speakers use the defaults above.
#define OW_NAME_BOX_VILLAIN_FG_COLOR      4 // red - Team Mutrid
#define OW_NAME_BOX_VILLAIN_SHADOW_COLOR  5
#define OW_NAME_BOX_QUEST_FG_COLOR        6 // green - quest-giver characters
#define OW_NAME_BOX_QUEST_SHADOW_COLOR    7

#endif // GUARD_CONFIG_NAME_BOX_H
