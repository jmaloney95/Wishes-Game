#ifndef GUARD_HALL_OF_FAME_H
#define GUARD_HALL_OF_FAME_H

struct HallofFameMon
{
    u32 tid;
    u32 personality;
    u16 isShiny:1;
    u16 species:15;
    // WoT: bit 7 of lvl carries shadow-ness. MAX_LEVEL is 100, so the
    // level itself needs 7 bits, and stealing the spare keeps the struct
    // the same size -- growing it would change how many teams fit in the
    // save sectors and invalidate every record already stored. Records
    // written before this have the bit clear, which reads as "not shadow".
    u8 lvl:7;
    u8 isShadow:1;
    u8 nickname[POKEMON_NAME_LENGTH];
};

struct HallofFameTeam
{
    struct HallofFameMon mon[PARTY_SIZE];
};

extern struct HallofFameTeam *gHoFSaveBuffer;

void CB2_DoHallOfFameScreen(void);
void CB2_DoHallOfFameScreenDontSaveData(void);
void CB2_DoHallOfFamePC(void);

// hof_pc.c
void ReturnFromHallOfFamePC(void);

#endif // GUARD_HALL_OF_FAME_H
