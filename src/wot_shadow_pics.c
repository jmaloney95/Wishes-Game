#include "global.h"
#include "wot_shadow_art.h"
#include "constants/species.h"

// The art table. Kept in its own translation unit so the ~46 sprites are
// linked once; battle_gfx_sfx_util.c, pokemon_summary_screen.c and
// hall_of_fame.c all reach it through WotFindShadowPic().
#include "data/wot_shadow_pics.h"

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
