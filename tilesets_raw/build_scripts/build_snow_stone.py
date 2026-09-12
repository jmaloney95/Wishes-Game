"""gTileset_SnowAndStone + gTileset_Droid779Snow -> one gTileset_SnowStone.

WHY THEY MERGE WELL. snow_and_stone is nothing but stone BUILDINGS drawn on
transparency -- there is not one ground tile on the sheet. droid779_snow is the
snow scenery: banks, ice, snow-laden trees, drifts. They were always two halves
of one set, and a map can only have one secondary.

THE CHECKERBOARD. snow_and_stone's source marks "empty" with a 16x16
checkerboard of (224,200,192) and (248,224,240), and the tileset in the ROM has
it BAKED IN -- that is the checkered background and the residue around every
building. Here it is keyed by colour, which is safe and was verified rather
than assumed: neither colour appears once anywhere off its own checker phase,
so neither is ever used as art. (The snow village sheet needed a phase + flood
test because its checker used pure white, which was also its snow. Check first;
the cheap key is not always available.)

WHY IT FITS. The old snow_and_stone sits at 512/512 metatiles, which looks like
a full tileset but is not: 237 of its 703 source cells are pure background and
297 of the remaining 466 are DUPLICATES of another cell. Dropping the
background and de-duplicating leaves 169 real metatiles, so the merge lands at
241/512 metatiles and 391/512 tiles with room to spare.

BASES. Both halves are objects that need ground baked underneath, and the maps
that use them sit on different ground, so each half gets its own:
  * the buildings on gTileset_General metatile 1, plain grass, which is what
    MUNEN_VILLAGE_2 -- the map that uses these buildings -- is painted on;
  * the snow scenery on General metatile 94, snow, which WALNUT_WOODS paints
    324 times and MELTING_MILE 35.

THE OLD TILESETS ARE LEFT IN PLACE. Four painted maps depend on them --
MUNEN_VILLAGE_2 (1421 blocks, 166 of its metatiles), MUNEN_VILLAGE_ACT2 (163),
WALNUT_WOODS (51) and MELTING_MILE (27) -- and merging renumbers everything, so
switching a map over means repainting it. Nothing here forces that.
"""
import os
import struct
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

from PIL import Image

from tilesetgen import (Sheet, Builder, PrimaryBase, canon, colors, snap,
                        LAYER_NORMAL, LAYER_COVERED)

RAW = "J:/ROM Hack Project/tilesets_raw/"
TS = "J:/ROM Hack Project/pokeemerald-expansion/data/tilesets"
GENERAL = TS + "/primary/general"

SAS_SRC = RAW + "snow_and_stone_by_magiscarf_dcjgz10.png"
CHECKER = ((224, 200, 192), (248, 224, 240))
SEL_W = 12

GRASS_ID = 1      # gTileset_General plain grass -- MUNEN_VILLAGE_2's ground
SNOW_ID = 94      # gTileset_General snow -- WALNUT_WOODS paints it 324 times


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


def flatten_secondary(primary_dir, secondary_dir, path, cols=SEL_W):
    """Flatten a built SECONDARY's metatiles into an RGBA grid.

    A secondary's metatiles address BOTH tilesets -- ids under 512 are the
    primary's with palettes 0-5, ids at or above are its own offset by 512 with
    palettes 6-12 -- so both sheets have to be read. Index 0 of a top-layer tile
    means transparent, and that transparency is preserved here so the merged
    build can put its own ground underneath.
    """
    def sheet(d):
        im = Image.open(os.path.join(d, "tiles.png")).convert("P")
        return im.load(), im.width // 8, (im.width // 8) * (im.height // 8)

    ptp, ptw, pn = sheet(primary_dir)
    stp, stw, sn = sheet(secondary_dir)
    pals = load_pals(primary_dir)[:6] + load_pals(secondary_dir)[6:]
    mb = open(os.path.join(secondary_dir, "metatiles.bin"), "rb").read()
    nm = len(mb) // 16
    rows = (nm + cols - 1) // cols
    out = Image.new("RGBA", (cols * 16, rows * 16), (0, 0, 0, 0))
    op = out.load()

    for m in range(nm):
        gx, gy = (m % cols) * 16, (m // cols) * 16
        for layer in range(2):
            for q in range(4):
                off = m * 16 + (layer * 4 + q) * 2
                v = struct.unpack("<H", mb[off:off + 2])[0]
                tid, xf, yf, sl = v & 0x3FF, (v >> 10) & 1, (v >> 11) & 1, (v >> 12) & 0xF
                if tid < 512:
                    tp, tw, cnt, j = ptp, ptw, pn, tid
                else:
                    tp, tw, cnt, j = stp, stw, sn, tid - 512
                if j >= cnt:
                    continue
                ox, oy = (j % tw) * 8, (j // tw) * 8
                qx, qy = (q % 2) * 8, (q // 2) * 8
                for y in range(8):
                    for x in range(8):
                        idx = tp[ox + (7 - x if xf else x), oy + (7 - y if yf else y)]
                        if idx == 0:
                            continue          # transparent on either layer
                        op[gx + qx + x, gy + qy + y] = pals[sl][idx] + (255,)
    out.save(path)
    return rows


def key_checker(path):
    """Key snow_and_stone's checkerboard by colour, straight out."""
    im = Image.open(path).convert("RGBA")
    px, W, H = im.load(), im.width, im.height
    n = 0
    for y in range(H):
        for x in range(W):
            if snap(px[x, y][:3]) in CHECKER:
                px[x, y] = (0, 0, 0, 0)
                n += 1
    out = os.path.join(tempfile.gettempdir(), "wot_sas_keyed.png")
    im.save(out)
    return out, n


def add_unique(b, S, base, seen, label):
    """Place every cell that carries art and is not already in the tileset.

    De-duplication is what makes the merge fit: 297 of snow_and_stone's 466
    content cells repeat another cell exactly.
    """
    placed = skipped = 0
    for r in range(S.h // 16):
        for c in range(S.w // 16):
            sub = S.metatile_px(c * 16, r * 16)
            if not any(colors(t) for t in sub):
                continue
            key = tuple(canon(t) for t in sub)
            if key in seen:
                skipped += 1
                continue
            seen.add(key)
            opaque = all((-1, -1, -1) not in t for t in sub)
            if opaque:
                b._put(*b._alloc(1, 1), ("%s[%d,%d]" % (label, r, c), sub, None, 0))
            else:
                b._put(*b._alloc(1, 1),
                       ("%s[%d,%d]" % (label, r, c), list(base), sub, LAYER_COVERED))
            placed += 1
    return placed, skipped


def main():
    keyed, nkeyed = key_checker(SAS_SRC)
    A = Sheet(keyed)
    print("snow_and_stone: keyed %d checkerboard pixels (%.0f%% of the sheet)"
          % (nkeyed, 100.0 * nkeyed / (A.w * A.h)))

    dpng = os.path.join(tempfile.gettempdir(), "wot_droid_flat.png")
    rows = flatten_secondary(GENERAL, TS + "/secondary/droid779_snow", dpng)
    B = Sheet(dpng)
    print("droid779_snow: flattened %d metatiles to a %dx%d grid" % (rows * SEL_W, B.w, B.h))

    grass = PrimaryBase(GENERAL, GRASS_ID).entries
    snow = PrimaryBase(GENERAL, SNOW_ID).entries

    b = Builder("SnowStone", sel_width=SEL_W)
    seen = set()
    p1, s1 = add_unique(b, A, grass, seen, "bld")
    print("   buildings : %3d placed, %3d duplicates skipped" % (p1, s1))
    p2, s2 = add_unique(b, B, snow, seen, "snow")
    print("   snow      : %3d placed, %3d duplicates skipped" % (p2, s2))

    b.pack(verbose=False)
    b.write(TS + "/secondary/snow_stone")
    print("   gTileset_SnowStone  %d/512 tiles  %d/512 metatiles"
          % (len(b.tiles), len(b.metas)))

    # Sootopolis's snowy crater wall is appended on top of the base build.
    # It has to run HERE rather than standalone: it edits the files write()
    # just produced, so a rebuild that skipped it would silently drop the
    # ridge. build_ridge re-stamps the guard afterwards.
    from build_ridge import add_ridge
    add_ridge(force=True)   # the base was just regenerated
    return b


if __name__ == "__main__":
    main()
