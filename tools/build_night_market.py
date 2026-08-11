#!/usr/bin/env python3
# !!! WARNING: REGENERATES gTileset_NightMarket from source art -- OVERWRITES
# !!! tiles.png / metatiles.bin / metatile_attributes.bin and WIPES any Porymap
# !!! edits (custom metatile layers, collision, behaviours). Once you paint the
# !!! night market in Porymap, do NOT re-run this.
#
# Builds the gTileset_NightMarket PRIMARY by fusing odisea's market stalls +
# hanging lanterns with shikari's dark town (walls, cobble, Poke Center, PC Mart).
#   sources: ../tilesets_raw/{shikari_tileset_supplement...,odisea_outdoors...}.png
#            -- both are 2X art (each 8x8 GBA tile drawn 16x16), so SS=2 downscales
#               them to native resolution; without this the tiles render DOUBLE size.
#   output : data/tilesets/primary/night_market/{tiles.png,palettes/NN.pal,
#            metatiles.bin,metatile_attributes.bin}
#   run    : python tools/build_night_market.py --emit    then  make
#
# Fits the GBA primary budget (500/512 tiles, 5/6 pals, slots 0-4; the
# gTileset_AshlandTrees secondary uses 6-9). Per-GROUP median-cut quantization
# (<=15 colors/group) forces <=6 palettes; a red-X "unused cell" filter drops the
# sheets' placeholder tiles; exterior near-black flood-fill -> transparent index 0.
import sys, os, struct
from collections import deque, Counter
from PIL import Image

RAW = r"J:\ROM Hack Project\tilesets_raw"
SHIK = os.path.join(RAW, "shikari_tileset_supplement_by_elinthind_djllm4z.png")
ODI  = os.path.join(RAW, "odisea_outdoors_by_ekat99_dej0k9q.png")
DST  = r"J:\ROM Hack Project\pokeemerald-expansion\data\tilesets\primary\night_market"
PREVIEW = os.path.join(DST, "_preview.png")  # true-color proof render (not built into the ROM)
TRANS = (255, 0, 255)
MAXTILES, MAXPALS = 512, 6
SS = 2   # sources are 2x art (each 8x8 GBA tile drawn 16x16) -> downscale by 2

# (name, src, c0, r0, c1, r1, group) -- coords in the 2x SOURCE's 8px units;
# spans must be multiples of 4 so the downscaled region is whole metatiles.
REGIONS = [
    ("cobble",     SHIK,  0, 112, 20, 128, 0),   # G0 stone: ground + ledge
    ("brickwall",  SHIK,  0, 32, 16, 48, 0),     # G0 stone: walls
    ("stall_shop", ODI,   0, 180, 24, 208, 1),   # G1 stalls: shopfronts + awnings
    ("stall_tent", ODI,   0, 208, 24, 224, 1),   # G1 stalls: A-frame tents
    ("lanterns",   ODI,   0, 344, 12, 360, 2),   # G2 lamps
    ("lamp_string",ODI,   0, 332,  8, 344, 2),   # G2 lamps: hanging strings
    ("pc_center",  SHIK,  0, 140, 32, 172, 3),   # G3 Poke Center building
    ("pc_mart",    SHIK,  0, 172, 28, 208, 4),   # G4 PC Mart storefront
]
NGROUPS = 5

def near_black(c): return max(c) <= 16

def is_xmarker(t):
    # source sheets flag unused cells with a red-X on white; skip those tiles
    n = 0
    for c in t:
        white = min(c) >= 225
        red = c[0] >= 170 and c[1] <= 70 and c[2] <= 70
        if white or red: n += 1
    return n >= 40   # >60% of 64 px

def load_region(src, c0, r0, c1, r1):
    im = Image.open(src).convert("RGB")
    crop = im.crop((c0*8, r0*8, c1*8, r1*8))
    crop = crop.resize((crop.size[0]//SS, crop.size[1]//SS), Image.NEAREST)  # 2x -> 1x
    W, H = crop.size; p = crop.load()
    tr = [[False]*W for _ in range(H)]; dq = deque()
    for x in range(W):
        for y in (0, H-1):
            if near_black(p[x,y]) and not tr[y][x]: tr[y][x]=True; dq.append((x,y))
    for y in range(H):
        for x in (0, W-1):
            if near_black(p[x,y]) and not tr[y][x]: tr[y][x]=True; dq.append((x,y))
    while dq:
        x,y=dq.popleft()
        for dx,dy in ((1,0),(-1,0),(0,1),(0,-1)):
            nx,ny=x+dx,y+dy
            if 0<=nx<W and 0<=ny<H and not tr[ny][nx] and near_black(p[nx,ny]):
                tr[ny][nx]=True; dq.append((nx,ny))
    TW,TH=W//8,H//8
    def tile(tx,ty):
        t = [TRANS if tr[ty*8+y][tx*8+x] else p[tx*8+x,ty*8+y] for y in range(8) for x in range(8)]
        return [TRANS]*64 if is_xmarker(t) else t
    out=[]; mw,mh=TW//2,TH//2
    for my in range(mh):
        for mx in range(mw):
            for dy in range(2):
                for dx in range(2):
                    out.append(tile(mx*2+dx, my*2+dy))
    return out, mw, mh

# load, grouped
group_pixels = [[] for _ in range(NGROUPS)]
metas = []   # (group, [4 tiles(list of 64 colors)], name)
for (name, src, c0, r0, c1, r1, g) in REGIONS:
    assert (c1-c0)%4==0 and (r1-r0)%4==0, name
    tiles, mw, mh = load_region(src, c0, r0, c1, r1)
    for m in range(mw*mh):
        quad = tiles[m*4:m*4+4]
        metas.append((g, quad, name))
        for t in quad:
            group_pixels[g] += [c for c in t if c != TRANS]

# quantize each group to <=15 colors
def quantize_palette(pixels, k=15):
    if not pixels: return [(0,0,0)]
    img = Image.new("RGB", (len(pixels), 1)); img.putdata(pixels)
    q = img.quantize(colors=min(k, len(set(pixels))), method=Image.MEDIANCUT)
    pal = q.getpalette()[:768]
    used = sorted(set(q.getdata()))
    return [tuple(pal[i*3:i*3+3]) for i in used][:k]
group_pal = [quantize_palette(group_pixels[g]) for g in range(NGROUPS)]

def nearest(c, pal):
    return min(range(len(pal)), key=lambda i: sum((int(pal[i][j])-int(c[j]))**2 for j in range(3)))

# remap + dedup
group_lut = [{} for _ in range(NGROUPS)]
def remap_tile(g, t):
    out=[]
    for c in t:
        if c==TRANS: out.append(TRANS); continue
        if c not in group_lut[g]: group_lut[g][c]=nearest(c, group_pal[g])
        out.append(group_pal[g][group_lut[g][c]])
    return tuple(out)

blank=tuple([TRANS]*64); uniq=[blank]; idx={blank:0}; tilegrp=[0]
for gi,(g,quad,name) in enumerate(metas):
    metas[gi]=(g,[remap_tile(g,t) for t in quad],name)
    for t in metas[gi][1]:
        if t not in idx: idx[t]=len(uniq); uniq.append(t); tilegrp.append(g)

print(f"regions={len(REGIONS)} metatiles={len(metas)} unique_tiles={len(uniq)}/{MAXTILES} palettes={NGROUPS}/{MAXPALS}")
for g in range(NGROUPS): print(f"  G{g}: {len(group_pal[g])} colors")
seen={0}; byr=Counter()
for g,quad,name in metas:
    for t in quad:
        if idx[t] not in seen: seen.add(idx[t]); byr[g]+=1

# build palettes: slot g -> TRANS + group_pal[g]
pal_list=[]
for g in range(NGROUPS):
    pp=group_pal[g]; pal_list.append([TRANS]+list(pp)+[(0,0,0)]*(15-len(pp)))
while len(pal_list)<16: pal_list.append([(0,0,0)]*16)

def reidx(i):
    pl={c:n for n,c in enumerate(pal_list[tilegrp[i]])}
    return [0 if c==TRANS else pl[c] for c in uniq[i]]
tpix=[reidx(i) for i in range(len(uniq))]

# preview: render each unique tile with true colors, 16 wide
cols=16; rows=(len(uniq)+15)//16
prev=Image.new("RGB",(cols*8, rows*8),(255,0,255))
pv=prev.load()
for n in range(len(uniq)):
    bx,by=(n%16)*8,(n//16)*8
    pal=pal_list[tilegrp[n]]
    for k in range(64):
        ci=tpix[n][k]; c=pal[ci] if ci!=0 else (40,20,50)
        pv[bx+k%8,by+k//8]=c
prev.resize((cols*8*3, rows*8*3), Image.NEAREST).save(PREVIEW)
print("preview ->", PREVIEW)

if "--emit" not in sys.argv:
    sys.exit(0)
assert len(uniq)<=MAXTILES and NGROUPS<=MAXPALS
os.makedirs(os.path.join(DST,"palettes"), exist_ok=True)
timg=Image.new("P",(cols*8, rows*8),0)
flat=[v for c in pal_list[0] for v in c]; timg.putpalette(flat+[0]*(768-len(flat))); tl=timg.load()
for n in range(len(uniq)):
    bx,by=(n%16)*8,(n//16)*8
    for k in range(64): tl[bx+k%8,by+k//8]=tpix[n][k]
timg.save(os.path.join(DST,"tiles.png"))
for s in range(16):
    pal=pal_list[s]
    with open(os.path.join(DST,"palettes",f"{s:02d}.pal"),"w",newline="") as f:
        f.write("JASC-PAL\r\n0100\r\n16\r\n")
        for c in pal: f.write(f"{c[0]} {c[1]} {c[2]}\r\n")
mt=bytearray(); B=struct.pack('<H',0)
for g,quad,name in metas:
    for t in quad:
        ti=idx[t]; mt+=struct.pack('<H', ti|(tilegrp[ti]<<12))
    mt+=B*4
open(os.path.join(DST,"metatiles.bin"),"wb").write(mt)
open(os.path.join(DST,"metatile_attributes.bin"),"wb").write(b'\x00\x00'*len(metas))
print(f"EMITTED {len(uniq)} tiles / {NGROUPS} pals / {len(metas)} metatiles -> {DST}")
