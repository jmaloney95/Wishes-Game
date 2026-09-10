"""little_office_tileset_by_ekat99 -> data/tilesets/secondary/office.

A SECONDARY paired with gTileset_Building, which is how every interior in this
project is set up: 189 maps use that primary, and their floors always come from
the secondary, because the building primary holds only 30 metatiles and none of
them is a floor.

The sheet has no floor of its own -- the furniture is drawn on transparency --
so a floor is lifted out of gTileset_GenericBuilding's own art and rebuilt as
real tiles here. That is what makes this tileset usable on its own: a metatile
cannot reference tiles belonging to a different secondary, so borrowing the id
would not have worked.

Pure black is keyed out: it is the artist's backdrop behind the building
facade, and it is safe to key globally because it occurs ONLY at y 0-95 -- the
furniture draws its dark outlines in (88,88,112) / (64,72,104) instead.

The 13 cells the artist crossed out are handled separately, NOT by keying.
The X is drawn white on red, so keying the red leaves the white box behind and
those cells paint as blank white squares. They are detected by their red and
blanked whole instead.

Everything is laid out as grid-aligned terrain rather than hand-boxed objects.
For a furniture sheet that is the right shape: each 16x16 cell becomes one
paintable metatile in the same arrangement as the sheet, and the pieces get
assembled in porymap rather than here.
"""
import os
import struct
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from PIL import Image
from tilesetgen import Sheet, Builder, LAYER_COVERED, LAYER_NORMAL

RAW = "J:/ROM Hack Project/tilesets_raw/little_office_tileset_by_ekat99_dh993ok.png"
TS = "J:/ROM Hack Project/pokeemerald-expansion/data/tilesets"
OUT = TS + "/secondary/office"
# Regenerated on every run, so it goes to the system temp dir rather than
# leaving a build artefact sitting in the source tree.
SCRATCH = os.path.join(tempfile.gettempdir(), "wot_office_floor.png")

WOOD_FLOOR = 35      # gTileset_GenericBuilding local ids; map ids 547 and 544.
TAN_FLOOR = 32       # 547 is what House1 and Dewford Hall are floored with.


def _load_pals(d):
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


def render_metatiles(primary_dir, secondary_dir, ids, path):
    """Flatten metatiles of a built SECONDARY back into a plain RGBA strip, so
    their pixels can go through the normal Sheet -> Builder path.

    A secondary's metatiles address BOTH tilesets: tile ids below 512 are the
    primary's, with palettes 0-5; ids at or above 512 are the secondary's own,
    offset by 512, with palettes 6-12. Reading only one of the two sheets
    indexes off the end of it.
    """
    def sheet(d):
        im = Image.open(os.path.join(d, "tiles.png")).convert("P")
        return im.load(), im.width // 8, (im.width // 8) * (im.height // 8)

    ptp, ptw, pn = sheet(primary_dir)
    stp, stw, sn = sheet(secondary_dir)
    ppals, spals = _load_pals(primary_dir), _load_pals(secondary_dir)
    pals = ppals[:6] + spals[6:]

    mb = open(os.path.join(secondary_dir, "metatiles.bin"), "rb").read()
    out = Image.new("RGBA", (16 * len(ids), 16), (0, 0, 0, 0))
    op = out.load()
    for n, mid in enumerate(ids):
        base = mid * 16
        for layer in range(2):
            for q in range(4):
                off = base + (layer * 4 + q) * 2
                v = struct.unpack("<H", mb[off:off + 2])[0]
                tid, xf, yf, sl = v & 0x3FF, (v >> 10) & 1, (v >> 11) & 1, (v >> 12) & 0xF
                if tid < 512:
                    tp, tw, cnt, j = ptp, ptw, pn, tid
                else:
                    tp, tw, cnt, j = stp, stw, sn, tid - 512
                if j >= cnt:
                    continue
                ox, oy = (j % tw) * 8, (j // tw) * 8
                qx, qy = n * 16 + (q % 2) * 8, (q // 2) * 8
                for y in range(8):
                    for x in range(8):
                        idx = tp[ox + (7 - x if xf else x), oy + (7 - y if yf else y)]
                        if layer == 1 and idx == 0:
                            continue
                        op[qx + x, qy + y] = pals[sl][idx] + (255,)
    out.save(path)
    return path


render_metatiles(TS + "/primary/building", TS + "/secondary/generic_building",
                 [WOOD_FLOOR, TAN_FLOOR], SCRATCH)

F = Sheet(SCRATCH)
WOOD = F.metatile_px(0, 0)

# The source is drawn at 2x -- verified, all 22,528 of its 2x2 blocks are
# uniform -- so half=True recovers the intended 1x pixels losslessly.
S = Sheet(RAW, half=True, extra_keys=((0, 0, 0),))

# The X is drawn WHITE ON RED, so keying the red alone would leave a white
# box behind. Detect the marked cells instead and blank each one whole --
# self-maintaining if the sheet is ever re-exported with different cells
# marked, where a hardcoded list of 13 coordinates would quietly rot.
def blank_marked_cells(sheet, mark=(248, 0, 0)):
    from tilesetgen import snap
    px, W, H = sheet.px, sheet.w, sheet.h
    marked = set()
    for y in range(H):
        for x in range(W):
            q = px[x, y]
            if q[3] >= 128 and snap(q[:3]) == mark:
                marked.add((y // 16, x // 16))
    for (r, c) in marked:
        for y in range(r * 16, min(H, r * 16 + 16)):
            for x in range(c * 16, min(W, c * 16 + 16)):
                px[x, y] = (0, 0, 0, 0)
    return sorted(marked)


_marked = blank_marked_cells(S)
print("blanked %d marker cells:" % len(_marked), _marked)

b = Builder("Office", sel_width=8)

# Floors first, so the two most-painted metatiles are the first two ids.
b.terrain(F, 0, 0, 2, 1, label="floor", newline=True)

# The building facade. Its rows are LAYER_NORMAL so the player walks behind the
# roof and upper storey rather than over them.
b.terrain(S, 0, 0, 8, 6, label="facade", base=WOOD,
          attr_over=LAYER_NORMAL, newline=True)

# Desks, chairs, monitors, bookshelves, the water cooler and the stairs.
# LAYER_COVERED: the player stands in front of these.
b.terrain(S, 0, 96, 8, 5, label="office", base=WOOD,
          attr_over=LAYER_COVERED, newline=True)

b.pack()
b.write(OUT)
b.manifest()
