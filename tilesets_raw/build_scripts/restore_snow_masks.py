"""Restore the transparent top layer on SnowStone's snow scenery.

THE BUG. build_snow_stone imported droid779_snow through flatten_secondary(),
which composites both layers into one image. add_unique() then re-derived the
layer split from that flattened result:

    opaque = all((-1, -1, -1) not in t for t in sub)
    if opaque: ... single layer

so any source cell whose top layer happened to cover its bottom came out
"opaque" and was baked flat. droid779 draws its scenery -- trees, rocks,
snowballs, signs, drifts -- as a MASKED top layer over ground, and 28 of those
lost the mask. You cannot change what is behind them, which is what Joe hit.

WHY THIS IS RECOVERABLE. The mask still exists in droid779_snow. Content
matching cannot find it (the build re-quantised every colour, so nothing
renders identically), but the placement is deterministic: replaying the build
to pack() and reading the Builder's labels gives metatile id -> source cell
exactly. That replay is verified against the known 427 tiles / 240 metatiles.

WHAT THIS MUST NOT DISTURB. Joe has painted MUNEN_VILLAGE with this tileset and
hand-added metatiles 267-349, so:
  * no metatile id may move;
  * no tile id may move either, since his metatiles reference them;
  * new tiles therefore go only into slots that nothing references once the
    conversion is done, plus the unallocated tail. A slot can be repurposed in
    place; only REMOVING one shifts the rest.

droid779's cells sat on their own ground, sometimes snow and sometimes grass.
The rebuilt bottom layer is gTileset_General's snow, matching what
build_snow_stone already uses for the cells that kept their mask -- so the few
grass-backed ones change from grass to snow, which is the point: the ground
becomes yours to choose.
"""
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

from PIL import Image

TS = r"J:\ROM Hack Project\pokeemerald-expansion\data\tilesets"
SNOW = os.path.join(TS, "secondary", "snow_stone")
DROID = os.path.join(TS, "secondary", "droid779_snow")
GENERAL = os.path.join(TS, "primary", "general")

SNOW_BASE_ID = 94        # General's snow metatile, the new bottom layer
MARKER = ".masks"


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
    return im, im.load(), im.width // 8, (im.width // 8) * (im.height // 8)


def metas(d):
    return open(os.path.join(d, "metatiles.bin"), "rb").read()


def ents(mb, m):
    return [struct.unpack("<H", mb[m * 16 + k * 2:m * 16 + k * 2 + 2])[0]
            for k in range(8)]


def dE(a, b):
    return (a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2 + (a[2] - b[2]) ** 2


def recover_mapping():
    """metatile id -> droid779 cell, by replaying the build to pack()."""
    import tempfile
    import build_snow_stone as B
    from tilesetgen import Sheet, Builder, PrimaryBase

    keyed, _ = B.key_checker(B.SAS_SRC)
    A = Sheet(keyed)
    dpng = os.path.join(tempfile.gettempdir(), "wot_droid_flat_mask.png")
    B.flatten_secondary(B.GENERAL, B.TS + "/secondary/droid779_snow", dpng)
    Bs = Sheet(dpng)
    b = Builder("SnowStone", sel_width=B.SEL_W)
    seen = set()
    B.add_unique(b, A, PrimaryBase(B.GENERAL, B.GRASS_ID).entries, seen, "bld")
    B.add_unique(b, Bs, PrimaryBase(B.GENERAL, B.SNOW_ID).entries, seen, "snow")
    b.pack(verbose=False)
    if (len(b.tiles), len(b.metas)) != (427, 240):
        raise SystemExit("replay produced %d tiles / %d metatiles, expected "
                         "427/240 -- the build has changed and the id mapping "
                         "can no longer be trusted" % (len(b.tiles), len(b.metas)))
    out = {}
    for i, item in enumerate(b.items):
        if item[0].startswith("snow["):
            r, c = item[0][5:-1].split(",")
            out[i] = int(r) * B.SEL_W + int(c)
    return out


def main(force=False):
    marker = os.path.join(SNOW, MARKER)
    if os.path.exists(marker) and not force:
        print("  masks already restored (%s says %s) -- nothing to do"
              % (MARKER, open(marker).read().strip()))
        return

    gim, gpx, gtw, gn = sheet(GENERAL)
    dim, dpx, dtw, dn = sheet(DROID)
    nim, npx, ntw, nn = sheet(SNOW)
    gp = load_pals(GENERAL)
    dpal = gp[:6] + load_pals(DROID)[6:]
    npal_all = load_pals(SNOW)
    npal = gp[:6] + npal_all[6:]

    nmb = bytearray(metas(SNOW))
    nab = bytearray(open(os.path.join(SNOW, "metatile_attributes.bin"), "rb").read())
    dmb = metas(DROID)
    dab = open(os.path.join(DROID, "metatile_attributes.bin"), "rb").read()
    gmb = metas(GENERAL)
    NM = len(nmb) // 16

    mapping = recover_mapping()
    targets = []
    for mid, src in sorted(mapping.items()):
        if all((v & 0x3FF) == 0 for v in ents(dmb, src)[4:]):
            continue                                  # flat in droid779 too
        if not all((v & 0x3FF) == 0 for v in ents(nmb, mid)[4:]):
            continue                                  # already two-layer here
        targets.append((mid, src))
    print("  metatiles to re-mask: %d" % len(targets))

    # ---- which tile slots may be written ---------------------------------
    keep = set()
    conv = {m for m, _ in targets}
    for m in range(NM):
        if m in conv:
            continue
        for v in ents(nmb, m):
            t = v & 0x3FF
            if t >= 512:
                keep.add(t - 512)
    spare = [t for t in range(512) if t not in keep]
    print("  tile slots referenced elsewhere: %d, spare for reuse: %d"
          % (len(keep), len(spare)))

    # ---- port a droid779 tile into SnowStone's palettes -------------------
    def droid_tile_px(tid):
        tp, tw, cnt, j = (gpx, gtw, gn, tid) if tid < 512 else (dpx, dtw, dn, tid - 512)
        if j >= cnt:
            return None
        ox, oy = (j % tw) * 8, (j // tw) * 8
        return [tp[ox + x, oy + y] & 15 for y in range(8) for x in range(8)]

    def port(tid, dslot):
        """Choose the SnowStone palette that fits best, and remap the indices.

        Index 0 stays 0: on the top layer that is the transparency the whole
        exercise is about.
        """
        px = droid_tile_px(tid)
        if px is None:
            return None
        want = sorted({i for i in px if i})
        if not want:
            return None
        best, bestcost = None, None
        for s in range(6, 13):
            cost, m = 0, {0: 0}
            for i in want:
                c = dpal[dslot][i]
                d, k = min((dE(c, npal[s][k]), k) for k in range(1, 16))
                cost += d
                m[i] = k
            if bestcost is None or cost < bestcost:
                best, bestcost = (s, m), cost
        s, m = best
        return tuple(m[i] for i in px), s, bestcost

    newtiles = {}          # (pixels, slot) -> tile id
    order = []
    worst = 0
    for mid, src in targets:
        for k in range(4, 8):
            v = ents(dmb, src)[k]
            tid, xf, yf, sl = v & 0x3FF, (v >> 10) & 1, (v >> 11) & 1, (v >> 12) & 0xF
            if not tid:
                continue
            r = port(tid, sl)
            if r is None:
                continue
            key = (r[0], r[1])
            worst = max(worst, r[2])
            if key not in newtiles:
                newtiles[key] = None
                order.append(key)
    print("  distinct new top-layer tiles: %d (worst per-tile colour error %d)"
          % (len(order), worst))
    if len(order) > len(spare):
        raise SystemExit("need %d tile slots, only %d spare" % (len(order), len(spare)))

    for key in order:
        newtiles[key] = spare.pop(0)

    # ---- rewrite the metatiles in place -----------------------------------
    base_ents = ents(gmb, SNOW_BASE_ID)[:4]        # General's snow, primary tiles
    for mid, src in targets:
        out = list(base_ents)                       # bottom layer
        for k in range(4, 8):
            v = ents(dmb, src)[k]
            tid, xf, yf, sl = v & 0x3FF, (v >> 10) & 1, (v >> 11) & 1, (v >> 12) & 0xF
            if not tid:
                out.append(0)
                continue
            r = port(tid, sl)
            if r is None:
                out.append(0)
                continue
            t = newtiles[(r[0], r[1])]
            out.append((r[1] << 12) | (yf << 11) | (xf << 10) | ((512 + t) & 0x3FF))
        for k, v in enumerate(out):
            struct.pack_into("<H", nmb, mid * 16 + k * 2, v)
        # take droid779's LAYER TYPE (bits 12-13) but keep Joe's behaviour bits
        old = struct.unpack_from("<H", nab, mid * 2)[0]
        srcattr = struct.unpack_from("<H", dab, src * 2)[0]
        struct.pack_into("<H", nab, mid * 2,
                         (old & ~(3 << 12)) | (srcattr & (3 << 12)))

    # ---- write the tiles ---------------------------------------------------
    maxslot = max([nn - 1] + list(newtiles.values()))
    rows = (maxslot + 16) // 16
    out = Image.new("P", (128, rows * 8), 0)
    out.putpalette(nim.getpalette())
    out.paste(nim, (0, 0))
    opx = out.load()
    for (pixels, slot), t in newtiles.items():
        ox, oy = (t % 16) * 8, (t // 16) * 8
        for y in range(8):
            for x in range(8):
                opx[ox + x, oy + y] = pixels[y * 8 + x]
    out.save(os.path.join(SNOW, "tiles.png"), bits=4)
    open(os.path.join(SNOW, "metatiles.bin"), "wb").write(nmb)
    open(os.path.join(SNOW, "metatile_attributes.bin"), "wb").write(nab)

    for junk in ("tiles.4bpp", "tiles.4bpp.lz"):
        p = os.path.join(SNOW, junk)
        if os.path.exists(p):
            os.remove(p)
    for i in range(16):
        p = os.path.join(SNOW, "palettes", "%02d.gbapal" % i)
        if os.path.exists(p):
            os.remove(p)

    open(marker, "w").write("%d snow metatiles re-masked, %d new tiles\n"
                            % (len(targets), len(order)))
    print("  gTileset_SnowStone now %d tile slots, %d metatiles (unchanged)"
          % ((maxslot // 16 + 1) * 16, NM))
    print("  wrote %s" % MARKER)


if __name__ == "__main__":
    main(force="--force" in sys.argv)
