#ifndef GUARD_WOT_SURF_MON_H
#define GUARD_WOT_SURF_MON_H

struct Pokemon *WotGetSurfMon(void);
bool32 WotHasSurfSprite(u16 species);
u8 WotCreateSurfMonSprite(s16 x, s16 y);
s16 WotSurfMonSpriteYOffset(struct Sprite *sprite);

#endif // GUARD_WOT_SURF_MON_H
