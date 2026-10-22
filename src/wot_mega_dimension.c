#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "fieldmap.h"
#include "battle.h"
#include "battle_interface.h"
#include "battle_setup.h"
#include "item.h"
#include "map_name_popup.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "random.h"
#include "rtc.h"
#include "script.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "wild_encounter.h"
#include "window.h"
#include "wot_randomizer.h"
#include "constants/event_objects.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "wot_mega_dimension.h"

// ============================================================================
//  WISHES OF TOMORROW -- THE MEGA DIMENSION
//
//  Once Mega Doomsday has begun, portals open at 8:00 AM each in-game day:
//  every free record (up to three) gets a portal on a free spot from
//  sPortalSpots, so no two ever share a spot. An unentered portal stays where
//  it is until its dimension collapses.
//
//  Each open portal is one record: its spot, its dimension copy (a random one
//  from sDimMaps -- same layout as a real map, its own header with no story
//  events), its rogue Mega or the rival who got there first, and a seed that
//  fixes the exit-portal and Mega slots and the wild Pokemon dealt. So every
//  trip through the same portal finds the same dimension; only the shiny odds
//  are rolled again (a shiny found stays shiny, and a Pokemon beaten or caught
//  stays gone, for as long as the portal lives: the record's mask flags).
//
//  The record's countdown only runs while the player is inside. The exit
//  portal leaves it open (each trip back in costs MEGA_DIM_REENTRY_PENALTY
//  seconds); the timer running out, a whiteout, or the rogue Mega caught or
//  KO'd (or its rival beaten) collapses it: the player lands back in front of
//  the portal, which is gone.
//
//  The grayscale itself is applied where palettes load (fieldmap.c for the
//  tilesets, field_weather.c for sprites); this file only answers "are we in
//  the dimension" and "is this the player's palette".
//
//  Adding a portal spot = one row in sPortalSpots + its own hide flag + a
//  portal object on that map using it (script MegaDim_EventScript_Portal).
//  Adding a destination = one row in sDimMaps (+ a map that shares the
//  source map's layout, see data/maps/MegaDimension_Route2).
// ============================================================================

#define MEGA_DIM_RECORDS        3
#define RECORD_NONE             3

// VAR_WOT_MEGA_DIM_STATE
#define STATE_SESSION_SHIFT     11      // bits 12-11: the record the player is inside (RECORD_NONE = none)
#define STATE_SESSION_MASK      0x3
#define STATE_DAY_SHIFT         7       // bits 10-7: the portal day portals last spawned, mod 16
#define STATE_DAY_MASK          0xF
#define TIMER_MASK              0x7F    // a record's seconds left (record 2's: STATE bits 6-0)
#define TIMER_1_SHIFT           7       // VAR_WOT_MEGA_DIM_TIMERS: record 0 bits 6-0, record 1 bits 13-7

// VAR_WOT_MEGA_DIM_PORTAL_0..2
#define PORTAL_OPEN             (1 << 15)
#define PORTAL_SPOT_SHIFT       12
#define PORTAL_SPOT_MASK        0x7
#define PORTAL_MAP_SHIFT        9
#define PORTAL_MAP_MASK         0x7
#define PORTAL_MEGA_SHIFT       4
#define PORTAL_MEGA_MASK        0x1F
#define PORTAL_ENTERED          (1 << 3)
#define PORTAL_RIVAL_MASK       0x7     // 0 = the Mega waits, 1-6 = sRivals[n - 1] got there first

// VAR_WOT_MEGA_DIM_SEEDS
#define SEED_BITS               5
#define SEED_MASK               0x1F

#define DIM_SLOT_COUNT      3

// Seconds off a portal's clock each time the player goes back in, so leaving
// and re-entering to re-roll the shinies can't go on forever. A portal with no
// more than this left fizzles out when touched.
#define MEGA_DIM_REENTRY_PENALTY 5

STATIC_ASSERT(MEGA_DIM_TIME_LIMIT <= TIMER_MASK, MegaDimTimerFits);

// About one portal in five, a Legends Z-A character has beaten the player to
// the Mega and battles for it.
#define MEGA_DIM_RIVAL_CHANCE   5
// A rival fields three: two of their own, then the Mega as their ace (the 3rd
// mon in their trainers.party block), always their highest level.
#define MEGA_DIM_RIVAL_ACE_SLOT  2
#define MEGA_DIM_RIVAL_TEAM_BELOW_MIN 2  // their own two: 2-4 under the ace
#define MEGA_DIM_RIVAL_TEAM_BELOW_MAX 4

// The rogue Mega (a rival's ace, Mable's legends) is 12-16 levels under the
// party's best, with perfect IVs (Joe, round 128: 5-8 under let a Lv92 Mega
// Kabutops beat a Lv100 Mewtwo). The dimension's wild Pokemon are their lines'
// first stages at about half the party's best (up to 2 under it).
#define MEGA_DIM_BOSS_BELOW_MIN 12
#define MEGA_DIM_BOSS_BELOW_MAX 16
#define MEGA_DIM_LEGEND_LEVEL   70  // Mable's legends ignore the window (Joe, round 129)
#define MEGA_DIM_WILD_SPREAD    2

struct MegaDimMega
{
    u16 baseSpecies;
    u16 megaSpecies;
    u16 stone;
};

struct MegaDimSpot
{
    u16 seenFlag;   // the spot only spawns portals once this is set (0 = always)
    u16 hideFlag;   // its portal object's flag
    u16 srcMap;     // MAP_* the portal stands on
    s8 x, y;        // the portal's tile; the player comes back out at (x, y + 1)
    const u8 *town; // what Lida calls it when a portal opens here
};

struct MegaDimSlot
{
    s8 x, y;
    u8 elevation;
};

struct MegaDimMap
{
    u16 seenFlag;   // only travelled to once its real map has been visited (0 = always)
    u16 map;        // MAP_* of the dimension copy
    struct MegaDimSlot slots[DIM_SLOT_COUNT]; // exit portal + Mega, two of these per portal
};

struct MegaDimSeenMap
{
    u16 flag;
    u16 map;
};

// The 19 fan Megas (round 113). The order is the claim-flag order:
// FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + index.
static const struct MegaDimMega sMegaRoster[] =
{
    {SPECIES_AMOONGUSS,  SPECIES_AMOONGUSS_MEGA,  ITEM_AMOONGUSSITE},
    {SPECIES_AZUMARILL,  SPECIES_AZUMARILL_MEGA,  ITEM_AZUMARILLITE},
    {SPECIES_BUTTERFREE, SPECIES_BUTTERFREE_MEGA, ITEM_BUTTERFRITE},
    {SPECIES_CONKELDURR, SPECIES_CONKELDURR_MEGA, ITEM_CONKELDURRITE},
    {SPECIES_DUGTRIO,    SPECIES_DUGTRIO_MEGA,    ITEM_DUGTRIONITE},
    {SPECIES_ELECTIVIRE, SPECIES_ELECTIVIRE_MEGA, ITEM_ELECTIVIRITE},
    {SPECIES_FLYGON,     SPECIES_FLYGON_MEGA,     ITEM_FLYGONITE},
    {SPECIES_GOLEM,      SPECIES_GOLEM_MEGA,      ITEM_GOLEMITE},
    {SPECIES_JUMPLUFF,   SPECIES_JUMPLUFF_MEGA,   ITEM_JUMPLUFFITE},
    {SPECIES_KABUTOPS,   SPECIES_KABUTOPS_MEGA,   ITEM_KABUTOPSITE},
    {SPECIES_MAGMORTAR,  SPECIES_MAGMORTAR_MEGA,  ITEM_MAGMORTARITE},
    {SPECIES_MILOTIC,    SPECIES_MILOTIC_MEGA,    ITEM_MILOTICITE},
    {SPECIES_OMASTAR,    SPECIES_OMASTAR_MEGA,    ITEM_OMASTARITE},
    {SPECIES_POLIWRATH,  SPECIES_POLIWRATH_MEGA,  ITEM_POLIWRATHITE},
    {SPECIES_SHIFTRY,    SPECIES_SHIFTRY_MEGA,    ITEM_SHIFTRITE},
    {SPECIES_SNORLAX,    SPECIES_SNORLAX_MEGA,    ITEM_SNORLAXITE},
    {SPECIES_TORKOAL,    SPECIES_TORKOAL_MEGA,    ITEM_TORKOALITE},
    {SPECIES_TYPHLOSION, SPECIES_TYPHLOSION_MEGA, ITEM_TYPHLOSIONITE},
    {SPECIES_WHIMSICOTT, SPECIES_WHIMSICOTT_MEGA, ITEM_WHIMSICOTTITE},
};

STATIC_ASSERT(ARRAY_COUNT(sMegaRoster) == FLAG_WOT_MEGA_CLAIMED_WHIMSICOTT - FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + 1, MegaRosterMatchesClaimFlags);
STATIC_ASSERT(ARRAY_COUNT(sMegaRoster) <= PORTAL_MEGA_MASK + 1, MegaRosterFitsRecord);

static const u8 sTown_TradewindTown[] = _("Tradewind Town");
static const u8 sTown_NationalPark[] = _("National Park");
static const u8 sTown_Ashlands[] = _("Ashlands");
static const u8 sTown_ShinTokyo[] = _("Shin Tokyo");

// Where portals open (Joe, round 127). Keep x/y in step with the portal
// objects in the source maps' map.json if they're moved in porymap; each tile
// and the one south of it were checked walkable.
//
// A spot only takes portals after the player has been to its map (seenFlag,
// set on entry), so a portal never opens somewhere the player can't reach
// yet -- Shin Tokyo is Act 3.
static const struct MegaDimSpot sPortalSpots[] =
{
    {FLAG_WOT_MEGA_DIM_SEEN_TRADEWIND,  FLAG_HIDE_MEGA_PORTAL_0, MAP_TRADEWIND_TOWN,      61, 24, sTown_TradewindTown},
    {FLAG_WOT_MEGA_DIM_SEEN_PARK_ACT2,  FLAG_HIDE_MEGA_PORTAL_1, MAP_NATIONAL_PARK_ACT2,  50, 38, sTown_NationalPark},
    {FLAG_WOT_MEGA_DIM_SEEN_PARK_ACT2,  FLAG_HIDE_MEGA_PORTAL_2, MAP_NATIONAL_PARK_ACT2,  10, 47, sTown_NationalPark},
    {FLAG_WOT_MEGA_DIM_SEEN_ASHLANDS,   FLAG_HIDE_MEGA_PORTAL_3, MAP_ASHLANDS,            19, 15, sTown_Ashlands},
    {FLAG_WOT_MEGA_DIM_SEEN_SHIN_TOKYO, FLAG_HIDE_MEGA_PORTAL_4, MAP_SHIN_TOKYO,          46, 24, sTown_ShinTokyo},
    {FLAG_WOT_MEGA_DIM_SEEN_SHIN_TOKYO, FLAG_HIDE_MEGA_PORTAL_5, MAP_SHIN_TOKYO,          55, 43, sTown_ShinTokyo},
    {FLAG_WOT_MEGA_DIM_SEEN_SHIN_TOKYO, FLAG_HIDE_MEGA_PORTAL_6, MAP_SHIN_TOKYO,          12, 18, sTown_ShinTokyo},
};

STATIC_ASSERT(ARRAY_COUNT(sPortalSpots) <= PORTAL_SPOT_MASK + 1, MegaPortalSpotsFitRecord);

#define DIM_MAP_STAR_SUMMIT   5
#define DIM_RANDOM_MAP_COUNT  7
#define DIM_MAP_MARINE_CAVE   7
#define DIM_MAP_SKY_PILLAR    8

// The dimension copies a portal can lead to, each with three slots. Each
// portal gives the exit portal one slot and the Mega another; the player
// arrives one tile south of the exit portal. Every slot was checked to be
// walkable with walkable ground south of it, off the source map's warps and
// triggers, and reachable on foot from the other two (scratch slots.py).
// A copy is only travelled to once its real map has been visited.
static const struct MegaDimMap sDimMaps[] =
{
    {0,                                    MAP_MEGA_DIMENSION_ROUTE_2,        {{24, 10, 3}, {28, 28, 3}, {13, 14, 3}}},
    {FLAG_WOT_MEGA_DIM_SEEN_NATIONAL_PARK, MAP_MEGA_DIMENSION_NATIONAL_PARK,  {{50, 38, 3}, {40, 51, 3}, {30, 39, 3}}},
    {FLAG_WOT_MEGA_DIM_SEEN_TRADEWIND,     MAP_MEGA_DIMENSION_TRADEWIND_TOWN, {{46, 13, 3}, {38, 43, 3}, {30, 40, 3}}},
    {FLAG_WOT_MEGA_DIM_SEEN_SHIN_TOKYO,    MAP_MEGA_DIMENSION_SHIN_TOKYO,     {{45, 25, 3}, {54, 42, 3}, {10, 41, 3}}},
    {FLAG_WOT_MEGA_DIM_SEEN_ASHLANDS,      MAP_MEGA_DIMENSION_ASHLANDS,       {{19, 15, 3}, { 5,  6, 3}, { 9, 20, 3}}},
    {FLAG_WOT_MEGA_DIM_SEEN_STAR_SUMMIT,   MAP_MEGA_DIMENSION_STAR_SUMMIT,    {{10, 17, 5}, {15, 15, 5}, {25,  9, 5}}},
    {FLAG_WOT_MEGA_DIM_SEEN_WALNUT_WOODS,  MAP_MEGA_DIMENSION_WALNUT_WOODS,   {{30, 20, 3}, { 9, 15, 3}, {40, 28, 3}}},
    // Legend-only copies (never in the random pool): slot 0 = the exit portal,
    // slot 1 = where the original legend stood. Primal Kyogre waits in Marine
    // Cave's pool, Mega Rayquaza on Sky Pillar's summit; the trigger in front
    // of each starts the battle (their map scripts).
    [DIM_MAP_MARINE_CAVE] = {0, MAP_MEGA_DIMENSION_MARINE_CAVE, {{12,  3, 3}, { 9, 22, 1}, {13, 26, 3}}},
    [DIM_MAP_SKY_PILLAR]  = {0, MAP_MEGA_DIMENSION_SKY_PILLAR,  {{14, 15, 3}, {14,  7, 3}, {16, 15, 3}}},
};

// Only the first DIM_RANDOM_MAP_COUNT are dealt to rogue Mega portals (the
// record keeps that index); a legend's map comes from sLegends.
STATIC_ASSERT(DIM_RANDOM_MAP_COUNT <= PORTAL_MAP_MASK + 1, MegaDimMapsFitRecord);

// Setting foot on one of these maps marks it visited (portal spots and
// dimension copies both go by these flags).
static const struct MegaDimSeenMap sSeenMaps[] =
{
    {FLAG_WOT_MEGA_DIM_SEEN_NATIONAL_PARK, MAP_ROUTE_2_2},
    {FLAG_WOT_MEGA_DIM_SEEN_PARK_ACT2,     MAP_NATIONAL_PARK_ACT2},
    {FLAG_WOT_MEGA_DIM_SEEN_TRADEWIND,     MAP_TRADEWIND_TOWN},
    {FLAG_WOT_MEGA_DIM_SEEN_SHIN_TOKYO,    MAP_SHIN_TOKYO},
    {FLAG_WOT_MEGA_DIM_SEEN_ASHLANDS,      MAP_ASHLANDS},
    {FLAG_WOT_MEGA_DIM_SEEN_STAR_SUMMIT,   MAP_STAR_SUMMIT_2},
    {FLAG_WOT_MEGA_DIM_SEEN_WALNUT_WOODS,  MAP_WALNUT_WOODS},
};

struct MegaDimRival
{
    u16 trainerId;
    u16 graphicsId;
    u16 music;      // each rival battles to a theme of their own (Joe, round 128)
};

// The order is the script's switch order (MegaDim_EventScript_Rival).
static const struct MegaDimRival sRivals[] =
{
    {TRAINER_MEGADIM_CANARI,   OBJ_EVENT_GFX_ZA_CANARI,    MUS_HG_VS_RIVAL},
    {TRAINER_MEGADIM_LIDA,     OBJ_EVENT_GFX_ZA_LIDA,      MUS_DP_VS_GALACTIC},
    {TRAINER_MEGADIM_EMMA,     OBJ_EVENT_GFX_ZA_EMMA,      MUS_DP_VS_GALACTIC_BOSS},
    {TRAINER_MEGADIM_TAUNIE,   OBJ_EVENT_GFX_ZA_TAUNIE,    MUS_DP_VS_GYM_LEADER},
    {TRAINER_MEGADIM_TULIP,    OBJ_EVENT_GFX_ZA_TULIP,     MUS_VS_FRONTIER_BRAIN},
    {TRAINER_MEGADIM_JACINTHE, OBJ_EVENT_GFX_WOT_JACINTHE, MUS_DP_VS_CHAMPION},
};

STATIC_ASSERT(ARRAY_COUNT(sRivals) <= PORTAL_RIVAL_MASK, MegaDimRivalsFitRecord);

// --- Mable's quest chain ------------------------------------------------------
// Two Minutes Is a Long Time: report the stones of all 19 rogue Megas to her
// (she only counts the dimension's own claim flags). The day after her tests
// start she calls about two Primal signatures (Primal Signatures); once both
// are caught she calls about Mega Rayquaza (Sky-High Mega). Each legend waits
// in a portal, as the boss, while its quest is on and until it's caught.
enum
{
    MABLE_STAGE_TALLY,          // collecting the 19 stones
    MABLE_STAGE_TESTS,          // all 19 in: her Primal call comes on a later portal day
    MABLE_STAGE_PRIMALS,        // Primal Signatures: catch Primal Groudon and Primal Kyogre
    MABLE_STAGE_RAYQUAZA_CALL,  // both caught: her Rayquaza call is due
    MABLE_STAGE_RAYQUAZA,       // Sky-High Mega: catch Mega Rayquaza
    MABLE_STAGE_DONE,
};

#define MABLE_TALLY_BITS 5
#define MABLE_STAGE_BITS 3
#define MABLE_DAY_BITS   4

struct MegaDimLegend
{
    u16 species;        // what is caught
    u16 formSpecies;    // what the overworld shows and the battle becomes
    u16 item;           // Groudon/Kyogre revert on switch-in holding their orb
    u16 move;           // forced into slot 1: Mega Rayquaza needs Dragon Ascent
    u16 caughtFlag;
    u8 stage;           // the Mable stage whose quest calls for it
    u8 dimMap;          // where its portal always leads (sDimMaps index)
};

// Where each legend waits (Joe, round 130): Groudon on the Star Summit copy,
// Kyogre in a gray Marine Cave, Rayquaza atop a gray Sky Pillar. No other
// Pokemon appear beside a legend.
static const struct MegaDimLegend sLegends[] =
{
    {SPECIES_GROUDON,  SPECIES_GROUDON_PRIMAL, ITEM_RED_ORB,  MOVE_NONE,          FLAG_WOT_CAUGHT_PRIMAL_GROUDON, MABLE_STAGE_PRIMALS,  DIM_MAP_STAR_SUMMIT},
    {SPECIES_KYOGRE,   SPECIES_KYOGRE_PRIMAL,  ITEM_BLUE_ORB, MOVE_NONE,          FLAG_WOT_CAUGHT_PRIMAL_KYOGRE,  MABLE_STAGE_PRIMALS,  DIM_MAP_MARINE_CAVE},
    {SPECIES_RAYQUAZA, SPECIES_RAYQUAZA_MEGA,  ITEM_NONE,     MOVE_DRAGON_ASCENT, FLAG_WOT_CAUGHT_MEGA_RAYQUAZA,  MABLE_STAGE_RAYQUAZA, DIM_MAP_SKY_PILLAR},
};

// A record's boss: 0-18 = sMegaRoster, BOSS_LEGEND_FIRST + n = sLegends[n].
#define BOSS_LEGEND_FIRST   ARRAY_COUNT(sMegaRoster)
#define BOSS_COUNT          (BOSS_LEGEND_FIRST + ARRAY_COUNT(sLegends))

STATIC_ASSERT(BOSS_COUNT <= PORTAL_MEGA_MASK + 1, MegaDimBossesFitRecord);

// --- Visible wild Pokemon -----------------------------------------------------
// The placeholder objects in each dimension map (script
// MegaDim_EventScript_WildMon, each with its own hide flag from FLAG_TEMP_3 up)
// stand where they were placed in porymap. Every MEGA_DIM_MON_PER_AREA of them,
// in map order, form one group; each portal fills MEGA_DIM_VISIT_SPECIES
// groups with one species apiece and hides the rest.
extern const u8 MegaDim_EventScript_WildMon[];

#define MEGA_DIM_MON_PER_AREA     3
#define MEGA_DIM_MON_FIRST_FLAG   FLAG_TEMP_3
#define MEGA_DIM_MON_MAX          (FLAG_TEMP_1F - FLAG_TEMP_3 + 1)
#define MEGA_DIM_VISIT_SPECIES    3   // species per visit: keeps OW sheets in VRAM (memory: ow-sheet-vram-gotcha)
#define MEGA_DIM_MAX_SHINY_SHEETS 2   // distinct shiny sheets per visit, same reason
#define MEGA_DIM_LEGEND_SPECIES   2   // species per visit beside a legend boss (its sheet is 64x64)
#define MEGA_DIM_DEALT_MAX        (MEGA_DIM_VISIT_SPECIES * MEGA_DIM_MON_PER_AREA)

// A record's mask flags (FLAG_WOT_MEGA_DIM_MASKS_START + record * MASK_FLAGS_PER_RECORD + bit).
// Dealt Pokemon k = its group's place in the deal * MEGA_DIM_MON_PER_AREA + its place in the group.
#define MASK_GONE(k)            (k)
#define MASK_SHINY(k)           (MEGA_DIM_DEALT_MAX + (k))
#define MASK_MEGA_SHINY         (2 * MEGA_DIM_DEALT_MAX)
#define MASK_FLAGS_PER_RECORD   (MASK_MEGA_SHINY + 1)

STATIC_ASSERT(FLAG_WOT_MEGA_DIM_MASKS_START + MEGA_DIM_RECORDS * MASK_FLAGS_PER_RECORD - 1 <= FLAG_UNUSED_0x91D, MegaDimMasksFitFreeFlags);

static EWRAM_DATA u8 sForceRival = 0; // debug: the next entry's rival (1-6), 0 = the portal's own
static EWRAM_DATA u8 sDebugNextRival = 0;
static EWRAM_DATA bool8 sForceShiny = FALSE; // debug: the next deal's first Pokemon is shiny

static EWRAM_DATA u8 sLastPlaySecond = 0;
static EWRAM_DATA bool8 sForceOnFoot = FALSE;

// overworld.c GetAdjustedInitialTransitionFlags: every warp into or out of
// the dimension lands the player on foot, even on a water tile (the National
// Park portal and its return tile are pond water). One-shot.
bool32 WotMegaDim_TakeForceOnFoot(void)
{
    bool32 force = sForceOnFoot;

    sForceOnFoot = FALSE;
    return force;
}

bool32 WotMegaDim_IsActive(void)
{
    return gMapHeader.regionMapSectionId == MAPSEC_MEGA_DIMENSION;
}

// The player's own palettes (every avatar the build can draw) and their
// reflections. These stay in colour inside the dimension; so does anything
// else drawn with the player's palette (surf blob, fly bird, warp arrow).
static bool32 IsPlayerTag(u16 tag)
{
    switch (tag)
    {
    case OBJ_EVENT_PAL_TAG_BRENDAN:
    case OBJ_EVENT_PAL_TAG_MAY:
    case OBJ_EVENT_PAL_TAG_PLAYER_UNDERWATER:
    case OBJ_EVENT_PAL_TAG_RS_BRENDAN:
    case OBJ_EVENT_PAL_TAG_RS_MAY:
    case OBJ_EVENT_PAL_TAG_PLAYER_RED:
    case OBJ_EVENT_PAL_TAG_PLAYER_GREEN:
    case OBJ_EVENT_PAL_TAG_BRENDAN_REFLECTION:
    case OBJ_EVENT_PAL_TAG_MAY_REFLECTION:
    case OBJ_EVENT_PAL_TAG_PLAYER_RED_REFLECTION:
    case OBJ_EVENT_PAL_TAG_PLAYER_GREEN_REFLECTION:
        return TRUE;
    }
    return FALSE;
}

bool32 WotMegaDim_IsPlayerPaletteTag(u16 tag)
{
    // A reflection's tag is its owner's + 0x800 (PAL_TAG_REFLECTION_OFFSET,
    // field_effect_helpers.c), so the player's reflection stays in colour too.
    return IsPlayerTag(tag) || IsPlayerTag(tag - 0x800);
}

static bool32 IsMegaClaimed(u32 index)
{
    return FlagGet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + index);
}

static bool32 IsCurrentMap(u16 map)
{
    return gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(map)
        && gSaveBlock1Ptr->location.mapNum == MAP_NUM(map);
}

static bool32 IsSeen(u16 flag)
{
    return flag == 0 || FlagGet(flag);
}

// --- Records ----------------------------------------------------------------

static const u16 sRecordVars[MEGA_DIM_RECORDS] =
{
    VAR_WOT_MEGA_DIM_PORTAL_0,
    VAR_WOT_MEGA_DIM_PORTAL_1,
    VAR_WOT_MEGA_DIM_PORTAL_2,
};

static u32 GetRecord(u32 r)
{
    return r < MEGA_DIM_RECORDS ? VarGet(sRecordVars[r]) : 0;
}

static void SetRecord(u32 r, u32 value)
{
    VarSet(sRecordVars[r], value);
}

static bool32 IsRecordOpen(u32 r)
{
    return (GetRecord(r) & PORTAL_OPEN) != 0;
}

static u32 GetRecordSpot(u32 r)
{
    u32 spot = (GetRecord(r) >> PORTAL_SPOT_SHIFT) & PORTAL_SPOT_MASK;

    return spot < ARRAY_COUNT(sPortalSpots) ? spot : 0;
}

// The record's boss: a sMegaRoster index, or BOSS_LEGEND_FIRST + a sLegends index.
static u32 GetRecordBoss(u32 r)
{
    u32 boss = (GetRecord(r) >> PORTAL_MEGA_SHIFT) & PORTAL_MEGA_MASK;

    return boss < BOSS_COUNT ? boss : 0;
}

// The record's rogue Mega (sMegaRoster index; 0 if its boss is a legend).
static u32 GetRecordMega(u32 r)
{
    u32 boss = GetRecordBoss(r);

    return boss < ARRAY_COUNT(sMegaRoster) ? boss : 0;
}

// The record's legend, or NULL if its boss is a rogue Mega.
static const struct MegaDimLegend *GetRecordLegend(u32 r)
{
    u32 boss = GetRecordBoss(r);

    return boss >= BOSS_LEGEND_FIRST ? &sLegends[boss - BOSS_LEGEND_FIRST] : NULL;
}

// 0 = no rival, else 1 + index into sRivals.
static u32 GetRecordRival(u32 r)
{
    u32 rival = GetRecord(r) & PORTAL_RIVAL_MASK;

    return rival <= ARRAY_COUNT(sRivals) ? rival : 0;
}

static u32 GetTimer(u32 r)
{
    switch (r)
    {
    case 0:  return VarGet(VAR_WOT_MEGA_DIM_TIMERS) & TIMER_MASK;
    case 1:  return (VarGet(VAR_WOT_MEGA_DIM_TIMERS) >> TIMER_1_SHIFT) & TIMER_MASK;
    case 2:  return VarGet(VAR_WOT_MEGA_DIM_STATE) & TIMER_MASK;
    default: return 0;
    }
}

static void SetTimer(u32 r, u32 seconds)
{
    u32 value;

    seconds &= TIMER_MASK;
    switch (r)
    {
    case 0:
        value = VarGet(VAR_WOT_MEGA_DIM_TIMERS);
        VarSet(VAR_WOT_MEGA_DIM_TIMERS, (value & ~TIMER_MASK) | seconds);
        break;
    case 1:
        value = VarGet(VAR_WOT_MEGA_DIM_TIMERS);
        VarSet(VAR_WOT_MEGA_DIM_TIMERS, (value & ~(TIMER_MASK << TIMER_1_SHIFT)) | (seconds << TIMER_1_SHIFT));
        break;
    case 2:
        value = VarGet(VAR_WOT_MEGA_DIM_STATE);
        VarSet(VAR_WOT_MEGA_DIM_STATE, (value & ~TIMER_MASK) | seconds);
        break;
    }
}

static u32 GetSeed(u32 r)
{
    return r < MEGA_DIM_RECORDS ? (VarGet(VAR_WOT_MEGA_DIM_SEEDS) >> (r * SEED_BITS)) & SEED_MASK : 0;
}

static void SetSeed(u32 r, u32 seed)
{
    u32 value = VarGet(VAR_WOT_MEGA_DIM_SEEDS) & ~(SEED_MASK << (r * SEED_BITS));

    VarSet(VAR_WOT_MEGA_DIM_SEEDS, value | ((seed & SEED_MASK) << (r * SEED_BITS)));
}

// The record the player is inside, or RECORD_NONE.
static u32 GetSession(void)
{
    return (VarGet(VAR_WOT_MEGA_DIM_STATE) >> STATE_SESSION_SHIFT) & STATE_SESSION_MASK;
}

static void SetSession(u32 r)
{
    u32 value = VarGet(VAR_WOT_MEGA_DIM_STATE) & ~(STATE_SESSION_MASK << STATE_SESSION_SHIFT);

    VarSet(VAR_WOT_MEGA_DIM_STATE, value | ((r & STATE_SESSION_MASK) << STATE_SESSION_SHIFT));
}

static u32 GetSpawnDay(void)
{
    return (VarGet(VAR_WOT_MEGA_DIM_STATE) >> STATE_DAY_SHIFT) & STATE_DAY_MASK;
}

static void SetSpawnDay(u32 day)
{
    u32 value = VarGet(VAR_WOT_MEGA_DIM_STATE) & ~(STATE_DAY_MASK << STATE_DAY_SHIFT);

    VarSet(VAR_WOT_MEGA_DIM_STATE, value | ((day & STATE_DAY_MASK) << STATE_DAY_SHIFT));
}

static u16 GetMaskFlag(u32 r, u32 bit)
{
    return FLAG_WOT_MEGA_DIM_MASKS_START + r * MASK_FLAGS_PER_RECORD + bit;
}

static bool32 MaskGet(u32 r, u32 bit)
{
    return r < MEGA_DIM_RECORDS && FlagGet(GetMaskFlag(r, bit));
}

static void MaskSet(u32 r, u32 bit)
{
    if (r < MEGA_DIM_RECORDS)
        FlagSet(GetMaskFlag(r, bit));
}

static void MaskClear(u32 r, u32 bit)
{
    if (r < MEGA_DIM_RECORDS)
        FlagClear(GetMaskFlag(r, bit));
}

static void ClearMasks(u32 r)
{
    u32 bit;

    for (bit = 0; bit < MASK_FLAGS_PER_RECORD; bit++)
        MaskClear(r, bit);
}

// The record whose portal stands on this spot, or RECORD_NONE.
static u32 FindRecordAtSpot(u32 spot)
{
    u32 r;

    if (!FlagGet(FLAG_WOT_MEGA_DIM_V2))
        return RECORD_NONE;
    for (r = 0; r < MEGA_DIM_RECORDS; r++)
    {
        if (IsRecordOpen(r) && GetRecordSpot(r) == spot)
            return r;
    }
    return RECORD_NONE;
}

// A new batch of portals opens each in-game day at 8:00 AM (the clock and date
// on the START menu: RtcCalcLocalTime / gLocalTime). A "portal day" therefore
// runs from 8:00 to 7:59 the next morning; before 8:00 it is still
// yesterday's. Kept mod 16. (The engine's own daily reset can't be used: it is
// gated on FLAG_SYS_CLOCK_SET, which only vanilla's wall clock sets.)
#define PORTAL_OPEN_HOUR 8

static u32 GetToday(void)
{
    s32 day;

    RtcCalcLocalTime();
    day = gLocalTime.days;
    if (gLocalTime.hours < PORTAL_OPEN_HOUR)
        day--;
    return (u32)day & STATE_DAY_MASK;
}

// The vars meant something else before round 127 (one portal a day). The
// first time through, wipe them and leave the spawn day different from today,
// so a save that already unlocked the DLC gets its portals right away.
static void EnsureV2(void)
{
    u32 r;

    if (FlagGet(FLAG_WOT_MEGA_DIM_V2))
        return;
    for (r = 0; r < MEGA_DIM_RECORDS; r++)
    {
        SetRecord(r, 0);
        ClearMasks(r);
    }
    VarSet(VAR_WOT_MEGA_DIM_SEEDS, 0);
    VarSet(VAR_WOT_MEGA_DIM_TIMERS, 0);
    VarSet(VAR_WOT_MEGA_DIM_STATE, (RECORD_NONE << STATE_SESSION_SHIFT)
                                 | (((GetToday() + 1) & STATE_DAY_MASK) << STATE_DAY_SHIFT));
    FlagSet(FLAG_WOT_MEGA_DIM_V2);
}

// --- Mable's quest state (flags; see FLAG_WOT_MABLE_*) ------------------------

static u32 GetFlagBits(u16 first, u32 count)
{
    u32 i, value = 0;

    for (i = 0; i < count; i++)
        value |= FlagGet(first + i) << i;
    return value;
}

static void SetFlagBits(u16 first, u32 count, u32 value)
{
    u32 i;

    for (i = 0; i < count; i++)
    {
        if (value & (1 << i))
            FlagSet(first + i);
        else
            FlagClear(first + i);
    }
}

static u32 GetMableStage(void)
{
    return GetFlagBits(FLAG_WOT_MABLE_STAGE_START, MABLE_STAGE_BITS);
}

static void SetMableStage(u32 stage)
{
    SetFlagBits(FLAG_WOT_MABLE_STAGE_START, MABLE_STAGE_BITS, stage);
}

// Whether any open portal other than record `except` holds this boss.
static bool32 IsBossHeld(u32 boss, u32 except)
{
    u32 r;

    for (r = 0; r < MEGA_DIM_RECORDS; r++)
    {
        if (r != except && IsRecordOpen(r) && GetRecordBoss(r) == boss)
            return TRUE;
    }
    return FALSE;
}

// The legend (sLegends index) Mable's current quest still needs that no
// other open portal holds, or ARRAY_COUNT(sLegends).
static u32 GetWantedLegend(u32 except)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sLegends); i++)
    {
        if (sLegends[i].stage == GetMableStage() && !FlagGet(sLegends[i].caughtFlag)
         && !IsBossHeld(BOSS_LEGEND_FIRST + i, except))
            return i;
    }
    return ARRAY_COUNT(sLegends);
}

// Rolls a new portal into record r on this spot: a dimension copy whose real
// map the player has visited (the Route 2 copy always qualifies), a boss, the
// 1-in-5 rival, a seed for the rest, and a full clock. The boss is a legend
// Mable's quest still needs if no other portal holds it (no rival then);
// otherwise a Mega nobody has claimed and no other open portal holds (else
// any unclaimed one; once all 19 are claimed, any of them -- rematches, no
// stone). `announce`: the 8:00 spawn, which Lida calls in.
// A rogue Mega for record r: one nobody has claimed and no other open portal
// holds, else any unclaimed one; once all 19 are claimed, any of them.
static u32 PickRogueMega(u32 r)
{
    u8 pool[ARRAY_COUNT(sMegaRoster)];
    u32 i, count = 0;

    for (i = 0; i < ARRAY_COUNT(sMegaRoster); i++)
    {
        if (!IsMegaClaimed(i) && !IsBossHeld(i, r))
            pool[count++] = i;
    }
    if (count == 0)
    {
        for (i = 0; i < ARRAY_COUNT(sMegaRoster); i++)
        {
            if (!IsMegaClaimed(i))
                pool[count++] = i;
        }
    }
    if (count == 0)
        return Random() % ARRAY_COUNT(sMegaRoster);
    return pool[Random() % count];
}

static void OpenPortal(u32 r, u32 spot, bool32 announce)
{
    u8 maps[ARRAY_COUNT(sDimMaps)];
    u32 i, mapCount = 0, mega = PickRogueMega(r), rival = 0, legend = GetWantedLegend(r);

    for (i = 0; i < DIM_RANDOM_MAP_COUNT; i++)
    {
        if (IsSeen(sDimMaps[i].seenFlag))
            maps[mapCount++] = i;
    }
    if (legend < ARRAY_COUNT(sLegends))
        mega = BOSS_LEGEND_FIRST + legend;
    else if (Random() % MEGA_DIM_RIVAL_CHANCE == 0)
        rival = 1 + Random() % ARRAY_COUNT(sRivals);

    SetRecord(r, PORTAL_OPEN | (spot << PORTAL_SPOT_SHIFT) | (maps[Random() % mapCount] << PORTAL_MAP_SHIFT)
               | (mega << PORTAL_MEGA_SHIFT) | rival);
    SetSeed(r, Random());
    SetTimer(r, MEGA_DIM_TIME_LIMIT);
    ClearMasks(r);
    if (announce)
        FlagSet(FLAG_WOT_MEGA_DIM_NEW_0 + r);
}

static void ClosePortal(u32 r);

// Mable's tally is complete: every open portal (all rogue Megas) goes away.
static void CloseAllPortals(void)
{
    u32 r;

    for (r = 0; r < MEGA_DIM_RECORDS; r++)
    {
        if (r != GetSession())
            ClosePortal(r);
    }
}

// The portal collapses: record r is free again and its portal is gone.
static void ClosePortal(u32 r)
{
    if (r >= MEGA_DIM_RECORDS)
        return;
    FlagClear(FLAG_WOT_MEGA_DIM_NEW_0 + r);
    SetRecord(r, 0);
    SetTimer(r, 0);
    ClearMasks(r);
}

// A random spot on a visited map that no open portal stands on, or
// ARRAY_COUNT(sPortalSpots) if there is none.
static u32 PickFreeSpot(void)
{
    u8 spots[ARRAY_COUNT(sPortalSpots)];
    u32 i, count = 0;

    for (i = 0; i < ARRAY_COUNT(sPortalSpots); i++)
    {
        if (IsSeen(sPortalSpots[i].seenFlag) && FindRecordAtSpot(i) == RECORD_NONE)
            spots[count++] = i;
    }
    return count == 0 ? ARRAY_COUNT(sPortalSpots) : spots[Random() % count];
}

// At 8:00 AM each portal day, every free record gets a new portal on a free
// spot (so up to three are open, never two on one spot). Once Mable has all
// 19 stones, portals only open for the legends her quests still need. Portals still open
// from earlier days stay where they are. Not while the player is inside: the
// portal they came through must not change under them.
static void EnsureDailyPortals(void)
{
    u32 r, spot, today;

    if (!FlagGet(FLAG_WOT_MEGA_DOOMSDAY_INTRO))
        return;
    EnsureV2();
    if (WotMegaDim_IsActive())
        return;
    today = GetToday();
    if (GetSpawnDay() == today)
        return;
    SetSpawnDay(today);
    for (r = 0; r < MEGA_DIM_RECORDS; r++)
    {
        if (IsRecordOpen(r))
            continue;
        if (GetMableStage() >= MABLE_STAGE_TESTS && GetWantedLegend(RECORD_NONE) >= ARRAY_COUNT(sLegends))
            break; // past the tally, portals open only for Mable's legends
        spot = PickFreeSpot();
        if (spot >= ARRAY_COUNT(sPortalSpots))
            break;
        OpenPortal(r, spot, TRUE);
    }
}

// Mable's call just started a legend quest: put each legend it needs in a
// portal now rather than at the next 8:00 -- a free record on a free spot,
// else a portal the player hasn't entered yet is re-rolled where it stands.
static void OpenLegendPortals(void)
{
    u32 r, spot;

    EnsureV2();
    while (GetWantedLegend(RECORD_NONE) < ARRAY_COUNT(sLegends))
    {
        for (r = 0; r < MEGA_DIM_RECORDS && IsRecordOpen(r); r++)
            ;
        if (r < MEGA_DIM_RECORDS && (spot = PickFreeSpot()) < ARRAY_COUNT(sPortalSpots))
        {
            OpenPortal(r, spot, FALSE);
            continue;
        }
        for (r = 0; r < MEGA_DIM_RECORDS; r++)
        {
            if (IsRecordOpen(r) && !(GetRecord(r) & PORTAL_ENTERED) && GetRecordBoss(r) < BOSS_LEGEND_FIRST)
                break;
        }
        if (r == MEGA_DIM_RECORDS)
            return; // every portal is in use; the 8:00 spawn will place it
        OpenPortal(r, GetRecordSpot(r), FALSE);
    }
}

// overworld.c, just before each map's ON_TRANSITION script, so object spawns
// see the right value: marks the map visited, opens the day's portals, and
// shows this map's portal objects whose spot has an open record.
void WotMegaDim_UpdatePortalFlag(void)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sSeenMaps); i++)
    {
        if (IsCurrentMap(sSeenMaps[i].map))
            FlagSet(sSeenMaps[i].flag);
    }

    EnsureDailyPortals();
    for (i = 0; i < ARRAY_COUNT(sPortalSpots); i++)
    {
        if (!IsCurrentMap(sPortalSpots[i].srcMap))
            continue;
        if (FindRecordAtSpot(i) != RECORD_NONE)
            FlagClear(sPortalSpots[i].hideFlag);
        else
            FlagSet(sPortalSpots[i].hideFlag);
    }
}

// The local id of this map's object that uses this hide flag, if any.
static u8 FindLocalIdByFlag(u16 flag)
{
    u32 i;

    for (i = 0; i < gMapHeader.events->objectEventCount && i < OBJECT_EVENT_TEMPLATES_COUNT; i++)
    {
        if (gSaveBlock1Ptr->objectEventTemplates[i].flagId == flag)
            return gSaveBlock1Ptr->objectEventTemplates[i].localId;
    }
    return LOCALID_NONE;
}

// The spot of the portal the player just talked to, or ARRAY_COUNT(sPortalSpots).
static u32 GetTalkedSpot(void)
{
    u32 i, j;

    for (i = 0; i < gMapHeader.events->objectEventCount && i < OBJECT_EVENT_TEMPLATES_COUNT; i++)
    {
        const struct ObjectEventTemplate *template = &gSaveBlock1Ptr->objectEventTemplates[i];

        if (template->localId != gSpecialVar_LastTalked)
            continue;
        for (j = 0; j < ARRAY_COUNT(sPortalSpots); j++)
        {
            if (sPortalSpots[j].hideFlag == template->flagId && IsCurrentMap(sPortalSpots[j].srcMap))
                return j;
        }
    }
    return ARRAY_COUNT(sPortalSpots);
}

extern const u8 MegaDim_EventScript_PortalAlert[];
extern const u8 MegaDim_EventScript_MablePrimalCall[];
extern const u8 MegaDim_EventScript_MableRayquazaCall[];

// Once a second on the field (not inside): a call that's due rings as soon as
// the player is free -- Mable's first, then Lida's portal alert.
static void TryStartFieldCall(void)
{
    const u8 *script = NULL;

    if (!FlagGet(FLAG_WOT_MEGA_DOOMSDAY_INTRO) || !FlagGet(FLAG_WOT_MEGA_DIM_V2)
     || gMain.callback2 != CB2_Overworld || ArePlayerFieldControlsLocked()
     || ScriptContext_IsEnabled() || gPaletteFade.active || MapNamePopupIsActive())
        return;

    if (GetMableStage() == MABLE_STAGE_TESTS
     && GetFlagBits(FLAG_WOT_MABLE_DAY_START, MABLE_DAY_BITS) != GetToday())
        script = MegaDim_EventScript_MablePrimalCall;
    else if (GetMableStage() == MABLE_STAGE_RAYQUAZA_CALL)
        script = MegaDim_EventScript_MableRayquazaCall;
    else if (FlagGet(FLAG_WOT_MEGA_DIM_NEW_0) || FlagGet(FLAG_WOT_MEGA_DIM_NEW_1) || FlagGet(FLAG_WOT_MEGA_DIM_NEW_2))
        script = MegaDim_EventScript_PortalAlert;
    if (script != NULL)
        ScriptContext_SetupScript(script);
}

// Once a second on the field (not inside): if 8:00 AM comes round while the
// player is standing on a portal map, open the new portals right there
// instead of waiting for the next map load. Waits for the player to be free,
// so nothing pops in mid-cutscene, and for them to step off the tile.
static void RefreshPortalLive(void)
{
    u32 i;
    s16 playerX, playerY;

    if (ArePlayerFieldControlsLocked() || ScriptContext_IsEnabled())
        return;
    EnsureDailyPortals();
    PlayerGetDestCoords(&playerX, &playerY);
    for (i = 0; i < ARRAY_COUNT(sPortalSpots); i++)
    {
        const struct MegaDimSpot *spot = &sPortalSpots[i];
        bool32 isOpen, shown;
        u8 localId;

        if (!IsCurrentMap(spot->srcMap))
            continue;
        isOpen = FindRecordAtSpot(i) != RECORD_NONE;
        shown = !FlagGet(spot->hideFlag);
        if (isOpen == shown)
            continue;
        localId = FindLocalIdByFlag(spot->hideFlag);
        if (localId == LOCALID_NONE)
        {
            if (isOpen)
                FlagClear(spot->hideFlag);
            else
                FlagSet(spot->hideFlag);
        }
        else if (isOpen)
        {
            if (playerX - MAP_OFFSET == spot->x && playerY - MAP_OFFSET == spot->y)
                continue;
            FlagClear(spot->hideFlag);
            TrySpawnObjectEvent(localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
        }
        else
        {
            // Sets the hide flag itself (the template's flag).
            RemoveObjectEventByLocalIdAndMap(localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
        }
    }
}

static u32 GetSessionTimeLeft(void)
{
    return GetTimer(GetSession());
}

// overworld.c OverworldBasic: counts the portal's clock down one second of
// play time at a time, only while the field is running, so battles and the
// bag/party screens pause it (and so does being outside). At zero it raises
// VAR_TEMP_1 for the map's ON_FRAME collapse script, which only runs once
// the player is free (a battle finishes first).
void WotMegaDim_Tick(void)
{
    u32 r, timeLeft;

    if (!WotMegaDim_IsActive())
    {
        if (gSaveBlock2Ptr->playTimeSeconds != sLastPlaySecond)
        {
            sLastPlaySecond = gSaveBlock2Ptr->playTimeSeconds;
            RefreshPortalLive();
            TryStartFieldCall();
        }
        return;
    }

    r = GetSession();
    timeLeft = GetTimer(r);
    if (timeLeft != 0 && gSaveBlock2Ptr->playTimeSeconds != sLastPlaySecond)
    {
        sLastPlaySecond = gSaveBlock2Ptr->playTimeSeconds;
        SetTimer(r, --timeLeft);
    }
    if (timeLeft == 0 && VarGet(VAR_TEMP_1) == 0)
        VarSet(VAR_TEMP_1, 1);
}

// Back out in front of the portal record r came through.
static void SetReturnWarp(u32 r)
{
    const struct MegaDimSpot *spot = &sPortalSpots[GetRecordSpot(r)];

    SetWarpDestination(MAP_GROUP(spot->srcMap), MAP_NUM(spot->srcMap), WARP_ID_NONE, spot->x, spot->y + 1);
    sForceOnFoot = TRUE;
}

// The way out that collapses the portal: the player goes back to it and it's
// gone.
static void CollapseSession(void)
{
    u32 r = GetSession();

    FlagClear(FLAG_WOT_SMART_WILD_AI);
    SetReturnWarp(r);
    ClosePortal(r);
    SetSession(RECORD_NONE);
}

// overworld.c DoWhiteOut: whiting out inside collapses the portal and sends
// the player back to it rather than to the last Pokemon Center. No
// setrespawn: that would outlive the visit.
bool32 WotMegaDim_TrySetWhiteoutWarp(void)
{
    if (!WotMegaDim_IsActive())
        return FALSE;
    CollapseSession();
    return TRUE;
}

static u8 GetHighestPartyLevel(void)
{
    u32 i, level, highest = 1;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) == SPECIES_NONE
         || GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
            continue;
        level = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
        if (level > highest)
            highest = level;
    }
    return highest;
}

// wild_encounter.c CreateWildMon: the dimension's wild Pokemon are capped at
// half the party's strongest member, 0-2 under that.
u8 WotMegaDim_GetWildLevel(void)
{
    s32 level = GetHighestPartyLevel() / 2 - (Random() % (MEGA_DIM_WILD_SPREAD + 1));

    if (level < 1)
        level = 1;
    return level;
}

// The rogue Mega and the rivals' teams: 5-8 levels under the party's best.
static u8 GetBossLevel(void)
{
    s32 level = GetHighestPartyLevel() - MEGA_DIM_BOSS_BELOW_MIN
              - (Random() % (MEGA_DIM_BOSS_BELOW_MAX - MEGA_DIM_BOSS_BELOW_MIN + 1));

    if (level < 1)
        level = 1;
    return level;
}

// --- The dimension behind a portal --------------------------------------------
// Everything but the shiny odds follows from the record, so each trip through
// the same portal finds the same place. The seed drives a private generator
// (not Random(), which the shiny rolls use).

struct MegaDimInstance
{
    const struct MegaDimMap *dim;
    u8 portalSlot;
    u8 megaSlot;
    u32 rng;
};

static u32 NextDeal(u32 *rng)
{
    *rng = *rng * 1103515245u + 12345u;
    return *rng >> 16;
}

static void DeriveInstance(u32 r, struct MegaDimInstance *inst)
{
    const struct MegaDimLegend *legend = GetRecordLegend(r);
    u32 map = (GetRecord(r) >> PORTAL_MAP_SHIFT) & PORTAL_MAP_MASK;

    if (legend != NULL)
        map = legend->dimMap;
    else if (map >= DIM_RANDOM_MAP_COUNT)
        map = 0;
    inst->dim = &sDimMaps[map];
    inst->rng = (GetSeed(r) + 1) * 0x9E3779B1u
              ^ (GetRecordSpot(r) | (map << 3) | (GetRecordBoss(r) << 6)) * 0x85EBCA6Bu;
    inst->portalSlot = NextDeal(&inst->rng) % DIM_SLOT_COUNT;
    inst->megaSlot = (inst->portalSlot + 1 + NextDeal(&inst->rng) % (DIM_SLOT_COUNT - 1)) % DIM_SLOT_COUNT;
    // A legend-only map: the exit portal and the legend stand where they belong.
    if (map >= DIM_RANDOM_MAP_COUNT)
    {
        inst->portalSlot = 0;
        inst->megaSlot = 1;
    }
}

// The map's wild placeholders, in map order.
static u32 CollectWildMons(struct ObjectEventTemplate **mons)
{
    u32 i, count = 0;

    for (i = 0; i < gMapHeader.events->objectEventCount && i < OBJECT_EVENT_TEMPLATES_COUNT && count < MEGA_DIM_MON_MAX; i++)
    {
        if (gSaveBlock1Ptr->objectEventTemplates[i].script == MegaDim_EventScript_WildMon)
            mons[count++] = &gSaveBlock1Ptr->objectEventTemplates[i];
    }
    return count;
}

// Shuffles the groups; the first MEGA_DIM_VISIT_SPECIES are dealt (only
// MEGA_DIM_LEGEND_SPECIES beside a legend, whose 64x64 overworld sprite takes
// the room of the third). Returns how many are.
static u32 DealGroups(u32 r, u32 *rng, u8 *order, u32 count)
{
    u32 i, groups = (count + MEGA_DIM_MON_PER_AREA - 1) / MEGA_DIM_MON_PER_AREA;

    for (i = 0; i < groups; i++)
        order[i] = i;
    for (i = groups; i > 1; i--)
    {
        u32 swap = NextDeal(rng) % i, tmp = order[i - 1];
        order[i - 1] = order[swap];
        order[swap] = tmp;
    }
    return min(groups, GetRecordLegend(r) != NULL ? MEGA_DIM_LEGEND_SPECIES : MEGA_DIM_VISIT_SPECIES);
}

// Same odds as an ordinary wild encounter (CreateBoxMon): one roll, plus the
// Shiny Charm's extra rolls, plus one while a Lure is active.
static bool32 RollShiny(void)
{
    u32 i, rolls = 1;

    if (CheckBagHasItem(ITEM_SHINY_CHARM, 1))
        rolls += I_SHINY_CHARM_ADDITIONAL_ROLLS;
    if (LURE_STEP_COUNT != 0)
        rolls++;
    for (i = 0; i < rolls; i++)
    {
        if (Random() < SHINY_ODDS)
            return TRUE;
    }
    return FALSE;
}

// Whether a shiny of this species can be drawn: a shiny needs its own sprite
// sheet, so allow a couple per visit.
static bool32 TakeShinySheet(u16 *sheets, u32 *sheetCount, u16 species)
{
    u32 i;

    for (i = 0; i < *sheetCount; i++)
    {
        if (sheets[i] == species)
            return TRUE;
    }
    if (*sheetCount >= MEGA_DIM_MAX_SHINY_SHEETS)
        return FALSE;
    sheets[(*sheetCount)++] = species;
    return TRUE;
}

// ON_TRANSITION: deal the portal's visible wild Pokemon. The map's
// placeholder objects, in map order, form groups of MEGA_DIM_MON_PER_AREA
// (the last may be smaller). MEGA_DIM_VISIT_SPECIES groups (picked by the
// seed) each get one species from the map's land table (picked by the seed,
// through the Randomizer if it's on); the other groups hide, and so does any
// Pokemon already beaten or caught from this portal. A shiny found earlier
// through this portal is still shiny; every other one rolls the usual odds
// again. Positions are left as placed in porymap. Species and shininess live
// in the template's graphics id (species + OBJ_EVENT_MON
// [+ OBJ_EVENT_MON_SHINY]), which is what the sprite draws and what the
// battle reads back.
static void SetupWildMons(u32 r, struct MegaDimInstance *inst)
{
    struct ObjectEventTemplate *mons[MEGA_DIM_MON_MAX];
    const struct WildPokemonInfo *land;
    u16 species[MEGA_DIM_VISIT_SPECIES];
    u16 shinySheets[MEGA_DIM_MAX_SHINY_SHEETS];
    u8 groupOrder[MEGA_DIM_MON_MAX];
    u32 i, j, k, count, dealt, headerId, shinyCount = 0;

    count = CollectWildMons(mons);
    for (i = 0; i < count; i++)
        FlagSet(mons[i]->flagId);   // hidden unless dealt below
    if (count == 0 || GetRecordLegend(r) != NULL)
        return;                     // a legend stands alone

    headerId = GetCurrentMapWildMonHeaderId();
    land = (headerId == HEADER_NONE) ? NULL
         : gWildMonHeaders[headerId].encounterTypes[GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND)].landMonsInfo;
    if (land == NULL)
        return;

    dealt = DealGroups(r, &inst->rng, groupOrder, count);
    for (i = 0; i < dealt; i++)
    {
        species[i] = land->wildPokemon[NextDeal(&inst->rng) % LAND_WILD_COUNT].species;
        if (FlagGet(FLAG_WOT_RAND_WILD))
            species[i] = WotRandomizeSpecies(species[i]);
    }

    // The shinies this portal already showed keep their sheets first.
    for (i = 0; i < dealt; i++)
    {
        for (j = 0; j < MEGA_DIM_MON_PER_AREA; j++)
        {
            k = i * MEGA_DIM_MON_PER_AREA + j;
            if (!MaskGet(r, MASK_GONE(k)) && MaskGet(r, MASK_SHINY(k)))
                TakeShinySheet(shinySheets, &shinyCount, species[i]);
        }
    }

    for (i = 0; i < dealt; i++)
    {
        u32 first = groupOrder[i] * MEGA_DIM_MON_PER_AREA;

        for (j = 0; j < MEGA_DIM_MON_PER_AREA && first + j < count; j++)
        {
            struct ObjectEventTemplate *mon = mons[first + j];
            bool32 shiny;

            k = i * MEGA_DIM_MON_PER_AREA + j;
            if (MaskGet(r, MASK_GONE(k)))
                continue;
            shiny = MaskGet(r, MASK_SHINY(k));
            if (!shiny && (RollShiny() || sForceShiny) && TakeShinySheet(shinySheets, &shinyCount, species[i]))
            {
                shiny = TRUE;
                sForceShiny = FALSE;
                MaskSet(r, MASK_SHINY(k));
            }
            mon->graphicsId = species[i] + OBJ_EVENT_MON + (shiny ? OBJ_EVENT_MON_SHINY : 0);
            FlagClear(mon->flagId);
        }
    }
}

static void PlaceTemplate(u16 graphicsId, const struct MegaDimSlot *slot)
{
    u32 i;

    for (i = 0; i < gMapHeader.events->objectEventCount && i < OBJECT_EVENT_TEMPLATES_COUNT; i++)
    {
        struct ObjectEventTemplate *template = &gSaveBlock1Ptr->objectEventTemplates[i];

        if (template->graphicsId == graphicsId)
        {
            template->x = slot->x;
            template->y = slot->y;
            template->elevation = slot->elevation;
            return;
        }
    }
}

// --- Specials -------------------------------------------------------------

// Talking to a portal in the normal world:
//   VAR_RESULT = 0 -- never entered: the usual prompt
//                1 -- entered before: STR_VAR_1 = the time left, and going
//                     back in costs MEGA_DIM_REENTRY_PENALTY seconds
//                2 -- too little time left to go back in: the portal
//                     collapses now (the script removes it)
void WotMegaDim_CheckPortal(void)
{
    u32 r = FindRecordAtSpot(GetTalkedSpot());
    u32 timeLeft;
    u8 *str;

    gSpecialVar_Result = 0;
    if (r == RECORD_NONE || !(GetRecord(r) & PORTAL_ENTERED))
        return;
    timeLeft = GetTimer(r);
    if (timeLeft <= MEGA_DIM_REENTRY_PENALTY)
    {
        ClosePortal(r);
        gSpecialVar_Result = 2;
        return;
    }
    str = ConvertIntToDecimalStringN(gStringVar1, timeLeft / 60, STR_CONV_MODE_LEFT_ALIGN, 1);
    *str++ = CHAR_COLON;
    ConvertIntToDecimalStringN(str, timeLeft % 60, STR_CONV_MODE_LEADING_ZEROS, 2);
    gSpecialVar_Result = 1;
}

// Through the portal: a trip back in costs MEGA_DIM_REENTRY_PENALTY seconds;
// then warp in one tile south of the exit portal's slot. Script contexts end
// at a warp, so nothing follows this.
void WotMegaDim_EnterPortal(void)
{
    struct MegaDimInstance inst;
    const struct MegaDimSlot *arrive;
    u32 r = FindRecordAtSpot(GetTalkedSpot());
    u32 record;

    if (r == RECORD_NONE)
    {
        // Can't happen (the portal object only shows with a record); open one
        // here rather than strand the script.
        u32 spot = GetTalkedSpot();

        for (r = 0; r < MEGA_DIM_RECORDS && IsRecordOpen(r); r++)
            ;
        if (r == MEGA_DIM_RECORDS)
            r = 0;
        OpenPortal(r, spot < ARRAY_COUNT(sPortalSpots) ? spot : 0, FALSE);
    }

    record = GetRecord(r);
    // Playtest (round 129): a Mega that has been claimed since this portal
    // opened must not turn up again. First trip in only -- after that the
    // portal's dimension is fixed.
    if (!(record & PORTAL_ENTERED) && GetRecordLegend(r) == NULL && IsMegaClaimed(GetRecordMega(r))
     && WotMegaDim_CountClaimed() < ARRAY_COUNT(sMegaRoster))
        record = (record & ~(PORTAL_MEGA_MASK << PORTAL_MEGA_SHIFT)) | (PickRogueMega(r) << PORTAL_MEGA_SHIFT);
    if (record & PORTAL_ENTERED)
        SetTimer(r, GetTimer(r) > MEGA_DIM_REENTRY_PENALTY ? GetTimer(r) - MEGA_DIM_REENTRY_PENALTY : 1);
    record |= PORTAL_ENTERED;
    if (sForceRival != 0 && GetRecordLegend(r) == NULL)
    {
        record = (record & ~PORTAL_RIVAL_MASK) | sForceRival;
        sForceRival = 0;
    }
    SetRecord(r, record);
    SetSession(r);
    sLastPlaySecond = gSaveBlock2Ptr->playTimeSeconds;

    DeriveInstance(r, &inst);
    arrive = &inst.dim->slots[inst.portalSlot];
    SetWarpDestination(MAP_GROUP(inst.dim->map), MAP_NUM(inst.dim->map), WARP_ID_NONE, arrive->x, arrive->y + 1);
    sForceOnFoot = TRUE;
    DoWhiteFadeWarp();
    ResetInitialPlayerAvatarState();
}

// The exit portal inside: back to the portal, which stays open (its clock
// stops until the player goes back in).
void WotMegaDim_LeaveOut(void)
{
    FlagClear(FLAG_WOT_SMART_WILD_AI);
    SetReturnWarp(GetSession());
    SetSession(RECORD_NONE);
    DoWhiteFadeWarp();
    ResetInitialPlayerAvatarState();
}

// The collapse: the clock ran out, or the rogue Mega was caught or KO'd (or
// its rival beaten). Back to the portal, which is gone.
void WotMegaDim_CollapseOut(void)
{
    CollapseSession();
    DoWhiteFadeWarp();
    ResetInitialPlayerAvatarState();
}

//   out VAR_0x8004 -- the boss's species as it appears (its cry)
//   out VAR_0x8005 -- its Mega Stone (ITEM_NONE for a legend)
//   out VAR_0x8006 -- the base species (for bufferspeciesname)
//   out VAR_0x8007 -- TRUE if the boss is one of Mable's legends
//   out VAR_RESULT -- TRUE if this Mega has already been claimed (rematch),
//                     or this legend already caught
void WotMegaDim_GetMega(void)
{
    u32 r = GetSession();
    const struct MegaDimLegend *legend = GetRecordLegend(r);
    u32 index = GetRecordMega(r);

    if (legend != NULL)
    {
        gSpecialVar_0x8004 = legend->formSpecies;
        gSpecialVar_0x8005 = ITEM_NONE;
        gSpecialVar_0x8006 = legend->species;
        gSpecialVar_0x8007 = TRUE;
        gSpecialVar_Result = FlagGet(legend->caughtFlag);
        return;
    }
    gSpecialVar_0x8004 = sMegaRoster[index].megaSpecies;
    gSpecialVar_0x8005 = sMegaRoster[index].stone;
    gSpecialVar_0x8006 = sMegaRoster[index].baseSpecies;
    gSpecialVar_0x8007 = FALSE;
    gSpecialVar_Result = IsMegaClaimed(index);
}

// pokemon.c GetBattleBGM, battle_setup.c: the session's boss is one of
// Mable's legends (it battles to its own theme and transition).
bool32 WotMegaDim_IsLegendBoss(void)
{
    return WotMegaDim_IsActive() && GetRecordLegend(GetSession()) != NULL;
}

// pokemon.c GetBattleBGM: a rival's own theme (any other trainer in the
// dimension keeps Giratina's).
u16 WotMegaDim_GetRivalMusic(u16 trainerId)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sRivals); i++)
    {
        if (sRivals[i].trainerId == trainerId)
            return sRivals[i].music;
    }
    return MUS_PL_VS_GIRATINA;
}

// ON_TRANSITION of a dimension map, before its objects spawn: the Mega object
// (OBJ_EVENT_GFX_VAR_1) draws the portal's Mega -- the fan Megas' overworld
// art is their base species', shiny if this portal's Mega came up shiny --
// and the exit portal and the Mega move onto the portal's slots. The
// templates live in the save block, so returning from a battle or reloading
// a save inside keeps them where they are.
void WotMegaDim_SetMegaGfx(void)
{
    struct MegaDimInstance inst;
    u32 r = GetSession();
    u32 index = GetRecordMega(r);
    bool32 shiny = MaskGet(r, MASK_MEGA_SHINY);

    // Shiny odds are rolled again on every trip in; a shiny one stays shiny.
    if (!shiny && RollShiny())
    {
        shiny = TRUE;
        MaskSet(r, MASK_MEGA_SHINY);
    }
    // A legend shows its Primal / Mega form; the fan Megas, their base species.
    VarSet(VAR_OBJ_GFX_ID_1, (GetRecordLegend(r) != NULL ? GetRecordLegend(r)->formSpecies : sMegaRoster[index].baseSpecies)
                           + OBJ_EVENT_MON + (shiny ? OBJ_EVENT_MON_SHINY : 0));

    DeriveInstance(r, &inst);
    if (IsCurrentMap(inst.dim->map))
    {
        u32 rival = GetRecordRival(r);

        PlaceTemplate(OBJ_EVENT_GFX_WOT_MEGA_PORTAL, &inst.dim->slots[inst.portalSlot]);
        PlaceTemplate(OBJ_EVENT_GFX_VAR_1, &inst.dim->slots[inst.megaSlot]);
        // A rival got here first: they stand where the Mega would have, and
        // the Mega (already in their party) isn't there. Temp flags are clear
        // on every warp-in, so both are set here either way.
        if (rival != 0)
        {
            FlagSet(FLAG_TEMP_1);
            FlagClear(FLAG_TEMP_2);
            VarSet(VAR_OBJ_GFX_ID_2, sRivals[rival - 1].graphicsId);
            PlaceTemplate(OBJ_EVENT_GFX_VAR_2, &inst.dim->slots[inst.megaSlot]);
        }
        else
        {
            FlagSet(FLAG_TEMP_2);
        }
        SetupWildMons(r, &inst);
    }
}

// The base species holding its stone, 12-16 levels under the party's best,
// with perfect IVs, shiny if the overworld one is. The smart-wild-AI flag
// (B_SMART_WILD_AI_FLAG) makes it pick its gimmick, so it Mega Evolves on turn
// one; WotMegaDim_EndMegaBattle clears it again. A legend is built the same
// way: Groudon/Kyogre hold their orb (Primal Reversion on switch-in needs no
// flag), Rayquaza gets Dragon Ascent in slot 1 (its gen-9 moveset drops it
// above Lv27) so it can Mega Evolve.
void WotMegaDim_SetupMegaBattle(void)
{
    u32 r = GetSession();
    const struct MegaDimLegend *legend = GetRecordLegend(r);
    u32 index = GetRecordMega(r);
    u32 level = (legend != NULL) ? MEGA_DIM_LEGEND_LEVEL : GetBossLevel();
    u32 i, iv = MAX_PER_STAT_IVS;
    bool32 shiny = MaskGet(r, MASK_MEGA_SHINY);

    if (level > MAX_LEVEL)
        level = MAX_LEVEL;
    if (legend != NULL)
    {
        CreateScriptedWildMon(legend->species, level, legend->item);
        if (legend->move != MOVE_NONE)
        {
            u16 move = legend->move;
            u8 pp = GetMovePP(move);

            SetMonData(&gEnemyParty[0], MON_DATA_MOVE1, &move);
            SetMonData(&gEnemyParty[0], MON_DATA_PP1, &pp);
        }
    }
    else
    {
        CreateScriptedWildMon(sMegaRoster[index].baseSpecies, level, sMegaRoster[index].stone);
    }
    for (i = 0; i < NUM_STATS; i++)
        SetMonData(&gEnemyParty[0], MON_DATA_HP_IV + i, &iv);
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY, &shiny);
    CalculateMonStats(&gEnemyParty[0]);
    FlagSet(FLAG_WOT_SMART_WILD_AI);
}

// --- Battle UI ----------------------------------------------------------------
// In the dimension the battle UI is black and white too: the textbox and
// message text (BG 0), the window border (BG 1), the menu text (BG 5), and the
// healthbox and HP/EXP bar sprites. Called right after each of those loads
// (battle_bg.c, battle_gfx_sfx_util.c); grayscale is idempotent, so a reload
// passing through twice is harmless. A shiny's gold box is derived from the
// healthbox palette when it's first needed, so it still comes out gold.
static void GrayPalette(u32 offset)
{
    TintPalette_GrayScale(&gPlttBufferUnfaded[offset], 16);
    TintPalette_GrayScale(&gPlttBufferFaded[offset], 16);
}

void WotMegaDim_GrayBattleUi(void)
{
    u8 slot;

    if (!WotMegaDim_IsActive())
        return;
    GrayPalette(BG_PLTT_ID(0));
    GrayPalette(BG_PLTT_ID(1));
    GrayPalette(BG_PLTT_ID(5));
    if ((slot = IndexOfSpritePaletteTag(TAG_HEALTHBOX_PAL)) != 0xFF)
        GrayPalette(OBJ_PLTT_ID(slot));
    if ((slot = IndexOfSpritePaletteTag(TAG_HEALTHBAR_PAL)) != 0xFF)
        GrayPalette(OBJ_PLTT_ID(slot));
}

// --- Rivals -----------------------------------------------------------------

// The rivals' overworld palettes stay in colour in the gray world (like the
// player): field_weather.c WotMegaDim_GraySpritePalette.
bool32 WotMegaDim_IsRivalPaletteTag(u16 tag)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sRivals); i++)
    {
        if (GetObjectEventGraphicsInfo(sRivals[i].graphicsId)->paletteTag == tag)
            return TRUE;
    }
    return FALSE;
}

bool32 WotMegaDim_IsRivalTrainer(u16 trainerId)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sRivals); i++)
    {
        if (sRivals[i].trainerId == trainerId)
            return TRUE;
    }
    return FALSE;
}

static void SetMonLevel(struct Pokemon *mon, s32 level)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);

    if (level < 1)
        level = 1;
    if (level > MAX_LEVEL)
        level = MAX_LEVEL;
    SetMonData(mon, MON_DATA_EXP, &gExperienceTables[gSpeciesInfo[species].growthRate][level]);
    CalculateMonStats(mon);
    GiveMonInitialMoveset(mon);
}

// battle_main.c CreateNPCTrainerParty: in the dimension, a rival's 3rd mon
// (a placeholder in src/data/trainers.party) becomes the portal's rogue Mega:
// its base species holding its stone, 12-16 levels under the party's best
// with perfect IVs, shiny if this portal's Mega came up shiny. Their own two
// sit 2-4 levels under it, so the Mega is always their highest. The AI Mega
// Evolves it (trainers need no Mega Ring).
void WotMegaDim_AdjustRivalParty(struct Pokemon *party, u16 trainerId)
{
    u32 i, r, index, aceLevel, iv = MAX_PER_STAT_IVS;
    bool32 shiny;
    struct OriginalTrainerId otId = OTID_STRUCT_RANDOM_NO_SHINY;
    struct Pokemon *ace = &party[MEGA_DIM_RIVAL_ACE_SLOT];

    if (!WotMegaDim_IsActive() || !WotMegaDim_IsRivalTrainer(trainerId))
        return;

    aceLevel = GetBossLevel();
    for (i = 0; i < MEGA_DIM_RIVAL_ACE_SLOT; i++)
    {
        if (GetMonData(&party[i], MON_DATA_SPECIES) == SPECIES_NONE)
            continue;
        SetMonLevel(&party[i], (s32)aceLevel - MEGA_DIM_RIVAL_TEAM_BELOW_MIN
                             - (s32)(Random() % (MEGA_DIM_RIVAL_TEAM_BELOW_MAX - MEGA_DIM_RIVAL_TEAM_BELOW_MIN + 1)));
    }

    r = GetSession();
    index = GetRecordMega(r);
    shiny = MaskGet(r, MASK_MEGA_SHINY);
    CreateMon(ace, sMegaRoster[index].baseSpecies, aceLevel, Random32(), otId);
    SetMonData(ace, MON_DATA_HELD_ITEM, &sMegaRoster[index].stone);
    for (i = 0; i < NUM_STATS; i++)
        SetMonData(ace, MON_DATA_HP_IV + i, &iv);
    SetMonData(ace, MON_DATA_IS_SHINY, &shiny);
    CalculateMonStats(ace);
    GiveMonInitialMoveset(ace);
}

// --- Visible wild Pokemon: talk to one ---------------------------------------
//   out VAR_0x8004 -- the talked-to Pokemon's species (for its cry)
//   out VAR_0x8005 -- TRUE if it's shiny
void WotMegaDim_BufferTalkedMon(void)
{
    u8 objectEventId = GetObjectEventIdByLocalIdAndMap(gSpecialVar_LastTalked, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    u16 graphicsId = (objectEventId < OBJECT_EVENTS_COUNT) ? gObjectEvents[objectEventId].graphicsId : 0;

    gSpecialVar_0x8004 = (graphicsId & OBJ_EVENT_MON) ? (graphicsId & OBJ_EVENT_MON_SPECIES_MASK) : SPECIES_NONE;
    gSpecialVar_0x8005 = (graphicsId & OBJ_EVENT_MON) && (graphicsId & OBJ_EVENT_MON_SHINY);
}

// That exact Pokemon (WotMegaDim_BufferTalkedMon's species + shininess) at the
// dimension's wild level, in an ordinary wild battle (wild music, wild
// transition). waitstate special.
void WotMegaDim_StartOverworldMonBattle(void)
{
    u32 shiny = gSpecialVar_0x8005;

    CreateScriptedWildMon(gSpecialVar_0x8004, WotMegaDim_GetWildLevel(), ITEM_NONE);
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY, &shiny);
    BattleSetup_StartScriptedWildBattle();
}

// The talked-to wild Pokemon was beaten or caught: it stays gone for as long
// as this portal is open (and, if it was shiny, it no longer holds a sheet).
// Works out its place in the deal the same way SetupWildMons did.
void WotMegaDim_MarkWildGone(void)
{
    struct ObjectEventTemplate *mons[MEGA_DIM_MON_MAX];
    struct MegaDimInstance inst;
    u8 groupOrder[MEGA_DIM_MON_MAX];
    u32 i, index, count, dealt, k, r = GetSession();

    if (r == RECORD_NONE)
        return;
    count = CollectWildMons(mons);
    for (index = 0; index < count && mons[index]->localId != gSpecialVar_LastTalked; index++)
        ;
    if (index == count)
        return;

    DeriveInstance(r, &inst);
    dealt = DealGroups(r, &inst.rng, groupOrder, count);
    for (i = 0; i < dealt; i++)
    {
        if (groupOrder[i] != index / MEGA_DIM_MON_PER_AREA)
            continue;
        k = i * MEGA_DIM_MON_PER_AREA + index % MEGA_DIM_MON_PER_AREA;
        MaskSet(r, MASK_GONE(k));
        MaskClear(r, MASK_SHINY(k));
    }
}

// How many rogue Megas the player has claimed in the dimension (stone won or
// Mega caught). Only the dimension sets these flags, so a Mega Stone found
// anywhere else never counts toward Mable's tally.
u16 WotMegaDim_CountClaimed(void)
{
    u32 i, count = 0;

    for (i = 0; i < ARRAY_COUNT(sMegaRoster); i++)
        count += IsMegaClaimed(i);
    return count;
}

// The portal's rival for the script: VAR_RESULT = 0 (none) or 1-6 (sRivals order).
void WotMegaDim_GetRival(void)
{
    gSpecialVar_Result = GetRecordRival(GetSession());
}

// End of the Tradewind intro: the rift Lida and Canari came through just
// collapsed, so today counts as spawned -- the first real portals open at
// 8:00 AM the next day.
void WotMegaDim_CloseTodaysPortal(void)
{
    EnsureV2();
    SetSpawnDay(GetToday());
}

void WotMegaDim_EndMegaBattle(void)
{
    FlagClear(FLAG_WOT_SMART_WILD_AI);
}

// The boss was claimed: a rogue Mega's stone won or the Mega caught -- or one
// of Mable's legends caught, which moves her quest chain on.
void WotMegaDim_MarkClaimed(void)
{
    const struct MegaDimLegend *legend = GetRecordLegend(GetSession());

    if (legend == NULL)
    {
        FlagSet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + GetRecordMega(GetSession()));
        return;
    }
    FlagSet(legend->caughtFlag);
    if (GetMableStage() == MABLE_STAGE_PRIMALS
     && FlagGet(FLAG_WOT_CAUGHT_PRIMAL_GROUDON) && FlagGet(FLAG_WOT_CAUGHT_PRIMAL_KYOGRE))
        SetMableStage(MABLE_STAGE_RAYQUAZA_CALL);
    else if (GetMableStage() == MABLE_STAGE_RAYQUAZA && FlagGet(FLAG_WOT_CAUGHT_MEGA_RAYQUAZA))
        SetMableStage(MABLE_STAGE_DONE);
}

// --- Mable (Hotel Z) ------------------------------------------------------------
// specialvar: report to Mable. The stones stay with the player; she keeps
// the tally (FLAG_WOT_MABLE_TALLY_*).
//   0 -- nothing new: VAR_0x8004 = the tally so far
//   1 -- new samples recorded: VAR_0x8004 = the new tally
//   2 -- the tally just reached 19: her tests start (the Primal call comes
//        on a later portal day)
//   3 -- the tally is long done: VAR_0x8005 = her stage (MABLE_STAGE_*)
u16 WotMegaDim_MableReport(void)
{
    u32 claimed = WotMegaDim_CountClaimed();
    u32 tally = GetFlagBits(FLAG_WOT_MABLE_TALLY_START, MABLE_TALLY_BITS);

    gSpecialVar_0x8005 = GetMableStage();
    if (GetMableStage() != MABLE_STAGE_TALLY)
        return 3;
    gSpecialVar_0x8004 = tally;
    if (claimed <= tally)
        return 0;
    SetFlagBits(FLAG_WOT_MABLE_TALLY_START, MABLE_TALLY_BITS, claimed);
    gSpecialVar_0x8004 = claimed;
    if (claimed < ARRAY_COUNT(sMegaRoster))
        return 1;
    SetMableStage(MABLE_STAGE_TESTS);
    SetFlagBits(FLAG_WOT_MABLE_DAY_START, MABLE_DAY_BITS, GetToday());
    CloseAllPortals();   // the rogue Megas are done; only legend portals from here
    return 2;
}

// Mable's call starts the next legend quest: Primal Signatures after her
// tests, Sky-High Mega once both Primals are caught. Its legends go into
// portals right away.
void WotMegaDim_StartLegendQuest(void)
{
    if (GetMableStage() == MABLE_STAGE_TESTS)
        SetMableStage(MABLE_STAGE_PRIMALS);
    else if (GetMableStage() == MABLE_STAGE_RAYQUAZA_CALL)
        SetMableStage(MABLE_STAGE_RAYQUAZA);
    OpenLegendPortals();
}

// Lida's 8:00 alert: STR_VAR_1..3 = the towns the new portals opened in (each
// once), VAR_RESULT = how many. Clears the new-portal flags.
void WotMegaDim_BufferPortalAlert(void)
{
    u8 *const bufs[] = {gStringVar1, gStringVar2, gStringVar3};
    const u8 *towns[MEGA_DIM_RECORDS];
    u32 r, i, count = 0;

    for (r = 0; r < MEGA_DIM_RECORDS; r++)
    {
        const u8 *town;

        if (!FlagGet(FLAG_WOT_MEGA_DIM_NEW_0 + r))
            continue;
        FlagClear(FLAG_WOT_MEGA_DIM_NEW_0 + r);
        if (!IsRecordOpen(r))
            continue;
        town = sPortalSpots[GetRecordSpot(r)].town;
        for (i = 0; i < count && towns[i] != town; i++)
            ;
        if (i == count)
            towns[count++] = town;
    }
    for (i = 0; i < count; i++)
        StringCopy(bufs[i], towns[i]);
    gSpecialVar_Result = count;
}

// --- Countdown HUD ----------------------------------------------------------
// A small m:ss box in the top-left corner. It only shows while the player
// has free control, and is taken down (tilemap cleared, window removed)
// whenever a script, menu, fade or the map-name pop-up is up, so it never
// draws over anything. The task dies with the task pool when the field is
// torn down; the map's ON_RESUME re-arms it.

#define tWindowId   data[0]
#define tShownTime  data[1]

// BG0 tiles live at charbase 2 (0x8000), but only tiles 0x000-0x2FF are safe:
// 0x300+ is 0xE000+, where BG2's tilemap (map base 28) sits, so drawing there
// scribbles over the map and the map scrolling scribbles over the digits.
// 0x2E0-0x2E7: clear of the dialog/std frames and pop-up edges (0x200-0x23A),
// the bag (0x231-0x25A) and match call (0x270-0x289).
#define HUD_BASE_BLOCK 0x2E0

static const struct WindowTemplate sHudWindowTemplate =
{
    .bg = 0,
    .tilemapLeft = 0,
    .tilemapTop = 0,
    .width = 4,
    .height = 2,
    .paletteNum = 15,
    .baseBlock = HUD_BASE_BLOCK,
};

static bool32 ShouldShowHud(void)
{
    return WotMegaDim_IsActive()
        && !ArePlayerFieldControlsLocked()
        && !ScriptContext_IsEnabled()
        && !MapNamePopupIsActive()
        && !gPaletteFade.active;
}

static void DrawHud(u8 windowId, u16 timeLeft)
{
    u8 *str = gStringVar4;

    str = ConvertIntToDecimalStringN(str, timeLeft / 60, STR_CONV_MODE_LEFT_ALIGN, 1);
    *str++ = CHAR_COLON;
    ConvertIntToDecimalStringN(str, timeLeft % 60, STR_CONV_MODE_LEADING_ZEROS, 2);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    AddTextPrinterParameterized(windowId, FONT_SMALL, gStringVar4, 6, 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void HideHud(s16 *data)
{
    if (tWindowId == WINDOW_NONE)
        return;
    ClearWindowTilemap(tWindowId);
    CopyWindowToVram(tWindowId, COPYWIN_MAP);
    RemoveWindow(tWindowId);
    tWindowId = WINDOW_NONE;
}

static void Task_MegaDimHud(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u16 timeLeft = GetSessionTimeLeft();

    if (!WotMegaDim_IsActive())
    {
        HideHud(data);
        DestroyTask(taskId);
        return;
    }

    if (!ShouldShowHud())
    {
        HideHud(data);
        return;
    }

    if (tWindowId == WINDOW_NONE)
    {
        tWindowId = AddWindow(&sHudWindowTemplate);
        if (tWindowId == WINDOW_NONE)
            return;
        PutWindowTilemap(tWindowId);
        tShownTime = -1;
    }
    if (tShownTime != timeLeft)
    {
        tShownTime = timeLeft;
        DrawHud(tWindowId, timeLeft);
    }
}

void WotMegaDim_StartHud(void)
{
    u8 taskId = FindTaskIdByFunc(Task_MegaDimHud);

    // ON_RESUME only runs after the field (and its windows) were rebuilt, so a
    // surviving task's window id is stale: drop it without touching it.
    if (taskId != TASK_NONE)
        DestroyTask(taskId);
    taskId = CreateTask(Task_MegaDimHud, 80);
    gTasks[taskId].tWindowId = WINDOW_NONE;
    gTasks[taskId].tShownTime = -1;
}

#undef tWindowId
#undef tShownTime

// --- Debug (Debug -> Scripts) -----------------------------------------------

// Opens a fresh portal on the first spot of the map the player is standing on
// (Tradewind's if this map has none), marks that map visited, unlocks the DLC
// (as if the Tradewind intro had played), and warps to the tile in front of
// it so the portal spawns. Re-rolls the portal already there, if any; else
// takes a free record (record 0 if all three are open).
void WotMegaDim_DebugOpenPortal(void)
{
    const struct MegaDimSpot *spot;
    u32 i, r, index = ARRAY_COUNT(sPortalSpots);

    FlagSet(FLAG_WOT_MEGA_DOOMSDAY_INTRO);
    EnsureV2();
    for (i = 0; i < ARRAY_COUNT(sPortalSpots) && index == ARRAY_COUNT(sPortalSpots); i++)
    {
        if (IsCurrentMap(sPortalSpots[i].srcMap))
            index = i;
    }
    if (index == ARRAY_COUNT(sPortalSpots))
        index = 0;
    spot = &sPortalSpots[index];
    FlagSet(spot->seenFlag);

    r = FindRecordAtSpot(index);
    if (r == RECORD_NONE)
    {
        for (r = 0; r < MEGA_DIM_RECORDS && IsRecordOpen(r); r++)
            ;
        if (r == MEGA_DIM_RECORDS)
            r = 0;
    }
    OpenPortal(r, index, FALSE);

    SetWarpDestination(MAP_GROUP(spot->srcMap), MAP_NUM(spot->srcMap), WARP_ID_NONE, spot->x, spot->y + 1);
    sForceOnFoot = TRUE;
    DoWarp();
    ResetInitialPlayerAvatarState();
}

// The next portal entered has a rival (cycling through the six on each use),
// and keeps them for as long as it's open.
void WotMegaDim_DebugForceRival(void)
{
    sForceRival = 1 + sDebugNextRival;
    sDebugNextRival = (sDebugNextRival + 1) % ARRAY_COUNT(sRivals);
    gSpecialVar_Result = sForceRival;
}

// The next deal's first wild Pokemon comes up shiny (to see it stay shiny
// across trips through the same portal).
void WotMegaDim_DebugForceShiny(void)
{
    sForceShiny = TRUE;
}

// --- Hotel Z's two doors --------------------------------------------------------
// Hotel Z's exits use the dynamic warp. ON_TRANSITION points it back the way
// the player came in: the Rebel Hideout's back door (its coord trigger at
// (9,0); out at (9,1)), else Tradewind's (23,17) door (its warp 5). A
// reloaded save keeps whichever was set last.
#define TRADEWIND_HOTEL_Z_WARP 5
#define HIDEOUT_BACK_DOOR_X 9
#define HIDEOUT_BACK_DOOR_Y 0

static bool32 IsWarpMap(const struct WarpData *warp, u16 map)
{
    return warp->mapGroup == MAP_GROUP(map) && warp->mapNum == MAP_NUM(map);
}

void WotHotelZ_SetExit(void)
{
    if (IsWarpMap(&gLastUsedWarp, MAP_REBEL_HIDEOUT))
        SetDynamicWarpWithCoords(0, MAP_GROUP(MAP_REBEL_HIDEOUT), MAP_NUM(MAP_REBEL_HIDEOUT), WARP_ID_NONE,
                                 HIDEOUT_BACK_DOOR_X, HIDEOUT_BACK_DOOR_Y + 1);
    else if (IsWarpMap(&gLastUsedWarp, MAP_TRADEWIND_TOWN)
          || (!IsWarpMap(&gSaveBlock1Ptr->dynamicWarp, MAP_REBEL_HIDEOUT)
           && !IsWarpMap(&gSaveBlock1Ptr->dynamicWarp, MAP_TRADEWIND_TOWN)))
        SetDynamicWarp(0, MAP_GROUP(MAP_TRADEWIND_TOWN), MAP_NUM(MAP_TRADEWIND_TOWN), TRADEWIND_HOTEL_Z_WARP);
}

// Jumps Mable's quest chain to its next call, as if earned:
//   collecting -> all 19 stones claimed and reported (the debug script
//                 completes the quest), her Primal call due now
//   tests      -> the Primal call due now
//   Primals    -> both caught, the Rayquaza call due now
// VAR_RESULT = the new stage.
void WotMegaDim_DebugMableNext(void)
{
    u32 i;

    FlagSet(FLAG_WOT_MEGA_DOOMSDAY_INTRO);
    EnsureV2();
    switch (GetMableStage())
    {
    case MABLE_STAGE_TALLY:
        for (i = 0; i < ARRAY_COUNT(sMegaRoster); i++)
            FlagSet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + i);
        SetFlagBits(FLAG_WOT_MABLE_TALLY_START, MABLE_TALLY_BITS, ARRAY_COUNT(sMegaRoster));
        SetMableStage(MABLE_STAGE_TESTS);
        CloseAllPortals();
        // fall through
    case MABLE_STAGE_TESTS:
        SetFlagBits(FLAG_WOT_MABLE_DAY_START, MABLE_DAY_BITS, GetToday() + 1);
        break;
    case MABLE_STAGE_PRIMALS:
        FlagSet(FLAG_WOT_CAUGHT_PRIMAL_GROUDON);
        FlagSet(FLAG_WOT_CAUGHT_PRIMAL_KYOGRE);
        SetMableStage(MABLE_STAGE_RAYQUAZA_CALL);
        break;
    }
    gSpecialVar_Result = GetMableStage();
}

// Inside the dimension: ten seconds left, to see the collapse.
void WotMegaDim_DebugShortTimer(void)
{
    u32 r = GetSession();

    if (GetTimer(r) > 10)
        SetTimer(r, 10);
}
