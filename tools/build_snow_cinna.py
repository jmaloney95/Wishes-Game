#!/usr/bin/env python3
# Build the gTileset_SnowCinna PRIMARY tileset from CustomTilesetSnow.png.
# Run from the repo root in WSL:  python3 tools/build_snow_cinna.py   (needs: pip install pillow)
# Writes data/tilesets/primary/snow_cinna/{tiles.png, palettes/NN.pal, metatiles.bin,
# metatile_attributes.bin}; `make` then converts + links it.
#
# Layout = simple ORDERED CATALOG: each feature is a whole rectangular block, stacked
# top-to-bottom in an 8-wide canvas (PANE_W=8 = Porymap default selector width) with a
# blank row between blocks, so every piece reads as a recognizable picture.
# Objects are 2-layer (snow on bottom, item on top, NORMAL layer) so snow shows behind
# them and tree canopies render walk-behind.
#
# To add more later WITHOUT breaking painted maps, APPEND new regions to the END of
# REGIONS (don't reorder existing ones) so existing metatile IDs stay stable.
from PIL import Image
import random, struct, os
random.seed(3)

SRC = os.environ.get("SNOW_SRC", "../tilesets_raw/SnowTileset/CustomTilesetSnow.png")
OUT = "data/tilesets/primary/snow_cinna"
NCOL, NPAL, PSZ, PANE_W, GAP = 60, 6, 15, 8, 1

im = Image.open(SRC).convert("RGBA"); W, H = im.size; opx = im.load()
def used(mx, my):
    op = red = white = 0
    for yy in range(16):
        for xx in range(16):
            r, g, b, a = opx[mx*16+xx, my*16+yy]
            if a < 40: continue
            op += 1
            if r > 150 and g < 90 and b < 90: red += 1
            if r > 200 and g > 200 and b > 200: white += 1
    if op == 0: return False
    if red >= 4 and white >= op*0.4: return False
    if op < 130 and red >= op*0.5: return False
    return True

# (label, c0, c1, r0, r1, is_object)   c1/r1 exclusive. Snow-only set with crisp trees
# (adding ice/rock/sign here dilutes the shared 6-palette budget and blurs the trees).
REGIONS = [
    ("snow ground",   2, 4, 2, 4, False),
    ("snowy grass",   0, 7, 56, 62, False),
    ("snow bushes",   0, 6, 44, 46, True),
    ("grass sprigs",  0, 10, 46, 50, True),
    ("snow pine",     2, 6, 62, 68, True),
    ("snow tree",     0, 5, 68, 74, True),
    ("big snow tree", 0, 6, 74, 86, True),
    ("snowman",       6, 9, 44, 46, True),
]
SNOWFILL = (2, 3)

allc = set([SNOWFILL])
for _, c0, c1, r0, r1, _o in REGIONS:
    for my in range(r0, r1):
        for mx in range(c0, c1): allc.add((mx, my))
allc = sorted(allc)
allpix = []
for (mx, my) in allc:
    for yy in range(16):
        for xx in range(16):
            r, g, b, a = opx[mx*16+xx, my*16+yy]
            if a >= 128: allpix.append((r, g, b))
NCOL = min(NCOL, len(set(allpix)))
qimg = Image.new("RGB", (len(allpix), 1)); qimg.putdata(allpix)
qp = qimg.quantize(colors=NCOL, method=Image.MEDIANCUT)
pl = qp.getpalette(); basecols = [tuple(pl[i*3:i*3+3]) for i in range(NCOL)]
from collections import Counter
wt = Counter(qp.tobytes())
def d2(a, b): return (a[0]-b[0])**2 + (a[1]-b[1])**2 + (a[2]-b[2])**2
cents = random.sample(basecols, NPAL)
for _ in range(60):
    g = [[] for _ in range(NPAL)]
    for ci, c in enumerate(basecols): g[min(range(NPAL), key=lambda j: d2(c, cents[j]))].append(ci)
    cents = [(tuple(sum(basecols[ci][k]*wt[ci] for ci in gg)/(sum(wt[ci] for ci in gg) or 1) for k in range(3))
              if gg else random.choice(basecols)) for gg in g]
palettes = [[basecols[ci] for ci in sorted(gg, key=lambda ci: -wt[ci])[:PSZ]] or [(0,0,0)] for gg in g]
def nin(pal, c): return min(range(len(pal)), key=lambda i: d2(pal[i], c))
def gt(mx, my, ox, oy):
    return [(None if opx[mx*16+ox*8+xx, my*16+oy*8+yy][3] < 128 else opx[mx*16+ox*8+xx, my*16+oy*8+yy][:3])
            for yy in range(8) for xx in range(8)]
def idx(raw):
    p = min(range(NPAL), key=lambda j: sum(0 if c is None else d2(palettes[j][nin(palettes[j], c)], c) for c in raw))
    return p, tuple(0 if c is None else nin(palettes[p], c)+1 for c in raw)
def Hf(t): return tuple(t[y*8+(7-x)] for y in range(8) for x in range(8))
def Vf(t): return tuple(t[(7-y)*8+x] for y in range(8) for x in range(8))
uniq = []; kmap = {}
def add(raw):
    p, ix = idx(raw)
    for cand, xf, yf in ((ix,0,0), (Hf(ix),1,0), (Vf(ix),0,1), (Hf(Vf(ix)),1,1)):
        if (p, cand) in kmap: return (kmap[(p, cand)], xf, yf, p)
    ti = len(uniq); uniq.append((p, ix)); kmap[(p, ix)] = ti; return (ti, 0, 0, p)
def mt(mx, my): return [add(gt(mx, my, ox, oy)) for oy in range(2) for ox in range(2)]
snowfill = mt(*SNOWFILL)
BLANK = ([(0,0,0,0)]*4, [(0,0,0,0)]*4)

canvas = {}; cy = 0; placements = []
for label, c0, c1, r0, r1, isobj in REGIONS:
    w, h = c1-c0, r1-r0; placements.append((label, cy, w, h))
    for my in range(r0, r1):
        for mx in range(c0, c1):
            if not used(mx, my): continue
            lx, ly = mx-c0, my-r0
            canvas[(lx, cy+ly)] = (list(snowfill), mt(mx, my)) if isobj else (mt(mx, my), [(0,0,0,0)]*4)
    cy += h + GAP
TH = cy
ordered = [canvas.get((x, y), BLANK) for y in range(TH) for x in range(PANE_W)]
assert len(uniq) <= 511 and len(ordered) <= 512
print(f"snow_cinna: tiles={len(uniq)} (free {511-len(uniq)})  metatiles={len(ordered)} (free {512-len(ordered)})  palettes={NPAL}")
for label, y, w, h in placements: print(f"  '{label}': pane row {y}  ({w}x{h})")

os.makedirs(OUT + "/palettes", exist_ok=True)
NT = len(uniq) + 1; cols = 16; rows = (NT + cols - 1)//cols
timg = Image.new("P", (cols*8, rows*8), 0)
p0 = [v for c in (palettes[0] + [(0,0,0)]*(15-len(palettes[0]))) for v in c]
timg.putpalette([255,0,255] + p0 + [0,0,0]*(256-16))
pm = [0]*(cols*8*rows*8)
def put(ti, ix):
    tx, ty = (ti % cols)*8, (ti // cols)*8
    for yy in range(8):
        for xx in range(8): pm[(ty+yy)*cols*8+tx+xx] = ix[yy*8+xx]
put(0, [0]*64)
for i, (p, ix) in enumerate(uniq): put(i+1, ix)
timg.putdata(pm); timg.save(OUT + "/tiles.png")
for j in range(NPAL):
    c16 = [(255,0,255)] + palettes[j] + [(0,0,0)]*(15-len(palettes[j]))
    open(f"{OUT}/palettes/{j:02d}.pal", "w").write("JASC-PAL\n0100\n16\n" + "".join(f"{int(c[0])} {int(c[1])} {int(c[2])}\n" for c in c16))
with open(OUT + "/metatiles.bin", "wb") as f:
    for bot, top in ordered:
        for (ti, xf, yf, p) in bot: f.write(struct.pack("<H", ((ti+1)&0x3FF)|(xf<<10)|(yf<<11)|((p&0xF)<<12)))
        for (ti, xf, yf, p) in top:
            v = 0 if (ti == 0 and p == 0) else ((ti+1)&0x3FF)|(xf<<10)|(yf<<11)|((p&0xF)<<12)
            f.write(struct.pack("<H", v))
with open(OUT + "/metatile_attributes.bin", "wb") as f:
    for _ in ordered: f.write(struct.pack("<H", 0))   # behavior 0, layer NORMAL; set per-tile in Porymap
print("wrote", OUT)
