#!/usr/bin/env python3
# NON-DESTRUCTIVE patch: turn the ashen grass-tuft metatile (#111) into proper
# TALL GRASS -- make its top-layer grey GROUND pixels transparent (so the player
# shows THROUGH it), and flag it MB_TALL_GRASS (rustle + wild encounters).
#
# Only metatile 111 and its 4 exclusive top-layer tiles (203/204/210/211) are
# touched; every other metatile, attribute, and Porymap edit is preserved.
#
#   Run from the repo root in WSL:  python3 tools/patch_ash_tallgrass.py
#   then `make`.  In Porymap, paint the grass with metatile 111 as usual.
#
# NOTE: this edits the tileset binaries in place. Do NOT run
# tools/build_ashland_trees.py afterwards -- that regenerates the whole tileset
# from source art and would wipe this patch (and your other Porymap edits).
import struct, sys
from PIL import Image
DST = sys.argv[1] if len(sys.argv) > 1 else "data/tilesets/secondary/ashland_trees"
NUM = 512; MB_TALL_GRASS = 0x0002; TARGET = 111

def loadpal(s):
    L = open(f"{DST}/palettes/{s:02d}.pal").read().split("\n")[3:]
    return ([tuple(map(int, l.split())) for l in L if l.strip()] + [(0, 0, 0)] * 16)[:16]
pals = [loadpal(s) for s in range(16)]

def is_ground(c):                       # near-neutral grey AND fairly bright = ash ground
    r, g, b = c; mx, mn = max(c), min(c)
    sat = 0 if mx == 0 else (mx - mn) / mx
    lum = 0.3 * r + 0.59 * g + 0.11 * b
    return sat < 0.18 and lum > 58

mt = open(f"{DST}/metatiles.bin", "rb").read(); N = len(mt) // 16
top = []
for i in range(4):
    v = struct.unpack_from("<H", mt, TARGET * 16 + 2 * (4 + i))[0]
    top.append(((v & 0x3FF) - NUM, (v >> 12) & 0xF))

def users(tid):
    out = set()
    for m in range(N):
        for i in range(8):
            if (struct.unpack_from("<H", mt, m * 16 + 2 * i)[0] & 0x3FF) - NUM == tid:
                out.add(m)
    return out
for tid, pal in top:
    if tid < 0:
        continue
    u = users(tid)
    assert u == {TARGET}, f"tile {tid} shared by {sorted(u)} -- abort (would affect other metatiles)"

im = Image.open(f"{DST}/tiles.png"); px = im.load(); cols = im.size[0] // 8
changed = 0
for tid, pal in top:
    if tid < 0:
        continue
    bx, by = (tid % cols) * 8, (tid // cols) * 8
    for y in range(8):
        for x in range(8):
            idx = px[bx + x, by + y]
            if idx != 0 and is_ground(pals[pal][idx]):
                px[bx + x, by + y] = 0; changed += 1
im.save(f"{DST}/tiles.png")

at = bytearray(open(f"{DST}/metatile_attributes.bin", "rb").read())
need = (TARGET + 1) * 2
if len(at) < need:
    at += b"\x00" * (need - len(at))
struct.pack_into("<H", at, TARGET * 2, MB_TALL_GRASS)
open(f"{DST}/metatile_attributes.bin", "wb").write(at)
print(f"patched metatile {TARGET}: {changed} ground px -> transparent; "
      f"attr=MB_TALL_GRASS; top tiles {[t for t, _ in top]}")
