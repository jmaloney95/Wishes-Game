"""Append Route 112's cable-car station (building, cable post, cable) from
gTileset_Lavaridge to gTileset_SnowVillage, Frostwood Town's secondary.

SnowVillage is hand-edited in porymap, so this is a DIRECT APPEND: existing
tiles, metatiles, attributes and every palette colour a tile uses stay
byte-identical (asserted). Nothing goes through tilesetgen.

All seven secondary palette slots are occupied, so the art is colour-fitted:
new colours go into palette indices no Frostwood tile uses (primary slots
0-5 and secondary 6-12), per colour family (see PLAN), and every other
colour maps to the nearest one already in that slot.

Lavaridge keeps the art in the top four entries on transparency with the
ground underneath, so the ground is swapped for Frostwood's plain snow
(SnowVillageHomes metatile 0x000).

    python import_cable_car.py            preview only (writes PNGs to scratch)
    python import_cable_car.py --apply    write into the tileset

Idempotent via .cablecar in the tileset directory.
"""
import os, sys, struct, shutil, re
from collections import Counter
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
G = os.path.normpath(os.path.join(HERE, "..", "..", "pokeemerald-expansion"))
LAV = os.path.join(G, "data/tilesets/secondary/lavaridge")
SV = os.path.join(G, "data/tilesets/secondary/snow_village")
SVH = os.path.join(G, "data/tilesets/primary/snow_village_homes")
GEN = os.path.join(G, "data/tilesets/primary/general")
MARK = os.path.join(SV, ".cablecar")
OUT = sys.argv[sys.argv.index("--out") + 1] if "--out" in sys.argv else HERE
APPLY = "--apply" in sys.argv

SNOW = None  # filler: plain snow, no art
# 8 columns, as porymap's metatile picker shows them. Building 6x5 in cols 0-5
# exactly as painted on Route 112 (27..32, 23..27); cable post 1x3 in col 6;
# cable pieces in col 7: horizontal, diagonal top half, full, bottom half.
LAYOUT = [
    [0x220, 0x221, 0x221, 0x223, 0x224, SNOW,  0x254, 0x25A],
    [0x228, 0x229, 0x229, 0x22B, 0x22C, 0x224, 0x25C, 0x25B],
    [0x230, 0x231, 0x231, 0x233, 0x234, 0x23D, 0x264, 0x252],
    [0x238, 0x239, 0x23A, 0x23B, 0x23C, 0x23D, SNOW,  0x253],
    [0x240, 0x241, 0x242, 0x243, 0x244, 0x245, SNOW,  SNOW],
]
SNOW_META = 0x000            # SnowVillageHomes plain snow
MB_NORMAL, MB_NON_ANIMATED_DOOR = 0x00, 0x60

# indices no Frostwood tile uses, measured from both tilesets' metatiles.
# SnowVillageHomes tiles 6/54/55 are unreferenced but use 13-14, so slot 5
# keeps 14 back in case they're for a pal-5 metatile Joe hasn't made yet.
FREE = {3: [7, 12, 15], 5: [15], 8: [7], 9: [13, 14, 15], 10: [14, 15], 11: [8, 15], 12: [13, 14, 15]}

# Which Lavaridge colours go into those indices. A greedy pixel-count fit
# scattered the wood across slots 3/10/12, so roof tiles that landed in a
# slot without the mid orange drew it salmon. Each family is kept whole here,
# picked against what the slot already has:
#   12 wood: + light/mid/dark orange; has A85850 (shadow), 484040 (outline), 789090 (grey)
#    9 walls: + two whites, khaki shade; has 607080, B8B888, 789090, 203048, 70B0D0 (window)
#    3 cable post: + two blues, base grey; has 384860 (outline), E0E0E0 (highlight)
#    8 cable: + light rope; has 384858, 606860, 988890
PLAN = {
    12: [(0xEE, 0xB4, 0x8B), (0xE6, 0x94, 0x52), (0xC5, 0x83, 0x4A)],
    9: [(0xF6, 0xEE, 0xEE), (0xE6, 0xDE, 0xDE), (0xB4, 0xAC, 0x73)],
    3: [(0x73, 0xBD, 0xEE), (0x8B, 0xCD, 0xF6), (0x94, 0x94, 0x8B)],
    8: [(0xC5, 0xC5, 0xAC)],
}
# Every Lavaridge tile is pinned to its family's slot, so neighbouring tiles
# never disagree about a colour (the door and side windows otherwise came out
# two-tone), and the navy outline is one colour everywhere it can be.
SLOT_OF = {}
for _t in (0x202, 0x203, 0x204, 0x205, 0x212, 0x213, 0x214, 0x215, 0x222, 0x223, 0x224, 0x235, 0x245):
    SLOT_OF[_t] = 12
for _t in (0x210, 0x211, 0x220, 0x221, 0x230, 0x231, 0x232, 0x233, 0x234,
           0x240, 0x241, 0x242, 0x243, 0x244, 0x250, 0x251, 0x252, 0x253, 0x255):
    SLOT_OF[_t] = 9
for _t in (0x209, 0x20A, 0x219, 0x21A, 0x229, 0x22A, 0x239, 0x23A):
    SLOT_OF[_t] = 3
for _t in (0x300, 0x310, 0x311, 0x312):
    SLOT_OF[_t] = 8
OVERRIDE = {(0x41, 0x4A, 0x6A): (0x48, 0x40, 0x40)}   # outline -> Frostwood's dark brown, where the slot has it


def read_pal(d, i):
    lines = open(os.path.join(d, "palettes", "%02d.pal" % i)).read().split("\n")
    cols = [tuple(int(x) for x in l.split()) for l in lines[3:] if l.strip()]
    return (cols + [(0, 0, 0)] * 16)[:16]


def write_pal(d, i, cols):
    p = os.path.join(d, "palettes", "%02d.pal" % i)
    raw = open(p, "rb").read()
    eol = "\r\n" if b"\r\n" in raw else "\n"
    text = eol.join(["JASC-PAL", "0100", "16"] + ["%d %d %d" % c for c in cols]) + eol
    open(p, "wb").write(text.encode())


def read_tiles(d):
    im = Image.open(os.path.join(d, "tiles.png"))
    px = im.load(); W, H = im.size
    return im, [[px[(t % (W // 8)) * 8 + x, (t // (W // 8)) * 8 + y] & 15 for y in range(8) for x in range(8)]
                for t in range((W // 8) * (H // 8))]


def u16s(path):
    b = open(path, "rb").read()
    return list(struct.unpack("<%dH" % (len(b) // 2), b))


def flip(t, h, v):
    return [t[(7 - y if v else y) * 8 + (7 - x if h else x)] for y in range(8) for x in range(8)]


def d2(a, b):
    r = (a[0] + b[0]) / 2
    dr, dg, db = a[0] - b[0], a[1] - b[1], a[2] - b[2]
    return (2 + r / 256) * dr * dr + 4 * dg * dg + (2 + (255 - r) / 256) * db * db


def main():
    if os.path.exists(MARK):
        sys.exit("already imported (%s exists)" % MARK)

    lav_meta = u16s(os.path.join(LAV, "metatiles.bin"))
    lav_attr = u16s(os.path.join(LAV, "metatile_attributes.bin"))
    _, lav_tiles = read_tiles(LAV)
    lav_pals = [read_pal(LAV, i) for i in range(16)]

    sv_png, sv_tiles = read_tiles(SV)
    svh_meta = u16s(os.path.join(SVH, "metatiles.bin"))
    sv_meta_raw = open(os.path.join(SV, "metatiles.bin"), "rb").read()
    sv_attr_raw = open(os.path.join(SV, "metatile_attributes.bin"), "rb").read()
    pals = [read_pal(SVH, i) for i in range(6)] + [read_pal(SV, i) for i in range(6, 13)]
    orig_pals = [list(p) for p in pals]

    # ---- the art: (lavaridge tile, lavaridge pal) pairs in the top layer
    items = {}
    for row in LAYOUT:
        for mid in row:
            if mid is None:
                continue
            for v in lav_meta[(mid - 512) * 8 + 4:(mid - 512) * 8 + 8]:
                tid, pl = v & 0x3FF, v >> 12
                if tid < 512:
                    assert tid == 0, "top layer uses a General tile %X in %X" % (tid, mid)
                    continue
                t = lav_tiles[tid - 512]
                if any(t):
                    items[(tid, pl)] = Counter(lav_pals[pl][i] for i in t if i)
    srccols = sorted({c for cnt in items.values() for c in cnt})

    # ---- colour fit: each colour family lives whole in one palette
    used = {s: [i for i in range(1, 16) if i not in FREE.get(s, [])] for s in range(13)}
    for s, cols in PLAN.items():
        assert len(cols) <= len(FREE[s]), s
        for j, c in enumerate(cols):
            pals[s][FREE[s][j]] = c
    avail = {s: used[s] + FREE.get(s, [])[:len(PLAN.get(s, []))] for s in range(13)}

    # ---- remap every art tile into its best slot
    remap = {}
    report = []
    for key, cnt in items.items():
        tid, pl = key
        s = SLOT_OF[tid]
        cmap = {0: 0}
        worst = 0
        for i in set(lav_tiles[tid - 512]) - {0}:
            c = lav_pals[pl][i]
            want = OVERRIDE.get(c, c)
            if want not in [tuple(pals[s][k]) for k in avail[s]]:
                want = c   # slot lacks the override colour: nearest to the original instead
            k = min(avail[s], key=lambda k: d2(want, pals[s][k]))
            cmap[i] = k
            worst = max(worst, d2(c, pals[s][k]) ** 0.5)
            if "--verbose" in sys.argv:
                print("    %03X p%d slot %d: %02X%02X%02X -> %02X%02X%02X x%d" % ((tid, pl, s) + c + tuple(pals[s][k]) + (cnt[c],)))
        remap[key] = (s, [cmap[i] for i in lav_tiles[tid - 512]])
        report.append((tid, pl, s, worst))

    # ---- tiles: reuse SnowVillage's trailing blank padding, then append; dedupe with flips
    blank_tail = []
    t = len(sv_tiles) - 1
    while t > 0 and not any(sv_tiles[t]):
        blank_tail.insert(0, t); t -= 1
    new_tiles = [list(x) for x in sv_tiles]
    free_ids = list(blank_tail)
    placed = {}   # (slot, tuple pixels) -> (local id, h, v)

    def place(pix, s):
        for h in (0, 1):
            for v in (0, 1):
                k = (s, tuple(flip(pix, h, v)))
                if k in placed:
                    lid, hh, vv = placed[k]
                    return lid, hh ^ h, vv ^ v
        if free_ids:
            lid = free_ids.pop(0)
            new_tiles[lid] = pix
        else:
            lid = len(new_tiles)
            new_tiles.append(pix)
        placed[(s, tuple(pix))] = (lid, 0, 0)
        return lid, 0, 0

    snow_bottom = svh_meta[SNOW_META * 8:SNOW_META * 8 + 4]
    metas, attrs = [], []
    for row in LAYOUT:
        for mid in row:
            top = [0, 0, 0, 0]
            attr = MB_NORMAL
            if mid is not None:
                for q, v in enumerate(lav_meta[(mid - 512) * 8 + 4:(mid - 512) * 8 + 8]):
                    tid, pl = v & 0x3FF, v >> 12
                    if (tid, pl) not in remap:
                        continue
                    s, pix = remap[(tid, pl)]
                    lid, h, vf = place(pix, s)
                    h ^= (v >> 10) & 1; vf ^= (v >> 11) & 1
                    top[q] = (512 + lid) | (h << 10) | (vf << 11) | (s << 12)
                la = lav_attr[mid - 512]
                beh = MB_NON_ANIMATED_DOOR if (la & 0xFF) == MB_NON_ANIMATED_DOOR else MB_NORMAL
                attr = beh | (la & 0xF000)
            metas.extend(list(snow_bottom) + top)
            attrs.append(attr)

    n_old_tiles = len(sv_tiles)
    n_tiles = len(new_tiles)
    n_old_meta = len(sv_meta_raw) // 16
    assert n_tiles <= 512, n_tiles
    assert n_old_meta + len(attrs) <= 512
    for s, cols in PLAN.items():
        print("  slot %2d += %s" % (s, " ".join("%02X%02X%02X@%d" % (c + (FREE[s][j],)) for j, c in enumerate(cols))))
    for tid, pl, s, w in sorted(report, key=lambda r: -r[3])[:12]:
        print("  tile %03X p%d -> slot %d, worst colour miss %.0f" % (tid, pl, s, w))
    print("tiles %d -> %d (reused %d blank padding), metatiles %d -> %d (new 0x%03X-0x%03X)"
          % (n_old_tiles, n_tiles, len(blank_tail) - len(free_ids), n_old_meta, n_old_meta + len(attrs),
             512 + n_old_meta, 512 + n_old_meta + len(attrs) - 1))

    # ---- preview: original (Lavaridge on General) vs imported (on Frostwood's pair)
    def render(tile_lookup, pal_lookup, entries_list, cols=8):
        rows = (len(entries_list) + cols - 1) // cols
        img = Image.new("RGB", (cols * 16, rows * 16), (255, 0, 255))
        px = img.load()
        for m, e in enumerate(entries_list):
            ox, oy = (m % cols) * 16, (m // cols) * 16
            for L in (0, 1):
                for q in range(4):
                    v = e[L * 4 + q]
                    tile = tile_lookup(v & 0x3FF)
                    if tile is None:
                        continue
                    tile = flip(tile, (v >> 10) & 1, (v >> 11) & 1)
                    for y in range(8):
                        for x in range(8):
                            i = tile[y * 8 + x]
                            if L == 1 and i == 0:
                                continue
                            px[ox + (q % 2) * 8 + x, oy + (q // 2) * 8 + y] = pal_lookup(v >> 12)[i]
        return img

    _, gen_tiles = read_tiles(GEN)
    gen_pals = [read_pal(GEN, i) for i in range(6)]
    lav_entries = [lav_meta[(m - 512) * 8:(m - 512) * 8 + 8] if m is not None else [0] * 8 for row in LAYOUT for m in row]
    before = render(lambda t: gen_tiles[t] if t < 512 else lav_tiles[t - 512], lambda s: (gen_pals + lav_pals[6:])[s], lav_entries)
    _, svh_tiles = read_tiles(SVH)
    after = render(lambda t: svh_tiles[t] if t < 512 else new_tiles[t - 512], lambda s: pals[s], [metas[i * 8:i * 8 + 8] for i in range(len(attrs))])
    both = Image.new("RGB", (before.width * 2 + 8, before.height), (40, 40, 40))
    both.paste(before, (0, 0)); both.paste(after, (before.width + 8, 0))
    both = both.resize((both.width * 4, both.height * 4), Image.NEAREST)
    prev = os.path.join(OUT, "cable_car_preview.png")
    both.save(prev)
    print("preview", prev)

    if not APPLY:
        return

    # ---- write, then prove the old data survived
    snap = os.path.join(OUT, "snow_village_snapshot")
    if os.path.exists(snap):
        shutil.rmtree(snap)
    shutil.copytree(SV, snap)
    shutil.copytree(os.path.join(SVH, "palettes"), os.path.join(snap, "svh_palettes"))

    W = 16 * 8
    H = ((n_tiles + 15) // 16) * 8
    out = Image.new("P", (W, H), 0)
    out.putpalette(sv_png.getpalette())
    opx = out.load()
    for t, pix in enumerate(new_tiles):
        for y in range(8):
            for x in range(8):
                opx[(t % 16) * 8 + x, (t // 16) * 8 + y] = pix[y * 8 + x]
    out.save(os.path.join(SV, "tiles.png"))
    open(os.path.join(SV, "metatiles.bin"), "wb").write(sv_meta_raw + struct.pack("<%dH" % len(metas), *metas))
    open(os.path.join(SV, "metatile_attributes.bin"), "wb").write(sv_attr_raw + struct.pack("<%dH" % len(attrs), *attrs))
    for s in range(13):
        if pals[s] != orig_pals[s]:
            write_pal(SVH if s < 6 else SV, s, pals[s])

    _, chk = read_tiles(SV)
    for t in range(n_old_tiles):
        if t not in blank_tail:
            assert chk[t] == sv_tiles[t], "tile %d changed" % t
    assert open(os.path.join(SV, "metatiles.bin"), "rb").read()[:len(sv_meta_raw)] == sv_meta_raw
    assert open(os.path.join(SV, "metatile_attributes.bin"), "rb").read()[:len(sv_attr_raw)] == sv_attr_raw
    for s in range(13):
        now = read_pal(SVH if s < 6 else SV, s)
        for i in range(16):
            if i not in FREE.get(s, []):
                assert now[i] == orig_pals[s][i], "slot %d idx %d changed" % (s, i)
    open(MARK, "w").write("cable car station appended as metatiles 0x%03X-0x%03X\n"
                          % (512 + n_old_meta, 512 + n_old_meta + len(attrs) - 1))
    print("applied; snapshot at", snap)


main()
