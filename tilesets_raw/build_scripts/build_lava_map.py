"""porytiles_lava/lava_map.png -> data/tilesets/secondary/lava_map.

Keeps rock terrain, cliffs/rock edges, caves, lava, planks and stairs.
Drops every grass/tree/greenery row and the buildings at the bottom of the sheet.
"""
import os, sys
from PIL import Image
sys.path.insert(0, os.path.dirname(__file__))
from tilesetgen import Sheet, Builder, LAYER_COVERED

RAW = "J:/ROM Hack Project/tilesets_raw/porytiles_lava/lava_map.png"
OUT = "J:/ROM Hack Project/pokeemerald-expansion/data/tilesets/secondary/lava_map"
PATCHED = os.path.join(os.path.dirname(__file__), "lava_patched.png")

# ---- 1. patch the sheet: strip RMXP "blank" X markers, refill the real holes ----
im = Image.open(RAW).convert("RGBA")


def cell(r, c):
    return im.crop((c * 16, r * 16, (c + 1) * 16, (r + 1) * 16))


def clear(r, c):
    im.paste(Image.new("RGBA", (16, 16), (0, 0, 0, 0)), (c * 16, r * 16))


X_CELLS = [(1, 6), (2, 5), (2, 6), (2, 7), (13, 4), (13, 5), (13, 6), (14, 4), (14, 5),
           (14, 6), (41, 4), (43, 1), (44, 1), (44, 7), (45, 7), (48, 1), (48, 2), (48, 3),
           (49, 2), (52, 1), (53, 6), (53, 7), (59, 6), (59, 7), (60, 6), (60, 7), (64, 0),
           (64, 1), (64, 7), (74, 3), (74, 5), (74, 6), (74, 7)]
for rc in X_CELLS:
    clear(*rc)

# holes that sit inside artwork we keep, refilled from a clean donor cell
FILLS = [
    ((41, 4), (52, 0)),   # mountain-top plateau interior <- solid grass
    ((43, 1), (52, 0)),   # cliff pass floor
    ((44, 1), (52, 0)),
    ((48, 1), (47, 2)),   # lava pool interior <- solid lava
    ((48, 2), (47, 2)),
    ((48, 3), (47, 2)),
    ((49, 2), (47, 2)),
]
donors = {src: cell(*src).copy() for _, src in FILLS}
for (dr, dc), src in FILLS:
    im.paste(donors[src], (dc * 16, dr * 16))
im.save(PATCHED)

# ---- 2. build ----
S = Sheet(PATCHED)
ROCK = S.metatile_px(3 * 16, 50 * 16)          # plain rock ground, used as the under-layer

b = Builder("LavaMap", sel_width=8)

# rock cliffs / mountains / caves / plank bridge / torches / boulders
b.terrain(S, 0, 37 * 16, 8, 9, label="cliffs_caves_planks", base=ROCK, newline=True)
# lava pool, lava jets, ground variants
b.terrain(S, 0, 46 * 16, 8, 5, label="lava_pool_ground", base=ROCK, newline=True)
# rock pillar walls, lava edges, planks, stairs
b.terrain(S, 0, 54 * 16, 8, 10, label="rockwall_lava_stairs", base=ROCK, newline=True)

b.pack()
b.write(OUT)
b.manifest()
