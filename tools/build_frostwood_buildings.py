#!/usr/bin/env python3
# Build the gTileset_FrostwoodBuildings SECONDARY (snow buildings) from a Gold/Silver
# tileset rip. Replaces the oversized SnowCinnaExtra; pairs with the SnowCinna primary.
# Run from repo root in WSL:  python3 tools/build_frostwood_buildings.py  (needs pillow)
# Writes data/tilesets/secondary/frostwood_buildings/{tiles.png, palettes/NN.pal,
# metatiles.bin, metatile_attributes.bin}; `make` converts + links it.
#
# Buildings (snow-roofed, GS-style, ~5x5 metatiles each): Pokemon Center, Gym, Mart,
# a house, and a market shop. 2-layer (snow bottom + building top, NORMAL) so the
# transparent roof edges show snow and the player walks behind them. SECONDARY encoding:
# tile = 512+local, palette slots 6..12.
# After Joe repaints Frostwood with these, fill DOOR_MT below (door metatile local IDs
# from the new FrostwoodTown map.bin) so the door warps fire (MB_NON_ANIMATED_DOOR).
from PIL import Image
import random, struct, os
random.seed(7)

SRC = os.environ.get("GS_SRC", "../tilesets_raw/SnowTownBuildings/golden_silver_tileset_by_skillmen_d2siwpu.png")
OUT = "data/tilesets/secondary/frostwood_buildings"
NPAL, PSZ, PAL_BASE, TILE_BASE = 7, 15, 6, 512
BG = (247, 151, 61)           # sheet background = transparent
SNOWCOL = (229, 229, 238)     # flat snow base baked under building edges
DOOR_MT = set()               # TODO: set door metatile local IDs after repainting Frostwood
MB_NON_ANIMATED_DOOR = 95

im = Image.open(SRC).convert("RGB"); W, H = im.size; px = im.load()
def isbg(c): return abs(c[0]-BG[0]) < 34 and abs(c[1]-BG[1]) < 34 and abs(c[2]-BG[2]) < 34
# (label, c0, c1, r0, r1)  GS-sheet metatile coords (sheet is 8 metatiles wide)
REGIONS = [("PC", 0, 5, 371, 376), ("Gym", 0, 4, 376, 381), ("Mart", 4, 8, 377, 381),
           ("house", 5, 8, 371, 376), ("shop", 0, 5, 381, 385)]
def used(mx, my):
    return sum(0 if isbg(px[mx*16+x, my*16+y]) else 1 for y in range(16) for x in range(16)) >= 8
def regcells(c0, c1, r0, r1): return [(mx, my) for my in range(r0, r1) for mx in range(c0, c1) if used(mx, my)]
allc = []
for _, c0, c1, r0, r1 in REGIONS: allc += regcells(c0, c1, r0, r1)
allc = sorted(set(allc))
allpix = [SNOWCOL]
for (mx, my) in allc:
    for y in range(16):
        for x in range(16):
            c = px[mx*16+x, my*16+y]
            if not isbg(c): allpix.append(c)
NCOL = min(90, len(set(allpix)))
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
    return [(None if isbg(px[mx*16+ox*8+xx, my*16+oy*8+yy]) else px[mx*16+ox*8+xx, my*16+oy*8+yy])
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
snowfill = [add([SNOWCOL]*64) for _ in range(4)]
def mt(mx, my): return [add(gt(mx, my, ox, oy)) for oy in range(2) for ox in range(2)]
BLANK = ("BLANK",)
def block(c0, c1, r0, r1):
    cs = regcells(c0, c1, r0, r1)
    minx = min(c for c, _ in cs); miny = min(r for _, r in cs)
    maxx = max(c for c, _ in cs); maxy = max(r for _, r in cs)
    cd = {(mx-minx, my-miny): (list(snowfill), mt(mx, my)) for (mx, my) in cs}
    return maxx-minx+1, maxy-miny+1, cd
blocks = [(lbl,) + block(c0, c1, r0, r1) for lbl, c0, c1, r0, r1 in REGIONS]
PANE_W = max(w for _, w, h, _ in blocks)
canvas = {}; cy = 0; placements = []
for lbl, w, h, cd in blocks:
    placements.append((lbl, cy, w, h))
    for (lx, ly), m in cd.items(): canvas[(lx, cy+ly)] = m
    cy += h + 1
TH = cy
ordered = [canvas.get((x, y), BLANK) for y in range(TH) for x in range(PANE_W)]
assert len(uniq) <= 511 and len(ordered) <= 512
print(f"frostwood_buildings: tiles={len(uniq)} metatiles={len(ordered)} palettes={NPAL}")
for lbl, y, w, h in placements: print(f"  {lbl}: pane row {y} ({w}x{h})  -> set Porymap 'metatiles per row' to {PANE_W}")

os.makedirs(OUT + "/palettes", exist_ok=True)
NT = len(uniq); cols = 16; rows = (NT + cols - 1)//cols
timg = Image.new("P", (cols*8, max(8, rows*8)), 0)
p0 = [v for c in (palettes[0] + [(0,0,0)]*(15-len(palettes[0]))) for v in c]
timg.putpalette([255,0,255] + p0 + [0,0,0]*(256-16))
pm = [0]*(cols*8*max(1, rows)*8)
def put(ti, ix):
    tx, ty = (ti % cols)*8, (ti // cols)*8
    for yy in range(8):
        for xx in range(8): pm[(ty+yy)*cols*8+tx+xx] = ix[yy*8+xx]
for i, (p, ix) in enumerate(uniq): put(i, ix)
timg.putdata(pm); timg.save(OUT + "/tiles.png")
for slot in range(13):
    if slot < PAL_BASE: c16 = [(255,0,255)] + [(0,0,0)]*15
    else:
        pal = palettes[slot - PAL_BASE]; c16 = [(255,0,255)] + pal + [(0,0,0)]*(15-len(pal))
    open(f"{OUT}/palettes/{slot:02d}.pal", "w").write("JASC-PAL\n0100\n16\n" + "".join(f"{int(c[0])} {int(c[1])} {int(c[2])}\n" for c in c16))
with open(OUT + "/metatiles.bin", "wb") as f:
    for md in ordered:
        if md is BLANK:
            f.write(struct.pack("<8H", 0,0,0,0,0,0,0,0)); continue
        bot, top = md
        for (ti, xf, yf, p) in bot + top:
            f.write(struct.pack("<H", ((TILE_BASE + ti) & 0x3FF) | (xf << 10) | (yf << 11) | (((PAL_BASE + p) & 0xF) << 12)))
with open(OUT + "/metatile_attributes.bin", "wb") as f:
    for i in range(len(ordered)):
        f.write(struct.pack("<H", MB_NON_ANIMATED_DOOR if i in DOOR_MT else 0))
print("wrote", OUT)
