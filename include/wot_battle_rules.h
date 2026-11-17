#ifndef GUARD_WOT_BATTLE_RULES_H
#define GUARD_WOT_BATTLE_RULES_H

// Arena rules for Wishes of Tomorrow's boss fights: the battle-field effects a
// trainer's party file can't express on its own. Anything the party file CAN
// express (Trick Room, terrains, Tailwind, hazards) lives there as a
// "Starting Status" instead.

void WotSetBattleRules(void);
u32 WotGetBattleStartWeather(u32 overworldWeather);
bool32 WotIsFloatedByMaglev(u32 battler);
bool32 WotIsBossTrainerBattle(void);
u32 WotGetBattleStyle(void);

#endif // GUARD_WOT_BATTLE_RULES_H
