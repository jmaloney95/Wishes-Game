#ifndef GUARD_REGION_MAP_SECTIONS_H
#define GUARD_REGION_MAP_SECTIONS_H

// ============================================================================
//  CUSTOM REGION MAP SECTIONS
// ----------------------------------------------------------------------------
//  These are the MAPSEC_* IDs for your custom island region. Add these entries
//  into your project's existing include/constants/region_map_sections.h,
//  renumbering so the values stay sequential with your other map sections.
//
//  IMPORTANT: MAPSEC_NONE must remain the LAST/highest value (in stock
//  pokeemerald it is 0xD5 / the final entry). Insert these BEFORE it and
//  bump the count accordingly. The values below assume an otherwise-empty
//  list -- adjust to fit your project.
// ============================================================================

#define MAPSEC_NORTHWOOD_TOWN     0x00
#define MAPSEC_MT_EMBER           0x01
#define MAPSEC_GLACIER_LAKE       0x02
#define MAPSEC_CROSSROADS         0x03
#define MAPSEC_SAKURA_GROVE       0x04
#define MAPSEC_EVERGREEN_FOREST   0x05
#define MAPSEC_TECH_CITY          0x06
#define MAPSEC_SNOWFIELD          0x07

#define MAPSEC_NONE               0x08   // must stay the final value
#define MAPSECS_COUNT             (MAPSEC_NONE + 1)

#endif // GUARD_REGION_MAP_SECTIONS_H
