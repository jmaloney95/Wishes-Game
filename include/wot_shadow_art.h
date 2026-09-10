#ifndef GUARD_WOT_SHADOW_ART_H
#define GUARD_WOT_SHADOW_ART_H

// Wishes of Tomorrow: per-species SHADOW front art (raw uncompressed 64x64
// 4bpp + its own 16-colour palette), shared by the battle, the summary screen
// and the Hall of Fame. The table itself lives in src/wot_shadow_pics.c so the
// art is linked exactly once -- including the data header in three places
// would have put three copies of ~46 sprites in the ROM.
//
// Species with no entry keep the engine's violet tint in battle and their
// ordinary art everywhere else.

struct WotShadowPic
{
    u16 species;
    const u32 *pic;
    const u16 *pal;
};

const struct WotShadowPic *WotFindShadowPic(u16 species);

#endif // GUARD_WOT_SHADOW_ART_H
