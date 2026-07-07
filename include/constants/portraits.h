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
#define PORTRAIT_COUNT            7

// Which side of the screen the portrait appears on, above the message box.
#define PORTRAIT_LEFT   0
#define PORTRAIT_RIGHT  1

#endif // GUARD_CONSTANTS_PORTRAITS_H
