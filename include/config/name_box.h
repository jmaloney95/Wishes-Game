#ifndef GUARD_CONFIG_NAME_BOX_H
#define GUARD_CONFIG_NAME_BOX_H

#define OW_FLAG_SUPPRESS_NAME_BOX   FLAG_SUPPRESS_SPEAKER_NAME  // If this flag is set, any namebox (whether its from a macro or a code) will not show up until this flag is unset.

// Namebox Speaker configs
#define OW_NAME_BOX_USE_DYNAMIC_WIDTH TRUE  // When TRUE, the namebox window can use different width depending on the length of the speaker's name.
#define OW_NAME_BOX_NPC_TRAINER       FALSE // When TRUE, EVERY approaching trainer auto-shows a (default-color) name plate. WoT keeps this OFF so random trainers have no plate; named cast get plates via setspeaker in their scripts.
#define OW_NAME_BOX_DEFAULT_WIDTH     12    // Maximum width of what OW_NAME_BOX_USE_DYNAMIC_WIDTH can set. Also the default width when the config above is set to FALSE (or the dynamic width exceeds this value). 12 fits "PROFESSOR CLARKSON".
#define OW_NAME_BOX_DEFAULT_HEIGHT    2     // Maximum height of the namebox window.

// Text colors of Namebox. The numbers corresponds to the palette index.
// The BG color is not provided as it always needs to be 0.
// WoT's dark message_box palette inverts vanilla: 2 = near-white text, 3 = grey shadow.
#define OW_NAME_BOX_FOREGROUND_COLOR  2
#define OW_NAME_BOX_SHADOW_COLOR      3

// Per-class plate and name colours: sNameboxStyles in src/field_name_box.c.
// Which speaker gets which class is data: gSpeakerNameTiers in
// src/data/speaker_names.h. Each plate is drawn by writing its two colours
// into the message-box palette before the name prints:
#define OW_NAME_BOX_PLATE_COLOR      15 // the plate fill in name_box.png (every plate)
#define OW_NAME_BOX_NAME_COLOR       10 // not used by any box art

#endif // GUARD_CONFIG_NAME_BOX_H
