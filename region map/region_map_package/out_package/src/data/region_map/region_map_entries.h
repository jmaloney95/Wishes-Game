// ============================================================================
//  CUSTOM REGION MAP ENTRIES
// ----------------------------------------------------------------------------
//  Drop these into src/data/region_map/region_map_entries.h
//
//  Each entry:  [MAPSEC] = {x, y, width, height, name}
//    x, y, width, height are in 8x8 TILE units on the 28x20-tile region map.
//    The cursor highlights this rectangle and shows `name` when hovered.
//
//  Coordinates were derived from your island layout. Tweak any rectangle to
//  taste in Porymap's Region Map Editor (Tools > Region Map Editor) -- it
//  reads this same array and lets you nudge boxes visually.
// ============================================================================

static const struct RegionMapLocation sRegionMapEntries[] =
{
    [MAPSEC_NORTHWOOD_TOWN]   = { 8,  2, 6, 3, gText_Northwood   },
    [MAPSEC_MT_EMBER]         = {14,  5, 6, 4, gText_MtEmber     },
    [MAPSEC_GLACIER_LAKE]     = { 3,  8, 5, 4, gText_GlacierLake },
    [MAPSEC_CROSSROADS]       = { 9,  9, 3, 2, gText_Crossroads  },
    [MAPSEC_SAKURA_GROVE]     = { 2, 12, 6, 4, gText_SakuraGrove },
    [MAPSEC_EVERGREEN_FOREST] = {13, 11, 8, 5, gText_Evergreen   },
    [MAPSEC_TECH_CITY]        = {23,  6, 4, 4, gText_TechCity    },
    [MAPSEC_SNOWFIELD]        = { 8, 15, 5, 4, gText_Snowfield   },
};
