"""middle_district_1x_q96.png -> data/tilesets/primary/middle_district
(custom PRIMARY -- pairs with gTileset_MiddleDistrict; new Shin-Tokyo kit).

Concrete-city ground + street props. The telephone wires and lamp pillars
are EXTRACTED to transparency (color-keyed cutouts) so their metatiles put
only the wire/pillar art in the TOP layer: sprites walk beneath wires and
behind pillars, over the baked ground. Each cutout ships in road and
sidewalk ground variants. Credit Emeiry / Odisea by ekat99."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from tilesetgen import Sheet, Builder, LAYER_NORMAL, LAYER_COVERED
from PIL import Image

_BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RAW = os.path.join(_BASE, "middle_district_1x_q96.png")
CUT = os.path.join(_BASE, "md_cutouts")
TS = os.path.join(os.path.dirname(_BASE), "pokeemerald-expansion/data/tilesets")
OUT = TS + "/primary/middle_district"
os.makedirs(CUT, exist_ok=True)

# --- transparent cutouts ----------------------------------------------------
src = Image.open(RAW).convert("RGBA")

def keep_dark_lines(p):
    r, g, b, a = p
    lum = (r + g + b) // 3
    bluish = b > r + 8 and b > g + 8
    greyish = abs(r - g) < 14 and abs(g - b) < 14
    return (bluish and lum < 140) or (greyish and lum < 110)

def keep_not_ground(p):
    r, g, b, a = p
    lum = (r + g + b) // 3
    warm = r > b + 16
    return not (warm and lum > 118)

def cutout(name, box, keep):
    im = src.crop(box)
    px = im.load()
    for y in range(im.height):
        for x in range(im.width):
            if not keep(px[x, y]):
                px[x, y] = (0, 0, 0, 0)
    im.save(os.path.join(CUT, name))
    return os.path.join(CUT, name)

WIRES_A = cutout("wires_a.png", (192, 208, 272, 256), keep_dark_lines)
WIRES_B = cutout("wires_b.png", (544, 320, 568, 364), keep_dark_lines)
def keep_cool(p):
    r, g, b, a = p
    lum = (r + g + b) // 3
    warm = r > b + 16
    return not (warm and lum > 105)

def cutout_dilated(name, box, lum_thr=90, grow=2):
    im = src.crop(box)
    px = im.load()
    W, H = im.size
    mask = [[(px[x, y][0] + px[x, y][1] + px[x, y][2]) // 3 < lum_thr
             for x in range(W)] for y in range(H)]
    for _ in range(grow):
        nm = [row[:] for row in mask]
        for y in range(H):
            for x in range(W):
                if not mask[y][x]:
                    for dx, dy in ((1,0),(-1,0),(0,1),(0,-1)):
                        if 0 <= x+dx < W and 0 <= y+dy < H and mask[y+dy][x+dx]:
                            nm[y][x] = True; break
        mask = nm
    for y in range(H):
        for x in range(W):
            if not mask[y][x]:
                px[x, y] = (0, 0, 0, 0)
    im.save(os.path.join(CUT, name))
    return os.path.join(CUT, name)

PILLAR  = cutout("pillar.png",  (526, 494, 546, 538), keep_not_ground)
HOLE    = cutout_dilated("hole.png", (576, 360, 616, 400))
CAR     = cutout("car.png", (152, 124, 208, 168), keep_cool)

S = Sheet(RAW, magenta_key=False)
WA = Sheet(WIRES_A, magenta_key=False)
WB = Sheet(WIRES_B, magenta_key=False)
PL = Sheet(PILLAR, magenta_key=False)
HO = Sheet(HOLE, magenta_key=False)
CA = Sheet(CAR, magenta_key=False)

b = Builder("MiddleDistrictGround", sel_width=16, primary=True)

# --- ground textures --------------------------------------------------------
b.terrain(S, 256, 496, 4, 2, label="sidewalk_curb_road", newline=True)
b.terrain(S, 192, 504, 2, 2, label="road_dashes")
b.terrain(S, 344, 496, 4, 2, label="crosswalk_h")
b.terrain(S, 216, 336, 2, 2, label="crosswalk_v")
b.terrain(S, 376, 712, 4, 2, label="plaza_pale")
b.terrain(S, 512, 84, 2, 1, label="dirt_patch")
b.terrain(S, 368, 576, 2, 2, label="steps_vertical", newline=True)

# --- structures moved from the secondary ------------------------------------
BASE = S.metatile_px(280, 504)      # plain sidewalk
ROAD = S.metatile_px(200, 520)      # plain road
b.obj(S, (152, 328, 216, 408), BASE, cols=4, rows=5, label="metro",
      attr=LAYER_COVERED, top_rows=3, top_attr=LAYER_NORMAL, newline=True)
b.obj(S, (88, 344, 136, 376), BASE, cols=3, rows=2, label="louvre_panel",
      attr=LAYER_COVERED, top_rows=1, top_attr=LAYER_NORMAL)

# --- street props -----------------------------------------------------------
b.newline()
b.obj(S, (472, 336, 520, 384), BASE, cols=3, rows=3, label="fountain",
      attr=LAYER_COVERED, top_rows=1, top_attr=LAYER_NORMAL)
b.obj(S, (152, 128, 200, 168), BASE, cols=3, rows=3, label="car",
      attr=LAYER_COVERED, top_rows=1, top_attr=LAYER_NORMAL)
b.obj(S, (508, 148, 532, 172), BASE, cols=2, rows=2, label="garbage_cans",
      attr=LAYER_COVERED, top_rows=1, top_attr=LAYER_NORMAL)
b.obj(S, (576, 360, 616, 400), BASE, cols=3, rows=3, label="wall_hole",
      attr=LAYER_COVERED)

# --- transparent overlays: wires above sprites, ground below ---------------
b.newline()
b.obj(WA, (0, 0, 80, 48), ROAD, cols=5, rows=3, label="wires_road",
      attr=LAYER_NORMAL, top_rows=3, top_attr=LAYER_NORMAL)
b.obj(WA, (0, 0, 80, 48), BASE, cols=5, rows=3, label="wires_side",
      attr=LAYER_NORMAL, top_rows=3, top_attr=LAYER_NORMAL, newline=True)
b.obj(WB, (0, 0, 24, 44), ROAD, cols=2, rows=3, label="wire_drop_road",
      attr=LAYER_NORMAL, top_rows=3, top_attr=LAYER_NORMAL)
b.obj(WB, (0, 0, 24, 44), BASE, cols=2, rows=3, label="wire_drop_side",
      attr=LAYER_NORMAL, top_rows=3, top_attr=LAYER_NORMAL)
b.obj(PL, (0, 0, 20, 44), BASE, cols=1, rows=3, label="pillar_side",
      attr=LAYER_NORMAL, top_rows=2, top_attr=LAYER_NORMAL)
b.obj(PL, (0, 0, 20, 44), ROAD, cols=1, rows=3, label="pillar_road",
      attr=LAYER_NORMAL, top_rows=2, top_attr=LAYER_NORMAL)

# ===== APPEND-ONLY EXTENSIONS (v5 content, frozen palettes) =================
# Joe painted maps against the v4 build: everything above this line is FROZEN
# (order, sizes, coords). New pieces go BELOW, and pack() runs with the v4
# palettes so existing tiles keep their exact colors and ids.
b.newline()
b.obj(S, (184, 400, 216, 432), BASE, cols=2, rows=2, label="metro_stairs",
      attr=LAYER_COVERED)
b.obj(S, (208, 424, 224, 440), BASE, cols=1, rows=1, label="metro_bin",
      attr=LAYER_COVERED)
b.obj(HO, (0, 0, 40, 40), BASE, cols=3, rows=3, label="hole_side",
      attr=LAYER_NORMAL, top_rows=3, top_attr=LAYER_NORMAL)
b.obj(CA, (0, 0, 56, 44), BASE, cols=4, rows=3, label="car_side",
      attr=LAYER_NORMAL, top_rows=3, top_attr=LAYER_NORMAL)
b.obj(CA, (0, 0, 56, 44), ROAD, cols=4, rows=3, label="car_road",
      attr=LAYER_NORMAL, top_rows=3, top_attr=LAYER_NORMAL)

# ===== v6 appends: street-corner kit, wall strip, satellite + slab roofs ====
b.newline()
b.terrain(S, 184, 448, 5, 5, label="street_corner_kit", newline=True)
b.terrain(S, 216, 392, 2, 2, label="wall_strip")
b.obj(S, (856, 336, 920, 392), BASE, cols=4, rows=4, label="roof_satellite",
      attr=LAYER_COVERED, top_rows=2, top_attr=LAYER_NORMAL)
b.obj(S, (416, 184, 480, 264), BASE, cols=4, rows=5, label="roof_slab",
      attr=LAYER_COVERED, top_rows=2, top_attr=LAYER_NORMAL)

# ===== v7 append (RETIRED): the extracted park fence was misaligned --------
# kept in the item list so ids stay stable; killed via kill_labels below.
def keep_fence(p):
    r, g, b_, a = p
    lum = (r + g + b_) // 3
    reddish = r > g + 12 and r > b_
    return reddish and lum < 150
FENCE = cutout("fence.png", (216, 768, 312, 800), keep_fence)
FE = Sheet(FENCE, magenta_key=False)
b.newline()
b.obj(FE, (0, 0, 48, 32), BASE, cols=3, rows=2, label="broken_fence",
      attr=LAYER_COVERED, top_rows=1, top_attr=LAYER_NORMAL, newline=True)

# ===== v8: the red lattice fence (posts + X-panels + patched panel) =========
b.newline()
b.terrain(S, 544, 408, 4, 2, label="lattice_fence", newline=True)

b.pack_append(os.path.join(_BASE, "md_v7_snapshot/primary"), 464,
              kill_labels=("broken_fence[",))
b.write(OUT)
b.manifest()

# post-write: duplicate the lattice fence broken-panel metatile (0x1E2) as a
# DOOR-behavior variant at id 0x1F0. The red-alert transition drops it onto
# the hideout warp at (16,12) via setmetatile -- the warp only works once
# the panel is kicked out.
import struct as _struct
MB_NON_ANIMATED_DOOR = 0x60
_mb = bytearray(open(OUT + "/metatiles.bin", "rb").read())
_ab = bytearray(open(OUT + "/metatile_attributes.bin", "rb").read())
_n = len(_mb) // 16
assert _n <= 0x1F0, _n
while _n < 0x1F0:
    _mb += bytes(16); _ab += bytes(2); _n += 1
_mb += _mb[0x1E2*16:(0x1E2+1)*16]
_ab += _struct.pack("<H", MB_NON_ANIMATED_DOOR)
open(OUT + "/metatiles.bin", "wb").write(_mb)
open(OUT + "/metatile_attributes.bin", "wb").write(_ab)
print("  door-variant metatile appended at 0x1F0")
