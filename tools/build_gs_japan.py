#!/usr/bin/env python3
# Build gTileset_GsJapan SECONDARY from the Gold/Silver "skillmen" sheet: the three
# Japanese sections (Ecruteak houses, pagoda towers, grand temples) combined into ONE
# secondary tileset. k-means packs every color into 7 secondary palettes (slots 6-12) --
# porytiles' 6/7-palette solver chokes on this dense mix sheet, so we pack it ourselves.
# Pairs with the General primary; each building's bottom layer references the primary's
# standard grass metatile (tiles 2,3 pal 2) so transparent edges show real grass seamlessly.
#
# Run from repo ROOT in WSL:  python3 tools/build_gs_japan.py   (needs pillow)
# Writes data/tilesets/secondary/gs_japan/{tiles.png, palettes/NN.pal, metatiles.bin,
# metatile_attributes.bin}; `make` converts + links it.
from PIL import Image
import random, struct, os
random.seed(7)

SRC   = os.environ.get("GS_SRC", "../tilesets_raw/full tileset/golden_silver_tileset_by_skillmen_d2siwpu.png")
OUT   = os.environ.get("OUT", "data/tilesets/secondary/gs_japan")
NPAL, PSZ, PAL_BASE, TILE_BASE = 7, 15, 6, 512
BG    = (247, 151, 61)        # sheet background = transparent
GROUND = (104, 176, 72)       # grass baked under building edges (tweak to match your primary)
# (label, col0, col1, row0, row1)  -- the 3 Japanese sections, full 8-wide
REGIONS = [("temple", 0, 8, 653, 663), ("redpagoda", 0, 6, 614, 620), ("darkbldg_path", 0, 8, 565, 572)]

im = Image.open(SRC).convert("RGB"); W, H = im.size; px = im.load()
def isbg(c): return abs(c[0]-BG[0]) < 40 and abs(c[1]-BG[1]) < 40 and abs(c[2]-BG[2]) < 40
def to15(c): return ((c[0] >> 3) << 3, (c[1] >> 3) << 3, (c[2] >> 3) << 3)
def used(mx, my):
    return sum(0 if isbg(px[mx*16+x, my*16+y]) else 1 for y in range(16) for x in range(16)) >= 8
def regcells(c0, c1, r0, r1): return [(mx, my) for my in range(r0, r1) for mx in range(c0, c1) if used(mx, my)]

allc = []
for _, c0, c1, r0, r1 in REGIONS: allc += regcells(c0, c1, r0, r1)
allc = sorted(set(allc))
allpix = []
for (mx, my) in allc:
    for y in range(16):
        for x in range(16):
            c = px[mx*16+x, my*16+y]
            if not isbg(c): allpix.append(to15(c))
NCOL = min(110, len(set(allpix)))
qimg = Image.new("RGB", (len(allpix), 1)); qimg.putdata(allpix)
qp = qimg.quantize(colors=NCOL, method=Image.MEDIANCUT)
pl = qp.getpalette(); basecols = [tuple(pl[i*3:i*3+3]) for i in range(NCOL)]
from collections import Counter
wt = Counter(qp.tobytes())
def d2(a, b): return (a[0]-b[0])**2 + (a[1]-b[1])**2 + (a[2]-b[2])**2
cents = random.sample(basecols, NPAL)
for _ in range(80):
    g = [[] for _ in range(NPAL)]
    for ci, c in enumerate(basecols): g[min(range(NPAL), key=lambda j: d2(c, cents[j]))].append(ci)
    cents = [(tuple(sum(basecols[ci][k]*wt[ci] for ci in gg)/(sum(wt[ci] for ci in gg) or 1) for k in range(3))
              if gg else random.choice(basecols)) for gg in g]
palettes = [[basecols[ci] for ci in sorted(gg, key=lambda ci: -wt[ci])[:PSZ]] or [(0,0,0)] for gg in g]
def nin(pal, c): return min(range(len(pal)), key=lambda i: d2(pal[i], c))
def gt(mx, my, ox, oy):
    return [(None if isbg(px[mx*16+ox*8+xx, my*16+oy*8+yy]) else to15(px[mx*16+ox*8+xx, my*16+oy*8+yy]))
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
BLANK = ("BLANK",)
def block(c0, c1, r0, r1):
    cs = regcells(c0, c1, r0, r1)
    minx = min(c for c, _ in cs); miny = min(r for _, r in cs)
    maxx = max(c for c, _ in cs); maxy = max(r for _, r in cs)
    cd = {(mx-minx, my-miny): mt(mx, my) for (mx, my) in cs}
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
print(f"gs_japan: tiles={len(uniq)} (limit 512), metatiles={len(ordered)} (limit 512), palettes={NPAL}, pane_width={PANE_W}")
for lbl, y, w, h in placements: print(f"  {lbl}: pane row {y} ({w}x{h})")
if len(uniq) > 512: print("  !! OVER 512 TILES -- drop a section or shrink a range")

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
PRIM_GRASS = [(2, 2), (3, 2), (3, 2), (2, 2)]   # General primary grass metatile bottom (NW,NE,SW,SE): (tile, pal)
with open(OUT + "/metatiles.bin", "wb") as f:
    for md in ordered:
        # bottom layer = the primary's standard grass tiles, so transparent edges show real grass
        for (tid, pal) in PRIM_GRASS:
            f.write(struct.pack("<H", (tid & 0x3FF) | ((pal & 0xF) << 12)))
        if md is BLANK:                              # blank cell = pure grass (top transparent)
            for _ in range(4): f.write(struct.pack("<H", 0))
            continue
        for (ti, xf, yf, p) in md:                   # top layer = building art (secondary tiles 512+)
            f.write(struct.pack("<H", ((TILE_BASE + ti) & 0x3FF) | (xf << 10) | (yf << 11) | (((PAL_BASE + p) & 0xF) << 12)))
with open(OUT + "/metatile_attributes.bin", "wb") as f:
    for _ in ordered: f.write(struct.pack("<H", 0))
print("wrote", OUT)
