#include "global.h"
#include "battle.h"
#include "battle_gimmick.h"
#include "battle_interface.h"
#include "battle_util.h"
#include "main.h"
#include "pokemon.h"
#include "test_runner.h"
#include "wot_permanent_mega.h"
#include "constants/form_change_types.h"

// ---------------------------------------------------------------------------
// Permanent Mega Evolution.
//
// When one of the player's Pokemon Mega Evolves, it stays Mega after the battle
// for as long as it holds its stone. The next battle it comes out already Mega,
// and that counts as the battle's one Mega Evolution: while a Mega like this is
// in the party and able to fight, no other Pokemon can Mega Evolve. It changes
// back when its stone comes off, or when it goes into the PC or the Day Care.
//
// Only the player's own Pokemon. Wild Pokemon, trainers and a partner's team
// revert as they always did. Link, recorded and Frontier battles revert too.
// A Mega that only exists through a move (Rayquaza) holds no stone, so it
// reverts.
// ---------------------------------------------------------------------------

// The expansion's battle tests are written for Megas that revert. WoT's own
// tests switch this on.
bool8 gWotPermanentMegaInTests = FALSE;

static bool32 IsPermanentMegaOn(void)
{
    return !gTestRunnerEnabled || gWotPermanentMegaInTests;
}

// A Mega shares its form table with its base species: the stone is the Mega
// Evolution entry that leads to it, and the base is where the battle's end leads.
u16 WotGetMegaStone(u16 megaSpecies)
{
    const struct FormChange *formChanges = GetSpeciesFormChanges(megaSpecies);
    u32 i;

    if (formChanges == NULL)
        return ITEM_NONE;
    for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM && formChanges[i].targetSpecies == megaSpecies)
            return formChanges[i].param1;
    }
    return ITEM_NONE;
}

u16 WotGetMegaBaseSpecies(u16 megaSpecies)
{
    const struct FormChange *formChanges = GetSpeciesFormChanges(megaSpecies);
    u32 i;

    if (formChanges == NULL)
        return SPECIES_NONE;
    for (i = 0; formChanges[i].method != FORM_CHANGE_TERMINATOR; i++)
    {
        if (formChanges[i].method == FORM_CHANGE_END_BATTLE && formChanges[i].targetSpecies != megaSpecies)
            return formChanges[i].targetSpecies;
    }
    return SPECIES_NONE;
}

bool32 WotIsKeptMega(u16 species, u16 heldItem)
{
    return species < NUM_SPECIES
        && gSpeciesInfo[species].isMegaEvolution
        && heldItem != ITEM_NONE
        && heldItem == WotGetMegaStone(species);
}

// Out of battle (pokemon.c GetFormChangeTargetSpecies_Internal): what a stored
// Mega becomes when an item changes hands, when it's deposited, or when it
// faints from poison in the field. Anything else: unchanged.
u32 WotGetOutOfBattleMegaTarget(u32 method, u16 species, u16 heldItem)
{
    u32 base;

    if (gMain.inBattle || species >= NUM_SPECIES || !gSpeciesInfo[species].isMegaEvolution)
        return species;
    switch (method)
    {
    case FORM_CHANGE_FAINT:
        // It stays Mega, as it does after fainting in battle.
        if (IsPermanentMegaOn())
            return species;
        break;
    case FORM_CHANGE_ITEM_HOLD:
        if (WotIsKeptMega(species, heldItem))
            return species;
        break;
    case FORM_CHANGE_DEPOSIT:
        break;
    default:
        return species;
    }
    base = WotGetMegaBaseSpecies(species);
    return base != SPECIES_NONE ? base : species;
}

static bool32 IsPlayersOwnBattler(u32 battler)
{
    u32 position = GetBattlerPosition(battler);

    return position == B_POSITION_PLAYER_LEFT
        || (!(gBattleTypeFlags & BATTLE_TYPE_MULTI) && position == B_POSITION_PLAYER_RIGHT);
}

static bool32 IsPermanentMegaBattle(void)
{
    return IsPermanentMegaOn()
        && !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED | BATTLE_TYPE_FRONTIER));
}

// battle_util.c CanBattlerFormChange: no reverting when it faints.
bool32 WotKeepsMegaInBattle(u32 battler)
{
    return IsPermanentMegaBattle()
        && IsPlayersOwnBattler(battler)
        && !gBattleMons[battler].volatiles.transformed
        && WotIsKeptMega(gBattleMons[battler].species, GetMonData(&gPlayerParty[gBattlerPartyIndexes[battler]], MON_DATA_HELD_ITEM));
}

// battle_util.c TryRevertPartyMonFormChange, at the end of a battle. Only one
// Mega is ever kept: the one that was Mega this battle, or, if none was, the
// one already stored.
bool32 WotShouldKeepPartyMegaAtEnd(u32 partyIndex)
{
    struct Pokemon *mon = &gPlayerParty[partyIndex];
    u32 i;

    if (!IsPermanentMegaBattle() || partyIndex >= PARTY_SIZE)
        return FALSE;
    if (!WotIsKeptMega(GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HELD_ITEM)))
        return FALSE;
    if (gBattleStruct->gimmick.activeGimmick[B_SIDE_PLAYER][partyIndex] == GIMMICK_MEGA)
        return TRUE;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (i != partyIndex && gBattleStruct->gimmick.activeGimmick[B_SIDE_PLAYER][i] == GIMMICK_MEGA)
            return FALSE;
    }
    return TRUE;
}

// battle_util.c CanMegaEvolve: a stored Mega that can still fight is this
// battle's Mega Evolution, whether or not it's out yet.
bool32 WotIsMegaBlockedByStoredMega(u32 battler)
{
    u32 i;

    if (!IsPermanentMegaBattle() || !IsPlayersOwnBattler(battler))
        return FALSE;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gPlayerParty[i];

        if (i == gBattlerPartyIndexes[battler] || GetMonData(mon, MON_DATA_HP) == 0 || GetMonData(mon, MON_DATA_IS_EGG))
            continue;
        if (WotIsKeptMega(GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HELD_ITEM)))
            return TRUE;
    }
    return FALSE;
}

// battle_util.c, on switch-in: a stored Mega comes out with its Mega Evolution
// already spent, so it shows the Mega icon and counts for the Rogue Core.
void WotTryMarkStoredMega(u32 battler)
{
    if (!IsPermanentMegaBattle() || !IsPlayersOwnBattler(battler) || !IsBattlerMegaEvolved(battler))
        return;
    if (GetActiveGimmick(battler) == GIMMICK_MEGA)
        return;
    SetActiveGimmick(battler, GIMMICK_MEGA);
    SetGimmickAsActivated(battler, GIMMICK_MEGA);
    UpdateIndicatorVisibilityAndType(gHealthboxSpriteIds[battler], FALSE);
}
