import os, struct
from PIL import Image
SRC="/sessions/adoring-zen-cerf/mnt/ROM Hack Project/tilesets_raw/route_224_by_ekat99_der10uy.png"
OUT="/sessions/adoring-zen-cerf/mnt/ROM Hack Project/pokeemerald-expansion/data/tilesets/secondary/route_224"
im=Image.open(SRC).convert("RGBA"); spx=im.load()
def op(p): return p[3]>=128
def snap(c): return ((c[0]>>3)<<3,(c[1]>>3)<<3,(c[2]>>3)<<3)
def isdark(c): return c[0]<40 and c[1]<40 and c[2]<40
TRANS=(-1,-1,-1); MARK=(248,0,0)
def gt(tx,ty,td=False):
    out=[]
    for y in range(8):
        for x in range(8):
            p=spx[tx*8+x,ty*8+y]
            if not op(p): out.append(TRANS); continue
            c=snap(p[:3])
            if c==MARK or (td and isdark(c)): out.append(TRANS); continue
            out.append(c)
    return tuple(out)
def flips(t):
    fx=tuple(t[r*8+(7-c)] for r in range(8) for c in range(8));fy=tuple(t[(7-r)*8+c] for r in range(8) for c in range(8));fxy=tuple(fx[(7-r)*8+c] for r in range(8) for c in range(8))
    return {t:(0,0),fx:(1,0),fy:(0,1),fxy:(1,1)}
def canon(t): return min(flips(t))
def colors(t): return {c for c in t if c!=TRANS}
def R(a,b,cols): return [(r,c) for r in range(a,b+1) for c in cols]
# ---- content ----
GRASS=[(0,0),(0,1),(0,2),(0,3)]
CLIFFTOP=[(3,0),(3,1),(3,2),(4,0),(4,2),(5,0),(5,1),(5,2)]
SAND=R(3,5,[3,4,5,6,7]); SW=R(6,8,[0,1,2,3,4]); WATER=R(6,8,[5,6,7])
CFRAME1=[(13,0),(13,1),(13,2),(14,0),(14,1),(14,2),(15,0),(15,1),(15,2)]
CFRAME2=[(13,3),(13,4),(14,3),(14,4)]
CSIDES=[(22,0),(22,1),(23,0),(23,1),(22,3),(22,4),(23,3),(23,4)]
CFACE=[(16,0),(16,1),(16,2),(17,0),(17,1),(17,2)]
CPEAK=[(15,5),(15,6),(16,5),(16,6),(17,5),(17,6)]
ROCKS=[(13,5),(13,6),(14,5),(14,6),(16,3),(16,4),(17,3),(17,4),(18,3),(18,4)]
SANDCLIFF=[(19,0),(19,1),(19,2),(20,0),(20,2),(21,0),(21,1),(21,2)]
CAVE=[(19,3),(19,4),(20,3),(20,4)]
STAIRS=[(15,7),(16,7),(17,7)]
MEMORIAL=[(25,0),(25,1),(26,0),(26,1),(27,0),(27,1)]
POKEBALL=[(22,2),(23,2)]
BUOY=[(30,7),(31,7)]
FLAT=GRASS+CLIFFTOP+SAND+SW+WATER+CFRAME1+CFRAME2+CSIDES+CFACE+CPEAK+ROCKS+SANDCLIFF+CAVE+STAIRS+MEMORIAL
OVERLAY=POKEBALL+BUOY  # composite on sand, dark->transparent
def subt(my,mx,td=False): return [gt(mx*2+sx,my*2+sy,td) for (sy,sx) in [(0,0),(0,1),(1,0),(1,1)]]
flat_subs=[(rc,subt(*rc)) for rc in FLAT]
ov_subs=[(rc,subt(rc[0],rc[1],True)) for rc in OVERLAY]
SANDTILE=gt(6*2+0,4*2+0)
# palette packing
uniq=[]; useen=set()
def reg(t):
    cs=colors(t)
    if not cs: return
    c=canon(t)
    if c not in useen: useen.add(c); uniq.append(cs)
for _,s in flat_subs:
    for t in s: reg(t)
for _,s in ov_subs:
    for t in s: reg(t)
reg(SANDTILE)
def agglo(sl,limit=15):
    pals=[set(s) for s in sl if s]; ch=True
    while ch:
        ch=False;best=None;bl=None
        for i in range(len(pals)):
            for j in range(i+1,len(pals)):
                u=pals[i]|pals[j]
                if len(u)<=limit and (bl is None or len(u)<bl): bl=len(u);best=(i,j)
        if best: i,j=best;pals[i]|=pals[j];pals.pop(j);ch=True
    return pals
pals=agglo(uniq)
print("agglo:",len(pals),"sizes",sorted(len(p) for p in pals))
# force-reduce to <=7
remap={}
def quantize(cs,limit=15):
    cols=list(cs);rmap={c:c for c in cols}
    def d(a,b):return (a[0]-b[0])**2+(a[1]-b[1])**2+(a[2]-b[2])**2
    while len(cols)>limit:
        bi=bj=None;bd=None
        for i in range(len(cols)):
            for j in range(i+1,len(cols)):
                dd=d(cols[i],cols[j])
                if bd is None or dd<bd: bd=dd;bi,bj=i,j
        a,b=cols[bi],cols[bj];m=snap(((a[0]+b[0])//2,(a[1]+b[1])//2,(a[2]+b[2])//2))
        for k,v in list(rmap.items()):
            if v in (a,b): rmap[k]=m
        cols=[m if c in (a,b) else c for c in cols];cols=list(dict.fromkeys(cols))
    return set(cols),rmap
while len(pals)>7:
    best=None;bl=None
    for i in range(len(pals)):
        for j in range(i+1,len(pals)):
            u=pals[i]|pals[j]
            if bl is None or len(u)<bl: bl=len(u);best=(i,j)
    i,j=best;u=pals[i]|pals[j];ns,rm=quantize(u,15)
    for k,v in rm.items(): remap[k]=v
    for k in list(remap):
        v=remap[k];seen=set()
        while v in remap and remap[v]!=v and v not in seen: seen.add(v);v=remap[v]
        remap[k]=v
    pals[i]=ns;pals.pop(j);print("force->",len(pals))
def rc(c): return remap.get(c,c)
pals=[{rc(c) for c in p} for p in pals]
print("final:",len(pals),"sizes",[len(p) for p in pals]); assert len(pals)<=7
pal_lists=[sorted(p) for p in pals]
def find_pal(cs):
    cs={rc(c) for c in cs}
    for pi,p in enumerate(pals):
        if cs<=p: return pi
    best=-1;bp=0
    for pi,p in enumerate(pals):
        o=len(cs&p)
        if o>best:best=o;bp=pi
    return bp
tiles=[(0,tuple(0 for _ in range(64)))]; tindex={}
def t2i(t,pi):
    cm={c:k+1 for k,c in enumerate(pal_lists[pi])}
    return tuple(0 if c==TRANS else cm[rc(c)] for c in t)
def tile_id(t):
    cs=colors(t)
    if not cs: return 0,0,0,0
    pi=find_pal(cs);c=canon(t);key=(c,pi)
    if key not in tindex: tindex[key]=len(tiles);tiles.append((pi,t2i(c,pi)))
    tid=tindex[key];xf,yf=flips(c)[t];return tid,xf,yf,pi
sand_tid,_,_,sand_pi=tile_id(SANDTILE)
grass_fill=gt(0,0)  # plain grass tile for cliff bottom layer
grass_tid,_,_,grass_pi=tile_id(grass_fill)
def slot(pi): return 6+pi
COVERED=set(CFRAME1+CFRAME2+CSIDES+CFACE+CPEAK+ROCKS)
LAYER_COVERED=0x1000
metas=[]; attrs=[]
for rc_,s in flat_subs:
    if rc_ in COVERED:
        # bottom layer = grass fill x4 ; top layer = cliff art
        ent=[(grass_tid,0,0,slot(grass_pi)) for _ in range(4)]
        for t in s:
            tid,xf,yf,pi=tile_id(t); ent.append((tid,xf,yf,slot(pi)) if tid else (0,0,0,0))
        metas.append((rc_,ent)); attrs.append(LAYER_COVERED)
    else:
        ent=[]
        for t in s:
            tid,xf,yf,pi=tile_id(t); ent.append((tid,xf,yf,slot(pi)) if tid else (0,0,0,0))
        ent+=[(0,0,0,0)]*4; metas.append((rc_,ent)); attrs.append(0)
for rc_,s in ov_subs:
    ent=[(sand_tid,0,0,slot(sand_pi)) for _ in range(4)]
    for t in s:
        tid,xf,yf,pi=tile_id(t); ent.append((tid,xf,yf,slot(pi)) if tid else (0,0,0,0))
    metas.append((rc_,ent)); attrs.append(0)
NT=len(tiles);NM=len(metas)
print("tiles",NT,"metatiles",NM,"covered",sum(1 for a in attrs if a)); assert NT<=512
TW=16;rows=(NT+TW-1)//TW
img=Image.new("P",(TW*8,rows*8),0)
flat=[0,0,0]
for c in pal_lists[0]: flat+=list(c)
flat+=[0,0,0]*(256-1-len(pal_lists[0]))
img.putpalette(flat);ipx=img.load()
for tid,item in enumerate(tiles):
    if tid==0: continue
    pi,idxs=item;ox=(tid%TW)*8;oy=(tid//TW)*8
    for y in range(8):
        for x in range(8): ipx[ox+x,oy+y]=idxs[y*8+x]
img.save(OUT+"/tiles.png",bits=4)
mb=bytearray()
for rc_,ent in metas:
    for (tid,xf,yf,sl) in ent:
        gid=(tid+512) if tid else 0
        mb+=struct.pack("<H",(sl<<12)|(yf<<11)|(xf<<10)|(gid&0x3FF))
open(OUT+"/metatiles.bin","wb").write(mb)
ab=bytearray()
for a in attrs: ab+=struct.pack("<H",a)
open(OUT+"/metatile_attributes.bin","wb").write(ab)
for sl in range(16):
    pi=sl-6;cols=pal_lists[pi] if 0<=pi<len(pal_lists) else []
    full=[(0,0,0)]+cols;full+=[(0,0,0)]*(16-len(full))
    open(OUT+f"/palettes/{sl:02d}.pal","w").write("\n".join(["JASC-PAL","0100","16"]+[f"{c[0]} {c[1]} {c[2]}" for c in full[:16]])+"\n")
mx=max((e[0] for _,ent in metas for e in ent if e[0]),default=0)
print("NUM_TILES",NT,"max_tid",mx,"num_tiles_needed",mx+1)
# layout summary
groups=[("grass",GRASS),("clifftop",CLIFFTOP),("sand",SAND),("sand/water",SW),("water",WATER),
("cliff frame",CFRAME1+CFRAME2),("cliff sides",CSIDES),("cliff face",CFACE),("cliff peak",CPEAK),
("rocks",ROCKS),("sand-cliff/cave frame",SANDCLIFF),("cave",CAVE),("STAIRS",STAIRS),
("MEMORIAL",MEMORIAL),("POKEBALL",POKEBALL),("BUOY",BUOY)]
i=0
for n,g in groups:
    print(f"  metas {i}-{i+len(g)-1}: {n}"); i+=len(g)
