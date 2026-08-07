#ifndef GUARD_NPC_PORTRAIT_H
#define GUARD_NPC_PORTRAIT_H

#include "config/ui.h"
#include "constants/portraits.h"

void ShowNpcPortrait(u8 portraitId, u8 side);
void HideNpcPortrait(void);
void WotShowSpeakerPortrait(u32 speakerId);
void GetNpcPortraitGfx(u8 portraitId, const u8 **tiles, const u16 **palette);
void WotHideAutoPortrait(void);

#endif // GUARD_NPC_PORTRAIT_H
