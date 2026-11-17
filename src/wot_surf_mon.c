#include "global.h"
#include "event_object_movement.h"
#include "field_effect.h"
#include "field_effect_helpers.h"
#include "field_player_avatar.h"
#include "field_weather.h"
#include "palette.h"
#include "pokemon.h"
#include "sprite.h"
#include "wot_permanent_mega.h"
#include "wot_surf_mon.h"
#include "constants/event_objects.h"
#include "constants/weather.h"
#include "constants/species.h"

// ============================================================================
//  WISHES OF TOMORROW -- SURF POKEMON
//
//  While surfing, the player rides a party Pokemon instead of the surf blob:
//  the lead if it has a surf sprite, else the first healthy party Pokemon
//  that does. With none, it's the blob as before. Nothing is saved: the
//  choice is made again each time the sprite is created (every warp, battle
//  and menu), so a new lead shows the next time the field reloads.
//
//  Two sprites. The base sits under the player and is the surf blob in every
//  other way (its callback, bobbing and data are the blob's, so Fly, fishing
//  and dismounting need nothing new). The overlay is the part drawn over the
//  player (a head or an arm), and copies the base every frame.
//
//  Sheets: graphics/object_events/pics/pokemon/surf/, listed in src/data/wot_surf_mons.h.
// ============================================================================

struct WotSurfMon
{
    u16 species;
    bool8 big;          // 64x64 frames
    bool8 asymmetric;   // frames 6-7 face east; otherwise east mirrors west
    const struct SpriteFrameImage *baseFrames;
    const struct SpriteFrameImage *overFrames;
    const u16 *pal;     // NULL: the sheet uses the species' follower palette
    const u16 *shinyPal;
};

#include "data/wot_surf_mons.h"

// A sheet keeping its own palette: tag + row * 4 (+1 shiny, +2 Shadow).
// Free: followers use 0x4000-0x75DC, Shadow silhouettes 0x9000-0xAEDC.
#define PAL_TAG_SURF_MON_OWN 0xB000

// The sheets' frames: north 0-1, south 2-3, west 4-5, east 6-7 (or west mirrored).
// Anim numbers follow the surf blob's (SynchronizeSurfAnim): S, N, W, E.
static const union AnimCmd sAnim_SurfMonSouth[] = {ANIMCMD_FRAME(2, 16), ANIMCMD_FRAME(3, 16), ANIMCMD_JUMP(0)};
static const union AnimCmd sAnim_SurfMonNorth[] = {ANIMCMD_FRAME(0, 16), ANIMCMD_FRAME(1, 16), ANIMCMD_JUMP(0)};
static const union AnimCmd sAnim_SurfMonWest[]  = {ANIMCMD_FRAME(4, 16), ANIMCMD_FRAME(5, 16), ANIMCMD_JUMP(0)};
static const union AnimCmd sAnim_SurfMonEast[]  = {ANIMCMD_FRAME(4, 16, .hFlip = TRUE), ANIMCMD_FRAME(5, 16, .hFlip = TRUE), ANIMCMD_JUMP(0)};
static const union AnimCmd sAnim_SurfMonEastAsym[] = {ANIMCMD_FRAME(6, 16), ANIMCMD_FRAME(7, 16), ANIMCMD_JUMP(0)};

static const union AnimCmd *const sAnims_SurfMon[] = {sAnim_SurfMonSouth, sAnim_SurfMonNorth, sAnim_SurfMonWest, sAnim_SurfMonEast};
static const union AnimCmd *const sAnims_SurfMonAsym[] = {sAnim_SurfMonSouth, sAnim_SurfMonNorth, sAnim_SurfMonWest, sAnim_SurfMonEastAsym};

// The live sprites keep pointing at their templates, so these can't be locals.
static EWRAM_DATA struct SpriteTemplate sBaseTemplate = {0};
static EWRAM_DATA struct SpriteTemplate sOverTemplate = {0};

// Overlay sprite data
#define sBaseId     data[0]
#define sRow        data[1]
#define sFrame      data[2]
#define sPaletteNum data[3]

static s32 FindRow(u16 species)
{
    u32 i;

    if (species >= NUM_SPECIES)
        return -1;
    if (gSpeciesInfo[species].isMegaEvolution && WotGetMegaBaseSpecies(species) != SPECIES_NONE)
        species = WotGetMegaBaseSpecies(species);
    for (i = 0; i < ARRAY_COUNT(sWotSurfMons); i++)
    {
        if (sWotSurfMons[i].species == species)
            return i;
    }
    return -1;
}

bool32 WotHasSurfSprite(u16 species)
{
    return FindRow(species) >= 0;
}

static bool32 CanCarry(struct Pokemon *mon)
{
    return mon != NULL
        && GetMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE
        && !GetMonData(mon, MON_DATA_IS_EGG)
        && GetMonData(mon, MON_DATA_HP) != 0
        && FindRow(GetMonData(mon, MON_DATA_SPECIES)) >= 0;
}

// The Pokemon the player rides, or NULL for the surf blob.
struct Pokemon *WotGetSurfMon(void)
{
    struct Pokemon *lead = GetFirstLiveMon();
    u32 i;

    if (CanCarry(lead))
        return lead;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (CanCarry(&gPlayerParty[i]))
            return &gPlayerParty[i];
    }
    return NULL;
}

static u32 LoadSurfMonPalette(struct Pokemon *mon, u32 row)
{
    const struct WotSurfMon *entry = &sWotSurfMons[row];
    bool32 shiny = GetMonData(mon, MON_DATA_IS_SHINY);
    bool32 shadow = GetMonData(mon, MON_DATA_IS_SHADOW);
    u16 tag;
    u32 paletteNum;
    u16 silhouette[16];
    struct SpritePalette spritePalette;

    if (entry->pal == NULL)
        return WotLoadMonOverworldPalette(entry->species, shiny, GetMonGender(mon) == MON_FEMALE, shadow);

    tag = PAL_TAG_SURF_MON_OWN + row * 4 + (shiny ? 1 : 0) + (shadow ? 2 : 0);
    if ((paletteNum = IndexOfSpritePaletteTag(tag)) < 16)
        return paletteNum;
    spritePalette.tag = tag;
    spritePalette.data = shiny ? entry->shinyPal : entry->pal;
    if (shadow)
    {
        WotBuildShadowPalette(entry->pal, silhouette, shiny);
        spritePalette.data = silhouette;
    }
    paletteNum = LoadSpritePalette(&spritePalette);
    if (paletteNum < 16 && gWeatherPtr->currWeather != WEATHER_FOG_HORIZONTAL)
        UpdateSpritePaletteWithWeather(paletteNum, FALSE);
    return paletteNum;
}

// The overlay follows the base: its place, bob, frame, flip and visibility.
// It shows only while the player sits on the Pokemon, not while they jump
// on or off, and goes when the base does.
static void SpriteCB_SurfMonOverlay(struct Sprite *sprite)
{
    struct Sprite *base = &gSprites[sprite->sBaseId];
    struct Sprite *player = &gSprites[gObjectEvents[gPlayerAvatar.objectEventId].spriteId];
    s16 frame;

    if (!base->inUse || base->images != sWotSurfMons[sprite->sRow].baseFrames)
    {
        u8 paletteNum = sprite->sPaletteNum;

        DestroySprite(sprite);
        FieldEffectFreePaletteIfUnused(paletteNum);
        return;
    }
    sprite->x = base->x;
    sprite->y = base->y;
    sprite->x2 = base->x2;
    sprite->y2 = base->y2;
    sprite->oam.priority = base->oam.priority;
    sprite->oam.matrixNum = base->oam.matrixNum; // the flip bits: neither sprite is affine
    sprite->subpriority = player->subpriority > 0 ? player->subpriority - 1 : 0;
    sprite->invisible = base->invisible || GetSurfBlobBobState(sprite->sBaseId) != BOB_PLAYER_AND_MON;

    frame = base->anims[base->animNum][base->animCmdIndex].frame.imageValue;
    if (frame >= 0 && frame != sprite->sFrame)
    {
        RequestSpriteFrameImageCopy(frame, sprite->oam.tileNum, sprite->images);
        sprite->sFrame = frame;
    }
}

// The sheets were drawn for a rider who sits 2 px taller than ours, so the
// Pokemon goes a little lower and the player's head clears its head facing south.
#define SURF_MON_Y_OFFSET 3

// field_effect_helpers.c UpdateBobbingEffect: how much lower than the blob
// this sprite sits under the player.
s16 WotSurfMonSpriteYOffset(struct Sprite *sprite)
{
    return sprite->template == &sBaseTemplate ? SURF_MON_Y_OFFSET : 0;
}

// FldEff_SurfBlob, for the player: the base sprite, ready for the blob's own
// setup, or MAX_SPRITES to draw the blob instead.
u8 WotCreateSurfMonSprite(s16 x, s16 y)
{
    struct Pokemon *mon = WotGetSurfMon();
    const struct WotSurfMon *entry;
    s32 row;
    u32 paletteNum;
    u8 baseId, overId;

    if (mon == NULL)
        return MAX_SPRITES;
    row = FindRow(GetMonData(mon, MON_DATA_SPECIES));
    entry = &sWotSurfMons[row];
    paletteNum = LoadSurfMonPalette(mon, row);
    if (paletteNum >= 16)
        return MAX_SPRITES;

    sBaseTemplate = (struct SpriteTemplate) {
        .tileTag = TAG_NONE,
        .paletteTag = TAG_NONE,
        .oam = entry->big ? &gObjectEventBaseOam_64x64 : &gObjectEventBaseOam_32x32,
        .anims = entry->asymmetric ? sAnims_SurfMonAsym : sAnims_SurfMon,
        .images = entry->baseFrames,
        .affineAnims = gDummySpriteAffineAnimTable,
        .callback = UpdateSurfBlobFieldEffect,
    };
    baseId = CreateSpriteAtEnd(&sBaseTemplate, x, y, 150);
    if (baseId == MAX_SPRITES)
        return MAX_SPRITES;
    gSprites[baseId].oam.paletteNum = paletteNum;

    sOverTemplate = sBaseTemplate;
    sOverTemplate.anims = gDummySpriteAnimTable;
    sOverTemplate.images = entry->overFrames;
    sOverTemplate.callback = SpriteCB_SurfMonOverlay;
    overId = CreateSpriteAtEnd(&sOverTemplate, x, y, 0);
    if (overId != MAX_SPRITES)
    {
        struct Sprite *over = &gSprites[overId];

        over->coordOffsetEnabled = TRUE;
        over->oam.paletteNum = paletteNum;
        over->invisible = TRUE;
        over->sBaseId = baseId;
        over->sRow = row;
        over->sFrame = -1;
        over->sPaletteNum = paletteNum;
    }
    return baseId;
}
