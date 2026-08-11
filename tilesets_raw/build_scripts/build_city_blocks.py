"""pokemon_gen_3_customized_building_sprites_*.png -> secondary/city_blocks
(secondary, pairs with gTileset_General).

The sheet's mega-complexes are 680-2158 metatiles each -- physically beyond
the 512-metatile secondary budget -- so this import takes the sheet's MODULAR
TOWER KIT (cap strips + column/window units, top right of the sheet) plus its
compact signature buildings. The kit assembles green towers of ANY footprint
directly in porymap."""
import sys
sys.path.insert(0, __import__("os").path.dirname(__file__))
from tilesetgen import Sheet, Builder, PrimaryBase, LAYER_NORMAL, LAYER_COVERED

import os
_BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RAW = os.path.join(_BASE, "pokemon_gen_3_customized_building_sprites_by_x_xdark_slayerx_x_d8j4oyg.png")
TS = os.path.join(os.path.dirname(_BASE), "pokeemerald-expansion/data/tilesets")
OUT = TS + "/secondary/city_blocks"

S = Sheet(RAW, white_key=True)
GRASS = PrimaryBase(TS + "/primary/general", 1).entries

b = Builder("CityBlocks", sel_width=16)

# Modular green-tower kit: caps + one repeating column + roof piece + logo
# wall. Tile these in porymap to raise towers of any footprint -- the sheet's
# mega-complexes (680-2158 metatiles) can never fit a secondary whole.
# Shelf-packed side by side: padding blanks count against the metatile cap.
b.terrain(S, 1342, 32, 8, 2, label="kit_caps_a", base=GRASS, newline=True)
b.terrain(S, 1470, 32, 8, 2, label="kit_caps_b", base=GRASS)
b.obj(S, (1878, 207, 1926, 303), GRASS, cols=3, rows=6, label="kit_column",
      attr=LAYER_COVERED, top_rows=5, top_attr=LAYER_NORMAL, newline=True)
b.obj(S, (1436, 78, 1534, 130), GRASS, cols=7, rows=4, label="kit_roofpiece",
      attr=LAYER_COVERED, top_rows=3, top_attr=LAYER_NORMAL)
b.obj(S, (1939, 106, 2035, 151), GRASS, cols=6, rows=3, label="kit_logo_wall",
      attr=LAYER_COVERED, top_rows=2, top_attr=LAYER_NORMAL)

b.obj(S, (2107, 24, 2232, 276), GRASS, cols=8, rows=16, label="tower_blue",
      attr=LAYER_COVERED, top_rows=14, top_attr=LAYER_NORMAL, newline=True)
b.obj(S, (292, 22, 380, 234), GRASS, cols=6, rows=14, label="tower_orange",
      attr=LAYER_COVERED, top_rows=12, top_attr=LAYER_NORMAL)
b.obj(S, (592, 20, 754, 90), GRASS, cols=11, rows=5, label="shop_red_pc",
      attr=LAYER_COVERED, top_rows=3, top_attr=LAYER_NORMAL, newline=True)

b.pack()
b.write(OUT)
b.manifest()
