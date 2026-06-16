#!/usr/bin/env python3
# Carve a row-range out of the Gold/Silver overworld sheet into a Porytiles SOURCE folder.
#
# The full sheet is 8 x 695 metatiles (~5560) -- WAY too big for one GBA tileset
# (limit ~512 unique 8x8 tiles / ~6-7 palettes). So carve out ONE area at a time.
# This makes a FLAT source (all art on bottom.png; middle/top transparent). After it
# compiles, move any roof/treetop metatiles into top.png so the player walks BEHIND them.
#
# Run in WSL from this folder (tilesets_raw/full tileset/):
#   python3 make_porytiles_source.py <row_start> <row_end> <out_folder_name>
#   example:  python3 make_porytiles_source.py 0 24 gs_town_a
#     -> carves metatile rows 0..23 (the top of the sheet) into ./gs_town_a/
#
# Then compile (PRIMARY = a standalone town tileset; no paired primary needed):
#   PE="/mnt/j/ROM Hack Project/pokeemerald-expansion"
#   porytiles compile-primary -dual-layer -Wall \
#     -o "$PE/data/tilesets/primary/gs_town_a" \
#     gs_town_a \
#     "$PE/include/constants/metatile_behaviors.h"
from PIL import Image
import sys, os

SRC      = "golden_silver_tileset_by_skillmen_d2siwpu.png"
GS_BG    = (247, 151, 61)    # the sheet's empty/background color
MAGENTA  = (255, 0, 255)     # what porytiles treats as transparent/empty

if len(sys.argv) != 4:
    print("usage: python3 make_porytiles_source.py <row_start> <row_end> <out_folder>")
    sys.exit(1)
r0, r1, out = int(sys.argv[1]), int(sys.argv[2]), sys.argv[3]

im = Image.open(SRC).convert("RGB"); W, H = im.size
maxrow = H // 16
if not (0 <= r0 < r1 <= maxrow):
    print(f"row range must be within 0..{maxrow}"); sys.exit(1)

crop = im.crop((0, r0 * 16, W, r1 * 16)).convert("RGB")
W2, H2 = crop.size
px = crop.load()

def to15(c):                       # round 8-bit RGB to GBA 5-bit-per-channel precision
    return ((c[0] >> 3) << 3, (c[1] >> 3) << 3, (c[2] >> 3) << 3)

# 1) GS background -> magenta (porytiles transparent); all other px -> GBA color precision
#    (the 15-bit round also merges the near-duplicate colors that cause -Wcolor-precision-loss)
for y in range(H2):
    for x in range(W2):
        c = px[x, y]
        if abs(c[0]-GS_BG[0]) < 40 and abs(c[1]-GS_BG[1]) < 40 and abs(c[2]-GS_BG[2]) < 40:
            px[x, y] = MAGENTA
        else:
            px[x, y] = to15(c)

# 2) GBA 4bpp hard limit: each 8x8 tile may have at most 15 opaque colors (+transparent).
#    Any tile over that gets median-cut quantized down to 15 so porytiles won't throw
#    "too many unique colors".
clamped = 0
for ty in range(H2 // 8):
    for tx in range(W2 // 8):
        box = (tx*8, ty*8, tx*8+8, ty*8+8)
        tile = crop.crop(box)
        data = list(tile.getdata())
        opq = [i for i, c in enumerate(data) if c != MAGENTA]
        if len({data[i] for i in opq}) <= 15:
            continue
        strip = Image.new("RGB", (len(opq), 1)); strip.putdata([data[i] for i in opq])
        q = list(strip.quantize(colors=15, method=Image.MEDIANCUT).convert("RGB").getdata())
        for k, i in enumerate(opq):
            data[i] = to15(q[k])
        tile.putdata(data); crop.paste(tile, box); clamped += 1

os.makedirs(f"{out}/anim", exist_ok=True)
crop.save(f"{out}/bottom.png")
Image.new("RGB", crop.size, MAGENTA).save(f"{out}/middle.png")
Image.new("RGB", crop.size, MAGENTA).save(f"{out}/top.png")

ncolors = len({c for c in crop.getdata() if c != MAGENTA})
print(f"wrote {out}/  ({W2}x{H2} px = 8 x {r1-r0} metatiles, {ncolors} colors, {clamped} tiles color-clamped)")
if ncolors > 90:
    print("  NOTE: >90 colors total -> porytiles re-fits into 6 primary palettes; if it errors on")
    print("        palette count, carve a smaller row range.")
