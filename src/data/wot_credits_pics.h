// WISHES OF TOMORROW -- the cast shown during the credits interludes.
// The stock credits parade the player's caught Pokemon (national dex art).
// This replaces that with a fixed roll call of the hack's own art: shadow
// battle sprites, custom forms, the villains' battle pics, and the studio
// mark. Every entry is a plain uncompressed 64x64 4bpp sheet + its own
// 16-colour palette, so they all load through one code path
// (WotCreateRawPicSprite in src/trainer_pokemon_sprites.c).

struct WotCreditsPic
{
    const u32 *gfx;
    const u16 *pal;
};

static const u32 sWotCreditsGfx_shadow_jirachi[] = INCGFX_U32("graphics/credits_portraits/shadow_jirachi.png", ".4bpp");
static const u16 sWotCreditsPal_shadow_jirachi[] = INCGFX_U16("graphics/credits_portraits/shadow_jirachi.png", ".gbapal");
static const u32 sWotCreditsGfx_armored_mewtwo[] = INCGFX_U32("graphics/credits_portraits/armored_mewtwo.png", ".4bpp");
static const u16 sWotCreditsPal_armored_mewtwo[] = INCGFX_U16("graphics/credits_portraits/armored_mewtwo.png", ".gbapal");
static const u32 sWotCreditsGfx_white_gengar[] = INCGFX_U32("graphics/credits_portraits/white_gengar.png", ".4bpp");
static const u16 sWotCreditsPal_white_gengar[] = INCGFX_U16("graphics/credits_portraits/white_gengar.png", ".gbapal");
static const u32 sWotCreditsGfx_the_oni[] = INCGFX_U32("graphics/credits_portraits/the_oni.png", ".4bpp");
static const u16 sWotCreditsPal_the_oni[] = INCGFX_U16("graphics/credits_portraits/the_oni.png", ".gbapal");
static const u32 sWotCreditsGfx_mutrid_officer[] = INCGFX_U32("graphics/credits_portraits/mutrid_officer.png", ".4bpp");
static const u16 sWotCreditsPal_mutrid_officer[] = INCGFX_U16("graphics/credits_portraits/mutrid_officer.png", ".gbapal");
static const u32 sWotCreditsGfx_shadow_teddiursa[] = INCGFX_U32("graphics/credits_portraits/shadow_teddiursa.png", ".4bpp");
static const u16 sWotCreditsPal_shadow_teddiursa[] = INCGFX_U16("graphics/credits_portraits/shadow_teddiursa.png", ".gbapal");
static const u32 sWotCreditsGfx_shadow_absol[] = INCGFX_U32("graphics/credits_portraits/shadow_absol.png", ".4bpp");
static const u16 sWotCreditsPal_shadow_absol[] = INCGFX_U16("graphics/credits_portraits/shadow_absol.png", ".gbapal");
static const u32 sWotCreditsGfx_flatfoot_logo[] = INCGFX_U32("graphics/credits_portraits/flatfoot_logo.png", ".4bpp");
static const u16 sWotCreditsPal_flatfoot_logo[] = INCGFX_U16("graphics/credits_portraits/flatfoot_logo.png", ".gbapal");

static const struct WotCreditsPic sWotCreditsPics[] =
{
    { sWotCreditsGfx_shadow_jirachi,   sWotCreditsPal_shadow_jirachi   },
    { sWotCreditsGfx_armored_mewtwo,   sWotCreditsPal_armored_mewtwo   },
    { sWotCreditsGfx_white_gengar,     sWotCreditsPal_white_gengar     },
    { sWotCreditsGfx_the_oni,          sWotCreditsPal_the_oni          },
    { sWotCreditsGfx_mutrid_officer,   sWotCreditsPal_mutrid_officer   },
    { sWotCreditsGfx_shadow_teddiursa, sWotCreditsPal_shadow_teddiursa },
    { sWotCreditsGfx_shadow_absol,     sWotCreditsPal_shadow_absol     },
    { sWotCreditsGfx_flatfoot_logo,    sWotCreditsPal_flatfoot_logo    },
};

#define NUM_WOT_CREDITS_PICS ARRAY_COUNT(sWotCreditsPics)
