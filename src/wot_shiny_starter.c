#include "global.h"
#include "event_data.h"
#include "random.h"
#include "constants/pokemon.h"

// Wishes of Tomorrow: the three starter pedestals in Munen Village each carry
// a one-in-ten chance of being shiny, and the preview portrait shows it, so
// the player picks knowing which of the three (if any) came up.
//
// The roll therefore has to be settled BEFORE the first portrait is drawn and
// then stay settled: showmonpic renders whatever we answer here, so a roll
// made per conversation would let the player re-roll a pedestal simply by
// talking to it again. It is kept in a saved var rather than a VAR_TEMP for
// the same reason -- stepping into a house and back out must not reshuffle
// the three. A soft reset before saving still re-rolls, which is exactly how
// players expect to hunt a starter.

#define WOT_STARTER_SHINY_ODDS  10   // per pedestal, rolled independently

#define STARTER_SHINY(slot)     (1 << (slot))
#define STARTER_SHINY_ROLLED    (1 << 3)

static u16 GetStarterShinyMask(void)
{
    u16 mask = VarGet(VAR_STARTER_SHINY_ROLL);
    u32 slot;

    if (mask & STARTER_SHINY_ROLLED)
        return mask;

    mask = STARTER_SHINY_ROLLED;
    for (slot = 0; slot < 3; slot++)
    {
        if (Random() % WOT_STARTER_SHINY_ODDS == 0)
            mask |= STARTER_SHINY(slot);
    }

    VarSet(VAR_STARTER_SHINY_ROLL, mask);
    return mask;
}

// VAR_0x8004 = starter slot (0/1/2)
//   -> VAR_0x8006 = TRUE/FALSE, for showmonpic's isShiny
//   -> VAR_0x8007 = the ShinyMode givemon should use
//
// Both answers come off the same bit, so the portrait the player is looking at
// is the Pokemon they are handed. The miss case is SHINY_MODE_NEVER rather
// than SHINY_MODE_RANDOM on purpose: a stray 1/256 roll inside givemon would
// contradict the preview that was just on screen.
void WotGetStarterShiny(void)
{
    u32 slot = gSpecialVar_0x8004;
    bool32 shiny;

    if (slot > 2)
        slot = 0;

    shiny = (GetStarterShinyMask() & STARTER_SHINY(slot)) != 0;

    gSpecialVar_0x8006 = shiny;
    gSpecialVar_0x8007 = shiny ? SHINY_MODE_ALWAYS : SHINY_MODE_NEVER;
}
