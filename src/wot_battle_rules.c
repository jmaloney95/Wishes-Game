#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "caps.h"
#include "data.h"
#include "event_data.h"
#include "test_runner.h"
#include "wot_battle_rules.h"
#include "constants/opponents.h"
#include "constants/weather.h"

// ---------------------------------------------------------------------------
// Boss arena rules.
//
//   * The Distortion World is an Inverse Battle: Vesper and the two rift
//     trainers before her (Silas, Kiyo) fight with the type chart reversed.
//     That is the expansion's own inverse battle, switched by
//     B_FLAG_INVERSE_BATTLE.
//   * The Underground Arena's maglev floor: in Volten's bout every Electric
//     and Steel Pokemon floats, on both sides, so Ground moves miss them.
//     Gravity, Smack Down, Thousand Arrows and an Iron Ball still ground them,
//     exactly like Levitate.
//   * Tower Mid is Suzaku's sun: the battle opens in harsh sunlight, as if the
//     overworld weather were a drought. A Drizzle or Rain Dance replaces it.
//
// The two flags are re-decided at the start of every battle, so a whiteout,
// a run-away or a soft reset can never leave a rule switched on for the next
// fight.
// ---------------------------------------------------------------------------

static bool32 IsTrainerBattleAgainst(u16 trainerId)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
        return FALSE;
    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED | BATTLE_TYPE_FRONTIER))
        return FALSE;
    return TRAINER_BATTLE_PARAM.opponentA == trainerId;
}

// ---------------------------------------------------------------------------
// Semi Shift (Options > BATTLE STYLE > SEMI): the free switch after a KO in
// ordinary trainer battles, none against a boss. A boss is a trainer the level
// caps are built on, or anyone of a boss class: gym leaders, the Elite Four and
// Champion, rivals, team leaders and officers, the Z-A rivals.
// ---------------------------------------------------------------------------
static const u8 sBossTrainerClasses[] =
{
    TRAINER_CLASS_LEADER,
    TRAINER_CLASS_ELITE_FOUR,
    TRAINER_CLASS_CHAMPION,
    TRAINER_CLASS_RIVAL,
    TRAINER_CLASS_MAGMA_LEADER,
    TRAINER_CLASS_AQUA_LEADER,
    TRAINER_CLASS_DRAGON_WARDEN,
    TRAINER_CLASS_GENERAL,
    TRAINER_CLASS_ONI,
    TRAINER_CLASS_CAPTAIN,
    TRAINER_CLASS_SIREN,
    TRAINER_CLASS_STREAMER,
    TRAINER_CLASS_TEAM_MZ,
    TRAINER_CLASS_DETECTIVE,
    TRAINER_CLASS_SOCIALITE,
};

static bool32 IsBossTrainer(u16 trainerId)
{
    u32 i, trainerClass;

    if (trainerId == 0)
        return FALSE;
    if (WotIsLevelCapBoss(trainerId))
        return TRUE;
    if (trainerId >= TRAINERS_COUNT)
        return FALSE;
    trainerClass = GetTrainerClassFromId(trainerId);
    for (i = 0; i < ARRAY_COUNT(sBossTrainerClasses); i++)
    {
        if (sBossTrainerClasses[i] == trainerClass)
            return TRUE;
    }
    return FALSE;
}

bool32 WotIsBossTrainerBattle(void)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
        return FALSE;
    if (gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED | BATTLE_TYPE_FRONTIER))
        return FALSE;
    return IsBossTrainer(TRAINER_BATTLE_PARAM.opponentA) || IsBossTrainer(TRAINER_BATTLE_PARAM.opponentB);
}

// What this battle plays as: the player's SHIFT or SET, or for SEMI, SET
// against a boss and SHIFT against everyone else.
u32 WotGetBattleStyle(void)
{
    if (!gSaveBlock2Ptr->optionsBattleStyleSemi)
        return gSaveBlock2Ptr->optionsBattleStyle;
    return WotIsBossTrainerBattle() ? OPTIONS_BATTLE_STYLE_SET : OPTIONS_BATTLE_STYLE_SHIFT;
}

static bool32 IsDistortionWorldBattle(void)
{
    return IsTrainerBattleAgainst(TRAINER_DISTORTION_LEADER)
        || IsTrainerBattleAgainst(TRAINER_RIFT_WARDEN)
        || IsTrainerBattleAgainst(TRAINER_RIFT_CORE_GUARD);
}

static void SetFlagTo(u32 flag, bool32 value)
{
    if (value)
        FlagSet(flag);
    else
        FlagClear(flag);
}

void WotSetBattleRules(void)
{
    // Battle tests set the rule flags themselves (FLAG_SET in GIVEN).
    if (gTestRunnerEnabled)
        return;

    SetFlagTo(B_FLAG_INVERSE_BATTLE, IsDistortionWorldBattle());
    SetFlagTo(FLAG_WOT_MAGLEV_FLOOR, IsTrainerBattleAgainst(TRAINER_TERRY));
}

u32 WotGetBattleStartWeather(u32 overworldWeather)
{
    if (IsTrainerBattleAgainst(TRAINER_SYLVIA))
        return WEATHER_DROUGHT;
    return overworldWeather;
}

bool32 WotIsFloatedByMaglev(u32 battler)
{
    if (!FlagGet(FLAG_WOT_MAGLEV_FLOOR))
        return FALSE;
    return IS_BATTLER_OF_TYPE(battler, TYPE_ELECTRIC) || IS_BATTLER_OF_TYPE(battler, TYPE_STEEL);
}
