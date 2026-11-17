#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "string_util.h"
#include "test/test.h"
#include "constants/characters.h"

// The Tradewind widow's evolution advice (src/wot_evo_advice.c).

void WotEvoAdvice_Prepare(void);
void WotEvoAdvice_Get(void);

static bool32 Says(const u8 *needle)
{
    u32 i, j;

    // The text is wrapped, so a line break counts as a space.
    for (i = 0; gStringVar4[i] != EOS; i++)
    {
        for (j = 0; needle[j] != EOS && (gStringVar4[i + j] == needle[j]
              || (needle[j] == CHAR_SPACE && (gStringVar4[i + j] == CHAR_NEWLINE || gStringVar4[i + j] == CHAR_PROMPT_SCROLL))); j++)
            ;
        if (needle[j] == EOS)
            return TRUE;
    }
    return FALSE;
}

static u32 Ask(u16 species, u8 gender)
{
    u32 personality = (gender == MON_FEMALE) ? 0 : 0xFF;

    ZeroPlayerPartyMons();
    CreateMon(&gPlayerParty[0], species, 20, personality, OTID_STRUCT_PLAYER_ID);
    gSpecialVar_0x8004 = 0;
    WotEvoAdvice_Prepare();
    return gSpecialVar_Result;
}

static void Read(u32 way)
{
    gSpecialVar_0x8005 = way;
    WotEvoAdvice_Get();
}

TEST("(WoT) Evo advice: a level, and where an item is sold")
{
    EXPECT_EQ(Ask(SPECIES_LARVESTA, MON_MALE), 1);
    Read(0);
    EXPECT(Says(COMPOUND_STRING("Volcarona")));
    EXPECT(Says(COMPOUND_STRING("Lv. 36")));

    EXPECT_EQ(Ask(SPECIES_SNEASEL_HISUI, MON_MALE), 1);
    Read(0);
    EXPECT(Says(COMPOUND_STRING("Sneasler")));
    EXPECT(Says(COMPOUND_STRING("during the day")));
    EXPECT(Says(COMPOUND_STRING("Razor Claw")));
    EXPECT(Says(COMPOUND_STRING("department store")));
}

TEST("(WoT) Evo advice: a trade is left out when an item does the same job")
{
    EXPECT_EQ(Ask(SPECIES_KADABRA, MON_MALE), 1);
    Read(0);
    EXPECT(Says(COMPOUND_STRING("Linking Cord")));
    EXPECT(Says(COMPOUND_STRING("Black Market")));
}

TEST("(WoT) Evo advice: the other gender's branch and fully evolved Pokemon")
{
    EXPECT_EQ(Ask(SPECIES_KIRLIA, MON_FEMALE), 1);   // Gardevoir only, no Gallade
    Read(0);
    EXPECT(Says(COMPOUND_STRING("Gardevoir")));
    EXPECT(!Says(COMPOUND_STRING("Gallade")));

    EXPECT_EQ(Ask(SPECIES_KIRLIA, MON_MALE), 2);
    EXPECT_EQ(Ask(SPECIES_VOLCARONA, MON_MALE), 0);
    EXPECT_GE(Ask(SPECIES_EEVEE, MON_MALE), 8);
}
