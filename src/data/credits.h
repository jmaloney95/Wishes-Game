// WISHES OF TOMORROW -- custom end credits.
// Replaces the stock Emerald staff roll wholesale. The credits engine
// (src/credits.c) is untouched: it walks PAGE_COUNT pages of ENTRIES_PER_PAGE
// lines each, changing scene every PAGE_INTERVAL (= PAGE_COUNT / 9) pages, so
// the page count is kept a comfortable multiple of 9.
// Keep every line at 30 characters or fewer -- longer strings overrun the
// credits window.

enum
{
    PAGE_TITLE,
    PAGE_SPRITE_ART_RAFAEL,
    PAGE_SPRITE_ART_2,
    PAGE_SPRITE_ART_3,
    PAGE_PORTRAIT_ART,
    PAGE_SHADOW_SPRITES,
    PAGE_TILESET_ART_1,
    PAGE_TILESET_ART_2,
    PAGE_SHIN_TOKYO_ART,
    PAGE_BATTLE_ART,
    PAGE_MUSIC,
    PAGE_PLAYTESTING,
    PAGE_ENGINE,
    PAGE_BASED_ON,
    PAGE_SPECIAL_THANKS,
    PAGE_MEMORIAL,
    PAGE_STUDIO,
    PAGE_CLOSING_1,
    PAGE_CLOSING_2,
    PAGE_COUNT // 19 -- PAGE_INTERVAL stays 2 (integer 19/9), scenes change every 2 pages
};

#define ENTRIES_PER_PAGE 5

static const u8 sCreditsText_EmptyString[]        = _("");
static const u8 sCreditsText_WishesOfTomorrow[]   = _("WISHES OF TOMORROW");
static const u8 sCreditsText_Credits[]            = _("Credits");

static const u8 sCreditsText_AGameBy[]            = _("A game by");
static const u8 sCreditsText_StoryAndDirection[]  = _("Story & Direction");
static const u8 sCreditsText_GameDesign[]         = _("Game Design");
static const u8 sCreditsText_JoeMaloney[]         = _("Joe Maloney");

static const u8 sCreditsText_SpriteDesign[]       = _("Sprite Design");
static const u8 sCreditsText_CharacterArt[]       = _("Character Art");
static const u8 sCreditsText_RafaelSanna[]        = _("Rafael Sanna");
static const u8 sCreditsText_Aveontrainer[]       = _("aveontrainer");
static const u8 sCreditsText_BaylorHernandez[]    = _("baylorhernandez");
static const u8 sCreditsText_Biadoxaf[]           = _("biadoxaf");
static const u8 sCreditsText_DarkusShadow[]       = _("DarkusShadow");
static const u8 sCreditsText_Jaov46[]             = _("jaov46");
static const u8 sCreditsText_ToxShadows[]         = _("ToxShadows642");
static const u8 sCreditsText_DarkSlayer[]         = _("Dark Slayer");

static const u8 sCreditsText_ShadowSprites[]      = _("Shadow Sprites");
static const u8 sCreditsText_Pogokitten[]         = _("pogokitten");
static const u8 sCreditsText_Weegeedude[]         = _("WeeGeeDude");
static const u8 sCreditsText_Quanyails[]          = _("Quanyails");
static const u8 sCreditsText_TilesetArt[]         = _("Tileset Art");
static const u8 sCreditsText_Pinkscales[]         = _("pinkscales");
static const u8 sCreditsText_Phyromatical[]       = _("Phyromatical");
static const u8 sCreditsText_Magiscarf[]          = _("MagiScarf");
static const u8 sCreditsText_Peekychew[]          = _("PeekyChew");
static const u8 sCreditsText_Elinthind[]          = _("Elinthind");
static const u8 sCreditsText_Lo8jd[]              = _("lo8jd");
static const u8 sCreditsText_Ekat99[]             = _("ekat99");

static const u8 sCreditsText_ShinTokyoArt[]       = _("Shin-Tokyo Art");
static const u8 sCreditsText_Emeiry[]             = _("Emeiry");
static const u8 sCreditsText_OdiseaByEkat99[]     = _("Odisea by ekat99");
static const u8 sCreditsText_BattleBackdrops[]    = _("Battle Backdrops");
static const u8 sCreditsText_Carchagui[]          = _("carchagui");

static const u8 sCreditsText_MapDesign[]          = _("Map Design");
static const u8 sCreditsText_EventScripting[]     = _("Event Scripting");
static const u8 sCreditsText_Music[]              = _("Music");
static const u8 sCreditsText_ArrangedFrom[]       = _("Arranged from the");
static const u8 sCreditsText_PkmnSoundtracks[]    = _("POKéMON soundtracks");

static const u8 sCreditsText_Playtesting[]        = _("Playtesting");
static const u8 sCreditsText_QualityAssurance[]   = _("Quality Assurance");
static const u8 sCreditsText_LukeDevereux[]       = _("Luke Devereux");
static const u8 sCreditsText_MikeMancuso[]        = _("Mike Mancuso");
static const u8 sCreditsText_KyleClarkson[]       = _("Kyle Clarkson");

static const u8 sCreditsText_BuiltWith[]          = _("Built With");
static const u8 sCreditsText_PokeemeraldExp[]     = _("pokeemerald-expansion");
static const u8 sCreditsText_RomHackingHideout[]  = _("Rom Hacking Hideout");
static const u8 sCreditsText_DecompTeam[]         = _("The pret decomp team");
static const u8 sCreditsText_BasedOn[]            = _("Based on");
static const u8 sCreditsText_PkmnEmerald[]        = _("POKéMON EMERALD VERSION");
static const u8 sCreditsText_GameFreakNintendo[]  = _("GAME FREAK · Nintendo");

static const u8 sCreditsText_SpecialThanks[]      = _("Special Thanks");
static const u8 sCreditsText_SpritingCommunity[]  = _("The spriting community");
static const u8 sCreditsText_EveryArtist[]        = _("Every artist who shares");
static const u8 sCreditsText_ThankYou[]           = _("Thank you for playing.");
static const u8 sCreditsText_FlatfootGames[]      = _("Flatfoot Games");

static const u8 sCreditsText_EveryWish[]          = _("Every wish is its own again.");
static const u8 sCreditsText_TheEnd[]             = _("Wishes of Tomorrow");
static const u8 sCreditsText_RestInParadise[]     = _("Rest in Paradise, Connor.");

static const struct CreditsEntry sCreditsEntry_EmptyString        = { 0, FALSE, sCreditsText_EmptyString};
static const struct CreditsEntry sCreditsEntry_WishesOfTomorrow   = { 6,  TRUE, sCreditsText_WishesOfTomorrow};
static const struct CreditsEntry sCreditsEntry_Credits            = {11,  TRUE, sCreditsText_Credits};
static const struct CreditsEntry sCreditsEntry_AGameBy            = {10,  TRUE, sCreditsText_AGameBy};
static const struct CreditsEntry sCreditsEntry_StoryAndDirection  = { 7,  TRUE, sCreditsText_StoryAndDirection};
static const struct CreditsEntry sCreditsEntry_GameDesign         = { 9,  TRUE, sCreditsText_GameDesign};
static const struct CreditsEntry sCreditsEntry_JoeMaloney         = { 9, FALSE, sCreditsText_JoeMaloney};
static const struct CreditsEntry sCreditsEntry_SpriteDesign       = { 9,  TRUE, sCreditsText_SpriteDesign};
static const struct CreditsEntry sCreditsEntry_CharacterArt       = { 9,  TRUE, sCreditsText_CharacterArt};
static const struct CreditsEntry sCreditsEntry_RafaelSanna        = { 9, FALSE, sCreditsText_RafaelSanna};
static const struct CreditsEntry sCreditsEntry_Aveontrainer       = { 9, FALSE, sCreditsText_Aveontrainer};
static const struct CreditsEntry sCreditsEntry_BaylorHernandez    = { 8, FALSE, sCreditsText_BaylorHernandez};
static const struct CreditsEntry sCreditsEntry_Biadoxaf           = {10, FALSE, sCreditsText_Biadoxaf};
static const struct CreditsEntry sCreditsEntry_DarkusShadow       = { 9, FALSE, sCreditsText_DarkusShadow};
static const struct CreditsEntry sCreditsEntry_Jaov46             = {11, FALSE, sCreditsText_Jaov46};
static const struct CreditsEntry sCreditsEntry_ToxShadows         = { 9, FALSE, sCreditsText_ToxShadows};
static const struct CreditsEntry sCreditsEntry_DarkSlayer         = { 9, FALSE, sCreditsText_DarkSlayer};
static const struct CreditsEntry sCreditsEntry_ShadowSprites      = { 9,  TRUE, sCreditsText_ShadowSprites};
static const struct CreditsEntry sCreditsEntry_Pogokitten         = {10, FALSE, sCreditsText_Pogokitten};
static const struct CreditsEntry sCreditsEntry_Weegeedude         = {10, FALSE, sCreditsText_Weegeedude};
static const struct CreditsEntry sCreditsEntry_Quanyails          = {10, FALSE, sCreditsText_Quanyails};
static const struct CreditsEntry sCreditsEntry_TilesetArt         = {10,  TRUE, sCreditsText_TilesetArt};
static const struct CreditsEntry sCreditsEntry_Pinkscales         = {10, FALSE, sCreditsText_Pinkscales};
static const struct CreditsEntry sCreditsEntry_Phyromatical       = { 9, FALSE, sCreditsText_Phyromatical};
static const struct CreditsEntry sCreditsEntry_Magiscarf          = {10, FALSE, sCreditsText_Magiscarf};
static const struct CreditsEntry sCreditsEntry_Peekychew          = {10, FALSE, sCreditsText_Peekychew};
static const struct CreditsEntry sCreditsEntry_Elinthind          = {10, FALSE, sCreditsText_Elinthind};
static const struct CreditsEntry sCreditsEntry_Lo8jd              = {11, FALSE, sCreditsText_Lo8jd};
static const struct CreditsEntry sCreditsEntry_Ekat99             = {11, FALSE, sCreditsText_Ekat99};
static const struct CreditsEntry sCreditsEntry_ShinTokyoArt       = { 9,  TRUE, sCreditsText_ShinTokyoArt};
static const struct CreditsEntry sCreditsEntry_Emeiry             = {11, FALSE, sCreditsText_Emeiry};
static const struct CreditsEntry sCreditsEntry_OdiseaByEkat99     = { 8, FALSE, sCreditsText_OdiseaByEkat99};
static const struct CreditsEntry sCreditsEntry_BattleBackdrops    = { 8,  TRUE, sCreditsText_BattleBackdrops};
static const struct CreditsEntry sCreditsEntry_Carchagui          = {10, FALSE, sCreditsText_Carchagui};
static const struct CreditsEntry sCreditsEntry_MapDesign          = {10,  TRUE, sCreditsText_MapDesign};
static const struct CreditsEntry sCreditsEntry_EventScripting     = { 8,  TRUE, sCreditsText_EventScripting};
static const struct CreditsEntry sCreditsEntry_Music              = {12,  TRUE, sCreditsText_Music};
static const struct CreditsEntry sCreditsEntry_ArrangedFrom       = { 8, FALSE, sCreditsText_ArrangedFrom};
static const struct CreditsEntry sCreditsEntry_PkmnSoundtracks    = { 7, FALSE, sCreditsText_PkmnSoundtracks};
static const struct CreditsEntry sCreditsEntry_Playtesting        = { 9,  TRUE, sCreditsText_Playtesting};
static const struct CreditsEntry sCreditsEntry_QualityAssurance   = { 7,  TRUE, sCreditsText_QualityAssurance};
static const struct CreditsEntry sCreditsEntry_LukeDevereux       = { 9, FALSE, sCreditsText_LukeDevereux};
static const struct CreditsEntry sCreditsEntry_MikeMancuso        = { 9, FALSE, sCreditsText_MikeMancuso};
static const struct CreditsEntry sCreditsEntry_KyleClarkson       = { 9, FALSE, sCreditsText_KyleClarkson};
static const struct CreditsEntry sCreditsEntry_BuiltWith          = {10,  TRUE, sCreditsText_BuiltWith};
static const struct CreditsEntry sCreditsEntry_PokeemeraldExp     = { 6, FALSE, sCreditsText_PokeemeraldExp};
static const struct CreditsEntry sCreditsEntry_RomHackingHideout  = { 7, FALSE, sCreditsText_RomHackingHideout};
static const struct CreditsEntry sCreditsEntry_DecompTeam         = { 7, FALSE, sCreditsText_DecompTeam};
static const struct CreditsEntry sCreditsEntry_BasedOn            = {11,  TRUE, sCreditsText_BasedOn};
static const struct CreditsEntry sCreditsEntry_PkmnEmerald        = { 5, FALSE, sCreditsText_PkmnEmerald};
static const struct CreditsEntry sCreditsEntry_GameFreakNintendo  = { 6, FALSE, sCreditsText_GameFreakNintendo};
static const struct CreditsEntry sCreditsEntry_SpecialThanks      = { 9,  TRUE, sCreditsText_SpecialThanks};
static const struct CreditsEntry sCreditsEntry_SpritingCommunity  = { 6, FALSE, sCreditsText_SpritingCommunity};
static const struct CreditsEntry sCreditsEntry_EveryArtist        = { 6, FALSE, sCreditsText_EveryArtist};
static const struct CreditsEntry sCreditsEntry_ThankYou           = { 6,  TRUE, sCreditsText_ThankYou};
static const struct CreditsEntry sCreditsEntry_FlatfootGames      = { 9,  TRUE, sCreditsText_FlatfootGames};
static const struct CreditsEntry sCreditsEntry_EveryWish          = { 4, FALSE, sCreditsText_EveryWish};
static const struct CreditsEntry sCreditsEntry_TheEnd             = { 7,  TRUE, sCreditsText_TheEnd};
static const struct CreditsEntry sCreditsEntry_RestInParadise     = { 7, FALSE, sCreditsText_RestInParadise};

#define _ &sCreditsEntry_EmptyString
static const struct CreditsEntry *const sCreditsEntryPointerTable[PAGE_COUNT][ENTRIES_PER_PAGE] =
{
    [PAGE_TITLE] = {
        _,
        &sCreditsEntry_WishesOfTomorrow,
        &sCreditsEntry_Credits,
        _,
        _
    },
    [PAGE_SPRITE_ART_RAFAEL] = {
        _,
        &sCreditsEntry_SpriteDesign,
        &sCreditsEntry_RafaelSanna,
        _,
        _
    },
    [PAGE_SPRITE_ART_2] = {
        _,
        &sCreditsEntry_SpriteDesign,
        &sCreditsEntry_Aveontrainer,
        &sCreditsEntry_BaylorHernandez,
        &sCreditsEntry_Biadoxaf
    },
    [PAGE_SPRITE_ART_3] = {
        _,
        &sCreditsEntry_SpriteDesign,
        &sCreditsEntry_DarkusShadow,
        &sCreditsEntry_Jaov46,
        _
    },
    [PAGE_PORTRAIT_ART] = {
        _,
        &sCreditsEntry_CharacterArt,
        &sCreditsEntry_ToxShadows,
        _,
        _
    },
    [PAGE_SHADOW_SPRITES] = {
        _,
        &sCreditsEntry_ShadowSprites,
        &sCreditsEntry_Pogokitten,
        &sCreditsEntry_Weegeedude,
        &sCreditsEntry_Quanyails
    },
    [PAGE_TILESET_ART_1] = {
        &sCreditsEntry_TilesetArt,
        &sCreditsEntry_Pinkscales,
        &sCreditsEntry_Phyromatical,
        &sCreditsEntry_Magiscarf,
        &sCreditsEntry_Peekychew
    },
    [PAGE_TILESET_ART_2] = {
        _,
        &sCreditsEntry_TilesetArt,
        &sCreditsEntry_Elinthind,
        &sCreditsEntry_Lo8jd,
        &sCreditsEntry_DarkSlayer
    },
    [PAGE_SHIN_TOKYO_ART] = {
        _,
        &sCreditsEntry_ShinTokyoArt,
        &sCreditsEntry_Emeiry,
        &sCreditsEntry_OdiseaByEkat99,
        _
    },
    [PAGE_BATTLE_ART] = {
        _,
        &sCreditsEntry_BattleBackdrops,
        &sCreditsEntry_Carchagui,
        &sCreditsEntry_Aveontrainer,
        _
    },
    [PAGE_MUSIC] = {
        _,
        &sCreditsEntry_Music,
        &sCreditsEntry_ArrangedFrom,
        &sCreditsEntry_PkmnSoundtracks,
        _
    },
    [PAGE_PLAYTESTING] = {
        _,
        &sCreditsEntry_Playtesting,
        &sCreditsEntry_LukeDevereux,
        &sCreditsEntry_MikeMancuso,
        &sCreditsEntry_KyleClarkson
    },
    [PAGE_ENGINE] = {
        _,
        &sCreditsEntry_BuiltWith,
        &sCreditsEntry_PokeemeraldExp,
        &sCreditsEntry_RomHackingHideout,
        &sCreditsEntry_DecompTeam
    },
    [PAGE_BASED_ON] = {
        _,
        &sCreditsEntry_BasedOn,
        &sCreditsEntry_PkmnEmerald,
        &sCreditsEntry_GameFreakNintendo,
        _
    },
    [PAGE_SPECIAL_THANKS] = {
        _,
        &sCreditsEntry_SpecialThanks,
        &sCreditsEntry_SpritingCommunity,
        &sCreditsEntry_EveryArtist,
        _
    },
    [PAGE_MEMORIAL] = {
        _,
        _,
        &sCreditsEntry_RestInParadise,
        _,
        _
    },
    [PAGE_STUDIO] = {
        _,
        &sCreditsEntry_FlatfootGames,
        _,
        _,
        _
    },
    [PAGE_CLOSING_1] = {
        _,
        &sCreditsEntry_ThankYou,
        _,
        _,
        _
    },
    [PAGE_CLOSING_2] = {
        _,
        &sCreditsEntry_AGameBy,
        &sCreditsEntry_JoeMaloney,
        _,
        _
    },
};
#undef _
