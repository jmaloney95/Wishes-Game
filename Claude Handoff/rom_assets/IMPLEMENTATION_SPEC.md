# IMPLEMENTATION SPEC — Pokémon Wishes of Tomorrow: Title Screen + Studio Intro
## Handoff brief for Claude Cowork (ROM hack integration)

You are integrating a custom title screen and a "Flatfoot Games presents" studio
intro into a Pokémon Emerald ROM hack. This document tells you exactly what the
assets are, where they go, and how to wire them up.

------------------------------------------------------------------------------
## 0. FIRST: TELL ME WHICH BASE THE HACK USES
------------------------------------------------------------------------------
The integration differs completely depending on the toolchain. Confirm which one
this project uses before writing code:

  (A) pokeemerald DECOMP (C source, compiled with devkitARM/agbcc) — RECOMMENDED,
      and this spec is written primarily for it. Inspired hacks like Unbound use
      decomp-style workflows.
  (B) Binary ROM hack (editing a .gba with tools like Advance Map, unLZ-GBA,
      NSE, Tilemap Studio). Assets are the same but insertion is manual; see
      Section 6.

If you can see the project files, check for: a `Makefile`, `src/` and
`graphics/` folders, and `src/title_screen.c` => that's decomp (A). A lone
`.gba` file => binary (B).

Everything below assumes (A) unless stated.

------------------------------------------------------------------------------
## 1. WHAT'S IN THIS PACKAGE
------------------------------------------------------------------------------
rom_assets/
  title_screen/
    bg_scene.png        240x160, 11 colors, INDEXED. Space + stars + Mt. Fuji,
                        flattened. This is the static title background (BG layer).
    pokemon_logo.png    120x42,  5 colors (idx0 transparent). The Pokémon logo.
    jirachi_sprite.png  64x47,   16 colors (idx0 transparent). Jirachi. PRIORITIZED
                        palette — give it its own 16-color slot.
    title_text.png      240x160, 7 colors (idx0 transparent). "WISHES OF TOMORROW"
                        + "PRESS START" on one transparent layer.
  intro/
    intro_bg_scene.png  240x160, 4 colors, INDEXED. Intro space background.
    flatfoot_logo.png   184x50,  8 colors (idx0 transparent). Full studio lockup.
    flatfoot_emblem.png 34x50,   6 colors (idx0 transparent). Footprint mark only
                        (use as a sprite for the "stamp-in" animation).
    presents_text.png   ~56x9,   4 colors (idx0 transparent). "PRESENTS".
  reference/
    title_screen_final.png         what the finished title must look like.
    intro_animation_reference.gif  the studio intro motion/timing.
    full_boot_reference.gif        intro -> fade -> title, the whole sequence.

ALL PNGs are already quantized to the GBA 5-bit color grid (channels divisible
by 8) and are palette-indexed with index 0 = transparent on sprite assets. Do
not re-quantize or dither them — feed them to gbagfx as-is.

------------------------------------------------------------------------------
## 2. GBA / EMERALD CONSTRAINTS (already satisfied — keep them satisfied)
------------------------------------------------------------------------------
- Screen 240x160. Tiles 8x8. BG/sprite palettes hold 16 colors (idx0 transparent).
- Per-palette color counts are all <=16 (verified). Don't merge assets onto one
  palette unless their combined unique colors stay <=15 visible + transparent.
- Palette budget plan:
    * Title BG scene (11)            -> one BG palette
    * Jirachi (15 visible)           -> its OWN sprite palette (do not share)
    * Pokémon logo (4 visible)       -> share with title_text (4) = 8 total, OK
      OR give logo its own; your call based on free slots.
    * Intro bg (4) + flatfoot logo (7) + presents (3) can share if combined <=15.

------------------------------------------------------------------------------
## 3. TITLE SCREEN — DECOMP INTEGRATION (pokeemerald)
------------------------------------------------------------------------------
Verified file locations (pret/pokeemerald master):
  - Graphics live in:      graphics/title_screen/
  - INCBIN declarations:   src/graphics.c   (gTitleScreen* symbols)
  - Screen logic:          src/title_screen.c

Stock Emerald title = Rayquaza bg + clouds bg + "Pokémon Emerald" logo + Press
Start + copyright, with a logo "shine" sprite. You are replacing the visual
content. Two viable approaches:

### Approach 3A (SIMPLEST / RECOMMENDED): replace the existing graphic slots
Map our assets onto the existing pipeline so you change pixels, not architecture.

  1) Convert + place files:
     - bg_scene.png   -> graphics/title_screen/  (becomes the main BG tilemap+gfx)
     - pokemon_logo.png -> replace pokemon_logo.* (gfx/tilemap/pal)
     - jirachi_sprite.png -> add as a NEW sprite (replaces Rayquaza's role)
     - title_text.png -> supplies PRESS START (and subtitle); or split subtitle
       onto the BG and keep press_start.* for the blinking prompt.

  2) In src/graphics.c, the relevant INCBINs (verify exact names in your tree):
       gTitleScreenBgPalettes      <- pokemon_logo.gbapal + rayquaza_and_clouds.gbapal
       gTitleScreenPokemonLogoGfx  <- pokemon_logo.8bpp.lz
       gTitleScreenPokemonLogoTilemap <- pokemon_logo.bin.lz
       gTitleScreenPressStartGfx   <- press_start.4bpp.lz
       gTitleScreenPressStartPal   <- press_start.gbapal
     Repoint these (or their underlying files) to our art. The build's gbagfx +
     scaninc will regenerate .lz/.gbapal from the PNGs via the graphics rules in
     the Makefile / graphics_file_rules. Add new rules for new files.

  3) In src/title_screen.c:
     - The stock code loads Rayquaza (sTitleScreenRayquazaTilemap) and animates
       clouds + a logo shine + version sprite. Replace Rayquaza load with our
       bg_scene tilemap, and replace the version-mon sprite path with the Jirachi
       sprite. Remove/disable cloud scroll if you don't want it (our bg is static).
     - Keep the PRESS START blink task; point it at our press_start gfx.
     - IMPORTANT (known Emerald quirk): during the logo intro there is a
       deliberate "green/yellow flash" via BLDCNT lighten effect
       (BLDCNT_TGT1_BG2 | BLDCNT_EFFECT_LIGHTEN). If you don't want that flash on
       the new logo, reduce/remove the lighten blend on BG2 in the title setup.
       (This is the same class of issue as the GIF flicker we already fixed in the
       art — here it's a hardware blend register, so handle it in code.)

  4) Jirachi placement: center horizontally; sit it on the mountain's flat top.
     On the 240x160 screen the sprite's top-left goes at approx (88, 63).
     Optional float: add a small sine offset to the sprite Y each frame
     (amplitude ~2px, period ~90 frames) — cheap and matches the reference.

### Approach 3B (cleaner long-term): rewrite CB2_InitTitleScreen scene
If comfortable in the decomp, build the scene explicitly:
   BG1 = bg_scene (priority lowest), Sprites = Jirachi + logo + press start,
   BG0 = subtitle text. Use the reference image for exact layout.

------------------------------------------------------------------------------
## 4. STUDIO INTRO — DECOMP INTEGRATION
------------------------------------------------------------------------------
Verified: the intro is in src/intro.c. Stock Emerald intro scenes:
   Scene 0 = copyright, Scene 1 = GameFreak logo, Scene 2 = biking,
   Scene 3 = Groudon/Kyogre/Rayquaza, then -> title screen.

Goal: show "Flatfoot Games presents" BEFORE (or in place of) the GameFreak logo
scene, then continue to the title.

Recommended minimal approach:
  1) Add a new scene task, e.g. Task_Scene_FlatfootLogo, inserted at the very
     start of the intro state machine (before Scene 0/1) OR repurpose Scene 1.
  2) Load intro_bg_scene as the BG, flatfoot_emblem as a sprite, flatfoot_logo
     + presents_text as additional sprites/BG.
  3) Animation timeline (see intro_animation_reference.gif), ~3 seconds @ 60fps:
       Phase A (~0.0-0.6s): emblem "stamps" in. Use OBJ AFFINE scaling: start the
         sprite at ~1.5x scale and shrink to 1.0x while fading in via palette.
         Optional 1px screen shake on landing + a sparkle sprite for 3 frames.
       Phase B (~0.6-1.2s): reveal "FLATFOOT GAMES" left-to-right. Easiest in HW:
         use a WINDOW (WIN0) that widens left->right to mask the logo, or unhide
         tile columns progressively.
       Phase C (~1.2-1.6s): fade "PRESENTS" in (palette fade) + sparkle wink.
       Phase D (~1.6-2.5s): hold.
       Phase E (~2.5-3.1s): palette-fade to black, then transition to title.
  4) FADES: use the standard pokeemerald palette fade helpers
     (BeginNormalPaletteFade / the fade task pattern used elsewhere in intro.c).
     DO NOT do per-pixel alpha — palette fading is how Emerald intros fade and it
     avoids the off-palette artifacting we already eliminated in the previews.
  5) Allow pressing A/Start to skip to the title (match stock intro skip behavior).

If you want the absolute minimum first pass: replace ONLY the GameFreak logo
graphic with flatfoot_logo.png (static, with the existing fade in/out), ship that,
then add the stamp/wipe polish in a second iteration.

------------------------------------------------------------------------------
## 5. ASSET CONVERSION NOTES (decomp build)
------------------------------------------------------------------------------
- pokeemerald's build converts PNGs via tools/gbagfx automatically when you add
  the right entries to the graphics file rules and INCBIN them. Indexed PNG ->
  .4bpp/.8bpp + .gbapal; tilemaps via .bin (+ .lz for compressed).
- Our sprites are 4bpp-friendly (<=16 colors). bg_scene can be 4bpp (11 colors).
- Keep index 0 = transparent on all sprite PNGs (already set).
- If a tool complains about palette order, the safe fix is to load our exact
  palette rather than letting the tool re-derive it. Each asset's palette is
  embedded in its PNG.
- Sizes: Jirachi 64x47 -> pad/allocate as a 64x64 OBJ (or 64x32+64x16). Logo
  120x42 spans multiple sprites if used as OBJ; using it as a BG tilemap is
  simpler. Emblem 34x50 -> 32x64 or 64x64 OBJ for affine scaling room.

------------------------------------------------------------------------------
## 6. BINARY ROM HACK PATH (if base is a raw .gba)
------------------------------------------------------------------------------
- Title background: import bg_scene.png as the title BG tileset+tilemap+palette
  using unLZ-GBA / Tilemap Studio at the title image offsets.
- Logo / Jirachi: insert as sprites or additional BG; repoint pointers.
- Use the reference PNG to match placement.
- The intro stamp/wipe animation is hard to reproduce purely in binary; for (B)
  consider a static "Flatfoot Games presents" frame inserted into the intro
  graphic slot, with the game's existing fade. Tell me if (B) is the case and I
  can produce a single flattened intro frame sized for the slot.

------------------------------------------------------------------------------
## 7. ACCEPTANCE CRITERIA
------------------------------------------------------------------------------
- Boot order: [Flatfoot Games intro] -> fade -> [title screen] -> PRESS START.
- Title screen matches reference/title_screen_final.png: logo top-center,
  "WISHES OF TOMORROW" overlapping the logo's lower edge, Jirachi on the mountain,
  PRESS START blinking at the bottom, twinkling stars.
- No color glitches/flashes during fades (use palette fades; mind BLDCNT on title).
- Runs on real hardware / mGBA without palette overflow warnings.
- Pressing Start/A during intro skips to title.

------------------------------------------------------------------------------
## 8. THINGS TO VERIFY AGAINST THE ACTUAL REPO (don't assume)
------------------------------------------------------------------------------
Symbol/file names drift between pokeemerald versions and forks. Before editing,
grep the tree for the real names:
    grep -rn "TitleScreenPokemonLogo" src/ include/
    grep -rn "title_screen" graphics/ src/graphics.c
    ls graphics/title_screen/
    ls graphics/intro/
Confirm the Makefile graphics rules pattern (some forks use *.png auto-rules,
others need explicit entries). Match the fork's existing conventions.

Questions for the project owner if anything is ambiguous:
  - decomp or binary base? which fork/commit?
  - keep or remove the stock cloud scroll / logo shine?
  - should the intro REPLACE the GameFreak scene or play in addition to it?
