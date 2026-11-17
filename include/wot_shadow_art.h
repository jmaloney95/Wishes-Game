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
    const u16 *shinyPal; // gold aura, else hue shift, else the official shiny shift
};

const struct WotShadowPic *WotFindShadowPic(u16 species);
// The palette to show the art with: the shiny one for a shiny Shadow.
const u16 *WotShadowPicPal(const struct WotShadowPic *art, bool32 isShiny);

// A form whose own art is what its Shadow shows in battle, as drawn: no Shadow
// art and no violet tint. Mega Deoxys, which the Prototype Lab's
// Shadow Deoxys Mega Evolves into (shiny or not -- the Mega has no shiny art).
// The mon stays a Shadow; only the picture is left alone.
bool32 WotShadowKeepsOwnArt(u16 species);

// The table as a list, for anything that wants "every species with custom
// Shadow art" (the debug menu's box filler does).
u32 WotShadowPicCount(void);
u16 WotShadowPicSpeciesAt(u32 index);

#endif // GUARD_WOT_SHADOW_ART_H
