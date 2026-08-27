#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "random.h"
#include "constants/pokedex.h"
#include "constants/species.h"
#include "battle_setup.h"
#include "constants/opponents.h"
#include "field_screen_effect.h"
#include "script.h"
#include "task.h"
#include "overworld.h"
#include "wot_randomizer.h"

// ============================================================================
//  WISHES OF TOMORROW -- RANDOMIZER MODE
//
//  Chosen once, at New Game (FLAG_WOT_RANDOMIZER). Every wild encounter and
//  every trainer's team is swapped for a different species; LEVELS ARE NEVER
//  TOUCHED, so the difficulty curve of the hack is preserved exactly.
//
//  The substitution is a pure function of (species, save seed):
//    * deterministic  -- the same Zigzagoon is the same replacement every
//                        time you meet one, so the world stays coherent and
//                        nothing has to be written to the save file;
//    * per-save        -- seeded from the player's own trainer id, so two
//                        playthroughs randomize differently;
//    * gen 1-9 only    -- the roll happens in NATIONAL DEX space and is mapped
//                        back with NationalPokedexNumToSpecies(), which yields
//                        base forms only. No megas, no regional forms, no
//                        Shadow duplicates, nothing outside the dex.
//
//  Deliberately NOT randomized (all of these bypass the two hook points):
//  script gifts (givemon), eggs, and static/legendary encounters set up with
//  setwildbattle -- i.e. the starters, the Shadow legendaries and every story
//  Pokemon stay exactly as written.
// ============================================================================

// A very short exemption list. Randomizer players EXPECT bosses to be rolled,
// so this is not "protect the difficulty curve" -- it is only the two battles
// whose species is load-bearing outside the battle itself:
//   * THE ONI          -- the finale; his team is the climax of the story.
//   * Draco's ARMORED MEWTWO -- you are handed that exact species afterwards
//     via givemon, which the randomizer never touches, so rolling the battle
//     would have you fight one thing and receive another.
// Shadow mons are exempted separately and automatically (see battle_main.c):
// the Shadow Log tracks them by species, so rolling them would make the log
// uncompletable and discard their custom battle art.
bool32 WotTrainerIsRandomizerExempt(void)
{
    u32 opponent;

    if (!WotRandomizerActive())
        return TRUE;   // nothing is being randomized anyway

    opponent = TRAINER_BATTLE_PARAM.opponentA;
    if (opponent == TRAINER_WALLY_VR_5 || opponent == TRAINER_WALLY_VR_4)
        return TRUE;

    opponent = TRAINER_BATTLE_PARAM.opponentB;
    if (opponent == TRAINER_WALLY_VR_5 || opponent == TRAINER_WALLY_VR_4)
        return TRUE;

    return FALSE;
}

static u32 WotRandomizerSeed(void)
{
    // The trainer id is already in the save and unique per playthrough.
    return  (u32)gSaveBlock2Ptr->playerTrainerId[0]
         | ((u32)gSaveBlock2Ptr->playerTrainerId[1] << 8)
         | ((u32)gSaveBlock2Ptr->playerTrainerId[2] << 16)
         | ((u32)gSaveBlock2Ptr->playerTrainerId[3] << 24);
}

// ----------------------------------------------------------------------------
//  Deferred commit.
//
//  The settings screen runs BEFORE CB2_NewGame, and NewGameInitData() wipes
//  every flag in the save block (InitEventData -> memset). Anything the menu
//  wrote was therefore erased a moment later, which is why the randomizer
//  appeared to do nothing whichever way it was set. So the choice is ALSO
//  parked here in BSS -- which a new game does not touch -- and re-applied
//  from the tail of NewGameInitData, after the fresh trainer id (our seed)
//  exists. The PC path has no new game to survive and just writes the flags.
// ----------------------------------------------------------------------------

#define QUEUED_MASTER   (1 << 0)
#define QUEUED_STARTERS (1 << 1)
#define QUEUED_WILD     (1 << 2)
#define QUEUED_TRAINERS (1 << 3)

static u8 sQueuedSettings;
static bool8 sHasQueuedSettings;

void WotQueueRandomizerSettings(bool32 master, bool32 starters, bool32 wild, bool32 trainers)
{
    sQueuedSettings = (master   ? QUEUED_MASTER   : 0)
                    | (starters ? QUEUED_STARTERS : 0)
                    | (wild     ? QUEUED_WILD     : 0)
                    | (trainers ? QUEUED_TRAINERS : 0);
    sHasQueuedSettings = TRUE;
}

// Called at the very END of NewGameInitData(). Consumes the queue so a later
// new game cannot inherit a choice the player never made this time round.
void WotApplyQueuedRandomizerSettings(void)
{
    if (!sHasQueuedSettings)
        return;

    sHasQueuedSettings = FALSE;

    if (sQueuedSettings & QUEUED_MASTER)   FlagSet(FLAG_WOT_RANDOMIZER);     else FlagClear(FLAG_WOT_RANDOMIZER);
    if (sQueuedSettings & QUEUED_STARTERS) FlagSet(FLAG_WOT_RAND_STARTERS);  else FlagClear(FLAG_WOT_RAND_STARTERS);
    if (sQueuedSettings & QUEUED_WILD)     FlagSet(FLAG_WOT_RAND_WILD);      else FlagClear(FLAG_WOT_RAND_WILD);
    if (sQueuedSettings & QUEUED_TRAINERS) FlagSet(FLAG_WOT_RAND_TRAINERS);  else FlagClear(FLAG_WOT_RAND_TRAINERS);
}

bool32 WotRandomizerActive(void)
{
    return FlagGet(FLAG_WOT_RANDOMIZER);
}

// Starters come from `givemon` in the Munen script, which bypasses both hook
// points, so they are randomized explicitly through here.
u16 WotRandomizeStarter(u16 species)
{
    if (!WotRandomizerActive() || !FlagGet(FLAG_WOT_RAND_STARTERS))
        return species;
    return WotRandomizeSpecies(species);
}

u16 WotRandomizeSpecies(u16 species)
{
    u32 h;
    u32 tries;

    if (species == SPECIES_NONE || species == SPECIES_EGG)
        return species;
    if (!WotRandomizerActive())
        return species;

    // Cheap avalanche hash of the species against the save seed. Any stable
    // mixer works; this one just needs to scatter neighbouring dex numbers.
    h = (u32)species * 2654435761u;
    h ^= WotRandomizerSeed();
    h ^= h >> 13;
    h *= 0x5BD1E995u;
    h ^= h >> 15;

    // Walk forward until we land on a dex slot that resolves to a real species
    // in this build (P_FAMILY_* switches can leave holes).
    for (tries = 0; tries < 64; tries++)
    {
        u32 dexNum = 1 + ((h + tries * 7919u) % NATIONAL_DEX_COUNT);
        u16 out = NationalPokedexNumToSpecies(dexNum);

        if (out != SPECIES_NONE && gSpeciesInfo[out].baseHP != 0)
            return out;
    }

    return species; // give up rather than hand back something invalid
}

// ---------------------------------------------------------------------------
//  Script glue
// ---------------------------------------------------------------------------

// VAR_0x8004 = starter slot (0/1/2) -> VAR_0x8005 = the species to show and
// give. Both `showmonpic` and `givemon` VarGet their species argument, so the
// mugshot and the Pokemon itself stay in step automatically.
void WotSetStarterSpecies(void)
{
    static const u16 sBaseStarters[3] = { SPECIES_GIBLE, SPECIES_FRIGIBAX, SPECIES_AXEW };
    u16 slot = gSpecialVar_0x8004;

    if (slot > 2)
        slot = 0;

    gSpecialVar_0x8005 = WotRandomizeStarter(sBaseStarters[slot]);
}

// Opens the settings screen from the bedroom PC and returns to the field.
void WotOpenRandomizerMenu(void)
{
    WotStartRandomizerMenu(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}
