"""middle_district_1x_q96.png -> data/tilesets/secondary/middle_district
(secondary; pairs with gTileset_MiddleDistrictGround as the Shin-Tokyo kit).

Sliced from Emeiry's "Middle District" render for Odisea (permission granted;
credit Emeiry / Odisea by ekat99). The render is drawn at 2x (verified 100%%
uniform 2x2 blocks) -- middle_district_1x.png is the LOSSLESS half-scale
version, quantized to 96 colors to collapse the baked lighting gradients."""
import sys
sys.path.insert(0, __import__("os").path.dirname(__file__))
from tilesetgen import Sheet, Builder, PrimaryBase, LAYER_NORMAL, LAYER_COVERED

import os
_BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RAW = os.path.join(_BASE, "middle_district_1x_q96.png")
TS = os.path.join(os.path.dirname(_BASE), "pokeemerald-expansion/data/tilesets")
OUT = TS + "/secondary/middle_district"

S = Sheet(RAW, magenta_key=False)
GRASS = PrimaryBase(TS + "/primary/general", 1).entries

b = Builder("MiddleDistrict", sel_width=16)

# --- buildings (true 1x scale: doors are one tile) --------------------------
b.obj(S, (328, 232, 424, 328), GRASS, cols=6, rows=6, label="arcade",
      attr=LAYER_COVERED, top_rows=4, top_attr=LAYER_NORMAL, newline=True)
b.obj(S, (384, 136, 448, 200), GRASS, cols=4, rows=4, label="facade_glow",
      attr=LAYER_COVERED, top_rows=3, top_attr=LAYER_NORMAL)
b.obj(S, (0, 200, 64, 272), GRASS, cols=4, rows=5, label="tower_left",
      attr=LAYER_COVERED, top_rows=4, top_attr=LAYER_NORMAL)
b.obj(S, (256, 184, 328, 216), GRASS, cols=5, rows=2, label="roof_kit",
      attr=LAYER_COVERED, top_rows=1, top_attr=LAYER_NORMAL, newline=True)
b.obj(S, (512, 264, 544, 328), GRASS, cols=2, rows=4, label="lattice_wall",
      attr=LAYER_COVERED, top_rows=3, top_attr=LAYER_NORMAL)
b.obj(S, (640, 312, 712, 360), GRASS, cols=5, rows=3, label="wall_fans",
      attr=LAYER_COVERED, top_rows=2, top_attr=LAYER_NORMAL)
b.obj(S, (824, 136, 888, 200), GRASS, cols=4, rows=4, label="kiosk",
      attr=LAYER_COVERED, top_rows=3, top_attr=LAYER_NORMAL, newline=True)
b.obj(S, (64, 48, 176, 128), GRASS, cols=7, rows=5, label="storefront",
      attr=LAYER_COVERED, top_rows=3, top_attr=LAYER_NORMAL)
b.obj(S, (848, 8, 960, 128), GRASS, cols=7, rows=8, label="dark_tower",
      attr=LAYER_COVERED, top_rows=7, top_attr=LAYER_NORMAL, newline=True)

# ===== APPEND-ONLY EXTENSIONS (v5 content, frozen palettes) =================
# Everything above is FROZEN (painted maps reference it). storefront_base =
# the store's bottom rows (door + light pillars); stack it under the
# existing 7x5 storefront in porymap.
b.newline()
b.obj(S, (64, 128, 176, 160), GRASS, cols=7, rows=2, label="storefront_base",
      attr=LAYER_COVERED)

# ===== v6 append: plain ocean water (animated via TilesetAnim_MiddleDistrict)
MB_OCEAN_WATER = 0x16
W = Sheet(_os.path.join(_BASE, "md_water_meta.png"), magenta_key=False) if False else None
import os as _os
from tilesetgen import Sheet as _Sheet
WS = _Sheet(_os.path.join(_BASE, "md_water_meta.png"), magenta_key=False)
b.newline()
b.terrain(WS, 0, 0, 1, 1, label="ocean_water", attr=MB_OCEAN_WATER)

WATER_COLS = [(82, 106, 213), (106, 131, 213), (139, 164, 222), (172, 197, 230)]
def _snap4(c):
    return ((c[0] >> 3) << 3, (c[1] >> 3) << 3, (c[2] >> 3) << 3)
# ===== v7 append: the teal kiosk COMPLETE (old import was right-shifted) =====
b.newline()
KS = _Sheet(_os.path.join(_BASE, "md_kiosk_sym.png"), magenta_key=False)
b.obj(KS, (0, 0, 80, 80), GRASS, cols=5, rows=5, label="kiosk_full",
      attr=LAYER_COVERED, top_rows=3, top_attr=LAYER_NORMAL)

b.pack_append(_os.path.join(_BASE, "md_v6_snapshot/secondary"), 416,
              extra_pal_colors={6: [_snap4(c) for c in WATER_COLS]},
              kill_labels=("kiosk[",))
b.write(OUT)
b.manifest()
