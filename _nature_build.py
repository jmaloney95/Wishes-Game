from PIL import Image
import numpy as np, struct
from collections import Counter
SRC="../tilesets_raw/mt__moon_village_by_ekat99_dejruu3.png"
SEC="data/tilesets/secondary/mt_moon_village_interior"
src=Image.open(SRC).convert('RGBA'); arr=np.array(src)
grass=arr[65*8:66*8, 5*8:6*8].copy()           # grass bg tile for baking trees

def baked(r0,r1,c0,c1):                          # tree region -> opaque (grass fill)
    reg=arr[r0*8:r1*8,c0*8:c1*8].copy(); h,w=reg.shape[:2]
    out=reg.copy()
    for y in range(h):
        for x in range(w):
            if reg[y,x,3]<=16: out[y,x]=grass[y%8,x%8]
    return out
def plain(r0,r1,c0,c1): return arr[r0*8:r1*8,c0*8:c1*8].copy()

def make_pal(block,n=15,drop=set()):
    cc=Counter()
    for y in range(block.shape[0]):
        for x in range(block.shape[1]):
            R,G,B,A=(int(v) for v in block[y,x])
            if A>16 and (R,G,B) not in drop: cc[(R,G,B)]+=1
    pal=[(255,0,255)]+[c for c,_ in cc.most_common(n)]
    while len(pal)<16: pal.append((0,0,0))
    return pal[:16]

def quant_tile(block, ty, tx, pal):              # 8x8 indices, int32 distance
    parr=np.array(pal[1:],dtype=np.int32)
    out=np.zeros((8,8),np.uint8)
    for y in range(8):
        for x in range(8):
            R,G,B,A=(int(v) for v in block[ty*8+y, tx*8+x])
            if A<=16: continue
            d=(parr[:,0]-R)**2+(parr[:,1]-G)**2+(parr[:,2]-B)**2
            out[y,x]=int(np.argmin(d))+1
    return out

# build object blocks + palettes
bigT=baked(4,10,0,4); smallT=baked(4,10,10,14)
# combined palette over both tree blocks
def make_pal2(blocks,n=15,drop=set()):
    cc=Counter()
    for block in blocks:
        for y in range(block.shape[0]):
            for x in range(block.shape[1]):
                R,G,B,A=(int(v) for v in block[y,x])
                if A>16 and (R,G,B) not in drop: cc[(R,G,B)]+=1
    pal=[(255,0,255)]+[c for c,_ in cc.most_common(n)]
    while len(pal)<16: pal.append((0,0,0))
    return pal[:16]
tree_pal=make_pal2([bigT,smallT],15)
pond=plain(68,74,0,6); water_pal=make_pal2([pond],15,drop={(255,0,0)})
crat=plain(12,18,11,15); crater_pal=make_pal2([crat],15)

# object metatile layout: (block, block_origin_row, block_origin_col, palslot, list of (mrow,mcol) within block in tile units)
def metas_for(block, palslot, wtiles, htiles):
    # returns list of (4 bottom tile-index-patterns) for 2x2 metatiles tiling the block
    res=[]
    for mr in range(0,htiles,2):
        for mc in range(0,wtiles,2):
            subs=[quant_tile(block,mr+dy,mc+dx,PAL[palslot]) for (dy,dx) in [(0,0),(0,1),(1,0),(1,1)]]
            res.append((palslot,subs))
    return res
PAL={10:tree_pal,11:water_pal,12:crater_pal}

objects=[]
objects+=metas_for(bigT,10,4,6)     # big tree 2x3 metatiles
objects+=metas_for(smallT,10,4,6)   # small tree
objects+=metas_for(pond,11,6,6)     # water 3x3
objects+=metas_for(crat,12,4,6)     # crater 2x3

# load secondary tiles.png
sec=Image.open(f"{SEC}/tiles.png"); secpal=sec.getpalette()[:48]
sarr=np.array(sec); SW=sec.size[0]; TPR=SW//8
n_exist=(SW//8)*(sec.size[1]//8)
tindex={}
for t in range(n_exist):
    r,c=t//TPR,t%TPR; tindex[tuple(sarr[r*8:r*8+8,c*8:c*8+8].flatten().tolist())]=t
new_tiles=[]
def tile_id(pat):
    key=tuple(pat.flatten().tolist())
    if key in tindex: return tindex[key]
    i=n_exist+len(new_tiles); tindex[key]=i; new_tiles.append(pat); return i

NUM_PRIM=512; metatiles=[]
for (slot,subs) in objects:
    ents=[]
    for pat in subs:
        if pat.max()==0: ents.append(0)
        else: ents.append((NUM_PRIM+tile_id(pat))|(slot<<12))
    metatiles.append(ents+[0,0,0,0])

total=n_exist+len(new_tiles); rows=(total+TPR-1)//TPR
out=np.zeros((rows*8,SW),np.uint8); out[:sarr.shape[0],:sarr.shape[1]]=sarr
for i,pat in enumerate(new_tiles):
    t=n_exist+i; r,c=t//TPR,t%TPR; out[r*8:r*8+8,c*8:c*8+8]=pat
oimg=Image.new('P',(SW,rows*8)); oimg.putdata(out.flatten().tolist()); oimg.putpalette(secpal)
oimg.save(f"{SEC}/tiles.png",bits=4)

mt=open(f"{SEC}/metatiles.bin","rb").read(); n_old=len(mt)//16
open(f"{SEC}/metatiles.bin","wb").write(mt+b"".join(struct.pack('<8H',*[e&0xFFFF for e in m]) for m in metatiles))
att=open(f"{SEC}/metatile_attributes.bin","rb").read()
open(f"{SEC}/metatile_attributes.bin","wb").write(att+b"".join(struct.pack('<H',0) for _ in metatiles))
def wpal(p,pal): open(p,"w",newline="").write("\r\n".join(["JASC-PAL","0100","16"]+[f"{r} {g} {b}" for r,g,b in pal])+"\r\n")
wpal(f"{SEC}/palettes/10.pal",tree_pal); wpal(f"{SEC}/palettes/11.pal",water_pal); wpal(f"{SEC}/palettes/12.pal",crater_pal)

print(f"tiles {n_exist}->{total} (new {len(new_tiles)}), png {SW}x{rows*8}")
print(f"metatiles {n_old}->{n_old+len(metatiles)} (new {len(metatiles)}: bigtree6 smalltree6 water9 crater6)")
print(f"png tile slots = {rows*TPR}")
print(f"palettes written: 10 tree, 11 water, 12 crater")
print(f"NEW_META_START={n_old}")
