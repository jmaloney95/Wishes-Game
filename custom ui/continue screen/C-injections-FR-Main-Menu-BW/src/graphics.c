#include "global.h"
#include "global.h"
#include "menu.h"
#include "task.h"
#include "constants/songs.h"
#include "bg.h"
#include "malloc.h"
#include "palette.h"
#include "sprite.h"
#include "window.h"
#include "string_util.h"
#include "sprite.h"
#include "event_data.h"
#include "constants/flags.h"
#include "constants/vars.h"
#include "constants/moves.h"
#include "party_menu.h"
#include "scanline_effect.h"
#include "gpu_regs.h"
#include "new_menu_helpers.h"
#include "field_effect.h"
#include "text_window.h"
#include "overworld.h"
#include "field_weather.h"
#include "trainer_pokemon_sprites.h"

#include "pokemon_summary_screen.h"
#include "menu_helpers.h"
#include "link.h"
#include "graphics.h"
//Main Menu
const u16 sMainMenuTextPal[] = {RGB(0, 0, 0), RGB(30, 30, 30), RGB(17, 17, 17), RGB(7, 20, 31), RGB(7, 13, 20)};
const u16 sMainMenuTextFemalePal[] = {RGB(31, 9, 11), RGB(21, 8, 8)};
const u16 gPalMainMenuBG[] = INCBIN_U16("graphics/bg.gbapal");
const u16 gPalMainMenuNoSel[] = INCBIN_U16("graphics/no_sel.gbapal");
const u16 gPalMainMenuSel[] = INCBIN_U16("graphics/sel.gbapal");
const u32 gTilesMainMenuBG1[] = INCBIN_U32("graphics/tiles_bg1.4bpp.lz");
const u32 gTilesMainMenuBG2[] = INCBIN_U32("graphics/tiles_bg2.4bpp.lz");
const u32 gMapMainMenuBG2[] = INCBIN_U32("graphics/bg.bin.lz");
const u32 gMapMainMenuNewGame[] = INCBIN_U32("graphics/new_game.bin.lz");
const u32 gMapMainMenuContinue[] = INCBIN_U32("graphics/continue.bin.lz");
