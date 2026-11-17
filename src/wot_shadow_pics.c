#include "global.h"
#include "wot_shadow_art.h"
#include "constants/species.h"

// The art table. Kept in its own translation unit so the ~46 sprites are
// linked once; battle_gfx_sfx_util.c, pokemon_summary_screen.c and
// hall_of_fame.c all reach it through WotFindShadowPic().
#include "data/wot_shadow_pics.h"

u32 WotShadowPicCount(void)
{
    return ARRAY_COUNT(sWotShadowPics);
}

u16 WotShadowPicSpeciesAt(u32 index)
{
    if (index >= ARRAY_COUNT(sWotShadowPics))
        return SPECIES_NONE;
    return sWotShadowPics[index].species;
}

const u16 *WotShadowPicPal(const struct WotShadowPic *art, bool32 isShiny)
{
    return (isShiny && art->shinyPal != NULL) ? art->shinyPal : art->pal;
}

bool32 WotShadowKeepsOwnArt(u16 species)
{
#if P_FAMILY_DEOXYS && P_MEGA_EVOLUTIONS
    if (species == SPECIES_DEOXYS_MEGA)
        return TRUE;
#endif
    // Tulip's sanctum legend: its Mega art, untinted.
#if P_FAMILY_DARKRAI && P_GEN_9_MEGA_EVOLUTIONS
    if (species == SPECIES_DARKRAI_MEGA)
        return TRUE;
#endif
    return FALSE;
}

const struct WotShadowPic *WotFindShadowPic(u16 species)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sWotShadowPics); i++)
    {
        if (sWotShadowPics[i].species == species)
            return &sWotShadowPics[i];
    }
    return NULL;
}
