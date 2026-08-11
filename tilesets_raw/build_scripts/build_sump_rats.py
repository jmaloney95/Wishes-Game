"""SumpRats_extracted/Tileset/sumprats.png -> data/tilesets/secondary/sump_rats
(secondary, pairs with gTileset_General). Slum settlement: swamp terrain,
shipping-container shacks, patchwork huts, market props."""
import sys
sys.path.insert(0, __import__("os").path.dirname(__file__))
from tilesetgen import Sheet, Builder, PrimaryBase, LAYER_NORMAL, LAYER_COVERED

import os
_BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RAW = os.path.join(_BASE, "SumpRats_extracted/Tileset/sumprats.png")
TS = os.path.join(os.path.dirname(_BASE), "pokeemerald-expansion/data/tilesets")
OUT = TS + "/secondary/sump_rats"

MB_POND_WATER = 0x10

S = Sheet(RAW, half=True)   # sheet is drawn at 2x -- verified 96.8% uniform 2x2 blocks
GRASS = PrimaryBase(TS + "/primary/general", 1).entries

b = Builder("SumpRats", sel_width=8)

# Joe's curated set (2026-07-30): terrain/pokeballs/signs/crates cut for
# palette + tile room. Huts and shacks, vent drains, mushroom barrels + soda
# can, one plant pot, skull flag, graffiti, and the critters (trubbish-likes
# + grimer) as static decorations.
BUILDINGS = [
    ("cont_green",  (0, 271, 64, 328),   4, 4),
    ("cont_red",    (0, 471, 64, 528),   4, 4),
    ("aframe_hut",  (10, 722, 70, 784),   4, 4),
    ("trap_shack",  (11, 950, 69, 1008),  4, 4),
    ("yurt",        (14, 1025, 66, 1080), 4, 4),
]
for name, bb, cols, rows in BUILDINGS:
    b.obj(S, bb, GRASS, cols=cols, rows=rows, label=name, newline=True,
          attr=LAYER_COVERED, top_rows=rows - 2, top_attr=LAYER_NORMAL)

b.newline()
PROPS = [
    ("drain_pipes",  (80, 360, 96, 400),   1, 3),
    ("vent_grate",   (113, 352, 127, 366), 1, 1),
    ("shroom_barrels", (1, 1200, 46, 1231), 3, 2),
    ("flower_pot",   (108, 1242, 128, 1279), 2, 3),
    ("graffiti",     (0, 1092, 32, 1114),  2, 2),
]
for name, bb, cols, rows in PROPS:
    b.obj(S, bb, GRASS, cols=cols, rows=rows, label=name,
          attr=LAYER_COVERED, valign="bottom")

b.newline()
b.obj(S, (80, 1328, 127, 1374), GRASS, cols=3, rows=3, label="skull_banner",
      attr=LAYER_COVERED, top_rows=2, top_attr=LAYER_NORMAL, newline=True)
CRITTERS = [
    ("trubbish_green",  (9, 1126, 38, 1151),  2, 2),
    ("trubbish_purple", (62, 1130, 82, 1149), 2, 2),
    ("grimer",          (89, 1153, 127, 1189), 3, 3),
]
for name, bb, cols, rows in CRITTERS:
    b.obj(S, bb, GRASS, cols=cols, rows=rows, label=name,
          attr=LAYER_COVERED, valign="bottom")

b.pack()
b.write(OUT)
b.manifest()
