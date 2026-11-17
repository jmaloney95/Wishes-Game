#include "global.h"
#include "data.h"
#include "move.h"
#include "pokeball.h"
#include "pokemon.h"
#include "wot_mega_dimension.h"
#include "test/test.h"
#include "constants/abilities.h"
#include "constants/battle_ai.h"
#include "constants/battle_partner.h"
#include "constants/items.h"
#include "constants/moves.h"

// WoT: the Mega Dimension partners have real movesets. They used to
// get the last four level-up moves at Lv30, kept however high they were
// re-levelled; Canari's Toxtricity had nothing that dealt damage but Venoshock.

#define WOT_PARTNERS       6
#define WOT_PARTNER_MONS   3
#define PARTNER_FIRST_SLOT 3    // the partner's half of gPlayerParty

// Test builds swap gBattlePartners for their own, so read the game's table.
static const struct Trainer sGamePartners[DIFFICULTY_COUNT][PARTNER_COUNT] =
{
#include "../src/data/battle_partners.h"
};

TEST("(WoT) Every DLC partner Pokemon has four moves and at least two attacks")
{
    u32 i, slot, m;

    for (i = 0; i < WOT_PARTNERS; i++)
    {
        const struct Trainer *partner = &sGamePartners[DIFFICULTY_NORMAL][PARTNER_MEGADIM_CANARI + i];

        EXPECT_EQ(partner->partySize, WOT_PARTNER_MONS);
        for (slot = 0; slot < WOT_PARTNER_MONS; slot++)
        {
            u32 attacks = 0;

            for (m = 0; m < MAX_MON_MOVES; m++)
            {
                u32 move = partner->party[slot].moves[m];

                EXPECT_NE(move, MOVE_NONE);
                if (GetMoveCategory(move) != DAMAGE_CATEGORY_STATUS)
                    attacks++;
            }
            EXPECT_GE(attacks, 2);
        }
    }
}

// The player picks these moves in a double battle beside their own Pokemon.
TEST("(WoT) No DLC partner move hits the player's own Pokemon or switches out")
{
    u32 i, slot, m;

    for (i = 0; i < WOT_PARTNERS; i++)
    {
        const struct Trainer *partner = &sGamePartners[DIFFICULTY_NORMAL][PARTNER_MEGADIM_CANARI + i];

        for (slot = 0; slot < WOT_PARTNER_MONS; slot++)
        {
            for (m = 0; m < MAX_MON_MOVES; m++)
            {
                u32 move = partner->party[slot].moves[m];

                EXPECT_NE(GetMoveTarget(move), TARGET_FOES_AND_ALLY);
                EXPECT_NE(GetMoveEffect(move), EFFECT_HIT_ESCAPE);
            }
        }
    }
}

TEST("(WoT) Moves saved by older builds don't override a partner's set")
{
    struct Pokemon *toxtricity = &gPlayerParty[PARTNER_FIRST_SLOT + 2];
    u32 m;

    ZeroMonData(&gPlayerParty[PARTNER_FIRST_SLOT]);
    ZeroMonData(&gPlayerParty[PARTNER_FIRST_SLOT + 1]);
    CreateMon(toxtricity, SPECIES_TOXTRICITY_AMPED, 30, 0, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(toxtricity, MOVE_OVERDRIVE, 0);

    // An old save: Canari's Toxtricity stored with its leftovers, no marker.
    gSaveBlock1Ptr->wotPartners[0].sanctum = 0;
    gSaveBlock1Ptr->wotPartners[0].order = 0;
    for (m = 0; m < MAX_MON_MOVES; m++)
        gSaveBlock1Ptr->wotPartners[0].moves[2][m] = MOVE_SCREECH;
    WotPartner_BuildTeam(0, 50, FALSE);
    EXPECT_EQ(GetMonData(toxtricity, MON_DATA_MOVE1), MOVE_OVERDRIVE);

    // Saved from now on: kept, and the sanctum bits are untouched.
    gSaveBlock1Ptr->wotPartners[0].sanctum = 1;
    SetMonMoveSlot(toxtricity, MOVE_THUNDERBOLT, 0);
    WotPartner_SaveMoves(0, 2, toxtricity);
    SetMonMoveSlot(toxtricity, MOVE_OVERDRIVE, 0);
    WotPartner_BuildTeam(0, 50, FALSE);
    EXPECT_EQ(GetMonData(toxtricity, MON_DATA_MOVE1), MOVE_THUNDERBOLT);
    EXPECT(gSaveBlock1Ptr->wotPartners[0].sanctum & 1);

    gSaveBlock1Ptr->wotPartners[0].sanctum = 0;
    for (m = 0; m < MAX_MON_MOVES; m++)
        gSaveBlock1Ptr->wotPartners[0].moves[2][m] = MOVE_NONE;
}
