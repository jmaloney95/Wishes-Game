#include "global.h"
#include "quest_indicator.h"
#include "quests.h"
#include "sprite.h"
#include "decompress.h"
#include "event_data.h"
#include "event_object_movement.h"

// Persistent "!" indicator floating over quest-giver NPCs so the player can
// spot who has something to offer. Each indicator is a small OBJ sprite that
// tracks its NPC every frame (modeled on the shadow field effect,
// src/field_effect_helpers.c) and self-destroys once its quest is picked up
// (or its flag is set) or the NPC leaves after having appeared.
//
// It is spawned from a map's OnLoad/OnResume script, which runs BEFORE object
// events spawn (src/overworld.c ResumeMap). So the sprite is created up front
// and simply waits (invisible) until its NPC exists, rather than checking for
// the NPC at spawn time.

#define QI_TAG        0x4B30
#define QI_MAX        8
#define QI_BOB_PERIOD 64

#define QI_COND_QUEST 0
#define QI_COND_FLAG  1

#define sLocalId   data[0]
#define sMapNum    data[1]
#define sMapGroup  data[2]
#define sBobTimer  data[3]
#define sSeen      data[4]
#define sCondType  data[5]
#define sCondId    data[6]

static const u32 sQuestIndicatorGfx[] = INCGFX_U32("graphics/interface/quest_indicator.png", ".4bpp.smol");
static const u16 sQuestIndicatorPal[] = INCGFX_U16("graphics/interface/quest_indicator.png", ".gbapal");

static EWRAM_DATA u8 sIndicatorSpriteIds[QI_MAX] = {0};

static const struct OamData sOamData_QuestIndicator =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
    .priority = 1,
};

static const union AnimCmd sAnim_QuestIndicator[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_QuestIndicator[] =
{
    sAnim_QuestIndicator,
};

static void SpriteCB_QuestIndicator(struct Sprite *sprite);

static const struct CompressedSpriteSheet sSpriteSheet_QuestIndicator =
{
    sQuestIndicatorGfx, 16 * 16 / 2, QI_TAG
};

static const struct SpritePalette sSpritePalette_QuestIndicator =
{
    sQuestIndicatorPal, QI_TAG
};

static const struct SpriteTemplate sSpriteTemplate_QuestIndicator =
{
    .tileTag = QI_TAG,
    .paletteTag = QI_TAG,
    .oam = &sOamData_QuestIndicator,
    .anims = sAnims_QuestIndicator,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCB_QuestIndicator,
};

// The condition is satisfied (indicator should keep showing) while the quest
// is still unstarted, or the gating flag is still clear.
static bool32 ConditionStillActive(struct Sprite *sprite)
{
    if (sprite->sCondType == QI_COND_FLAG)
        return !FlagGet(sprite->sCondId);
    return !QuestMenu_GetSetQuestState(sprite->sCondId, FLAG_GET_UNLOCKED);
}

static void SpriteCB_QuestIndicator(struct Sprite *sprite)
{
    u8 objectEventId;
    s16 bob;

    // Picked up the quest / earned the reward: retire the indicator.
    if (!ConditionStillActive(sprite))
    {
        DestroySprite(sprite);
        return;
    }

    if (TryGetObjectEventIdByLocalIdAndMap(sprite->sLocalId, sprite->sMapNum, sprite->sMapGroup, &objectEventId))
    {
        // NPC not currently spawned. This is USUALLY just the camera window --
        // RemoveObjectEventsOutsideView despawns off-screen NPCs -- so never
        // destroy here (that permanently killed the "!" the moment the quest
        // giver scrolled off-screen); wait invisibly and re-attach on respawn.
        // Genuine retirement is handled by ConditionStillActive above and by
        // QuestIndicator_Reset on map load.
        sprite->invisible = TRUE;
        return;
    }

    {
        struct Sprite *npc = &gSprites[gObjectEvents[objectEventId].spriteId];

        sprite->sSeen = TRUE;
        sprite->sBobTimer = (sprite->sBobTimer + 1) % QI_BOB_PERIOD;
        bob = (sprite->sBobTimer < QI_BOB_PERIOD / 2) ? 0 : -2;
        sprite->x = npc->x;
        sprite->y = npc->y + npc->centerToCornerVecY - 6 + bob;
        sprite->x2 = npc->x2;
        sprite->y2 = npc->y2;
        sprite->invisible = npc->invisible;
    }
}

static void SpawnIndicator(u8 localId, u8 condType, u16 condId)
{
    u8 spriteId;
    u32 i;

    if (!USE_QUEST_GIVER_INDICATOR)
        return;

    for (i = 0; i < QI_MAX; i++)
    {
        // Already tracking this NPC on this map - don't stack a second one.
        if (sIndicatorSpriteIds[i] != SPRITE_NONE
            && gSprites[sIndicatorSpriteIds[i]].inUse
            && gSprites[sIndicatorSpriteIds[i]].callback == SpriteCB_QuestIndicator
            && gSprites[sIndicatorSpriteIds[i]].sLocalId == localId)
            return;
    }

    for (i = 0; i < QI_MAX; i++)
    {
        if (sIndicatorSpriteIds[i] != SPRITE_NONE)
            continue;

        if (GetSpriteTileStartByTag(QI_TAG) == 0xFFFF)
            LoadCompressedSpriteSheet(&sSpriteSheet_QuestIndicator);
        LoadSpritePalette(&sSpritePalette_QuestIndicator);

        spriteId = CreateSprite(&sSpriteTemplate_QuestIndicator, 0, 0, 0);
        if (spriteId == MAX_SPRITES)
            return;

        gSprites[spriteId].sLocalId = localId;
        gSprites[spriteId].sMapNum = gSaveBlock1Ptr->location.mapNum;
        gSprites[spriteId].sMapGroup = gSaveBlock1Ptr->location.mapGroup;
        gSprites[spriteId].sCondType = condType;
        gSprites[spriteId].sCondId = condId;
        gSprites[spriteId].sSeen = FALSE;
        gSprites[spriteId].invisible = TRUE;
        gSprites[spriteId].coordOffsetEnabled = TRUE;
        sIndicatorSpriteIds[i] = spriteId;
        return;
    }
}

void QuestIndicator_TryShow(u8 localId, u8 questId)
{
    if (!QuestMenu_GetSetQuestState(questId, FLAG_GET_UNLOCKED))
        SpawnIndicator(localId, QI_COND_QUEST, questId);
}

void QuestIndicator_TryShowFlag(u8 localId, u16 flag)
{
    if (!FlagGet(flag))
        SpawnIndicator(localId, QI_COND_FLAG, flag);
}

void QuestIndicator_ClearAll(void)
{
    u32 i;

    for (i = 0; i < QI_MAX; i++)
    {
        if (sIndicatorSpriteIds[i] != SPRITE_NONE)
        {
            DestroySprite(&gSprites[sIndicatorSpriteIds[i]]);
            sIndicatorSpriteIds[i] = SPRITE_NONE;
        }
    }
    FreeSpriteTilesByTag(QI_TAG);
    FreeSpritePaletteByTag(QI_TAG);
}

// Field reload wipes every sprite; just drop the bookkeeping so the next
// map's OnLoad can re-add indicators cleanly.
void QuestIndicator_Reset(void)
{
    u32 i;

    for (i = 0; i < QI_MAX; i++)
        sIndicatorSpriteIds[i] = SPRITE_NONE;
}
