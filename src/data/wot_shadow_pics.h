// WoT Shadow system: per-species SHADOW battle art (64x64 front sprites,
// uncompressed 4bpp + raw palette). Species without an entry keep the violet
// engine tint. Player-side (back) sprites always keep the tint.
struct WotShadowPic
{
    u16 species;
    const u32 *pic;
    const u16 *pal;
};

static const u32 sWotShadowPic_Absol[] = INCGFX_U32("graphics/pokemon_shadow/absol.png", ".4bpp");
static const u16 sWotShadowPal_Absol[] = INCBIN_U16("graphics/pokemon_shadow/absol.gbapal");
static const u32 sWotShadowPic_Clefable[] = INCGFX_U32("graphics/pokemon_shadow/clefable.png", ".4bpp");
static const u16 sWotShadowPal_Clefable[] = INCBIN_U16("graphics/pokemon_shadow/clefable.gbapal");
static const u32 sWotShadowPic_Teddiursa[] = INCGFX_U32("graphics/pokemon_shadow/teddiursa.png", ".4bpp");
static const u16 sWotShadowPal_Teddiursa[] = INCBIN_U16("graphics/pokemon_shadow/teddiursa.gbapal");
static const u32 sWotShadowPic_Umbreon[] = INCGFX_U32("graphics/pokemon_shadow/umbreon.png", ".4bpp");
static const u16 sWotShadowPal_Umbreon[] = INCBIN_U16("graphics/pokemon_shadow/umbreon.gbapal");
static const u32 sWotShadowPic_Wigglytuff[] = INCGFX_U32("graphics/pokemon_shadow/wigglytuff.png", ".4bpp");
static const u16 sWotShadowPal_Wigglytuff[] = INCBIN_U16("graphics/pokemon_shadow/wigglytuff.gbapal");
static const u32 sWotShadowPic_Lugia[] = INCGFX_U32("graphics/pokemon_shadow/lugia.png", ".4bpp");
static const u16 sWotShadowPal_Lugia[] = INCBIN_U16("graphics/pokemon_shadow/lugia.gbapal");
// The endgame boss of the whole game: Joe's "galaxy alt" art.
static const u32 sWotShadowPic_Jirachi[] = INCGFX_U32("graphics/pokemon_shadow/jirachi.png", ".4bpp");
static const u16 sWotShadowPal_Jirachi[] = INCBIN_U16("graphics/pokemon_shadow/jirachi.gbapal");

static const u32 sWotShadowPic_Alakazam[] = INCGFX_U32("graphics/pokemon_shadow/alakazam.png", ".4bpp");
static const u16 sWotShadowPal_Alakazam[] = INCBIN_U16("graphics/pokemon_shadow/alakazam.gbapal");
static const u32 sWotShadowPic_Charizard[] = INCGFX_U32("graphics/pokemon_shadow/charizard.png", ".4bpp");
static const u16 sWotShadowPal_Charizard[] = INCBIN_U16("graphics/pokemon_shadow/charizard.gbapal");
static const u32 sWotShadowPic_Dragonite[] = INCGFX_U32("graphics/pokemon_shadow/dragonite.png", ".4bpp");
static const u16 sWotShadowPal_Dragonite[] = INCBIN_U16("graphics/pokemon_shadow/dragonite.gbapal");
static const u32 sWotShadowPic_HoOh[] = INCGFX_U32("graphics/pokemon_shadow/ho_oh.png", ".4bpp");
static const u16 sWotShadowPal_HoOh[] = INCBIN_U16("graphics/pokemon_shadow/ho_oh.gbapal");
static const u32 sWotShadowPic_Moltres[] = INCGFX_U32("graphics/pokemon_shadow/moltres.png", ".4bpp");
static const u16 sWotShadowPal_Moltres[] = INCBIN_U16("graphics/pokemon_shadow/moltres.gbapal");
static const u32 sWotShadowPic_Onix[] = INCGFX_U32("graphics/pokemon_shadow/onix.png", ".4bpp");
static const u16 sWotShadowPal_Onix[] = INCBIN_U16("graphics/pokemon_shadow/onix.gbapal");
static const u32 sWotShadowPic_Raikou[] = INCGFX_U32("graphics/pokemon_shadow/raikou.png", ".4bpp");
static const u16 sWotShadowPal_Raikou[] = INCBIN_U16("graphics/pokemon_shadow/raikou.gbapal");
static const u32 sWotShadowPic_Rapidash[] = INCGFX_U32("graphics/pokemon_shadow/rapidash.png", ".4bpp");
static const u16 sWotShadowPal_Rapidash[] = INCBIN_U16("graphics/pokemon_shadow/rapidash.gbapal");

static const u32 sWotShadowPic_Camerupt[] = INCGFX_U32("graphics/pokemon_shadow/camerupt.png", ".4bpp");
static const u16 sWotShadowPal_Camerupt[] = INCBIN_U16("graphics/pokemon_shadow/camerupt.gbapal");
static const u32 sWotShadowPic_Charmeleon[] = INCGFX_U32("graphics/pokemon_shadow/charmeleon.png", ".4bpp");
static const u16 sWotShadowPal_Charmeleon[] = INCBIN_U16("graphics/pokemon_shadow/charmeleon.gbapal");
static const u32 sWotShadowPic_Chimchar[] = INCGFX_U32("graphics/pokemon_shadow/chimchar.png", ".4bpp");
static const u16 sWotShadowPal_Chimchar[] = INCBIN_U16("graphics/pokemon_shadow/chimchar.gbapal");
static const u32 sWotShadowPic_Deoxys[] = INCGFX_U32("graphics/pokemon_shadow/deoxys.png", ".4bpp");
static const u16 sWotShadowPal_Deoxys[] = INCBIN_U16("graphics/pokemon_shadow/deoxys.gbapal");
static const u32 sWotShadowPic_Dragonair[] = INCGFX_U32("graphics/pokemon_shadow/dragonair.png", ".4bpp");
static const u16 sWotShadowPal_Dragonair[] = INCBIN_U16("graphics/pokemon_shadow/dragonair.gbapal");
static const u32 sWotShadowPic_Empoleon[] = INCGFX_U32("graphics/pokemon_shadow/empoleon.png", ".4bpp");
static const u16 sWotShadowPal_Empoleon[] = INCBIN_U16("graphics/pokemon_shadow/empoleon.gbapal");
static const u32 sWotShadowPic_Gallade[] = INCGFX_U32("graphics/pokemon_shadow/gallade.png", ".4bpp");
static const u16 sWotShadowPal_Gallade[] = INCBIN_U16("graphics/pokemon_shadow/gallade.gbapal");
static const u32 sWotShadowPic_Garchomp[] = INCGFX_U32("graphics/pokemon_shadow/garchomp.png", ".4bpp");
static const u16 sWotShadowPal_Garchomp[] = INCBIN_U16("graphics/pokemon_shadow/garchomp.gbapal");
static const u32 sWotShadowPic_Gardevoir[] = INCGFX_U32("graphics/pokemon_shadow/gardevoir.png", ".4bpp");
static const u16 sWotShadowPal_Gardevoir[] = INCBIN_U16("graphics/pokemon_shadow/gardevoir.gbapal");
static const u32 sWotShadowPic_Kingdra[] = INCGFX_U32("graphics/pokemon_shadow/kingdra.png", ".4bpp");
static const u16 sWotShadowPal_Kingdra[] = INCBIN_U16("graphics/pokemon_shadow/kingdra.gbapal");
static const u32 sWotShadowPic_Lanturn[] = INCGFX_U32("graphics/pokemon_shadow/lanturn.png", ".4bpp");
static const u16 sWotShadowPal_Lanturn[] = INCBIN_U16("graphics/pokemon_shadow/lanturn.gbapal");
static const u32 sWotShadowPic_Mudkip[] = INCGFX_U32("graphics/pokemon_shadow/mudkip.png", ".4bpp");
static const u16 sWotShadowPal_Mudkip[] = INCBIN_U16("graphics/pokemon_shadow/mudkip.gbapal");
static const u32 sWotShadowPic_Pikachu[] = INCGFX_U32("graphics/pokemon_shadow/pikachu.png", ".4bpp");
static const u16 sWotShadowPal_Pikachu[] = INCBIN_U16("graphics/pokemon_shadow/pikachu.gbapal");
static const u32 sWotShadowPic_Piplup[] = INCGFX_U32("graphics/pokemon_shadow/piplup.png", ".4bpp");
static const u16 sWotShadowPal_Piplup[] = INCBIN_U16("graphics/pokemon_shadow/piplup.gbapal");
static const u32 sWotShadowPic_Politoed[] = INCGFX_U32("graphics/pokemon_shadow/politoed.png", ".4bpp");
static const u16 sWotShadowPal_Politoed[] = INCBIN_U16("graphics/pokemon_shadow/politoed.gbapal");
static const u32 sWotShadowPic_Squirtle[] = INCGFX_U32("graphics/pokemon_shadow/squirtle.png", ".4bpp");
static const u16 sWotShadowPal_Squirtle[] = INCBIN_U16("graphics/pokemon_shadow/squirtle.gbapal");
static const u32 sWotShadowPic_Turtwig[] = INCGFX_U32("graphics/pokemon_shadow/turtwig.png", ".4bpp");
static const u16 sWotShadowPal_Turtwig[] = INCBIN_U16("graphics/pokemon_shadow/turtwig.gbapal");


// Wave 3 (2026-08-11): the pogokitten batch.
static const u32 sWotShadowPic_Arcanine[] = INCGFX_U32("graphics/pokemon_shadow/arcanine.png", ".4bpp");
static const u16 sWotShadowPal_Arcanine[] = INCBIN_U16("graphics/pokemon_shadow/arcanine.gbapal");
static const u32 sWotShadowPic_Arceus[] = INCGFX_U32("graphics/pokemon_shadow/arceus.png", ".4bpp");
static const u16 sWotShadowPal_Arceus[] = INCBIN_U16("graphics/pokemon_shadow/arceus.gbapal");
static const u32 sWotShadowPic_Azurill[] = INCGFX_U32("graphics/pokemon_shadow/azurill.png", ".4bpp");
static const u16 sWotShadowPal_Azurill[] = INCBIN_U16("graphics/pokemon_shadow/azurill.gbapal");
static const u32 sWotShadowPic_Blastoise[] = INCGFX_U32("graphics/pokemon_shadow/blastoise.png", ".4bpp");
static const u16 sWotShadowPal_Blastoise[] = INCBIN_U16("graphics/pokemon_shadow/blastoise.gbapal");
static const u32 sWotShadowPic_Cresselia[] = INCGFX_U32("graphics/pokemon_shadow/cresselia.png", ".4bpp");
static const u16 sWotShadowPal_Cresselia[] = INCBIN_U16("graphics/pokemon_shadow/cresselia.gbapal");
static const u32 sWotShadowPic_Flygon[] = INCGFX_U32("graphics/pokemon_shadow/flygon.png", ".4bpp");
static const u16 sWotShadowPal_Flygon[] = INCBIN_U16("graphics/pokemon_shadow/flygon.gbapal");
static const u32 sWotShadowPic_Giratina[] = INCGFX_U32("graphics/pokemon_shadow/giratina.png", ".4bpp");
static const u16 sWotShadowPal_Giratina[] = INCBIN_U16("graphics/pokemon_shadow/giratina.gbapal");
static const u32 sWotShadowPic_Luxray[] = INCGFX_U32("graphics/pokemon_shadow/luxray.png", ".4bpp");
static const u16 sWotShadowPal_Luxray[] = INCBIN_U16("graphics/pokemon_shadow/luxray.gbapal");
static const u32 sWotShadowPic_Manectric[] = INCGFX_U32("graphics/pokemon_shadow/manectric.png", ".4bpp");
static const u16 sWotShadowPal_Manectric[] = INCBIN_U16("graphics/pokemon_shadow/manectric.gbapal");
static const u32 sWotShadowPic_Marowak[] = INCGFX_U32("graphics/pokemon_shadow/marowak.png", ".4bpp");
static const u16 sWotShadowPal_Marowak[] = INCBIN_U16("graphics/pokemon_shadow/marowak.gbapal");
static const u32 sWotShadowPic_Suicune[] = INCGFX_U32("graphics/pokemon_shadow/suicune.png", ".4bpp");
static const u16 sWotShadowPal_Suicune[] = INCBIN_U16("graphics/pokemon_shadow/suicune.gbapal");
static const u32 sWotShadowPic_Typhlosion[] = INCGFX_U32("graphics/pokemon_shadow/typhlosion.png", ".4bpp");
static const u16 sWotShadowPal_Typhlosion[] = INCBIN_U16("graphics/pokemon_shadow/typhlosion.gbapal");
static const u32 sWotShadowPic_Vaporeon[] = INCGFX_U32("graphics/pokemon_shadow/vaporeon.png", ".4bpp");
static const u16 sWotShadowPal_Vaporeon[] = INCBIN_U16("graphics/pokemon_shadow/vaporeon.gbapal");
static const u32 sWotShadowPic_Venusaur[] = INCGFX_U32("graphics/pokemon_shadow/venusaur.png", ".4bpp");
static const u16 sWotShadowPal_Venusaur[] = INCBIN_U16("graphics/pokemon_shadow/venusaur.gbapal");

static const struct WotShadowPic sWotShadowPics[] =
{
    { SPECIES_ABSOL,      sWotShadowPic_Absol,      sWotShadowPal_Absol },
    { SPECIES_CLEFABLE,   sWotShadowPic_Clefable,   sWotShadowPal_Clefable },
    { SPECIES_TEDDIURSA,  sWotShadowPic_Teddiursa,  sWotShadowPal_Teddiursa },
    { SPECIES_UMBREON,    sWotShadowPic_Umbreon,    sWotShadowPal_Umbreon },
    { SPECIES_WIGGLYTUFF, sWotShadowPic_Wigglytuff, sWotShadowPal_Wigglytuff },
    { SPECIES_LUGIA,      sWotShadowPic_Lugia,      sWotShadowPal_Lugia },
    { SPECIES_ALAKAZAM,      sWotShadowPic_Alakazam, sWotShadowPal_Alakazam },
    { SPECIES_CHARIZARD,     sWotShadowPic_Charizard, sWotShadowPal_Charizard },
    { SPECIES_DRAGONITE,     sWotShadowPic_Dragonite, sWotShadowPal_Dragonite },
    { SPECIES_HO_OH,         sWotShadowPic_HoOh, sWotShadowPal_HoOh },
    { SPECIES_MOLTRES,       sWotShadowPic_Moltres, sWotShadowPal_Moltres },
    { SPECIES_ONIX,          sWotShadowPic_Onix, sWotShadowPal_Onix },
    { SPECIES_RAIKOU,        sWotShadowPic_Raikou, sWotShadowPal_Raikou },
    { SPECIES_RAPIDASH,      sWotShadowPic_Rapidash, sWotShadowPal_Rapidash },
    { SPECIES_CAMERUPT,      sWotShadowPic_Camerupt, sWotShadowPal_Camerupt },
    { SPECIES_CHARMELEON,    sWotShadowPic_Charmeleon, sWotShadowPal_Charmeleon },
    { SPECIES_CHIMCHAR,      sWotShadowPic_Chimchar, sWotShadowPal_Chimchar },
    { SPECIES_DEOXYS,        sWotShadowPic_Deoxys, sWotShadowPal_Deoxys },
    { SPECIES_DRAGONAIR,     sWotShadowPic_Dragonair, sWotShadowPal_Dragonair },
    { SPECIES_EMPOLEON,      sWotShadowPic_Empoleon, sWotShadowPal_Empoleon },
    { SPECIES_GALLADE,       sWotShadowPic_Gallade, sWotShadowPal_Gallade },
    { SPECIES_GARCHOMP,      sWotShadowPic_Garchomp, sWotShadowPal_Garchomp },
    { SPECIES_GARDEVOIR,     sWotShadowPic_Gardevoir, sWotShadowPal_Gardevoir },
    { SPECIES_KINGDRA,       sWotShadowPic_Kingdra, sWotShadowPal_Kingdra },
    { SPECIES_LANTURN,       sWotShadowPic_Lanturn, sWotShadowPal_Lanturn },
    { SPECIES_MUDKIP,        sWotShadowPic_Mudkip, sWotShadowPal_Mudkip },
    { SPECIES_PIKACHU,       sWotShadowPic_Pikachu, sWotShadowPal_Pikachu },
    { SPECIES_PIPLUP,        sWotShadowPic_Piplup, sWotShadowPal_Piplup },
    { SPECIES_POLITOED,      sWotShadowPic_Politoed, sWotShadowPal_Politoed },
    { SPECIES_SQUIRTLE,      sWotShadowPic_Squirtle, sWotShadowPal_Squirtle },
    { SPECIES_TURTWIG,       sWotShadowPic_Turtwig, sWotShadowPal_Turtwig },
    { SPECIES_JIRACHI,       sWotShadowPic_Jirachi, sWotShadowPal_Jirachi },
    { SPECIES_ARCANINE,     sWotShadowPic_Arcanine, sWotShadowPal_Arcanine },
    { SPECIES_ARCEUS,       sWotShadowPic_Arceus, sWotShadowPal_Arceus },
    { SPECIES_AZURILL,      sWotShadowPic_Azurill, sWotShadowPal_Azurill },
    { SPECIES_BLASTOISE,    sWotShadowPic_Blastoise, sWotShadowPal_Blastoise },
    { SPECIES_CRESSELIA,    sWotShadowPic_Cresselia, sWotShadowPal_Cresselia },
    { SPECIES_FLYGON,       sWotShadowPic_Flygon, sWotShadowPal_Flygon },
    { SPECIES_GIRATINA,     sWotShadowPic_Giratina, sWotShadowPal_Giratina },
    { SPECIES_LUXRAY,       sWotShadowPic_Luxray, sWotShadowPal_Luxray },
    { SPECIES_MANECTRIC,    sWotShadowPic_Manectric, sWotShadowPal_Manectric },
    { SPECIES_MAROWAK,      sWotShadowPic_Marowak, sWotShadowPal_Marowak },
    { SPECIES_SUICUNE,      sWotShadowPic_Suicune, sWotShadowPal_Suicune },
    { SPECIES_TYPHLOSION,   sWotShadowPic_Typhlosion, sWotShadowPal_Typhlosion },
    { SPECIES_VAPOREON,     sWotShadowPic_Vaporeon, sWotShadowPal_Vaporeon },
    { SPECIES_VENUSAUR,     sWotShadowPic_Venusaur, sWotShadowPal_Venusaur },
};
