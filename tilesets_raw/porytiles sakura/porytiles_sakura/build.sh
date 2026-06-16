#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# Build the Sakura (Press Garden) secondary tileset with Porytiles.
# Fresh pokeemerald is DUAL-LAYER, so we pass -dual-layer.
# (If you ever switch to a triple-layer project, just delete that flag.)
# ---------------------------------------------------------------------------
set -euo pipefail

# ---- PATHS FOR "Pokémon Wishes of Tomorrow" (edit if your layout differs) ----
# NOTE: this is the WSL/Linux view of the project. From Windows the repo lives at
#   D:\ROM Hack Project\pokeemerald-expansion
# In WSL that's typically /mnt/d/ROM Hack Project/pokeemerald-expansion
POKEEMERALD="/mnt/d/ROM Hack Project/pokeemerald-expansion"   # your decomp project root
PORYTILES="porytiles"                                          # or absolute path to the binary
# Munen uses gTileset_General as its PRIMARY. Decompile it once to a porytiles
# source folder (see README step 2), then point PRIMARY_SRC at that output:
PRIMARY_SRC="$HOME/porytiles-primary-source"                  # paired PRIMARY porytiles source folder
# -----------------------------------------------------------------------------

HERE="$(cd "$(dirname "$0")" && pwd)"
SECONDARY_SRC="$HERE/sakura_secondary"
BEHAVIORS="$POKEEMERALD/include/constants/metatile_behaviors.h"
OUT="$POKEEMERALD/data/tilesets/secondary/sakura_press_garden"

echo ">> Compiling secondary tileset -> $OUT"
"$PORYTILES" compile-secondary -dual-layer -Wall \
  -o "$OUT" \
  "$SECONDARY_SRC" \
  "$PRIMARY_SRC" \
  "$BEHAVIORS"

echo ">> Done. Now in Porymap: File -> Reload Project."
