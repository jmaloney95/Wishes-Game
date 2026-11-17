#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "event_data.h"
#include "caps.h"
#include "pokemon.h"
#include "test_runner.h"
#include "constants/opponents.h"

// ---------------------------------------------------------------------------
// Wishes of Tomorrow level caps.
//
// The cap is the ace level of the next required boss. Each row is a milestone:
// once any flag in it is set, the cap becomes the row's level. The table is
// read from the bottom up and the LATEST milestone reached wins, so a player
// who takes Act 3's open stretch out of order (the Arena before the Tower, or
// the other way round) never gets a cap lower than where they really are.
//
// Milestones are trainer-defeated flags (and one story flag), so the cap needs
// no save variable and an existing save picks up the right cap on load.
// Optional fights (Siren Allison, Shadow Deoxys, the Dragon Wardens) never
// move it.
// ---------------------------------------------------------------------------
// Red Fatality's Arcanine. Held one under 16, where all three starters evolve,
// so nobody walks into the first gym with an evolved starter.
#define WOT_CAP_START 15

#define TRAINER_FLAG(trainer) (TRAINER_FLAGS_START + (trainer))

struct WotCapMilestone
{
    u16 flags[3]; // any one of these set = passed (0 = unused slot)
    u8 cap;
};

static const struct WotCapMilestone sWotCapMilestones[] =
{
    { { TRAINER_FLAG(TRAINER_ROXANNE_1) }, 21 },                                  // Red Fatality, Ember Badge
    { { TRAINER_FLAG(TRAINER_TUNNEL_CAPTAIN) }, 25 },                             // Captain McCormick, Munen Tunnel
    { { TRAINER_FLAG(TRAINER_DRACO_TORII_ARMALDO),
        TRAINER_FLAG(TRAINER_DRACO_TORII_AURORUS),
        TRAINER_FLAG(TRAINER_DRACO_TORII_TYRANTRUM) }, 32 },                      // Draco at Torii: end of Act 1
    { { TRAINER_FLAG(TRAINER_MADAM_TSUJI) }, 36 },                                // Madame Tsuji, Lantern Badge
    { { TRAINER_FLAG(TRAINER_DISTORTION_LEADER) }, 39 },                          // Vesper, Distortion Badge
    { { TRAINER_FLAG(TRAINER_DRACO_LAB_ARMALDO),
        TRAINER_FLAG(TRAINER_DRACO_LAB_AURORUS),
        TRAINER_FLAG(TRAINER_DRACO_LAB_TYRANTRUM) }, 42 },                        // Draco at Munen Lab
    { { TRAINER_FLAG(TRAINER_SENNEN_CAPTAIN_KANNON) }, 45 },                      // General Edwards: end of Act 2
    { { FLAG_WOT_HIDEOUT_REVEAL_DONE }, 50 },                                     // the Rebel Hideout briefing opens Act 3
    { { TRAINER_FLAG(TRAINER_BLAKE) }, 52 },                                      // the White Tiger, Tower Street
    { { TRAINER_FLAG(TRAINER_SYLVIA) }, 55 },                                     // Suzaku, Tower Mid
    { { TRAINER_FLAG(TRAINER_KATHLEEN) }, 58 },                                   // Mutrid Alpha, Tower Badge
    { { TRAINER_FLAG(TRAINER_WALLY_VR_5) }, MAX_LEVEL },                          // the Oni: after the credits
};

static bool32 WotCapMilestonePassed(const struct WotCapMilestone *milestone)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(milestone->flags); i++)
    {
        if (milestone->flags[i] != 0 && FlagGet(milestone->flags[i]))
            return TRUE;
    }
    return FALSE;
}

// A trainer whose defeat moves the cap: the story's required bosses.
bool32 WotIsLevelCapBoss(u16 trainerId)
{
    u32 i, j;

    for (i = 0; i < ARRAY_COUNT(sWotCapMilestones); i++)
    {
        for (j = 0; j < ARRAY_COUNT(sWotCapMilestones[i].flags); j++)
        {
            if (sWotCapMilestones[i].flags[j] == TRAINER_FLAG(trainerId))
                return TRUE;
        }
    }
    return FALSE;
}

// The cap from story progress alone, ignoring the options toggle.
u32 WotGetMilestoneLevelCap(void)
{
    s32 i;

    for (i = ARRAY_COUNT(sWotCapMilestones) - 1; i >= 0; i--)
    {
        if (WotCapMilestonePassed(&sWotCapMilestones[i]))
            return sWotCapMilestones[i].cap;
    }
    return WOT_CAP_START;
}

bool32 WotLevelCapsActive(void)
{
    if (FlagGet(FLAG_WOT_LEVEL_CAPS_OFF))
        return FALSE;
    // The expansion's own EXP tests are written for an uncapped game. WoT's
    // cap tests call WotGetMilestoneLevelCap directly.
    if (gTestRunnerEnabled)
        return FALSE;
    return WotGetMilestoneLevelCap() < MAX_LEVEL;
}

u32 GetCurrentLevelCap(void)
{
    if (B_LEVEL_CAP_TYPE == LEVEL_CAP_FLAG_LIST)
    {
        if (!WotLevelCapsActive())
            return MAX_LEVEL;
        return WotGetMilestoneLevelCap();
    }
    else if (B_LEVEL_CAP_TYPE == LEVEL_CAP_VARIABLE)
    {
        return VarGet(B_LEVEL_CAP_VARIABLE);
    }

    return MAX_LEVEL;
}

u32 GetSoftLevelCapExpValue(u32 level, u32 expValue)
{
    static const u32 sExpScalingDown[5] = { 4, 8, 16, 32, 64 };
    static const u32 sExpScalingUp[5]   = { 16, 8, 4, 2, 1 };

    u32 levelDifference;
    u32 currentLevelCap = GetCurrentLevelCap();

    if (B_EXP_CAP_TYPE == EXP_CAP_NONE)
        return expValue;

    // No cap in force (switched off in Options, or after the credits): no
    // catch-up bonus either, or every mon under 96 would earn double EXP.
    if (currentLevelCap >= MAX_LEVEL)
        return expValue;

    if (level < currentLevelCap)
    {
        if (B_LEVEL_CAP_EXP_UP)
        {
            levelDifference = currentLevelCap - level;
            if (levelDifference > ARRAY_COUNT(sExpScalingUp) - 1)
                return expValue + (expValue / sExpScalingUp[ARRAY_COUNT(sExpScalingUp) - 1]);
            else
                return expValue + (expValue / sExpScalingUp[levelDifference]);
        }
        else
        {
            return expValue;
        }
    }
    else if (B_EXP_CAP_TYPE == EXP_CAP_HARD)
    {
        return 0;
    }
    else if (B_EXP_CAP_TYPE == EXP_CAP_SOFT)
    {
        levelDifference = level - currentLevelCap;
        if (levelDifference > ARRAY_COUNT(sExpScalingDown) - 1)
            return expValue / sExpScalingDown[ARRAY_COUNT(sExpScalingDown) - 1];
        else
            return expValue / sExpScalingDown[levelDifference];
    }
    else
    {
       return expValue;
    }
}

u32 GetCurrentEVCap(void)
{
    static const u16 sEvCapFlagMap[][2] = {
        // Define EV caps for each milestone
        {FLAG_BADGE01_GET, MAX_TOTAL_EVS *  1 / 17},
        {FLAG_BADGE02_GET, MAX_TOTAL_EVS *  3 / 17},
        {FLAG_BADGE03_GET, MAX_TOTAL_EVS *  5 / 17},
        {FLAG_BADGE04_GET, MAX_TOTAL_EVS *  7 / 17},
        {FLAG_BADGE05_GET, MAX_TOTAL_EVS *  9 / 17},
        {FLAG_BADGE06_GET, MAX_TOTAL_EVS * 11 / 17},
        {FLAG_BADGE07_GET, MAX_TOTAL_EVS * 13 / 17},
        {FLAG_BADGE08_GET, MAX_TOTAL_EVS * 15 / 17},
        {FLAG_IS_CHAMPION, MAX_TOTAL_EVS},
    };

    if (B_EV_CAP_TYPE == EV_CAP_FLAG_LIST)
    {
        for (u32 evCap = 0; evCap < ARRAY_COUNT(sEvCapFlagMap); evCap++)
        {
            if (!FlagGet(sEvCapFlagMap[evCap][0]))
                return sEvCapFlagMap[evCap][1];
        }
    }
    else if (B_EV_CAP_TYPE == EV_CAP_VARIABLE)
    {
        return VarGet(B_EV_CAP_VARIABLE);
    }
    else if (B_EV_CAP_TYPE == EV_CAP_NO_GAIN)
    {
        return 0;
    }

    return MAX_TOTAL_EVS;
}
