#include "global.h"
#include "caps.h"
#include "event_data.h"
#include "item.h"
#include "battle_main.h"
#include "line_break.h"
#include "move.h"
#include "overworld.h"
#include "pokemon.h"
#include "regions.h"
#include "region_map.h"
#include "string_util.h"
#include "text.h"
#include "constants/items.h"
#include "constants/rtc.h"
#include "constants/weather.h"

// ---------------------------------------------------------------------------
// The widow in Tradewind's house knows how every Pokemon grows. Shown one of
// the party, she gives each way it can evolve -- the level, the item and where
// that item is sold, the time of day or whatever else it takes -- one message
// per way, and says so when the level cap stands in the way.
//
//   WotEvoAdvice_Prepare: VAR_0x8004 = party slot. VAR_RESULT = how many ways
//                         (0: it doesn't evolve, 0xFF: an Egg). STR_VAR_1 = its species.
//   WotEvoAdvice_Get:     VAR_0x8005 = which way. gStringVar4 = what she says.
//
// Ways the game can never satisfy are left out: a Hisui- or Alola-only branch,
// the other gender's, and a trade when an item does the same job.
// ---------------------------------------------------------------------------

#define ADVICE_MAX 16
#define ADVICE_WIDTH 208

static EWRAM_DATA u16 sAdviceSpecies = 0;
static EWRAM_DATA u8 sAdviceCount = 0;
static EWRAM_DATA u8 sAdvice[ADVICE_MAX] = {0};

// Where each evolution item is sold in WoT. Anything else isn't.
static const struct { u16 item; const u8 *place; } sItemPlaces[] =
{
    {ITEM_FIRE_STONE,       COMPOUND_STRING("the department store here, 3F")},
    {ITEM_WATER_STONE,      COMPOUND_STRING("the department store here, 3F")},
    {ITEM_THUNDER_STONE,    COMPOUND_STRING("the department store here, 3F")},
    {ITEM_LEAF_STONE,       COMPOUND_STRING("the department store here, 3F")},
    {ITEM_ICE_STONE,        COMPOUND_STRING("the department store here, 3F")},
    {ITEM_MOON_STONE,       COMPOUND_STRING("the department store here, 3F")},
    {ITEM_DUSK_STONE,       COMPOUND_STRING("the department store here, 3F")},
    {ITEM_DAWN_STONE,       COMPOUND_STRING("the department store here, 3F")},
    {ITEM_RAZOR_CLAW,       COMPOUND_STRING("the department store here, 3F")},
    {ITEM_METAL_COAT,       COMPOUND_STRING("the department store here, 3F")},
    {ITEM_KINGS_ROCK,       COMPOUND_STRING("the department store here, 3F")},
    {ITEM_SUN_STONE,        COMPOUND_STRING("the Black Market in Shin Tokyo")},
    {ITEM_LINKING_CORD,     COMPOUND_STRING("the Black Market in Shin Tokyo")},
    {ITEM_ELECTIRIZER,      COMPOUND_STRING("the Black Market in Shin Tokyo")},
    {ITEM_MAGMARIZER,       COMPOUND_STRING("the Black Market in Shin Tokyo")},
    {ITEM_PRISM_SCALE,      COMPOUND_STRING("the Black Market in Shin Tokyo")},
    {ITEM_PROTECTOR,        COMPOUND_STRING("the Black Market in Shin Tokyo")},
    {ITEM_UPGRADE,          COMPOUND_STRING("the Black Market in Shin Tokyo")},
    {ITEM_BLACK_AUGURITE,   COMPOUND_STRING("the Black Market in Shin Tokyo")},
    {ITEM_PEAT_BLOCK,       COMPOUND_STRING("the Black Market in Shin Tokyo")},
    {ITEM_LEADERS_CREST,    COMPOUND_STRING("the Game Corner's prize counter")},
    {ITEM_METAL_ALLOY,      COMPOUND_STRING("the Game Corner's prize counter")},
    {ITEM_AUSPICIOUS_ARMOR, COMPOUND_STRING("the Game Corner's prize counter")},
    {ITEM_MALICIOUS_ARMOR,  COMPOUND_STRING("the Game Corner's prize counter")},
};

static const u8 *GetItemPlace(u16 item)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sItemPlaces); i++)
    {
        if (sItemPlaces[i].item == item)
            return sItemPlaces[i].place;
    }
    return NULL;
}

static bool32 IsUsableMethod(u16 method)
{
    switch (method)
    {
    case EVO_LEVEL:
    case EVO_ITEM:
    case EVO_TRADE:
    case EVO_LEVEL_BATTLE_ONLY:
    case EVO_BATTLE_END:
    case EVO_SPIN:
        return TRUE;
    default:
        return FALSE;
    }
}

// Conditions this Pokemon, in this game, can never meet.
static bool32 CanEverMeet(const struct Evolution *evo, struct Pokemon *mon)
{
    u32 j;

    for (j = 0; evo->params != NULL && evo->params[j].condition != CONDITIONS_END; j++)
    {
        const struct EvolutionParam *param = &evo->params[j];

        switch (param->condition)
        {
        case IF_REGION:
            if (param->arg1 != GetCurrentRegion())
                return FALSE;
            break;
        case IF_NOT_REGION:
            if (param->arg1 == GetCurrentRegion())
                return FALSE;
            break;
        case IF_GENDER:
            if (param->arg1 != GetMonGender(mon))
                return FALSE;
            break;
        default:
            break;
        }
    }
    return TRUE;
}

// Lower is easier to explain and to do: an item, then a level, then the rest,
// and a trade last of all.
static u32 GetWayRank(u16 method)
{
    switch (method)
    {
    case EVO_ITEM:  return 0;
    case EVO_LEVEL: return 1;
    case EVO_TRADE: return 3;
    default:        return 2;
    }
}

void WotEvoAdvice_Prepare(void)
{
    struct Pokemon *mon = &gPlayerParty[min(gSpecialVar_0x8004, PARTY_SIZE - 1)];
    const struct Evolution *evolutions;
    u32 i, k;

    sAdviceSpecies = GetMonData(mon, MON_DATA_SPECIES);
    sAdviceCount = 0;
    if (GetMonData(mon, MON_DATA_IS_EGG))
    {
        gSpecialVar_Result = 0xFF;
        return;
    }
    StringCopy(gStringVar1, GetSpeciesName(sAdviceSpecies));
    evolutions = GetSpeciesEvolutions(sAdviceSpecies);
    // One way per Pokemon it can become: the easiest one.
    for (i = 0; evolutions != NULL && evolutions[i].method != EVOLUTIONS_END && sAdviceCount < ADVICE_MAX; i++)
    {
        bool32 beaten = FALSE;

        if (!IsUsableMethod(evolutions[i].method) || !CanEverMeet(&evolutions[i], mon))
            continue;
        for (k = 0; evolutions[k].method != EVOLUTIONS_END; k++)
        {
            if (k == i || evolutions[k].targetSpecies != evolutions[i].targetSpecies
             || !IsUsableMethod(evolutions[k].method) || !CanEverMeet(&evolutions[k], mon))
                continue;
            if (GetWayRank(evolutions[k].method) < GetWayRank(evolutions[i].method)
             || (GetWayRank(evolutions[k].method) == GetWayRank(evolutions[i].method) && k < i))
                beaten = TRUE;
        }
        if (!beaten)
            sAdvice[sAdviceCount++] = i;
    }
    gSpecialVar_Result = sAdviceCount;
}

static void AppendNumber(u32 n)
{
    u8 buffer[8];

    ConvertIntToDecimalStringN(buffer, n, STR_CONV_MODE_LEFT_ALIGN, 5);
    StringAppend(gStringVar4, buffer);
}

static void AppendItemWithArticle(u16 item)
{
    const u8 *name = GetItemName(item);
    u8 first = name[0];

    if (first == CHAR_A || first == CHAR_E || first == CHAR_I || first == CHAR_O || first == CHAR_U)
        StringAppend(gStringVar4, COMPOUND_STRING("an "));
    else
        StringAppend(gStringVar4, COMPOUND_STRING("a "));
    StringAppend(gStringVar4, name);
}

static void AppendCondition(const struct EvolutionParam *param)
{
    u8 buffer[32];

    switch (param->condition)
    {
    case IF_TIME:
        switch (param->arg1)
        {
        case TIME_MORNING: StringAppend(gStringVar4, COMPOUND_STRING(" in the morning")); break;
        case TIME_DAY:     StringAppend(gStringVar4, COMPOUND_STRING(" during the day")); break;
        case TIME_EVENING: StringAppend(gStringVar4, COMPOUND_STRING(" in the evening")); break;
        case TIME_NIGHT:   StringAppend(gStringVar4, COMPOUND_STRING(" at night"));       break;
        }
        break;
    case IF_NOT_TIME:
        if (param->arg1 == TIME_NIGHT)
            StringAppend(gStringVar4, COMPOUND_STRING(" during the day"));
        else
            StringAppend(gStringVar4, COMPOUND_STRING(" at night"));
        break;
    case IF_MIN_FRIENDSHIP:
        StringAppend(gStringVar4, COMPOUND_STRING(" once it's truly attached to you"));
        break;
    case IF_ATK_GT_DEF:
        StringAppend(gStringVar4, COMPOUND_STRING(" if its Attack is higher than its Defense"));
        break;
    case IF_ATK_EQ_DEF:
        StringAppend(gStringVar4, COMPOUND_STRING(" if its Attack and Defense are equal"));
        break;
    case IF_ATK_LT_DEF:
        StringAppend(gStringVar4, COMPOUND_STRING(" if its Defense is higher than its Attack"));
        break;
    case IF_HOLD_ITEM:
        StringAppend(gStringVar4, I_USE_EVO_HELD_ITEMS_FROM_BAG ? COMPOUND_STRING(" with ") : COMPOUND_STRING(" while holding "));
        AppendItemWithArticle(param->arg1);
        if (I_USE_EVO_HELD_ITEMS_FROM_BAG)
            StringAppend(gStringVar4, COMPOUND_STRING(" in your Bag"));
        break;
    case IF_PID_UPPER_MODULO_10_GT:
    case IF_PID_UPPER_MODULO_10_EQ:
    case IF_PID_UPPER_MODULO_10_LT:
    case IF_PID_MODULO_100_GT:
    case IF_PID_MODULO_100_EQ:
    case IF_PID_MODULO_100_LT:
        StringAppend(gStringVar4, COMPOUND_STRING(", depending on the Pokémon itself"));
        break;
    case IF_MIN_BEAUTY:
        StringAppend(gStringVar4, COMPOUND_STRING(" once its Beauty is high"));
        break;
    case IF_MIN_COOLNESS:
        StringAppend(gStringVar4, COMPOUND_STRING(" once its Coolness is high"));
        break;
    case IF_MIN_SMARTNESS:
        StringAppend(gStringVar4, COMPOUND_STRING(" once its Smartness is high"));
        break;
    case IF_MIN_TOUGHNESS:
        StringAppend(gStringVar4, COMPOUND_STRING(" once its Toughness is high"));
        break;
    case IF_MIN_CUTENESS:
        StringAppend(gStringVar4, COMPOUND_STRING(" once its Cuteness is high"));
        break;
    case IF_SPECIES_IN_PARTY:
        StringAppend(gStringVar4, COMPOUND_STRING(" with "));
        StringAppend(gStringVar4, GetSpeciesName(param->arg1));
        StringAppend(gStringVar4, COMPOUND_STRING(" in your party"));
        break;
    case IF_IN_MAPSEC:
        StringAppend(gStringVar4, COMPOUND_STRING(" in "));
        GetMapName(buffer, param->arg1, 0);
        StringAppend(gStringVar4, buffer);
        break;
    case IF_IN_MAP:
        StringAppend(gStringVar4, COMPOUND_STRING(" in "));
        GetMapName(buffer, Overworld_GetMapHeaderByGroupAndId(param->arg1 >> 8, param->arg1 & 0xFF)->regionMapSectionId, 0);
        StringAppend(gStringVar4, buffer);
        break;
    case IF_KNOWS_MOVE:
        StringAppend(gStringVar4, COMPOUND_STRING(" while it knows "));
        StringAppend(gStringVar4, GetMoveName(param->arg1));
        break;
    case IF_TRADE_PARTNER_SPECIES:
        StringAppend(gStringVar4, COMPOUND_STRING(" for "));
        StringAppend(gStringVar4, GetSpeciesName(param->arg1));
        break;
    case IF_TYPE_IN_PARTY:
        StringAppend(gStringVar4, COMPOUND_STRING(" with a "));
        StringAppend(gStringVar4, gTypesInfo[param->arg1].name);
        StringAppend(gStringVar4, COMPOUND_STRING("-type in your party"));
        break;
    case IF_WEATHER:
        if (param->arg1 == WEATHER_RAIN || param->arg1 == WEATHER_RAIN_THUNDERSTORM || param->arg1 == WEATHER_DOWNPOUR)
            StringAppend(gStringVar4, COMPOUND_STRING(" in the rain"));
        else if (param->arg1 == WEATHER_FOG || param->arg1 == WEATHER_FOG_HORIZONTAL)
            StringAppend(gStringVar4, COMPOUND_STRING(" in fog"));
        else
            StringAppend(gStringVar4, COMPOUND_STRING(" in unusual weather"));
        break;
    case IF_KNOWS_MOVE_TYPE:
        StringAppend(gStringVar4, COMPOUND_STRING(" while it knows a "));
        StringAppend(gStringVar4, gTypesInfo[param->arg1].name);
        StringAppend(gStringVar4, COMPOUND_STRING("-type move"));
        break;
    case IF_NATURE:
        StringAppend(gStringVar4, COMPOUND_STRING(" if it has a "));
        StringAppend(gStringVar4, gNaturesInfo[param->arg1].name);
        StringAppend(gStringVar4, COMPOUND_STRING(" nature"));
        break;
    case IF_AMPED_NATURE:
        StringAppend(gStringVar4, COMPOUND_STRING(" (its nature decides which form)"));
        break;
    case IF_LOW_KEY_NATURE:
        StringAppend(gStringVar4, COMPOUND_STRING(" (its nature decides which form)"));
        break;
    case IF_RECOIL_DAMAGE_GE:
        StringAppend(gStringVar4, COMPOUND_STRING(" after taking "));
        AppendNumber(param->arg1);
        StringAppend(gStringVar4, COMPOUND_STRING(" HP of recoil"));
        break;
    case IF_CURRENT_DAMAGE_GE:
        StringAppend(gStringVar4, COMPOUND_STRING(" while it's down at least "));
        AppendNumber(param->arg1);
        StringAppend(gStringVar4, COMPOUND_STRING(" HP"));
        break;
    case IF_CRITICAL_HITS_GE:
        StringAppend(gStringVar4, COMPOUND_STRING(" after landing "));
        AppendNumber(param->arg1);
        StringAppend(gStringVar4, COMPOUND_STRING(" critical hits in one battle"));
        break;
    case IF_USED_MOVE_X_TIMES:
        StringAppend(gStringVar4, COMPOUND_STRING(" after using "));
        StringAppend(gStringVar4, GetMoveName(param->arg1));
        StringAppend(gStringVar4, COMPOUND_STRING(" "));
        AppendNumber(param->arg2);
        StringAppend(gStringVar4, COMPOUND_STRING(" times"));
        break;
    case IF_DEFEAT_X_WITH_ITEMS:
        StringAppend(gStringVar4, COMPOUND_STRING(" after defeating "));
        AppendNumber(param->arg3);
        StringAppend(gStringVar4, COMPOUND_STRING(" "));
        StringAppend(gStringVar4, GetSpeciesName(param->arg1));
        StringAppend(gStringVar4, COMPOUND_STRING(" holding "));
        StringAppend(gStringVar4, GetItemName(param->arg2));
        break;
    case IF_MIN_OVERWORLD_STEPS:
        StringAppend(gStringVar4, COMPOUND_STRING(" after you walk "));
        AppendNumber(param->arg1);
        StringAppend(gStringVar4, COMPOUND_STRING(" steps with it in the lead"));
        break;
    case IF_BAG_ITEM_COUNT:
        StringAppend(gStringVar4, COMPOUND_STRING(" with "));
        AppendNumber(param->arg2);
        StringAppend(gStringVar4, COMPOUND_STRING(" "));
        StringAppend(gStringVar4, GetItemName(param->arg1));
        StringAppend(gStringVar4, COMPOUND_STRING(" in your Bag"));
        break;
    default:
        break;
    }
}

// "You'll find a Razor Claw at ..." for every item the way needs.
static void AppendItemPlace(u16 item)
{
    const u8 *place = GetItemPlace(item);

    if (item == ITEM_NONE)
        return;
    StringAppend(gStringVar4, COMPOUND_STRING("\p"));
    if (place != NULL)
    {
        StringAppend(gStringVar4, COMPOUND_STRING("You'll find "));
        AppendItemWithArticle(item);
        StringAppend(gStringVar4, COMPOUND_STRING(" at "));
        StringAppend(gStringVar4, place);
        StringAppend(gStringVar4, COMPOUND_STRING(", dear."));
    }
    else
    {
        StringAppend(gStringVar4, COMPOUND_STRING("I've never seen "));
        AppendItemWithArticle(item);
        StringAppend(gStringVar4, COMPOUND_STRING(" for sale around here, I'm afraid."));
    }
}

void WotEvoAdvice_Get(void)
{
    const struct Evolution *evolutions = GetSpeciesEvolutions(sAdviceSpecies);
    const struct Evolution *evo;
    u32 j, level = 0;

    gStringVar4[0] = EOS;
    if (evolutions == NULL || gSpecialVar_0x8005 >= sAdviceCount)
        return;
    evo = &evolutions[sAdvice[gSpecialVar_0x8005]];

    StringAppend(gStringVar4, GetSpeciesName(sAdviceSpecies));
    StringAppend(gStringVar4, COMPOUND_STRING(" becomes "));
    StringAppend(gStringVar4, GetSpeciesName(evo->targetSpecies));
    switch (evo->method)
    {
    case EVO_LEVEL:
    case EVO_LEVEL_BATTLE_ONLY:
        if (evo->param > 1)
        {
            level = evo->param;
            StringAppend(gStringVar4, COMPOUND_STRING(" at Lv. "));
            AppendNumber(level);
        }
        else
        {
            StringAppend(gStringVar4, COMPOUND_STRING(" when it levels up"));
        }
        if (evo->method == EVO_LEVEL_BATTLE_ONLY)
            StringAppend(gStringVar4, COMPOUND_STRING(" in battle"));
        break;
    case EVO_ITEM:
        StringAppend(gStringVar4, COMPOUND_STRING(" when you use "));
        AppendItemWithArticle(evo->param);
        StringAppend(gStringVar4, COMPOUND_STRING(" on it"));
        break;
    case EVO_TRADE:
        StringAppend(gStringVar4, COMPOUND_STRING(" when it's traded"));
        break;
    case EVO_BATTLE_END:
        StringAppend(gStringVar4, COMPOUND_STRING(" after a battle"));
        break;
    case EVO_SPIN:
        StringAppend(gStringVar4, COMPOUND_STRING(" if you spin around with it following you"));
        break;
    }
    for (j = 0; evo->params != NULL && evo->params[j].condition != CONDITIONS_END; j++)
        AppendCondition(&evo->params[j]);
    StringAppend(gStringVar4, COMPOUND_STRING("."));

    // Where to get what it needs.
    if (evo->method == EVO_ITEM)
        AppendItemPlace(evo->param);
    for (j = 0; evo->params != NULL && evo->params[j].condition != CONDITIONS_END; j++)
    {
        if (evo->params[j].condition == IF_HOLD_ITEM || evo->params[j].condition == IF_BAG_ITEM_COUNT)
            AppendItemPlace(evo->params[j].arg1);
        else if (evo->params[j].condition == IF_DEFEAT_X_WITH_ITEMS)
            AppendItemPlace(evo->params[j].arg2);
    }
    if (evo->method == EVO_TRADE)
        StringAppend(gStringVar4, COMPOUND_STRING("\pYou'd need a friend to trade with, dear."));

    // The level cap.
    if (level > GetCurrentLevelCap())
    {
        StringAppend(gStringVar4, COMPOUND_STRING("\pYour level cap is Lv. "));
        AppendNumber(GetCurrentLevelCap());
        StringAppend(gStringVar4, COMPOUND_STRING(" for now, so that will have to wait."));
    }

    BreakStringAutomatic(gStringVar4, ADVICE_WIDTH, 2, FONT_NORMAL, SHOW_SCROLL_PROMPT);
}
