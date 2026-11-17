#ifndef GUARD_WOT_FILM_SHINY_H
#define GUARD_WOT_FILM_SHINY_H

// Debug: shiny wild Pokemon on demand, for filming (see wot_film_shiny.c).
enum WotFilmShinyMode
{
    WOT_FILM_SHINY_OFF,
    WOT_FILM_SHINY_NEXT, // the next wild battle only
    WOT_FILM_SHINY_ALL,  // every wild battle until turned off
};

void WotFilmShiny_SetMode(enum WotFilmShinyMode mode);
enum WotFilmShinyMode WotFilmShiny_GetMode(void);
void WotFilmShiny_OnBattleStart(void);
void WotFilmShiny_OnTrainerPartyMade(void);
void WotFilmShiny_DebugToggleNext(void);
void WotFilmShiny_DebugToggleAll(void);

// Debug: every Poke Ball catches, for testing.
void WotFilmCatch_Set(bool32 on);
bool32 WotFilmCatch_IsOn(void);
void WotFilmCatch_DebugToggle(void);

// Debug: no music anywhere, for recording clean audio.
bool32 WotFilmMusic_IsOff(void);
void WotFilmMusic_DebugToggle(void);

#endif // GUARD_WOT_FILM_SHINY_H
