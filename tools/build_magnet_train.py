#!/usr/bin/env python3
# Build gTileset_MagnetTrain SECONDARY from the gen-3-style magnet train PNG
# (the frozen train that blocks the route north of Frostwood -- Scott thaws it w/ Arcanine).
# Run from repo ROOT in WSL:  python3 tools/build_magnet_train.py   (needs pillow)
# Writes data/tilesets/secondary/magnet_train/{tiles.png, palettes/NN.pal, metatiles.bin,
# metatile_attributes.bin}; `make` then converts tiles.png->4bpp.lz + *.pal->*.gbapal and links.
#
# Source PNG is clean tile-based art (transparent bg, ~13 colors) so it fits ONE secondary
# palette (slot 6). 14x12 metatile sheet; the top-left red-X cells are placeholders -> blanked.
# Each train metatile = baked snow ground on the BOTTOM layer + train art on the TOP layer
# (layer type NORMAL) so the transparent edges show snow instead of black. SECONDARY encoding:
# tile id = 512+local, palette slot 6. Collision/impassability is set per-map in Porymap.
from PIL import Image
import struct, os

SRC = os.environ.get("TRAIN_SRC", "../tilesets_raw/pokemon_gen_3_style_magnet_train_tileset_by_lo8jd_dhu5r16.png")
OUT = os.environ.get("TRAIN_OUT", "data/tilesets/secondary/magnet_train")
PAL_BASE, TILE_BASE = 6, 512
SNOW_TARGET = (224, 232, 240)   # baked ground; reuses the nearest existing palette entry

im = Image.open(SRC).convert("RGBA"); W, H = im.size; px = im.load()
GW, GH = W // 16, H // 16
def isred(c):   return c[0] > 180 and c[1] < 80 and c[2] < 80
def iswhite(c): return c[0] > 235 and c[1] > 235 and c[2] > 235
def is_train_cell(mx, my):
    for y in range(16):
        for x in range(16):
            c = px[mx*16+x, my*16+y]
            if c[3] > 0 and not isred(c) and not iswhite(c): return True
    return False
train_cells = [(mx, my) for my in range(GH) for mx in range(GW) if is_train_cell(mx, my)]
train_set = set(train_cells)

pixels = []
for (mx, my) in train_cells:
    for y in range(16):
        for x in range(16):
            c = px[mx*16+x, my*16+y]
            if c[3] > 0: pixels.append((c[0], c[1], c[2]))
qimg = Image.new("RGB", (len(pixels), 1)); qimg.putdata(pixels)
qp = qimg.quantize(colors=15, method=Image.MEDIANCUT)
plt = qp.getpalette(); ncols = min(15, len(plt)//3)
palette = [tuple(plt[i*3:i*3+3]) for i in range(ncols)]
while len(palette) < 15: palette.append((0, 0, 0))
def nearest(c): return min(range(ncols), key=lambda i:(palette[i][0]-c[0])**2+(palette[i][1]-c[1])**2+(palette[i][2]-c[2])**2)
snow_idx = nearest(SNOW_TARGET)

def get_tile(mx, my, ox, oy):
    return tuple(0 if px[mx*16+ox*8+xx, my*16+oy*8+yy][3] == 0
                 else nearest(px[mx*16+ox*8+xx, my*16+oy*8+yy][:3]) + 1
                 for yy in range(8) for xx in range(8))
def Hf(t): return tuple(t[y*8+(7-x)] for y in range(8) for x in range(8))
def Vf(t): return tuple(t[(7-y)*8+x] for y in range(8) for x in range(8))
uniq = []; kmap = {}
def add(t):
    for cand, xf, yf in ((t,0,0),(Hf(t),1,0),(Vf(t),0,1),(Hf(Vf(t)),1,1)):
        if cand in kmap: return (kmap[cand], xf, yf)
    ti = len(uniq); uniq.append(t); kmap[t] = ti; return (ti, 0, 0)
snow_tile = add(tuple([snow_idx+1]*64))

metatiles = []
for my in range(GH):
    for mx in range(GW):
        if (mx, my) not in train_set:
            metatiles.append(None); continue
        top = [add(get_tile(mx, my, ox, oy)) for oy in range(2) for ox in range(2)]
        metatiles.append(([snow_tile]*4, top))
assert len(uniq) <= 512 and len(metatiles) <= 512
print(f"magnet_train: tiles={len(uniq)} metatiles={len(metatiles)} train_cells={len(train_cells)} "
      f"palette_colors={ncols} grid={GW}x{GH}  (set Porymap 'metatiles per row' to {GW})")

os.makedirs(OUT + "/palettes", exist_ok=True)
cols = 16; rows = (len(uniq)+cols-1)//cols
timg = Image.new("P", (cols*8, max(8, rows*8)), 0)
flat = [v for c in palette for v in c]
timg.putpalette([255,0,255] + flat + [0,0,0]*(256-16))
pm = [0]*(cols*8*max(1,rows)*8)
for ti, t in enumerate(uniq):
    tx, ty = (ti%cols)*8, (ti//cols)*8
    for yy in range(8):
        for xx in range(8): pm[(ty+yy)*cols*8+tx+xx] = t[yy*8+xx]
timg.putdata(pm); timg.save(OUT + "/tiles.png")
for slot in range(13):
    c16 = [(255,0,255)] + (palette if slot == PAL_BASE else [(0,0,0)]*15)
    open(f"{OUT}/palettes/{slot:02d}.pal","w").write("JASC-PAL\n0100\n16\n"+"".join(f"{int(a)} {int(b)} {int(c)}\n" for (a,b,c) in c16))
with open(OUT + "/metatiles.bin", "wb") as f:
    for md in metatiles:
        if md is None: f.write(struct.pack("<8H",0,0,0,0,0,0,0,0)); continue
        bot, top = md
        for (ti, xf, yf) in bot + top:
            f.write(struct.pack("<H", ((TILE_BASE+ti)&0x3FF)|(xf<<10)|(yf<<11)|((PAL_BASE&0xF)<<12)))
with open(OUT + "/metatile_attributes.bin", "wb") as f:
    for _ in metatiles: f.write(struct.pack("<H", 0))
print("wrote", OUT)
