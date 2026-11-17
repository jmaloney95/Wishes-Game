#include "global.h"
#include "data.h"
#include "wot_tester.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_specials.h"
#include "field_weather.h"
#include "follower_npc.h"
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
#include "quests.h"
#include "shop.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "util.h"
#include "wild_encounter.h"
#include "window.h"
#include "wot_randomizer.h"
#include "constants/battle_partner.h"
#include "constants/event_objects.h"
#include "constants/field_weather.h"
#include "constants/follower_npc.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/map_event_ids.h"
#include "constants/opponents.h"
#include "constants/region_map_sections.h"
#include "constants/rgb.h"
#include "constants/moves.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "wot_mega_dimension.h"
#include "wot_mystery_code.h"
#include "wot_nuzlocke.h"

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
#define STATE_PARTNER_SHIFT     13      // bits 15-13: the recruited partner (0 = none, 1-6 = sRivals[n - 1])
#define STATE_PARTNER_MASK      0x7
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

// The rogue Mega stands at the party's best level with perfect IVs, a fight
// worth bringing a partner to (12-16 under was too easy, even
// solo). It's never under 30 (portals open from Tradewind on, about Lv30), or
// under 45 once the player has reached Shin Tokyo.
// A rival is the easier fight: their Mega ace
// is 10 under the party's best and their own two 11-12 under. The
// dimension's wild Pokemon are their lines' first stages at about half the
// party's best (up to 2 under it).
#define MEGA_DIM_BOSS_EARLY_MIN_LEVEL 30
#define MEGA_DIM_BOSS_MIN_LEVEL       45
#define MEGA_DIM_RIVAL_ACE_BELOW      10
#define MEGA_DIM_RIVAL_TEAM_BELOW_MIN 11
#define MEGA_DIM_RIVAL_TEAM_BELOW_MAX 12
#define MEGA_DIM_LEGEND_LEVEL   70  // Mable's legends ignore the rule
#define MEGA_DIM_WILD_SPREAD    2

// VAR_WOT_MEGA_DIM_TIMERS bits 15-14: the one guaranteed rival.
// The first collapse of any portal makes it due; the next portal entered for
// the first time then has a rival in it.
#define TIMERS_INTRO_SHIFT  14
#define TIMERS_INTRO_MASK   0x3
enum
{
    RIVAL_INTRO_WAITING,    // no portal has collapsed yet
    RIVAL_INTRO_DUE,
    RIVAL_INTRO_DONE,
};

// Friendship: a partner levels up every 5 portal runs together,
// Lv1-Lv4, and their perk grows with it, reaching full strength at
// Lv4. A run is a portal collapsing with them along, whiteouts included
// (leaving by the exit portal is not a run).
#define FRIENDSHIP_RUNS_PER_LEVEL 5
// FRIENDSHIP_MAX_LEVEL is in wot_mega_dimension.h (the card lights its emblem there)
#define FRIENDSHIP_MAX_RUNS       255

struct MegaDimMega
{
    u16 baseSpecies;
    u16 megaSpecies;
    u16 stone;
    u16 moves[MAX_MON_MOVES]; // the rogue Mega's set: four attacks, no setup or status
    u16 zMove;                // which of those moves it throws as its Z-Move
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

// The 23 rogue Megas: the 19 fan Megas, in claim-flag order
// (FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + index), then 4 reskinned canon Megas
//, whose claims are kept elsewhere: see IsMegaClaimed. Darkrai, the
// fifth, is a Rift Sanctum exclusive and never a rogue.
#define FAN_MEGA_COUNT 19

static const struct MegaDimMega sMegaRoster[] =
{
    {SPECIES_AMOONGUSS,  SPECIES_AMOONGUSS_MEGA,  ITEM_AMOONGUSSITE, {MOVE_SLUDGE_BOMB, MOVE_ENERGY_BALL, MOVE_EARTH_POWER, MOVE_FOUL_PLAY}, MOVE_SLUDGE_BOMB},
    {SPECIES_AZUMARILL,  SPECIES_AZUMARILL_MEGA,  ITEM_AZUMARILLITE, {MOVE_LIQUIDATION, MOVE_PLAY_ROUGH, MOVE_SUPERPOWER, MOVE_AQUA_JET}, MOVE_LIQUIDATION},
    {SPECIES_BUTTERFREE, SPECIES_BUTTERFREE_MEGA, ITEM_BUTTERFRITE, {MOVE_BUG_BUZZ, MOVE_PSYCHIC, MOVE_HURRICANE, MOVE_ENERGY_BALL}, MOVE_PSYCHIC},
    {SPECIES_CONKELDURR, SPECIES_CONKELDURR_MEGA, ITEM_CONKELDURRITE, {MOVE_DRAIN_PUNCH, MOVE_MACH_PUNCH, MOVE_STONE_EDGE, MOVE_KNOCK_OFF}, MOVE_STONE_EDGE},
    {SPECIES_DUGTRIO,    SPECIES_DUGTRIO_MEGA,    ITEM_DUGTRIONITE, {MOVE_EARTHQUAKE, MOVE_STONE_EDGE, MOVE_IRON_HEAD, MOVE_THROAT_CHOP}, MOVE_EARTHQUAKE},
    {SPECIES_ELECTIVIRE, SPECIES_ELECTIVIRE_MEGA, ITEM_ELECTIVIRITE, {MOVE_WILD_CHARGE, MOVE_ICE_PUNCH, MOVE_CROSS_CHOP, MOVE_EARTHQUAKE}, MOVE_WILD_CHARGE},
    {SPECIES_FLYGON,     SPECIES_FLYGON_MEGA,     ITEM_FLYGONITE, {MOVE_EARTHQUAKE, MOVE_OUTRAGE, MOVE_FIRE_PUNCH, MOVE_STONE_EDGE}, MOVE_EARTHQUAKE},
    {SPECIES_GOLEM,      SPECIES_GOLEM_MEGA,      ITEM_GOLEMITE, {MOVE_EARTHQUAKE, MOVE_STONE_EDGE, MOVE_HEAVY_SLAM, MOVE_FIRE_PUNCH}, MOVE_STONE_EDGE},
    {SPECIES_JUMPLUFF,   SPECIES_JUMPLUFF_MEGA,   ITEM_JUMPLUFFITE, {MOVE_LEAF_STORM, MOVE_HURRICANE, MOVE_MOONBLAST, MOVE_GIGA_DRAIN}, MOVE_HURRICANE},
    {SPECIES_KABUTOPS,   SPECIES_KABUTOPS_MEGA,   ITEM_KABUTOPSITE, {MOVE_RAZOR_SHELL, MOVE_STONE_EDGE, MOVE_X_SCISSOR, MOVE_AQUA_JET}, MOVE_X_SCISSOR},
    {SPECIES_MAGMORTAR,  SPECIES_MAGMORTAR_MEGA,  ITEM_MAGMORTARITE, {MOVE_FIRE_BLAST, MOVE_THUNDERBOLT, MOVE_FOCUS_BLAST, MOVE_PSYCHIC}, MOVE_FIRE_BLAST},
    {SPECIES_MILOTIC,    SPECIES_MILOTIC_MEGA,    ITEM_MILOTICITE, {MOVE_HYDRO_PUMP, MOVE_MOONBLAST, MOVE_ICE_BEAM, MOVE_DRAGON_PULSE}, MOVE_ICE_BEAM},
    {SPECIES_OMASTAR,    SPECIES_OMASTAR_MEGA,    ITEM_OMASTARITE, {MOVE_ROCK_BLAST, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_EARTH_POWER}, MOVE_HYDRO_PUMP},
    {SPECIES_POLIWRATH,  SPECIES_POLIWRATH_MEGA,  ITEM_POLIWRATHITE, {MOVE_LIQUIDATION, MOVE_CLOSE_COMBAT, MOVE_ICE_PUNCH, MOVE_EARTHQUAKE}, MOVE_CLOSE_COMBAT},
    {SPECIES_SHIFTRY,    SPECIES_SHIFTRY_MEGA,    ITEM_SHIFTRITE, {MOVE_LEAF_BLADE, MOVE_KNOCK_OFF, MOVE_LOW_KICK, MOVE_ROCK_SLIDE}, MOVE_KNOCK_OFF},
    {SPECIES_SNORLAX,    SPECIES_SNORLAX_MEGA,    ITEM_SNORLAXITE, {MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_CRUNCH, MOVE_HEAVY_SLAM}, MOVE_DOUBLE_EDGE},
    {SPECIES_TORKOAL,    SPECIES_TORKOAL_MEGA,    ITEM_TORKOALITE, {MOVE_ERUPTION, MOVE_FLASH_CANNON, MOVE_SOLAR_BEAM, MOVE_EARTH_POWER}, MOVE_FLASH_CANNON},
    {SPECIES_TYPHLOSION, SPECIES_TYPHLOSION_MEGA, ITEM_TYPHLOSIONITE, {MOVE_ERUPTION, MOVE_EARTH_POWER, MOVE_SHADOW_BALL, MOVE_FOCUS_BLAST}, MOVE_SHADOW_BALL},
    {SPECIES_WHIMSICOTT, SPECIES_WHIMSICOTT_MEGA, ITEM_WHIMSICOTTITE, {MOVE_MOONBLAST, MOVE_ENERGY_BALL, MOVE_HURRICANE, MOVE_SHADOW_BALL}, MOVE_MOONBLAST},
    {SPECIES_MEGANIUM,   SPECIES_MEGANIUM_MEGA,   ITEM_MEGANIUMITE, {MOVE_SOLAR_BEAM, MOVE_MOONBLAST, MOVE_EARTH_POWER, MOVE_HYPER_VOICE}, MOVE_SOLAR_BEAM},
    {SPECIES_FERALIGATR, SPECIES_FERALIGATR_MEGA, ITEM_FERALIGITE, {MOVE_LIQUIDATION, MOVE_DRAGON_CLAW, MOVE_ICE_PUNCH, MOVE_CRUNCH}, MOVE_DRAGON_CLAW},
    {SPECIES_EXCADRILL,  SPECIES_EXCADRILL_MEGA,  ITEM_EXCADRITE, {MOVE_EARTHQUAKE, MOVE_IRON_HEAD, MOVE_ROCK_SLIDE, MOVE_X_SCISSOR}, MOVE_IRON_HEAD},
    {SPECIES_PYROAR,     SPECIES_PYROAR_MEGA,     ITEM_PYROARITE, {MOVE_FIRE_BLAST, MOVE_HYPER_VOICE, MOVE_DARK_PULSE, MOVE_GRASS_KNOT}, MOVE_DARK_PULSE},
};

STATIC_ASSERT(FAN_MEGA_COUNT == FLAG_WOT_MEGA_CLAIMED_WHIMSICOTT - FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + 1, MegaRosterMatchesClaimFlags);
STATIC_ASSERT(ARRAY_COUNT(sMegaRoster) - FAN_MEGA_COUNT <= WOT_PARTNER_SAVE_COUNT, MegaRosterExtraClaimsFit);
STATIC_ASSERT(ARRAY_COUNT(sMegaRoster) <= PORTAL_MEGA_MASK + 1, MegaRosterFitsRecord);

static const u8 sTown_TradewindTown[] = _("Tradewind Town");
static const u8 sTown_NationalPark[] = _("National Park");
static const u8 sTown_Ashlands[] = _("Ashlands");
static const u8 sTown_ShinTokyo[] = _("Shin Tokyo");

// Where portals open. Keep x/y in step with the portal
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
    {FLAG_WOT_MEGA_DIM_SEEN_SHIN_TOKYO, FLAG_HIDE_MEGA_PORTAL_6, MAP_SHIN_TOKYO,          28, 18, sTown_ShinTokyo},
};

STATIC_ASSERT(ARRAY_COUNT(sPortalSpots) <= PORTAL_SPOT_MASK + 1, MegaPortalSpotsFitRecord);

#define DIM_MAP_STAR_SUMMIT   5
#define DIM_RANDOM_MAP_COUNT  7
#define DIM_MAP_MARINE_CAVE   7
#define DIM_MAP_SKY_PILLAR    8
#define DIM_MAP_NAVEL_BOTTOM  9
#define DIM_MAP_NAVEL_TOP     10

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
    // Mega Lugia in Navel Rock's flooded bottom, Mega Ho-Oh on its summit.
    [DIM_MAP_NAVEL_BOTTOM] = {0, MAP_MEGA_DIMENSION_NAVEL_ROCK_BOTTOM, {{12, 19, 0}, {11, 13, 0}, {13, 19, 0}}},
    [DIM_MAP_NAVEL_TOP]    = {0, MAP_MEGA_DIMENSION_NAVEL_ROCK_TOP,    {{12, 20, 3}, {12,  9, 3}, {14, 19, 3}}},
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
    u16 music;      // each rival battles to a theme of their own
    u8 partnerId;   // PARTNER_MEGADIM_*: who they are once recruited
};

// The order is the script's switch order (MegaDim_EventScript_Rival,
// MegaDim_EventScript_PartnerTalk) and Hotel Z's VAR_0x8004 numbering (1-6).
static const struct MegaDimRival sRivals[] =
{
    {TRAINER_MEGADIM_CANARI,   OBJ_EVENT_GFX_ZA_CANARI,    MUS_HG_VS_RIVAL,         PARTNER_MEGADIM_CANARI},
    {TRAINER_MEGADIM_LIDA,     OBJ_EVENT_GFX_ZA_LIDA,      MUS_DP_VS_GALACTIC,      PARTNER_MEGADIM_LIDA},
    {TRAINER_MEGADIM_EMMA,     OBJ_EVENT_GFX_ZA_EMMA,      MUS_DP_VS_GALACTIC_BOSS, PARTNER_MEGADIM_EMMA},
    {TRAINER_MEGADIM_TAUNIE,   OBJ_EVENT_GFX_ZA_TAUNIE,    MUS_DP_VS_GYM_LEADER,    PARTNER_MEGADIM_TAUNIE},
    {TRAINER_MEGADIM_TULIP,    OBJ_EVENT_GFX_ZA_TULIP,     MUS_VS_FRONTIER_BRAIN,   PARTNER_MEGADIM_TULIP},
    {TRAINER_MEGADIM_JACINTHE, OBJ_EVENT_GFX_WOT_JACINTHE, MUS_DP_VS_CHAMPION,      PARTNER_MEGADIM_JACINTHE},
};

STATIC_ASSERT(ARRAY_COUNT(sRivals) <= STATE_PARTNER_MASK, MegaDimPartnersFitState);

STATIC_ASSERT(ARRAY_COUNT(sRivals) <= PORTAL_RIVAL_MASK, MegaDimRivalsFitRecord);

// --- Rift Sanctums ---------------------------------------------------
// After the game is beaten, each partner at max friendship can open one
// barrier in one dimension map. Behind it is a sanctum: three Mutrid holdouts,
// then a Shadow legend. One row per partner, in sRivals order. The legends,
// barriers and holdout teams are placeholders; the barrier and
// sanctum objects themselves are in the maps (porymap can move
// them), and arrive/back below must stay walkable.
struct MegaDimSanctum
{
    const u8 *ability;          // the partner's max-friendship ability
    const u8 *hint;             // what the barrier says to anyone else
    u16 barrierMap;             // the dimension map with the barrier
    u16 barrierFlag;            // its object's temp flag there
    u16 sanctumMap;
    s8 arriveX, arriveY;        // where the sanctum puts you
    s8 backX, backY;            // where leaving puts you, beside the barrier
    u16 legend;
    u8 legendLevel;
    u16 holdoutShadow[3];       // holdouts A-C: the Shadow each one fields...
    u16 holdoutMon[3];          // ...and the Pokemon beside it
    // A legend holding a stone fights as a rogue does: these moves,
    // perfect IVs, Mega on turn one, a Z-Move and a last stand. Catching it
    // keeps the stone. The others have neither, and their level-up moves.
    u16 legendItem;
    u16 legendMoves[MAX_MON_MOVES];
    u16 legendZMove;            // the move it throws as its Z-Move
};

static const struct MegaDimSanctum sSanctums[] =
{
    { // Canari
        COMPOUND_STRING("SURGE"),
        COMPOUND_STRING("A live fence of crackling energy\nbars the way.\pIt would take a real surge of\npower to overload it…"),
        MAP_MEGA_DIMENSION_ROUTE_2, FLAG_TEMP_10, MAP_MEGA_DIMENSION_SANCTUM_SURGE, 32, 34, 30, 21,
        SPECIES_RAIKOU, 70,
        {SPECIES_MANECTRIC, SPECIES_LUXRAY, SPECIES_LANTURN}, {SPECIES_MAGNETON, SPECIES_ELECTRODE, SPECIES_JOLTEON},
    },
    { // Lida
        COMPOUND_STRING("LIFELINE"),
        COMPOUND_STRING("The ground is torn open here,\nits far side out of reach.\pSomeone would have to be your\nlifeline across…"),
        MAP_MEGA_DIMENSION_NATIONAL_PARK, FLAG_TEMP_10, MAP_MEGA_DIMENSION_SANCTUM_LIFELINE, 13, 19, 24, 53,
        SPECIES_YVELTAL, 70,
        {SPECIES_GARCHOMP, SPECIES_DRAGONITE, SPECIES_FLYGON}, {SPECIES_SALAMENCE, SPECIES_HAXORUS, SPECIES_ALTARIA},
    },
    { // Emma
        COMPOUND_STRING("TIME FREEZE"),
        COMPOUND_STRING("A great clock seal blocks the\nway, its hands ticking backward.\pOnly someone who could stop\ntime could slip past it…"),
        MAP_MEGA_DIMENSION_TRADEWIND_TOWN, FLAG_TEMP_10, MAP_MEGA_DIMENSION_SANCTUM_TIME_FREEZE, 3, 2, 62, 39,
        // Cresselia, not Arceus: Shadow Arceus is the Shadow Log's
        // reward at Tower Top, and catching one here would fill its row early.
        SPECIES_CRESSELIA, 70,
        {SPECIES_ALAKAZAM, SPECIES_GALLADE, SPECIES_GARDEVOIR}, {SPECIES_BRONZONG, SPECIES_METAGROSS, SPECIES_ESPEON},
    },
    { // Taunie
        COMPOUND_STRING("RIVAL'S DASH"),
        COMPOUND_STRING("A howling wall of wind blocks\nthe way.\pIt would take a rival's dash\nto break through it…"),
        MAP_MEGA_DIMENSION_WALNUT_WOODS, FLAG_TEMP_1B, MAP_MEGA_DIMENSION_SANCTUM_DASH, 13, 17, 29, 32,
        SPECIES_LATIAS, 70,
        {SPECIES_KINGDRA, SPECIES_DRAGONAIR, SPECIES_RAPIDASH}, {SPECIES_SWELLOW, SPECIES_CROBAT, SPECIES_TALONFLAME},
    },
    { // Tulip
        COMPOUND_STRING("ILLUSION SIGHT"),
        COMPOUND_STRING("Something shimmers here… a wall\nthat isn't quite real.\pSomeone with an eye for\nillusions could see through it…"),
        MAP_MEGA_DIMENSION_SHIN_TOKYO, FLAG_TEMP_1B, MAP_MEGA_DIMENSION_SANCTUM_ILLUSION, 12, 18, 28, 49,
        SPECIES_DARKRAI, 70,
        {SPECIES_ABSOL, SPECIES_UMBREON, SPECIES_CLEFABLE}, {SPECIES_MISMAGIUS, SPECIES_BANETTE, SPECIES_ZOROARK},
        ITEM_DARKRANITE, {MOVE_DARK_VOID, MOVE_DARK_PULSE, MOVE_SLUDGE_BOMB, MOVE_ICE_BEAM}, MOVE_DARK_PULSE,
    },
    { // Jacinthe
        COMPOUND_STRING("BREAKTHROUGH"),
        COMPOUND_STRING("A wall of ancient runestones\nseals the way.\pIt would take real strength to\nbreak through it…"),
        MAP_MEGA_DIMENSION_ASHLANDS, FLAG_TEMP_1B, MAP_MEGA_DIMENSION_SANCTUM_BREAKTHROUGH, 8, 28, 6, 14,
        SPECIES_REGIGIGAS, 70,
        {SPECIES_ONIX, SPECIES_MAROWAK, SPECIES_CAMERUPT}, {SPECIES_GOLEM, SPECIES_RHYPERIOR, SPECIES_AGGRON},
    },
};

STATIC_ASSERT(ARRAY_COUNT(sSanctums) == ARRAY_COUNT(sRivals), MegaDimOneSanctumPerPartner);

#define SANCTUM_LEGEND_CAUGHT   (1 << 0)
#define SANCTUM_HOLDOUT(k)      (1 << (1 + (k)))
#define SANCTUM_HOLDOUTS_ALL    (SANCTUM_HOLDOUT(0) | SANCTUM_HOLDOUT(1) | SANCTUM_HOLDOUT(2))
// Same byte, nothing to do with the sanctums: the saved moveset is
// current. Saves from before the partners had real sets lack it, and open with
// the sets in battle_partners.party rather than their old level-up leftovers.
#define PARTNER_MOVES_CURRENT   (1 << 4)

// --- Mable's quest chain ------------------------------------------------------
// Two Minutes Is a Long Time: report the stones of all 23 rogue Megas to her
// (she only counts the dimension's own claim flags). The day after her tests
// start she calls about four power signatures (Primal Signatures: Primal
// Groudon and Kyogre, Mega Lugia and Ho-Oh); once all four are caught she
// calls about Mega Rayquaza (Sky-High Mega). Each legend waits in a portal,
// as the boss, while its quest is on and until it's caught.
enum
{
    MABLE_STAGE_TALLY,          // collecting the 23 stones
    MABLE_STAGE_TESTS,          // all 23 in: her Primal call comes on a later portal day
    MABLE_STAGE_PRIMALS,        // Primal Signatures: catch Primal Groudon, Primal Kyogre, Mega Lugia and Mega Ho-Oh
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
    // A Mega fights as a rogue does: this set and a Z-Move from zMove. The
    // Primals have neither (MOVE_NONE), and keep their level-up moves.
    u16 moves[MAX_MON_MOVES];
    u16 zMove;
};

// Where each legend waits: Groudon on the Star Summit copy,
// Kyogre in a gray Marine Cave, Rayquaza atop a gray Sky Pillar, Lugia at
// the bottom of a gray Navel Rock and Ho-Oh on its summit. No other
// Pokemon appear beside a legend. Append only: a portal's record keeps the
// row number (BOSS_LEGEND_FIRST + n).
static const struct MegaDimLegend sLegends[] =
{
    {SPECIES_GROUDON,  SPECIES_GROUDON_PRIMAL, ITEM_RED_ORB,  MOVE_NONE,          FLAG_WOT_CAUGHT_PRIMAL_GROUDON, MABLE_STAGE_PRIMALS,  DIM_MAP_STAR_SUMMIT},
    {SPECIES_KYOGRE,   SPECIES_KYOGRE_PRIMAL,  ITEM_BLUE_ORB, MOVE_NONE,          FLAG_WOT_CAUGHT_PRIMAL_KYOGRE,  MABLE_STAGE_PRIMALS,  DIM_MAP_MARINE_CAVE},
    {SPECIES_RAYQUAZA, SPECIES_RAYQUAZA_MEGA,  ITEM_NONE,     MOVE_DRAGON_ASCENT, FLAG_WOT_CAUGHT_MEGA_RAYQUAZA,  MABLE_STAGE_RAYQUAZA, DIM_MAP_SKY_PILLAR,
        {MOVE_DRAGON_ASCENT, MOVE_EXTREME_SPEED, MOVE_EARTHQUAKE, MOVE_DRACO_METEOR}, MOVE_DRAGON_ASCENT},
    // The Mega Log's row for Zygarde. No Mable stage matches, so no
    // portal ever holds it: it waits in the Drained Hotel.
    {SPECIES_ZYGARDE_COMPLETE, SPECIES_ZYGARDE_MEGA, ITEM_ZYGARDITE, MOVE_NONE,   FLAG_WOT_CAUGHT_ZYGARDE,        MABLE_STAGE_DONE + 1, 0},
    // Primal Signatures' other two.
    {SPECIES_LUGIA,    SPECIES_LUGIA_MEGA,     ITEM_LUGIANITE, MOVE_NONE,         FLAG_WOT_CAUGHT_MEGA_LUGIA,     MABLE_STAGE_PRIMALS,  DIM_MAP_NAVEL_BOTTOM,
        {MOVE_AEROBLAST, MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_EARTH_POWER}, MOVE_AEROBLAST},
    {SPECIES_HO_OH,    SPECIES_HO_OH_MEGA,     ITEM_HO_OHNITE, MOVE_NONE,         FLAG_WOT_CAUGHT_MEGA_HO_OH,     MABLE_STAGE_PRIMALS,  DIM_MAP_NAVEL_TOP,
        {MOVE_SACRED_FIRE, MOVE_BRAVE_BIRD, MOVE_EARTHQUAKE, MOVE_STONE_EDGE}, MOVE_SACRED_FIRE},
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
#define MEGA_DIM_DEALT_MAX        (MEGA_DIM_VISIT_SPECIES * MEGA_DIM_MON_PER_AREA)

// What the visible wild Pokemon are dealt from, in every dimension map alike
// (each dealt group draws one, all equally likely): the first stage of each
// rogue Mega's line, in sMegaRoster order. The five water lines (Azurill,
// Kabuto, Omanyte, Poliwag, Feebas) are left to the dimension maps' surfing
// tables in wild_encounters.json instead. The maps have no land table: this
// list replaces it.
static const u16 sDimWildSpecies[] =
{
    SPECIES_FOONGUS,
    SPECIES_CATERPIE,
    SPECIES_TIMBURR,
    SPECIES_DIGLETT,
    SPECIES_ELEKID,
    SPECIES_TRAPINCH,
    SPECIES_GEODUDE,
    SPECIES_HOPPIP,
    SPECIES_MAGBY,
    SPECIES_SEEDOT,
    SPECIES_MUNCHLAX,
    SPECIES_TORKOAL,
    SPECIES_CYNDAQUIL,
    SPECIES_COTTONEE,
    SPECIES_CHIKORITA,
    SPECIES_TOTODILE,
    SPECIES_DRILBUR,
    SPECIES_LITLEO,
};

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
static EWRAM_DATA bool8 sShinyCue = FALSE; // a shiny is in sight: play SE_WOT_SHINY once the field runs
static EWRAM_DATA bool8 sNoShinyReroll = FALSE; // back from a sanctum: this map load rolls no shinies

static EWRAM_DATA u8 sLastPlaySecond = 0;

// Where this map's boss stands, for the HUD's signal and the aura
// on its sprite. RefreshBossTarget sets them each time the field starts.
static EWRAM_DATA bool8 sBossHere = FALSE;
static EWRAM_DATA s16 sBossX = 0;
static EWRAM_DATA s16 sBossY = 0;
static EWRAM_DATA u8 sBossLocalId = 0;      // its object, LOCALID_NONE if a rival has it
static EWRAM_DATA u16 sBossAuraColor = 0;
static EWRAM_DATA u8 sBossAuraStep = 0;

// Z-Shards, the bag item Hotel Z's desk takes. First-pass numbers.
#define ZSHARDS_WILD            1
#define ZSHARDS_WILD_SHINY      5
#define ZSHARDS_ROGUE_FIRST     10
#define ZSHARDS_ROGUE_REMATCH   6
#define ZSHARDS_ROGUE_THREW_Z   3
#define ZSHARDS_LEGEND          20
#define ZSHARD_FLASH_FRAMES     90

// The Zygarde arc.
enum
{
    ZYGARDE_STAGE_NONE,
    ZYGARDE_STAGE_DRAIN_CALLED,     // after Mega Rayquaza: Mable's call, the desk is next
    ZYGARDE_STAGE_AZ_BEATEN,        // the rift at the desk leads to Mega Floette
    ZYGARDE_STAGE_SEALED,           // Zygarde waits in the Drained Hotel
};
#define ZYGARDE_STAGE_BITS      2
#define ZYGARDE_CELLS_FOR_CALL  5
#define ZYGARDE_CELLS_FOR_DESK  20
#define ZYGARDE_CLAIMS_FOR_DOG  3
#define ZYGARDE_CELL_SECONDS    15

static EWRAM_DATA bool8 sRogueThrewZ = FALSE;
static EWRAM_DATA u8 sShardFlashFrames = 0;     // the HUD's "+N", with no text box to cost clock time
static EWRAM_DATA u8 sShardFlashAmount = 0;

static void GiveZShards(u32 count)
{
    AddBagItem(ITEM_Z_SHARD, count);
    ConvertIntToDecimalStringN(gStringVar1, count, STR_CONV_MODE_LEFT_ALIGN, 3);
}
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
    case OBJ_EVENT_PAL_TAG_WOT_ZYGARDE_CELL:    // not the player's, but it's the one green thing here
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

// The 19 fan Megas have flags. No flags are left for the other four (every
// "unused" one left is a Mable bit-field or in the daily-reset range), so
// their claims are bit 6 of the first four partners' spare save bytes. That
// leaves the save layout as it was, and older saves read as unclaimed.
#define EXTRA_MEGA_CLAIMED  (1 << 6)

static bool32 IsMegaClaimed(u32 index)
{
    if (index < FAN_MEGA_COUNT)
        return FlagGet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + index);
    return (gSaveBlock1Ptr->wotPartners[index - FAN_MEGA_COUNT].sanctum & EXTRA_MEGA_CLAIMED) != 0;
}

void WotMegaDim_SetMegaClaimed(u32 index, bool32 claimed)
{
    if (index < FAN_MEGA_COUNT)
    {
        if (claimed)
            FlagSet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + index);
        else
            FlagClear(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + index);
    }
    else if (index < ARRAY_COUNT(sMegaRoster))
    {
        if (claimed)
            gSaveBlock1Ptr->wotPartners[index - FAN_MEGA_COUNT].sanctum |= EXTRA_MEGA_CLAIMED;
        else
            gSaveBlock1Ptr->wotPartners[index - FAN_MEGA_COUNT].sanctum &= ~EXTRA_MEGA_CLAIMED;
    }
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

// --- Partners ------------------------------------------------------
// A rival beaten in the dimension is unlocked: never met there again, and
// recruitable in Hotel Z. "Beaten" is the rival's own trainer flag (the
// engine sets it on a win; the rival scripts no longer clear it).
static bool32 IsRivalBeaten(u32 i)
{
    return FlagGet(TRAINER_FLAGS_START + sRivals[i].trainerId);
}

// The recruited partner: 0 = none, else 1 + index into sRivals.
static u32 GetPartner(void)
{
    u32 partner = (VarGet(VAR_WOT_MEGA_DIM_STATE) >> STATE_PARTNER_SHIFT) & STATE_PARTNER_MASK;

    return partner <= ARRAY_COUNT(sRivals) ? partner : 0;
}

static void SetPartnerBits(u32 partner)
{
    u32 value = VarGet(VAR_WOT_MEGA_DIM_STATE) & ~(STATE_PARTNER_MASK << STATE_PARTNER_SHIFT);

    VarSet(VAR_WOT_MEGA_DIM_STATE, value | ((partner & STATE_PARTNER_MASK) << STATE_PARTNER_SHIFT));
}

static bool32 IsPartner(u32 partnerId)
{
    u32 partner = GetPartner();

    return partner != 0 && sRivals[partner - 1].partnerId == partnerId;
}

// --- Friendship -----------------------------------------------------
// Saved per partner in gSaveBlock1Ptr->wotPartners, in sRivals order.
STATIC_ASSERT(ARRAY_COUNT(sRivals) == WOT_PARTNER_SAVE_COUNT, MegaDimPartnerSaveFits);

// The partner card (wot_partner_card.c): the chosen partner, 1-6 (0 = none).
u32 WotPartner_GetCurrent(void)
{
    return GetPartner();
}

void WotPartner_SetCurrent(u32 partner)
{
    SetPartnerBits(partner <= ARRAY_COUNT(sRivals) ? partner : 0);
}

static u32 LevelForRuns(u32 runs)
{
    return min(1 + runs / FRIENDSHIP_RUNS_PER_LEVEL, FRIENDSHIP_MAX_LEVEL);
}

u32 WotPartner_GetRuns(u32 i)
{
    return gSaveBlock1Ptr->wotPartners[i].runs;
}

u32 WotPartner_GetLevel(u32 i)
{
    return LevelForRuns(gSaveBlock1Ptr->wotPartners[i].runs);
}

// The friendship level of partnerId if they're the current partner, else 0:
// a perk only works with them along.
static u32 GetPerkLevel(u32 partnerId)
{
    return IsPartner(partnerId) ? WotPartner_GetLevel(GetPartner() - 1) : 0;
}

// A portal collapsed with them along: one more run, and a level-up to tell
// about once the player is back on the field.
static void CountPartnerRun(u32 i)
{
    struct WotPartnerSave *save = &gSaveBlock1Ptr->wotPartners[i];
    u32 before = LevelForRuns(save->runs);

    if (save->runs < FRIENDSHIP_MAX_RUNS)
        save->runs++;
    if (LevelForRuns(save->runs) > before)
        save->pendingLevel = LevelForRuns(save->runs);
}

static s32 FindPendingFriendship(void)
{
    u32 i;

    for (i = 0; i < WOT_PARTNER_SAVE_COUNT; i++)
    {
        if (gSaveBlock1Ptr->wotPartners[i].pendingLevel != 0)
            return i;
    }
    return -1;
}

// What each partner's perk does at each friendship level (the card's first
// line, the level-up call), and the next level's change (the card's second).
static const u8 *const sPerkNow[WOT_PARTNER_SAVE_COUNT][FRIENDSHIP_MAX_LEVEL] =
{
    { // Canari
        COMPOUND_STRING("Shiny odds in rifts x1.25"),
        COMPOUND_STRING("Shiny odds in rifts x1.5"),
        COMPOUND_STRING("Shiny odds in rifts x1.75"),
        COMPOUND_STRING("Shiny odds in rifts x2"),
    },
    { // Lida
        COMPOUND_STRING("Lucario holds Lucarionite"),
        COMPOUND_STRING("Mega Lucario: perfect IVs"),
        COMPOUND_STRING("Mega Lucario: IVs, steady level"),
        COMPOUND_STRING("Mega Lucario: steady level, max EVs"),
    },
    { // Emma
        COMPOUND_STRING("Portal clock lasts 2:30"),
        COMPOUND_STRING("Portal clock lasts 3:00"),
        COMPOUND_STRING("Portal clock lasts 3:30"),
        COMPOUND_STRING("Portal clock lasts 4:00"),
    },
    { // Taunie
        COMPOUND_STRING("Reads which Mega is inside"),
        COMPOUND_STRING("Reads the Mega and rivals"),
        COMPOUND_STRING("Reads Mega, rivals, shinies"),
        COMPOUND_STRING("Reads all, even a shiny Mega"),
    },
    { // Tulip
        COMPOUND_STRING("Gardevoir holds Gardevoirite"),
        COMPOUND_STRING("Mega Gardevoir: perfect IVs"),
        COMPOUND_STRING("Mega Gardevoir: IVs, steady level"),
        COMPOUND_STRING("Mega Gardevoir: steady level, max EVs"),
    },
    { // Jacinthe
        COMPOUND_STRING("Mawile holds Mawilite"),
        COMPOUND_STRING("Mega Mawile: perfect IVs"),
        COMPOUND_STRING("Mega Mawile: IVs, steady level"),
        COMPOUND_STRING("Mega Mawile: steady level, max EVs"),
    },
};

static const u8 *const sPerkNext[WOT_PARTNER_SAVE_COUNT][FRIENDSHIP_MAX_LEVEL - 1] =
{
    { COMPOUND_STRING("shiny x1.5"), COMPOUND_STRING("shiny x1.75"), COMPOUND_STRING("shiny x2") },
    { COMPOUND_STRING("perfect IVs"), COMPOUND_STRING("steady level"), COMPOUND_STRING("max EVs") },
    { COMPOUND_STRING("clock 3:00"), COMPOUND_STRING("clock 3:30"), COMPOUND_STRING("clock 4:00") },
    { COMPOUND_STRING("spots rivals"), COMPOUND_STRING("spots shinies"), COMPOUND_STRING("spots a shiny Mega") },
    { COMPOUND_STRING("perfect IVs"), COMPOUND_STRING("steady level"), COMPOUND_STRING("max EVs") },
    { COMPOUND_STRING("perfect IVs"), COMPOUND_STRING("steady level"), COMPOUND_STRING("max EVs") },
};

#include "data/wot_partner_lines.h"

// A partner's line for their friendship level, for
// `msgbox gWotPartnerLine`. Kept apart from gStringVar4, which the message
// box expands it into.
EWRAM_DATA u8 gWotPartnerLine[400] = {0};

// Talking to the partner inside the dimension.
void WotPartner_BufferFieldLine(void)
{
    u32 partner = GetPartner();

    gWotPartnerLine[0] = EOS;
    if (partner != 0)
        StringCopy(gWotPartnerLine, sPartnerFieldLines[partner - 1][WotPartner_GetLevel(partner - 1) - 1]);
}

// Hotel Z, VAR_0x8004 = the rival (1-6): from friendship Lv2 their greeting
// is buffered and VAR_RESULT = 0xFF; at Lv1 VAR_RESULT is left alone, so the
// script carries on to their pitch.
void WotPartner_BufferHotelLine(void)
{
    u32 rival = gSpecialVar_0x8004, level;

    if (rival == 0 || rival > WOT_PARTNER_SAVE_COUNT)
        return;
    level = WotPartner_GetLevel(rival - 1);
    if (level < 2)
        return;
    StringCopy(gWotPartnerLine, sPartnerHotelLines[rival - 1][level - 2]);
    gSpecialVar_Result = 0xFF;
}

const u8 *WotPartner_GetPerkText(u32 i, u32 level)
{
    return sPerkNow[i][min(max(level, 1), FRIENDSHIP_MAX_LEVEL) - 1];
}

// The next level's change, or NULL at the top.
const u8 *WotPartner_GetNextPerkText(u32 i)
{
    u32 level = WotPartner_GetLevel(i);

    return level >= FRIENDSHIP_MAX_LEVEL ? NULL : sPerkNext[i][level - 1];
}

u32 WotPartner_GetRunsForNextLevel(u32 i)
{
    return WotPartner_GetLevel(i) * FRIENDSHIP_RUNS_PER_LEVEL;
}

static u32 GetRivalIntro(void)
{
    return (VarGet(VAR_WOT_MEGA_DIM_TIMERS) >> TIMERS_INTRO_SHIFT) & TIMERS_INTRO_MASK;
}

static void SetRivalIntro(u32 stage)
{
    u32 value = VarGet(VAR_WOT_MEGA_DIM_TIMERS) & ~(TIMERS_INTRO_MASK << TIMERS_INTRO_SHIFT);

    VarSet(VAR_WOT_MEGA_DIM_TIMERS, value | ((stage & TIMERS_INTRO_MASK) << TIMERS_INTRO_SHIFT));
}

// A rival for a new portal: one not yet beaten and not the current partner,
// or 0 if none is left.
static u32 PickRival(void)
{
    u8 pool[ARRAY_COUNT(sRivals)];
    u32 i, count = 0;

    for (i = 0; i < ARRAY_COUNT(sRivals); i++)
    {
        if (!IsRivalBeaten(i) && i + 1 != GetPartner())
            pool[count++] = i;
    }
    return count == 0 ? 0 : 1 + pool[Random() % count];
}

extern const u8 MegaDim_EventScript_PartnerTalk[];

// Through a portal: the partner (if any) comes along as an NPC follower and
// battle partner. CreateFollowerNPC spawns it hidden; it appears on arrival.
static void StartPartnerFollower(void)
{
    u32 partner = GetPartner();

    if (partner == 0 || PlayerHasFollowerNPC())
        return;
    SetFollowerNPCData(FNPC_DATA_BATTLE_PARTNER, sRivals[partner - 1].partnerId);
    CreateFollowerNPC(sRivals[partner - 1].graphicsId, FNPC_ALL_LAND, MegaDim_EventScript_PartnerTalk);
    FlagSet(FLAG_WOT_MEGA_DIM_PARTNER_ON);
}

// Out of the dimension: the partner stays behind (they never walk the normal
// world). onField: the field is running (a script), so the object can be
// removed; otherwise (whiteout) only the data is cleared and the next map
// load spawns nothing.
static void StopPartnerFollower(bool32 onField)
{
    if (!FlagGet(FLAG_WOT_MEGA_DIM_PARTNER_ON))
        return;
    FlagClear(FLAG_WOT_MEGA_DIM_PARTNER_ON);
    if (!PlayerHasFollowerNPC())
        return;
    if (onField)
        DestroyFollowerNPC();
    else
        ClearFollowerNPCData();
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
// yesterday's. Kept mod 16. (Not the engine's daily reset in clock.c, which
// runs at midnight: keep the portals off it, or they would roll twice a day.)
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

// The vars meant something else in older saves (one portal a day). The
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

// Whether Mable's quests still want legend i: uncaught, and her current step
// calls for it -- or an earlier step did and it's still loose. (Lugia and Ho-Oh
// joined the Primal step after some saves were already past it.)
static bool32 IsLegendWanted(u32 i)
{
    u32 stage = GetMableStage();

    if (FlagGet(sLegends[i].caughtFlag))
        return FALSE;
    return sLegends[i].stage == stage || (sLegends[i].stage < stage && stage <= MABLE_STAGE_DONE);
}

// Whether every legend Mable's step `stage` calls for is caught.
static bool32 AreStageLegendsCaught(u32 stage)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sLegends); i++)
    {
        if (sLegends[i].stage == stage && !FlagGet(sLegends[i].caughtFlag))
            return FALSE;
    }
    return TRUE;
}

// A legend (sLegends index) Mable's quests still want that no other open
// portal holds, picked at random, or ARRAY_COUNT(sLegends) if there's none.
static u32 GetWantedLegend(u32 except)
{
    u8 wanted[ARRAY_COUNT(sLegends)];
    u32 i, count = 0;

    for (i = 0; i < ARRAY_COUNT(sLegends); i++)
    {
        if (IsLegendWanted(i) && !IsBossHeld(BOSS_LEGEND_FIRST + i, except))
            wanted[count++] = i;
    }
    return count == 0 ? ARRAY_COUNT(sLegends) : wanted[Random() % count];
}

// Rolls a new portal into record r on this spot: a dimension copy whose real
// map the player has visited (the Route 2 copy always qualifies), a boss, the
// 1-in-5 rival, a seed for the rest, and a full clock. The boss is a legend
// Mable's quest still needs if no other portal holds it (no rival then);
// otherwise a Mega nobody has claimed and no other open portal holds (else
// any unclaimed one; once all 23 are claimed, any of them -- rematches, no
// stone). `announce`: the 8:00 spawn, which Lida calls in.
// A rogue Mega for record r: one nobody has claimed and no other open portal
// holds, else any unclaimed one; once all 23 are claimed, any of them.
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
    // All 23 claimed: any of them, still keeping clear of the other open
    // portals' (The last one claimed kept turning up, because
    // every portal opened near the end held it).
    if (count == 0)
    {
        for (i = 0; i < ARRAY_COUNT(sMegaRoster); i++)
        {
            if (!IsBossHeld(i, r))
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
        rival = PickRival();

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
// 23 stones, portals only open for the legends her quests still need. Portals still open
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
        // Past the tally, portals open only for Mable's legends, until her
        // chain is done: then rematches again, or Cells, the Rogue
        // Core and the sanctums would have no rifts left.
        if (GetMableStage() >= MABLE_STAGE_TESTS && GetMableStage() < MABLE_STAGE_DONE
         && GetWantedLegend(RECORD_NONE) >= ARRAY_COUNT(sLegends))
            break;
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

    // A partner never walks the normal world (a save loaded outside, say).
    if (!WotMegaDim_IsActive())
        StopPartnerFollower(FALSE);

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
extern const u8 MegaDim_EventScript_TwoHearts[];
extern const u8 MegaDim_EventScript_FriendshipUp[];
extern const u8 MegaDim_EventScript_MablePrimalCall[];
extern const u8 MegaDim_EventScript_MableRayquazaCall[];
extern const u8 MegaDim_EventScript_EmmaCellCall[];
extern const u8 MegaDim_EventScript_MableDrainCall[];

// Once a second on the field (not inside): a call that's due rings as soon as
// the player is free -- Mable's first, then Lida's portal alert.
static bool32 WotZygarde_EmmaCallDue(void);
static bool32 WotZygarde_DrainCallDue(void);

static void TryStartFieldCall(void)
{
    const u8 *script = NULL;

    if (!FlagGet(FLAG_WOT_MEGA_DOOMSDAY_INTRO) || !FlagGet(FLAG_WOT_MEGA_DIM_V2)
     || gMain.callback2 != CB2_Overworld || ArePlayerFieldControlsLocked()
     || ScriptContext_IsEnabled() || gPaletteFade.active || MapNamePopupIsActive())
        return;

    if (FlagGet(FLAG_WOT_TWO_HEARTS_PENDING))
        script = MegaDim_EventScript_TwoHearts;
    else if (FindPendingFriendship() >= 0)
        script = MegaDim_EventScript_FriendshipUp;
    else if (GetMableStage() == MABLE_STAGE_TESTS
     && GetFlagBits(FLAG_WOT_MABLE_DAY_START, MABLE_DAY_BITS) != GetToday())
        script = MegaDim_EventScript_MablePrimalCall;
    else if (GetMableStage() == MABLE_STAGE_RAYQUAZA_CALL)
        script = MegaDim_EventScript_MableRayquazaCall;
    else if (WotZygarde_EmmaCallDue())
        script = MegaDim_EventScript_EmmaCellCall;
    else if (WotZygarde_DrainCallDue())
        script = MegaDim_EventScript_MableDrainCall;
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

// Emma along: the portal's clock runs slow. The record counts whole units of
// the 2:00 clock; with Emma each unit lasts (4 + level)/4 seconds, so 2:00
// reads 2:30 at friendship Lv1 up to 4:00 at Lv4
// The quarter-seconds already spent of the current unit live
// here, for the portal the player is inside.
static EWRAM_DATA u8 sClockAcc = 0;

static u32 GetQuartersPerUnit(void)
{
    return 4 + GetPerkLevel(PARTNER_MEGADIM_EMMA);
}

// A fresh portal's clock in seconds: 2:00, longer with Emma along.
static u32 GetFullClockSeconds(void)
{
    return MEGA_DIM_TIME_LIMIT * GetQuartersPerUnit() / 4;
}

static u32 GetSecondsLeft(u32 r)
{
    u32 units = GetTimer(r);

    if (units == 0)
        return 0;
    return (units * GetQuartersPerUnit() - (r == GetSession() ? sClockAcc : 0)) / 4;
}

static void SetSecondsLeft(u32 r, u32 seconds)
{
    u32 perUnit = GetQuartersPerUnit();
    u32 quarters = seconds * 4;
    u32 units = (quarters + perUnit - 1) / perUnit;

    SetTimer(r, units);
    sClockAcc = units * perUnit - quarters;
}

static u32 GetSessionTimeLeft(void)
{
    return GetSecondsLeft(GetSession());
}

// The boss glows, so it stands out from the wild Pokemon around it.
// A slow pulse on its palette. Blending writes the faded buffer from the
// unfaded one, so it steps aside while anything else is fading the screen.
#define BOSS_AURA_PERIOD_MASK 63
#define BOSS_AURA_COLOR       RGB(31, 8, 24)
#define BOSS_AURA_COLOR_SHINY RGB(31, 26, 8)

static void UpdateBossAura(void)
{
    u8 objId;
    u32 coeff;

    if (!sBossHere || sBossLocalId == LOCALID_NONE || gPaletteFade.active
     || gWeatherPtr->palProcessingState != WEATHER_PAL_STATE_IDLE)
        return;
    objId = GetObjectEventIdByLocalIdAndMap(sBossLocalId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    if (objId >= OBJECT_EVENTS_COUNT)
        return;
    sBossAuraStep = (sBossAuraStep + 1) & BOSS_AURA_PERIOD_MASK;
    coeff = (sBossAuraStep <= BOSS_AURA_PERIOD_MASK / 2 ? sBossAuraStep : BOSS_AURA_PERIOD_MASK - sBossAuraStep) / 4;
    BlendPalette(OBJ_PLTT_ID(gSprites[gObjectEvents[objId].spriteId].oam.paletteNum), 16, coeff, sBossAuraColor);
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

    if (sShinyCue && gMain.callback2 == CB2_Overworld && !gPaletteFade.active
     && !ScriptContext_IsEnabled() && !ArePlayerFieldControlsLocked())
    {
        sShinyCue = FALSE;
        PlaySE(SE_WOT_SHINY);
    }
    UpdateBossAura();

    r = GetSession();
    timeLeft = GetSecondsLeft(r);
    if (timeLeft != 0 && gSaveBlock2Ptr->playTimeSeconds != sLastPlaySecond)
    {
        sLastPlaySecond = gSaveBlock2Ptr->playTimeSeconds;
        SetSecondsLeft(r, --timeLeft);
    }
    if (timeLeft == 0 && VarGet(VAR_TEMP_1) == 0)
        VarSet(VAR_TEMP_1, 1);
}

// Back out to the portal record r came through: onto it, to step out of it
//, or in front of it (a whiteout).
static void SetReturnWarp(u32 r, bool32 onPortal)
{
    const struct MegaDimSpot *spot = &sPortalSpots[GetRecordSpot(r)];

    SetWarpDestination(MAP_GROUP(spot->srcMap), MAP_NUM(spot->srcMap), WARP_ID_NONE, spot->x, spot->y + (onPortal ? 0 : 1));
    sForceOnFoot = TRUE;
}

// The way out that collapses the portal: the player goes back to it and it's
// gone.
static void CollapseSession(bool32 whiteout)
{
    u32 r = GetSession();

    // A run seen through to the collapse with a partner along: Two Hearts One
    // Rift (a whiteout doesn't count). The quest completes back on the field.
    // Friendship counts every run together, failed ones too.
    if (FlagGet(FLAG_WOT_MEGA_DIM_PARTNER_ON))
    {
        if (!whiteout)
            FlagSet(FLAG_WOT_TWO_HEARTS_PENDING);
        if (GetPartner() != 0)
            CountPartnerRun(GetPartner() - 1);
    }
    if (GetRivalIntro() == RIVAL_INTRO_WAITING)
        SetRivalIntro(RIVAL_INTRO_DUE);
    StopPartnerFollower(!whiteout);
    FlagClear(FLAG_WOT_SMART_WILD_AI);
    SetReturnWarp(r, !whiteout);
    ClosePortal(r);
    SetSession(RECORD_NONE);
}

// overworld.c DoWhiteOut: whiting out inside collapses the portal and sends
// the player back to it rather than to the last Pokemon Center. No
// setrespawn: that would outlive the visit.
bool32 WotMegaDim_TrySetWhiteoutWarp(void)
{
    // The Drained Hotel is in the dimension but behind no portal: a normal whiteout.
    if (!WotMegaDim_IsActive() || GetSession() == RECORD_NONE)
        return FALSE;
    CollapseSession(TRUE);
    return TRUE;
}

// Set while a partner battle is being set up and fought: the player's party
// then holds 3 of theirs and the partner's 3, so their real team is the copy
// saved before the battle trimmed it.
static EWRAM_DATA bool8 sPartnerBattle = FALSE;

// The best level among the three the player would bring: a partner battle only
// uses the first three healthy Pokemon (follower_npc.c
// ChooseFirstThreeEligibleMons), so a Lv80 sitting in slot 5 stays out of it.
static u8 GetBroughtLevel(void)
{
    u32 i, level, highest = 1, brought = 0;

    for (i = 0; i < PARTY_SIZE && brought < MULTI_PARTY_SIZE; i++)
    {
        struct Pokemon *mon = sPartnerBattle ? GetSavedPlayerPartyMon(i) : &gPlayerParty[i];

        if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE
         || GetMonData(mon, MON_DATA_IS_EGG)
         || GetMonData(mon, MON_DATA_HP) == 0)
            continue;
        brought++;
        level = GetMonData(mon, MON_DATA_LEVEL);
        if (level > highest)
            highest = level;
    }
    return highest;
}

// What the dimension's levels key off: with a partner along, the three the
// player brings (The partner and boss both matched a benched Lv80);
// alone, the whole party, since all of it can battle.
static u8 GetHighestPartyLevel(void)
{
    u32 i, level, highest = 1;

    if (sPartnerBattle || FlagGet(FLAG_WOT_MEGA_DIM_PARTNER_ON))
        return GetBroughtLevel();
    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gPlayerParty[i];

        if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE
         || GetMonData(mon, MON_DATA_IS_EGG))
            continue;
        level = GetMonData(mon, MON_DATA_LEVEL);
        if (level > highest)
            highest = level;
    }
    return highest;
}

// battle_setup.c: a talk-to wild Pokemon or the boss becomes a 2v1 with the
// partner (rival fights already add the follower partner on their own).
bool32 WotMegaDim_HasPartnerHere(void)
{
    return WotMegaDim_IsActive() && FlagGet(FLAG_WOT_MEGA_DIM_PARTNER_ON) && FollowerNPCIsBattlePartner();
}

// A partner stands about 3 under what the player brings, never over it, so
// they back the player up instead of carrying the fight.
#define MEGA_DIM_PARTNER_LEVELS_BELOW 3
#define MEGA_DIM_PARTNER_LEVEL_SPREAD 2

// The order a partner's three go out in, by the saved permutation index:
// sTeamOrders[order][position] = their slot in battle_partners.party.
static const u8 sTeamOrders[][WOT_PARTNER_TEAM_SIZE] =
{
    {0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0},
};

const u8 *WotPartner_GetOrder(u32 i)
{
    u32 order = gSaveBlock1Ptr->wotPartners[i].order;

    return sTeamOrders[order < ARRAY_COUNT(sTeamOrders) ? order : 0];
}

// Save the order whose position p holds original slot slots[p].
void WotPartner_SetOrder(u32 i, const u8 *slots)
{
    u32 order;

    for (order = 0; order < ARRAY_COUNT(sTeamOrders); order++)
    {
        if (sTeamOrders[order][0] == slots[0] && sTeamOrders[order][1] == slots[1])
        {
            gSaveBlock1Ptr->wotPartners[i].order = order;
            return;
        }
    }
}

// Keep a mon's four moves as its saved moveset (by original slot).
void WotPartner_SaveMoves(u32 i, u32 slot, struct Pokemon *mon)
{
    u32 m;

    for (m = 0; m < MAX_MON_MOVES; m++)
        gSaveBlock1Ptr->wotPartners[i].moves[slot][m] = GetMonData(mon, MON_DATA_MOVE1 + m);
    gSaveBlock1Ptr->wotPartners[i].sanctum |= PARTNER_MOVES_CURRENT;
}

static void LevelPartnerMon(struct Pokemon *mon, s32 level)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);

    level = min(max(level, 1), MAX_LEVEL);
    SetMonData(mon, MON_DATA_EXP, &gExperienceTables[gSpeciesInfo[species].growthRate][level]);
}

// Friendship for Lida, Tulip and Jacinthe: the partner Pokemon holding a Mega
// Stone gets perfect IVs at Lv2, a steady level from Lv3 (exactly
// MEGA_DIM_PARTNER_LEVELS_BELOW under, no spread roll: WotPartner_BuildTeam),
// and 252 EVs in its two highest base stats at Lv4.
static void ApplyStonePerks(struct Pokemon *mon, u32 friendship)
{
    static const u8 sStatOrder[NUM_STATS] = {STAT_HP, STAT_ATK, STAT_DEF, STAT_SPEED, STAT_SPATK, STAT_SPDEF};
    const struct SpeciesInfo *info = &gSpeciesInfo[GetMonData(mon, MON_DATA_SPECIES)];
    u8 base[NUM_STATS];
    u32 s, first = 0, second = 1, iv = MAX_PER_STAT_IVS, ev = MAX_PER_STAT_EVS;

    if (friendship >= 2)
    {
        for (s = 0; s < NUM_STATS; s++)
            SetMonData(mon, MON_DATA_HP_IV + s, &iv);
    }
    if (friendship >= FRIENDSHIP_MAX_LEVEL)
    {
        base[STAT_HP] = info->baseHP;
        base[STAT_ATK] = info->baseAttack;
        base[STAT_DEF] = info->baseDefense;
        base[STAT_SPEED] = info->baseSpeed;
        base[STAT_SPATK] = info->baseSpAttack;
        base[STAT_SPDEF] = info->baseSpDefense;
        if (base[second] > base[first])
            SWAP(first, second, s);
        for (s = 2; s < NUM_STATS; s++)
        {
            if (base[s] > base[first])
            {
                second = first;
                first = s;
            }
            else if (base[s] > base[second])
            {
                second = s;
            }
        }
        SetMonData(mon, MON_DATA_HP_EV + sStatOrder[first], &ev);
        SetMonData(mon, MON_DATA_HP_EV + sStatOrder[second], &ev);
    }
}

static bool32 HoldsMegaStone(struct Pokemon *mon)
{
    return GetItemHoldEffect(GetMonData(mon, MON_DATA_HELD_ITEM)) == HOLD_EFFECT_MEGA_STONE;
}

// After FillPartnerParty has written partner i's three into gPlayerParty[3..5]
//: levels (in battle the player's best +/-3 each, rolled on its
// own; on the partner card the best itself), the movesets the player set,
// the friendship stone perks, then the order the player set.
void WotPartner_BuildTeam(u32 i, u32 best, bool32 forBattle)
{
    struct Pokemon team[WOT_PARTNER_TEAM_SIZE];
    const u8 *order = WotPartner_GetOrder(i);
    u32 friendship = WotPartner_GetLevel(i);
    u32 slot, m;

    for (slot = 0; slot < WOT_PARTNER_TEAM_SIZE; slot++)
    {
        struct Pokemon *mon = &gPlayerParty[MULTI_PARTY_SIZE + slot];
        const u16 *moves = gSaveBlock1Ptr->wotPartners[i].moves[slot];
        bool32 savedMoves = moves[0] != MOVE_NONE && (gSaveBlock1Ptr->wotPartners[i].sanctum & PARTNER_MOVES_CURRENT);
        s32 level = (s32)best - MEGA_DIM_PARTNER_LEVELS_BELOW;

        team[slot] = *mon;
        if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE)
            continue;
        if (forBattle && !(HoldsMegaStone(mon) && friendship >= 3))
            level += (s32)(Random() % (2 * MEGA_DIM_PARTNER_LEVEL_SPREAD + 1)) - MEGA_DIM_PARTNER_LEVEL_SPREAD;
        LevelPartnerMon(&team[slot], max(level, 1));
        if (savedMoves)
        {
            for (m = 0; m < MAX_MON_MOVES; m++)
                SetMonMoveSlot(&team[slot], moves[m], m);
        }
        if (HoldsMegaStone(mon))
            ApplyStonePerks(&team[slot], friendship);
        CalculateMonStats(&team[slot]);
    }
    for (slot = 0; slot < WOT_PARTNER_TEAM_SIZE; slot++)
        gPlayerParty[MULTI_PARTY_SIZE + slot] = team[order[slot]];
}

// The partner card, outside the dimension: what they'd match if they came.
u32 WotMegaDim_GetBestLevel(void)
{
    return GetBroughtLevel();
}

// follower_npc.c PrepareForFollowerNPCBattle, right after FillPartnerParty.
void WotMegaDim_AdjustPartnerParty(void)
{
    if (gPartnerTrainerId < TRAINER_PARTNER(PARTNER_MEGADIM_CANARI)
     || gPartnerTrainerId > TRAINER_PARTNER(PARTNER_MEGADIM_JACINTHE))
        return;
    sPartnerBattle = TRUE;
    WotPartner_BuildTeam(gPartnerTrainerId - TRAINER_PARTNER(PARTNER_MEGADIM_CANARI), GetHighestPartyLevel(), TRUE);
}

// In a Mega Dimension partner battle the player picks the partner's
// moves and switches too. Battler 2 keeps the partner controller (sprite,
// slides, "<partner> sent out" lines, their own half of the party), but its
// choosing commands run on the player's handlers
// (TryRunPlayerCommandForPartner), and the engine takes the two sides' choices
// one after the other as in an ordinary double battle.
bool32 WotMegaDim_PartnerPlayerControlled(void)
{
    return (gBattleTypeFlags & BATTLE_TYPE_MULTI)
        && (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER)
        && !(gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_RECORDED))
        && gPartnerTrainerId >= TRAINER_PARTNER(PARTNER_MEGADIM_CANARI)
        && gPartnerTrainerId <= TRAINER_PARTNER(PARTNER_MEGADIM_JACINTHE);
}

bool32 WotMegaDim_IsControlledPartner(u32 battler)
{
    return WotMegaDim_PartnerPlayerControlled() && GetBattlerPosition(battler) == B_POSITION_PLAYER_RIGHT;
}

// battle_setup.c, at the end of a Mega Dimension battle.
void WotMegaDim_EndPartnerBattle(void)
{
    sPartnerBattle = FALSE;
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

// The rogue Mega: the party's best, at least MEGA_DIM_BOSS_EARLY_MIN_LEVEL, and
// from Shin Tokyo on at least MEGA_DIM_BOSS_MIN_LEVEL.
static u8 GetBossLevel(void)
{
    u32 floor = IsSeen(FLAG_WOT_MEGA_DIM_SEEN_SHIN_TOKYO) ? MEGA_DIM_BOSS_MIN_LEVEL : MEGA_DIM_BOSS_EARLY_MIN_LEVEL;

    return max(GetHighestPartyLevel(), floor);
}

// A rival's Pokemon: `below` levels under the party's best, at least 1.
static u8 GetRivalLevel(u32 below)
{
    s32 level = (s32)GetHighestPartyLevel() - (s32)below;

    return level < 1 ? 1 : level;
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

// Placeholders this close (in tiles, on both axes) are one bunch, chained;
// no bunch shows within MEGA_DIM_MEGA_CLEARANCE of the Mega's spot.
#define MEGA_DIM_BUNCH_REACH     2
#define MEGA_DIM_MEGA_CLEARANCE  4

static bool32 IsNear(s32 ax, s32 ay, s32 bx, s32 by, s32 reach)
{
    return abs(ax - bx) <= reach && abs(ay - by) <= reach;
}

// Deals the visible wild Pokemon. The placeholders are split into
// bunches by where they stand, not their order in the map, and each dealt
// bunch is one species -- so a species never mixes into another's bunch, as
// map order let it in Shin Tokyo. Bunches by the Mega's spot sit out, so the
// Mega stands apart. slotOf[n] = the mask index of mons[n] (the i-th dealt
// bunch's j-th placeholder in map order is i * MEGA_DIM_MON_PER_AREA + j),
// or -1 if it isn't dealt. Only the shuffle draws on the deal generator, so
// SetupWildMons and WotMegaDim_MarkWildGone agree.
static u32 DealBunches(struct MegaDimInstance *inst, struct ObjectEventTemplate **mons, u32 count, s8 *slotOf)
{
    const struct MegaDimSlot *mega = &inst->dim->slots[inst->megaSlot];
    u8 bunchOf[MEGA_DIM_MON_MAX], order[MEGA_DIM_MON_MAX], stack[MEGA_DIM_MON_MAX];
    u32 n, m, b, i, j, sp, bunches = 0, eligible = 0, dealt;

    for (n = 0; n < count; n++)
    {
        bunchOf[n] = 0xFF;
        slotOf[n] = -1;
    }
    for (n = 0; n < count; n++)
    {
        if (bunchOf[n] != 0xFF)
            continue;
        bunchOf[n] = bunches;
        sp = 0;
        stack[sp++] = n;
        while (sp != 0)
        {
            u32 a = stack[--sp];

            for (m = 0; m < count; m++)
            {
                if (bunchOf[m] == 0xFF && IsNear(mons[a]->x, mons[a]->y, mons[m]->x, mons[m]->y, MEGA_DIM_BUNCH_REACH))
                {
                    bunchOf[m] = bunches;
                    stack[sp++] = m;
                }
            }
        }
        bunches++;
    }
    for (b = 0; b < bunches; b++)
    {
        bool32 byMega = FALSE;

        for (n = 0; n < count; n++)
        {
            if (bunchOf[n] == b && IsNear(mons[n]->x, mons[n]->y, mega->x, mega->y, MEGA_DIM_MEGA_CLEARANCE))
                byMega = TRUE;
        }
        if (!byMega)
            order[eligible++] = b;
    }
    for (i = eligible; i > 1; i--)
    {
        u32 swap = NextDeal(&inst->rng) % i, tmp = order[i - 1];
        order[i - 1] = order[swap];
        order[swap] = tmp;
    }
    dealt = min(eligible, MEGA_DIM_VISIT_SPECIES);
    for (i = 0; i < dealt; i++)
    {
        for (n = 0, j = 0; n < count && j < MEGA_DIM_MON_PER_AREA; n++)
        {
            if (bunchOf[n] == order[i])
                slotOf[n] = i * MEGA_DIM_MON_PER_AREA + j++;
        }
    }
    return dealt;
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
        // Canari along: each roll's odds are x(4 + level)/4, so x1.25 at
        // friendship Lv1 up to x2 at Lv4.
        if (Random() < SHINY_ODDS * (4 + GetPerkLevel(PARTNER_MEGADIM_CANARI)) / 4)
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
// seed) each get one species from sDimWildSpecies (picked by the seed,
// through the Randomizer if it's on); the other groups hide, and so does any
// Pokemon already beaten or caught from this portal. A shiny found earlier
// through this portal is still shiny; every other one rolls the usual odds
// again. Positions are left as placed in porymap. Species and shininess live
// in the template's graphics id (species + OBJ_EVENT_MON
// [+ OBJ_EVENT_MON_SHINY]), which is what the sprite draws and what the
// battle reads back.
// Taunie's shiny reads (friendship Lv3: the wild Pokemon, Lv4: the Mega) are
// settled when she reads the portal, and the next trip into it honours them:
// a "yes" makes one appear, a "no" holds the rolls off for that trip. Kept in
// EWRAM, so a reload just rolls as usual. Reading the same portal again
// repeats the answer instead of re-rolling.
enum
{
    SCOUT_NONE,
    SCOUT_NO,
    SCOUT_YES,
};
static EWRAM_DATA u8 sScoutRecord = 0;
// Taunie picked sScoutRecord's Mega: the first trip in keeps what she said.
static EWRAM_DATA bool8 sScoutPicked = FALSE;
static EWRAM_DATA u8 sScoutWild = SCOUT_NONE;
static EWRAM_DATA u8 sScoutMega = SCOUT_NONE;

static u32 ScoutRoll(u32 rolls)
{
    u32 i;

    for (i = 0; i < rolls; i++)
    {
        if (RollShiny())
            return SCOUT_YES;
    }
    return SCOUT_NO;
}

// The scouted answer for r, used up; SCOUT_NONE if there isn't one.
static u32 TakeScout(u32 r, u8 *scout)
{
    u32 answer = (sScoutRecord == r) ? *scout : SCOUT_NONE;

    *scout = SCOUT_NONE;
    return answer;
}

// Returns whether any wild Pokemon it put out is shiny.
static bool32 SetupWildMons(u32 r, struct MegaDimInstance *inst)
{
    struct ObjectEventTemplate *mons[MEGA_DIM_MON_MAX];
    u16 species[MEGA_DIM_VISIT_SPECIES];
    u16 shinySheets[MEGA_DIM_MAX_SHINY_SHEETS];
    s8 slotOf[MEGA_DIM_MON_MAX];
    u32 i, j, k, n, count, dealt, shinyCount = 0;
    u32 scout = TakeScout(r, &sScoutWild);
    u32 cellA = MEGA_DIM_MON_MAX, cellB = MEGA_DIM_MON_MAX;
    bool32 anyShiny = FALSE;

    count = CollectWildMons(mons);
    for (i = 0; i < count; i++)
        FlagSet(mons[i]->flagId);   // hidden unless dealt below
    if (count == 0 || GetRecordLegend(r) != NULL)
        return FALSE;               // a legend stands alone

    dealt = DealBunches(inst, mons, count, slotOf);
    for (i = 0; i < dealt; i++)
    {
        species[i] = sDimWildSpecies[NextDeal(&inst->rng) % ARRAY_COUNT(sDimWildSpecies)];
        if (WotRandomizerOption(FLAG_WOT_RAND_WILD))
            species[i] = WotRandomizeSpecies(species[i]);
    }

    // With the Odd Pebble, one or two of the dealt slots are Zygarde
    // Cells instead. A taken Cell is a gone slot, so re-entering can't farm it.
    if (dealt != 0 && CheckBagHasItem(ITEM_ODD_PEBBLE, 1))
    {
        cellA = NextDeal(&inst->rng) % (dealt * MEGA_DIM_MON_PER_AREA);
        if (dealt > 1 && (NextDeal(&inst->rng) & 1))
            cellB = (cellA + MEGA_DIM_MON_PER_AREA) % (dealt * MEGA_DIM_MON_PER_AREA);
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

    for (n = 0; n < count; n++)
    {
        {
            struct ObjectEventTemplate *mon = mons[n];
            bool32 shiny;

            if (slotOf[n] < 0)
                continue;
            k = slotOf[n];
            i = k / MEGA_DIM_MON_PER_AREA;
            if (MaskGet(r, MASK_GONE(k)))
                continue;
            shiny = MaskGet(r, MASK_SHINY(k));
            if (!shiny && (k == cellA || k == cellB))
            {
                mon->graphicsId = OBJ_EVENT_GFX_WOT_ZYGARDE_CELL;
                mon->movementType = MOVEMENT_TYPE_NONE;
                FlagClear(mon->flagId);
                continue;
            }
            if (!shiny && ((scout == SCOUT_NONE ? (!sNoShinyReroll && RollShiny()) : scout == SCOUT_YES) || sForceShiny)
             && TakeShinySheet(shinySheets, &shinyCount, species[i]))
            {
                shiny = TRUE;
                sForceShiny = FALSE;
                if (scout == SCOUT_YES)
                    scout = SCOUT_NO;   // Taunie read one shiny
                MaskSet(r, MASK_SHINY(k));
            }
            mon->graphicsId = species[i] + OBJ_EVENT_MON + (shiny ? OBJ_EVENT_MON_SHINY : 0);
            FlagClear(mon->flagId);
            anyShiny |= shiny;
        }
    }
    return anyShiny;
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
    timeLeft = GetSecondsLeft(r);
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

// Settles what a never-entered portal holds, before anyone looks inside
// (Taunie's scouting, or the first trip in). Both writes stick to the record.
// reroll: Taunie looks again, so the Mega inside is drawn again. Returns
// whether the portal's Mega could still change (never entered, no legend).
static bool32 PrepareFirstEntry(u32 r, bool32 reroll)
{
    u32 record = GetRecord(r);

    if ((record & PORTAL_ENTERED) || GetRecordLegend(r) != NULL)
        return FALSE;
    if (reroll)
        record = (record & ~(PORTAL_MEGA_MASK << PORTAL_MEGA_SHIFT)) | (PickRogueMega(r) << PORTAL_MEGA_SHIFT);
    // Playtest: a Mega that has been claimed since this portal
    // opened must not turn up again. Once all 23 are claimed every portal is
    // a rematch, drawn afresh on its first trip in, so the
    // portals that all held the last one don't keep repeating it.
    // What Taunie last named stays put.
    else if (IsMegaClaimed(GetRecordMega(r)) && !(sScoutPicked && sScoutRecord == r))
        record = (record & ~(PORTAL_MEGA_MASK << PORTAL_MEGA_SHIFT)) | (PickRogueMega(r) << PORTAL_MEGA_SHIFT);
    // The guaranteed rival: the first new portal after the first collapse.
    // A portal that already has one counts.
    if (GetRivalIntro() == RIVAL_INTRO_DUE)
    {
        if ((record & PORTAL_RIVAL_MASK) == 0)
            record |= PickRival();
        SetRivalIntro(RIVAL_INTRO_DONE);
    }
    SetRecord(r, record);
    return TRUE;
}

// Taunie along: she reads the portal before the player steps in,
// more with each friendship level.
//   VAR_RESULT = 0 -- Taunie isn't the partner (or no portal here)
//                1 -- a rogue Mega: STR_VAR_2 = its species
//                2 -- (Lv2+) a rival got there first: STR_VAR_1 = who,
//                     STR_VAR_2 = the Mega
//                3 -- one of Mable's legends: STR_VAR_2 = its species
//   VAR_0x8005 = (Lv3+) a shiny wild Pokemon inside: 1 yes, 2 no, 0 not read
//   VAR_0x8006 = (Lv4)  the Mega or legend is shiny: 1 yes, 2 no, 0 not read
void WotMegaDim_TaunieScout(void)
{
    u32 r = FindRecordAtSpot(GetTalkedSpot());
    u32 level = GetPerkLevel(PARTNER_MEGADIM_TAUNIE);
    const struct MegaDimLegend *legend;
    u32 rival, k;
    bool32 shinyThere = FALSE;

    gSpecialVar_Result = 0;
    gSpecialVar_0x8005 = 0;
    gSpecialVar_0x8006 = 0;
    if (r == RECORD_NONE || level == 0)
        return;
    if (sScoutRecord != r)
    {
        sScoutRecord = r;
        sScoutWild = SCOUT_NONE;
        sScoutMega = SCOUT_NONE;
    }
    // Every look at a portal nobody has entered draws its Mega again, and
    // the trip in finds the one she named. A new Mega gets a new shine read.
    sScoutPicked = PrepareFirstEntry(r, TRUE);
    if (sScoutPicked)
        sScoutMega = SCOUT_NONE;
    legend = GetRecordLegend(r);

    if (level >= 3 && legend == NULL)
    {
        for (k = 0; k < MEGA_DIM_DEALT_MAX; k++)
        {
            if (!MaskGet(r, MASK_GONE(k)) && MaskGet(r, MASK_SHINY(k)))
                shinyThere = TRUE;
        }
        // One already showed: it's still there, so nothing to settle.
        if (!shinyThere && sScoutWild == SCOUT_NONE)
            sScoutWild = ScoutRoll(MEGA_DIM_DEALT_MAX);
        gSpecialVar_0x8005 = (shinyThere || sScoutWild == SCOUT_YES) ? 1 : 2;
    }
    if (level >= FRIENDSHIP_MAX_LEVEL)
    {
        if (!MaskGet(r, MASK_MEGA_SHINY) && sScoutMega == SCOUT_NONE)
            sScoutMega = ScoutRoll(1);
        gSpecialVar_0x8006 = (MaskGet(r, MASK_MEGA_SHINY) || sScoutMega == SCOUT_YES) ? 1 : 2;
    }

    if (legend != NULL)
    {
        StringCopy(gStringVar2, GetSpeciesName(legend->species));
        gSpecialVar_Result = 3;
        return;
    }
    StringCopy(gStringVar2, GetSpeciesName(sMegaRoster[GetRecordMega(r)].baseSpecies));
    rival = GetRecordRival(r);
    if (rival != 0 && level >= 2)
    {
        StringCopy(gStringVar1, GetTrainerNameFromId(sRivals[rival - 1].trainerId));
        gSpecialVar_Result = 2;
        return;
    }
    gSpecialVar_Result = 1;
}

// The far side of the portal: the map fades in still distorted,
// with the player standing in the exit portal; once the fade is done
// MegaDim_EventScript_PortalArrival steps them out while the distortion
// settles, and only then does the partner join (WotMegaDim_PortalArrivalDone).
extern const u8 MegaDim_EventScript_PortalArrival[];

static void Task_PortalArrival(u8 taskId)
{
    if (gPaletteFade.active)
        return;
    ScriptContext_SetupScript(MegaDim_EventScript_PortalArrival);
    DestroyTask(taskId);
}

static void FieldCB_PortalArrival(void)
{
    Overworld_PlaySpecialMapMusic();
    StartRiftDistortionFull();
    FadeInFromWhite();
    LockPlayerFieldControls();
    CreateTask(Task_PortalArrival, 10);
}

// The step out is done: the partner (if any) appears behind the player on
// their next step, like any follower after a warp.
void WotMegaDim_PortalArrivalDone(void)
{
    if (FlagGet(FLAG_WOT_MEGA_DIM_PARTNER_ON))
        StartPartnerFollower();
}

// --- Back in the real world -----------------------------------------
// The exit portal and a collapse land the player on the portal's own tile,
// hidden; MegaDim_EventScript_PortalReturn grows the portal open, brings the
// player in through a distorted white flash and steps them out, and a
// collapsed portal then shrinks away. A whiteout keeps landing in front of it.
extern const u8 MegaDim_EventScript_PortalReturn[];

static EWRAM_DATA u8 sReturnSpot = 0;          // the portal spot coming back to
static EWRAM_DATA bool8 sReturnClosing = FALSE; // its portal collapsed

static u8 GetReturnPortalObjectId(void)
{
    u8 localId = FindLocalIdByFlag(sPortalSpots[sReturnSpot].hideFlag);

    if (localId == LOCALID_NONE)
        return OBJECT_EVENTS_COUNT;
    return GetObjectEventIdByLocalIdAndMap(localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
}

static void Task_PortalReturn(u8 taskId)
{
    if (gPaletteFade.active)
        return;
    ScriptContext_SetupScript(MegaDim_EventScript_PortalReturn);
    DestroyTask(taskId);
}

static void FieldCB_PortalReturn(void)
{
    const struct MegaDimSpot *spot = &sPortalSpots[sReturnSpot];
    u8 localId = FindLocalIdByFlag(spot->hideFlag);
    u8 objId;

    Overworld_PlaySpecialMapMusic();
    // A collapsed portal's record is already gone, so the map loaded without
    // it: put it back for now, to watch it close.
    if (sReturnClosing && localId != LOCALID_NONE)
    {
        FlagClear(spot->hideFlag);
        TrySpawnObjectEvent(localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    }
    // Neither the portal nor the player is there yet.
    objId = GetReturnPortalObjectId();
    if (objId < OBJECT_EVENTS_COUNT)
        gObjectEvents[objId].invisible = TRUE;
    gObjectEvents[gPlayerAvatar.objectEventId].invisible = TRUE;
    FadeInFromWhite();
    LockPlayerFieldControls();
    CreateTask(Task_PortalReturn, 10);
}

// Out through a portal, landing on it (the exit portal, or a collapse).
static void ReturnThroughPortal(u32 spot, bool32 closing)
{
    sReturnSpot = spot;
    sReturnClosing = closing;
    DoWhiteFadeWarp();
    gFieldCallback = FieldCB_PortalReturn;
    ResetInitialPlayerAvatarState();
}

// VAR_0x8004 = the portal's local id, VAR_0x8005 = TRUE if it's closing.
void WotMegaDim_GetReturnPortal(void)
{
    gSpecialVar_0x8004 = FindLocalIdByFlag(sPortalSpots[sReturnSpot].hideFlag);
    gSpecialVar_0x8005 = sReturnClosing;
}

void WotMegaDim_ShowReturningPlayer(void)
{
    gObjectEvents[gPlayerAvatar.objectEventId].invisible = FALSE;
}

// The portal growing open or shrinking shut: an affine scale (with a turn)
// over PORTAL_SCALE_FRAMES. Specials with waitstate.
#define PORTAL_SCALE_FRAMES 24
#define PORTAL_SCALE_MIN    8    // of 0x100: never a zero scale

#define tFrame  data[0]
#define tShrink data[1]
#define tMatrix data[2]

static void SetPortalScale(u8 matrixNum, u32 progress)
{
    u32 scale = max(0x100 * progress / PORTAL_SCALE_FRAMES, PORTAL_SCALE_MIN);

    SetOamMatrixRotationScaling(matrixNum, scale, scale, (PORTAL_SCALE_FRAMES - progress) * 0x10000 / PORTAL_SCALE_FRAMES);
}

static void Task_PortalScale(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u8 objId = GetReturnPortalObjectId();
    bool32 done = ++tFrame >= PORTAL_SCALE_FRAMES;

    if (objId < OBJECT_EVENTS_COUNT && !done)
    {
        SetPortalScale(tMatrix, tShrink ? PORTAL_SCALE_FRAMES - tFrame : tFrame);
        return;
    }
    if (objId < OBJECT_EVENTS_COUNT)
    {
        struct Sprite *sprite = &gSprites[gObjectEvents[objId].spriteId];

        sprite->oam.affineMode = ST_OAM_AFFINE_OFF;
        sprite->oam.matrixNum = 0;
        sprite->subspriteMode = SUBSPRITES_IGNORE_PRIORITY;
        if (tShrink)
            gObjectEvents[objId].invisible = TRUE;
    }
    FreeOamMatrix(tMatrix);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void StartPortalScale(bool32 shrink)
{
    u8 objId = GetReturnPortalObjectId();
    u8 matrixNum = AllocOamMatrix();
    struct Sprite *sprite;
    u8 taskId;

    if (objId >= OBJECT_EVENTS_COUNT || matrixNum == 0xFF)
    {
        // Nothing to animate: just show or hide it.
        if (objId < OBJECT_EVENTS_COUNT)
            gObjectEvents[objId].invisible = shrink;
        if (matrixNum != 0xFF)
            FreeOamMatrix(matrixNum);
        ScriptContext_Enable();
        return;
    }
    sprite = &gSprites[gObjectEvents[objId].spriteId];
    SetPortalScale(matrixNum, shrink ? PORTAL_SCALE_FRAMES : 0);
    sprite->subspriteMode = SUBSPRITES_OFF;
    sprite->oam.affineMode = ST_OAM_AFFINE_NORMAL;
    sprite->oam.matrixNum = matrixNum;
    gObjectEvents[objId].invisible = FALSE;
    taskId = CreateTask(Task_PortalScale, 80);
    gTasks[taskId].tFrame = 0;
    gTasks[taskId].tShrink = shrink;
    gTasks[taskId].tMatrix = matrixNum;
}

void WotMegaDim_PortalGrow(void)
{
    StartPortalScale(FALSE);
}

void WotMegaDim_PortalShrink(void)
{
    StartPortalScale(TRUE);
}

#undef tFrame
#undef tShrink
#undef tMatrix

// Through the portal: a trip back in costs MEGA_DIM_REENTRY_PENALTY seconds;
// then warp in onto the exit portal's slot, to step out of it (it
// used to land one tile south). Script contexts end at a warp, so nothing
// follows this.
void WotMegaDim_EnterPortal(void)
{
    struct MegaDimInstance inst;
    const struct MegaDimSlot *arrive;
    u32 r = FindRecordAtSpot(GetTalkedSpot());
    u32 record;
    bool32 reentry;

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

    PrepareFirstEntry(r, FALSE);
    record = GetRecord(r);
    reentry = (record & PORTAL_ENTERED) != 0;
    record |= PORTAL_ENTERED;
    if (sForceRival != 0 && GetRecordLegend(r) == NULL)
    {
        record = (record & ~PORTAL_RIVAL_MASK) | sForceRival;
        sForceRival = 0;
    }
    SetRecord(r, record);
    SetSession(r);
    sClockAcc = 0;
    if (reentry)
    {
        u32 timeLeft = GetSecondsLeft(r);

        SetSecondsLeft(r, timeLeft > MEGA_DIM_REENTRY_PENALTY ? timeLeft - MEGA_DIM_REENTRY_PENALTY : 1);
    }
    // The partner is along from here (runs count, perks apply), but only
    // shows up once the player has stepped out of the portal.
    if (GetPartner() != 0)
        FlagSet(FLAG_WOT_MEGA_DIM_PARTNER_ON);
    sLastPlaySecond = gSaveBlock2Ptr->playTimeSeconds;

    DeriveInstance(r, &inst);
    arrive = &inst.dim->slots[inst.portalSlot];
    SetWarpDestination(MAP_GROUP(inst.dim->map), MAP_NUM(inst.dim->map), WARP_ID_NONE, arrive->x, arrive->y);
    sForceOnFoot = TRUE;
    DoWhiteFadeWarp();
    gFieldCallback = FieldCB_PortalArrival;
    ResetInitialPlayerAvatarState();
}

// The exit portal inside: back to the portal, which stays open (its clock
// stops until the player goes back in).
void WotMegaDim_LeaveOut(void)
{
    u32 r = GetSession();

    StopPartnerFollower(TRUE);
    FlagClear(FLAG_WOT_SMART_WILD_AI);
    SetReturnWarp(r, TRUE);
    SetSession(RECORD_NONE);
    ReturnThroughPortal(GetRecordSpot(r), FALSE);
}

// The collapse: the clock ran out, or the rogue Mega was caught or KO'd (or
// its rival beaten). Back to the portal, which closes behind the player.
void WotMegaDim_CollapseOut(void)
{
    u32 spot = GetRecordSpot(GetSession());

    CollapseSession(FALSE);
    ReturnThroughPortal(spot, TRUE);
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

#if TESTING
// test/wot_rift_legends.c: an AI test battle's opponent counts as a rift boss.
static EWRAM_DATA bool8 sTestRiftBoss = FALSE;
#endif

// battle_ai_main.c: this is a rift boss battle -- the only wild battle in
// the dimension that sets the smart-wild-AI flag (WotMegaDim_SetupMegaBattle).
bool32 WotMegaDim_IsRiftBossBattle(void)
{
#if TESTING
    if (sTestRiftBoss)
        return TRUE;
#endif
    return WotMegaDim_IsActive()
        && !(gBattleTypeFlags & BATTLE_TYPE_TRAINER)
        && FlagGet(FLAG_WOT_SMART_WILD_AI);
}

static bool32 IsRiftBoss(u32 battler)
{
    return WotMegaDim_IsRiftBossBattle() && GetBattlerSide(battler) == B_SIDE_OPPONENT;
}

// A sanctum's stone-holding legend fights as a rogue does, whatever the
// portal it came through holds.
static EWRAM_DATA bool8 sSanctumRogue = FALSE;
// The Z-Move move of a boss with no portal row: a sanctum legend, Mega Floette,
// Zygarde. Every boss setup sets or clears it.
static EWRAM_DATA u16 sBossZOverride = MOVE_NONE;

// The rogue Mega proper, not one of Mable's legends: its last stand and its
// catch-rate cap.
static bool32 IsRogueBoss(u32 battler)
{
    return IsRiftBoss(battler) && (sSanctumRogue || GetRecordLegend(GetSession()) == NULL);
}

// The move this battle's boss throws as its Z-Move, or MOVE_NONE if it has none.
static u16 GetBossZMove(void)
{
    const struct MegaDimLegend *legend;

    if (sBossZOverride != MOVE_NONE)
        return sBossZOverride;
    legend = GetRecordLegend(GetSession());
    if (legend != NULL)
        return legend->zMove;
    return sMegaRoster[GetRecordMega(GetSession())].zMove;
}

// battle_z_move.c: a rift boss -- a rogue Mega, a sanctum's stone-holding
// legend or one of Mable's Mega legends, never a rival's ace -- gets one
// Z-Move a battle on top of its Mega Evolution, with no Z-Crystal.
bool32 WotMegaDim_IsRogueZMoveUser(u32 battler)
{
    return IsRiftBoss(battler) && GetBossZMove() != MOVE_NONE;
}

// battle_z_move.c: that Z-Move comes from the one move its set names
// (the type-based Z-Move of that move).
bool32 WotMegaDim_IsBossZMove(u16 move)
{
    return move != MOVE_NONE && move == GetBossZMove() && !IsBattleMoveStatus(move);
}

// battle_ai_main.c: the move this battler throws as its Z-Move, if it's a
// rift boss with one.
u16 WotMegaDim_GetBossZMoveFor(u32 battler)
{
    return WotMegaDim_IsRogueZMoveUser(battler) ? GetBossZMove() : MOVE_NONE;
}

#if TESTING
void WotMegaDim_Test_SetRiftBoss(u16 zMove)
{
    sTestRiftBoss = (zMove != MOVE_NONE);
    sBossZOverride = zMove;
    sSanctumRogue = FALSE;
}
#endif

// The rogue Mega's last stand, once a battle: the first attack that
// would KO it leaves it at 1 HP (battle_util.c GetAdjustedDamage); then, when
// the hit's messages come up (Cmd_resultmessage), the rift pours back into it:
// half its health, and a glow on its sprite for the rest of the fight.
#define BOSS_LAST_STAND_HEAL_PERCENT 50
#define BOSS_LAST_STAND_GLOW_COEFF   5
#define BOSS_LAST_STAND_GLOW_COLOR   RGB(31, 6, 22)

enum
{
    LAST_STAND_READY,
    LAST_STAND_PENDING,
    LAST_STAND_USED,
};

static EWRAM_DATA u8 sBossLastStand = LAST_STAND_READY;

bool32 WotMegaDim_TryBossLastStand(u32 battler)
{
    if (sBossLastStand != LAST_STAND_READY || !IsRogueBoss(battler))
        return FALSE;
    sBossLastStand = LAST_STAND_PENDING;
    return TRUE;
}

bool32 WotMegaDim_TakeBossLastStand(u32 battler)
{
    u32 palette;

    if (sBossLastStand != LAST_STAND_PENDING || !IsRogueBoss(battler) || gBattleMons[battler].hp == 0)
        return FALSE;
    sBossLastStand = LAST_STAND_USED;
    SetHealAmount(battler, GetNonDynamaxMaxHP(battler) * BOSS_LAST_STAND_HEAL_PERCENT / 100);
    // Into both buffers, so the move animations' fades keep it.
    palette = OBJ_PLTT_ID(gSprites[gBattlerSpriteIds[battler]].oam.paletteNum);
    BlendPalette(palette, 16, BOSS_LAST_STAND_GLOW_COEFF, BOSS_LAST_STAND_GLOW_COLOR);
    CpuCopy16(&gPlttBufferFaded[palette], &gPlttBufferUnfaded[palette], PLTT_SIZE_4BPP);
    return TRUE;
}

// battle_script_commands.c ComputeCaptureOdds: a rogue Mega is a boss, not a
// wild Pokemon, whatever its species' catch rate (Amoonguss's 75 fell to a
// Poke Ball first try). Cap it: a weakened, sleeping one in an Ultra Ball is
// still about 2 in 5, a fresh one in a Poke Ball about 1 in 20.
#define MEGA_DIM_BOSS_CATCH_RATE 15

s32 WotMegaDim_CapCatchRate(s32 catchRate, u32 battler)
{
    // A sanctum legend keeps the Shadow catch floor: only a catch counts there.
    if (!sSanctumRogue && IsRogueBoss(battler) && catchRate > MEGA_DIM_BOSS_CATCH_RATE)
        return MEGA_DIM_BOSS_CATCH_RATE;
    return catchRate;
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
static void HideBarrierBeforePostgame(void);

void WotMegaDim_SetMegaGfx(void)
{
    struct MegaDimInstance inst;
    u32 r = GetSession();
    u32 index = GetRecordMega(r);
    bool32 shiny = MaskGet(r, MASK_MEGA_SHINY);
    u32 scout = TakeScout(r, &sScoutMega);

    // Shiny odds are rolled again on every trip in; a shiny one stays shiny.
    // Debug "shiny" forces it too (Legend portals have no wild
    // Pokemon for it to land on otherwise).
    if (!shiny && ((scout == SCOUT_NONE ? (!sNoShinyReroll && RollShiny()) : scout == SCOUT_YES) || sForceShiny))
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
        // A shiny in sight (the Mega, unless a rival has it, or a
        // wild one) chimes once the map has faded in -- every trip in.
        sShinyCue = (shiny && rival == 0) | SetupWildMons(r, &inst);
        HideBarrierBeforePostgame();
        sForceShiny = FALSE;
    }
    sNoShinyReroll = FALSE;
}

// The base species holding its stone, at the party's best level (45 at least),
// with perfect IVs, shiny if the overworld one is. The smart-wild-AI flag
// (B_SMART_WILD_AI_FLAG) makes it pick its gimmick, so it Mega Evolves on turn
// one; WotMegaDim_EndMegaBattle clears it again. A legend is built the same
// way: Groudon/Kyogre hold their orb (Primal Reversion on switch-in needs no
// flag) and keep their level-up moves; the Mega legends get their set
// (Rayquaza's starts with Dragon Ascent, which it needs to Mega Evolve);
// Lugia and Ho-Oh hold their stone.
void WotMegaDim_SetupMegaBattle(void)
{
    sBossLastStand = LAST_STAND_READY;
    sSanctumRogue = FALSE;
    sBossZOverride = MOVE_NONE;
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
        if (legend->moves[0] != MOVE_NONE)
        {
            for (i = 0; i < MAX_MON_MOVES; i++)
                SetMonMoveSlot(&gEnemyParty[0], legend->moves[i], i);
        }
        else if (legend->move != MOVE_NONE)
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
        for (i = 0; i < MAX_MON_MOVES; i++)
            SetMonMoveSlot(&gEnemyParty[0], sMegaRoster[index].moves[i], i);
    }
    for (i = 0; i < NUM_STATS; i++)
        SetMonData(&gEnemyParty[0], MON_DATA_HP_IV + i, &iv);
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY, &shiny);
    CalculateMonStats(&gEnemyParty[0]);
    FlagSet(FLAG_WOT_SMART_WILD_AI);
    sRogueThrewZ = FALSE;
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
    // The sanctum barriers glow in colour too.
    return tag >= OBJ_EVENT_PAL_TAG_WOT_BARRIER_SURGE && tag <= OBJ_EVENT_PAL_TAG_WOT_BARRIER_DASH;
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
// its base species holding its stone, 10 levels under the party's best
// with perfect IVs, shiny if this portal's Mega came up shiny. Their own two
// sit 11-12 under the party's best, so the Mega is always their highest. The AI Mega
// Evolves it (trainers need no Mega Ring).
void WotMegaDim_AdjustRivalParty(struct Pokemon *party, u16 trainerId)
{
    u32 i, r, index, aceLevel, iv = MAX_PER_STAT_IVS;
    bool32 shiny;
    struct OriginalTrainerId otId = OTID_STRUCT_RANDOM_NO_SHINY;
    struct Pokemon *ace = &party[MEGA_DIM_RIVAL_ACE_SLOT];

    // Trainer battles only: CreateNPCTrainerParty runs for wild battles too,
    // with a stale opponent id, so after a rival fight a wild or boss battle
    // here used to have its Pokemon re-levelled and an ace planted in slot 2
    // (likely the "wilds at max level" report).
    if (!WotMegaDim_IsActive() || !(gBattleTypeFlags & BATTLE_TYPE_TRAINER) || !WotMegaDim_IsRivalTrainer(trainerId))
        return;

    aceLevel = GetRivalLevel(MEGA_DIM_RIVAL_ACE_BELOW);
    for (i = 0; i < MEGA_DIM_RIVAL_ACE_SLOT; i++)
    {
        if (GetMonData(&party[i], MON_DATA_SPECIES) == SPECIES_NONE)
            continue;
        SetMonLevel(&party[i], GetRivalLevel(MEGA_DIM_RIVAL_TEAM_BELOW_MIN
                                             + Random() % (MEGA_DIM_RIVAL_TEAM_BELOW_MAX - MEGA_DIM_RIVAL_TEAM_BELOW_MIN + 1)));
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
    WotNuzlocke_NextScriptedWildCounts(); // a visible wild is an ordinary encounter
    BattleSetup_StartScriptedWildBattle();
}

// The talked-to wild Pokemon was beaten or caught: it stays gone for as long
// as this portal is open (and, if it was shiny, it no longer holds a sheet).
// Works out its place in the deal the same way SetupWildMons did.
static void MarkTalkedSlotGone(bool32 isCell)
{
    struct ObjectEventTemplate *mons[MEGA_DIM_MON_MAX];
    struct MegaDimInstance inst;
    s8 slotOf[MEGA_DIM_MON_MAX];
    u32 index, count, k, r = GetSession();

    if (r == RECORD_NONE)
        return;
    count = CollectWildMons(mons);
    for (index = 0; index < count && mons[index]->localId != gSpecialVar_LastTalked; index++)
        ;
    if (index == count)
        return;

    DeriveInstance(r, &inst);
    DealBunches(&inst, mons, count, slotOf);
    if (slotOf[index] < 0)
        return;
    k = slotOf[index];
    if (isCell)
    {
        sShardFlashAmount = ZYGARDE_CELL_SECONDS;
    }
    else
    {
        sShardFlashAmount = MaskGet(r, MASK_SHINY(k)) ? ZSHARDS_WILD_SHINY : ZSHARDS_WILD;
        GiveZShards(sShardFlashAmount);
    }
    sShardFlashFrames = ZSHARD_FLASH_FRAMES;
    MaskSet(r, MASK_GONE(k));
    MaskClear(r, MASK_SHINY(k));
}

void WotMegaDim_MarkWildGone(void)
{
    MarkTalkedSlotGone(FALSE);
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
    sSanctumRogue = FALSE;
}

// The boss was claimed: a rogue Mega's stone won or the Mega caught -- or one
// of Mable's legends caught, which moves her quest chain on.
static void ClaimLegend(const struct MegaDimLegend *legend)
{
    FlagSet(legend->caughtFlag);
    if (GetMableStage() == MABLE_STAGE_PRIMALS && AreStageLegendsCaught(MABLE_STAGE_PRIMALS))
        SetMableStage(MABLE_STAGE_RAYQUAZA_CALL);
    else if (GetMableStage() == MABLE_STAGE_RAYQUAZA && AreStageLegendsCaught(MABLE_STAGE_RAYQUAZA))
        SetMableStage(MABLE_STAGE_DONE);
}

void WotMegaDim_MarkClaimed(void)
{
    const struct MegaDimLegend *legend = GetRecordLegend(GetSession());

    if (legend == NULL)
        WotMegaDim_SetMegaClaimed(GetRecordMega(GetSession()), TRUE);
    else
        ClaimLegend(legend);
}

#if TESTING
// test/wot_rift_legends.c
u32 WotMegaDim_Test_LegendRow(u16 species)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sLegends) && sLegends[i].species != species; i++)
        ;
    return i;
}

u32 WotMegaDim_Test_LegendCount(void)
{
    return ARRAY_COUNT(sLegends);
}

void WotMegaDim_Test_SetMableStage(u32 stage)
{
    SetMableStage(stage);
}

u32 WotMegaDim_Test_GetMableStage(void)
{
    return GetMableStage();
}

void WotMegaDim_Test_CatchLegend(u32 row)
{
    ClaimLegend(&sLegends[row]);
}

bool32 WotMegaDim_Test_IsLegendWanted(u32 row)
{
    return IsLegendWanted(row);
}

u32 WotMegaDim_Test_LegendMap(u32 row)
{
    return sDimMaps[sLegends[row].dimMap].map;
}

// Every rift boss with a set has four attacks and throws its Z-Move from one
// of them: the first that doesn't (a roster row, or 100 + a legend row), or -1.
s32 WotMegaDim_Test_FindBadBossSet(void)
{
    u32 i, k;

    for (i = 0; i < ARRAY_COUNT(sMegaRoster) + ARRAY_COUNT(sLegends); i++)
    {
        const u16 *moves = i < ARRAY_COUNT(sMegaRoster) ? sMegaRoster[i].moves : sLegends[i - ARRAY_COUNT(sMegaRoster)].moves;
        u16 zMove = i < ARRAY_COUNT(sMegaRoster) ? sMegaRoster[i].zMove : sLegends[i - ARRAY_COUNT(sMegaRoster)].zMove;
        bool32 found = FALSE;

        if (i >= ARRAY_COUNT(sMegaRoster) && moves[0] == MOVE_NONE)
            continue; // the Primals and Zygarde have no set
        for (k = 0; k < MAX_MON_MOVES; k++)
        {
            if (moves[k] == MOVE_NONE)
                break;
            if (moves[k] == zMove)
                found = TRUE;
        }
        // Every move in a set is an attack: the bosses never set up or stall.
        for (k = 0; k < MAX_MON_MOVES; k++)
        {
            if (moves[k] == MOVE_NONE || IsBattleMoveStatus(moves[k]))
                break;
        }
        if (k < MAX_MON_MOVES || !found || IsBattleMoveStatus(zMove))
            return i < ARRAY_COUNT(sMegaRoster) ? (s32)i : 100 + (s32)(i - ARRAY_COUNT(sMegaRoster));
    }
    return -1;
}
#endif

// --- Mable (Hotel Z) ------------------------------------------------------------
// specialvar: report to Mable. The stones stay with the player; she keeps
// the tally (FLAG_WOT_MABLE_TALLY_*).
//   0 -- nothing new: VAR_0x8004 = the tally so far
//   1 -- new samples recorded: VAR_0x8004 = the new tally
//   2 -- the tally just reached 23: her tests start (the Primal call comes
//        on a later portal day)
//   3 -- the tally is long done: VAR_0x8005 = her stage (MABLE_STAGE_*)
u16 WotMegaDim_MableReport(void)
{
    u32 claimed = WotMegaDim_CountClaimed();
    u32 tally = GetFlagBits(FLAG_WOT_MABLE_TALLY_START, MABLE_TALLY_BITS);

    gSpecialVar_0x8005 = GetMableStage();
    if (GetMableStage() != MABLE_STAGE_TALLY)
        return 3;
    // An older build put the followers option inside these bits by mistake, so an
    // old save's tally can read high. Real claims only ever go up: repair it.
    if (tally > claimed)
    {
        tally = claimed;
        SetFlagBits(FLAG_WOT_MABLE_TALLY_START, MABLE_TALLY_BITS, tally);
    }
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
// tests, Sky-High Mega once its four are caught. Its legends go into
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

// The alert's last line, once the towns are shown: VAR_RESULT = TRUE when
// Emma is the partner, with STR_VAR_1 = her clock ("3:30"); FALSE = 2:00.
void WotMegaDim_BufferAlertClock(void)
{
    u32 seconds = GetFullClockSeconds();

    gSpecialVar_Result = IsPartner(PARTNER_MEGADIM_EMMA);
    ConvertIntToDecimalStringN(gStringVar1, seconds / 60, STR_CONV_MODE_LEFT_ALIGN, 1);
    StringAppend(gStringVar1, COMPOUND_STRING(":"));
    ConvertIntToDecimalStringN(gStringVar2, seconds % 60, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar1, gStringVar2);
}

// --- Countdown HUD ----------------------------------------------------------
// A small m:ss box in the top-left corner. It only shows while the player
// has free control, and is taken down (tilemap cleared, window removed)
// whenever a script, menu, fade or the map-name pop-up is up, so it never
// draws over anything. The task dies with the task pool when the field is
// torn down; the map's ON_RESUME re-arms it.

#define tWindowId   data[0]
#define tShownTime  data[1]
#define tShownSignal data[2]
#define tShownFlash data[3]

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

// The wider HUD on a map with a boss: the clock, then "BOSS" and its signal.
static const struct WindowTemplate sHudWindowTemplate_Signal =
{
    .bg = 0,
    .tilemapLeft = 0,
    .tilemapTop = 0,
    .width = 9,
    .height = 2,
    .paletteNum = 15,
    .baseBlock = HUD_BASE_BLOCK,
};

// How close the boss is, by tiles walked as the crow flies: 1 (far) to 4.
// No direction, so finding it is still a hunt. 0 = no boss on this map.
#define HUD_SIGNAL_BARS 4

static u32 GetBossSignal(void)
{
    s16 x, y;
    u32 distance;

    if (!sBossHere)
        return 0;
    PlayerGetDestCoords(&x, &y);
    distance = abs(x - MAP_OFFSET - sBossX) + abs(y - MAP_OFFSET - sBossY);
    if (distance <= 4)
        return 4;
    if (distance <= 10)
        return 3;
    if (distance <= 18)
        return 2;
    return 1;
}

static void RefreshBossTarget(void)
{
    struct MegaDimInstance inst;
    u32 r = GetSession(), i;

    sBossHere = FALSE;
    sBossLocalId = LOCALID_NONE;
    if (r >= MEGA_DIM_RECORDS)
        return;
    DeriveInstance(r, &inst);
    if (!IsCurrentMap(inst.dim->map))
        return;     // a sanctum: no boss of this kind
    sBossHere = TRUE;
    sBossX = inst.dim->slots[inst.megaSlot].x;
    sBossY = inst.dim->slots[inst.megaSlot].y;
    sBossAuraColor = MaskGet(r, MASK_MEGA_SHINY) ? BOSS_AURA_COLOR_SHINY : BOSS_AURA_COLOR;
    if (GetRecordRival(r) != 0)
        return;     // a rival stands there: nothing to glow
    for (i = 0; i < gMapHeader.events->objectEventCount && i < OBJECT_EVENT_TEMPLATES_COUNT; i++)
    {
        if (gSaveBlock1Ptr->objectEventTemplates[i].graphicsId == OBJ_EVENT_GFX_VAR_1)
            sBossLocalId = gSaveBlock1Ptr->objectEventTemplates[i].localId;
    }
}

static bool32 ShouldShowHud(void)
{
    return WotMegaDim_IsActive()
        && !ArePlayerFieldControlsLocked()
        && !ScriptContext_IsEnabled()
        && !MapNamePopupIsActive()
        && !gPaletteFade.active;
}

static void DrawHud(u8 windowId, u16 timeLeft, u32 signal, u32 shards)
{
    u8 *str = gStringVar4;
    u32 i;

    str = ConvertIntToDecimalStringN(str, timeLeft / 60, STR_CONV_MODE_LEFT_ALIGN, 1);
    *str++ = CHAR_COLON;
    ConvertIntToDecimalStringN(str, timeLeft % 60, STR_CONV_MODE_LEADING_ZEROS, 2);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    AddTextPrinterParameterized(windowId, FONT_SMALL, gStringVar4, 6, 1, TEXT_SKIP_DRAW, NULL);
    if (signal != 0)
    {
        // A wild Pokemon's Z-Shards flash here for a moment.
        if (shards != 0)
        {
            gStringVar4[0] = CHAR_PLUS;
            ConvertIntToDecimalStringN(gStringVar4 + 1, shards, STR_CONV_MODE_LEFT_ALIGN, 2);
        }
        AddTextPrinterParameterized(windowId, FONT_SMALL, shards != 0 ? gStringVar4 : COMPOUND_STRING("BOSS"), 31, 1, TEXT_SKIP_DRAW, NULL);
        // Rising bars, lit up to the signal.
        for (i = 0; i < HUD_SIGNAL_BARS; i++)
        {
            u32 height = 3 + i * 2;

            FillWindowPixelRect(windowId, PIXEL_FILL(i < signal ? 2 : 3), 54 + i * 4, 12 - height, 3, height);
        }
    }
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
    u32 signal;

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
        tWindowId = AddWindow(sBossHere ? &sHudWindowTemplate_Signal : &sHudWindowTemplate);
        if (tWindowId == WINDOW_NONE)
            return;
        PutWindowTilemap(tWindowId);
        tShownTime = -1;
    }
    signal = GetBossSignal();
    if (sShardFlashFrames != 0)
    {
        if (sShardFlashFrames == ZSHARD_FLASH_FRAMES)
            PlaySE(SE_SHOP);
        sShardFlashFrames--;
    }
    if (tShownTime != timeLeft || tShownSignal != (s32)signal || tShownFlash != (sShardFlashFrames != 0))
    {
        tShownTime = timeLeft;
        tShownSignal = signal;
        tShownFlash = (sShardFlashFrames != 0);
        DrawHud(tWindowId, timeLeft, signal, sShardFlashFrames != 0 ? sShardFlashAmount : 0);
    }
}

void WotMegaDim_StartHud(void)
{
    u8 taskId = FindTaskIdByFunc(Task_MegaDimHud);

    // ON_RESUME only runs after the field (and its windows) were rebuilt, so a
    // surviving task's window id is stale: drop it without touching it.
    if (taskId != TASK_NONE)
        DestroyTask(taskId);
    RefreshBossTarget();
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

// The next trip in: the portal's Mega or legend and its first wild Pokemon
// come up shiny (to see them stay shiny across trips through the same portal).
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
#define SHIN_TOKYO_HOTEL_Z_X 12
#define SHIN_TOKYO_HOTEL_Z_Y 17

static bool32 IsWarpMap(const struct WarpData *warp, u16 map)
{
    return warp->mapGroup == MAP_GROUP(map) && warp->mapNum == MAP_NUM(map);
}

void WotHotelZ_SetExit(void)
{
    // Walking in through a warp has already pointed the exit at that warp, so
    // Shin Tokyo's two-wide door lets you out the side you came in.
    if (IsWarpMap(&gLastUsedWarp, MAP_SHIN_TOKYO))
        return;
    if (IsWarpMap(&gLastUsedWarp, MAP_TRADEWIND_TOWN)
          || (!IsWarpMap(&gSaveBlock1Ptr->dynamicWarp, MAP_SHIN_TOKYO)
           && !IsWarpMap(&gSaveBlock1Ptr->dynamicWarp, MAP_TRADEWIND_TOWN)))
        SetDynamicWarp(0, MAP_GROUP(MAP_TRADEWIND_TOWN), MAP_NUM(MAP_TRADEWIND_TOWN), TRADEWIND_HOTEL_Z_WARP);
}

// Shin Tokyo's on-load: Hotel Z's two-wide door is shut until the Mega Doomsday
// intro.
void WotHotelZ_SetupShinTokyoDoor(void)
{
    bool32 locked = !FlagGet(FLAG_WOT_MEGA_DOOMSDAY_INTRO);

    MapGridSetMetatileImpassabilityAt(SHIN_TOKYO_HOTEL_Z_X + MAP_OFFSET, SHIN_TOKYO_HOTEL_Z_Y + MAP_OFFSET, locked);
    MapGridSetMetatileImpassabilityAt(SHIN_TOKYO_HOTEL_Z_X + 1 + MAP_OFFSET, SHIN_TOKYO_HOTEL_Z_Y + MAP_OFFSET, locked);
}

// Jumps Mable's quest chain to its next call, as if earned:
//   collecting -> all 23 stones claimed and reported (the debug script
//                 completes the quest), her Primal call due now
//   tests      -> the Primal call due now
//   Primals    -> all four caught, the Rayquaza call due now
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
            WotMegaDim_SetMegaClaimed(i, TRUE);
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
        FlagSet(FLAG_WOT_CAUGHT_MEGA_LUGIA);
        FlagSet(FLAG_WOT_CAUGHT_MEGA_HO_OH);
        SetMableStage(MABLE_STAGE_RAYQUAZA_CALL);
        break;
    }
    gSpecialVar_Result = GetMableStage();
}

// --- Partners: Hotel Z and the scripts ------------------------------------------
// VAR_0x8004 = a rival (1-6, sRivals order). VAR_RESULT = 0 not yet beaten,
// 1 unlocked (recruitable), 2 your partner.
void WotMegaDim_GetRivalState(void)
{
    u32 rival = gSpecialVar_0x8004;

    if (rival == 0 || rival > ARRAY_COUNT(sRivals) || !IsRivalBeaten(rival - 1))
        gSpecialVar_Result = 0;
    else
        gSpecialVar_Result = (GetPartner() == rival) ? 2 : 1;
}

// VAR_0x8004 = the new partner (1-6), or 0 to go solo.
void WotMegaDim_SetPartner(void)
{
    SetPartnerBits(gSpecialVar_0x8004 <= ARRAY_COUNT(sRivals) ? gSpecialVar_0x8004 : 0);
}

// VAR_RESULT = the partner (0 = none), for the follower's own talk script.
void WotMegaDim_BufferPartner(void)
{
    gSpecialVar_Result = GetPartner();
}

// A friendship level-up to announce (MegaDim_EventScript_FriendshipUp):
//   VAR_RESULT = the partner, 1-6 (0 = none); STR_VAR_1 = the new level,
//   STR_VAR_2 = what their perk does now; STR_VAR_3 = at max friendship, the
//   ability that opens their Rift Sanctum's barrier, else empty.
void WotMegaDim_TakeFriendshipUp(void)
{
    s32 i = FindPendingFriendship();
    u32 level;

    gSpecialVar_Result = 0;
    if (i < 0)
        return;
    level = gSaveBlock1Ptr->wotPartners[i].pendingLevel;
    gSaveBlock1Ptr->wotPartners[i].pendingLevel = 0;
    ConvertIntToDecimalStringN(gStringVar1, level, STR_CONV_MODE_LEFT_ALIGN, 1);
    StringCopy(gStringVar2, WotPartner_GetPerkText(i, level));
    gStringVar3[0] = EOS;
    if (level >= FRIENDSHIP_MAX_LEVEL)
    {
        StringCopy(gStringVar3, COMPOUND_STRING("\pAnd I can feel a new power…\nAbility unlocked: "));
        StringAppend(gStringVar3, sSanctums[i].ability);
        StringAppend(gStringVar3, COMPOUND_STRING("!"));
    }
    gSpecialVar_Result = i + 1;
}

// Two Hearts One Rift has been earned; the script completes the quest.
void WotMegaDim_TakeTwoHearts(void)
{
    FlagClear(FLAG_WOT_TWO_HEARTS_PENDING);
}

// Hotel Z ON_TRANSITION: Emma, Taunie, Tulip and Jacinthe (hidden by
// FLAG_TEMP_1-4) only check in once they have been beaten in the dimension.
// Lida and Canari are Team MZ and are always there.
void WotHotelZ_ShowRivals(void)
{
    u32 i;

    for (i = 0; i < 4; i++)
    {
        if (IsRivalBeaten(2 + i))
            FlagClear(FLAG_TEMP_1 + i);
        else
            FlagSet(FLAG_TEMP_1 + i);
    }
}

// Debug: every rival beaten (unlocked), and the partner cycles none -> 1 -> ... -> 6.
// VAR_RESULT = the new partner.
// Debug "+5 runs": the current partner gains 5 portal runs (one friendship
// level). VAR_RESULT = their runs now, VAR_0x8004 = their level (0 = none).
void WotMegaDim_DebugPartnerRuns(void)
{
    u32 partner = GetPartner(), n;

    gSpecialVar_Result = 0;
    gSpecialVar_0x8004 = 0;
    if (partner == 0)
        return;
    for (n = 0; n < FRIENDSHIP_RUNS_PER_LEVEL; n++)
        CountPartnerRun(partner - 1);
    gSpecialVar_Result = WotPartner_GetRuns(partner - 1);
    gSpecialVar_0x8004 = WotPartner_GetLevel(partner - 1);
}

// --- Z-Power ---------------------------------------------------------
// Rogue Megas teach the player Z-Power. Mable gives out the ring after the first
// stone; rift battles pay Z-Shards; AZ at Hotel Z's desk trades them for the
// crystals of the types you've claimed. No save data beyond the bag, the claim
// flags and one bit of each partner's sanctum byte.

// battle_z_move.c ActivateZMove: did the rogue throw its Z-Move this battle?
void WotMegaDim_NoteZMoveUsed(u32 battler)
{
    if (WotMegaDim_IsRogueZMoveUser(battler))
        sRogueThrewZ = TRUE;
}

// battle_z_move.c: the Rogue Core. In any battle, the player's own Mega throws
// a crystal-free Z-Move as the rogues do. Their partner doesn't get this.
bool32 WotMegaDim_IsRogueCoreZUser(u32 battler)
{
    return GetBattlerPosition(battler) == B_POSITION_PLAYER_LEFT
        && GetActiveGimmick(battler) == GIMMICK_MEGA
        && CheckBagHasItem(ITEM_ROGUE_CORE, 1);
}

// VAR_0x8004 = how many. STR_VAR_1 = the same, for the line.
void WotZShards_Give(void)
{
    GiveZShards(gSpecialVar_0x8004);
}

// Before WotMegaDim_MarkClaimed, so a first claim can tell. STR_VAR_1 = how many.
void WotZShards_GiveBoss(void)
{
    u32 r = GetSession(), count;

    if (GetRecordLegend(r) != NULL)
        count = ZSHARDS_LEGEND;
    else
        count = IsMegaClaimed(GetRecordMega(r)) ? ZSHARDS_ROGUE_REMATCH : ZSHARDS_ROGUE_FIRST;
    if (sRogueThrewZ)
        count += ZSHARDS_ROGUE_THREW_Z;
    sRogueThrewZ = FALSE;
    GiveZShards(count);
}

// Older builds set FLAG_WOT_CAUGHT_ZYGARDE on winning any partner trainer battle
// (SetBattledTrainersFlags). Zygarde can't be caught before the machine is sealed,
// so a flag set any earlier is false. The Cube goes too: keeping it would stop Mable
// giving the real reward later. A Floette she already gave stays with the player.
static void RepairFalseZygardeCatch(void)
{
    if (FlagGet(FLAG_WOT_CAUGHT_ZYGARDE) && WotZygarde_GetStage() < ZYGARDE_STAGE_SEALED)
    {
        FlagClear(FLAG_WOT_CAUGHT_ZYGARDE);
        if (CheckBagHasItem(ITEM_ZYGARDE_CUBE, 1))
            RemoveBagItem(ITEM_ZYGARDE_CUBE, 1);
    }
}

// What Mable has for you when you talk to her: VAR_RESULT = 1 the ring (a
// claim made, no ring yet, older saves too), 2 the Rogue Core (Mega Rayquaza
// caught), else 0.
u16 WotMegaDim_MableGiftDue(void)
{
    RepairFalseZygardeCatch();
    if (WotMegaDim_CountClaimed() != 0 && !CheckBagHasItem(ITEM_Z_POWER_RING, 1))
        return 1;
    if (FlagGet(FLAG_WOT_CAUGHT_MEGA_RAYQUAZA) && !CheckBagHasItem(ITEM_ROGUE_CORE, 1))
        return 2;
    if (FlagGet(FLAG_WOT_CAUGHT_ZYGARDE) && !CheckBagHasItem(ITEM_ZYGARDE_CUBE, 1))
        return 3;
    return 0;
}

#define ZSHOP_PRICE_TYPE    40
#define ZSHOP_PRICE_SNORLIUM 60

u32 WotZShop_GetPrice(u16 item)
{
    return item == ITEM_SNORLIUM_Z ? ZSHOP_PRICE_SNORLIUM : ZSHOP_PRICE_TYPE;
}

static const struct { u8 type; u16 crystal; } sZCrystals[] =
{
    {TYPE_NORMAL,   ITEM_NORMALIUM_Z},
    {TYPE_FIRE,     ITEM_FIRIUM_Z},
    {TYPE_WATER,    ITEM_WATERIUM_Z},
    {TYPE_ELECTRIC, ITEM_ELECTRIUM_Z},
    {TYPE_GRASS,    ITEM_GRASSIUM_Z},
    {TYPE_ICE,      ITEM_ICIUM_Z},
    {TYPE_FIGHTING, ITEM_FIGHTINIUM_Z},
    {TYPE_POISON,   ITEM_POISONIUM_Z},
    {TYPE_GROUND,   ITEM_GROUNDIUM_Z},
    {TYPE_FLYING,   ITEM_FLYINIUM_Z},
    {TYPE_PSYCHIC,  ITEM_PSYCHIUM_Z},
    {TYPE_BUG,      ITEM_BUGINIUM_Z},
    {TYPE_ROCK,     ITEM_ROCKIUM_Z},
    {TYPE_GHOST,    ITEM_GHOSTIUM_Z},
    {TYPE_DRAGON,   ITEM_DRAGONIUM_Z},
    {TYPE_DARK,     ITEM_DARKINIUM_Z},
    {TYPE_STEEL,    ITEM_STEELIUM_Z},
    {TYPE_FAIRY,    ITEM_FAIRIUM_Z},
};

// One slot per crystal, Snorlium, and the end marker.
static EWRAM_DATA u16 sZShopStock[ARRAY_COUNT(sZCrystals) + 2] = {0};

// Normalium, then the crystals of every claimed rogue's two types. All of them
// once all 23 are claimed, which is what brings in Ice, Psychic and Ghost: no
// rogue has those types.
const u16 *WotZShop_BuildStock(void)
{
    u32 i, n = 0, types = 1u << TYPE_NORMAL;
    bool32 allClaimed = WotMegaDim_CountClaimed() == ARRAY_COUNT(sMegaRoster);
    bool32 snorlium = FALSE;

    for (i = 0; i < ARRAY_COUNT(sMegaRoster); i++)
    {
        if (!IsMegaClaimed(i))
            continue;
        types |= 1u << gSpeciesInfo[sMegaRoster[i].megaSpecies].types[0];
        types |= 1u << gSpeciesInfo[sMegaRoster[i].megaSpecies].types[1];
        if (sMegaRoster[i].baseSpecies == SPECIES_SNORLAX)
            snorlium = TRUE;
    }
    for (i = 0; i < ARRAY_COUNT(sZCrystals); i++)
    {
        if (allClaimed || (types & (1u << sZCrystals[i].type)))
            sZShopStock[n++] = sZCrystals[i].crystal;
    }
    if (snorlium)
        sZShopStock[n++] = ITEM_SNORLIUM_Z;
    sZShopStock[n] = ITEM_NONE;
    return sZShopStock;
}

// AZ's desk. Waits like pokemart (data/specials.inc).
void WotZShop_Open(void)
{
    CreateZShardMartMenu(WotZShop_BuildStock());
}

// A partner's crystal, the first time they're talked to at friendship Lv3+.
//   in  VAR_0x8004 = the rival (1-6)
//   out VAR_RESULT = the crystal, or ITEM_NONE; gWotPartnerLine = what they say
#define PARTNER_CRYSTAL_GIVEN (1 << 5)

void WotPartner_TakeCrystalGift(void)
{
    static const u16 sPartnerCrystals[WOT_PARTNER_SAVE_COUNT] =
    {
        ITEM_ELECTRIUM_Z, ITEM_FIGHTINIUM_Z, ITEM_DARKINIUM_Z, ITEM_DRAGONIUM_Z, ITEM_PSYCHIUM_Z, ITEM_STEELIUM_Z,
    };
    u32 rival = gSpecialVar_0x8004 - 1;

    gSpecialVar_Result = ITEM_NONE;
    if (rival >= WOT_PARTNER_SAVE_COUNT || WotPartner_GetLevel(rival) < 3
     || (gSaveBlock1Ptr->wotPartners[rival].sanctum & PARTNER_CRYSTAL_GIVEN)
     || !CheckBagHasSpace(sPartnerCrystals[rival], 1))
        return;
    gSaveBlock1Ptr->wotPartners[rival].sanctum |= PARTNER_CRYSTAL_GIVEN;
    StringCopy(gWotPartnerLine, sPartnerCrystalLines[rival]);
    gSpecialVar_Result = sPartnerCrystals[rival];
}

// Debug "Mega Dim: Z kit": the ring, the Rogue Core and 200 Z-Shards.
void WotMegaDim_DebugZKit(void)
{
    if (!CheckBagHasItem(ITEM_Z_POWER_RING, 1))
        AddBagItem(ITEM_Z_POWER_RING, 1);
    if (!CheckBagHasItem(ITEM_ROGUE_CORE, 1))
        AddBagItem(ITEM_ROGUE_CORE, 1);
    AddBagItem(ITEM_Z_SHARD, 200);
}

// --- The Zygarde arc ---------------------------------------------------
// The gray world is gray because AZ has spent it keeping his Floette alive. Rifts
// have a clock because Zygarde is sealing them. Z-Shards are that stolen energy,
// and every one traded at his desk fed the machine. The dog at the desk isn't his.
// Save data: 2 stage bits, one caught flag, and the bag.

u16 WotZygarde_GetStage(void)
{
    return GetFlagBits(FLAG_WOT_ZYGARDE_STAGE_START, ZYGARDE_STAGE_BITS);
}

// VAR_0x8004 = the stage.
void WotZygarde_SetStage(void)
{
    SetFlagBits(FLAG_WOT_ZYGARDE_STAGE_START, ZYGARDE_STAGE_BITS, gSpecialVar_0x8004);
    // Sealed: every Cell you carried went into Zygarde.
    if (gSpecialVar_0x8004 == ZYGARDE_STAGE_SEALED)
        RemoveBagItem(ITEM_ZYGARDE_CELL, CountTotalItemQuantityInBag(ITEM_ZYGARDE_CELL));
}

static u32 CountCells(void)
{
    return CountTotalItemQuantityInBag(ITEM_ZYGARDE_CELL);
}

// The dog attacks on the next walk into Hotel Z.
u16 WotZygarde_PebbleDue(void)
{
#ifdef WOT_TESTER_HOLD_ZYGARDE
    return FALSE;
#endif
    return FlagGet(FLAG_WOT_MEGA_DOOMSDAY_INTRO)
        && WotMegaDim_CountClaimed() >= ZYGARDE_CLAIMS_FOR_DOG
        && CheckBagHasItem(ITEM_Z_POWER_RING, 1)
        && !CheckBagHasItem(ITEM_ODD_PEBBLE, 1)
        && WotZygarde_GetStage() == ZYGARDE_STAGE_NONE;
}

u16 WotZygarde_RevealDue(void)
{
    return WotZygarde_GetStage() == ZYGARDE_STAGE_DRAIN_CALLED && CountCells() >= ZYGARDE_CELLS_FOR_DESK;
}

static bool32 WotZygarde_EmmaCallDue(void)
{
    return CheckBagHasItem(ITEM_ODD_PEBBLE, 1) && CountCells() >= ZYGARDE_CELLS_FOR_CALL
        && !QuestMenu_GetSetQuestState(QUEST_THE_LONG_STAY, FLAG_GET_UNLOCKED);
}

static bool32 WotZygarde_DrainCallDue(void)
{
    return WotZygarde_GetStage() == ZYGARDE_STAGE_NONE && GetMableStage() == MABLE_STAGE_DONE
        && QuestMenu_GetSetQuestState(QUEST_THE_LONG_STAY, FLAG_GET_UNLOCKED);
}

// Hotel Z's ON_TRANSITION: who's at the desk, and where the dog is.
//   FLAG_TEMP_5 AZ, FLAG_TEMP_6 the dog, FLAG_TEMP_7 the rift at the desk.
void WotZygarde_SetupDesk(void)
{
    static const struct MegaDimSlot sDogAtDoor = {.x = 7, .y = 9, .elevation = 3};
    u32 stage = WotZygarde_GetStage();
    bool32 azGone = stage >= ZYGARDE_STAGE_AZ_BEATEN;

    if (!FlagGet(FLAG_WOT_MEGA_DOOMSDAY_INTRO) || azGone)
        FlagSet(FLAG_TEMP_5);
    if (!FlagGet(FLAG_WOT_MEGA_DOOMSDAY_INTRO) || azGone || !CheckBagHasItem(ITEM_Z_POWER_RING, 1))
        FlagSet(FLAG_TEMP_6);
#ifdef WOT_TESTER_HOLD_ZYGARDE
    FlagSet(FLAG_TEMP_6);
#endif
    if (!azGone || FlagGet(FLAG_WOT_CAUGHT_ZYGARDE))
        FlagSet(FLAG_TEMP_7);
    if (WotZygarde_PebbleDue())
        PlaceTemplate(OBJ_EVENT_GFX_SPECIES(ZYGARDE_10_AURA_BREAK), &sDogAtDoor);
}

// The Drained Hotel's ON_TRANSITION.
//   FLAG_TEMP_1 Floette, 2 AZ, 3 the machine, 4 the dog, 5 Zygarde.
void WotZygarde_SetupDrainedHotel(void)
{
    FlagSet(FLAG_TEMP_4);
    if (WotZygarde_GetStage() >= ZYGARDE_STAGE_SEALED)
    {
        FlagSet(FLAG_TEMP_1);
        FlagSet(FLAG_TEMP_2);
        FlagSet(FLAG_TEMP_3);
        if (FlagGet(FLAG_WOT_CAUGHT_ZYGARDE))
            FlagSet(FLAG_TEMP_5);
    }
    else
    {
        FlagSet(FLAG_TEMP_5);
    }
}

// MegaDim_EventScript_WildMon: is this slot a Cell rather than a Pokemon?
u16 WotZygarde_IsTalkedCell(void)
{
    u8 objId = GetObjectEventIdByLocalIdAndMap(gSpecialVar_LastTalked, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);

    return objId < OBJECT_EVENTS_COUNT && gObjectEvents[objId].graphicsId == OBJ_EVENT_GFX_WOT_ZYGARDE_CELL;
}

// Taking one slows the closing, up to the portal's full clock (Emma's longer
// one with her along). VAR_RESULT = TRUE on the very first Cell.
void WotZygarde_TakeCell(void)
{
    u32 r = GetSession();

    AddBagItem(ITEM_ZYGARDE_CELL, 1);
    if (r != RECORD_NONE)
        SetSecondsLeft(r, min(GetSecondsLeft(r) + ZYGARDE_CELL_SECONDS, GetFullClockSeconds()));
    MarkTalkedSlotGone(TRUE);
    gSpecialVar_Result = (CountCells() == 1);
}

// STR_VAR_1 = the Cells held, STR_VAR_2 = the Cells wanted.
static void BufferCells(void)
{
    ConvertIntToDecimalStringN(gStringVar1, CountCells(), STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar2, ZYGARDE_CELLS_FOR_DESK, STR_CONV_MODE_LEFT_ALIGN, 2);
}

// Talking to the dog. VAR_RESULT = TRUE: it's counting your Cells.
u16 WotZygarde_DogCounts(void)
{
    BufferCells();
    return WotZygarde_GetStage() == ZYGARDE_STAGE_DRAIN_CALLED && CountCells() < ZYGARDE_CELLS_FOR_DESK;
}

// What Mable adds when you talk to her:
//   0 nothing, 1 what Cells are, 2 the drain and the count, 3 it's over,
//   4 enough Cells: go to AZ's desk.
u16 WotZygarde_MableLine(void)
{
    u32 stage = WotZygarde_GetStage();

    BufferCells();
    if (stage == ZYGARDE_STAGE_DRAIN_CALLED)
        return CountCells() >= ZYGARDE_CELLS_FOR_DESK ? 4 : 2;
    if (stage >= ZYGARDE_STAGE_SEALED)
        return 3;
    if (stage == ZYGARDE_STAGE_NONE && CountCells() >= ZYGARDE_CELLS_FOR_CALL)
        return 1;
    return 0;
}

static void SetPerfectIVs(struct Pokemon *mon)
{
    u32 i, iv = MAX_PER_STAT_IVS;

    for (i = 0; i < NUM_STATS; i++)
        SetMonData(mon, MON_DATA_HP_IV + i, &iv);
    CalculateMonStats(mon);
}

// A bit under the party's best: it's testing you.
void WotZygarde_SetupDogBattle(void)
{
    CreateScriptedWildMon(SPECIES_ZYGARDE_10_AURA_BREAK, max((s32)GetBossLevel() - 5, 15), ITEM_NONE);
    SetPerfectIVs(&gEnemyParty[0]);
    gSpecialVar_0x8004 = SPECIES_ZYGARDE_10_AURA_BREAK;
}

// Mega Floette, a rogue like the others: it Mega Evolves on turn one and has its
// Z-Move on top, from Light of Ruin.
void WotZygarde_SetupFloetteBattle(void)
{
    static const u16 sMoves[MAX_MON_MOVES] = {MOVE_LIGHT_OF_RUIN, MOVE_MOONBLAST, MOVE_PSYCHIC, MOVE_ENERGY_BALL};
    u32 i;

    CreateScriptedWildMon(SPECIES_FLOETTE_ETERNAL, min(max((u32)GetBossLevel() + 2, 70u), (u32)MAX_LEVEL), ITEM_FLOETTITE);
    for (i = 0; i < MAX_MON_MOVES; i++)
        SetMonMoveSlot(&gEnemyParty[0], sMoves[i], i);
    SetPerfectIVs(&gEnemyParty[0]);
    FlagSet(FLAG_WOT_SMART_WILD_AI);
    sRogueThrewZ = FALSE;
    sSanctumRogue = FALSE;
    sBossZOverride = MOVE_LIGHT_OF_RUIN;
    gSpecialVar_0x8004 = SPECIES_FLOETTE_ETERNAL;
}

void WotZygarde_SetupZygardeBattle(void)
{
    static const u16 sMoves[MAX_MON_MOVES] = {MOVE_THOUSAND_ARROWS, MOVE_CORE_ENFORCER, MOVE_DRAGON_PULSE, MOVE_EXTREME_SPEED};
    u32 i;

    CreateScriptedWildMon(SPECIES_ZYGARDE_COMPLETE, min(max((u32)GetBossLevel(), 75u), (u32)MAX_LEVEL), ITEM_ZYGARDITE);
    for (i = 0; i < MAX_MON_MOVES; i++)
        SetMonMoveSlot(&gEnemyParty[0], sMoves[i], i);
    SetPerfectIVs(&gEnemyParty[0]);
    FlagSet(FLAG_WOT_SMART_WILD_AI);
    sRogueThrewZ = FALSE;
    sSanctumRogue = FALSE;
    sBossZOverride = MOVE_THOUSAND_ARROWS;
    gSpecialVar_0x8004 = SPECIES_ZYGARDE_COMPLETE;
}

// battle_main.c CreateNPCTrainerParty: AZ's three sit at or just over your best.
void WotZygarde_AdjustAZParty(struct Pokemon *party, u16 trainerId)
{
    u32 k, best = max((u32)GetHighestPartyLevel() + 2, 60u);

    if (trainerId != TRAINER_HOTEL_Z_AZ)
        return;
    for (k = 0; k < 3; k++)
        SetMonLevel(&party[k], min(best - (2 - k), (u32)MAX_LEVEL));
}

// Debug "Mega Dim: Zygarde next": the arc's next beat, as if earned.
//   1 no ring/pebble -> 3 claims and the ring: walk into Hotel Z for the dog
//   2 pebble         -> 20 Cells and Mega Rayquaza caught: Emma's and Mable's calls
//   3 Mable's called -> the rift at the desk open, AZ beaten
//   4 AZ beaten      -> sealed, Zygarde waiting
// VAR_RESULT = the step just taken.
void WotZygarde_DebugNext(void)
{
    u32 i;

    FlagSet(FLAG_WOT_MEGA_DOOMSDAY_INTRO);
    EnsureV2();
    if (!CheckBagHasItem(ITEM_ODD_PEBBLE, 1) && WotZygarde_GetStage() == ZYGARDE_STAGE_NONE)
    {
        for (i = 0; i < ZYGARDE_CLAIMS_FOR_DOG; i++)
            FlagSet(FLAG_WOT_MEGA_CLAIMED_AMOONGUSS + i);
        if (!CheckBagHasItem(ITEM_Z_POWER_RING, 1))
            AddBagItem(ITEM_Z_POWER_RING, 1);
        gSpecialVar_Result = 1;
    }
    else if (WotZygarde_GetStage() == ZYGARDE_STAGE_NONE)
    {
        if (CountCells() < ZYGARDE_CELLS_FOR_DESK)
            AddBagItem(ITEM_ZYGARDE_CELL, ZYGARDE_CELLS_FOR_DESK - CountCells());
        FlagSet(FLAG_WOT_CAUGHT_PRIMAL_GROUDON);
        FlagSet(FLAG_WOT_CAUGHT_PRIMAL_KYOGRE);
        FlagSet(FLAG_WOT_CAUGHT_MEGA_LUGIA);
        FlagSet(FLAG_WOT_CAUGHT_MEGA_HO_OH);
        FlagSet(FLAG_WOT_CAUGHT_MEGA_RAYQUAZA);
        SetMableStage(MABLE_STAGE_DONE);
        gSpecialVar_Result = 2;
    }
    else
    {
        gSpecialVar_0x8004 = min(WotZygarde_GetStage() + 1, (u32)ZYGARDE_STAGE_SEALED);
        WotZygarde_SetStage();
        gSpecialVar_Result = 2 + gSpecialVar_0x8004 - 1;
    }
}

// --- The Mega Log: src/wot_mega_log.c --------------------------------
// The 23 rogue Megas, then Mable's legends. An entry shows as obtained once
// the dimension's own claim flag is set, or once a redeemed Mystery Code gave
// that species (src/wot_mystery_code.c). The code doesn't set the claim, so
// Mable's tally and the quests only ever count the dimension's catches.
u32 WotMegaLog_RogueCount(void)
{
    return ARRAY_COUNT(sMegaRoster);
}

u32 WotMegaLog_LegendCount(void)
{
    return ARRAY_COUNT(sLegends);
}

void WotMegaLog_GetEntry(u32 i, struct WotMegaLogEntry *entry)
{
    if (i < ARRAY_COUNT(sMegaRoster))
    {
        entry->species = sMegaRoster[i].megaSpecies;
        entry->iconSpecies = sMegaRoster[i].baseSpecies;
        entry->stone = sMegaRoster[i].stone;
        entry->claimed = IsMegaClaimed(i) || WotMysteryCode_IsSpeciesRedeemed(sMegaRoster[i].baseSpecies);
        entry->primal = FALSE;
    }
    else
    {
        const struct MegaDimLegend *legend = &sLegends[i - ARRAY_COUNT(sMegaRoster)];

        RepairFalseZygardeCatch();
        entry->species = legend->formSpecies;
        entry->iconSpecies = legend->species;
        entry->stone = legend->item;
        entry->claimed = FlagGet(legend->caughtFlag) || WotMysteryCode_IsSpeciesRedeemed(legend->species);
        entry->primal = legend->species == SPECIES_GROUDON || legend->species == SPECIES_KYOGRE;
    }
}

// --- Rift Sanctums: the barriers and the sanctums -------------------

const u8 *WotPartner_GetAbilityName(u32 i)
{
    return sSanctums[i].ability;
}

// The sanctum the player is standing in (or whose barrier map this is):
// its row, or -1.
static s32 GetSanctumHere(bool32 barrierMap)
{
    u16 here = (gSaveBlock1Ptr->location.mapGroup << 8) | gSaveBlock1Ptr->location.mapNum;
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sSanctums); i++)
    {
        if ((barrierMap ? sSanctums[i].barrierMap : sSanctums[i].sanctumMap) == here)
            return i;
    }
    return -1;
}

// battle_bg.c: a sanctum's battles (the holdouts and the legend) are fought on
// the black-and-white graveyard, not the dimension's white void.
bool32 WotMegaDim_IsInSanctum(void)
{
    return GetSanctumHere(FALSE) >= 0;
}

// The dimension map's ON_TRANSITION: a barrier only stands once the game has
// been beaten -- the dimension opens in Act 2, the Shadows belong to the end.
static void HideBarrierBeforePostgame(void)
{
    s32 i = GetSanctumHere(TRUE);

    if (i >= 0 && !FlagGet(FLAG_SYS_GAME_CLEAR))
        FlagSet(sSanctums[i].barrierFlag);
}

// Talking to a barrier. VAR_0x8004 = its sanctum (sRivals order).
//   VAR_RESULT = 1 -- its partner is along at max friendship: it opens
//                0 -- anyone else (gWotPartnerLine = the barrier's hint)
// STR_VAR_1 = the partner, STR_VAR_2 = the ability.
void WotSanctum_CheckBarrier(void)
{
    u32 i = min(gSpecialVar_0x8004, ARRAY_COUNT(sSanctums) - 1);

    gSpecialVar_Result = 0;
    StringCopy(gStringVar1, GetTrainerNameFromId(sRivals[i].trainerId));
    StringCopy(gStringVar2, sSanctums[i].ability);
    if (GetPartner() == i + 1 && FlagGet(FLAG_WOT_MEGA_DIM_PARTNER_ON) && WotPartner_GetLevel(i) >= FRIENDSHIP_MAX_LEVEL)
    {
        StringCopy(gWotPartnerLine, COMPOUND_STRING("{STR_VAR_1} used {STR_VAR_2}!\pThe barrier gives way!"));
        gSpecialVar_Result = 1;
        return;
    }
    StringCopy(gWotPartnerLine, sSanctums[i].hint);
    if (GetPartner() == i + 1 && FlagGet(FLAG_WOT_MEGA_DIM_PARTNER_ON))
        StringAppend(gWotPartnerLine, COMPOUND_STRING("\p{STR_VAR_1} feels something stir…\nbut not enough. Not yet."));
}

static void WarpWithinDimension(u16 map, s8 x, s8 y)
{
    SetWarpDestination(map >> 8, map & 0xFF, WARP_ID_NONE, x, y);
    sForceOnFoot = TRUE;
    DoWhiteFadeWarp();
    ResetInitialPlayerAvatarState();
}

// Through an opened barrier. VAR_0x8004 = the sanctum.
void WotSanctum_Enter(void)
{
    const struct MegaDimSanctum *sanctum = &sSanctums[min(gSpecialVar_0x8004, ARRAY_COUNT(sSanctums) - 1)];

    WarpWithinDimension(sanctum->sanctumMap, sanctum->arriveX, sanctum->arriveY);
}

// The sanctum's exit rift: back beside the barrier, with the session still
// running. Coming back re-runs the dimension map's ON_TRANSITION, which would
// roll the shinies again for free -- the exit portal charges 5 s to stop
// exactly that -- so this one return skips the rolls.
void WotSanctum_Leave(void)
{
    s32 i = GetSanctumHere(FALSE);

    if (i < 0)
        return;
    sNoShinyReroll = TRUE;
    WarpWithinDimension(sSanctums[i].barrierMap, sSanctums[i].backX, sSanctums[i].backY);
}

// The sanctum's ON_TRANSITION: beaten holdouts stay gone (FLAG_TEMP_2-4), and
// the legend (FLAG_TEMP_5) waits until all three are and until it's caught.
void WotSanctum_Setup(void)
{
    s32 i = GetSanctumHere(FALSE);
    u32 state, k;

    if (i < 0)
        return;
    state = gSaveBlock1Ptr->wotPartners[i].sanctum;
    for (k = 0; k < 3; k++)
    {
        if (state & SANCTUM_HOLDOUT(k))
            FlagSet(FLAG_TEMP_2 + k);
    }
    if ((state & SANCTUM_HOLDOUTS_ALL) != SANCTUM_HOLDOUTS_ALL || (state & SANCTUM_LEGEND_CAUGHT))
        FlagSet(FLAG_TEMP_5);
    VarSet(VAR_OBJ_GFX_ID_3, sSanctums[i].legend + OBJ_EVENT_MON);
}

// The sanctum's ON_RESUME: the legend wears the Shadow silhouette.
void WotSanctum_ShadeLegend(void)
{
    if (GetSanctumHere(FALSE) < 0 || FlagGet(FLAG_TEMP_5))
        return;
    gSpecialVar_0x8004 = LOCALID_SANCTUM_LEGEND;
    WotMakeObjectEventShadow();
}

// A holdout lost. VAR_0x8004 = which (0-2). Their trainer id is shared by
// every sanctum, so its flag is cleared again; the sanctum keeps the record.
//   VAR_RESULT = TRUE once all three are beaten.
void WotSanctum_HoldoutBeaten(void)
{
    static const u16 sHoldouts[] = {TRAINER_SANCTUM_HOLDOUT_A, TRAINER_SANCTUM_HOLDOUT_B, TRAINER_SANCTUM_HOLDOUT_C};
    s32 i = GetSanctumHere(FALSE);
    u32 k = min(gSpecialVar_0x8004, 2);

    FlagClear(TRAINER_FLAGS_START + sHoldouts[k]);
    gSpecialVar_Result = FALSE;
    if (i < 0)
        return;
    gSaveBlock1Ptr->wotPartners[i].sanctum |= SANCTUM_HOLDOUT(k);
    gSpecialVar_Result = (gSaveBlock1Ptr->wotPartners[i].sanctum & SANCTUM_HOLDOUTS_ALL) == SANCTUM_HOLDOUTS_ALL;
}

// The legend: the table's species, as a Shadow. VAR_0x8004 = its species (cry).
void WotSanctum_SetupLegendBattle(void)
{
    s32 i = GetSanctumHere(FALSE);
    u32 one = TRUE;

    if (i < 0)
        i = 0;
    CreateScriptedWildMon(sSanctums[i].legend, sSanctums[i].legendLevel, sSanctums[i].legendItem);
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHADOW, &one);
    sSanctumRogue = FALSE;
    sBossZOverride = MOVE_NONE;
    if (sSanctums[i].legendItem != ITEM_NONE)
    {
        u32 k;

        for (k = 0; k < MAX_MON_MOVES; k++)
            SetMonMoveSlot(&gEnemyParty[0], sSanctums[i].legendMoves[k], k);
        SetPerfectIVs(&gEnemyParty[0]);
        sSanctumRogue = TRUE;
        sBossZOverride = sSanctums[i].legendZMove;
        sBossLastStand = LAST_STAND_READY;
        FlagSet(FLAG_WOT_SMART_WILD_AI);
    }
    gSpecialVar_0x8004 = sSanctums[i].legend;
}

void WotSanctum_LegendCaught(void)
{
    s32 i = GetSanctumHere(FALSE);

    if (i >= 0)
        gSaveBlock1Ptr->wotPartners[i].sanctum |= SANCTUM_LEGEND_CAUGHT;
}

// battle_main.c CreateNPCTrainerParty: a holdout's team in this sanctum -- the
// partner-themed Pokemon at 7 under the party's best and its Shadow at 5 under.
void WotMegaDim_AdjustSanctumParty(struct Pokemon *party, u16 trainerId)
{
    s32 i = GetSanctumHere(FALSE);
    u32 k, best, one = TRUE;
    struct OriginalTrainerId otId = OTID_STRUCT_RANDOM_NO_SHINY;

    if (i < 0 || !(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
        return;
    if (trainerId == TRAINER_SANCTUM_HOLDOUT_A)
        k = 0;
    else if (trainerId == TRAINER_SANCTUM_HOLDOUT_B)
        k = 1;
    else if (trainerId == TRAINER_SANCTUM_HOLDOUT_C)
        k = 2;
    else
        return;
    best = GetHighestPartyLevel();
    // CreateMon* gives no moves here; GiveMonInitialMoveset does.
    CreateMonWithIVs(&party[0], sSanctums[i].holdoutMon[k], max((s32)best - 7, 1), Random32(), otId, USE_RANDOM_IVS);
    GiveMonInitialMoveset(&party[0]);
    CreateMonWithIVs(&party[1], sSanctums[i].holdoutShadow[k], max((s32)best - 5, 1), Random32(), otId, USE_RANDOM_IVS);
    GiveMonInitialMoveset(&party[1]);
    SetMonData(&party[1], MON_DATA_IS_SHADOW, &one);
}

// Debug "Mega Dim: sanctum": the game counts as beaten, the partner (Canari
// if none) is at max friendship, and portal 0 leads to their barrier's map.
void WotMegaDim_DebugSanctum(void)
{
    u32 partner = GetPartner(), map;

    if (partner == 0)
    {
        partner = 1;
        FlagSet(TRAINER_FLAGS_START + sRivals[0].trainerId);
        SetPartnerBits(partner);
    }
    FlagSet(FLAG_SYS_GAME_CLEAR);
    gSaveBlock1Ptr->wotPartners[partner - 1].runs = max(gSaveBlock1Ptr->wotPartners[partner - 1].runs, 15);
    for (map = 0; map < DIM_RANDOM_MAP_COUNT; map++)
    {
        if (sDimMaps[map].map == sSanctums[partner - 1].barrierMap && IsRecordOpen(0))
            SetRecord(0, (GetRecord(0) & ~(PORTAL_MAP_MASK << PORTAL_MAP_SHIFT)) | (map << PORTAL_MAP_SHIFT));
    }
    gSpecialVar_Result = partner;
}

void WotMegaDim_DebugPartner(void)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sRivals); i++)
        FlagSet(TRAINER_FLAGS_START + sRivals[i].trainerId);
    SetPartnerBits((GetPartner() + 1) % (ARRAY_COUNT(sRivals) + 1));
    gSpecialVar_Result = GetPartner();
}

// Inside the dimension: ten seconds left, to see the collapse.
void WotMegaDim_DebugShortTimer(void)
{
    u32 r = GetSession();

    if (GetTimer(r) > 10)
        SetTimer(r, 10);
}

// A save from before the update that's past Tradewind has no reason to walk its
// corridor again, so the intro never plays and the Mega Dimension stays shut. A new
// game always crosses the trigger row first: it's the only road into town. FLY is
// the first point where the way back is guaranteed, so that's when the rumor starts.
bool32 WotMegaDim_SaveMissedIntro(void)
{
    return FlagGet(FLAG_WOT_FLY_GRANTED) && !FlagGet(FLAG_WOT_MEGA_DOOMSDAY_INTRO);
}

#ifdef WOT_TESTER
// Test profiles only (src/wot_tester.c).

// A base save can come from a dev run that has played all of this. Back to never
// having seen the Mega Dimension, while its story flags stay as they are.
void WotTester_ResetMegaDimension(void)
{
    static const u16 sFlags[] =
    {
        FLAG_WOT_MEGA_DOOMSDAY_INTRO, FLAG_WOT_HOTEL_Z_MET, FLAG_WOT_MEGA_DIM_V2,
        FLAG_WOT_MEGA_DIM_NEW_0, FLAG_WOT_MEGA_DIM_NEW_1, FLAG_WOT_MEGA_DIM_NEW_2,
        FLAG_WOT_CAUGHT_PRIMAL_GROUDON, FLAG_WOT_CAUGHT_PRIMAL_KYOGRE, FLAG_WOT_CAUGHT_MEGA_RAYQUAZA,
        FLAG_WOT_CAUGHT_MEGA_LUGIA, FLAG_WOT_CAUGHT_MEGA_HO_OH,
        FLAG_WOT_CAUGHT_ZYGARDE, FLAG_WOT_MEGA_DIM_PARTNER_ON, FLAG_WOT_TWO_HEARTS_PENDING,
        FLAG_WOT_SMART_WILD_AI, FLAG_WOT_NO_CATCHING, FLAG_WOT_GIRATINA_DREAM_ARMED,
        FLAG_WOT_GIRATINA_DREAM_FIGHTING, FLAG_WOT_GIRATINA_DREAM_CAUGHT,
    };
    static const u8 sQuests[] =
    {
        QUEST_MEGA_DOOMSDAY, QUEST_TWO_MINUTES, QUEST_PRIMAL_SIGNATURES, QUEST_SKY_HIGH_MEGA,
        QUEST_TWO_HEARTS, QUEST_ORDER_IN_THE_RIFTS, QUEST_THE_LONG_STAY,
    };
    static const u16 sTrainers[] = {TRAINER_SANCTUM_HOLDOUT_A, TRAINER_SANCTUM_HOLDOUT_B, TRAINER_SANCTUM_HOLDOUT_C};
    static const u16 sKeyItems[] = {ITEM_Z_POWER_RING, ITEM_Z_SHARD, ITEM_ROGUE_CORE, ITEM_ODD_PEBBLE, ITEM_ZYGARDE_CELL, ITEM_ZYGARDE_CUBE};
    u32 i, bit;

    for (i = 0; i < ARRAY_COUNT(sFlags); i++)
        FlagClear(sFlags[i]);
    for (i = 0; i < ARRAY_COUNT(sSeenMaps); i++)
        FlagClear(sSeenMaps[i].flag);
    for (i = 0; i < ARRAY_COUNT(sRivals); i++)
        FlagClear(TRAINER_FLAGS_START + sRivals[i].trainerId);
    for (i = 0; i < ARRAY_COUNT(sTrainers); i++)
        FlagClear(TRAINER_FLAGS_START + sTrainers[i]);
    for (i = 0; i < ARRAY_COUNT(sMegaRoster); i++)
        WotMegaDim_SetMegaClaimed(i, FALSE);
    SetFlagBits(FLAG_WOT_MABLE_TALLY_START, MABLE_TALLY_BITS, 0);
    SetFlagBits(FLAG_WOT_MABLE_DAY_START, MABLE_DAY_BITS, 0);
    SetFlagBits(FLAG_WOT_ZYGARDE_STAGE_START, ZYGARDE_STAGE_BITS, 0);
    SetMableStage(MABLE_STAGE_TALLY);
    memset(gSaveBlock1Ptr->wotPartners, 0, sizeof(gSaveBlock1Ptr->wotPartners));
    for (i = 0; i < ARRAY_COUNT(sQuests); i++)
    {
        for (bit = sQuests[i] * 5; bit < sQuests[i] * 5u + 5; bit++)
            gSaveBlock2Ptr->questData[bit / 8] &= ~(1 << (bit % 8));
    }
    for (i = 0; i < ARRAY_COUNT(sKeyItems); i++)
    {
        u32 count = CountTotalItemQuantityInBag(sKeyItems[i]);

        if (count != 0)
            RemoveBagItem(sKeyItems[i], count);
    }
    // With the layout flag clear, this empties the portal vars and their masks.
    EnsureV2();
}

// A real player has walked these maps by the time they matter, and a rift only
// opens, or leads, somewhere already seen.
void WotTester_SetSeenMaps(bool32 shinTokyo)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sSeenMaps); i++)
    {
        if (shinTokyo || sSeenMaps[i].flag != FLAG_WOT_MEGA_DIM_SEEN_SHIN_TOKYO)
            FlagSet(sSeenMaps[i].flag);
    }
}

void WotTester_OpenMegaDimension(void)
{
    FlagSet(FLAG_WOT_MEGA_DOOMSDAY_INTRO);
    QuestMenu_GetSetQuestState(QUEST_MEGA_DOOMSDAY, FLAG_SET_UNLOCKED);
    QuestMenu_GetSetQuestState(QUEST_MEGA_DOOMSDAY, FLAG_SET_ACTIVE);
}

void WotTester_MaxPartners(void)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sRivals); i++)
    {
        FlagSet(TRAINER_FLAGS_START + sRivals[i].trainerId);
        gSaveBlock1Ptr->wotPartners[i].runs = max(gSaveBlock1Ptr->wotPartners[i].runs, (FRIENDSHIP_MAX_LEVEL - 1) * FRIENDSHIP_RUNS_PER_LEVEL);
        gSaveBlock1Ptr->wotPartners[i].pendingLevel = 0;
    }
    SetPartnerBits(1);
}

void WotTester_SetMableTally(u32 claimed)
{
    u32 i;

    for (i = 0; i < claimed && i < ARRAY_COUNT(sMegaRoster); i++)
        WotMegaDim_SetMegaClaimed(i, TRUE);
    SetFlagBits(FLAG_WOT_MABLE_TALLY_START, MABLE_TALLY_BITS, min(claimed, ARRAY_COUNT(sMegaRoster)));
}
#endif // WOT_TESTER
