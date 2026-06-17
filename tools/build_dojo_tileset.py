#!/usr/bin/env python3
# Build gTileset_Dojo SECONDARY from ekat99's halcyon dojo sheet (tilesets_raw).
# Strips the red-X "empty" marker cells + background, repacks metatiles 8 per row.
# Run from repo root:  python3 tools/build_dojo_tileset.py
from PIL import Image
import numpy as np, struct, os, random
from collections import Counter
random.seed(7)

SRC = "../tilesets_raw/pokemon_halcyon___dojo_by_ekat99_dfbfw1s.png"
OUT = "data/tilesets/secondary/dojo"
NPAL, PAL_BASE, TILE_BASE, PAL_FILES = 7, 6, 512, 13

im = Image.open(SRC)
ai = np.array(im)                       # palette indices
rgbpal = im.getpalette()
def irgb(i): return tuple(rgbpal[i*3:i*3+3])
H, W = ai.shape[:2]
MW, MH = W//16, H//16
TRANS_IDX = 55                          # black = transparency/filler (beige 0 = sand, opaque!)

cells = {}
for my in range(MH):
    for mx in range(MW):
        cells[(mx,my)] = ai[my*16:(my+1)*16, mx*16:(mx+1)*16]

def is_empty(c):
    u, cnt = np.unique(c, return_counts=True)
    d = dict(zip(u.tolist(), cnt.tolist()))
    if len(d) == 1 and TRANS_IDX in d: return True
    if d.get(33,0) + d.get(34,0) >= 200: return True   # red-X marker cells
    return False

kept, seen = [], set()
for my in range(MH):
    for mx in range(MW):
        c = cells[(mx,my)]
        if is_empty(c): continue
        k = c.tobytes()
        if k in seen: continue
        seen.add(k); kept.append((mx,my))
print(f"sheet {MW}x{MH}; kept {len(kept)} unique non-empty metatiles")
assert len(kept) <= 512

# palette clustering (colors from kept cells, bg -> transparent)
cols = Counter()
for mx,my in kept:
    for i in cells[(mx,my)].ravel().tolist():
        if i != TRANS_IDX: cols[irgb(i)] += 1
base = sorted(cols, key=cols.get, reverse=True)
npal = min(NPAL, max(1, -(-len(base)//15)))
print("colors:", len(base), "-> palettes:", npal)
def d2(x,y): return sum((int(i)-int(j))**2 for i,j in zip(x,y))
if len(base) <= 15:
    palettes = [base]
else:
    cents = random.sample(base, npal)
    for _ in range(60):
        g = [[] for _ in range(npal)]
        for c in base: g[min(range(npal), key=lambda j: d2(c,cents[j]))].append(c)
        cents = [tuple(sum(c[k]*cols[c] for c in gg)/(sum(cols[c] for c in gg) or 1) for k in range(3))
                 if gg else random.choice(base) for gg in g]
    palettes = [sorted(gg, key=lambda c:-cols[c])[:15] for gg in g if gg]
while len(palettes) < npal: palettes.append([(0,0,0)])
def nin(pal,c): return min(range(len(pal)), key=lambda i: d2(pal[i],c))
def tile_raw(cell,ox,oy):
    return [None if cell[oy*8+yy,ox*8+xx] == TRANS_IDX else irgb(cell[oy*8+yy,ox*8+xx])
            for yy in range(8) for xx in range(8)]
def idx(raw):
    p = min(range(len(palettes)), key=lambda j: sum(0 if c is None else d2(palettes[j][nin(palettes[j],c)],c) for c in raw))
    return p, tuple(0 if c is None else nin(palettes[p],c)+1 for c in raw)
def Hf(t): return tuple(t[y*8+(7-x)] for y in range(8) for x in range(8))
def Vf(t): return tuple(t[(7-y)*8+x] for y in range(8) for x in range(8))
uniq, kmap = [], {}
def add(raw):
    p, ix = idx(raw)
    for cand,xf,yf in ((ix,0,0),(Hf(ix),1,0),(Vf(ix),0,1),(Hf(Vf(ix)),1,1)):
        if (p,cand) in kmap: return (kmap[(p,cand)],xf,yf,p)
    ti = len(uniq); uniq.append((p,ix)); kmap[(p,ix)] = ti; return (ti,0,0,p)
add([None]*64)
mts = []
for mx,my in kept:
    mts.append([add(tile_raw(cells[(mx,my)],ox,oy)) for oy in range(2) for ox in range(2)])
print("unique tiles:", len(uniq))
if len(uniq) > 512:
    # cluster rarest-into-nearest within the same palette group
    use = Counter()
    for quads in mts:
        for (ti,xf,yf,p) in quads: use[ti] += 1
    palarr = [np.array([(255,0,255)]+pal+[(0,0,0)]*(15-len(pal)), np.float32) for pal in palettes]
    vecs = [palarr[p][np.array(ix)].reshape(-1) for p,ix in uniq]
    alive = [True]*len(uniq)
    remap = list(range(len(uniq)))
    while sum(alive) > 512:
        live = [i for i in range(1,len(uniq)) if alive[i]]
        rare = min(live, key=lambda i: use.get(i,0))
        same = [i for i in live if i != rare and uniq[i][0] == uniq[rare][0]]
        tgt = min(same, key=lambda i: float(((vecs[i]-vecs[rare])**2).sum()))
        alive[rare] = False
        use[tgt] = use.get(tgt,0) + use.get(rare,0)
        for k in range(len(remap)):
            if remap[k] == rare: remap[k] = tgt
    newid, n = {}, 0
    for i in range(len(uniq)):
        if alive[i]: newid[i] = n; n += 1
    uniq = [uniq[i] for i in range(len(uniq)) if alive[i]]
    mts = [[(newid[remap[ti]],xf,yf,p) for (ti,xf,yf,p) in quads] for quads in mts]
    print("clustered to", len(uniq), "tiles")

os.makedirs(OUT+"/palettes", exist_ok=True)
NT = len(uniq); rows = (NT+15)//16
sheet = np.zeros((max(8,rows*8),128), np.uint8)
for i,(p,ix) in enumerate(uniq):
    tx,ty = (i%16)*8,(i//16)*8
    for yy in range(8):
        for xx in range(8): sheet[ty+yy,tx+xx] = ix[yy*8+xx]
timg = Image.fromarray(sheet, mode="P")
p0 = [v for c in (palettes[0]+[(0,0,0)]*(15-len(palettes[0]))) for v in c]
timg.putpalette([255,0,255]+p0+[0,0,0]*(256-16))
timg.save(OUT+"/tiles.png")
for slot in range(PAL_FILES):
    pi = slot - PAL_BASE
    if 0 <= pi < len(palettes):
        pal = palettes[pi]; c16 = [(255,0,255)]+pal+[(0,0,0)]*(15-len(pal))
    else: c16 = [(255,0,255)]+[(0,0,0)]*15
    open(f"{OUT}/palettes/{slot:02d}.pal","w").write(
        "JASC-PAL\n0100\n16\n"+"".join(f"{int(c[0])} {int(c[1])} {int(c[2])}\n" for c in c16))
with open(OUT+"/metatiles.bin","wb") as f:
    for quads in mts:
        for (ti,xf,yf,p) in quads:
            f.write(struct.pack("<H", ((TILE_BASE+ti)&0x3FF)|(xf<<10)|(yf<<11)|(((PAL_BASE+p)&0xF)<<12)))
        for _ in range(4):
            f.write(struct.pack("<H", (TILE_BASE)&0x3FF | ((PAL_BASE&0xF)<<12)))
with open(OUT+"/metatile_attributes.bin","wb") as f:
    f.write(struct.pack(f"<{len(mts)}H", *([0]*len(mts))))
print(f"gTileset_Dojo: tiles={NT} metatiles={len(mts)} palettes={len(palettes)}")
