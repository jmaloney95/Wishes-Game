"""fear town.png -> data/tilesets/secondary/fear_town (secondary, pairs with gTileset_General)."""
import sys
sys.path.insert(0, __import__("os").path.dirname(__file__))
from tilesetgen import Sheet, Builder, PrimaryBase, LAYER_NORMAL, LAYER_COVERED

RAW = "J:/ROM Hack Project/tilesets_raw/fear town.png"
TS = "J:/ROM Hack Project/pokeemerald-expansion/data/tilesets"
OUT = TS + "/secondary/fear_town"

# sheet content sits 8px low relative to the 16px metatile grid
S = Sheet(RAW, shift=(0, -8))
GRASS = PrimaryBase(TS + "/primary/general", 1).entries   # gTileset_General plain grass

b = Builder("FearTown", sel_width=8)

BUILDINGS = [
    ("pokecenter_red",    (0, 1, 80, 76),     5, 5),
    ("pokecenter_purple", (98, 5, 158, 73),   4, 5),
    ("mart_one_screen",   (1, 95, 63, 168),   4, 5),
    ("mart_two_screens",  (82, 96, 144, 168), 4, 5),
]
for name, bb, cols, rows in BUILDINGS:
    b.obj(S, bb, GRASS, cols=cols, rows=rows, label=name, newline=True,
          attr=LAYER_COVERED, top_rows=rows - 1, top_attr=LAYER_NORMAL)

b.newline()
PATCHES = [
    ("dirt_patch_light", (4, 172, 44, 212),  3, 3),
    ("dirt_patch_dark",  (52, 172, 92, 212), 3, 3),
    ("dirt_small_light", (0, 216, 32, 248),  2, 2),
    ("dirt_small_dark",  (48, 216, 80, 248), 2, 2),
]
for name, bb, cols, rows in PATCHES:
    b.obj(S, bb, GRASS, cols=cols, rows=rows, label=name,
          attr=LAYER_COVERED, valign="center", halign="center")

b.pack()
b.write(OUT)
b.manifest()
