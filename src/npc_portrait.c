#include "global.h"
#include "npc_portrait.h"
#include "sprite.h"
#include "main.h"

// 64x64 character portraits shown beside the message box during important
// NPC dialogue (visual-novel style). One hardware sprite, one 16-color OBJ
// palette, shown/hidden by the showportrait/hideportrait script commands.
// The sprite persists across message boxes until hidden; it is defensively
// cleared when a script ends (src/script.c) and dies naturally on warp
// (map load resets all sprites - state here self-validates against that).

#define PORTRAIT_PAL_TAG   0x4B10
#define PORTRAIT_SIZE_4BPP 0x800 // 64x64 4bpp = 64 tiles

#define PORTRAIT_X_LEFT    40
#define PORTRAIT_X_RIGHT   200
#define PORTRAIT_Y         72  // sprite center; bottom edge sits flush on the namebox plate (y=104)

struct NpcPortrait
{
    const u8 *tiles;    // raw 4bpp, 64x64
    const u16 *palette; // 16 colors
};

static const u8 sPortraitPlaceholderGfx[] = INCGFX_U8("graphics/portraits/placeholder.png", ".4bpp");
static const u16 sPortraitPlaceholderPal[] = INCGFX_U16("graphics/portraits/placeholder.png", ".gbapal");
static const u8 sPortraitPlaceholder2Gfx[] = INCGFX_U8("graphics/portraits/placeholder2.png", ".4bpp");
static const u16 sPortraitPlaceholder2Pal[] = INCGFX_U16("graphics/portraits/placeholder2.png", ".gbapal");
static const u8 sPortraitClarksonGfx[] = INCGFX_U8("graphics/portraits/clarkson.png", ".4bpp");
static const u16 sPortraitClarksonPal[] = INCGFX_U16("graphics/portraits/clarkson.png", ".gbapal");

// Real art later = add the PNG, repoint that id's row. Nothing else changes.
// PORTRAIT_COBRA intentionally points at the second placeholder to prove the
// drop-in path works (see the UI features handoff acceptance criteria).
static const struct NpcPortrait sNpcPortraits[PORTRAIT_COUNT] =
{
    [PORTRAIT_PLACEHOLDER]     = {sPortraitPlaceholderGfx,  sPortraitPlaceholderPal},
    [PORTRAIT_CLARKSON_GENGAR] = {sPortraitClarksonGfx,     sPortraitClarksonPal},
    [PORTRAIT_MADAM_TSUJI]     = {sPortraitPlaceholderGfx,  sPortraitPlaceholderPal},
    [PORTRAIT_MUTRID_LEADER]   = {sPortraitPlaceholderGfx,  sPortraitPlaceholderPal},
    [PORTRAIT_RED_FATALITY]    = {sPortraitPlaceholderGfx,  sPortraitPlaceholderPal},
    [PORTRAIT_DRACO]           = {sPortraitPlaceholderGfx,  sPortraitPlaceholderPal},
    [PORTRAIT_COBRA]           = {sPortraitPlaceholder2Gfx, sPortraitPlaceholder2Pal},
};

static EWRAM_INIT u8 sPortraitSpriteId = SPRITE_NONE;
static EWRAM_DATA struct SpriteFrameImage sPortraitImage = {0};

static const struct OamData sOamData_Portrait =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x64),
    .size = SPRITE_SIZE(64x64),
    .priority = 0,
};

// A real frame command is required: image-based sprites only DMA their frame
// when an ANIMCMD_FRAME executes (gDummySpriteAnimTable never loads one).
static const union AnimCmd sAnim_Portrait[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sAnims_Portrait[] =
{
    sAnim_Portrait,
};

static const struct SpriteTemplate sSpriteTemplate_Portrait =
{
    .tileTag = TAG_NONE,
    .paletteTag = PORTRAIT_PAL_TAG,
    .oam = &sOamData_Portrait,
    .anims = sAnims_Portrait,
    .images = &sPortraitImage,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

// The sprite pool is wiped wholesale on map load; our saved id may then be
// stale or reused. Only treat it as ours if it still points at our frame image.
static bool32 PortraitSpriteIsAlive(void)
{
    return sPortraitSpriteId != SPRITE_NONE
        && gSprites[sPortraitSpriteId].inUse
        && gSprites[sPortraitSpriteId].images == &sPortraitImage;
}

void HideNpcPortrait(void)
{
    if (PortraitSpriteIsAlive())
        DestroySprite(&gSprites[sPortraitSpriteId]);
    if (sPortraitSpriteId != SPRITE_NONE)
        FreeSpritePaletteByTag(PORTRAIT_PAL_TAG);
    sPortraitSpriteId = SPRITE_NONE;
}

void ShowNpcPortrait(u8 portraitId, u8 side)
{
    struct SpritePalette palette = { .tag = PORTRAIT_PAL_TAG };
    s16 x;

    if (!USE_NPC_PORTRAITS)
        return;
    if (portraitId >= PORTRAIT_COUNT)
        portraitId = PORTRAIT_PLACEHOLDER;

    HideNpcPortrait(); // showing over an existing portrait swaps cleanly

    sPortraitImage.data = sNpcPortraits[portraitId].tiles;
    sPortraitImage.size = PORTRAIT_SIZE_4BPP;
    palette.data = sNpcPortraits[portraitId].palette;
    LoadSpritePalette(&palette);

    x = (side == PORTRAIT_LEFT) ? PORTRAIT_X_LEFT : PORTRAIT_X_RIGHT;
    sPortraitSpriteId = CreateSprite(&sSpriteTemplate_Portrait, x, PORTRAIT_Y, 0);
    if (sPortraitSpriteId == MAX_SPRITES)
    {
        FreeSpritePaletteByTag(PORTRAIT_PAL_TAG);
        sPortraitSpriteId = SPRITE_NONE;
    }
}
