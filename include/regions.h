#ifndef GUARD_REGIONS_H
#define GUARD_REGIONS_H

#include "global.h"
#include "constants/regions.h"

enum KantoSubRegion GetKantoSubregion(u32 mapSecId);

// WoT: two of its own places sit on map section ids borrowed from the Kanto
// range -- MAPSEC_ROUTE_2 is SENNEN (the beach, its lifeguard shack and the
// yacht) and MAPSEC_ROUTE_3 is TORII ROUTE. region_map_sections.json renames
// and re-positions both and they have cells on WoT's chart, but the engine
// decides the region from the id alone, so standing in either counted as
// being in Kanto: the region map opened the Kanto chart with Kanto's cursor
// grid, and battles there picked the FRLG themes. The FRLG maps that also use
// these two ids are unreachable and their encounter tables are compiled out.
static inline bool32 WotIsBorrowedKantoSectionId(u32 sectionId)
{
    return sectionId == MAPSEC_ROUTE_2 || sectionId == MAPSEC_ROUTE_3;
}

static inline enum Region GetRegionForSectionId(u32 sectionId)
{
    if (sectionId >= KANTO_MAPSEC_START && sectionId < MAPSEC_SPECIAL_AREA
     && !WotIsBorrowedKantoSectionId(sectionId))
        return REGION_KANTO;
    return REGION_HOENN;
}

static inline enum Region GetCurrentRegion(void)
{
    return GetRegionForSectionId(gMapHeader.regionMapSectionId);
}

#endif // GUARD_REGIONS_H
