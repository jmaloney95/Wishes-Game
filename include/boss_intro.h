#ifndef GUARD_BOSS_INTRO_H
#define GUARD_BOSS_INTRO_H

struct ScriptContext;

extern bool8 gWotBossIntroPrimed;

void BossIntro_Start(u32 bossId);
void BossIntro_StartFromScript(struct ScriptContext *ctx);

#endif // GUARD_BOSS_INTRO_H
