"""snow_village_tiles_by_carchagui -> a PRIMARY + a SECONDARY that pair together.

  gTileset_SnowVillageHomes   PRIMARY    snow ground, three chalets, fir, lamp,
                                         bridge
  gTileset_SnowVillage        SECONDARY  Pokemon Center, Shop

A map takes one primary and one secondary, so making the homes set the primary
is what lets both halves of this sheet appear on the same map.

THE PRIMARY HAS TO CARRY THE GROUND. A map on this pair no longer has
gTileset_SnowCinna to draw snow from, and the sheet has no ground art of its
own, so the ground is lifted out of snow_cinna and rebuilt here as real tiles.
The set is chosen by what the existing snow maps actually paint -- Munen
Village and Frostwood Town use 154 distinct primary metatiles between them, and
the ones carried over are the most-used of those, ground first.

A primary gets SIX palettes, not seven, and its tile ids start at 0 rather than
512; Builder(primary=True) handles both.

Why the sheet needed splitting at all: all eight objects come to 755 tiles
against a budget of 512, and the excess is shape, not colour -- median-cutting
the source from 511 colours to 48 moved it by five tiles. Metatiles were a
separate, fixable problem: shelf padding, cured by packing two objects per
shelf rather than one.

The sheet is NATIVE 1x (8,857 non-uniform 2x2 blocks), and its "empty" marker
is a 16x16 checkerboard of (224,216,208) and (248,248,248) with no alpha --
see snow_prep.strip(), which cannot key either colour outright because
(248,248,248) is also the snow.
"""
import math
import os
import struct
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

from PIL import Image

from tilesetgen import Sheet, Builder, PrimaryBase, LAYER_NORMAL, LAYER_COVERED
from snow_prep import strip

TS = "J:/ROM Hack Project/pokeemerald-expansion/data/tilesets"
SNOW_CINNA = TS + "/primary/snow_cinna"
KEYED = os.path.join(tempfile.gettempdir(), "wot_snow_village_keyed.png")
GROUND_PNG = os.path.join(tempfile.gettempdir(), "wot_snow_ground.png")

SEL_W = 12                      # the Pokemon Center is 10 cells across

# snow_cinna metatiles to carry over, most-painted first. 0/9/1/8 are the plain
# snow that is half of everything those maps are made of; the rest are edges,
# paths and cliff pieces.
GROUND_IDS = [0, 9, 1, 8, 43, 58, 51, 42, 35, 50, 34, 33,
              297, 298, 299, 300, 301, 305, 306, 307, 308, 309, 312, 313,
              314, 315, 320, 321, 322, 323]


def load_pals(d):
    pals = []
    for i in range(16):
        p = os.path.join(d, "palettes", "%02d.pal" % i)
        if not os.path.exists(p):
            pals.append([(0, 0, 0)] * 16)
            continue
        cols = []
        for ln in open(p).read().splitlines()[3:]:
            ln = ln.strip()
            if ln:
                cols.append(tuple(int(x) for x in ln.split()))
        cols += [(0, 0, 0)] * (16 - len(cols))
        pals.append(cols[:16])
    return pals


def flatten_primary(d, ids, path, cols=SEL_W):
    """Flatten metatiles of a built PRIMARY into an RGBA grid, so their pixels
    can go back through the normal Sheet -> Builder path. Primary metatiles only
    ever address the primary's own tiles and palettes 0-5, so unlike a
    secondary there is just one sheet to read."""
    im = Image.open(os.path.join(d, "tiles.png")).convert("P")
    tp, tw = im.load(), im.width // 8
    ntiles = (im.width // 8) * (im.height // 8)
    pals = load_pals(d)
    mb = open(os.path.join(d, "metatiles.bin"), "rb").read()

    rows = math.ceil(len(ids) / cols)
    out = Image.new("RGBA", (cols * 16, rows * 16), (0, 0, 0, 0))
    op = out.load()
    for n, mid in enumerate(ids):
        assert (mid + 1) * 16 <= len(mb), "metatile %d out of range" % mid
        base = mid * 16
        gx, gy = (n % cols) * 16, (n // cols) * 16
        for layer in range(2):
            for q in range(4):
                off = base + (layer * 4 + q) * 2
                v = struct.unpack("<H", mb[off:off + 2])[0]
                tid, xf, yf, sl = v & 0x3FF, (v >> 10) & 1, (v >> 11) & 1, (v >> 12) & 0xF
                if tid >= ntiles:
                    continue
                ox, oy = (tid % tw) * 8, (tid // tw) * 8
                qx, qy = (q % 2) * 8, (q // 2) * 8
                for y in range(8):
                    for x in range(8):
                        idx = tp[ox + (7 - x if xf else x), oy + (7 - y if yf else y)]
                        if layer == 1 and idx == 0:
                            continue
                        op[gx + qx + x, gy + qy + y] = pals[sl][idx] + (255,)
    out.save(path)
    return out.size, rows


REGIONS = {
    "house_a":    (0,   0,   128, 176),
    "house_b":    (144, 0,   256, 176),
    "house_c":    (272, 0,   400, 176),
    "pokecenter": (0,   176, 176, 304),
    "shop":       (176, 176, 320, 304),
    "fir":        (320, 176, 400, 304),
    "lamp":       (232, 304, 296, 384),
    "bridge":     (296, 304, 400, 384),
}


def place(b, sheet, base, shelves):
    for shelf in shelves:
        for i, obj in enumerate(shelf):
            bb = sheet.bbox(*REGIONS[obj])
            assert bb is not None, "%s: nothing found in %s" % (obj, REGIONS[obj])
            rows = math.ceil((bb[3] - bb[1]) / 16)
            # The bridge is walked ON, so it stays under the player the whole
            # way up; everything else puts its upper rows over him so he passes
            # behind roofs and canopies.
            b.obj(sheet, bb, base, label=obj, newline=(i == 0),
                  attr=LAYER_COVERED,
                  top_rows=0 if obj == "bridge" else rows - 1,
                  top_attr=LAYER_NORMAL)


def main():
    strip(out=KEYED)
    S = Sheet(KEYED)

    # ── primary ────────────────────────────────────────────────────────────────
    size, grows = flatten_primary(SNOW_CINNA, GROUND_IDS, GROUND_PNG)
    print("ground strip %s = %d metatiles over %d rows\n" % (size, len(GROUND_IDS), grows))
    G = Sheet(GROUND_PNG)

    print("== gTileset_SnowVillageHomes (PRIMARY) ==")
    p = Builder("SnowVillageHomes", sel_width=SEL_W, primary=True)
    p.terrain(G, 0, 0, SEL_W, grows, label="snow", newline=True)
    GROUND = G.metatile_px(0, 0)          # metatile 0: the plain snow everything sits on
    place(p, S, GROUND, [("house_a", "fir"), ("house_b", "bridge"), ("house_c", "lamp")])
    p.pack()
    p.write(TS + "/primary/snow_village_homes")
    p.manifest()
    print()

    # ── secondary ──────────────────────────────────────────────────────────────
    print("== gTileset_SnowVillage (SECONDARY) ==")
    # Base comes from the primary just written, by id, which is what a secondary is
    # allowed to reference. Ground metatile 0 is the first thing placed above.
    SNOW = PrimaryBase(TS + "/primary/snow_village_homes", 0).entries
    s = Builder("SnowVillage", sel_width=SEL_W)
    place(s, S, SNOW, [("pokecenter",), ("shop",)])
    s.pack()
    s.write(TS + "/secondary/snow_village")
    s.manifest()


# Guarded so importing this module for flatten_primary/GROUND_IDS does not
# re-run the whole snow village build as a side effect.
if __name__ == "__main__":
    main()
