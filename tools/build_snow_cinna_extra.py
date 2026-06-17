#!/usr/bin/env python3
# Build the gTileset_SnowCinnaExtra SECONDARY tileset (buildings) from CustomTilesetSnow.png.
# Pairs with the SnowCinna primary. Run from repo root in WSL:
#   python3 tools/build_snow_cinna_extra.py     (needs: pip install pillow)
# Writes data/tilesets/secondary/snow_cinna_extra/{tiles.png, palettes/NN.pal,
# metatiles.bin, metatile_attributes.bin}; `make` converts + links it.
#
# Contents: Pokemon Center, Mart, snow house, statue. Each building is a whole block,
# stacked in order. Buildings are 2-layer (snow on the BOTTOM layer, building art on the
# TOP layer, NORMAL layer type) so transparent roof edges show snow and the player walks
# behind them. SECONDARY encoding: tile indices stored as 512+local, palettes as 6..12.
# Add more buildings later by APPENDING to REGIONS (don't reorder) to keep metatile IDs stable.
from PIL import Image
import random, struct, os
random.seed(5)

SRC = os.environ.get("SNOW_SRC", "../tilesets_raw/SnowTileset/CustomTilesetSnow.png")
OUT = "data/tilesets/secondary/snow_cinna_extra"
NPAL, PSZ, NCOL = 7, 15, 95          # secondary gets 7 palettes (engine slots 6..12)
PAL_BASE, TILE_BASE = 6, 512         # secondary palette slot / tile index offsets

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
    other = op - white - red
    if white >= op*0.6 and red >= 4 and other <= op*0.10: return False  # red-X empty marker
    if op < 130 and red >= op*0.5: return False
    return True

# (label, c0, c1, r0, r1)   c1/r1 exclusive
REGIONS = [
    ("PC",     0, 10, 223, 232),   # full width incl. right roof curl + window column
    ("Mart",   0, 8, 232, 240),
    ("house",  0, 10, 189, 198),   # full width incl. right pink trim
    ("gym",    0, 10, 242, 256),
]
SNOWFILL = (2, 3)

def cells(c0, c1, r0, r1): return [(mx, my) for my in range(r0, r1) for mx in range(c0, c1) if used(mx, my)]
allc = set([SNOWFILL])
for _, c0, c1, r0, r1 in REGIONS: allc |= set(cells(c0, c1, r0, r1))
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
BLANK = ([(0,0,0,0)]*4, [(0,0,0,0)]*4)  # top entries stay 0 => primary blank tile 0

def block(c0, c1, r0, r1):
    cs = cells(c0, c1, r0, r1)
    minx = min(c for c, _ in cs); miny = min(r for _, r in cs)
    maxx = max(c for c, _ in cs); maxy = max(r for _, r in cs)
    # 2-layer: bottom = snow, top = building art (transparent edges show snow)
    cd = {(mx-minx, my-miny): (list(snowfill), mt(mx, my)) for (mx, my) in cs}
    return maxx-minx+1, maxy-miny+1, cd
blocks = [(lbl,) + block(c0, c1, r0, r1) for lbl, c0, c1, r0, r1 in REGIONS]
PANE_W = max(w for _, w, h, _cd in blocks)
canvas = {}; cy = 0; placements = []
for lbl, w, h, cd in blocks:
    placements.append((lbl, cy, w, h))
    for (lx, ly), m in cd.items(): canvas[(lx, cy+ly)] = m
    cy += h + 1
TH = cy
ordered = [canvas.get((x, y), BLANK) for y in range(TH) for x in range(PANE_W)]
assert len(uniq) <= 511 and len(ordered) <= 512
print(f"snow_cinna_extra: tiles={len(uniq)} (free {511-len(uniq)})  metatiles={len(ordered)} (free {512-len(ordered)})  palettes={NPAL} (slots 6..12)")
for lbl, y, w, h in placements: print(f"  {lbl}: pane row {y}  ({w}x{h})  -> set Porymap 'metatiles per row' to {PANE_W}")

os.makedirs(OUT + "/palettes", exist_ok=True)
NT = len(uniq); cols = 16; rows = (NT + cols - 1)//cols   # no reserved blank tile (blanks use primary tile 0)
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
# 13 palette files: 00-05 dummy, 06-12 = the 7 building palettes
for slot in range(13):
    if slot < PAL_BASE:
        c16 = [(255,0,255)] + [(0,0,0)]*15
    else:
        pal = palettes[slot - PAL_BASE]
        c16 = [(255,0,255)] + pal + [(0,0,0)]*(15-len(pal))
    open(f"{OUT}/palettes/{slot:02d}.pal", "w").write("JASC-PAL\n0100\n16\n" + "".join(f"{int(c[0])} {int(c[1])} {int(c[2])}\n" for c in c16))
with open(OUT + "/metatiles.bin", "wb") as f:
    for md in ordered:
        if md is BLANK:                       # empty catalog gap -> all primary blank tile 0
            f.write(struct.pack("<8H", 0,0,0,0,0,0,0,0)); continue
        bot, top = md
        for (ti, xf, yf, p) in bot + top:     # real secondary tiles: offset by 512 / palette 6..12
            f.write(struct.pack("<H", ((TILE_BASE + ti) & 0x3FF) | (xf << 10) | (yf << 11) | (((PAL_BASE + p) & 0xF) << 12)))
# Door metatiles (the building doorstep tiles the player warps on) get
# MB_NON_ANIMATED_DOOR(=95) so warp_events placed on them actually fire.
# Local IDs taken from the painted FrostwoodTown map.bin (PC 85/86, Mart 174/175, Gym 385/386).
DOOR_MT = {85, 86, 174, 175, 385, 386}
MB_NON_ANIMATED_DOOR = 95
with open(OUT + "/metatile_attributes.bin", "wb") as f:
    for i, _ in enumerate(ordered):
        f.write(struct.pack("<H", MB_NON_ANIMATED_DOOR if i in DOOR_MT else 0))
print("wrote", OUT)
