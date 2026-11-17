// ---------------------------------------------------------------------------
// Wishes of Tomorrow: the Frostwood bar's jukebox.
//
// Each use fades into a random field track from the list below, never the one
// already playing. The track is kept as the map's saved music, so it carries
// on after a battle in the bar and after a save and reload there. Leaving the
// bar clears it (Overworld_ClearSavedMusic on every map load), so the bar's
// own music is back next time.
//
// The list is towns, routes and places only: no battle themes (they'd sound
// like a trainer had spotted you), no fanfares or short jingles, and nothing
// spooky.
// ---------------------------------------------------------------------------
#include "global.h"
#include "overworld.h"
#include "random.h"
#include "sound.h"
#include "constants/songs.h"

static const u16 sJukeboxSongs[] =
{
    // Hoenn
    MUS_LITTLEROOT,
    MUS_OLDALE,
    MUS_PETALBURG,
    MUS_RUSTBORO,
    MUS_DEWFORD,
    MUS_SLATEPORT,
    MUS_VERDANTURF,
    MUS_FALLARBOR,
    MUS_FORTREE,
    MUS_LILYCOVE,
    MUS_SOOTOPOLIS,
    MUS_EVER_GRANDE,
    MUS_ROUTE101,
    MUS_ROUTE104,
    MUS_ROUTE110,
    MUS_ROUTE113,
    MUS_ROUTE119,
    MUS_ROUTE120,
    MUS_ROUTE122,
    MUS_SURF,
    MUS_SAILING,
    MUS_CYCLING,
    MUS_GAME_CORNER,
    MUS_SAFARI_ZONE,
    MUS_CABLE_CAR,
    MUS_POKE_CENTER,
    MUS_POKE_MART,
    MUS_CONTEST_LOBBY,
    MUS_B_FRONTIER,
    MUS_GSC_ROUTE38,
    MUS_GSC_PEWTER,
    // Kanto and the Sevii Islands
    MUS_RG_PALLET,
    MUS_RG_ROUTE1,
    MUS_RG_ROUTE3,
    MUS_RG_ROUTE11,
    MUS_RG_ROUTE24,
    MUS_RG_PEWTER,
    MUS_RG_VERMILLION,
    MUS_RG_CELADON,
    MUS_RG_FUCHSIA,
    MUS_RG_CINNABAR,
    MUS_RG_GAME_CORNER,
    MUS_RG_CYCLING,
    MUS_RG_SURF,
    MUS_RG_SS_ANNE,
    MUS_RG_UNION_ROOM,
    MUS_RG_SEVII_ROUTE,
    MUS_RG_SEVII_123,
    MUS_RG_SEVII_45,
    MUS_RG_SEVII_67,
    // Johto, Sinnoh and Unova
    MUS_NATIONAL_PARK,
    MUS_HG_ECRUTEAK,
    MUS_DP_SNOWPOINT_DAY,
    MUS_DP_SUNYSHORE_DAY,
    MUS_DP_ETERNA_FOREST,
    MUS_BW_ANVILLE,
    MUS_BW_CASTELIA,
    MUS_BW_BLACK_CITY,
};

void WotJukebox_PlayRandomSong(void)
{
    u16 current = GetCurrentMapMusic();
    u16 song;

    do
        song = sJukeboxSongs[Random() % ARRAY_COUNT(sJukeboxSongs)];
    while (song == current);

    Overworld_SetSavedMusic(song);
    FadeOutAndPlayNewMapMusic(song, 8);
}
