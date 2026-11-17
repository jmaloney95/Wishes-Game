#ifndef GUARD_CONSTANTS_PORTRAITS_H
#define GUARD_CONSTANTS_PORTRAITS_H

// Portrait ids for the showportrait script command. Every id renders the
// placeholder until real art is added: drop the PNG into graphics/portraits/
// and repoint that id's row in sNpcPortraits (src/npc_portrait.c). No script
// changes are ever needed.
#define PORTRAIT_PLACEHOLDER      0
#define PORTRAIT_CLARKSON_GENGAR  1
#define PORTRAIT_MADAM_TSUJI      2
#define PORTRAIT_MUTRID_LEADER    3
#define PORTRAIT_RED_FATALITY     4
#define PORTRAIT_DRACO            5
#define PORTRAIT_COBRA            6
#define PORTRAIT_MINISTER         7
#define PORTRAIT_EDWARDS          8
#define PORTRAIT_MIKMANC          9
#define PORTRAIT_ALLISON          10
#define PORTRAIT_YIFFER           11
#define PORTRAIT_NESSA            12
// The Z-A cast (Mega Dimension). Tulip, Jacinthe and Mable are placeholder
// copies until their art exists.
#define PORTRAIT_ZA_CANARI        13
#define PORTRAIT_ZA_LIDA          14
#define PORTRAIT_ZA_EMMA          15
#define PORTRAIT_ZA_TAUNIE        16
#define PORTRAIT_ZA_TULIP         17
#define PORTRAIT_ZA_JACINTHE      18
#define PORTRAIT_ZA_MABLE         19
#define PORTRAIT_ZA_AZ            20
#define PORTRAIT_COUNT            21

// Ids below this are AI-assisted art and are never drawn (config/ui.h). Real
// art for one of them: move it to a new id at or past this one.
#define PORTRAIT_FIRST_CLEARED    PORTRAIT_ZA_CANARI

// Which side of the screen the portrait appears on, above the message box.
#define PORTRAIT_LEFT   0
#define PORTRAIT_RIGHT  1

#endif // GUARD_CONSTANTS_PORTRAITS_H
