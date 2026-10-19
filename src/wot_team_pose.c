#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "link.h"
#include "main.h"
#include "metatile_behavior.h"
#include "overworld.h"
#include "pokemon.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "task.h"
#include "wot_team_pose.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/region_map_sections.h"
#include "constants/songs.h"

// The team pose: press L in the field and the whole party pops out of their
// Poke Balls into a V behind the player -- the player at the point, two mons
// per row, each row one tile further back and one tile further out -- all
// facing the camera like a team photo. Any button sends them back.
//
// L starts EventScript_WotTeamPose (data/scripts/wot_team_pose.inc), which
// locks the field, asks WotTeamPose_Plan where the V fits, tucks the follower
// into its ball, then hands over to WotTeamPose_Run until the team is away.
//
// Each mon is an ordinary OW Pokemon object with a local id the maps never
// use (POSE_LOCALID_FIRST + slot). WotTeamPoseGetMon maps that id back to the
// party, which is how the object code gives each one its own ball and a
// Shadow mon its silhouette, exactly like the follower.

#define POSE_LOCALID_FIRST      224 // 224-229: maps stop far short, link players start at 236
#define POSE_POP_GAP            2   // frames after one ball finishes before the next opens
#define POSE_RETURN_STAGGER     6   // frames between mons going back
// OBJ VRAM (1024 tiles) left free after the team's sheets are in: room for a
// plain Poke Ball's sheet while a mon swaps to it, and field effects such as
// grass. Each mon's whole sheet is resident while it poses -- 96 tiles for
// most, 384 for a 64x64 legendary -- so two or three of those fill VRAM.
#define POSE_VRAM_HEADROOM      64
#define POSE_MIN_HOLD           30  // frames before a button can end the pose
#define POSE_TIMEOUT            600 // a stuck animation must never softlock the field
// Palette slots kept spare while the balls open and close: each mon briefly
// wears its ball's palette and the white flash, and the staggers keep at
// most two mons mid-animation at once.
#define POSE_PALETTE_HEADROOM   2

enum {
    SLOT_WAITING,   // spawned, invisible, ball not opened yet
    SLOT_POPPING,   // coming out of its ball
    SLOT_IDLE,      // posing, walking in place like the follower does
    SLOT_RETURNING, // going back into its ball
    SLOT_GONE,      // object removed
};

static EWRAM_DATA struct {
    bool8 active;
    u8 count;
    u8 facing;
    u8 partySlot[PARTY_SIZE];
    u8 objectId[PARTY_SIZE];
    u8 state[PARTY_SIZE];
} sPose = {0};

// The pose first faces wherever the player already faces, with the V behind
// them; if that ground is blocked, these are tried in order (up, down, then
// the sides), skipping the one already tried.
static const u8 sPoseFallbackFacings[] = {DIR_NORTH, DIR_SOUTH, DIR_WEST, DIR_EAST};

static void Task_WotTeamPose(u8 taskId);

struct Pokemon *WotTeamPoseGetMon(u8 localId)
{
    u32 slot = localId - POSE_LOCALID_FIRST;

    if (!sPose.active || localId < POSE_LOCALID_FIRST || slot >= sPose.count)
        return NULL;
    return &gPlayerParty[sPose.partySlot[slot]];
}

static bool32 IsPoseMon(struct Pokemon *mon)
{
    u32 species = GetMonData(mon, MON_DATA_SPECIES);

    return species != SPECIES_NONE && species < NUM_SPECIES && !GetMonData(mon, MON_DATA_IS_EGG);
}

// Any map, indoors included, so long as the V fits (WotTeamPose_Plan checks
// the ground). Only the cases that would break are left out.
static bool32 IsPoseAllowedHere(void)
{
    // Surfing or diving: the V would be water, and the mons can't stand on it.
    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER))
        return FALSE;
    // Walking the Distortion World's ceiling: the player is drawn upside
    // down and the mons wouldn't be.
    if (gMapHeader.regionMapSectionId == MAPSEC_DISTORTION_WORLD && FlagGet(FLAG_DW_INVERTED))
        return FALSE;
    // Link play and the Union Room run their own objects and field input.
    if (IsOverworldLinkActive() || InUnionRoom())
        return FALSE;
    return TRUE;
}

// Called for an L press in ProcessPlayerFieldInput. Somewhere the pose can
// never happen, L just does nothing, as it always has.
bool32 WotTryStartTeamPose(void)
{
    u32 i;

    // With L=A, L is the A button.
    if (gSaveBlock2Ptr->optionsButtonMode == OPTIONS_BUTTON_MODE_L_EQUALS_A)
        return FALSE;
    if (!IsPoseAllowedHere())
        return FALSE;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (IsPoseMon(&gPlayerParty[i]))
        {
            ScriptContext_SetupScript(EventScript_WotTeamPose);
            return TRUE;
        }
    }
    return FALSE;
}

// Slot n stands (n / 2 + 1) tiles behind the player and as many to one side,
// alternating left and right, so six mons make rows of two at 1, 2 and 3.
static void GetPoseSlotOffset(u32 slot, u32 facing, s16 *dx, s16 *dy)
{
    s16 back = slot / 2 + 1;
    s16 side = (slot & 1) ? back : -back;

    switch (facing)
    {
    case DIR_SOUTH:
    default:
        *dx = side;
        *dy = -back;
        break;
    case DIR_NORTH:
        *dx = side;
        *dy = back;
        break;
    case DIR_WEST:
        *dx = back;
        *dy = side;
        break;
    case DIR_EAST:
        *dx = -back;
        *dy = side;
        break;
    }
}

static bool32 IsPoseTileFree(s16 x, s16 y, u8 elevation)
{
    u32 i;

    if (MapGridGetCollisionAt(x, y))
        return FALSE;
    if (IsElevationMismatchAt(elevation, x, y))
        return FALSE;
    if (MetatileBehavior_IsSurfableWaterOrUnderwater(MapGridGetMetatileBehaviorAt(x, y)))
        return FALSE;
    // Previous coords too: lockall can freeze an NPC halfway between tiles.
    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        struct ObjectEvent *obj = &gObjectEvents[i];

        if (!obj->active)
            continue;
        if ((obj->currentCoords.x == x && obj->currentCoords.y == y)
         || (obj->previousCoords.x == x && obj->previousCoords.y == y))
            return FALSE;
    }
    return TRUE;
}

static bool32 DoesPoseFit(u32 facing, u32 count)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    u32 i;
    s16 dx, dy;

    for (i = 0; i < count; i++)
    {
        GetPoseSlotOffset(i, facing, &dx, &dy);
        if (!IsPoseTileFree(player->currentCoords.x + dx, player->currentCoords.y + dy, player->currentElevation))
            return FALSE;
    }
    return TRUE;
}

// special: picks the mons and the direction. VAR_RESULT = how many will pose,
// 0 when the V does not fit here in any direction.
void WotTeamPose_Plan(void)
{
    u32 i, j, count = 0, newPalettes = 0, freeObjects = 0;
    u32 freePalettes = CountFreeSpritePalettes();
    u16 newTags[PARTY_SIZE];

    sPose.active = FALSE;
    sPose.count = 0;
    gSpecialVar_Result = 0;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (!gObjectEvents[i].active)
            freeObjects++;
    }

    // Whole party in order, eggs aside, fainted mons included -- it's a photo.
    // A mon that would need a palette slot the field can't spare sits this one
    // out rather than draw in another mon's colours.
    for (i = 0; i < PARTY_SIZE && count < freeObjects; i++)
    {
        struct Pokemon *mon = &gPlayerParty[i];
        u16 tag;

        if (!IsPoseMon(mon))
            continue;
        tag = WotGetPartyMonObjectPaletteTag(mon);
        if (IndexOfSpritePaletteTag(tag) == 0xFF)
        {
            for (j = 0; j < newPalettes; j++)
            {
                if (newTags[j] == tag)
                    break;
            }
            if (j == newPalettes)
            {
                if (newPalettes + 1 + POSE_PALETTE_HEADROOM > freePalettes)
                    continue;
                newTags[newPalettes++] = tag;
            }
        }
        sPose.partySlot[count++] = i;
    }

    if (count == 0)
        return;
    for (i = 0; i <= ARRAY_COUNT(sPoseFallbackFacings); i++)
    {
        u32 facing = (i == 0) ? GetPlayerFacingDirection() : sPoseFallbackFacings[i - 1];

        if (i != 0 && facing == GetPlayerFacingDirection())
            continue;
        if (DoesPoseFit(facing, count))
        {
            sPose.facing = facing;
            sPose.count = count;
            gSpecialVar_Result = count;
            return;
        }
    }
}

static struct ObjectEvent *GetPoseObject(u32 slot)
{
    struct ObjectEvent *obj = &gObjectEvents[sPose.objectId[slot]];

    if (!obj->active || obj->localId != POSE_LOCALID_FIRST + slot)
        return NULL;
    return obj;
}

// special: spawns the team (invisible) on the planned tiles and hands over
// to the task, which re-enables the script once everyone is back. Run it
// after Plan returned nonzero (the special waits by itself).
void WotTeamPose_Run(void)
{
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
    u32 i, n = 0;
    s16 dx, dy;

    sPose.active = TRUE;
    ObjectEventTurn(player, sPose.facing);
    for (i = 0; i < sPose.count; i++)
    {
        struct ObjectEvent *obj;
        u16 graphicsId = WotGetPartyMonObjectGraphicsId(&gPlayerParty[sPose.partySlot[i]]);
        u8 objectId;

        // A mon whose sheet won't fit in OBJ VRAM sits this one out; the
        // ones after it may be smaller and still fit. Mons pack forward into
        // the V's first slots, all of which Plan already checked for room.
        if (!WotReserveObjectSheet(graphicsId, POSE_VRAM_HEADROOM))
            continue;
        sPose.partySlot[n] = sPose.partySlot[i]; // before the spawn: it reads this
        GetPoseSlotOffset(n, sPose.facing, &dx, &dy);
        objectId = SpawnSpecialObjectEventParameterized(graphicsId,
                                                        MOVEMENT_TYPE_NONE,
                                                        POSE_LOCALID_FIRST + n,
                                                        player->currentCoords.x + dx,
                                                        player->currentCoords.y + dy,
                                                        player->currentElevation);
        if (objectId >= OBJECT_EVENTS_COUNT)
        {
            WotReleaseObjectSheet(graphicsId);
            continue;
        }
        sPose.objectId[n] = objectId;
        sPose.state[n] = SLOT_WAITING;
        obj = &gObjectEvents[objectId];
        obj->invisible = TRUE;
        gSprites[obj->spriteId].invisible = TRUE;
        ObjectEventTurn(obj, sPose.facing);
        n++;
    }
    sPose.count = n;
    CreateTask(Task_WotTeamPose, 80);
}

#define tPhase      data[0]
#define tTimer      data[1]
#define tNext       data[2]
#define tFrames     data[3]

enum {
    PHASE_POP_OUT,
    PHASE_HOLD,
    PHASE_PUT_AWAY,
};

// Posing mons walk in place, as the follower does when it stands still.
static void UpdatePoseIdle(void)
{
    u32 i;

    for (i = 0; i < sPose.count; i++)
    {
        struct ObjectEvent *obj = GetPoseObject(i);
        u8 status;

        if (obj == NULL)
            continue;
        if (sPose.state[i] == SLOT_POPPING)
        {
            if (ObjectEventClearHeldMovementIfFinished(obj) != TRUE)
                continue;
            sPose.state[i] = SLOT_IDLE;
        }
        if (sPose.state[i] != SLOT_IDLE)
            continue;
        status = ObjectEventClearHeldMovementIfFinished(obj);
        if (status == TRUE || status == 16) // finished, or nothing held
            ObjectEventSetHeldMovement(obj, GetWalkInPlaceNormalMovementAction(sPose.facing));
    }
}

static bool32 IsPoseSlotDone(u32 slot, u32 doneState)
{
    return sPose.state[slot] >= doneState || GetPoseObject(slot) == NULL;
}

static bool32 IsAnyPoseSlotPopping(void)
{
    u32 i;

    for (i = 0; i < sPose.count; i++)
    {
        if (sPose.state[i] == SLOT_POPPING && GetPoseObject(i) != NULL)
            return TRUE;
    }
    return FALSE;
}

static void FinishPose(u8 taskId)
{
    u32 i;

    for (i = 0; i < sPose.count; i++)
    {
        struct ObjectEvent *obj = GetPoseObject(i);

        if (obj != NULL)
            RemoveObjectEvent(obj);
    }
    sPose.active = FALSE;
    sPose.count = 0;
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void Task_WotTeamPose(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    u32 i;

    if (sPose.count == 0 || ++tFrames > POSE_TIMEOUT)
    {
        FinishPose(taskId);
        return;
    }

    switch (tPhase)
    {
    case PHASE_POP_OUT:
        // One ball at a time. A mon coming out of a plain Poke Ball briefly
        // frees its sheet and loads it again; two doing that at once can
        // swap holes in VRAM and leave one with nowhere to go.
        UpdatePoseIdle();
        if (IsAnyPoseSlotPopping())
        {
            tTimer = POSE_POP_GAP;
        }
        else if (tTimer != 0)
        {
            tTimer--;
        }
        else if (tNext < sPose.count)
        {
            struct ObjectEvent *obj = GetPoseObject(tNext);

            if (obj != NULL)
            {
                PlaySE(SE_BALL_OPEN);
                if (!ObjectEventSetHeldMovement(obj, MOVEMENT_ACTION_EXIT_POKEBALL))
                {
                    sPose.state[tNext] = SLOT_POPPING;
                }
                else
                {
                    // Couldn't take the movement: skip the ball, just appear.
                    obj->invisible = FALSE;
                    sPose.state[tNext] = SLOT_IDLE;
                }
            }
            tNext++;
        }
        if (tNext < sPose.count)
            break;
        for (i = 0; i < sPose.count; i++)
        {
            if (!IsPoseSlotDone(i, SLOT_IDLE))
                break;
        }
        if (i == sPose.count)
        {
            // Everyone's out: the lead says cheese.
            PlayCry_Normal(GetMonData(&gPlayerParty[sPose.partySlot[0]], MON_DATA_SPECIES), 0);
            tPhase = PHASE_HOLD;
            tTimer = POSE_MIN_HOLD;
            tFrames = 0;
        }
        break;
    case PHASE_HOLD:
        // Held as long as the player likes: no timeout here.
        tFrames = 0;
        UpdatePoseIdle();
        if (tTimer != 0)
        {
            tTimer--;
        }
        else if (JOY_NEW(A_BUTTON | B_BUTTON | START_BUTTON | SELECT_BUTTON | L_BUTTON | R_BUTTON | DPAD_ANY))
        {
            tPhase = PHASE_PUT_AWAY;
            tNext = sPose.count; // back row first
            tTimer = 0;
        }
        break;
    case PHASE_PUT_AWAY:
        if (tTimer != 0)
        {
            tTimer--;
        }
        else if (tNext > 0)
        {
            struct ObjectEvent *obj;

            tNext--;
            obj = GetPoseObject(tNext);
            if (obj != NULL)
            {
                ObjectEventClearHeldMovement(obj);
                ObjectEventSetHeldMovement(obj, MOVEMENT_ACTION_ENTER_POKEBALL);
                sPose.state[tNext] = SLOT_RETURNING;
            }
            tTimer = POSE_RETURN_STAGGER;
        }
        UpdatePoseIdle(); // those still waiting their turn
        for (i = 0; i < sPose.count; i++)
        {
            struct ObjectEvent *obj = GetPoseObject(i);

            // Gone as soon as its ball closes, freeing its palette for the next.
            if (sPose.state[i] == SLOT_RETURNING && obj != NULL && ObjectEventCheckHeldMovementStatus(obj) == TRUE)
            {
                RemoveObjectEvent(obj);
                sPose.state[i] = SLOT_GONE;
            }
        }
        if (tNext > 0)
            break;
        for (i = 0; i < sPose.count; i++)
        {
            if (!IsPoseSlotDone(i, SLOT_GONE))
                break;
        }
        if (i == sPose.count)
            FinishPose(taskId);
        break;
    }
}

#undef tPhase
#undef tTimer
#undef tNext
#undef tFrames
