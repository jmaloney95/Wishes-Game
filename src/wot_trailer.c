#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_screen_effect.h"
#include "map_name_popup.h"
#include "overworld.h"
#include "palette.h"
#include "script.h"
#include "task.h"
#include "wot_trailer.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/field_effects.h"
#include "constants/maps.h"
#include "constants/trainer_types.h"

// Wishes of Tomorrow: scenes replayed for trailer capture. Debug menu only
// (R + START > Scripts). Release builds have no debug menu and the testers'
// build leaves these rows out; every entry point here also checks
// WOT_TRAILER_CAPTURE (wot_trailer.h), so players can never reach it.
//
// The National Park guide's camera tour, as if it happened in the Mega
// Dimension: the map in black and white, but every NPC, every Pokemon, the
// player and the message box in colour. The real dimension greys sprites too
// (field_weather.c) and is a different map; this greys only the tileset
// palettes (fieldmap.c's grayscale hook) and the field-effect sprites that
// belong to the ground (grass rustles, sand prints, ripples). The switch is
// RAM only: no flag, no var, nothing in the save, and a power cycle clears it.
//
// "Trailer: grey park" warps to the guide, greys the park as it loads and runs
// the tour (data/scripts/wot_trailer.inc). At the end the park reloads in
// colour where it started.
//
// "Trailer: park portal" is the same tour with the park empty: no guide, no
// swimmers, no Pokemon (map objects and the follower are kept from spawning,
// again RAM only), and a Mega Dimension portal on the shore where Marill
// wanders, so it swirls past mid-screen during the southward pan.

// Where the player stood beside the guide (29,66) in the recorded tour.
#define PARK_PLAYER_X 28
#define PARK_PLAYER_Y 66
// Marill's spot on the shore; the camera pans south down x50 past it.
#define PARK_PORTAL_X 51
#define PARK_PORTAL_Y 59

extern const u8 Route_2_2_EventScript_ParkGuide[];
extern const u8 WotTrailer_EventScript_ParkTour[];
extern const u8 WotTrailer_EventScript_ParkTourText[];
extern const u8 WotTrailer_EventScript_ParkTourEmpty[];

static EWRAM_DATA bool8 sGrayMap = FALSE;
static EWRAM_DATA bool8 sEmptyPark = FALSE;
static EWRAM_DATA const u8 *sTourScript = NULL;

bool32 WotTrailer_IsGrayMap(void)
{
    return WOT_TRAILER_CAPTURE && sGrayMap;
}

bool32 WotTrailer_IsEmptyPark(void)
{
    return WotTrailer_IsGrayMap() && sEmptyPark;
}

// Stands in for TrySpawnObjectEvents' spawn while the park is empty: every map
// object stays unspawned except the one at the portal spot, which comes up as
// a still portal instead. Its local id is that object's, so the usual
// off-screen despawn and respawn work as they do for it. Nothing is written to
// the save's templates.
void WotTrailer_SpawnParkObject(const struct ObjectEventTemplate *template, s16 cameraX, s16 cameraY)
{
    struct ObjectEventTemplate portal;

    if (template->x != PARK_PORTAL_X || template->y != PARK_PORTAL_Y)
        return;
    portal = *template;
    portal.graphicsId = OBJ_EVENT_GFX_WOT_MEGA_PORTAL;
    portal.movementType = MOVEMENT_TYPE_NONE;
    portal.movementRangeX = 0;
    portal.movementRangeY = 0;
    portal.trainerType = TRAINER_TYPE_NONE;
    portal.script = NULL;
    TrySpawnObjectEventTemplate(&portal, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, cameraX, cameraY);
}

// Field-effect palettes (FLDEFF_PAL_TAG_*) sit below the object-event tags.
// Object events (NPCs, Pokemon, the player) and blend-immune tags stay colour.
bool32 WotTrailer_IsGraySpriteTag(u16 tag)
{
    return WotTrailer_IsGrayMap()
        && tag >= FLDEFF_PAL_TAG_CUT_GRASS && tag < OBJ_EVENT_PAL_TAG_BRENDAN;
}

static void WarpToParkGuide(void)
{
    SetWarpDestination(MAP_GROUP(MAP_ROUTE_2_2), MAP_NUM(MAP_ROUTE_2_2), WARP_ID_NONE, PARK_PLAYER_X, PARK_PLAYER_Y);
    DoDiveWarp(); // the quiet warp (warpsilent): no door sound
    ResetInitialPlayerAvatarState();
}

// Once the warp has faded in and handed back control, start the tour as if the
// player had just talked to the guide (when she is there).
static void Task_StartParkTour(u8 taskId)
{
    u32 i;

    if (gPaletteFade.active || ArePlayerFieldControlsLocked() || ScriptContext_IsEnabled())
        return;
    DestroyTask(taskId);
    gSpecialVar_LastTalked = LOCALID_NONE;
    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (gObjectEvents[i].active
         && GetObjectEventScriptPointerByObjectEventId(i) == Route_2_2_EventScript_ParkGuide)
        {
            gSelectedObjectEvent = i;
            gSpecialVar_LastTalked = gObjectEvents[i].localId;
            break;
        }
    }
    if (gSpecialVar_LastTalked != LOCALID_NONE || sEmptyPark)
        ScriptContext_SetupScript(sTourScript);
}

static void FieldCB_ArriveForParkTour(void)
{
    FieldCB_DefaultWarpExit();
    HideMapNamePopUpWindow(); // no "NATIONAL PARK" banner in the opening shot
    CreateTask(Task_StartParkTour, 80);
}

// Debug special. VAR_0x8004 = WOT_TRAILER_PARK_* (wot_trailer.h).
// VAR_RESULT = whether the warp started (the script waits on it only then).
void WotTrailer_StartParkTour(void)
{
    gSpecialVar_Result = WOT_TRAILER_CAPTURE;
    if (!WOT_TRAILER_CAPTURE)
        return;
    sGrayMap = TRUE;
    sEmptyPark = (gSpecialVar_0x8004 == WOT_TRAILER_PARK_PORTAL);
    if (gSpecialVar_0x8004 == WOT_TRAILER_PARK_TEXT)
        sTourScript = WotTrailer_EventScript_ParkTourText;
    else if (sEmptyPark)
        sTourScript = WotTrailer_EventScript_ParkTourEmpty;
    else
        sTourScript = WotTrailer_EventScript_ParkTour;
    WarpToParkGuide();
    gFieldCallback = FieldCB_ArriveForParkTour;
}

// Debug special: the park back in colour and full, reloaded where the tour
// started.
void WotTrailer_EndParkTour(void)
{
    sGrayMap = FALSE;
    sEmptyPark = FALSE;
    WarpToParkGuide();
}
