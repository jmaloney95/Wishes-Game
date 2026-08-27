#include "global.h"
#include "event_data.h"
#include "item.h"
#include "pokemon.h"
#include "string_util.h"
#include "constants/form_change_types.h"
#include "constants/items.h"
#include "constants/species.h"
#include "wot_mega_shop.h"

// No species in the build offers more than two.
#define MAX_MEGA_STONES_PER_SPECIES 2

// ============================================================================
//  WISHES OF TOMORROW -- THE MINISTER'S MEGA STONE TRADE
//
//  The Minister sells the Mega Stone that the player's LEAD Pokemon needs,
//  looked up live from that species' own form-change table. That is the whole
//  point of doing it this way: every Mega Stone in the build is reachable from
//  one NPC, so none of them have to be hidden around the world, and the list
//  stays correct by construction as Megas are added or removed.
// ============================================================================

// Reports the Mega Stone(s) the FIRST party Pokemon can use.
//
//   in  gSpecialVar_0x8004 -- which stone to select (0 or 1)
//   out gSpecialVar_Result -- that stone's item id, or ITEM_NONE if the species
//                             has no Mega at all (the script refuses the sale)
//   out gSpecialVar_0x8006 -- how many stones this species has
//   out gStringVar1        -- the lead Pokemon's species name
//   out gStringVar2/3      -- the first and second stone's names
//
// Six species carry two stones (Charizard/Raichu/Mewtwo X and Y, and the Z
// variants of Absol, Garchomp and Lucario), so the caller is given the count
// and can offer a choice rather than silently handing over the first one.
void WotGetPartyLeaderMegaStone(void)
{
    const struct FormChange *formChanges;
    u16 stones[MAX_MEGA_STONES_PER_SPECIES];
    u16 species;
    u32 count = 0;
    u32 i;

    gSpecialVar_Result = ITEM_NONE;
    gSpecialVar_0x8006 = 0;

    // An egg has no species to trade against, and MON_DATA_SPECIES would still
    // report the mon inside it -- so check the egg flag explicitly.
    if (GetMonData(&gPlayerParty[0], MON_DATA_SPECIES_OR_EGG, NULL) == SPECIES_NONE
     || GetMonData(&gPlayerParty[0], MON_DATA_IS_EGG, NULL))
        return;

    species = GetMonData(&gPlayerParty[0], MON_DATA_SPECIES, NULL);
    StringCopy(gStringVar1, GetSpeciesName(species));

    formChanges = GetSpeciesFormChanges(species);
    for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method != FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM)
            continue;

        stones[count++] = formChanges[i].param1;
        if (count == ARRAY_COUNT(stones))
            break;
    }

    if (count == 0)
        return;

    CopyItemName(stones[0], gStringVar2);
    if (count > 1)
        CopyItemName(stones[1], gStringVar3);

    gSpecialVar_0x8006 = count;
    gSpecialVar_Result = stones[gSpecialVar_0x8004 < count ? gSpecialVar_0x8004 : 0];
}
