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
// The species a mon should be SHOWN as. A Pokemon carrying its own Mega Stone
// is drawn in its Mega form on the Hall of Fame and on the continue screen --
// both are trophy shots, and the Mega is the shape the player actually finished
// the game with. Nothing about battle is affected: this is presentation only,
// and a mon holding no stone (or a stone that is not its own) is unchanged.
u16 WotGetDisplaySpecies(struct Pokemon *mon)
{
    const struct FormChange *formChanges;
    u16 species, heldItem;
    u32 i;

    // Eggs report SPECIES_EGG and have nothing to evolve into.
    if (GetMonData(mon, MON_DATA_IS_EGG, NULL))
        return GetMonData(mon, MON_DATA_SPECIES_OR_EGG, NULL);

    species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, NULL);
    if (heldItem == ITEM_NONE)
        return species;

    // NULL for any species with no form-change table -- which is MOST of them.
    // Without this the champion roll and the continue screen dereference NULL
    // for any party member holding an item that is not a Mega Stone.
    formChanges = GetSpeciesFormChanges(species);
    if (formChanges == NULL)
        return species;

    for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM
         && formChanges[i].param1 == heldItem)
            return formChanges[i].targetSpecies;
    }

    return species;
}

// MUST return u16, not void: `specialvar` assigns this function's RETURN
// VALUE to the script variable (`*ptr = gSpecials[index]()`), and SpecialFunc
// is `u16 (*)(void)`. Declared void, the script received a leftover register
// value -- which is how a Pokemon with no Mega still got an offer, and how a
// garbage id reached bufferitemname and additem.
// gSpecialVar_Result is still set as well, for any plain `special` caller.
u16 WotGetPartyLeaderMegaStone(void)
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
        return ITEM_NONE;

    species = GetMonData(&gPlayerParty[0], MON_DATA_SPECIES, NULL);
    StringCopy(gStringVar1, GetSpeciesName(species));

    // NOT GetSpeciesFormChanges(): that helper never reports "this species has
    // no form changes". When the table is NULL it quietly substitutes
    // gSpeciesInfo[SPECIES_NONE].formChangeTable, so checking its result for
    // NULL does not answer the question we are actually asking. Read the
    // species' own pointer, where NULL genuinely means "no Mega, no stone".
    formChanges = gSpeciesInfo[SanitizeSpeciesId(species)].formChangeTable;
    if (formChanges == NULL)
        return ITEM_NONE;

    for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        u16 stone;

        if (formChanges[i].method != FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM)
            continue;

        // Never hand the script an id it cannot buy. VAR_TEMP_1 goes straight
        // to additem, and additem indexing past the item table is a hard crash
        // -- a bad id here shows up first as a blank name in the offer and then
        // as a blue screen the moment the player accepts.
        stone = formChanges[i].param1;
        if (stone == ITEM_NONE || stone >= ITEMS_COUNT)
            continue;

        stones[count++] = stone;
        if (count == ARRAY_COUNT(stones))
            break;
    }

    if (count == 0)
        return ITEM_NONE;

    CopyItemName(stones[0], gStringVar2);
    if (count > 1)
        CopyItemName(stones[1], gStringVar3);

    gSpecialVar_0x8006 = count;
    gSpecialVar_Result = stones[gSpecialVar_0x8004 < count ? gSpecialVar_0x8004 : 0];
    return gSpecialVar_Result;
}

// Buy the stone the script resolved, with the id validated on this side of the
// fence. VAR_0x8005 carries it in; VAR_RESULT comes back TRUE only if the stone
// is real AND it actually reached the bag.
//
// This exists because `additem VAR_TEMP_1, 1` in script cannot check anything:
// it passes the variable straight through, and an id past the end of the item
// table takes the game down. Buying here also re-buffers the item name, so the
// name and the item can never disagree.
u16 WotBuyLeaderMegaStone(void)
{
    u16 stone = gSpecialVar_0x8005;

    gSpecialVar_Result = FALSE;

    if (stone == ITEM_NONE || stone >= ITEMS_COUNT)
        return FALSE;
    if (!CheckBagHasSpace(stone, 1))
        return FALSE;
    if (!AddBagItem(stone, 1))
        return FALSE;

    CopyItemName(stone, gStringVar2);
    gSpecialVar_Result = TRUE;
    return TRUE;
}
