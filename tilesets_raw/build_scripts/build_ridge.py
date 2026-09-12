"""Append Sootopolis's snowy crater-wall ridge to gTileset_SnowStone.

WHY THIS IS NOT A STRAIGHT METATILE COPY. Sootopolis's crater wall is not art
Sootopolis owns -- it is gTileset_General's cliff geometry painted with a pale
palette Sootopolis supplies. Both tilesets pair with General, so the tempting
move is to copy the metatiles verbatim and rewrite the palette slot.

That does not work, and the reason is worth keeping. A metatile entry names a
TILE and a PALETTE SLOT; the tile's pixels are indices into that slot. Those
pixels live in the PRIMARY and cannot be rewritten, so every colour has to sit
at exactly the index Sootopolis put it at -- indices 3, 5, 8, 9, 10, 11, 12,
13, 14. "Slot 10 has seven free indices" is therefore not enough: they have to
be the RIGHT seven, and no slot in SnowStone offers that set (slot 10, the
roomiest, still clashes at 3, 8, 9 and 10).

So the tiles are COPIED into SnowStone and their indices repacked into whatever
that slot has free. That costs real tiles -- 8x8 pixels each -- which is why the
selection below is kept tight.

BUDGET. SnowStone was at 432/512 tiles and 240/512 metatiles, so tiles are the
scarce side. The ridge fits comfortably; the exact figures are printed.

PALETTE. Only the crater-wall colours need a home; quadrants drawn with the
primary's own palettes 0-3 keep referencing General for free. Colours already
present in the target slot are reused rather than duplicated.
"""
import os
import shutil
import struct

from PIL import Image

TS = r"J:\ROM Hack Project\pokeemerald-expansion\data\tilesets"
SNOW = os.path.join(TS, "secondary", "snow_stone")
SOOT = os.path.join(TS, "secondary", "sootopolis")
GENERAL = os.path.join(TS, "primary", "general")

SOOT_SLOT = 7          # Sootopolis's crater-wall palette
TARGET_SLOT = 10       # SnowStone's roomiest slot: 9 indices used, 7 free
REUSE_DE = 26          # below this, reuse a colour the slot already has
RIDGE_RANGE = range(205, 254)
MIN_PALE = 0.70


def load_pal(d, i):
    p = os.path.join(d, "palettes", "%02d.pal" % i)
    cols = [tuple(int(x) for x in ln.split())
            for ln in open(p).read().splitlines()[3:] if ln.strip()]
    return (cols + [(0, 0, 0)] * 16)[:16]


def save_pal(d, i, cols):
    with open(os.path.join(d, "palettes", "%02d.pal" % i), "w") as f:
        f.write("\n".join(["JASC-PAL", "0100", "16"] +
                          ["%d %d %d" % c for c in cols[:16]]) + "\n")


def tilesheet(d):
    im = Image.open(os.path.join(d, "tiles.png")).convert("P")
    return im, im.load(), im.width // 8, (im.width // 8) * (im.height // 8)


def entries(mb, m):
    return [struct.unpack("<H", mb[m * 16 + k * 2:m * 16 + k * 2 + 2])[0]
            for k in range(8)]


def unpack(v):
    return v & 0x3FF, (v >> 10) & 1, (v >> 11) & 1, (v >> 12) & 0xF


def dE(a, b):
    return sum((a[i] - b[i]) ** 2 for i in range(3)) ** 0.5


def tile_pixels(gpx, gtw, gn, spx, stw, sn, tid):
    """The 64 palette indices of a tile, primary or Sootopolis-own."""
    tp, tw, cnt, j = (gpx, gtw, gn, tid) if tid < 512 else (spx, stw, sn, tid - 512)
    if j >= cnt:
        return None
    ox, oy = (j % tw) * 8, (j // tw) * 8
    return [tp[ox + x, oy + y] for y in range(8) for x in range(8)]


def used_indices_of_slot(d, slot):
    """Which colour indices the tileset's own art actually references in a slot.

    Anything NOT in here is free to overwrite -- that is the whole basis for
    fitting the ridge without disturbing what is already drawn.
    """
    gim, gpx, gtw, gn = tilesheet(GENERAL)
    sim, spx, stw, sn = tilesheet(d)
    mb = open(os.path.join(d, "metatiles.bin"), "rb").read()
    used = set()
    for m in range(len(mb) // 16):
        for v in entries(mb, m):
            tid, _, _, sl = unpack(v)
            if sl != slot:
                continue
            px = tile_pixels(gpx, gtw, gn, spx, stw, sn, tid)
            if px:
                used |= set(px)
    return used


def select_ridge(mb, gpx, gtw, gn, spx, stw, sn, sootpals):
    """The crater wall: pale, and drawn only from its own palette plus the
    primary's. Anything reaching for another Sootopolis palette is a roof, a
    sign or a bridge and is dropped."""
    picked = []
    for m in RIDGE_RANGE:
        ents = entries(mb, m)
        if all(unpack(v)[0] == 0 for v in ents):
            continue
        slots = set(unpack(v)[3] for v in ents if unpack(v)[0])
        if not slots <= {0, 1, 2, 3, SOOT_SLOT}:
            continue
        tot = good = 0
        for v in ents:
            tid, _, _, sl = unpack(v)
            px = tile_pixels(gpx, gtw, gn, spx, stw, sn, tid)
            if not px:
                continue
            for i in px:
                if not i:
                    continue
                c = sootpals[sl][i]
                tot += 1
                good += (max(c) >= 120 and max(c) - min(c) <= 60)
        if tot and good / float(tot) >= MIN_PALE:
            picked.append(m)
    return picked


MARKER = ".ridge"


def add_ridge(force=False):
    """Append the ridge. force=True when the base has just been regenerated.

    This APPENDS, and it re-stamps .generated on the way out, so the hand-edit
    guard cannot see a second run of this script -- it would silently double
    the metatiles and burn another 54 tiles. Hence the marker file. Guarding on
    a palette colour was tried and is not reliable: whether a given colour
    reaches the palette depends on REUSE_DE and on what the target slot already
    holds.
    """
    marker = os.path.join(SNOW, MARKER)
    if os.path.exists(marker) and not force:
        print("  ridge already present (%s says %s) -- nothing to do; "
              "pass force=True to append again"
              % (MARKER, open(marker).read().strip()))
        return None, None
    gim, gpx, gtw, gn = tilesheet(GENERAL)
    sim, spx, stw, sn = tilesheet(SOOT)
    nim, npx, ntw, nn = tilesheet(SNOW)
    sootpals = [load_pal(GENERAL, i) for i in range(6)] + \
               [load_pal(SOOT, i) for i in range(6, 16)]
    mb = open(os.path.join(SOOT, "metatiles.bin"), "rb").read()
    ab = open(os.path.join(SOOT, "metatile_attributes.bin"), "rb").read()

    picked = select_ridge(mb, gpx, gtw, gn, spx, stw, sn, sootpals)
    print("  ridge metatiles selected : %d" % len(picked))

    # --- which tiles actually need copying -------------------------------
    # Only those drawn with Sootopolis's own palette. Quadrants using the
    # primary's palettes 0-3 keep pointing at General and cost nothing.
    need = []
    for m in picked:
        for v in entries(mb, m):
            tid, _, _, sl = unpack(v)
            if sl == SOOT_SLOT and tid and tid not in need:
                need.append(tid)
    print("  tiles that must be copied: %d (%d from General, %d Sootopolis-own)"
          % (len(need), sum(1 for t in need if t < 512),
             sum(1 for t in need if t >= 512)))

    free_tiles = 512 - nn
    if len(need) > free_tiles:
        raise SystemExit("ridge needs %d tiles, only %d free" % (len(need), free_tiles))

    # --- colour placement -------------------------------------------------
    src_pal = sootpals[SOOT_SLOT]
    src_used = set()
    for tid in need:
        px = tile_pixels(gpx, gtw, gn, spx, stw, sn, tid)
        src_used |= set(px)
    src_used.discard(0)

    tgt = load_pal(SNOW, TARGET_SLOT)
    tgt_used = used_indices_of_slot(SNOW, TARGET_SLOT)
    free_idx = [i for i in range(1, 16) if i not in tgt_used]

    remap = {0: 0}
    reused = added = 0
    for si in sorted(src_used):
        c = src_pal[si]
        cands = [(dE(c, tgt[i]), i) for i in sorted(tgt_used) if i]
        d, i = min(cands) if cands else (999, -1)
        if d <= REUSE_DE:
            remap[si] = i
            reused += 1
        else:
            if not free_idx:
                raise SystemExit("target slot %d out of free indices" % TARGET_SLOT)
            ni = free_idx.pop(0)
            tgt[ni] = c
            remap[si] = ni
            added += 1
    print("  colours: %d needed -> %d reused from slot %d, %d written into free "
          "indices (%d still spare)" % (len(src_used), reused, TARGET_SLOT,
                                        added, len(free_idx)))

    if force or True:
        save_pal(SNOW, TARGET_SLOT, tgt)

    # --- append the tiles -------------------------------------------------
    newid = {}
    rows = (nn + len(need) + 15) // 16
    out = Image.new("P", (128, rows * 8), 0)
    out.putpalette(nim.getpalette())
    out.paste(nim, (0, 0))
    opx = out.load()
    for k, tid in enumerate(need):
        dst = nn + k
        newid[tid] = 512 + dst
        px = tile_pixels(gpx, gtw, gn, spx, stw, sn, tid)
        ox, oy = (dst % 16) * 8, (dst // 16) * 8
        for y in range(8):
            for x in range(8):
                opx[ox + x, oy + y] = remap[px[y * 8 + x]]
    out.save(os.path.join(SNOW, "tiles.png"), bits=4)

    # --- append the metatiles --------------------------------------------
    nmb = bytearray(open(os.path.join(SNOW, "metatiles.bin"), "rb").read())
    nab = bytearray(open(os.path.join(SNOW, "metatile_attributes.bin"), "rb").read())
    first = len(nmb) // 16
    for m in picked:
        for v in entries(mb, m):
            tid, xf, yf, sl = unpack(v)
            if sl == SOOT_SLOT and tid:
                tid, sl = newid[tid], TARGET_SLOT
            nmb += struct.pack("<H", (sl << 12) | (yf << 11) | (xf << 10) | (tid & 0x3FF))
        nab += ab[m * 2:m * 2 + 2]          # keep Sootopolis's collision/behaviour
    open(os.path.join(SNOW, "metatiles.bin"), "wb").write(nmb)
    open(os.path.join(SNOW, "metatile_attributes.bin"), "wb").write(nab)

    # stale build products must go so make regenerates them
    for junk in ("tiles.4bpp", "tiles.4bpp.lz"):
        p = os.path.join(SNOW, junk)
        if os.path.exists(p):
            os.remove(p)
    for i in range(16):
        p = os.path.join(SNOW, "palettes", "%02d.gbapal" % i)
        if os.path.exists(p):
            os.remove(p)

    # Re-stamp: these files were written by tilesetgen and have now been
    # changed on purpose, so the guard has to be told, or the next build
    # reports the ridge as a hand edit and refuses to run.
    from tilesetgen import Builder
    Builder._write_stamp(Builder.__new__(Builder), SNOW)
    open(marker, "w").write(
        "sootopolis crater wall, metatiles %d-%d, %d tiles\n"
        % (first, len(nmb) // 16 - 1, len(need)))

    total_t, total_m = nn + len(need), len(nmb) // 16
    print("  gTileset_SnowStone now %d/512 tiles, %d/512 metatiles "
          "(ridge is %d..%d)" % (total_t, total_m, first, total_m - 1))
    return first, total_m - 1


if __name__ == "__main__":
    add_ridge()
