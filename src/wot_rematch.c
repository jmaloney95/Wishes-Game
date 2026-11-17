// ---------------------------------------------------------------------------
// Wishes of Tomorrow: trainer rematches.
//
// Talking to a trainer you have already beaten runs EventScript_NoNormalTrainer
// Battle (or the double version). WoT asks WotRematch_CanOffer first; if the
// trainer qualifies, they offer to battle again. The rematch is the same
// trainer id, so it needs no new trainer data (the id pool is full): at party
// creation every Pokemon is raised to the rematch level and evolved to fit.
//
// Who qualifies: trainers that spot you (TRAINER_TYPE_NORMAL / BURIED) whose
// fight is a plain trainerbattle_single or trainerbattle_double. Fights with a
// beaten script carry story logic, so they never repeat. On top of that, the
// level-cap bosses, the deny list below, anyone with a Shadow Pokemon (a second
// snag) and everything inside the Mega Dimension are left out.
//
// The rematch level is the current level cap. With caps switched off, or after
// the credits, it is the level of the player's best Pokemon. The trainer's last
// Pokemon sits at that level and the rest WOT_REMATCH_LEVEL_GAP under it; no
// Pokemon ever drops below its first-battle level. Prize money is unchanged:
// the expansion pays from the authored level, so rematches can't farm cash.
// ---------------------------------------------------------------------------
#include "global.h"
#include "battle.h"
#include "battle_pyramid.h"
#include "battle_setup.h"
#include "caps.h"
#include "data.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "frontier_util.h"
#include "pokemon.h"
#include "string_util.h"
#include "trainer_hill.h"
#include "wot_mega_dimension.h"
#include "wot_rematch.h"
#include "constants/battle_pyramid.h"
#include "constants/battle_setup.h"
#include "constants/characters.h"
#include "constants/event_object_movement.h"
#include "constants/opponents.h"
#include "constants/trainer_types.h"

// The trainer the next battle is a rematch against (TRAINER_NONE = none).
static EWRAM_DATA u16 sWotRematchTrainerId = TRAINER_NONE;

// Story trainers that spot you and use a plain trainerbattle, but are bosses.
static const u16 sWotRematchDenyList[] =
{
    TRAINER_SENNEN_CAPTAIN_RYOKO, // Act 2: Captain Ryoko
    TRAINER_RIFT_WARDEN,          // Silas, the Distortion World warden
    TRAINER_NATE,                 // Shin-Tokyo patrols: rerolled Shadow teams
    TRAINER_MACEY,
    TRAINER_CLIFFORD,
};

static bool32 WotRematch_TrainerHasShadowMon(u16 trainerId)
{
    const struct Trainer *trainer = GetTrainerStructFromId(trainerId);
    const struct TrainerMon *party = trainer->party;
    u32 count = trainer->poolSize != 0 ? trainer->poolSize : trainer->partySize;
    u32 i;

    if (trainer->overrideTrainer != 0)
    {
        const struct Trainer *orig = GetTrainerStructFromId(trainer->overrideTrainer);
        party = orig->party;
        count = orig->poolSize != 0 ? orig->poolSize : orig->partySize;
    }
    if (party == NULL)
        return FALSE;
    for (i = 0; i < count; i++)
    {
        if (party[i].isShadow)
            return TRUE;
    }
    return FALSE;
}

// specialvar: called from the trainer_battle.inc paths for a trainer whose
// defeated flag is already set.
bool8 WotRematch_CanOffer(void)
{
    u16 trainerId = TRAINER_BATTLE_PARAM.opponentA;
    struct ObjectEvent *obj;
    u32 i;

    if (!WOT_REMATCHES)
        return FALSE;
    if (gSelectedObjectEvent >= OBJECT_EVENTS_COUNT)
        return FALSE;
    obj = &gObjectEvents[gSelectedObjectEvent];
    if (!obj->active)
        return FALSE;
    if (obj->trainerType != TRAINER_TYPE_NORMAL && obj->trainerType != TRAINER_TYPE_BURIED)
        return FALSE;
    if (TRAINER_BATTLE_PARAM.mode != TRAINER_BATTLE_SINGLE
     && TRAINER_BATTLE_PARAM.mode != TRAINER_BATTLE_DOUBLE)
        return FALSE;
    if (trainerId == TRAINER_NONE || trainerId >= TRAINERS_COUNT)
        return FALSE;
    if (!HasTrainerBeenFought(trainerId))
        return FALSE;
    if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE || InTrainerHill())
        return FALSE;
    if (WotMegaDim_IsActive())
        return FALSE;
    if (WotIsLevelCapBoss(trainerId))
        return FALSE;
    for (i = 0; i < ARRAY_COUNT(sWotRematchDenyList); i++)
    {
        if (sWotRematchDenyList[i] == trainerId)
            return FALSE;
    }
    if (WotRematch_TrainerHasShadowMon(trainerId))
        return FALSE;
    return TRUE;
}

// special: STR_VAR_1 = "HIKER Clyde"
void WotRematch_BufferName(void)
{
    u16 trainerId = TRAINER_BATTLE_PARAM.opponentA;
    u8 *end = StringCopy(gStringVar1, GetTrainerClassNameFromId(trainerId));

    *end++ = CHAR_SPACE;
    StringCopy(end, GetTrainerNameFromId(trainerId));
}

// special: the battle dotrainerbattle starts next is a rematch.
void WotRematch_Arm(void)
{
    sWotRematchTrainerId = TRAINER_BATTLE_PARAM.opponentA;
}

// Called as every trainer battle ends, won or lost.
void WotRematch_Disarm(void)
{
    sWotRematchTrainerId = TRAINER_NONE;
}

bool32 WotRematch_IsActiveFor(u16 trainerId)
{
    return sWotRematchTrainerId != TRAINER_NONE
        && sWotRematchTrainerId == trainerId
        && (gBattleTypeFlags & BATTLE_TYPE_TRAINER);
}

u32 WotRematch_TargetLevel(void)
{
    u32 cap = GetCurrentLevelCap();
    s32 top;

    if (cap < MAX_LEVEL)
        return cap;
    top = GetHighestLevelInPlayerParty();
    return top > 0 ? (u32)top : MIN_LEVEL;
}

// slot = position in the battle party; the last one is the ace.
u32 WotRematch_ScaleLevel(u32 baseLevel, u32 slot, u32 monsCount)
{
    u32 level = WotRematch_TargetLevel();

    if (slot + 1 < monsCount)
        level = (level > WOT_REMATCH_LEVEL_GAP) ? level - WOT_REMATCH_LEVEL_GAP : MIN_LEVEL;
    if (level < baseLevel)
        level = baseLevel;
    if (level > MAX_LEVEL)
        level = MAX_LEVEL;
    return level;
}

static u32 WotRematch_EvolveOnce(u32 species, u32 level)
{
    const struct Evolution *evos = GetSpeciesEvolutions(species);
    u32 i;

    if (evos == NULL)
        return SPECIES_NONE;
    for (i = 0; evos[i].method != EVOLUTIONS_END; i++)
    {
        u32 target = evos[i].targetSpecies;
        u32 needed;

        if (target == SPECIES_NONE || target == species || !IsSpeciesEnabled(target))
            continue;
        switch (evos[i].method)
        {
        case EVO_LEVEL:
        case EVO_LEVEL_BATTLE_ONLY:
            needed = (evos[i].param != 0) ? evos[i].param : WOT_REMATCH_EVO_LEVEL_CONDITION;
            break;
        case EVO_TRADE:
        case EVO_ITEM:
        case EVO_SCRIPT_TRIGGER:
        case EVO_BATTLE_END:
        case EVO_SPIN:
            needed = WOT_REMATCH_EVO_LEVEL_OTHER;
            break;
        default: // EVO_NONE, EVO_SPLIT_FROM_EVO
            continue;
        }
        if (level >= needed)
            return target;
    }
    return SPECIES_NONE;
}

// The species a Pokemon of this level would have grown into. When a species
// splits (Snorunt, Eevee, Tyrogue), the first branch that fits wins.
u32 WotRematch_EvolveSpecies(u32 species, u32 level)
{
    u32 i;

    for (i = 0; i < 3; i++)
    {
        u32 next = WotRematch_EvolveOnce(species, level);
        if (next == SPECIES_NONE)
            break;
        species = next;
    }
    return species;
}
