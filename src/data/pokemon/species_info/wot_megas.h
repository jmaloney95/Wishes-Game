// Wishes of Tomorrow custom Mega Evolutions.
// Art: Joe's MEGA SPRITES pack (80x80 scaled to 64x64, tools/build_mega_art.py).
// Shiny palettes take the base species' shiny shift (tools/build_mega_shiny.py).
// Stats are a first pass: HP unchanged, +100 base stat total, like canon Megas.
// Party icons and overworld sprites reuse the base species'.
// The stones are sold by the Minister (src/wot_mega_shop.c reads the form-change tables).

#if P_FAMILY_FOONGUS && P_MEGA_EVOLUTIONS
    [SPECIES_AMOONGUSS_MEGA] =
    {
        .baseHP        = 114,
        .baseAttack    = 95,
        .baseDefense   = 110,
        .baseSpeed     = 30,
        .baseSpAttack  = 105,
        .baseSpDefense = 110,
        .types = MON_TYPES(TYPE_GRASS, TYPE_POISON),
        .catchRate = 75,
        .expYield = 197,
        .evYield_HP = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_GRASS),
        .abilities = { ABILITY_TOXIC_DEBRIS, ABILITY_TOXIC_DEBRIS, ABILITY_TOXIC_DEBRIS },
        .bodyColor = BODY_COLOR_PURPLE,
        .speciesName = _("Amoonguss"),
        .cryId = CRY_AMOONGUSS,
        .natDexNum = NATIONAL_DEX_AMOONGUSS,
        .categoryName = _("Mushroom"),
        .height = 13,
        .weight = 520,
        .description = COMPOUND_STRING(
            "Mega Evolution swells its cap into a\n"
            "spiked dome. When struck, it sheds\n"
            "poison spikes that litter the ground\n"
            "around its foes."),
        .pokemonScale = 422,
        .pokemonOffset = 13,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_AmoongussMega,
        .frontPicSize = MON_COORDS_SIZE(62, 51),
        .frontPicYOffset = 7,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_H_SLIDE,
        .backPic = gMonBackPic_AmoongussMega,
        .backPicSize = MON_COORDS_SIZE(64, 53),
        .backPicYOffset = 6,
        .backAnimId = BACK_ANIM_GROW_STUTTER,
        .palette = gMonPalette_AmoongussMega,
        .shinyPalette = gMonShinyPalette_AmoongussMega,
        .iconSprite = gMonIcon_Amoonguss,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-2, 6, SHADOW_SIZE_L)
        FOOTPRINT(Amoonguss)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Amoonguss,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following_Asym,
            gOverworldPalette_Amoonguss,
            gShinyOverworldPalette_Amoonguss
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sAmoongussLevelUpLearnset,
        .teachableLearnset = sAmoongussTeachableLearnset,
        .formSpeciesIdTable = sAmoongussFormSpeciesIdTable,
        .formChangeTable = sAmoongussFormChangeTable,
    },
#endif //P_FAMILY_FOONGUS && P_MEGA_EVOLUTIONS

#if P_FAMILY_MARILL && P_MEGA_EVOLUTIONS
    [SPECIES_AZUMARILL_MEGA] =
    {
        .baseHP        = 100,
        .baseAttack    = 90,
        .baseDefense   = 100,
        .baseSpeed     = 60,
        .baseSpAttack  = 70,
        .baseSpDefense = 100,
        .types = MON_TYPES(TYPE_WATER, TYPE_FAIRY),
        .catchRate = 75,
        .expYield = 245,
        .evYield_HP = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 10,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_1, EGG_GROUP_FAIRY),
        .abilities = { ABILITY_WATER_BUBBLE, ABILITY_WATER_BUBBLE, ABILITY_WATER_BUBBLE },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Azumarill"),
        .cryId = CRY_AZUMARILL,
        .natDexNum = NATIONAL_DEX_AZUMARILL,
        .categoryName = _("Aqua Rabbit"),
        .height = 12,
        .weight = 610,
        .description = COMPOUND_STRING(
            "Its body holds so much water that it\n"
            "floats like a balloon. A coat of\n"
            "bubbles shields it from flames and\n"
            "doubles the force of its water moves."),
        .pokemonScale = 448,
        .pokemonOffset = 16,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_AzumarillMega,
        .frontPicSize = MON_COORDS_SIZE(53, 60),
        .frontPicYOffset = 2,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = P_GBA_STYLE_SPECIES_GFX ? ANIM_BOUNCE_ROTATE_TO_SIDES_SMALL_SLOW : ANIM_SHRINK_GROW,
        .backPic = gMonBackPic_AzumarillMega,
        .backPicSize = MON_COORDS_SIZE(62, 56),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_DIP_RIGHT_SIDE,
        .palette = gMonPalette_AzumarillMega,
        .shinyPalette = gMonShinyPalette_AzumarillMega,
        .iconSprite = gMonIcon_Azumarill,
        .iconPalIndex = P_GBA_STYLE_SPECIES_ICONS ? 2 : 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-2, 11, SHADOW_SIZE_L)
        FOOTPRINT(Azumarill)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Azumarill,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Azumarill,
            gShinyOverworldPalette_Azumarill
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sAzumarillLevelUpLearnset,
        .teachableLearnset = sAzumarillTeachableLearnset,
        .formSpeciesIdTable = sAzumarillFormSpeciesIdTable,
        .formChangeTable = sAzumarillFormChangeTable,
    },
#endif //P_FAMILY_MARILL && P_MEGA_EVOLUTIONS

#if P_FAMILY_CATERPIE && P_MEGA_EVOLUTIONS
    [SPECIES_BUTTERFREE_MEGA] =
    {
        .baseHP        = 60,
        .baseAttack    = 45,
        .baseDefense   = 70,
        .baseSpeed     = 100,
        .baseSpAttack  = 120,
        .baseSpDefense = 100,
        .types = MON_TYPES(TYPE_BUG, TYPE_PSYCHIC),
        .catchRate = 45,
        .expYield = 233,
        .evYield_SpAttack = 2,
        .evYield_SpDefense = 1,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_BUG),
        .abilities = { ABILITY_PSYCHIC_SURGE, ABILITY_PSYCHIC_SURGE, ABILITY_PSYCHIC_SURGE },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Butterfree"),
        .cryId = CRY_BUTTERFREE,
        .natDexNum = NATIONAL_DEX_BUTTERFREE,
        .categoryName = _("Butterfly"),
        .height = 15,
        .weight = 385,
        .description = COMPOUND_STRING(
            "Its wings turn to stained glass that\n"
            "bends light into psychic waves. The\n"
            "air around it hums, and its foes\n"
            "can no longer strike first."),
        .pokemonScale = 312,
        .pokemonOffset = 2,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_ButterfreeMega,
        .frontPicSize = MON_COORDS_SIZE(62, 62),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = P_GBA_STYLE_SPECIES_GFX ? ANIM_H_SLIDE_WOBBLE : ANIM_V_SLIDE_WOBBLE,
        .enemyMonElevation = 10,
        .backPic = gMonBackPic_ButterfreeMega,
        .backPicSize = MON_COORDS_SIZE(63, 62),
        .backPicYOffset = 1,
        .backAnimId = BACK_ANIM_CONVEX_DOUBLE_ARC,
        .palette = gMonPalette_ButterfreeMega,
        .shinyPalette = gMonShinyPalette_ButterfreeMega,
        .iconSprite = gMonIcon_Butterfree,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(2, 17, SHADOW_SIZE_L)
        FOOTPRINT(Butterfree)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Butterfree,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Butterfree,
            gShinyOverworldPalette_Butterfree
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sButterfreeLevelUpLearnset,
        .teachableLearnset = sButterfreeTeachableLearnset,
        .formSpeciesIdTable = sButterfreeFormSpeciesIdTable,
        .formChangeTable = sButterfreeFormChangeTable,
    },
#endif //P_FAMILY_CATERPIE && P_MEGA_EVOLUTIONS

#if P_FAMILY_TIMBURR && P_MEGA_EVOLUTIONS
    [SPECIES_CONKELDURR_MEGA] =
    {
        .baseHP        = 105,
        .baseAttack    = 170,
        .baseDefense   = 125,
        .baseSpeed     = 55,
        .baseSpAttack  = 55,
        .baseSpDefense = 95,
        .types = MON_TYPES(TYPE_FIGHTING, TYPE_ROCK),
        .catchRate = 45,
        .expYield = 288,
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(25),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_HUMAN_LIKE),
        .abilities = { ABILITY_ROCKY_PAYLOAD, ABILITY_ROCKY_PAYLOAD, ABILITY_ROCKY_PAYLOAD },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Conkeldurr"),
        .cryId = CRY_CONKELDURR,
        .natDexNum = NATIONAL_DEX_CONKELDURR,
        .categoryName = _("Muscular"),
        .height = 16,
        .weight = 1290,
        .description = COMPOUND_STRING(
            "It swings a whole tower of stone as\n"
            "lightly as a stick. Any rock it\n"
            "throws strikes with the weight of\n"
            "a falling building."),
        .pokemonScale = 265,
        .pokemonOffset = 3,
        .trainerScale = 262,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_ConkeldurrMega,
        .frontPicSize = MON_COORDS_SIZE(62, 62),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_SHAKE_TWICE,
        .backPic = gMonBackPic_ConkeldurrMega,
        .backPicSize = MON_COORDS_SIZE(62, 62),
        .backPicYOffset = 1,
        .backAnimId = BACK_ANIM_V_SHAKE_H_SLIDE,
        .palette = gMonPalette_ConkeldurrMega,
        .shinyPalette = gMonShinyPalette_ConkeldurrMega,
        .iconSprite = gMonIcon_Conkeldurr,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(12, 12, SHADOW_SIZE_L)
        FOOTPRINT(Conkeldurr)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Conkeldurr,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Conkeldurr,
            gShinyOverworldPalette_Conkeldurr
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sConkeldurrLevelUpLearnset,
        .teachableLearnset = sConkeldurrTeachableLearnset,
        .formSpeciesIdTable = sConkeldurrFormSpeciesIdTable,
        .formChangeTable = sConkeldurrFormChangeTable,
    },
#endif //P_FAMILY_TIMBURR && P_MEGA_EVOLUTIONS

#if P_FAMILY_DIGLETT && P_MEGA_EVOLUTIONS
    [SPECIES_DUGTRIO_MEGA] =
    {
        .baseHP        = 35,
        .baseAttack    = 135,
        .baseDefense   = 80,
        .baseSpeed     = 135,
        .baseSpAttack  = 50,
        .baseSpDefense = 90,
        .types = MON_TYPES(TYPE_GROUND, TYPE_FIGHTING),
        .catchRate = 50,
        .expYield = 184,
        .evYield_Speed = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_EARTH_EATER, ABILITY_EARTH_EATER, ABILITY_EARTH_EATER },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Dugtrio"),
        .cryId = CRY_DUGTRIO,
        .natDexNum = NATIONAL_DEX_DUGTRIO,
        .categoryName = _("Mole"),
        .height = 16,
        .weight = 1800,
        .description = COMPOUND_STRING(
            "Mega Evolution raises its three heads\n"
            "on a brawler's body of packed earth.\n"
            "It feeds on the soil, so Ground-type\n"
            "attacks only heal it."),
        .pokemonScale = 406,
        .pokemonOffset = 18,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_DugtrioMega,
        .frontPicSize = MON_COORDS_SIZE(61, 54),
        .frontPicYOffset = 4,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_H_SHAKE_SLOW,
        .backPic = gMonBackPic_DugtrioMega,
        .backPicSize = MON_COORDS_SIZE(64, 42),
        .backPicYOffset = 10,
        .backAnimId = BACK_ANIM_V_SHAKE,
        .palette = gMonPalette_DugtrioMega,
        .shinyPalette = gMonShinyPalette_DugtrioMega,
        .iconSprite = gMonIcon_Dugtrio,
        .iconPalIndex = 2,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-1, 9, SHADOW_SIZE_L)
        FOOTPRINT(Dugtrio)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Dugtrio,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_NONE,
            sAnimTable_Following,
            gOverworldPalette_Dugtrio,
            gShinyOverworldPalette_Dugtrio
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sDugtrioLevelUpLearnset,
        .teachableLearnset = sDugtrioTeachableLearnset,
        .formSpeciesIdTable = sDugtrioFormSpeciesIdTable,
        .formChangeTable = sDugtrioFormChangeTable,
    },
#endif //P_FAMILY_DIGLETT && P_MEGA_EVOLUTIONS

#if P_FAMILY_ELECTABUZZ && P_MEGA_EVOLUTIONS
    [SPECIES_ELECTIVIRE_MEGA] =
    {
        .baseHP        = 75,
        .baseAttack    = 153,
        .baseDefense   = 97,
        .baseSpeed     = 110,
        .baseSpAttack  = 105,
        .baseSpDefense = 100,
        .types = MON_TYPES(TYPE_ELECTRIC),
        .catchRate = 30,
        .expYield = 305,
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(25),
        .eggCycles = 25,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_HUMAN_LIKE),
        .abilities = { ABILITY_ELECTRIC_SURGE, ABILITY_ELECTRIC_SURGE, ABILITY_ELECTRIC_SURGE },
        .bodyColor = BODY_COLOR_YELLOW,
        .speciesName = _("Electivire"),
        .cryId = CRY_ELECTIVIRE,
        .natDexNum = NATIONAL_DEX_ELECTIVIRE,
        .categoryName = _("Thunderbolt"),
        .height = 20,
        .weight = 1650,
        .description = COMPOUND_STRING(
            "The coils on its back hum with so\n"
            "much current that the ground around\n"
            "it crackles. Nothing can fall asleep\n"
            "while it is near."),
        .pokemonScale = 267,
        .pokemonOffset = 2,
        .trainerScale = 286,
        .trainerOffset = 1,
        .frontPic = gMonFrontPic_ElectivireMega,
        .frontPicSize = MON_COORDS_SIZE(60, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_GLOW_YELLOW,
        .backPic = gMonBackPic_ElectivireMega,
        .backPicSize = MON_COORDS_SIZE(64, 60),
        .backPicYOffset = 2,
        .backAnimId = BACK_ANIM_SHAKE_FLASH_YELLOW,
        .palette = gMonPalette_ElectivireMega,
        .shinyPalette = gMonShinyPalette_ElectivireMega,
        .iconSprite = gMonIcon_Electivire,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(12, 13, SHADOW_SIZE_L)
        FOOTPRINT(Electivire)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Electivire,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Electivire,
            gShinyOverworldPalette_Electivire
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sElectivireLevelUpLearnset,
        .teachableLearnset = sElectivireTeachableLearnset,
        .formSpeciesIdTable = sElectivireFormSpeciesIdTable,
        .formChangeTable = sElectivireFormChangeTable,
    },
#endif //P_FAMILY_ELECTABUZZ && P_MEGA_EVOLUTIONS

#if P_FAMILY_TRAPINCH && P_MEGA_EVOLUTIONS
    [SPECIES_FLYGON_MEGA] =
    {
        .baseHP        = 80,
        .baseAttack    = 130,
        .baseDefense   = 100,
        .baseSpeed     = 130,
        .baseSpAttack  = 90,
        .baseSpDefense = 90,
        .types = MON_TYPES(TYPE_GROUND, TYPE_DRAGON),
        .catchRate = 45,
        .expYield = 295,
        .evYield_Attack = 1,
        .evYield_Speed = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = TRAPINCH_FAMILY_EGG_GROUPS,
        .abilities = { ABILITY_SAND_STREAM, ABILITY_SAND_STREAM, ABILITY_SAND_STREAM },
        .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Flygon"),
        .cryId = CRY_FLYGON,
        .natDexNum = NATIONAL_DEX_FLYGON,
        .categoryName = _("Mystic"),
        .height = 23,
        .weight = 950,
        .description = COMPOUND_STRING(
            "Each beat of its wings whips up a\n"
            "sandstorm that hides it from sight.\n"
            "The song of its wings is the only\n"
            "sound that carries through the storm."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 268,
        .trainerOffset = 1,
        .frontPic = gMonFrontPic_FlygonMega,
        .frontPicSize = MON_COORDS_SIZE(64, 63),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_ZIGZAG_SLOW,
        .enemyMonElevation = 7,
        .backPic = gMonBackPic_FlygonMega,
        .backPicSize = MON_COORDS_SIZE(60, 60),
        .backPicYOffset = 2,
        .backAnimId = BACK_ANIM_TRIANGLE_DOWN,
        .palette = gMonPalette_FlygonMega,
        .shinyPalette = gMonShinyPalette_FlygonMega,
        .iconSprite = gMonIcon_Flygon,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(1, 17, SHADOW_SIZE_L)
        FOOTPRINT(Flygon)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Flygon,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_NONE,
            sAnimTable_Following,
            gOverworldPalette_Flygon,
            gShinyOverworldPalette_Flygon
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sFlygonLevelUpLearnset,
        .teachableLearnset = sFlygonTeachableLearnset,
        .formSpeciesIdTable = sFlygonFormSpeciesIdTable,
        .formChangeTable = sFlygonFormChangeTable,
    },
#endif //P_FAMILY_TRAPINCH && P_MEGA_EVOLUTIONS

#if P_FAMILY_GEODUDE && P_MEGA_EVOLUTIONS
    [SPECIES_GOLEM_MEGA] =
    {
        .baseHP        = 80,
        .baseAttack    = 150,
        .baseDefense   = 160,
        .baseSpeed     = 55,
        .baseSpAttack  = 55,
        .baseSpDefense = 95,
        .types = MON_TYPES(TYPE_ROCK, TYPE_GROUND),
        .catchRate = 45,
        .expYield = 283,
        .evYield_Defense = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_MINERAL),
        .abilities = { ABILITY_SOLID_ROCK, ABILITY_SOLID_ROCK, ABILITY_SOLID_ROCK },
        .bodyColor = BODY_COLOR_GRAY,
        .speciesName = _("Golem"),
        .cryId = CRY_GOLEM,
        .natDexNum = NATIONAL_DEX_GOLEM,
        .categoryName = _("Megaton"),
        .height = 16,
        .weight = 3800,
        .description = COMPOUND_STRING(
            "Crystals as hard as diamond cover its\n"
            "body. Even attacks it is weak to\n"
            "barely scratch it. It sheds its old\n"
            "crystals once a year."),
        .pokemonScale = 256,
        .pokemonOffset = 3,
        .trainerScale = 296,
        .trainerOffset = 2,
        .frontPic = gMonFrontPic_GolemMega,
        .frontPicSize = MON_COORDS_SIZE(63, 61),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_ROTATE_UP_SLAM_DOWN,
        .backPic = gMonBackPic_GolemMega,
        .backPicSize = MON_COORDS_SIZE(64, 50),
        .backPicYOffset = 8,
        .backAnimId = BACK_ANIM_H_SHAKE,
        .palette = gMonPalette_GolemMega,
        .shinyPalette = gMonShinyPalette_GolemMega,
        .iconSprite = gMonIcon_Golem,
        .iconPalIndex = P_GBA_STYLE_SPECIES_ICONS ? 1 : 2,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(11, 12, SHADOW_SIZE_L)
        FOOTPRINT(Golem)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Golem,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Golem,
            gShinyOverworldPalette_Golem
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sGolemLevelUpLearnset,
        .teachableLearnset = sGolemTeachableLearnset,
        .formSpeciesIdTable = sGolemFormSpeciesIdTable,
        .formChangeTable = sGolemFormChangeTable,
    },
#endif //P_FAMILY_GEODUDE && P_MEGA_EVOLUTIONS

#if P_FAMILY_HOPPIP && P_MEGA_EVOLUTIONS
    [SPECIES_JUMPLUFF_MEGA] =
    {
        .baseHP        = 75,
        .baseAttack    = 65,
        .baseDefense   = 100,
        .baseSpeed     = 120,
        .baseSpAttack  = 75,
        .baseSpDefense = 125,
        .types = MON_TYPES(TYPE_GRASS, TYPE_FLYING),
        .catchRate = 45,
        .expYield = 265,
        .evYield_Speed = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FAIRY, EGG_GROUP_GRASS),
        .abilities = { ABILITY_COTTON_DOWN, ABILITY_COTTON_DOWN, ABILITY_COTTON_DOWN },
        .bodyColor = BODY_COLOR_YELLOW,
        .speciesName = _("Jumpluff"),
        .cryId = CRY_JUMPLUFF,
        .natDexNum = NATIONAL_DEX_JUMPLUFF,
        .categoryName = _("Cottonweed"),
        .height = 14,
        .weight = 40,
        .description = COMPOUND_STRING(
            "Its cotton swells into a huge ball\n"
            "that rides the wind. Anything that\n"
            "strikes it is wrapped in cotton\n"
            "that slows it down."),
        .pokemonScale = 418,
        .pokemonOffset = -4,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_JumpluffMega,
        .frontPicSize = MON_COORDS_SIZE(59, 55),
        .frontPicYOffset = 5,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_SLIDE_WOBBLE_SMALL,
        .enemyMonElevation = 9,
        .backPic = gMonBackPic_JumpluffMega,
        .backPicSize = MON_COORDS_SIZE(64, 50),
        .backPicYOffset = 6,
        .backAnimId = BACK_ANIM_CONVEX_DOUBLE_ARC,
        .palette = gMonPalette_JumpluffMega,
        .shinyPalette = gMonShinyPalette_JumpluffMega,
        .iconSprite = gMonIcon_Jumpluff,
        .iconPalIndex = 2,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(2, 13, SHADOW_SIZE_L)
        FOOTPRINT(Jumpluff)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Jumpluff,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_NONE,
            sAnimTable_Following,
            gOverworldPalette_Jumpluff,
            gShinyOverworldPalette_Jumpluff
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sJumpluffLevelUpLearnset,
        .teachableLearnset = sJumpluffTeachableLearnset,
        .formSpeciesIdTable = sJumpluffFormSpeciesIdTable,
        .formChangeTable = sJumpluffFormChangeTable,
    },
#endif //P_FAMILY_HOPPIP && P_MEGA_EVOLUTIONS

#if P_FAMILY_KABUTO && P_MEGA_EVOLUTIONS
    [SPECIES_KABUTOPS_MEGA] =
    {
        .baseHP        = 60,
        .baseAttack    = 150,
        .baseDefense   = 125,
        .baseSpeed     = 105,
        .baseSpAttack  = 65,
        .baseSpDefense = 90,
        .types = MON_TYPES(TYPE_ROCK, TYPE_WATER),
        .catchRate = 45,
        .expYield = 208,
        .evYield_Attack = 2,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 30,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_1, EGG_GROUP_WATER_3),
        .abilities = { ABILITY_SHARPNESS, ABILITY_SHARPNESS, ABILITY_SHARPNESS },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Kabutops"),
        .cryId = CRY_KABUTOPS,
        .natDexNum = NATIONAL_DEX_KABUTOPS,
        .categoryName = _("Shellfish"),
        .height = 16,
        .weight = 580,
        .description = COMPOUND_STRING(
            "Its scythes have been honed to an\n"
            "edge that cuts through steel. It\n"
            "slashes its prey in a flash, then\n"
            "drags it under the water."),
        .pokemonScale = 271,
        .pokemonOffset = 3,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_KabutopsMega,
        .frontPicSize = MON_COORDS_SIZE(64, 58),
        .frontPicYOffset = 3,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_H_SHAKE,
        .backPic = gMonBackPic_KabutopsMega,
        .backPicSize = MON_COORDS_SIZE(64, 58),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_JOLT_RIGHT,
        .palette = gMonPalette_KabutopsMega,
        .shinyPalette = gMonShinyPalette_KabutopsMega,
        .iconSprite = gMonIcon_Kabutops,
        .iconPalIndex = 2,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(6, 10, SHADOW_SIZE_L)
        FOOTPRINT(Kabutops)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Kabutops,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Kabutops,
            gShinyOverworldPalette_Kabutops
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sKabutopsLevelUpLearnset,
        .teachableLearnset = sKabutopsTeachableLearnset,
        .formSpeciesIdTable = sKabutopsFormSpeciesIdTable,
        .formChangeTable = sKabutopsFormChangeTable,
    },
#endif //P_FAMILY_KABUTO && P_MEGA_EVOLUTIONS

#if P_FAMILY_MAGMAR && P_MEGA_EVOLUTIONS
    [SPECIES_MAGMORTAR_MEGA] =
    {
        .baseHP        = 75,
        .baseAttack    = 95,
        .baseDefense   = 92,
        .baseSpeed     = 108,
        .baseSpAttack  = 155,
        .baseSpDefense = 115,
        .types = MON_TYPES(TYPE_FIRE),
        .catchRate = 30,
        .expYield = 305,
        .evYield_SpAttack = 3,
        .genderRatio = PERCENT_FEMALE(25),
        .eggCycles = 25,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_HUMAN_LIKE),
        .abilities = { ABILITY_SHEER_FORCE, ABILITY_SHEER_FORCE, ABILITY_SHEER_FORCE },
        .bodyColor = BODY_COLOR_RED,
        .speciesName = _("Magmortar"),
        .cryId = CRY_MAGMORTAR,
        .natDexNum = NATIONAL_DEX_MAGMORTAR,
        .categoryName = _("Blast"),
        .height = 18,
        .weight = 980,
        .description = COMPOUND_STRING(
            "Its arm cannon now fires shells of\n"
            "magma hot enough to melt rock. It\n"
            "pours all its power into each blast\n"
            "and holds nothing back."),
        .pokemonScale = 259,
        .pokemonOffset = 1,
        .trainerScale = 296,
        .trainerOffset = 1,
        .frontPic = gMonFrontPic_MagmortarMega,
        .frontPicSize = MON_COORDS_SIZE(64, 61),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_STRETCH,
        .backPic = gMonBackPic_MagmortarMega,
        .backPicSize = MON_COORDS_SIZE(64, 53),
        .backPicYOffset = 6,
        .backAnimId = BACK_ANIM_SHAKE_GLOW_RED,
        .palette = gMonPalette_MagmortarMega,
        .shinyPalette = gMonShinyPalette_MagmortarMega,
        .iconSprite = gMonIcon_Magmortar,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(7, 12, SHADOW_SIZE_L)
        FOOTPRINT(Magmortar)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Magmortar,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Magmortar,
            gShinyOverworldPalette_Magmortar
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sMagmortarLevelUpLearnset,
        .teachableLearnset = sMagmortarTeachableLearnset,
        .formSpeciesIdTable = sMagmortarFormSpeciesIdTable,
        .formChangeTable = sMagmortarFormChangeTable,
    },
#endif //P_FAMILY_MAGMAR && P_MEGA_EVOLUTIONS

#if P_FAMILY_FEEBAS && P_MEGA_EVOLUTIONS
    [SPECIES_MILOTIC_MEGA] =
    {
        .baseHP        = 95,
        .baseAttack    = 60,
        .baseDefense   = 109,
        .baseSpeed     = 101,
        .baseSpAttack  = 130,
        .baseSpDefense = 145,
        .types = MON_TYPES(TYPE_WATER, TYPE_FAIRY),
        .catchRate = 60,
        .expYield = 224,
        .evYield_SpDefense = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_ERRATIC,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_1, EGG_GROUP_DRAGON),
        .abilities = { ABILITY_MISTY_SURGE, ABILITY_MISTY_SURGE, ABILITY_MISTY_SURGE },
        .bodyColor = BODY_COLOR_PINK,
        .speciesName = _("Milotic"),
        .cryId = CRY_MILOTIC,
        .natDexNum = NATIONAL_DEX_MILOTIC,
        .categoryName = _("Tender"),
        .height = 70,
        .weight = 2100,
        .description = COMPOUND_STRING(
            "A shimmering mist rises from its\n"
            "scales. Pokémon standing in the mist\n"
            "cannot be poisoned, burned or put\n"
            "to sleep."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 360,
        .trainerOffset = 7,
        .frontPic = gMonFrontPic_MiloticMega,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = P_GBA_STYLE_SPECIES_GFX ? ANIM_GLOW_BLUE : ANIM_CIRCULAR_STRETCH_TWICE,
        .backPic = gMonBackPic_MiloticMega,
        .backPicSize = MON_COORDS_SIZE(64, 60),
        .backPicYOffset = 2,
        .backAnimId = BACK_ANIM_SHAKE_GLOW_BLUE,
        .palette = gMonPalette_MiloticMega,
        .shinyPalette = gMonShinyPalette_MiloticMega,
        .iconSprite = gMonIcon_Milotic,
        .iconPalIndex = P_GBA_STYLE_SPECIES_ICONS ? 0 : 2,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(1, 13, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Milotic)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Milotic,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_SLITHER,
            sAnimTable_Following,
            gOverworldPalette_Milotic,
            gShinyOverworldPalette_Milotic
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sMiloticLevelUpLearnset,
        .teachableLearnset = sMiloticTeachableLearnset,
        .formSpeciesIdTable = sMiloticFormSpeciesIdTable,
        .formChangeTable = sMiloticFormChangeTable,
    },
#endif //P_FAMILY_FEEBAS && P_MEGA_EVOLUTIONS

#if P_FAMILY_OMANYTE && P_MEGA_EVOLUTIONS
    [SPECIES_OMASTAR_MEGA] =
    {
        .baseHP        = 70,
        .baseAttack    = 105,
        .baseDefense   = 140,
        .baseSpeed     = 75,
        .baseSpAttack  = 115,
        .baseSpDefense = 90,
        .types = MON_TYPES(TYPE_ROCK, TYPE_WATER),
        .catchRate = 45,
        .expYield = 208,
        .evYield_Defense = 2,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 30,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_1, EGG_GROUP_WATER_3),
        .abilities = { ABILITY_SKILL_LINK, ABILITY_SKILL_LINK, ABILITY_SKILL_LINK },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Omastar"),
        .cryId = CRY_OMASTAR,
        .natDexNum = NATIONAL_DEX_OMASTAR,
        .categoryName = _("Spiral"),
        .height = 19,
        .weight = 720,
        .description = COMPOUND_STRING(
            "Its shell has grown into a towering\n"
            "drill. It fires the spikes on its\n"
            "shell in volleys, and every volley\n"
            "hits five times."),
        .pokemonScale = 307,
        .pokemonOffset = 7,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_OmastarMega,
        .frontPicSize = MON_COORDS_SIZE(59, 60),
        .frontPicYOffset = 2,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_GROW_VIBRATE,
        .backPic = gMonBackPic_OmastarMega,
        .backPicSize = MON_COORDS_SIZE(62, 62),
        .backPicYOffset = 1,
        .backAnimId = BACK_ANIM_DIP_RIGHT_SIDE,
        .palette = gMonPalette_OmastarMega,
        .shinyPalette = gMonShinyPalette_OmastarMega,
        .iconSprite = gMonIcon_Omastar,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 11, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Omastar)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Omastar,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Omastar,
            gShinyOverworldPalette_Omastar
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sOmastarLevelUpLearnset,
        .teachableLearnset = sOmastarTeachableLearnset,
        .formSpeciesIdTable = sOmastarFormSpeciesIdTable,
        .formChangeTable = sOmastarFormChangeTable,
    },
#endif //P_FAMILY_OMANYTE && P_MEGA_EVOLUTIONS

#if P_FAMILY_POLIWAG && P_MEGA_EVOLUTIONS
    [SPECIES_POLIWRATH_MEGA] =
    {
        .baseHP        = 90,
        .baseAttack    = 125,
        .baseDefense   = 125,
        .baseSpeed     = 90,
        .baseSpAttack  = 70,
        .baseSpDefense = 110,
        .types = MON_TYPES(TYPE_WATER, TYPE_FIGHTING),
        .catchRate = 45,
        .expYield = 290,
        .evYield_Defense = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_1),
        .abilities = { ABILITY_IRON_FIST, ABILITY_IRON_FIST, ABILITY_IRON_FIST },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Poliwrath"),
        .cryId = CRY_POLIWRATH,
        .natDexNum = NATIONAL_DEX_POLIWRATH,
        .categoryName = _("Tadpole"),
        .height = 15,
        .weight = 820,
        .description = COMPOUND_STRING(
            "Its fists harden into heavy\n"
            "gauntlets. One punch can stop a\n"
            "charging Pokémon cold. The swirl on\n"
            "its belly spins faster as it fights."),
        .pokemonScale = 256,
        .pokemonOffset = 6,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_PoliwrathMega,
        .frontPicSize = MON_COORDS_SIZE(62, 60),
        .frontPicYOffset = 2,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_SHAKE_TWICE,
        .backPic = gMonBackPic_PoliwrathMega,
        .backPicSize = MON_COORDS_SIZE(57, 60),
        .backPicYOffset = 2,
        .backAnimId = BACK_ANIM_V_SHAKE_LOW,
        .palette = gMonPalette_PoliwrathMega,
        .shinyPalette = gMonShinyPalette_PoliwrathMega,
        .iconSprite = gMonIcon_Poliwrath,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(13, 11, SHADOW_SIZE_L)
        FOOTPRINT(Poliwrath)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Poliwrath,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Poliwrath,
            gShinyOverworldPalette_Poliwrath
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sPoliwrathLevelUpLearnset,
        .teachableLearnset = sPoliwrathTeachableLearnset,
        .formSpeciesIdTable = sPoliwrathFormSpeciesIdTable,
        .formChangeTable = sPoliwrathFormChangeTable,
    },
#endif //P_FAMILY_POLIWAG && P_MEGA_EVOLUTIONS

#if P_FAMILY_SEEDOT && P_MEGA_EVOLUTIONS
    [SPECIES_SHIFTRY_MEGA] =
    {
        .baseHP        = 90,
        .baseAttack    = 135,
        .baseDefense   = 80,
        .baseSpeed     = 95,
        .baseSpAttack  = 100,
        .baseSpDefense = 80,
        .types = MON_TYPES(TYPE_GRASS, TYPE_DARK),
        .catchRate = 45,
        .expYield = 275,
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_GRASS),
        .abilities = { ABILITY_WIND_RIDER, ABILITY_WIND_RIDER, ABILITY_WIND_RIDER },
        .bodyColor = BODY_COLOR_RED,
        .speciesName = _("Shiftry"),
        .cryId = CRY_SHIFTRY,
        .natDexNum = NATIONAL_DEX_SHIFTRY,
        .categoryName = _("Wicked"),
        .height = 17,
        .weight = 740,
        .description = COMPOUND_STRING(
            "It fans its blazing autumn leaves to\n"
            "whip up gales. With the wind at its\n"
            "back, it grows stronger and moves\n"
            "faster than the eye can follow."),
        .pokemonScale = 290,
        .pokemonOffset = 4,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_ShiftryMega,
        .frontPicSize = MON_COORDS_SIZE(59, 60),
        .frontPicYOffset = 2,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_H_VIBRATE,
        .backPic = gMonBackPic_ShiftryMega,
        .backPicSize = MON_COORDS_SIZE(61, 60),
        .backPicYOffset = 2,
        .backAnimId = BACK_ANIM_SHRINK_GROW_VIBRATE,
        .palette = gMonPalette_ShiftryMega,
        .shinyPalette = gMonShinyPalette_ShiftryMega,
        .iconSprite = gMonIcon_Shiftry,
        .iconPalIndex = P_GBA_STYLE_SPECIES_ICONS ? 0 : 5,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(4, 11, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Shiftry)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Shiftry,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Shiftry,
            gShinyOverworldPalette_Shiftry
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sShiftryLevelUpLearnset,
        .teachableLearnset = sShiftryTeachableLearnset,
        .formSpeciesIdTable = sShiftryFormSpeciesIdTable,
        .formChangeTable = sShiftryFormChangeTable,
    },
#endif //P_FAMILY_SEEDOT && P_MEGA_EVOLUTIONS

#if P_FAMILY_SNORLAX && P_MEGA_EVOLUTIONS
    [SPECIES_SNORLAX_MEGA] =
    {
        .baseHP        = 160,
        .baseAttack    = 140,
        .baseDefense   = 105,
        .baseSpeed     = 40,
        .baseSpAttack  = 65,
        .baseSpDefense = 130,
        .types = MON_TYPES(TYPE_NORMAL),
        .catchRate = 25,
        .expYield = 224,
        .evYield_HP = 2,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 40,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_MONSTER),
        .abilities = { ABILITY_FLUFFY, ABILITY_FLUFFY, ABILITY_FLUFFY },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("Snorlax"),
        .cryId = CRY_SNORLAX,
        .natDexNum = NATIONAL_DEX_SNORLAX,
        .categoryName = _("Sleeping"),
        .height = 25,
        .weight = 5200,
        .description = COMPOUND_STRING(
            "Mega Evolution covers it in a thick,\n"
            "shaggy coat. Blows sink into the fur\n"
            "and lose their force, but the coat\n"
            "catches fire easily."),
        .pokemonScale = 256,
        .pokemonOffset = 4,
        .trainerScale = 423,
        .trainerOffset = 11,
        .frontPic = gMonFrontPic_SnorlaxMega,
        .frontPicSize = MON_COORDS_SIZE(60, 62),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = P_GBA_STYLE_SPECIES_GFX ? ANIM_SWING_CONCAVE : ANIM_V_STRETCH,
        .backPic = gMonBackPic_SnorlaxMega,
        .backPicSize = MON_COORDS_SIZE(64, 52),
        .backPicYOffset = 6,
        .backAnimId = BACK_ANIM_DIP_RIGHT_SIDE,
        .palette = gMonPalette_SnorlaxMega,
        .shinyPalette = gMonShinyPalette_SnorlaxMega,
        .iconSprite = gMonIcon_Snorlax,
        .iconPalIndex = P_GBA_STYLE_SPECIES_ICONS ? 1 : 3,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(1, 12, SHADOW_SIZE_L)
        FOOTPRINT(Snorlax)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Snorlax,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Snorlax,
            gShinyOverworldPalette_Snorlax
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sSnorlaxLevelUpLearnset,
        .teachableLearnset = sSnorlaxTeachableLearnset,
        .eggMoveLearnset = sSnorlaxEggMoveLearnset,
        .formSpeciesIdTable = sSnorlaxFormSpeciesIdTable,
        .formChangeTable = sSnorlaxFormChangeTable,
    },
#endif //P_FAMILY_SNORLAX && P_MEGA_EVOLUTIONS

#if P_FAMILY_TORKOAL && P_MEGA_EVOLUTIONS
    [SPECIES_TORKOAL_MEGA] =
    {
        .baseHP        = 70,
        .baseAttack    = 95,
        .baseDefense   = 160,
        .baseSpeed     = 20,
        .baseSpAttack  = 130,
        .baseSpDefense = 95,
        .types = MON_TYPES(TYPE_FIRE, TYPE_STEEL),
        .catchRate = 90,
        .expYield = 200,
        .evYield_Defense = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_DROUGHT, ABILITY_DROUGHT, ABILITY_DROUGHT },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Torkoal"),
        .cryId = CRY_TORKOAL,
        .natDexNum = NATIONAL_DEX_TORKOAL,
        .categoryName = _("Coal"),
        .height = 9,
        .weight = 1320,
        .description = COMPOUND_STRING(
            "Its shell becomes an iron furnace\n"
            "with a cannon on its back. The heat\n"
            "it gives off turns the sunlight\n"
            "harsh the moment it appears."),
        .pokemonScale = 390,
        .pokemonOffset = 9,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_TorkoalMega,
        .frontPicSize = MON_COORDS_SIZE(58, 61),
        .frontPicYOffset = 2,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_STRETCH,
        .backPic = gMonBackPic_TorkoalMega,
        .backPicSize = MON_COORDS_SIZE(62, 61),
        .backPicYOffset = 2,
        .backAnimId = BACK_ANIM_SHAKE_GLOW_RED,
        .palette = gMonPalette_TorkoalMega,
        .shinyPalette = gMonShinyPalette_TorkoalMega,
        .iconSprite = gMonIcon_Torkoal,
        .iconPalIndex = P_GBA_STYLE_SPECIES_ICONS ? 1 : 2,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(3, 11, SHADOW_SIZE_L)
        FOOTPRINT(Torkoal)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Torkoal,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Torkoal,
            gShinyOverworldPalette_Torkoal
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sTorkoalLevelUpLearnset,
        .teachableLearnset = sTorkoalTeachableLearnset,
        .eggMoveLearnset = sTorkoalEggMoveLearnset,
        .formSpeciesIdTable = sTorkoalFormSpeciesIdTable,
        .formChangeTable = sTorkoalFormChangeTable,
    },
#endif //P_FAMILY_TORKOAL && P_MEGA_EVOLUTIONS

#if P_FAMILY_CYNDAQUIL && P_MEGA_EVOLUTIONS
    [SPECIES_TYPHLOSION_MEGA] =
    {
        .baseHP        = 78,
        .baseAttack    = 94,
        .baseDefense   = 93,
        .baseSpeed     = 125,
        .baseSpAttack  = 144,
        .baseSpDefense = 100,
        .types = MON_TYPES(TYPE_FIRE, TYPE_GROUND),
        .catchRate = 45,
        .expYield = 302,
        .evYield_SpAttack = 3,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_ADAPTABILITY, ABILITY_ADAPTABILITY, ABILITY_ADAPTABILITY },
        .bodyColor = BODY_COLOR_YELLOW,
        .speciesName = _("Typhlosion"),
        .cryId = CRY_TYPHLOSION,
        .natDexNum = NATIONAL_DEX_TYPHLOSION,
        .categoryName = _("Volcano"),
        .height = 19,
        .weight = 920,
        .description = COMPOUND_STRING(
            "Its back erupts like a volcano,\n"
            "raining fire and molten rock. It has\n"
            "mastered its own flames so well that\n"
            "its fire burns hotter than ever."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 268,
        .trainerOffset = 1,
        .frontPic = gMonFrontPic_TyphlosionMega,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_V_SHAKE,
        .backPic = gMonBackPic_TyphlosionMega,
        .backPicSize = MON_COORDS_SIZE(63, 62),
        .backPicYOffset = 1,
        .backAnimId = BACK_ANIM_SHAKE_GLOW_RED,
        .palette = gMonPalette_TyphlosionMega,
        .shinyPalette = gMonShinyPalette_TyphlosionMega,
        .iconSprite = gMonIcon_Typhlosion,
        .iconPalIndex = P_GBA_STYLE_SPECIES_ICONS ? 1 : 3,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(1, 13, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Typhlosion)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Typhlosion,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Typhlosion,
            gShinyOverworldPalette_Typhlosion
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sTyphlosionLevelUpLearnset,
        .teachableLearnset = sTyphlosionTeachableLearnset,
        .formSpeciesIdTable = sTyphlosionFormSpeciesIdTable,
        .formChangeTable = sTyphlosionFormChangeTable,
    },
#endif //P_FAMILY_CYNDAQUIL && P_MEGA_EVOLUTIONS

#if P_FAMILY_COTTONEE && P_MEGA_EVOLUTIONS
    [SPECIES_WHIMSICOTT_MEGA] =
    {
        .baseHP        = 60,
        .baseAttack    = 77,
        .baseDefense   = 115,
        .baseSpeed     = 126,
        .baseSpAttack  = 97,
        .baseSpDefense = 105,
        .types = MON_TYPES(TYPE_GRASS, TYPE_FAIRY),
        .catchRate = 75,
        .expYield = 203,
        .evYield_Speed = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_GRASS, EGG_GROUP_FAIRY),
        .abilities = { ABILITY_PRANKSTER, ABILITY_PRANKSTER, ABILITY_PRANKSTER },
        .bodyColor = BODY_COLOR_WHITE,
        .speciesName = _("Whimsicott"),
        .cryId = CRY_WHIMSICOTT,
        .natDexNum = NATIONAL_DEX_WHIMSICOTT,
        .categoryName = _("Windveiled"),
        .height = 11,
        .weight = 95,
        .description = COMPOUND_STRING(
            "Its cotton grows into a cloud\n"
            "crowned with antlers. It loves\n"
            "mischief more than ever, and its\n"
            "pranks come too fast to see coming."),
        .pokemonScale = 365,
        .pokemonOffset = 12,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_WhimsicottMega,
        .frontPicSize = MON_COORDS_SIZE(64, 60),
        .frontPicYOffset = 2,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        .frontAnimId = ANIM_SHRINK_GROW,
        .backPic = gMonBackPic_WhimsicottMega,
        .backPicSize = MON_COORDS_SIZE(64, 50),
        .backPicYOffset = 6,
        .backAnimId = BACK_ANIM_CONCAVE_ARC_LARGE,
        .palette = gMonPalette_WhimsicottMega,
        .shinyPalette = gMonShinyPalette_WhimsicottMega,
        .iconSprite = gMonIcon_Whimsicott,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-2, 11, SHADOW_SIZE_L)
        FOOTPRINT(Whimsicott)
    #if OW_BATTLE_ONLY_FORMS
        OVERWORLD(
            sPicTable_Whimsicott,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Whimsicott,
            gShinyOverworldPalette_Whimsicott
        )
    #endif //OW_BATTLE_ONLY_FORMS
        .isMegaEvolution = TRUE,
        .levelUpLearnset = sWhimsicottLevelUpLearnset,
        .teachableLearnset = sWhimsicottTeachableLearnset,
        .formSpeciesIdTable = sWhimsicottFormSpeciesIdTable,
        .formChangeTable = sWhimsicottFormChangeTable,
    },
#endif //P_FAMILY_COTTONEE && P_MEGA_EVOLUTIONS
