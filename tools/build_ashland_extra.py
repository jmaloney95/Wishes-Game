#!/usr/bin/env python3
# APPENDS the shikari grass+cobble ground and the (bannerless) dark market booth
# to the EXISTING gTileset_AshlandTrees SECONDARY -- without regenerating it, so
# the Porymap-authored tree metatiles/collision are preserved. New tiles go AFTER
# the current ones (local 272+); new palettes fill free slots 10-11; new metatiles
# append after the current 144. Idempotent-ish: re-running rebuilds from the
# ORIGINAL baseline saved on first run (ashland_trees/.baseline_*), so it never
# double-appends.
#   source : ../tilesets_raw/shikari_tileset_supplement...png  (2x art -> SS=2)
#   run    : python tools/build_ashland_extra.py    then  make
import os, struct, shutil, json
from collections import deque
from PIL import Image

RAW  = r"J:\ROM Hack Project\tilesets_raw"
SHIK = os.path.join(RAW, "shikari_tileset_supplement_by_elinthind_djllm4z.png")
DST  = r"J:\ROM Hack Project\pokeemerald-expansion\data\tilesets\secondary\ashland_trees"
TRANS = (255, 0, 255)
SS = 2
NUM_TILES_IN_PRIMARY = 512
FIRST_FREE_PAL = 10                  # ashland uses 6-9

# (name, c0,r0,c1,r1, palslot) in the 2x source's 8px units; spans multiple of 4
REGIONS = [
    ("grasscobble", 20, 48, 32, 92, 10),   # green grass over cobbled stone ground
    ("booth",        0,  0, 12, 12, 11),   # full market booth (dark frame + striped banner)
]

def near_black(c): return max(c) <= 16
def is_xmarker(t):
    n=sum(1 for c in t if min(c)>=225 or (c[0]>=170 and c[1]<=70 and c[2]<=70))
    return n>=40

def load_region(c0,r0,c1,r1):
    im=Image.open(SHIK).convert("RGB")
    crop=im.crop((c0*8,r0*8,c1*8,r1*8))
    crop=crop.resize((crop.size[0]//SS, crop.size[1]//SS), Image.NEAREST)
    W,H=crop.size; p=crop.load()
    tr=[[False]*W for _ in range(H)]; dq=deque()
    for x in range(W):
        for y in (0,H-1):
            if near_black(p[x,y]) and not tr[y][x]: tr[y][x]=True; dq.append((x,y))
    for y in range(H):
        for x in (0,W-1):
            if near_black(p[x,y]) and not tr[y][x]: tr[y][x]=True; dq.append((x,y))
    while dq:
        x,y=dq.popleft()
        for dx,dy in ((1,0),(-1,0),(0,1),(0,-1)):
            nx,ny=x+dx,y+dy
            if 0<=nx<W and 0<=ny<H and not tr[ny][nx] and near_black(p[nx,ny]):
                tr[ny][nx]=True; dq.append((nx,ny))
    TW,TH=W//8,H//8
    def tile(tx,ty):
        t=[TRANS if tr[ty*8+y][tx*8+x] else p[tx*8+x,ty*8+y] for y in range(8) for x in range(8)]
        return [TRANS]*64 if is_xmarker(t) else t
    out=[]; mw,mh=TW//2,TH//2
    for my in range(mh):
        for mx in range(mw):
            for dy in range(2):
                for dx in range(2):
                    out.append(tile(mx*2+dx,my*2+dy))
    return out, mw*mh

def quantize(pixels,k=15):
    img=Image.new("RGB",(len(pixels),1)); img.putdata(pixels)
    q=img.quantize(colors=min(k,len(set(pixels))),method=Image.MEDIANCUT)
    pal=q.getpalette()[:768]; used=sorted(set(q.getdata()))
    return [tuple(pal[i*3:i*3+3]) for i in used][:k]
def nearest(c,pal): return min(range(len(pal)),key=lambda i:sum((int(pal[i][j])-int(c[j]))**2 for j in range(3)))

# ---- restore-from-baseline so re-runs don't stack appends ----
base=os.path.join(DST,".baseline")
for f in ("tiles.png","metatiles.bin","metatile_attributes.bin"):
    b=base+"_"+f
    if not os.path.exists(b):
        os.makedirs(base and os.path.dirname(b) or ".", exist_ok=True)
        shutil.copy(os.path.join(DST,f), b)          # first run: snapshot original
    else:
        shutil.copy(b, os.path.join(DST,f))          # later runs: restore original first

# ---- existing tiles.png (preserve pixel indices exactly) ----
tp=Image.open(os.path.join(DST,"tiles.png")).convert("P")
base_pal=tp.getpalette()
ex=tp.load(); EW,EH=tp.size
n_existing=(EW//8)*(EH//8)
def get_ex_tile(n):
    bx,by=(n%16)*8,(n//16)*8
    return [ex[bx+x,by+y] for y in range(8) for x in range(8)]

# ---- build new tiles per region/palette ----
new_meta=[]      # list of (palslot, [4 tiles as 64 idx])
pal_by_slot={}
for (name,c0,r0,c1,r1,slot) in REGIONS:
    assert (c1-c0)%4==0 and (r1-r0)%4==0, name
    tiles,nm=load_region(c0,r0,c1,r1)
    pix=[c for t in tiles for c in t if c!=TRANS]
    pal=quantize(pix); pal_by_slot[slot]=pal
    lut={}
    def rt(t):
        o=[]
        for c in t:
            if c==TRANS: o.append(0); continue
            if c not in lut: lut[c]=nearest(c,pal)
            o.append(lut[c]+1)     # +1: index 0 reserved transparent
        return o
    for m in range(nm):
        new_meta.append((slot,[rt(t) for t in tiles[m*4:m*4+4]]))

# ---- dedup new tiles, assign local indices after existing ----
uniq=[]; key2idx={}
def add_tile(idxs):
    k=tuple(idxs)
    if k not in key2idx: key2idx[k]=n_existing+len(uniq); uniq.append(idxs)
    return key2idx[k]
meta_entries=[]
for slot,quad in new_meta:
    locs=[add_tile(t) for t in quad]
    meta_entries.append((slot,locs))

total_tiles=n_existing+len(uniq)
assert total_tiles<=512, f"secondary tile overflow: {total_tiles}"
print(f"existing tiles={n_existing}, new unique={len(uniq)}, total={total_tiles}/512")
print(f"new metatiles={len(meta_entries)} (appended after 144)")

# ---- write enlarged tiles.png (existing pixels preserved + new appended) ----
new_total=n_existing+len(uniq); rows=(new_total+15)//16
out=Image.new("P",(128,rows*8),0); out.putpalette(base_pal or [0]*768); op=out.load()
for n in range(n_existing):
    t=get_ex_tile(n); bx,by=(n%16)*8,(n//16)*8
    for k in range(64): op[bx+k%8,by+k//8]=t[k]
for j,t in enumerate(uniq):
    n=n_existing+j; bx,by=(n%16)*8,(n//16)*8
    for k in range(64): op[bx+k%8,by+k//8]=t[k]
out.save(os.path.join(DST,"tiles.png"))

# ---- write new palettes (slots 10,11); leave 6-9 + others untouched ----
for slot,pal in pal_by_slot.items():
    full=[TRANS]+list(pal)+[(0,0,0)]*(15-len(pal))
    with open(os.path.join(DST,"palettes",f"{slot:02d}.pal"),"w",newline="") as f:
        f.write("JASC-PAL\r\n0100\r\n16\r\n")
        for c in full: f.write(f"{c[0]} {c[1]} {c[2]}\r\n")

# ---- append metatiles + attributes (bottom=art, top=blank tile 0) ----
mt=bytearray(open(os.path.join(DST,"metatiles.bin"),"rb").read())
attr=bytearray(open(os.path.join(DST,"metatile_attributes.bin"),"rb").read())
B=struct.pack('<H',0)
for slot,locs in meta_entries:
    for L in locs:
        mt+=struct.pack('<H',(NUM_TILES_IN_PRIMARY+L)|(slot<<12))
    mt+=B*4
    attr+=struct.pack('<H',0)
open(os.path.join(DST,"metatiles.bin"),"wb").write(mt)
open(os.path.join(DST,"metatile_attributes.bin"),"wb").write(attr)
print(f"metatiles.bin now {len(mt)//16} metatiles; new ground/booth start at secondary metatile #{len(mt)//16 - len(meta_entries)}")
