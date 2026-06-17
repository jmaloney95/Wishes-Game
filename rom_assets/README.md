# Pokémon Wishes of Tomorrow — ROM Hack Asset Handoff

This package contains everything needed to implement the custom title screen and
the "Flatfoot Games presents" studio intro into the Pokémon Emerald ROM hack.

## START HERE
1. Read `IMPLEMENTATION_SPEC.md` — the full integration guide.
2. `asset_manifest.json` — machine-readable summary of every asset (sizes,
   colors, placements, timings).
3. `reference/` — look at these images/GIFs to know the target result.

## FOLDERS
- `title_screen/` — indexed PNGs for the title screen (background, logo,
  Jirachi sprite, text).
- `intro/`        — indexed PNGs for the studio intro (background, Flatfoot logo,
  footprint emblem, "PRESENTS" text).
- `reference/`    — final-look stills + animation GIFs (visual targets only;
  not for direct import).

## KEY FACTS FOR THE IMPLEMENTER
- All art is GBA-spec: 240x160, palettes ≤16 colors, index 0 = transparent on
  sprites, colors already on the 5-bit grid. Feed PNGs to the build's gbagfx
  as-is; do not re-quantize or dither.
- Boot order: Flatfoot intro → fade → title screen → PRESS START.
- Jirachi has its own full 16-color palette (intentionally prioritized for
  detail) — give it a dedicated palette slot.
- Use pokeemerald palette-fade routines for all fades (avoids color artifacts).
- Confirm decomp vs binary base, and grep the repo for exact symbol names before
  editing (names drift between forks). See spec §0 and §8.

## QUESTIONS FOR THE PROJECT OWNER (resolve before/while implementing)
- Is the base a pokeemerald decomp or a binary .gba?  Which fork/commit?
- Should the Flatfoot intro REPLACE the GameFreak logo scene or play in addition?
- Keep or remove the stock title cloud-scroll / logo-shine effects?
- Is a static first-pass intro acceptable, with the animated stamp/wipe added in
  a second iteration? (Lower risk, faster to ship.)
