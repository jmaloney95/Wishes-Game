#!/usr/bin/env python3
# Build a battle environment background (tiles.png/map.bin/palette.pal) from a
# 256x112 backdrop image. Layout written: block A rows 0-13 = backdrop, block B
# rows 30-45 + block C rows 62-63 = darkened seamless pattern from the art.
# Palette bank 2 (battle env slot), 48-colour palette.pal, <=512 tiles.
# Usage: python3 tools/build_battle_env.py <art.png> <env_folder> [crop_top]
from PIL import Image
import numpy as np, struct, sys, os
from collections import Counter

art_path, out = sys.argv[1], sys.argv[2]
crop_top = int(sys.argv[3]) if len(sys.argv) > 3 else 0
MAXT = 511

img = Image.open(art_path).convert('RGB')
art = np.array(img)[crop_top:crop_top+112]
if art.shape[1] != 256:
    art = np.array(Image.fromarray(art).resize((256,112), Image.LANCZOS))
assert art.shape[0] == 112, art.shape

canvas = np.zeros((512,256,3), np.int16) - 1
canvas[0:112] = art
patch = (art[40:72, 112:144].astype(np.int16) * 0.5).astype(np.int16)
for y in range(30*8, 46*8):
    for x in range(256): canvas[y,x] = patch[(y-240)%32, x%32]
for y in range(62*8, 64*8):
    for x in range(256): canvas[y,x] = patch[y%32, x%32]

solidpx = canvas[canvas[:,:,0] >= 0].reshape(-1,3).astype(np.uint8)
uniq_cols = sorted(set(map(tuple, solidpx)))
if len(uniq_cols) <= 15:
    pal = uniq_cols + [(0,0,0)]*(15-len(uniq_cols))
else:
    q = Image.fromarray(solidpx.reshape(-1,1,3),'RGB').quantize(colors=15, method=Image.MEDIANCUT, dither=Image.Dither.NONE)
    pal = [tuple(q.getpalette()[i*3:i*3+3]) for i in range(15)]
arr = np.array(pal, np.int32)
idx = np.zeros((512,256), np.uint8)
solid = canvas[:,:,0] >= 0
d = ((canvas[solid][:,None,:].astype(np.int32) - arr[None,:,:])**2).sum(-1)
idx[solid] = 1 + np.argmin(d, -1).astype(np.uint8)

T = idx.reshape(64,8,32,8).transpose(0,2,1,3).reshape(64*32,8,8)
keys, order, assign = {}, [], []
blank = np.zeros((8,8),np.uint8); keys[blank.tobytes()] = (0,0,0); order.append(blank)
def flips(t): return [(t,0,0),(t[:,::-1],1,0),(t[::-1,:],0,1),(t[::-1,::-1],1,1)]
for t in T:
    hit = None
    for ft,hf,vf in flips(t):
        if ft.tobytes() in keys:
            ti,_,_ = keys[ft.tobytes()]; hit=(ti,hf,vf); break
    if hit is None:
        ti = len(order); order.append(t.copy()); keys[t.tobytes()]=(ti,0,0); hit=(ti,0,0)
    assign.append(hit)
if len(order) > MAXT+1:        # cluster rarest-into-nearest until it fits
    palarr = np.array([(255,0,255)]+pal, np.float32)
    vecs = np.stack([palarr[t.reshape(-1)].reshape(-1) for t in order[1:]])
    counts = Counter(a[0] for a in assign)
    ids = list(range(1,len(order)))
    alive = {i:True for i in ids}
    remap = {i:i for i in ids}
    while sum(alive.values()) > MAXT:
        live = [i for i in ids if alive[i]]
        rare = min(live, key=lambda i: counts.get(i,0))
        dd = ((vecs[np.array(live)-1] - vecs[rare-1])**2).sum(-1)
        dd[[k for k,i in enumerate(live) if i==rare]] = 1e18
        tgt = live[int(np.argmin(dd))]
        alive[rare] = False
        counts[tgt] = counts.get(tgt,0)+counts.get(rare,0)
        for k,v in remap.items():
            if v == rare: remap[k] = tgt
    newid = {0:0}; n = 1
    for i in ids:
        if alive[i]: newid[i] = n; n += 1
    order = [order[0]] + [order[i] for i in ids if alive[i]]
    assign = [(newid[remap.get(ti,ti)] if ti else 0, hf, vf) for ti,hf,vf in assign]
    print("  clustered to", len(order), "tiles")

NT = len(order); rows = (NT+15)//16
sheet = np.zeros((max(8,rows*8),128), np.uint8)
for i,t in enumerate(order): sheet[(i//16)*8:(i//16)*8+8,(i%16)*8:(i%16)*8+8] = t
o = Image.fromarray(sheet, mode="P")
o.putpalette([255,0,255]+sum([list(c) for c in pal],[])+[0]*(256-16)*3)
os.makedirs(out, exist_ok=True)
o.save(f"{out}/tiles.png")
with open(f"{out}/map.bin","wb") as f:
    for ti,hf,vf in assign:
        f.write(struct.pack("<H", ti | (hf<<10) | (vf<<11) | (2<<12)))
cols48 = [(0,0,0)] + pal + [(0,0,0)]*32
open(f"{out}/palette.pal","w",newline='\n').write(
  "JASC-PAL\n0100\n48\n" + "".join(f"{r} {g} {b}\n" for r,g,b in cols48))
print(f"{out}: {NT} tiles, {len(uniq_cols) if len(uniq_cols)<=15 else 15} colors (exact={len(uniq_cols)<=15})")
