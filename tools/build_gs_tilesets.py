#!/usr/bin/env python3
# Build Gold/Silver-style tilesets from the carved tilesets_raw sources so they are
# pickable in Porymap:  gTileset_GsTownA (PRIMARY) + gTileset_GsJapanHouses (SECONDARY).
# Sheet arrangement is preserved (8 metatiles per row, like the carve), flat layer-0 art.
# Run from repo root:  python3 tools/build_gs_tilesets.py   (needs pillow)
from PIL import Image
import struct, os, random
random.seed(7)

MAG = (255, 0, 255)
def d2(a,b): return (a[0]-b[0])**2 + (a[1]-b[1])**2 + (a[2]-b[2])**2

def build(src, out, npal, pal_base, tile_base, pal_files):
    im = Image.open(src).convert("RGB"); W,H = im.size; px = im.load()
    MW, MH = W//16, H//16
    def ismag(c): return c == MAG
    # collect colors
    cols = {}
    for y in range(H):
        for x in range(W):
            c = px[x,y]
            if not ismag(c): cols[c] = cols.get(c,0)+1
    base = sorted(cols, key=cols.get, reverse=True)
    # cluster into npal palettes of <=15 (k-means on colors, weighted)
    npal = min(npal, max(1, -(-len(base)//15)))   # fewest palettes that fit the colors
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
    def tile_raw(mx,my,ox,oy):
        return [None if ismag(px[mx*16+ox*8+xx, my*16+oy*8+yy]) else px[mx*16+ox*8+xx, my*16+oy*8+yy]
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
    add([None]*64)                                   # tile 0 = blank
    mts = []
    for my in range(MH):
        for mx in range(MW):
            quads = [add(tile_raw(mx,my,ox,oy)) for oy in range(2) for ox in range(2)]
            mts.append(quads)
    assert len(uniq) <= 512, f"{out}: {len(uniq)} tiles > 512"
    assert len(mts) <= 512, f"{out}: {len(mts)} metatiles > 512"
    # write
    os.makedirs(out+"/palettes", exist_ok=True)
    NT = len(uniq); rows = (NT+15)//16
    timg = Image.new("P", (128, max(8, rows*8)), 0)
    p0 = [v for c in (palettes[0]+[(0,0,0)]*(15-len(palettes[0]))) for v in c]
    timg.putpalette([255,0,255]+p0+[0,0,0]*(256-16))
    pm = [0]*(128*max(8, rows*8))
    for i,(p,ix) in enumerate(uniq):
        tx,ty = (i%16)*8,(i//16)*8
        for yy in range(8):
            for xx in range(8): pm[(ty+yy)*128+tx+xx] = ix[yy*8+xx]
    timg.putdata(pm); timg.save(out+"/tiles.png")
    for slot in range(pal_files):
        pi = slot - pal_base
        if 0 <= pi < len(palettes):
            pal = palettes[pi]; c16 = [MAG]+pal+[(0,0,0)]*(15-len(pal))
        else: c16 = [MAG]+[(0,0,0)]*15
        open(f"{out}/palettes/{slot:02d}.pal","w").write(
            "JASC-PAL\n0100\n16\n"+"".join(f"{int(c[0])} {int(c[1])} {int(c[2])}\n" for c in c16))
    with open(out+"/metatiles.bin","wb") as f:
        for quads in mts:
            for (ti,xf,yf,p) in quads:
                f.write(struct.pack("<H", ((tile_base+ti)&0x3FF)|(xf<<10)|(yf<<11)|(((pal_base+p)&0xF)<<12)))
            for _ in range(4):                       # layer 1 empty
                f.write(struct.pack("<H", (tile_base)&0x3FF | ((pal_base&0xF)<<12)))
    with open(out+"/metatile_attributes.bin","wb") as f:
        f.write(struct.pack(f"<{len(mts)}H", *([0]*len(mts))))
    print(f"{out}: tiles={NT} metatiles={len(mts)} palettes={len(palettes)} (8 metatiles/row)")

RAW = "../tilesets_raw/full tileset"
build(f"{RAW}/gs_town_a/bottom.png",       "data/tilesets/primary/gs_town_a",          6, 0, 0,   6)
build(f"{RAW}/gs_japan_houses/bottom.png", "data/tilesets/secondary/gs_japan_houses",  7, 6, 512, 13)
