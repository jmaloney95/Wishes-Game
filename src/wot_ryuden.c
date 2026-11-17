#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "random.h"
#include "script_pokemon_util.h"
#include "wot_ryuden.h"
#include "constants/species.h"

// Wishes of Tomorrow: the Ryuden Island incubator hands out ONE dragon egg,
// drawn from this pool. It used to be eight hardcoded giveegg branches in the
// map script, most of them legendaries (and two battle/fusion-only forms:
// Ultra Necrozma, White Kyurem). Legendaries are placed elsewhere now, so the
// pool is the pseudo-legendary dragon lines, given as their first stage like
// any real egg. Kept in C so test/wot_ryuden.c checks the exact list the game
// draws from.
const u16 gWotRyudenEggPool[] =
{
    SPECIES_DRATINI,
    SPECIES_BAGON,
    SPECIES_GIBLE,
    SPECIES_DEINO,
    SPECIES_GOOMY,
    SPECIES_JANGMO_O,
    SPECIES_DREEPY,
    SPECIES_FRIGIBAX,
};

const u32 gWotRyudenEggPoolCount = ARRAY_COUNT(gWotRyudenEggPool);

// -> VAR_RESULT = ScriptGiveEgg's MON_GIVEN_TO_PARTY / _PC / MON_CANT_GIVE
//    VAR_0x8004 = the species handed over
void WotGiveRyudenEgg(void)
{
    u16 species = gWotRyudenEggPool[Random() % gWotRyudenEggPoolCount];

    gSpecialVar_0x8004 = species;
    gSpecialVar_Result = ScriptGiveEgg(species);
}
