#!/usr/bin/env python3
# !!! WARNING: this REGENERATES the whole tileset from source art -- it OVERWRITES
# !!! metatiles.bin / metatile_attributes.bin / tiles.png and will WIPE any edits
# !!! you made in Porymap (custom metatile layers, collision, behaviours). The
# !!! Ashlands tileset has since been edited in Porymap, so do NOT re-run this.
# !!! For the tall-grass tweak use the in-place tools/patch_ash_tallgrass.py.
#
# Builds the gTileset_AshlandTrees SECONDARY tileset from the cropped source art,
# with the grass/orange BACKGROUNDS made transparent so the trees are clean cutouts
# you can drop onto any terrain (no baked grass square, no un-editable orange).
#
#   source: ../tilesets_raw/ashland_trees.png   (top 6px are a stray strip -> cropped)
#   output: data/tilesets/secondary/ashland_trees/{tiles.png, palettes/NN.pal,
#                                                   metatiles.bin, metatile_attributes.bin}
#
# Run from the repo root:   python3 tools/build_ashland_trees.py   then  `make`.
#
# How it works: the background (saturated grass-green + bright orange ground) is
# flood-filled inward from the image borders -- the trees' dark outlines stop the
# fill, so the green LEAFY trees survive. Those pixels become palette index 0
# (transparent). Metatiles put the tree art on the TOP layer (transparent shows
# through) over a grass base on the BOTTOM layer.
#
# In Porymap afterwards: the metatiles default to layer type "Normal" (art over the
# player). Set ground/path metatiles to "Covered", set the grass base layer to your
# own primary grass if you prefer, and add collision on trunks.
import sys, os, struct, colorsys
from collections import deque, Counter
from PIL import Image

SRC="../tilesets_raw/ashland_trees.png"
DST="data/tilesets/secondary/ashland_trees"
CROP_TOP=6
SLOT0=6                 # secondary palettes occupy game slots 6..15
NUM_TILES_IN_PRIMARY=512
TRANS=(255,0,255)       # sentinel -> palette index 0 (transparent)

os.makedirs(f"{DST}/palettes", exist_ok=True)
im=Image.open(SRC).convert("RGB")
src=im.crop((0,CROP_TOP,im.size[0],im.size[1])); W,H=src.size; p=src.load()

def is_bg(c):
    r,g,b=c; mx,mn=max(c),min(c); v=mx; s=0 if mx==0 else (mx-mn)/mx
    h=colorsys.rgb_to_hsv(r/255,g/255,b/255)[0]*360
    return (s>0.33 and 60<=h<=180) or (s>0.33 and (h<=48 or h>=330) and v>150)

# flood-fill transparency mask from the borders, through background colours only
tr=[[False]*W for _ in range(H)]; dq=deque()
for x in range(W):
    for y in (0,H-1):
        if is_bg(p[x,y]): tr[y][x]=True; dq.append((x,y))
for y in range(H):
    for x in (0,W-1):
        if is_bg(p[x,y]): tr[y][x]=True; dq.append((x,y))
while dq:
    x,y=dq.popleft()
    for dx,dy in ((1,0),(-1,0),(0,1),(0,-1)):
        nx,ny=x+dx,y+dy
        if 0<=nx<W and 0<=ny<H and not tr[ny][nx] and is_bg(p[nx,ny]):
            tr[ny][nx]=True; dq.append((nx,ny))

TW,TH=W//8,H//8
def art_tile(tx,ty):
    return tuple(TRANS if tr[ty*8+y][tx*8+x] else p[tx*8+x,ty*8+y] for y in range(8) for x in range(8))

# grass base = most common fully-background 8x8 tile from the original
cand=Counter()
for ty in range(TH):
    for tx in range(TW):
        if all(is_bg(p[tx*8+x,ty*8+y]) for y in range(8) for x in range(8)):
            cand[tuple(p[tx*8+x,ty*8+y] for y in range(8) for x in range(8))]+=1
grass=cand.most_common(1)[0][0] if cand else tuple([(46,136,40)]*64)

# dedup art tiles (tile 0 = fully transparent blank)
grid=[[0]*TW for _ in range(TH)]; uniq=[tuple([TRANS]*64)]; idx={uniq[0]:0}
for ty in range(TH):
    for tx in range(TW):
        t=art_tile(tx,ty)
        if t not in idx: idx[t]=len(uniq); uniq.append(t)
        grid[ty][tx]=idx[t]

# pack art palettes: index0 reserved transparent, foreground colours 1..15
artpal=[]; tilepal=[0]*len(uniq)
for i,t in enumerate(uniq):
    fg=set(c for c in t if c!=TRANS); placed=False
    for pi,pp in enumerate(artpal):
        if fg<=pp: tilepal[i]=pi; placed=True; break
        if len(pp|fg)<=15: artpal[pi]=pp|fg; tilepal[i]=pi; placed=True; break
    if not placed: artpal.append(set(fg)); tilepal[i]=len(artpal)-1
NART=len(artpal)
artpal_list=[[(255,0,255)]+sorted(pp)+[(0,0,0)]*(15-len(pp)) for pp in artpal]

# grass palette (own slot; index0 is opaque on the bottom layer)
gset=list(dict.fromkeys(grass)); gpal=gset+[(0,0,0)]*(16-len(gset)); gci={c:n for n,c in enumerate(gpal)}
GRASS_SLOT=SLOT0+NART
assert GRASS_SLOT<=15, f"too many palettes: {NART} art + 1 grass"

def reidx(i):
    pl={c:n for n,c in enumerate(artpal_list[tilepal[i]])}
    return [0 if c==TRANS else pl[c] for c in uniq[i]]
tpix=[reidx(i) for i in range(len(uniq))]
grass_px=[gci[c] for c in grass]

GRASS_TILE=len(uniq)

# ---------------------------------------------------------------------------
# ASH TALL GRASS (B_ash tint): a real tall-grass metatile so the player shows
# THROUGH the blades. Vanilla General grass shapes (metatile 37: ground 16/17/
# 32/33 + blades 6/7/22) are recoloured to an ashen grey-green and appended as
# the LAST secondary metatile, flagged MB_TALL_GRASS so it rustles + triggers
# wild encounters. Blades sit on the TOP layer (lower-weighted) over an opaque
# ground base, so a standing sprite is covered only from the shins down.
# Paint with this metatile (the last one in the secondary set) in Porymap.
# ---------------------------------------------------------------------------
GEN="data/tilesets/primary/general"; MB_TALL_GRASS=0x0002
def _loadpal(s):
    L=open(f"{GEN}/palettes/{s:02d}.pal").read().split("\n")[3:]
    return [tuple(map(int,l.split())) for l in L if l.strip()][:16]
_gp2=_loadpal(2)
def _ashify(c):
    r,g,b=c; l=0.3*r+0.59*g+0.11*b; k=0.78; w=10   # k=desaturation, w=warm bias
    return (max(0,min(int(l*k+r*(1-k)+w),255)),
            max(0,min(int(l*k+g*(1-k)),255)),
            max(0,min(int(l*k+b*(1-k)-w*0.4),255)))
ASHPAL=([_ashify(c) for c in _gp2]+[(0,0,0)]*16)[:16]
ASH_SLOT=GRASS_SLOT+1
assert ASH_SLOT<=15, "no free palette slot for ash grass"
_gim=Image.open(f"{GEN}/tiles.png").convert("P"); _gpx=_gim.load()
def _gtile(t):
    bx,by=(t%16)*8,(t//16)*8
    return [_gpx[bx+x,by+y] for y in range(8) for x in range(8)]
ASH_SRC=[16,17,32,33,6,7,22]            # ground x4, then blades x3
ash_px=[_gtile(t) for t in ASH_SRC]
ASH_BASE=GRASS_TILE+1                    # first local tile index of the ash set
for gi in range(4):                      # ground must avoid idx0 (else holes)
    assert 0 not in ash_px[gi], f"ground tile {ASH_SRC[gi]} uses index0!"

# tiles.png (art tiles + 1 grass tile + 7 ash-grass tiles), 16 wide
extra=1+len(ASH_SRC); ntiles=len(uniq)+extra; cols=16; rows=(ntiles+15)//16
timg=Image.new("P",(cols*8,rows*8),0)
flat=[v for c in artpal_list[0] for v in c]; timg.putpalette(flat+[0]*(768-len(flat))); tl=timg.load()
for n in range(len(uniq)):
    bx,by=(n%16)*8,(n//16)*8
    for k in range(64): tl[bx+k%8,by+k//8]=tpix[n][k]
gx,gy=(GRASS_TILE%16)*8,(GRASS_TILE//16)*8
for k in range(64): tl[gx+k%8,gy+k//8]=grass_px[k]
for j,px in enumerate(ash_px):
    n=ASH_BASE+j; bx,by=(n%16)*8,(n//16)*8
    for k in range(64): tl[bx+k%8,by+k//8]=px[k]
timg.save(f"{DST}/tiles.png")

# 16 palette files
for s in range(16):
    if SLOT0<=s<SLOT0+NART: pal=artpal_list[s-SLOT0]
    elif s==GRASS_SLOT: pal=gpal
    elif s==ASH_SLOT: pal=ASHPAL
    else: pal=[(0,0,0)]*16
    with open(f"{DST}/palettes/{s:02d}.pal","w",newline="") as f:
        f.write("JASC-PAL\r\n0100\r\n16\r\n"); [f.write(f"{c[0]} {c[1]} {c[2]}\r\n") for c in pal]

# metatiles: tree art over grass base, then ONE ash tall-grass metatile at end
MW,MH=TW//2,TH//2; mt=bytearray()
gz=struct.pack('<H',(NUM_TILES_IN_PRIMARY+GRASS_TILE)|(GRASS_SLOT<<12))
for my in range(MH):
    for mx in range(MW):
        mt+=gz*4
        for dy in range(2):
            for dx in range(2):
                ti=grid[my*2+dy][mx*2+dx]
                mt+=struct.pack('<H',(NUM_TILES_IN_PRIMARY+ti)|((SLOT0+tilepal[ti])<<12))
HFLIP=1<<10
def _ash(local,flip=0): return struct.pack('<H',(NUM_TILES_IN_PRIMARY+local)|(ASH_SLOT<<12)|flip)
# bottom = ground (locals +0..+3); top = blades 6,7,22,22h (locals +4,+5,+6,+6)
mt+= _ash(ASH_BASE+0)+_ash(ASH_BASE+1)+_ash(ASH_BASE+2)+_ash(ASH_BASE+3)
mt+= _ash(ASH_BASE+4)+_ash(ASH_BASE+5)+_ash(ASH_BASE+6)+_ash(ASH_BASE+6,HFLIP)
open(f"{DST}/metatiles.bin","wb").write(mt)
GRASS_META=MW*MH
attrs=bytearray(b'\x00\x00'*(MW*MH))+struct.pack('<H',MB_TALL_GRASS)
open(f"{DST}/metatile_attributes.bin","wb").write(attrs)

print(f"ashland_trees: {len(uniq)} art +1 grass +{len(ASH_SRC)} ash-grass tiles, "
      f"{NART} art pals + grass slot {GRASS_SLOT} + ash slot {ASH_SLOT}, "
      f"{MW*MH}+1 metatiles (ash TALL GRASS = secondary metatile #{GRASS_META}) -> {DST}")
