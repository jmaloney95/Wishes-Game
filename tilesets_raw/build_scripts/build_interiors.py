"""Five Frostwood/Pompeii interior sheets -> pokeemerald tilesets.

WHAT THESE SHEETS COST. Measured before building anything, as unique 8x8 tiles
after flip-dedup:

    Frostwood-PC                  236 metatiles    297 tiles
    Deoxys-lab                    189 metatiles    332 tiles
    Frostwood-bar                 276 metatiles    529 tiles
    Japanese-interior-Pompeii     662 metatiles   1055 tiles
    Frostwood-Cabin               609 metatiles   1403 tiles

The budget is 512 tiles and 512 metatiles per tileset, so the two small ones
are single secondaries and the rest have to be split. That is arithmetic, not
preference: 3,616 unique tiles cannot go in fewer than eight tilesets.

Colour reduction does NOT help -- median-cutting the cabin from 369 colours to
32 moved it from 1403 tiles to 1344. The tiles differ in SHAPE. (Same result as
the snow village sheet; worth stopping to check, not worth trying twice.)

HOW THE SPLIT WORKS. Rows are packed greedily into tilesets, and the FIRST
tileset of each sheet is built as a PRIMARY. That matters: a map takes one
primary and one secondary, so a sheet that needs two tilesets can have both on
the same map only if one of them is the primary. Rows 0-3 are the floor and
wall panels on every one of these sheets, so they land in the primary, which is
also where the base fill for the object cells comes from.

Sheets that fit in one tileset are plain SECONDARIES on gTileset_Building,
which is how every other interior in this project is set up.

Two per-sheet quirks, both detected rather than assumed:
  * Frostwood-PC marks 14 cells with a red X, drawn white on red exactly like
    the office sheet. Keying the red alone would leave white boxes, so cells
    containing pure red are blanked whole.
  * Every sheet has fully-black filler cells (1 to 12 of them). A solid black
    metatile is not worth a slot, so cells that are >=94% pure black are
    dropped. Black used as outline art is untouched, because those cells are
    nowhere near that threshold.
"""
import math
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

from tilesetgen import (Sheet, Builder, PrimaryBase, canon, colors, snap,
                        LAYER_NORMAL, LAYER_COVERED)

RAW = "J:/ROM Hack Project/tilesets_raw/"
TS = "J:/ROM Hack Project/pokeemerald-expansion/data/tilesets"

MAX_TILES = 512
MAX_METAS = 512
SEL_W = 16                     # these sheets are 16 or 8 cells wide

# WHICH SHEETS ARE DRAWN AT 2x, measured as the share of 2x2 pixel blocks whose
# four pixels disagree. Genuine 1x pixel art disagrees inside most blocks; 2x art
# only disagrees where it was retouched after scaling.
#
#   Frostwood-bar        3.1%  -> 2x
#   Frostwood-Cabin      1.8%  -> 2x
#   Deoxys-lab           0.7%  -> 2x
#   Pompeii              5.9%  -> 2x
#   Frostwood-PC        56.4%  -> native 1x
#
# TESTING FOR EXACTLY ZERO IS WRONG and is what shipped these four at double
# size the first time. A handful of touched-up pixels is normal in scaled art;
# that is precisely the case Sheet(half=True)'s majority-vote downsample exists
# to handle. Test the RATIO, and test the even sub-region so an odd height does
# not silently skip the check altogether (Frostwood-Cabin is 895 tall, and the
# first pass read "1x" off a test that never ran).

# name, file, C name stem, drawn at 2x?
SHEETS = [
    ("frostwood_pc",    "Frostwood-PC.png",              "FrostwoodPC",    False),
    ("deoxys_lab",      "Deoxys-lab.png",                "DeoxysLab",      True),
    ("frostwood_bar",   "Frostwood-bar.png",             "FrostwoodBar",   True),
    ("pompeii_house",   "Japanese-interior-Pompeii.png", "PompeiiHouse",   True),
    ("frostwood_cabin", "Frostwood-Cabin.png",           "FrostwoodCabin", True),
]


def prepare(path, half=False):
    """Load a sheet and blank the cells the artist marked as unusable.

    Red X markers are drawn WHITE ON RED, so keying the red alone leaves a
    white box behind; the whole cell goes instead. Fully-black cells are the
    artist's filler and would otherwise each eat a metatile slot.
    """
    S = Sheet(path, half=half)
    px, W, H = S.px, S.w, S.h
    dropped_red = dropped_black = 0
    for r in range(H // 16):
        for c in range(W // 16):
            red = blk = 0
            for y in range(r * 16, min(H, r * 16 + 16)):
                for x in range(c * 16, min(W, c * 16 + 16)):
                    p = px[x, y]
                    if p[3] < 128:
                        continue
                    s = snap(p[:3])
                    if s == (248, 0, 0):
                        red += 1
                    elif s == (0, 0, 0):
                        blk += 1
            if red or blk >= 240:
                for y in range(r * 16, min(H, r * 16 + 16)):
                    for x in range(c * 16, min(W, c * 16 + 16)):
                        px[x, y] = (0, 0, 0, 0)
                if red:
                    dropped_red += 1
                else:
                    dropped_black += 1
    return S, dropped_red, dropped_black


# Sheets whose floor the repetition heuristic gets wrong, checked by eye.
# Both of these lead with WALL panels rather than floor, so the most-repeated
# cell is a wall. (col, row) of a real floor cell instead.
# The floor each sheet's furniture is composited over, as (col, row) in the cell
# grid AFTER any halving.
#
# ALL FIVE ARE NAMED, and the repetition heuristic below is only a fallback.
# It was wrong on three of these five: it took the lab's wall trim, the bar's
# booth upholstery and the cabin's log wall, because these sheets lead with wall
# panels and a wall repeats just as happily as a floor does. A five-line table
# that was checked by eye beats a rule that is right most of the time.
FLOOR_OVERRIDE = {
    "Frostwood-PC.png":              (4, 0),   # wood planks (also a herringbone at 4,1)
    "Deoxys-lab.png":                (5, 1),   # plain lab grey
    "Frostwood-bar.png":             (0, 1),   # diagonal wood boards
    "Japanese-interior-Pompeii.png": (2, 4),   # tatami -- the cream at 0,2 is plaster wall
    "Frostwood-Cabin.png":           (6, 4),   # mid-brown planks; rows 0-1 are log wall
}


def find_floor(S, filename=None):
    """The sheet's own floor: the fully-opaque cell that repeats most often.

    Floors are drawn as a repeating field and furniture is not, so repetition
    identifies one on three of these five sheets. The other two lead with wall
    panels, and are named in FLOOR_OVERRIDE rather than having the heuristic
    bent until it happens to agree. Run AFTER prepare(), or the black filler
    cells win.
    """
    if filename in FLOOR_OVERRIDE:
        c, r = FLOOR_OVERRIDE[filename]
        return S.metatile_px(c * 16, r * 16), (c, r, -1)
    reps, where = {}, {}
    for r in range(S.h // 16):
        for c in range(S.w // 16):
            sub = S.metatile_px(c * 16, r * 16)
            if not all(t and (-1, -1, -1) not in t for t in sub):
                continue                                  # not fully opaque
            cs = set()
            for t in sub:
                cs |= colors(t)
            if max((sum(x) for x in cs), default=0) < 90:
                continue                                  # near-black: filler
            key = tuple(canon(t) for t in sub)
            reps[key] = reps.get(key, 0) + 1
            where.setdefault(key, (c, r))
    if not reps:
        return None, None
    best = max(reps, key=lambda k: reps[k])
    c, r = where[best]
    return S.metatile_px(c * 16, r * 16), (c, r, reps[best])


def row_cost(S, r):
    """Cells and unique tiles in one metatile row."""
    cells, tiles = 0, set()
    for c in range(S.w // 16):
        sub = S.metatile_px(c * 16, r * 16)
        if not any(colors(t) for t in sub):
            continue
        cells += 1
        for t in sub:
            if colors(t):
                tiles.add(canon(t))
    return cells, tiles


def plan_split(S):
    """Greedily pack whole metatile rows into tilesets that fit the budget.

    Rows are kept together and in order so the result still reads like the
    original sheet in porymap's selector, which is what makes it paintable.
    A tile budget of 460 rather than 512 leaves room for the inflation that
    palette assignment adds -- a tile used under two palettes is stored twice.
    """
    groups, cur, cur_tiles, cur_cells = [], [], set(), 0
    for r in range(S.h // 16):
        cells, tiles = row_cost(S, r)
        if cells == 0:
            continue
        if cur and (len(cur_tiles | tiles) > 460 or cur_cells + cells > MAX_METAS - 40):
            groups.append(cur)
            cur, cur_tiles, cur_cells = [], set(), 0
        cur.append(r)
        cur_tiles |= tiles
        cur_cells += cells
    if cur:
        groups.append(cur)
    if len(groups) < 2:
        return groups

    # Greedy filling leaves a stub at the end (the bar split 460/88). Re-split
    # into the same NUMBER of groups, balanced by tile cost, so every tileset
    # keeps headroom for later edits.
    n = len(groups)
    rows = [r for g in groups for r in g]
    costs = {r: row_cost(S, r) for r in rows}
    target = sum(len(costs[r][1]) for r in rows) / float(n)
    out, cur, acc = [], [], 0.0
    for i, r in enumerate(rows):
        cur.append(r)
        acc += len(costs[r][1])
        remaining = len(rows) - i - 1
        if len(out) < n - 1 and acc >= target and remaining >= (n - len(out) - 1):
            out.append(cur); cur, acc = [], 0.0
    if cur:
        out.append(cur)
    # only accept the rebalance if every group still fits
    for g in out:
        tiles, cells = set(), 0
        for r in g:
            cc, tt = costs[r]
            cells += cc; tiles |= tt
        if len(tiles) > 460 or cells > MAX_METAS - 40:
            return groups
    return out


def build_group(stem, dirname, S, rows, base, primary):
    # The selector is exactly as wide as the sheet. A shelf is padded out to
    # sel_width whether it needs it or not, so a fixed 16 turned the 8-wide
    # Frostwood-PC sheet into 480 metatiles for 240 cells of art.
    b = Builder(stem, sel_width=S.w // 16, primary=primary)
    for r in rows:
        # One metatile row at a time, so a gap in the sheet does not shift
        # everything after it out of alignment with the original layout.
        b.terrain(S, 0, r * 16, S.w // 16, 1, label="r%02d" % r,
                  base=base, attr_over=LAYER_COVERED, newline=True)
    b.pack(verbose=False)
    out = "%s/%s/%s" % (TS, "primary" if primary else "secondary", dirname)
    b.write(out)
    return b, out


def process(dirname, filename, stem, half):
    S, red, blk = prepare(RAW + filename, half=half)
    floor, at = find_floor(S, filename)
    assert floor is not None, "%s: no floor cell found" % filename

    # Try the whole sheet as ONE tileset first and only split if packing
    # actually overflows. plan_split works off an estimate of the tile cost,
    # which is necessarily conservative; pack() knows the real number after
    # palette assignment. Guessing cost the cabin an unnecessary second
    # tileset at 478/512 -- comfortably inside the budget.
    allrows = [r for r in range(S.h // 16) if row_cost(S, r)[0]]
    try:
        Builder(stem, sel_width=S.w // 16).__class__      # cheap sanity
        probe = Builder(stem, sel_width=S.w // 16)
        for r in allrows:
            probe.terrain(S, 0, r * 16, S.w // 16, 1, label="r%02d" % r,
                          base=floor, attr_over=LAYER_COVERED, newline=True)
        probe.pack(verbose=False)
        groups = [allrows]
    except AssertionError:
        groups = plan_split(S)
    print("== %s ==  %s%dx%d, %d metatile rows; dropped %d red-X and %d black filler"
          % (filename, "HALVED to " if half else "", S.w, S.h, S.h // 16, red, blk))
    print("   floor from cell (col %d, row %d)%s"
          % (at[0], at[1], " [named override]" if at[2] < 0 else ", repeats %dx" % at[2]))
    print("   splits into %d tileset(s): %s"
          % (len(groups), ", ".join("rows %d-%d" % (g[0], g[-1]) for g in groups)))

    made = []
    for i, rows in enumerate(groups):
        # First group is the PRIMARY when the sheet needs more than one, so both
        # halves can sit on the same map. A sheet that fits in one tileset is a
        # plain secondary on gTileset_Building, like every other interior here.
        primary = (len(groups) > 1 and i == 0)
        name = dirname if i == 0 else "%s_%d" % (dirname, i + 1)
        cname = stem if i == 0 else "%s%d" % (stem, i + 1)
        # Furniture cells are drawn on transparency, so they need a floor baked
        # underneath or they would composite over the backdrop and draw a black
        # halo. The sheet's own floor is used, not gTileset_Building's black
        # metatile 1, which is what the first pass got wrong.
        base = floor
        b, out = build_group(cname, name, S, rows, base, primary)
        made.append((cname, out, primary, len(b.tiles), len(b.metas)))
        print("   %-18s %-9s %3d/512 tiles  %3d/512 metatiles"
              % (cname, "PRIMARY" if primary else "secondary",
                 len(b.tiles), len(b.metas)))
    print()
    return made


if __name__ == "__main__":
    allmade = []
    for dirname, filename, stem, half in SHEETS:
        allmade += process(dirname, filename, stem, half)
    print("built %d tilesets" % len(allmade))
    for cname, out, primary, t, m in allmade:
        print("  gTileset_%-18s %-9s %s" % (cname, "PRIMARY" if primary else "secondary", out))
