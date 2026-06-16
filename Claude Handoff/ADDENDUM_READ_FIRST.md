# ADDENDUM — Read this first. Resolves scope + points you at the right files.

## 1. YOU ARE PROBABLY LOOKING AT THE WRONG FILES
The GIFs (`boot_sequence_v2.gif`, `logo_intro.gif`) and the `flatfoot_logo_package.zip`
are ANIMATION REFERENCES ONLY — they are not importable assets. Do not try to
use them as game data.

The real, importable assets are in:  `wishes_of_tomorrow_ROM_handoff.zip`
Unpack it. Inside is a `rom_assets/` folder containing:
  - rom_assets/IMPLEMENTATION_SPEC.md   <- the full build guide (read this)
  - rom_assets/asset_manifest.json      <- sizes/colors/positions/timings
  - rom_assets/title_screen/*.png       <- INDEXED, GBA-ready (≤16 colors, idx0 transparent)
  - rom_assets/intro/*.png              <- INDEXED, GBA-ready
  - rom_assets/reference/*              <- target look (images + the GIFs)

These PNGs are already 240×160-compatible, palette-indexed, on the GBA 5-bit
color grid, with index 0 = transparent on sprites. Feed them to the build's
gbagfx as-is. This is the drop-in art. There is intentionally no .c/.h drop-in,
because the code edits depend on the project's fork — you write those.

## 2. YOUR SCOPE QUESTION — ANSWERED
You correctly noted GBA can't play a 69-frame GIF and that scope drives the work.
Correct. Build to **TIER 2** below unless the owner says otherwise.

### TIER 1 — Static replacement (baseline, do this first, commit it)
  - Title screen: replace background + logo + Jirachi + text using the
    rom_assets/title_screen PNGs. Reuse Emerald's existing title fade.
  - Intro: replace ONLY the GameFreak logo graphic with rom_assets/intro/
    flatfoot_logo.png, using the existing intro fade in/out. Static.
  - No new animation code. This is the safety net — get it rendering correctly
    on mGBA before adding motion.

### TIER 2 — Light, GBA-native animation (TARGET)  ✅ build to here
  Tier 1 PLUS only these, all cheap and hardware-idiomatic:
  - PRESS START blink: keep/reuse stock Emerald's existing blink task.
  - Jirachi float: add a sine offset to the Jirachi sprite's Y each frame
    (amplitude ~2px, period ~90 frames). ~5 lines in the title task.
  - Star twinkle: cycle 1–2 palette entries' brightness on the background
    palette (palette animation), not per-pixel edits.
  - Intro→title transition: use pokeemerald's BeginNormalPaletteFade to fade the
    Flatfoot logo to black, then load the title. No frame-by-frame work.
  NO affine scaling, NO window wipes. The GIFs show more motion than Tier 2 —
  that extra motion is Tier 3 and is optional.

### TIER 3 — Full animated studio intro (OPTIONAL, only if owner asks)
  Tier 2 PLUS:
  - Footprint "stamp-in": flatfoot_emblem.png as an OBJ, start at ~1.5× via OBJ
    affine scaling, shrink to 1.0× over ~0.6s while palette-fading in; optional
    1px shake + a sparkle OBJ for 3 frames on landing.
  - Logo "wipe-in": reveal flatfoot_logo left→right using a WIN0 window that
    widens across the logo, or by unhiding tile columns progressively.
  - "PRESENTS" palette-fades in, brief hold, fade to black.
  Use rom_assets/reference/intro_animation_reference.gif purely for TIMING.

## 3. DEFINITION OF DONE (Tier 2)
  - Boot: [Flatfoot logo, fades in/out] → fade → [title screen] → PRESS START.
  - Title matches rom_assets/reference/title_screen_final.png: logo top-center,
    "WISHES OF TOMORROW" overlapping the logo's lower edge, Jirachi floating over
    Mt. Fuji, blinking PRESS START, twinkling stars.
  - No palette overflow; no color flashing during fades (use palette fades; if a
    green/yellow flash appears on the title logo, it's Emerald's BLDCNT lighten
    effect on BG2 — reduce/remove it; see SPEC §3.3).
  - Start/A skips the intro to the title (match stock skip behavior).
  - Builds clean; runs on mGBA.

## 4. CONCRETE FILE TARGETS (pokeemerald decomp — verified upstream)
  Title: graphics/title_screen/  +  src/graphics.c (INCBINs)  +  src/title_screen.c
  Intro: graphics/intro/  +  src/intro.c
  Before editing, grep for real symbol names in THIS fork (they drift):
    grep -rn "TitleScreenPokemonLogo" src/ include/
    ls graphics/title_screen/ graphics/intro/
  Match the fork's existing graphics_file_rules / Makefile conventions for new PNGs.

## 5. IF THE BASE IS A BINARY .gba (not decomp)
  Stop and tell the owner. Tier 2/3 animation is impractical in binary. We'd ship
  Tier 1 only: import title PNGs via Tilemap Studio/unLZ-GBA at the title offsets
  and a single static Flatfoot frame in the intro slot. Ask me (the asset author)
  for a flattened intro frame sized to the slot if so.

## 6. RECOMMENDED ORDER OF WORK
  1) Unpack wishes_of_tomorrow_ROM_handoff.zip; read IMPLEMENTATION_SPEC.md.
  2) Confirm decomp vs binary; confirm fork/commit.
  3) Build Tier 1, commit, screenshot on mGBA.
  4) Add Tier 2 motion, commit.
  5) Only if asked: Tier 3.
  Ask the owner before expanding past Tier 2.
