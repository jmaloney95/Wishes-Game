#ifndef GUARD_MAP_NAME_POPUP_H
#define GUARD_MAP_NAME_POPUP_H

// Exported type declarations

// Exported RAM declarations

// Exported ROM declarations
void HideMapNamePopUpWindow(void);
void ShowMapNamePopup(void);
void ShowQuestPopup(void);
void ShowQuestToastPopup(const u8 *label);
bool32 MapNamePopupIsActive(void);
#endif //GUARD_MAP_NAME_POPUP_H
