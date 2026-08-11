"""FairyCleanedUp -> data/tilesets/secondary/fairy_town (secondary, pairs with gTileset_General).

The source is RPG Maker XP art drawn at 2x (32px tiles), so everything is halved
to the GBA's 16x16 metatile grid first.

Selection follows the priority order: terrain, trees and purple water first,
then the mossy fairy buildings, then props while palette room lasts.
Plain houses, shops, Mart, Gym and every indoor floor/furniture band are dropped.
"""
import os, sys
import numpy as np
from PIL import Image
sys.path.insert(0, os.path.dirname(__file__))
from tilesetgen import Sheet, Builder, colors, TRANS, LAYER_COVERED, LAYER_NORMAL

MB_POND_WATER = 16      # include/constants/metatile_behaviors.h - surfable

SRC = "J:/ROM Hack Project/tilesets_raw/FairyCleanedUp"
RAW = SRC + "/FairyTileset.png"
AUTO = SRC + "/PutThisIntoAutotile"
OUT = "J:/ROM Hack Project/pokeemerald-expansion/data/tilesets/secondary/fairy_town"
PATCHED = os.path.join(os.path.dirname(__file__), "fairy_patched.png")

# ---- 1. strip RMXP "unused tile" markers ----
# They are a flat neutral cell (white or 197-grey) crossed by saturated blue /
# orange guide lines. The real art here is all low-saturation pastel, so
# "flat neutral background + fully saturated accents" identifies them cleanly.
im = Image.open(RAW).convert("RGBA")
a = np.array(im)


def is_marker(blk):
    op = blk[:, :, 3] >= 128
    if not op.any():
        return False
    cnt = {}
    for y in range(blk.shape[0]):
        for x in range(blk.shape[1]):
            if op[y, x]:
                k = tuple(int(v) for v in blk[y, x, :3])
                cnt[k] = cnt.get(k, 0) + 1
    if len(cnt) > 8:
        return False
    tot = sum(cnt.values())

    def neutral(k):
        return max(k) - min(k) < 12 and min(k) >= 190

    bg = sum(v for k, v in cnt.items() if neutral(k))
    rest = [k for k in cnt if not neutral(k)]
    return bg > tot * 0.65 and all(max(k) - min(k) >= 90 for k in rest)


cleared = 0
for r in range(a.shape[0] // 32):
    for c in range(a.shape[1] // 32):
        if is_marker(a[r * 32:(r + 1) * 32, c * 32:(c + 1) * 32]):
            a[r * 32:(r + 1) * 32, c * 32:(c + 1) * 32] = 0
            cleared += 1
print(f"cleared {cleared} RMXP blank-marker cells")
Image.fromarray(a).save(PATCHED)

# ---- 2. build ----
S = Sheet(PATCHED, half=True)


def mt(r, c):
    return c * 16, r * 16


# plain mossy ground, used as the under-layer for anything with transparency
GROUND = S.metatile_px(*mt(21, 2))
assert all(TRANS not in t for t in GROUND), "base fill must be opaque"

b = Builder("FairyTown", sel_width=8)

# ---------------- terrain ----------------
b.terrain(S, *mt(19, 0), 8, 6, label="mossy_ground", newline=True)
# the rows 82-86 brown dirt ovals are dropped - the Dirt2 autotile below covers
# the same job as a proper nine-slice patch

# RMXP autotiles -> a 3x3 nine-slice patch set each
for fn, lbl in [("Light grassFairy.png", "auto_grass"),
                ("Light grassFairy2.png", "auto_grass_pink"),
                ("FairyPath.png", "auto_path"),
                ("FairyPath2.png", "auto_path_pink"),
                ("Dirt2.png", "auto_dirt")]:
    sh = Sheet(os.path.join(AUTO, fn), half=True, white_key=True)
    b.terrain(sh, *mt(1, 0), 3, 3, label=lbl, base=GROUND)
b.newline()

# cliffs / rock walls
b.terrain(S, *mt(10, 0), 5, 9, label="pink_cliff_wall", base=GROUND, newline=True)
# The rows 4-6 purple crystal boulder is dropped: 47 tiles for one non-tileable
# decoration, which the purple water and sprouts need more.

# ---------------- purple water ----------------
# Surfable: every metatile here carries MB_POND_WATER. The pool interior tiles
# are flat purple, so they repeat to make a pond of any size.
# (FountainFairy.png is not usable as a nine-slice - it is a fountain-shaped
# animated autotile whose template cells are mostly RMXP blank markers.)
b.terrain(S, *mt(93, 0), 3, 4, label="purple_pool", base=GROUND, newline=True,
          attr=MB_POND_WATER, attr_over=MB_POND_WATER | LAYER_COVERED)
b.terrain(S, *mt(26, 0), 8, 3, label="water_shore", base=GROUND, newline=True)
b.terrain(S, *mt(35, 3), 5, 1, label="water_sand_fill", newline=True)
# the rows 29-31 peach recolour of the shore set is dropped: same shapes, and it
# cost ~40 tiles / ~18 colours that the trees and props need instead.

# ---------------- underwater grass / flowers / sprouts ----------------
b.terrain(S, *mt(32, 0), 8, 3, label="sprouts_flowers", base=GROUND, newline=True)

# ---------------- trees ----------------
b.obj(S, (65, 1075, 95, 1120), GROUND, cols=2, rows=3, label="pine_purple",
      newline=True, top_rows=2)
b.obj(S, (64, 1121, 96, 1168), GROUND, cols=2, rows=3, label="pine_tan", top_rows=2)
b.obj(S, (0, 1184, 48, 1264), GROUND, cols=3, rows=5, label="tree_pink_round",
      newline=True, top_rows=4)
# the rows 79-81 blossom bush is dropped to make room for the water and sprouts

# ---------------- mossy fairy buildings ----------------
b.obj(S, (17, 2914, 85, 2976), GROUND, cols=5, rows=4, label="moss_building_a",
      newline=True, top_rows=3)

# ---------------- props ----------------
b.terrain(S, *mt(128, 0), 5, 4, label="rocks", base=GROUND, newline=True)
# 4x4 with the basin centred, on its own row band, so the rim has a clear margin
# of ground on every side instead of sitting flush against the selector edge.
b.obj(S, (84, 2045, 124, 2090), GROUND, cols=4, rows=4, label="fountain",
      newline=True, halign="center", valign="center")
b.obj(S, (8, 1860, 24, 1899), GROUND, cols=1, rows=3, label="lamppost_curved",
      top_rows=2)
b.obj(S, (102, 1834, 123, 1854), GROUND, cols=2, rows=2, label="statue_pig", top_rows=1)

b.pack()
b.write(OUT)
b.manifest()
