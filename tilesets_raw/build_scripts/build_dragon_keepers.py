"""terrible jared dragon keepers.png -> data/tilesets/secondary/dragon_keepers
(secondary, pairs with gTileset_General). Mud-brick dragon-cult settlement:
tarp-roofed houses, fire plates, dragon statues, arch, nest perch, banner."""
import sys
sys.path.insert(0, __import__("os").path.dirname(__file__))
from tilesetgen import Sheet, Builder, PrimaryBase, LAYER_NORMAL, LAYER_COVERED

import os
_BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RAW = os.path.join(_BASE, "terrible jared dragon keepers.png")
TS = os.path.join(os.path.dirname(_BASE), "pokeemerald-expansion/data/tilesets")
OUT = TS + "/secondary/dragon_keepers"

S = Sheet(RAW, half=True)   # sheet is drawn at 2x -- verified 100% uniform 2x2 blocks
GRASS = PrimaryBase(TS + "/primary/general", 1).entries

b = Builder("DragonKeepers", sel_width=8)

# Joe's curated set (2026-07-30): three houses + statue + arch + banner PLUS
# the three egg-incubating plates and both pillars (bare + nest). Out by
# call: salamence sprites, small statue, fourth house; the fenced house
# also gave way -- the plates + pillars cost ~180 tiles.
HOUSES = [
    ("house_tower", (10, 6, 116, 111),  7, 7),
    ("house_court", (1, 405, 116, 501),  8, 6),
]
for name, bb, cols, rows in HOUSES:
    b.obj(S, bb, GRASS, cols=cols, rows=rows, label=name, newline=True,
          attr=LAYER_COVERED, top_rows=rows - 2, top_attr=LAYER_NORMAL)

b.newline()
PROPS = [
    ("statue_dragon", (0, 658, 56, 719),   4, 4, 2),
    ("arch",          (70, 660, 120, 718), 4, 4, 2),
    ("fire_plate",    (1, 524, 61, 581),   4, 4, 1),
    ("plate_bare",    (65, 524, 125, 581), 4, 4, 1),
    ("plate_small",   (1, 584, 32, 616),   2, 2, 0),
    ("pillar",        (1, 737, 31, 831),   2, 6, 4),
    ("nest_perch",    (33, 737, 95, 800),  4, 4, 2),
]
for name, bb, cols, rows, top in PROPS:
    b.obj(S, bb, GRASS, cols=cols, rows=rows, label=name,
          attr=LAYER_COVERED, top_rows=top, top_attr=LAYER_NORMAL)

b.pack()
b.write(OUT)
b.manifest()
