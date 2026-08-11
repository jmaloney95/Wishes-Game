#include "global.h"
#include "decompress.h"
#include "graphics.h"
#include "palette.h"
#include "random.h"
#include "sprite.h"
#include "task.h"
#include "trig.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "overworld.h"
#include "wot_confetti.h"

// Field confetti for the endgame celebration on TowerTop: the Hall of Fame's
// confetti sheet (17 coloured 8x8 chips) rained over the overworld instead of
// over a HoF scene. Start it with `special WotStartFieldConfetti`, and free it
// with `special WotStopFieldConfetti` once the screen is already black -- the
// stop is invisible then, and nothing leaks into the credits handoff.

#define TAG_WOT_CONFETTI    0x4B40
#define WOT_CONFETTI_TILES  17   // one 8x8 tile per colour/shape in the sheet
#define WOT_CONFETTI_PER_SPAWN 1
#define WOT_CONFETTI_SPAWN_DELAY 4
// HARD CAP. The field's own object events (player, NPCs, cutscene actors) live
// in the same 64-sprite pool: an uncapped spawner starved them and took the
// endgame down. 18 chips still reads as a downpour.
#define WOT_CONFETTI_MAX_LIVE 18

static u8 sLiveConfetti;

#define sSineIdx data[0]
#define sExtraY  data[1]

static const struct CompressedSpriteSheet sSpriteSheet_WotConfetti[] =
{
    {.data = gConfetti_Gfx, .size = 0x220, .tag = TAG_WOT_CONFETTI},
    {},
};

static const struct SpritePalette sSpritePalette_WotConfetti[] =
{
    {.data = gConfetti_Pal, .tag = TAG_WOT_CONFETTI},
    {},
};

static const struct OamData sOamData_WotConfetti =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 0, // over every BG layer, including the map's top layer
};

static void SpriteCB_WotConfetti(struct Sprite *sprite);

static const struct SpriteTemplate sSpriteTemplate_WotConfetti =
{
    .tileTag = TAG_WOT_CONFETTI,
    .paletteTag = TAG_WOT_CONFETTI,
    .oam = &sOamData_WotConfetti,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_WotConfetti,
};

// Falls, swaying on a sine; dies once it has drifted off the bottom.
static void SpriteCB_WotConfetti(struct Sprite *sprite)
{
    if (sprite->y + sprite->y2 > DISPLAY_HEIGHT + 8)
    {
        if (sLiveConfetti != 0)
            sLiveConfetti--;
        DestroySprite(sprite);
        return;
    }
    sprite->y2 += 1 + sprite->sExtraY;
    sprite->x2 = ((Random() % 4) + 8) * gSineTable[(u8)sprite->sSineIdx] / 256;
    sprite->sSineIdx += 4;
}

static void SpawnOneConfetti(void)
{
    u8 spriteId;

    if (sLiveConfetti >= WOT_CONFETTI_MAX_LIVE)
        return;
    spriteId = CreateSprite(&sSpriteTemplate_WotConfetti,
                            Random() % DISPLAY_WIDTH, -(Random() % 16), 0);
    if (spriteId == MAX_SPRITES)
        return;
    sLiveConfetti++;
    // No anim table needed: each colour is just the next tile in the sheet.
    gSprites[spriteId].oam.tileNum += Random() % WOT_CONFETTI_TILES;
    gSprites[spriteId].sExtraY = (Random() & 3) ? 0 : 1;
}

static void Task_WotConfettiSpawner(u8 taskId)
{
    u32 i;

    if (++gTasks[taskId].data[0] < WOT_CONFETTI_SPAWN_DELAY)
        return;
    gTasks[taskId].data[0] = 0;
    for (i = 0; i < WOT_CONFETTI_PER_SPAWN; i++)
        SpawnOneConfetti();
}

void WotStartFieldConfetti(void)
{
    if (FuncIsActiveTask(Task_WotConfettiSpawner))
        return;
    sLiveConfetti = 0;
    LoadCompressedSpriteSheet(sSpriteSheet_WotConfetti);
    LoadSpritePalettes(sSpritePalette_WotConfetti);
    // Out of OBJ palettes (heavy field scene): skip rather than draw garbage.
    if (IndexOfSpritePaletteTag(TAG_WOT_CONFETTI) == 0xFF)
    {
        FreeSpriteTilesByTag(TAG_WOT_CONFETTI);
        return;
    }
    CreateTask(Task_WotConfettiSpawner, 80);
}

void WotStopFieldConfetti(void)
{
    u32 i;

    if (FuncIsActiveTask(Task_WotConfettiSpawner))
        DestroyTask(FindTaskIdByFunc(Task_WotConfettiSpawner));
    for (i = 0; i < MAX_SPRITES; i++)
    {
        if (gSprites[i].inUse && gSprites[i].callback == SpriteCB_WotConfetti)
            DestroySprite(&gSprites[i]);
    }
    FreeSpriteTilesByTag(TAG_WOT_CONFETTI);
    FreeSpritePaletteByTag(TAG_WOT_CONFETTI);
    sLiveConfetti = 0;
}

// --- Idle bob for a field object (Shadow Jirachi floating in the machine) ---
// VAR_0x8004 = local id. Self-destructs once that object is gone, and dies
// naturally on any map change with the rest of the task pool.
#define tLocalId data[0]
#define tSine    data[1]

static void Task_WotObjectBob(u8 taskId)
{
    u8 objectEventId;

    if (!TryGetObjectEventIdByLocalIdAndMap(gTasks[taskId].tLocalId,
                                            gSaveBlock1Ptr->location.mapNum,
                                            gSaveBlock1Ptr->location.mapGroup,
                                            &objectEventId))
    {
        DestroyTask(taskId);
        return;
    }
    // +/-3px float, one cycle every ~4 seconds.
    gSprites[gObjectEvents[objectEventId].spriteId].y2 =
        gSineTable[(u8)gTasks[taskId].tSine] / 80;
    gTasks[taskId].tSine += 2;
}

void WotStartObjectBob(void)
{
    u8 taskId;

    if (FuncIsActiveTask(Task_WotObjectBob))
        return;
    taskId = CreateTask(Task_WotObjectBob, 90);
    gTasks[taskId].tLocalId = gSpecialVar_0x8004;
    gTasks[taskId].tSine = 0;
}

#undef tLocalId
#undef tSine
