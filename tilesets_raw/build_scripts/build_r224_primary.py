import os, struct, subprocess, filecmp
from PIL import Image
SRC = "/sessions/adoring-zen-cerf/mnt/ROM Hack Project/tilesets_raw/route_224_by_ekat99_der10uy.png"
OUT = "/sessions/adoring-zen-cerf/mnt/ROM Hack Project/pokeemerald-expansion/data/tilesets/primary/route_224"
GBAGFX = "/sessions/adoring-zen-cerf/mnt/ROM Hack Project/pokeemerald-expansion/tools/gbagfx/gbagfx"
os.makedirs(os.path.join(OUT, "palettes"), exist_ok=True)
im = Image.open(SRC).convert("RGBA"); W, H = im.size; px = im.load()
def op(p): return p[3] >= 128
def snap(c): return ((c[0]>>3)<<3,(c[1]>>3)<<3,(c[2]>>3)<<3)
TRANS=(-1,-1,-1); MARKER=(248,0,0)
FLOWERS=[(0,4),(0,5),(1,4),(1,5),(1,6),(1,7),(2,4),(2,5),(2,6),(2,7)]
TREES=[(my,mx) for my in range(9,13) for mx in range(8)]
METAS=TREES+FLOWERS
def get_tile(tx,ty):
    return tuple(snap(px[tx*8+x,ty*8+y][:3]) if op(px[tx*8+x,ty*8+y]) else TRANS for y in range(8) for x in range(8))
def flips(t):
    fx=tuple(t[r*8+(7-c)] for r in range(8) for c in range(8))
    fy=tuple(t[(7-r)*8+c] for r in range(8) for c in range(8))
    fxy=tuple(fx[(7-r)*8+c] for r in range(8) for c in range(8))
    return {t:(0,0),fx:(1,0),fy:(0,1),fxy:(1,1)}
def canon(t): return min(flips(t).keys())
def colors(t): return {c for c in t if c!=TRANS}
uniq=[]; useen=set(); meta_tiles=[]
for (my,mx) in METAS:
    subs=[]
    for (sy,sx) in [(0,0),(0,1),(1,0),(1,1)]:
        t=get_tile(mx*2+sx,my*2+sy); cs=colors(t)
        if MARKER in cs: t=tuple(TRANS for _ in range(64)); cs=set()
        subs.append(t)
        if cs:
            c=canon(t)
            if c not in useen: useen.add(c); uniq.append(c)
    meta_tiles.append(subs)
def agglo(setlist,limit=15):
    pals=[set(s) for s in setlist if s]; changed=True
    while changed:
        changed=False; best=None; bl=None
        for i in range(len(pals)):
            for j in range(i+1,len(pals)):
                u=pals[i]|pals[j]
                if len(u)<=limit and (bl is None or len(u)<bl): bl=len(u); best=(i,j)
        if best: i,j=best; pals[i]|=pals[j]; pals.pop(j); changed=True
    return pals
pals=agglo([colors(t) for t in uniq])
print("after agglo:",len(pals),"sizes",sorted(len(p) for p in pals))
remap={}
def quantize(colset,limit=15):
    cols=list(colset); rmap={c:c for c in cols}
    def d(a,b): return (a[0]-b[0])**2+(a[1]-b[1])**2+(a[2]-b[2])**2
    while len(cols)>limit:
        bi=bj=None; bd=None
        for i in range(len(cols)):
            for j in range(i+1,len(cols)):
                dd=d(cols[i],cols[j])
                if bd is None or dd<bd: bd=dd; bi,bj=i,j
        a,b=cols[bi],cols[bj]; merged=snap(((a[0]+b[0])//2,(a[1]+b[1])//2,(a[2]+b[2])//2))
        for k,v in list(rmap.items()):
            if v==a or v==b: rmap[k]=merged
        cols=[merged if c in (a,b) else c for c in cols]; cols=list(dict.fromkeys(cols))
    return set(cols),rmap
while len(pals)>6:
    best=None; bl=None
    for i in range(len(pals)):
        for j in range(i+1,len(pals)):
            u=pals[i]|pals[j]
            if bl is None or len(u)<bl: bl=len(u); best=(i,j)
    i,j=best; union=pals[i]|pals[j]; newset,rmap=quantize(union,15)
    for k,v in rmap.items(): remap[k]=v
    for k in list(remap.keys()):
        v=remap[k]; seen=set()
        while v in remap and remap[v]!=v and v not in seen: seen.add(v); v=remap[v]
        remap[k]=v
    pals[i]=newset; pals.pop(j)
    print("force-merged ->",len(pals),"union",len(union),"->",len(newset))
def rc(c): return remap.get(c,c)
pals=[{rc(c) for c in p} for p in pals]
print("final palettes:",len(pals),"sizes",[len(p) for p in pals])
pal_lists=[sorted(p) for p in pals]
def find_pal(tc):
    tc={rc(c) for c in tc}
    for pi,p in enumerate(pals):
        if tc<=p: return pi
    best=-1; bp=0
    for pi,p in enumerate(pals):
        ov=len(tc&p)
        if ov>best: best=ov; bp=pi
    return bp
tile_index={}; tile_bitmaps=[(0,tuple(0 for _ in range(64)))]
def t2i(t,pi):
    lst=pal_lists[pi]; cmap={c:k+1 for k,c in enumerate(lst)}
    return tuple(0 if c==TRANS else cmap.get(rc(c),0) for c in t)
def get_tile_id(raw):
    cs=colors(raw)
    if not cs: return 0,0,0
    pi=find_pal(cs); c=canon(raw); key=(c,pi)
    if key not in tile_index:
        tile_index[key]=len(tile_bitmaps); tile_bitmaps.append((pi,t2i(c,pi)))
    tid=tile_index[key]; xf,yf=flips(c)[raw]
    return tid,xf,yf
meta_bytes=bytearray()
for subs in meta_tiles:
    ent=[]
    for raw in subs:
        tid,xf,yf=get_tile_id(raw); ent.append((tid,xf,yf,tile_bitmaps[tid][0]))
    for _ in range(4): ent.append((0,0,0,0))
    for (tid,xf,yf,pi) in ent:
        meta_bytes+=struct.pack("<H",(pi<<12)|(yf<<11)|(xf<<10)|tid)
NT=len(tile_bitmaps)
print("unique tiles:",NT,"metatiles:",len(meta_tiles))
assert NT<=512
TW=16; rows=(NT+TW-1)//TW
out=Image.new("P",(TW*8,rows*8),0)
pal0=pal_lists[0] if pal_lists else []
flat=[0,0,0]
for c in pal0: flat+=list(c)
flat+=[0,0,0]*(256-1-len(pal0))
out.putpalette(flat); pix=out.load()
for tid,(pi,idxs) in enumerate(tile_bitmaps):
    ox=(tid%TW)*8; oy=(tid//TW)*8
    for y in range(8):
        for x in range(8): pix[ox+x,oy+y]=idxs[y*8+x]
out.save(os.path.join(OUT,"tiles.png"),bits=4)
print("tiles.png",TW*8,"x",rows*8)
open(os.path.join(OUT,"metatiles.bin"),"wb").write(meta_bytes)
open(os.path.join(OUT,"metatile_attributes.bin"),"wb").write(b"\x00\x00"*len(meta_tiles))
for pi in range(6):
    lst=pal_lists[pi] if pi<len(pal_lists) else []
    full=[(0,0,0)]+lst; full+=[(0,0,0)]*(16-len(full))
    lines=["JASC-PAL","0100","16"]+[f"{c[0]} {c[1]} {c[2]}" for c in full[:16]]
    open(os.path.join(OUT,"palettes",f"{pi:02d}.pal"),"w").write("\n".join(lines)+"\n")
def run(*a):
    r=subprocess.run(a,capture_output=True,text=True)
    if r.returncode: print("ERR",a[-3:],r.stderr[:200])
    return r
run(GBAGFX,os.path.join(OUT,"tiles.png"),os.path.join(OUT,"tiles.4bpp"),"-num_tiles",str(NT),"-Wnum_tiles")
run(GBAGFX,os.path.join(OUT,"tiles.4bpp"),os.path.join(OUT,"tiles.4bpp.lz"))
for pi in range(6): run(GBAGFX,os.path.join(OUT,"palettes",f"{pi:02d}.pal"),os.path.join(OUT,"palettes",f"{pi:02d}.gbapal"))
run(GBAGFX,os.path.join(OUT,"tiles.4bpp.lz"),"/tmp/rt.4bpp")
print("LZ roundtrip OK:",filecmp.cmp(os.path.join(OUT,"tiles.4bpp"),"/tmp/rt.4bpp"))
print("tiles.4bpp bytes:",os.path.getsize(os.path.join(OUT,"tiles.4bpp")))
