#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "m4a.h"
#include "pokemon.h"
#include "sound.h"
#include "wot_film_shiny.h"
#include "wot_mega_dimension.h"
#include "constants/battle.h"
#include "constants/songs.h"

// Wishes of Tomorrow: shiny wild Pokemon on demand, for filming.
// Two debug-menu switches (Scripts > "Shiny: next" / "Shiny: all"):
//   next -- the next wild battle's Pokemon are shiny, then it turns itself off
//   all  -- every wild battle's Pokemon are shiny until switched off again
//
// The mode lives in RAM only, like the Mega Dimension's own debug shiny: no
// flag, no var, nothing in the save, so a power cycle always clears it. It is
// applied as a wild battle starts, after the encounter (random, fishing, Rock
// Smash, DexNav, a scripted or legendary battle, the randomizer's swap) has
// built gEnemyParty, so every kind of wild battle is covered from one place.
// Trainer-owned SHADOW Pokemon are covered too (the snaggable ones, and the
// shiny Shadow art is what gets filmed): battle_main.c's WotRollShadowShiny
// answers yes while a switch is on, and a trainer team holding a Shadow uses
// up "next" (WotFilmShiny_OnTrainerPartyMade). Ordinary trainer Pokemon,
// gifts, eggs and the Battle Frontier are never touched. The Mega Dimension is left out:
// its wild Pokemon decide shininess when they are dealt onto the map (the
// sprite shows it) and keep a record of it; its "Mega Dim: shiny" switch is
// the one to use there.
//
// A third switch, "Catch: 100%", makes every thrown Poke Ball catch (read in
// battle_script_commands.c right after the catch odds are worked out), for
// testing. Same rules: RAM only, off unless chosen. Wherever a ball can be
// thrown it applies, the Mega Dimension and snagging trainer Shadows included.

static EWRAM_DATA u8 sMode = WOT_FILM_SHINY_OFF;
static EWRAM_DATA bool8 sCatchAll = FALSE;

void WotFilmShiny_SetMode(enum WotFilmShinyMode mode)
{
    sMode = mode;
}

enum WotFilmShinyMode WotFilmShiny_GetMode(void)
{
    return sMode;
}

static bool32 IsFilmableWildBattle(void)
{
    if (gBattleTypeFlags & (BATTLE_TYPE_TRAINER | BATTLE_TYPE_CATCH_TUTORIAL | BATTLE_TYPE_GHOST
                          | BATTLE_TYPE_PIKE | BATTLE_TYPE_PYRAMID | BATTLE_TYPE_FRONTIER))
        return FALSE;
    return !WotMegaDim_IsActive();
}

// battle_setup.c CreateBattleStartTask: gBattleTypeFlags and gEnemyParty are
// final by now.
void WotFilmShiny_OnBattleStart(void)
{
    u32 i, shiny = TRUE;

    if (sMode == WOT_FILM_SHINY_OFF || !IsFilmableWildBattle())
        return;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gEnemyParty[i], MON_DATA_SPECIES) != SPECIES_NONE)
            SetMonData(&gEnemyParty[i], MON_DATA_IS_SHINY, &shiny);
    }
    if (sMode == WOT_FILM_SHINY_NEXT)
        sMode = WOT_FILM_SHINY_OFF;
}

// battle_main.c, right after the trainer teams are built (that happens after
// CreateBattleStartTask, so trainers can't use the hook above). Their Shadows
// already came out shiny; a team that had one uses up "next".
void WotFilmShiny_OnTrainerPartyMade(void)
{
    u32 i;

    if (sMode != WOT_FILM_SHINY_NEXT || !(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
        return;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gEnemyParty[i], MON_DATA_IS_SHADOW))
        {
            sMode = WOT_FILM_SHINY_OFF;
            return;
        }
    }
}

// Debug menu specials. VAR_RESULT = whether the switch is now on.
void WotFilmShiny_DebugToggleNext(void)
{
    sMode = (sMode == WOT_FILM_SHINY_NEXT) ? WOT_FILM_SHINY_OFF : WOT_FILM_SHINY_NEXT;
    gSpecialVar_Result = (sMode == WOT_FILM_SHINY_NEXT);
}

void WotFilmShiny_DebugToggleAll(void)
{
    sMode = (sMode == WOT_FILM_SHINY_ALL) ? WOT_FILM_SHINY_OFF : WOT_FILM_SHINY_ALL;
    gSpecialVar_Result = (sMode == WOT_FILM_SHINY_ALL);
}

void WotFilmCatch_Set(bool32 on)
{
    sCatchAll = on;
}

bool32 WotFilmCatch_IsOn(void)
{
    return sCatchAll;
}

// Debug menu special. VAR_RESULT = whether every ball now catches.
void WotFilmCatch_DebugToggle(void)
{
    sCatchAll = !sCatchAll;
    gSpecialVar_Result = sCatchAll;
}

// "Music: off" (map music added in 162): no music anywhere -- map,
// battle, scripted and boss themes, fanfares -- so footage can be recorded
// with only cries, move sounds and other sound effects. m4a.c's
// m4aSongNumStart / m4aSongNumStartOrChange swap any music-player song for
// the silent dummy while this is on. Fanfares keep their timing (silently).
// RAM only, off unless chosen.
static EWRAM_DATA bool8 sMusicOff = FALSE;

bool32 WotFilmMusic_IsOff(void)
{
    return sMusicOff;
}

// Debug menu special. VAR_RESULT = whether music is now off. Silences the
// current song at once; turning it back on restarts the map's music.
void WotFilmMusic_DebugToggle(void)
{
    sMusicOff = !sMusicOff;
    if (sMusicOff)
        m4aSongNumStart(MUS_DUMMY);
    else if (GetCurrentMapMusic() != MUS_DUMMY)
        PlayBGM(GetCurrentMapMusic());
    gSpecialVar_Result = sMusicOff;
}
