#include "global.h"

// Closed test build for Discord testers: `make WOT_TESTER=1` (see Tools/make_tester_patch.py).
// Turns a save into a test profile from the debug menu: its story flags stay, and its team,
// bag and Mega Dimension progress are replaced. Never in a public ROM: Tools/make_bps_patch.py
// refuses any ROM holding the marker below.
#ifdef WOT_TESTER

#include "event_data.h"
#include "fake_rtc.h"
#include "item.h"
#include "load_save.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "rtc.h"
#include "save.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "wot_mega_dimension.h"
#include "wot_tester.h"
#include "constants/characters.h"
#include "constants/items.h"
#include "constants/moves.h"

void SetContinueGameWarp(s8 mapGroup, s8 mapNum, s8 warpId, s8 x, s8 y);

// Bump the build name for every build handed to testers.
#define WOT_TESTER_MARKER_PREFIX "WOT_TESTER_BUILD:"
const char gWotTesterMarker[] KEEP_SECTION USED = WOT_TESTER_MARKER_PREFIX "T2";

// A saved byte nothing reads holds the profile: the secret half of the trainer id.
#define PROFILE_ID_MAGIC 0x57

#define TRADEWIND_ABOVE_ROW_X 30
#define TRADEWIND_ABOVE_ROW_Y 8
#define HOTEL_Z_DOORSTEP_X 23
#define HOTEL_Z_DOORSTEP_Y 18

// The portal day turns over at 08:00, so the intro's "first rifts tomorrow" is
// three real minutes away at 05:00.
#define INTRO_START_HOUR 5
// Already unlocked: the morning's rifts are ten game minutes away.
#define UNLOCKED_START_HOUR 7
#define UNLOCKED_START_MINUTE 50

#define MABLE_TALLY_NEAR_DONE 20
#define MIN_TESTER_MONEY 100000

struct TesterMon
{
    u16 species;
    u8 level;
    u8 shiny;
    u16 heldItem;
    u16 firstMove;
};

enum TesterStart
{
    START_BEFORE_INTRO,
    START_UNLOCKED,
};

struct TesterProfile
{
    const struct TesterMon *team;
    u8 teamSize;
    u8 starterChoice;
    enum TesterStart start;
};

// Each team: the starter at about Lv40 with its stone, a Gen X starter, then catches.
static const struct TesterMon sTeamGarchomp[] =
{
    {SPECIES_GARCHOMP, 40, FALSE, ITEM_GARCHOMPITE, MOVE_THOUSAND_ARROWS},
    {SPECIES_GECQUADORF, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_LUXRAY, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_TALONFLAME, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_GARDEVOIR, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_ALTARIA, 37, FALSE, ITEM_NONE, MOVE_DEFAULT},
};

static const struct TesterMon sTeamHaxorus[] =
{
    {SPECIES_HAXORUS, 40, FALSE, ITEM_HAXORITE, MOVE_DRAGON_DARTS},
    {SPECIES_HORNBREW, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_PIKACHU, 37, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_GURDURR, 37, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_SCRAGGY, 37, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_NINETALES_ALOLA, 37, FALSE, ITEM_NONE, MOVE_DEFAULT},
};

// The bedroom's Developer's Gift, plus a normal team: does the level scaling hold?
static const struct TesterMon sTeamMewtwo[] =
{
    {SPECIES_MEWTWO_ARMORED, 80, FALSE, ITEM_NONE, MOVE_PSYSTRIKE},
    {SPECIES_GARCHOMP, 40, FALSE, ITEM_GARCHOMPITE, MOVE_THOUSAND_ARROWS},
    {SPECIES_CALDERUFF, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_LUXRAY, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_TALONFLAME, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_GARDEVOIR, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
};

static const struct TesterMon sTeamBaxcalibur[] =
{
    {SPECIES_BAXCALIBUR, 40, FALSE, ITEM_BAXCALIBRITE, MOVE_GLACIATE},
    {SPECIES_CALDERUFF, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_LUXRAY, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_TALONFLAME, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_GARDEVOIR, 38, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_ALTARIA, 37, FALSE, ITEM_NONE, MOVE_DEFAULT},
};

static const struct TesterMon sTeamLate[] =
{
    {SPECIES_BAXCALIBUR, 46, FALSE, ITEM_BAXCALIBRITE, MOVE_GLACIATE},
    {SPECIES_GECQUADORF, 44, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_GENGAR, 45, TRUE, ITEM_GENGARITE, MOVE_DEFAULT},
    {SPECIES_LUXRAY, 44, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_TALONFLAME, 44, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_GARDEVOIR, 44, FALSE, ITEM_NONE, MOVE_DEFAULT},
};

static const struct TesterMon sTeamPostgame[] =
{
    {SPECIES_GARCHOMP, 72, FALSE, ITEM_GARCHOMPITE, MOVE_THOUSAND_ARROWS},
    {SPECIES_HORNBREW, 70, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_GENGAR, 70, TRUE, ITEM_GENGARITE, MOVE_DEFAULT},
    {SPECIES_LUXRAY, 70, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_TALONFLAME, 70, FALSE, ITEM_NONE, MOVE_DEFAULT},
    {SPECIES_GARDEVOIR, 70, FALSE, ITEM_NONE, MOVE_DEFAULT},
};

static const struct TesterProfile sProfiles[WOT_TESTER_PROFILE_COUNT] =
{
    [WOT_TESTER_PROFILE_A] = {sTeamGarchomp, ARRAY_COUNT(sTeamGarchomp), 1, START_BEFORE_INTRO},
    [WOT_TESTER_PROFILE_B] = {sTeamHaxorus, ARRAY_COUNT(sTeamHaxorus), 3, START_BEFORE_INTRO},
    [WOT_TESTER_PROFILE_C] = {sTeamMewtwo, ARRAY_COUNT(sTeamMewtwo), 1, START_BEFORE_INTRO},
    [WOT_TESTER_PROFILE_D] = {sTeamBaxcalibur, ARRAY_COUNT(sTeamBaxcalibur), 2, START_BEFORE_INTRO},
    [WOT_TESTER_PROFILE_E] = {sTeamLate, ARRAY_COUNT(sTeamLate), 2, START_UNLOCKED},
    [WOT_TESTER_PROFILE_F] = {sTeamPostgame, ARRAY_COUNT(sTeamPostgame), 1, START_UNLOCKED},
};

static u8 AsciiToGameChar(char c)
{
    if (c >= 'A' && c <= 'Z')
        return CHAR_A + (c - 'A');
    if (c >= '0' && c <= '9')
        return CHAR_0 + (c - '0');
    return CHAR_SPACE;
}

void WotTester_BuildLabel(u8 *dest)
{
    const char *build = gWotTesterMarker + sizeof(WOT_TESTER_MARKER_PREFIX) - 1;
    u32 profile = WOT_TESTER_PROFILE_COUNT;

    if (gSaveBlock2Ptr->playerTrainerId[3] == PROFILE_ID_MAGIC
     && gSaveBlock2Ptr->playerTrainerId[2] < WOT_TESTER_PROFILE_COUNT)
        profile = gSaveBlock2Ptr->playerTrainerId[2];
    while (*build != '\0')
        *dest++ = AsciiToGameChar(*build++);
    *dest++ = CHAR_HYPHEN;
    *dest++ = CHAR_A + profile;
    *dest = EOS;
}

static void GiveTeam(const struct TesterProfile *profile)
{
    u32 i;

    ZeroPlayerPartyMons();
    gPlayerPartyCount = 0;
    ResetPokemonStorageSystem();
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(gSaveBlock1Ptr->dexCaught));
    memset(gSaveBlock1Ptr->dexSeen, 0, sizeof(gSaveBlock1Ptr->dexSeen));
    for (i = 0; i < profile->teamSize; i++)
    {
        const struct TesterMon *mon = &profile->team[i];

        WotTester_GiveMon(mon->species, mon->level, mon->heldItem, mon->firstMove, mon->shiny);
    }
    CalculatePlayerPartyCount();
    HealPlayerParty();
    VarSet(VAR_STARTER_CHOICE, profile->starterChoice);
}

static void EmptyPocket(enum Pocket pocketId)
{
    u32 i;

    for (i = gBagPockets[pocketId].capacity; i-- > 0;)
    {
        enum Item item = GetBagItemId(pocketId, i);

        if (item != ITEM_NONE)
            RemoveBagItem(item, GetBagItemQuantity(pocketId, i));
    }
}

// Key items and TMs stay, since the story handed those out. The rest may be a dev's
// stockpile, Mega stones and Z crystals included.
static void SetUpBag(void)
{
    EmptyPocket(POCKET_ITEMS);
    EmptyPocket(POCKET_POKE_BALLS);
    EmptyPocket(POCKET_BERRIES);
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    AddBagItem(ITEM_ULTRA_BALL, 30);
    AddBagItem(ITEM_GREAT_BALL, 20);
    AddBagItem(ITEM_HYPER_POTION, 20);
    AddBagItem(ITEM_REVIVE, 10);
    AddBagItem(ITEM_FULL_HEAL, 10);
    AddBagItem(ITEM_MAX_REPEL, 10);
    if (!CheckBagHasItem(ITEM_MEGA_RING, 1))
        AddBagItem(ITEM_MEGA_RING, 1);
    if (GetMoney(&gSaveBlock1Ptr->money) < MIN_TESTER_MONEY)
        SetMoney(&gSaveBlock1Ptr->money, MIN_TESTER_MONEY);
}

// The local clock, which the rifts follow: a save's time offset can put it hours from the RTC.
static void ForwardLocalClockTo(u32 hour, u32 minute)
{
    s32 minutes;

    RtcCalcLocalTime();
    minutes = (s32)(hour * MINUTES_PER_HOUR + minute) - (gLocalTime.hours * MINUTES_PER_HOUR + gLocalTime.minutes);
    if (minutes < 0)
        minutes += HOURS_PER_DAY * MINUTES_PER_HOUR;
    // Hours apart from minutes: the clock adds its minutes to a u8.
    FakeRtc_AdvanceTimeBy(0, minutes / MINUTES_PER_HOUR, minutes % MINUTES_PER_HOUR, 0);
}

static void SetStart(enum TesterStart start)
{
    if (start == START_UNLOCKED)
    {
        ForwardLocalClockTo(UNLOCKED_START_HOUR, UNLOCKED_START_MINUTE);
        WotTester_SetSeenMaps(TRUE);
        WotTester_OpenMegaDimension();
        SetContinueGameWarp(MAP_GROUP(MAP_TRADEWIND_TOWN), MAP_NUM(MAP_TRADEWIND_TOWN), WARP_ID_NONE,
                            HOTEL_Z_DOORSTEP_X, HOTEL_Z_DOORSTEP_Y);
    }
    else
    {
        ForwardLocalClockTo(INTRO_START_HOUR, 0);
        WotTester_SetSeenMaps(FALSE);
        SetContinueGameWarp(MAP_GROUP(MAP_TRADEWIND_TOWN), MAP_NUM(MAP_TRADEWIND_TOWN), WARP_ID_NONE,
                            TRADEWIND_ABOVE_ROW_X, TRADEWIND_ABOVE_ROW_Y);
    }
    SetContinueGameWarpStatus();
}

// Run on a base save with the story progress you want, then turn the game off and take
// the .sav. The game writes the save itself, so its format can't drift from the build's.
void WotTester_ApplyProfile(enum WotTesterProfile profileId)
{
    const struct TesterProfile *profile = &sProfiles[profileId];

    // The id comes first: new Pokémon take their OT and shininess from it.
    StringCopy(gSaveBlock2Ptr->playerName, COMPOUND_STRING("TESTER"));
    gSaveBlock2Ptr->playerTrainerId[2] = profileId;
    gSaveBlock2Ptr->playerTrainerId[3] = PROFILE_ID_MAGIC;

    WotTester_ResetMegaDimension();
    FlagClear(FLAG_GOT_DEV_MEWTWO);
    FlagClear(FLAG_WOT_RANDOMIZER);
    FlagClear(FLAG_WOT_RAND_STARTERS);
    FlagClear(FLAG_WOT_RAND_WILD);
    FlagClear(FLAG_WOT_RAND_TRAINERS);
    SetUpBag();
    GiveTeam(profile);
    SetStart(profile->start);

    switch (profileId)
    {
    case WOT_TESTER_PROFILE_C:
        FlagSet(FLAG_GOT_DEV_MEWTWO);
        break;
    case WOT_TESTER_PROFILE_D:
        FlagSet(FLAG_WOT_RANDOMIZER);
        FlagSet(FLAG_WOT_RAND_WILD);
        FlagSet(FLAG_WOT_RAND_TRAINERS);
        break;
    case WOT_TESTER_PROFILE_E:
        // 23 claims would take more rifts than a tester's three hours. Three to go.
        WotTester_SetMableTally(MABLE_TALLY_NEAR_DONE);
        break;
    case WOT_TESTER_PROFILE_F:
        WotTester_MaxPartners();
        break;
    default:
        break;
    }

    // Twice, so the save's two slots match.
    TrySavingData(SAVE_NORMAL);
    TrySavingData(SAVE_NORMAL);
}

#endif // WOT_TESTER
