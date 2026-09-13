# -*- coding: utf-8 -*-
import sys
sys.exit("SUPERSEDED -- do not run. it hardcodes MB_ICE=37 / MB_FOOTPRINTS=42, which are grep LINE NUMBERS, not enum values (37 is MB_THIN_ICE, 42 is MB_REFLECTION_UNDER_BRIDGE). --apply would reintroduce the no-slide / underwater-bubble bug. Use fix_snow_behaviours.py instead.")
"""Pick the exact ice and snow metatiles, by signature rather than by eye.

Metatile 210 is unambiguously the ice sheet -- MunenVillage paints it 59 times
as walkable -- so its colour set is the reference. A colour threshold alone
over-catches: snowdrifts and blue-grey rocks score as "blue" too, which is why
the first pass returned 84 candidates including windows, lamps and grass.

Dry run by default; pass --apply to write.
"""
import os
import struct
import sys
from collections import Counter

from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = r"J:\ROM Hack Project\pokeemerald-expansion"
TS = os.path.join(ROOT, "data", "tilesets")
SNOW = os.path.join(TS, "secondary", "snow_stone")
GEN = os.path.join(TS, "primary", "general")
MAPBIN = os.path.join(ROOT, "data", "layouts", "MunenVillage", "map.bin")

MB_ICE, MB_FOOTPRINTS = 37, 42
ICE_REF = 210
BEHAVIOR_MASK = 0x1FF


def load_pals(d):
    out = []
    for i in range(16):
        p = os.path.join(d, "palettes", "%02d.pal" % i)
        if not os.path.exists(p):
            out.append([(0, 0, 0)] * 16)
            continue
        cols = [tuple(int(x) for x in ln.split())
                for ln in open(p).read().splitlines()[3:] if ln.strip()]
        out.append((cols + [(0, 0, 0)] * 16)[:16])
    return out


def sheet(d):
    im = Image.open(os.path.join(d, "tiles.png")).convert("P")
    return im.load(), im.width // 8, (im.width // 8) * (im.height // 8)


gpx, gtw, gn = sheet(GEN)
npx, ntw, nn = sheet(SNOW)
pal = load_pals(GEN)[:6] + load_pals(SNOW)[6:]
mb = open(os.path.join(SNOW, "metatiles.bin"), "rb").read()
ab = bytearray(open(os.path.join(SNOW, "metatile_attributes.bin"), "rb").read())
NM = len(mb) // 16


def ents(m):
    return [struct.unpack("<H", mb[m * 16 + k * 2:m * 16 + k * 2 + 2])[0]
            for k in range(8)]


def pixels(m):
    out = {}
    for k in range(8):
        v = ents(m)[k]
        q = k % 4
        t, xf, yf, sl = v & 0x3FF, (v >> 10) & 1, (v >> 11) & 1, (v >> 12) & 0xF
        tp, tw, cnt, j = (gpx, gtw, gn, t) if t < 512 else (npx, ntw, nn, t - 512)
        if j >= cnt:
            continue
        ox, oy = (j % tw) * 8, (j // tw) * 8
        qx, qy = (q % 2) * 8, (q // 2) * 8
        for y in range(8):
            for x in range(8):
                i = tp[ox + (7 - x if xf else x), oy + (7 - y if yf else y)] & 15
                if i:
                    out[(qx + x, qy + y)] = pal[sl][i]
    return out


usage, walkable = Counter(), Counter()
b = open(MAPBIN, "rb").read()
for i in range(0, len(b), 2):
    v = struct.unpack("<H", b[i:i + 2])[0]
    mt, col = v & 0x3FF, (v >> 10) & 3
    if mt >= 512:
        usage[mt - 512] += 1
        if col == 0:
            walkable[mt - 512] += 1

# ---- ice: share the reference tile's colour set ------------------------
ref = Counter(pixels(ICE_REF).values())
ref_cols = {c for c, n in ref.items() if n >= 8}      # its real palette
print("reference ice metatile %d uses %d colours: %s"
      % (ICE_REF, len(ref_cols),
         " ".join("%02X%02X%02X" % c for c in sorted(ref_cols))))

ice = []
for m in range(NM):
    px = pixels(m)
    if not px:
        continue
    # 0.55, not 0.85: the ice EDGE tiles carry a rock or fence border and dip
    # below a strict threshold, but the discriminator here is two very specific
    # cyans that nothing else in the tileset uses -- snowdrifts are built from
    # A8C8D8/C8E0E8/E8E8F0, so a lower bar stays safe.
    share = sum(1 for c in px.values() if c in ref_cols) / float(len(px))
    if share >= 0.55:
        ice.append(m)

# ---- snow ground: pale, not ice, and something the player stands on ----
snow = []
for m in range(NM):
    if m in ice:
        continue
    px = pixels(m)
    if not px or len(px) < 256:            # a full, solid cell
        continue
    pale = sum(1 for c in px.values() if max(c) >= 170 and max(c) - min(c) <= 40)
    if pale / float(len(px)) >= 0.85 and walkable.get(m, 0) > 0:
        snow.append(m)

print("\nICE (>=85%% of the reference colour set): %d metatiles" % len(ice))
print("   %s" % ice)
print("   walkable in the map: %s" % [m for m in ice if walkable.get(m)])
print("\nSNOW GROUND (pale, solid, walkable in the map): %d metatiles" % len(snow))
print("   %s" % snow)

print("\nbehaviours already set (will not be touched):")
for m in range(NM):
    bv = struct.unpack("<H", ab[m * 2:m * 2 + 2])[0] & BEHAVIOR_MASK
    if bv:
        print("   metatile %3d : MB %d" % (m, bv))

# ---- render for confirmation -------------------------------------------
Z, COLS = 4, 20
sets = [("ICE -> MB_ICE (slippery)", ice), ("SNOW -> MB_FOOTPRINTS (tracks)", snow)]
H = sum(20 + ((len(s[1]) + COLS - 1) // COLS) * (16 * Z + 14) for s in sets) + 10
img = Image.new("RGB", (COLS * (16 * Z + 4) + 6, H), (28, 28, 36))
dr = ImageDraw.Draw(img)
y = 4
for title, ms in sets:
    dr.text((4, y), "%s  (%d)" % (title, len(ms)), fill=(230, 230, 240))
    y += 18
    for i, m in enumerate(ms):
        gx = 4 + (i % COLS) * (16 * Z + 4)
        gy = y + (i // COLS) * (16 * Z + 14)
        for (a, c2), col in pixels(m).items():
            dr.rectangle([gx + a * Z, gy + c2 * Z, gx + a * Z + Z - 1,
                          gy + c2 * Z + Z - 1], fill=col)
        dr.text((gx, gy + 16 * Z + 1), "%d/%d" % (m, walkable.get(m, 0)),
                fill=(150, 220, 150) if walkable.get(m) else (140, 140, 150))
    y += ((len(ms) + COLS - 1) // COLS) * (16 * Z + 14) + 6
img.save(os.path.join(HERE, "behaviours_final.png"))
print("\nwrote behaviours_final.png (label = id / walkable uses in the map)")

if "--apply" in sys.argv:
    n = 0
    for m in ice:
        v = struct.unpack("<H", ab[m * 2:m * 2 + 2])[0]
        struct.pack_into("<H", ab, m * 2, (v & ~BEHAVIOR_MASK) | MB_ICE)
        n += 1
    for m in snow:
        v = struct.unpack("<H", ab[m * 2:m * 2 + 2])[0]
        struct.pack_into("<H", ab, m * 2, (v & ~BEHAVIOR_MASK) | MB_FOOTPRINTS)
        n += 1
    open(os.path.join(SNOW, "metatile_attributes.bin"), "wb").write(ab)
    print("\nAPPLIED: %d metatile behaviours written" % n)
else:
    print("\n(dry run -- pass --apply to write)")
