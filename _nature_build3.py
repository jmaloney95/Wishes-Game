from PIL import Image
import numpy as np, struct
from collections import Counter
SRC="../tilesets_raw/mt__moon_village_by_ekat99_dejruu3.png"
GEN="data/tilesets/primary/general"
SEC="data/tilesets/secondary/mt_moon_village_interior"
src=Image.open(SRC).convert('RGBA'); arr=np.array(src)
def loadpal(folder,i):
    L=open(f"{folder}/palettes/{i:02d}.pal").read().splitlines()
    c=[tuple(map(int,l.split()[:3])) for l in L[3:] if l.strip()]; return (c+[(0,0,0)]*16)[:16]

def block(r0,r1,c0,c1): return arr[r0*8:r1*8,c0*8:c1*8].copy()
def palfrom(blocks,n=15,drop=set()):
    cc=Counter()
    for b in blocks:
        for y in range(b.shape[0]):
            for x in range(b.shape[1]):
                R,G,B,A=(int(v) for v in b[y,x])
                if A>16 and (R,G,B) not in drop: cc[(R,G,B)]+=1
    pal=[(255,0,255)]+[c for c,_ in cc.most_common(n)]
    while len(pal)<16: pal.append((0,0,0))
    return pal[:16]

# ---- palettes ----
bigT=block(4,10,0,4); smallT=block(4,10,10,14)
grassbg=block(65,66,5,6); tallg=block(0,2,8,16)
tree_pal=palfrom([bigT,smallT,grassbg,tallg],15)                       # slot 10
# UNIFIED water palette: vanilla waterfall (bright blues+foam+tan) + pond/pool essentials
water_pal=[(255,0,255),(74,115,172),(82,139,197),(98,164,222),(115,189,246),(222,230,238),
           (189,189,131),(130,186,126),(80,66,57),(53,45,39),(101,85,85),(125,106,106),
           (38,85,151),(84,151,212),(168,168,136),(148,129,129)]                        # slot 11
cratB=block(12,18,11,15); crater_pal=palfrom([cratB],15)              # slot 12
PAL={10:tree_pal,11:water_pal,12:crater_pal}

def qrgb(rgb,pal):
    if rgb==(255,0,255): return 0
    parr=np.array(pal[1:],np.int32)
    d=(parr[:,0]-rgb[0])**2+(parr[:,1]-rgb[1])**2+(parr[:,2]-rgb[2])**2
    return int(np.argmin(d))+1
def qtile(r,c,pal,bake=None):
    out=np.zeros((8,8),np.uint8)
    for y in range(8):
        for x in range(8):
            R,G,B,A=(int(v) for v in arr[r*8+y,c*8+x])
            if A<=16:
                if bake is None: continue
                R,G,B=bake
            out[y,x]=qrgb((R,G,B),pal)
    return out

# vanilla waterfall tiles from general frame 0 (8x48 -> 6 tiles), quantized to water_pal
genp0=loadpal(GEN,0); f0=np.array(Image.open(f"{GEN}/anim/waterfall/0.png"))
def wf_tile(ti):
    out=np.zeros((8,8),np.uint8)
    for y in range(8):
        for x in range(8):
            v=int(f0[ti*8+y,x])
            if v: out[y,x]=qrgb(genp0[v],water_pal)
    return out

sec=Image.open(f"{SEC}/tiles.png"); secpal=sec.getpalette()[:48]
sarr=np.array(sec); SW=sec.size[0]; TPR=SW//8
n_exist=(SW//8)*(sec.size[1]//8)
tindex={}
for t in range(n_exist):
    r,c=t//TPR,t%TPR; tindex[tuple(sarr[r*8:r*8+8,c*8:c*8+8].flatten().tolist())]=t
new_tiles=[]
def tid(pat):
    k=tuple(pat.flatten().tolist())
    if k in tindex: return tindex[k]
    i=n_exist+len(new_tiles); tindex[k]=i; new_tiles.append(pat); return i
def entry(pat,slot,hf=0):
    return 0 if pat.max()==0 else (512+tid(pat))|(hf<<10)|(slot<<12)

metatiles=[]; idx={}
def mark(n): idx[n]=len(metatiles)
def add_bottom(mr,mc,slot,bake=None):
    subs=[qtile(mr+dy,mc+dx,PAL[slot],bake) for dy,dx in [(0,0),(0,1),(1,0),(1,1)]]
    metatiles.append([entry(s,slot) for s in subs]+[0,0,0,0])
def add_top(mr,mc,slot):
    subs=[qtile(mr+dy,mc+dx,PAL[slot]) for dy,dx in [(0,0),(0,1),(1,0),(1,1)]]
    metatiles.append([0,0,0,0]+[entry(s,slot) for s in subs])

mark("bigtree")
for mr in (4,6):
    for mc in (0,2): add_top(mr,mc,10)
for mc in (0,2): add_bottom(8,mc,10)
mark("smalltree")
for mr in (4,6):
    for mc in (10,12): add_top(mr,mc,10)
for mc in (10,12): add_bottom(8,mc,10)
mark("tallgrass")
for mc in (8,10,12,14): add_bottom(0,mc,10)
mark("pond")
for mr in (68,70,72):
    for mc in (0,2,4): add_bottom(mr,mc,11)
mark("waterfall")   # VANILLA cascade: 3 metatiles, 16px wide (tile + hflip right)
for ti in (0,2,4):
    top=wf_tile(ti); bot=wf_tile(ti+1)
    tT=entry(top,11); tTf=entry(top,11,1); tB=entry(bot,11); tBf=entry(bot,11,1)
    metatiles.append([tT,tTf,tB,tBf,0,0,0,0])
mark("pools")
for mr in (70,72):
    for mc in (9,11): add_bottom(mr,mc,11,bake=water_pal[13])
mark("crater")
for mr in (12,14,16):
    for mc in (11,13): add_bottom(mr,mc,12)
mark("END")

total=n_exist+len(new_tiles); rows=(total+TPR-1)//TPR
out=np.zeros((rows*8,SW),np.uint8); out[:sarr.shape[0],:sarr.shape[1]]=sarr
for i,pat in enumerate(new_tiles):
    t=n_exist+i; r,c=t//TPR,t%TPR; out[r*8:r*8+8,c*8:c*8+8]=pat
oimg=Image.new('P',(SW,rows*8)); oimg.putdata(out.flatten().tolist()); oimg.putpalette(secpal); oimg.save(f"{SEC}/tiles.png",bits=4)
mt=open(f"{SEC}/metatiles.bin","rb").read(); n_old=len(mt)//16
open(f"{SEC}/metatiles.bin","wb").write(mt+b"".join(struct.pack('<8H',*[e&0xFFFF for e in m]) for m in metatiles))
att=open(f"{SEC}/metatile_attributes.bin","rb").read()
open(f"{SEC}/metatile_attributes.bin","wb").write(att+b"".join(struct.pack('<H',0) for _ in metatiles))
def wpal(p,pal): open(p,"w",newline="").write("\r\n".join(["JASC-PAL","0100","16"]+[f"{a} {b} {c}" for a,b,c in pal])+"\r\n")
wpal(f"{SEC}/palettes/10.pal",tree_pal); wpal(f"{SEC}/palettes/11.pal",water_pal); wpal(f"{SEC}/palettes/12.pal",crater_pal)
print("tiles",n_exist,"->",total,"new",len(new_tiles),"png",SW,"x",rows*8,"slots",rows*TPR)
print("metatiles",n_old,"->",n_old+len(metatiles),"new",len(metatiles))
print("offsets",idx)
