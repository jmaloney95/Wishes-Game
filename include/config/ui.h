#ifndef GUARD_CONFIG_UI_H
#define GUARD_CONFIG_UI_H

// Wishes of Tomorrow UI features (see wishes_of_tomorrow_ui_features_handoff.md).
// The speaker namebox is configured separately in include/config/name_box.h.

// 64x64 character portraits beside the message box, driven by the
// showportrait/hideportrait script commands (src/npc_portrait.c).
#define USE_NPC_PORTRAITS TRUE

// Horizontal icon-bar start menu (sprites along the bottom + a label window)
// instead of the vanilla vertical text list. Presentation only: the action
// list, conditional entries, and every callback are untouched. FALSE restores
// the vanilla menu.
#define USE_GRAPHICAL_START_MENU TRUE

// Non-blocking quest toast: startquest/completequest slide a corner popup in
// and out (no A press, player keeps moving) instead of the blocking message
// box. NUMERIC 1/0 (not TRUE/FALSE) because asm/macros/event.inc gates the
// quest macros on this through the assembler's cpp pass, where TRUE is not
// defined. 0 restores the old blocking announcement.
#define USE_QUEST_TOAST 1

// Floating "!" over quest-giver NPCs while their quest is still available
// (src/quest_indicator.c). FALSE hides all indicators.
#define USE_QUEST_GIVER_INDICATOR TRUE

#endif // GUARD_CONFIG_UI_H
