# Sakura / Press Garden — Porytiles secondary tileset

A starter secondary tileset converted from ekat99's Press Garden art for a
**fresh (dual-layer) pokeemerald** project. It contains two large ornamental
sakura trees (pink + purple) and a row of small blossom/decor tiles.

## What's in here

```
porytiles_sakura/
├── sakura_secondary/        <- the Porytiles SOURCE folder
│   ├── bottom.png           128px-wide layer, drawn UNDER the player (tree trunks/bases)
│   ├── middle.png           drawn under the player (small decor tiles)
│   ├── top.png              drawn OVER the player (tree canopies — you walk behind them)
│   └── anim/                (empty — put animated-tile subfolders here later)
├── build.sh                 one-shot compile command (edit the 3 paths at top)
└── README.md                this file
```

Magenta (`RGB 255,0,255`) is the **empty/transparent** color Porytiles expects —
do not paint with it. Each layer is 128px wide = 8 metatiles per row.

## One-time setup

1. **Install Porytiles** (Homebrew is easiest, works on WSL too):
   see https://github.com/grunt-lucas/porytiles/wiki/Installing-A-Release
   Confirm it's on your PATH: `porytiles --help`

2. **Get a paired PRIMARY source folder.** A secondary set in Porytiles must be
   compiled against a primary *source* folder (not the compiled game files).
   The simplest route is to decompile your project's existing general/grass
   primary into Porytiles format once:

   ```
   porytiles decompile-primary -dual-layer \
     -o "$HOME/porytiles-primary-source" \
     "$HOME/pokeemerald/data/tilesets/primary/general" \
     "$HOME/pokeemerald/include/constants/metatile_behaviors.h"
   ```

   Point `PRIMARY_SRC` in `build.sh` at that output folder. (Pairing lets
   Porytiles reuse the primary's grass tiles/palettes instead of duplicating
   them — the trees will sit over your existing grass.)

3. **Create the empty tileset in Porymap** so the game knows it exists:
   - `Tools -> New Tileset`, type **Secondary**, name it `SakuraPressGarden`
     (Porymap will create `gTileset_SakuraPressGarden`).
   - Open the map you want to paint on, set its **secondary tileset** to
     `gTileset_SakuraPressGarden`, and **Save**.

## Build it

Edit the three paths at the top of `build.sh`, then:

```
chmod +x build.sh
./build.sh
```

Then in Porymap: **File -> Reload Project**. The sakura metatiles appear in the
secondary half of the metatile picker, ready to paint.

## After it compiles — finishing the trees in Porymap

Porytiles handles the tiles and palettes; a few things are set per-metatile in
Porymap (they aren't part of the source art):

- **Collision / passability:** paint the trunk + canopy metatiles as impassable
  on the map's movement-permission layer so the player can't walk through them.
- **Layer type:** if a canopy doesn't render over the player correctly, set that
  metatile's *Layer Type* in the Tileset Editor (Normal / Covered / Split).
- **Re-run `build.sh` whenever you edit the source PNGs**, and also re-run it if
  you ever recompile the paired primary, so the two stay in sync.

## Extending the set

Drop more art into `bottom.png` / `middle.png` / `top.png` at any free
(magenta) metatile cell — canopies/overheads on `top.png`, everything the player
walks in front of on `bottom.png`/`middle.png` — then rebuild. The same
conversion pipeline (transparency + GBA palette snap) was already applied to the
source art, so new pieces just need to follow the same magenta-empty convention.
