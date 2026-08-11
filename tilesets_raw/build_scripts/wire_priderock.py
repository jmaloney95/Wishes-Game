import os, struct
from PIL import Image, ImageDraw
from collections import Counter
os.chdir('J:/ROM Hack Project/pokeemerald-expansion')
SCR = r'C:\Users\jmalo\AppData\Local\Temp\claude\J--ROM-Hack-Project\923c4402-0ea0-4e04-98a1-9d6ea8243a46\scratchpad'
def dist(u, v): return sum((u[i]-v[i])**2 for i in range(3))

# ---------- halve + white-background flood fill --------------------------------
im = Image.open('../tilesets_raw/pride_rock_by_pinkscales_da_dhbero4.png').convert('RGBA')
half = Image.new('RGBA', (im.width//2, im.height//2))
px, hp = im.load(), half.load()
for y in range(0, im.height-1, 2):
    for x in range(0, im.width-1, 2):
        blk = [px[x, y], px[x+1, y], px[x, y+1], px[x+1, y+1]]
        hp[x//2, y//2] = Counter(blk).most_common(1)[0][0]
W, H = half.size
def iswhite(p): return p[0] > 235 and p[1] > 235 and p[2] > 235
# flood fill background whites from every edge pixel
from collections import deque
mask = [[False]*W for _ in range(H)]
dq = deque()
for x in range(W):
    for y in (0, H-1):
        if iswhite(hp[x, y]) and not mask[y][x]: mask[y][x] = True; dq.append((x, y))
for y in range(H):
    for x in (0, W-1):
        if iswhite(hp[x, y]) and not mask[y][x]: mask[y][x] = True; dq.append((x, y))
while dq:
    x, y = dq.popleft()
    for dx, dy in ((1,0),(-1,0),(0,1),(0,-1)):
        nx, ny = x+dx, y+dy
        if 0 <= nx < W and 0 <= ny < H and not mask[ny][nx] and iswhite(hp[nx, ny]):
            mask[ny][nx] = True; dq.append((nx, ny))
for y in range(H):
    for x in range(W):
        if mask[y][x]: hp[x, y] = (0, 0, 0, 0)

COLS, ROWS = 13, 8
OVERLAY = {(11, 4), (12, 4), (10, 4)}   # green-keyed horn pieces -> top layer
def classify(cx, cy):
    cell = half.crop((cx*16, cy*16, cx*16+16, cy*16+16))
    p = cell.load()
    red = black = green = opaque = 0
    for y in range(16):
        for x in range(16):
            c = p[x, y]
            if c[3] < 128: continue
            opaque += 1
            if c[0] > 200 and c[1] < 70 and c[2] < 70: red += 1
            if c[0] < 40 and c[1] < 40 and c[2] < 40: black += 1
            if c[1] > 150 and c[0] < 120 and c[2] < 120: green += 1
    if opaque < 4: return 'empty', cell
    if red > 8: return 'redx', cell
    if black > opaque * 0.55: return 'text', cell
    return 'art', cell

cells = {}
for cy in range(ROWS):
    for cx in range(COLS):
        kind, cell = classify(cx, cy)
        if kind != 'art': continue
        if (cx, cy) in OVERLAY:
            p = cell.load()
            for y in range(16):
                for x in range(16):
                    c = p[x, y]
                    if c[3] >= 128 and c[1] > 130 and c[0] < 140 and c[2] < 140:
                        p[x, y] = (0, 0, 0, 0)
            if not cell.getbbox(): continue
        cells[(cx, cy)] = cell
print('importable cells:', len(cells))

# ---------- palette -------------------------------------------------------------
allc = []
for cell in cells.values():
    p = cell.load()
    for y in range(16):
        for x in range(16):
            c = p[x, y]
            if c[3] >= 128:
                cc = ((c[0]>>3)<<3, (c[1]>>3)<<3, (c[2]>>3)<<3)
                if cc not in allc: allc.append(cc)
print('distinct colors:', len(allc))
merged = 0
while len(allc) > 15:
    bd = bi = bj = None
    for i in range(len(allc)):
        for j in range(i+1, len(allc)):
            d2 = dist(allc[i], allc[j])
            if bd is None or d2 < bd: bd, bi, bj = d2, i, j
    m = tuple((((allc[bi][k]+allc[bj][k])//2)>>3)<<3 for k in range(3))
    allc = [c for k2, c in enumerate(allc) if k2 not in (bi, bj)] + [m]
    merged += 1
print('palette: %d colors (%d merges)' % (len(allc), merged))
PAL_SLOT = 7

def to_indices(cell):
    p = cell.load()
    t = [[0]*16 for _ in range(16)]
    for y in range(16):
        for x in range(16):
            c = p[x, y]
            if c[3] >= 128:
                cc = ((c[0]>>3)<<3, (c[1]>>3)<<3, (c[2]>>3)<<3)
                t[y][x] = (allc.index(cc)+1) if cc in allc else (min(range(len(allc)), key=lambda i: dist(allc[i], cc))+1)
    return t

# ---------- tile dedup + append -------------------------------------------------
mt_img = Image.open('data/tilesets/secondary/magnet_train/tiles.png')
MTW = mt_img.width // 8
existing_n = (mt_img.height // 8) * MTW
new_tiles = []          # list of 64-int tuples
tile_index = {}
def tile_variants(t8):
    base = tuple(v for row in t8 for v in row)
    hf = tuple(t8[y][7-x] for y in range(8) for x in range(8))
    vf = tuple(t8[7-y][x] for y in range(8) for x in range(8))
    hv = tuple(t8[7-y][7-x] for y in range(8) for x in range(8))
    return base, hf, vf, hv
def get_tile(t8):
    base, hf, vf, hv = tile_variants(t8)
    for var, flips in ((base, 0), (hf, 0x400), (vf, 0x800), (hv, 0xC00)):
        if var in tile_index:
            return tile_index[var], flips
    idx = existing_n + len(new_tiles)
    new_tiles.append(base)
    tile_index[base] = idx
    return idx, 0

mm = bytearray(open('data/tilesets/secondary/magnet_train/metatiles.bin', 'rb').read())
ma = bytearray(open('data/tilesets/secondary/magnet_train/metatile_attributes.bin', 'rb').read())
base_meta = len(mm) // 16
assignments = {}
for (cx, cy) in sorted(cells, key=lambda c: (c[1], c[0])):
    cell = cells[(cx, cy)]
    t16 = to_indices(cell)
    quads = []
    for q in range(4):
        qx, qy = (q % 2) * 8, (q // 2) * 8
        t8 = [[t16[qy+y][qx+x] for x in range(8)] for y in range(8)]
        if all(v == 0 for row in t8 for v in row):
            quads.append(0)
        else:
            idx, flips = get_tile(t8)
            quads.append((512 + idx) | flips | (PAL_SLOT << 12))
    overlay = (cx, cy) in OVERLAY
    if overlay:
        entry = [0, 0, 0, 0] + quads      # art in TOP layer -> renders above player
    else:
        entry = quads + [0, 0, 0, 0]      # art in BOTTOM layer (ground objects)
    mm += struct.pack('<8H', *entry)
    ma += struct.pack('<H', 0x0000)       # MB_NORMAL, layer NORMAL
    assignments[(cx, cy)] = 0x200 + base_meta + len(assignments)
print('new tiles: %d (after flip dedup), new metatiles: %d' % (len(new_tiles), len(assignments)))
total_tiles = existing_n + len(new_tiles)
assert total_tiles <= 512, 'tile budget!'

rows_needed = (total_tiles + MTW - 1) // MTW
grown = Image.new('P', (mt_img.width, rows_needed * 8), 0)
grown.putpalette(mt_img.getpalette())
grown.paste(mt_img, (0, 0))
gp = grown.load()
for i, tdata in enumerate(new_tiles):
    tid = existing_n + i
    tx, ty = (tid % MTW) * 8, (tid // MTW) * 8
    for y in range(8):
        for x in range(8):
            gp[tx+x, ty+y] = tdata[y*8+x]
grown.save('data/tilesets/secondary/magnet_train/tiles.png', bits=4)
open('data/tilesets/secondary/magnet_train/metatiles.bin', 'wb').write(mm)
open('data/tilesets/secondary/magnet_train/metatile_attributes.bin', 'wb').write(ma)
lines = ['JASC-PAL', '0100', '16', '16 120 16'] + ['%d %d %d' % c for c in allc] + ['0 0 0'] * (15 - len(allc))
open('data/tilesets/secondary/magnet_train/palettes/%02d.pal' % PAL_SLOT, 'w').write('\n'.join(lines) + '\n')

# ---------- labeled preview -----------------------------------------------------
prev = Image.new('RGBA', (COLS*52 + 8, ROWS*60 + 8), (60, 80, 120, 255))
dr = ImageDraw.Draw(prev)
for (cx, cy), mid in assignments.items():
    cell = cells[(cx, cy)]
    prev.alpha_composite(cell.resize((48, 48), Image.NEAREST), (4 + cx*52, 4 + cy*60))
    dr.text((4 + cx*52, 52 + cy*60), '%03X' % mid, fill=(255, 255, 100))
prev.save(os.path.join(SCR, 'priderock_ids.png'))
print('id map saved; range 0x%03X-0x%03X' % (0x200 + base_meta, 0x200 + base_meta + len(assignments) - 1))
