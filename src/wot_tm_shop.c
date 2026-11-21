#include "global.h"
#include "event_data.h"
#include "shop.h"
#include "constants/items.h"
#include "constants/opponents.h"

// ---------------------------------------------------------------------------
// The TM floor of Tradewind's department store. TMs are reusable, so each one
// is bought once. Besides the floor's own stock, it carries every TM whose
// only placement was on a Hoenn map the story never visits, in three batches:
// from the start, after General Edwards (the end of Act 2), and after the
// White Tiger (Tower Base). VAR_0x8004 picks the clerk: 0 attack, 1 defense.
// ---------------------------------------------------------------------------

#define TRAINER_FLAG(trainer) (TRAINER_FLAGS_START + (trainer))

enum
{
    TM_SHOP_ALWAYS,
    TM_SHOP_AFTER_EDWARDS,
    TM_SHOP_AFTER_BLAKE,
};

struct WotTmShopItem
{
    u16 item;
    u8 batch;
};

static const struct WotTmShopItem sAttackStock[] =
{
    {ITEM_TM_FIRE_BLAST,    TM_SHOP_ALWAYS},
    {ITEM_TM_THUNDER,       TM_SHOP_ALWAYS},
    {ITEM_TM_BLIZZARD,      TM_SHOP_ALWAYS},
    {ITEM_TM_HYPER_BEAM,    TM_SHOP_ALWAYS},
    {ITEM_TM_ICE_BEAM,      TM_SHOP_ALWAYS},
    {ITEM_TM_PLAY_ROUGH,    TM_SHOP_ALWAYS},
    {ITEM_TM_WATER_PULSE,   TM_SHOP_ALWAYS},
    {ITEM_TM_BULLET_SEED,   TM_SHOP_ALWAYS},
    {ITEM_TM_HIDDEN_POWER,  TM_SHOP_ALWAYS},
    {ITEM_TM_RETURN,        TM_SHOP_ALWAYS},
    {ITEM_TM_FRUSTRATION,   TM_SHOP_ALWAYS},
    {ITEM_TM_BRICK_BREAK,   TM_SHOP_ALWAYS},
    {ITEM_TM_SHOCK_WAVE,    TM_SHOP_ALWAYS},
    {ITEM_TM_AERIAL_ACE,    TM_SHOP_ALWAYS},
    {ITEM_TM_FACADE,        TM_SHOP_ALWAYS},
    {ITEM_TM_SECRET_POWER,  TM_SHOP_ALWAYS},
    {ITEM_TM_THIEF,         TM_SHOP_ALWAYS},
    {ITEM_TM_DRAGON_CLAW,   TM_SHOP_AFTER_EDWARDS},
    {ITEM_TM_GIGA_DRAIN,    TM_SHOP_AFTER_EDWARDS},
    {ITEM_TM_IRON_TAIL,     TM_SHOP_AFTER_EDWARDS},
    {ITEM_TM_FLAMETHROWER,  TM_SHOP_AFTER_EDWARDS},
    {ITEM_TM_SLUDGE_BOMB,   TM_SHOP_AFTER_EDWARDS},
    {ITEM_TM_STEEL_WING,    TM_SHOP_AFTER_EDWARDS},
    {ITEM_TM_FOCUS_PUNCH,   TM_SHOP_AFTER_BLAKE},
    {ITEM_TM_SOLAR_BEAM,    TM_SHOP_AFTER_BLAKE},
    {ITEM_TM_OVERHEAT,      TM_SHOP_AFTER_BLAKE},
    {ITEM_AM_GUTS,          TM_SHOP_ALWAYS},
    {ITEM_AM_TECHNICIAN,    TM_SHOP_ALWAYS},
    {ITEM_AM_MOXIE,         TM_SHOP_ALWAYS},
    {ITEM_AM_ADAPTABILITY,  TM_SHOP_ALWAYS},
    {ITEM_AM_SHEER_FORCE,   TM_SHOP_ALWAYS},
    {ITEM_AM_TOUGH_CLAWS,   TM_SHOP_ALWAYS},
    {ITEM_AM_IRON_FIST,     TM_SHOP_ALWAYS},
    {ITEM_AM_TINTED_LENS,   TM_SHOP_ALWAYS},
    {ITEM_AM_SKILL_LINK,    TM_SHOP_ALWAYS},
    {ITEM_AM_PRANKSTER,     TM_SHOP_ALWAYS},
    {ITEM_AM_SPEED_BOOST,   TM_SHOP_ALWAYS},
    {ITEM_AM_NO_GUARD,      TM_SHOP_ALWAYS},
    {ITEM_AM_CONTRARY,      TM_SHOP_ALWAYS},
    {ITEM_AM_PROTEAN,       TM_SHOP_ALWAYS},
    {ITEM_AM_DESOLATE_LAND, TM_SHOP_ALWAYS},
    {ITEM_AM_PRIMORDIAL_SEA, TM_SHOP_ALWAYS},
};

static const struct WotTmShopItem sDefenseStock[] =
{
    {ITEM_TM_PROTECT,       TM_SHOP_ALWAYS},
    {ITEM_TM_SAFEGUARD,     TM_SHOP_ALWAYS},
    {ITEM_TM_REFLECT,       TM_SHOP_ALWAYS},
    {ITEM_TM_LIGHT_SCREEN,  TM_SHOP_ALWAYS},
    {ITEM_TM_TAUNT,         TM_SHOP_ALWAYS},
    {ITEM_TM_TOXIC,         TM_SHOP_ALWAYS},
    {ITEM_TM_ROAR,          TM_SHOP_ALWAYS},
    {ITEM_TM_REST,          TM_SHOP_ALWAYS},
    {ITEM_TM_RAIN_DANCE,    TM_SHOP_ALWAYS},
    {ITEM_TM_SUNNY_DAY,     TM_SHOP_ALWAYS},
    {ITEM_TM_HAIL,          TM_SHOP_ALWAYS},
    {ITEM_TM_SANDSTORM,     TM_SHOP_ALWAYS},
    {ITEM_TM_DOUBLE_TEAM,   TM_SHOP_ALWAYS},
    {ITEM_TM_ATTRACT,       TM_SHOP_ALWAYS},
    {ITEM_TM_TORMENT,       TM_SHOP_ALWAYS},
    {ITEM_TM_CALM_MIND,     TM_SHOP_AFTER_EDWARDS},
    {ITEM_TM_BULK_UP,       TM_SHOP_AFTER_EDWARDS},
    {ITEM_TM_SKILL_SWAP,    TM_SHOP_AFTER_EDWARDS},
    {ITEM_TM_SNATCH,        TM_SHOP_AFTER_EDWARDS},
    {ITEM_AM_INTIMIDATE,    TM_SHOP_ALWAYS},
    {ITEM_AM_THICK_FAT,     TM_SHOP_ALWAYS},
    {ITEM_AM_STURDY,        TM_SHOP_ALWAYS},
    {ITEM_AM_LEVITATE,      TM_SHOP_ALWAYS},
    {ITEM_AM_UNAWARE,       TM_SHOP_ALWAYS},
    {ITEM_AM_SERENE_GRACE,  TM_SHOP_ALWAYS},
    {ITEM_AM_POISON_HEAL,   TM_SHOP_ALWAYS},
    {ITEM_AM_MULTISCALE,    TM_SHOP_ALWAYS},
    {ITEM_AM_MAGIC_GUARD,   TM_SHOP_ALWAYS},
    {ITEM_AM_REGENERATOR,   TM_SHOP_ALWAYS},
    {ITEM_AM_MAGIC_BOUNCE,  TM_SHOP_ALWAYS},
};

#define TM_SHOP_STOCK_MAX (ARRAY_COUNT(sAttackStock) > ARRAY_COUNT(sDefenseStock) ? ARRAY_COUNT(sAttackStock) : ARRAY_COUNT(sDefenseStock))

static EWRAM_DATA u16 sTmShopList[TM_SHOP_STOCK_MAX + 1] = {0};

static u32 GetTmShopBatch(void)
{
    if (FlagGet(TRAINER_FLAG(TRAINER_BLAKE)))
        return TM_SHOP_AFTER_BLAKE;
    if (FlagGet(TRAINER_FLAG(TRAINER_SENNEN_CAPTAIN_KANNON)))
        return TM_SHOP_AFTER_EDWARDS;
    return TM_SHOP_ALWAYS;
}

// The clerk's list as it stands now. Exposed for the tests.
const u16 *WotTmShop_BuildStock(u32 clerk)
{
    const struct WotTmShopItem *stock = (clerk == 0) ? sAttackStock : sDefenseStock;
    u32 count = (clerk == 0) ? ARRAY_COUNT(sAttackStock) : ARRAY_COUNT(sDefenseStock);
    u32 batch = GetTmShopBatch();
    u32 i, n = 0;

    for (i = 0; i < count; i++)
    {
        if (stock[i].batch <= batch)
            sTmShopList[n++] = stock[i].item;
    }
    sTmShopList[n] = ITEM_NONE;
    return sTmShopList;
}

// Waits like pokemart (data/specials.inc).
void WotTmShop_Open(void)
{
    CreatePokemartMenu(WotTmShop_BuildStock(gSpecialVar_0x8004));
}
