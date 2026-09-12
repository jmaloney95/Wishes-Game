"""Dojo (rebuilt at 1x) and snowy_cherries -> pokeemerald tilesets.

DOJO. The source is drawn at 2x -- 0.0% of its 2x2 pixel blocks disagree, as
clean a signal as the office sheet gave -- and the tileset in the ROM was built
from it full size, which is why it looks double in game. Halved it is 196 cells
and 269 tiles; at 2x it was jammed at 512/512 tiles, which was the tell.

THE EXISTING TILESET CANNOT SIMPLY BE OVERWRITTEN. LAYOUT_DOJO is a painted
41x41 map using 259 distinct metatiles from it, and halving changes what every
id points at, so the map would turn to noise. It cannot be converted either:
if the map had been stamped in 2x2 groups the blocks could be collapsed, but
only 1-2% of its aligned 2x2 blocks form a natural quad, so it was painted
freely, cell by cell.

So the old tileset is KEPT as gTileset_DojoOversized with LAYOUT_DOJO pointed
at it -- the map goes on rendering exactly as it does now -- and the corrected
build takes the gTileset_Dojo name. Repainting DOJO onto it is Joe's call;
nothing here forces it.

SNOWY CHERRIES. A tree sheet, not a grid: 341x394, no 16px alignment, 1541
colours and 254 distinct alpha values, so it is illustration-quality art rather
than GBA-ready tiles. Trees are found as connected components of opaque pixels
and each is snapped into its own cell block by Builder.obj(), which is what that
path is for. The soft alpha is hard-cut at 128, which is what the hardware
needs; tilesetgen's 5-bit snap and 15-colour palette packing handle the rest.

Both pair as SECONDARIES: the dojo with gTileset_General, which is what
LAYOUT_DOJO already uses, and the cherries with gTileset_SnowCinna, the snow
primary Munen Village and Frostwood Town run on.
"""
import math
import os
import sys
import tempfile
from collections import deque

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

from PIL import Image

from build_interiors import prepare, row_cost, RAW, TS
from tilesetgen import Sheet, Builder, PrimaryBase, LAYER_NORMAL, LAYER_COVERED

DOJO_SRC = "pokemon_halcyon___dojo_by_ekat99_dfbfw1s.png"
DOJO_GROUND = (2, 12)          # plain raked sand, post-halve cell coords
CHERRY_SRC = "snowy_cherries.png"
MIN_COMPONENT = 40             # px; below this it is a stray speck, not a tree


def build_dojo():
    S, red, blk = prepare(RAW + DOJO_SRC, half=True)
    print("== dojo ==  HALVED to %dx%d, %d rows; dropped %d red-X, %d black filler"
          % (S.w, S.h, S.h // 16, red, blk))
    ground = S.metatile_px(DOJO_GROUND[0] * 16, DOJO_GROUND[1] * 16)
    b = Builder("Dojo", sel_width=S.w // 16)
    for r in range(S.h // 16):
        if not row_cost(S, r)[0]:
            continue
        b.terrain(S, 0, r * 16, S.w // 16, 1, label="r%02d" % r,
                  base=ground, attr_over=LAYER_COVERED, newline=True)
    b.pack(verbose=False)
    b.write(TS + "/secondary/dojo")
    print("   gTileset_Dojo       %d/512 tiles  %d/512 metatiles" % (len(b.tiles), len(b.metas)))
    return b


def find_components(im, minpx=MIN_COMPONENT):
    """Bounding boxes of connected runs of opaque pixels, 8-connected."""
    px, W, H = im.load(), im.width, im.height
    seen = bytearray(W * H)
    out = []
    for sy in range(H):
        for sx in range(W):
            if seen[sy * W + sx] or px[sx, sy][3] < 128:
                continue
            q = deque([(sx, sy)])
            seen[sy * W + sx] = 1
            x0 = x1 = sx
            y0 = y1 = sy
            n = 0
            while q:
                x, y = q.popleft()
                n += 1
                x0, x1 = min(x0, x), max(x1, x)
                y0, y1 = min(y0, y), max(y1, y)
                for dx in (-1, 0, 1):
                    for dy in (-1, 0, 1):
                        nx, ny = x + dx, y + dy
                        if (0 <= nx < W and 0 <= ny < H and not seen[ny * W + nx]
                                and px[nx, ny][3] >= 128):
                            seen[ny * W + nx] = 1
                            q.append((nx, ny))
            if n >= minpx:
                out.append((x0, y0, x1 + 1, y1 + 1))
    out.sort(key=lambda c: (c[1] // 8, c[0]))       # reading order, row-banded
    return out


def quantise_per_tree(S, comps, ncolors=15):
    """Reduce each tree to <=15 colours ON ITS OWN, in place.

    THIS IS WHERE THE FIDELITY IS WON OR LOST. The source is illustration art:
    the median 16x16 cell wants 55 distinct colours and 245 of the 284 cells
    want more than 15, while a metatile may use exactly one 15-colour palette.
    Something has to give.

    Left to itself tilesetgen merges nearest colours GLOBALLY, across every tree
    at once, which is what made the first build look washed out -- a pink
    blossom and a brown trunk end up competing for the same slots. Median-cutting
    each tree separately picks the best 15 colours for THAT tree, so each keeps
    its own identity and nothing further is merged downstream.
    """
    from PIL import Image
    px = S.px
    for _, (x0, y0, x1, y1) in comps:
        w, h = x1 - x0, y1 - y0
        crop = Image.new("RGBA", (w, h), (0, 0, 0, 0))
        cp = crop.load()
        for y in range(h):
            for x in range(w):
                cp[x, y] = px[x0 + x, y0 + y]
        alpha = crop.split()[3]
        rgb = Image.new("RGB", (w, h), (0, 0, 0))
        rgb.paste(crop, mask=alpha)
        q = rgb.quantize(colors=ncolors, method=Image.MEDIANCUT, dither=Image.NONE).convert("RGB")
        qp, ap = q.load(), alpha.load()
        for y in range(h):
            for x in range(w):
                if ap[x, y] >= 128:
                    r, g, bl = qp[x, y]
                    px[x0 + x, y0 + y] = (r, g, bl, 255)


def place_trees(b, S, comps, base):
    for i, bb in comps:
        rows = math.ceil((bb[3] - bb[1]) / 16)
        cols = math.ceil((bb[2] - bb[0]) / 16)
        # A tree's canopy draws over the player and its trunk under him, so
        # everything above the bottom row is LAYER_NORMAL.
        b.obj(S, bb, base, label="t%02d" % i, newline=(cols > 6),
              attr=LAYER_COVERED, top_rows=max(0, rows - 1), top_attr=LAYER_NORMAL)


def build_cherries():
    """A PRIMARY + SECONDARY pair, so one map can use all 34 trees.

    851 tiles will not fit one tileset, and cleanup does not rescue it: a
    stricter alpha cutoff gets to 819 and quantising to 16 colours to 806,
    because 34 distinct canopies are 34 distinct shapes. A pair holds 1024, so
    the split is by tree type with the halves balanced by tile cost.

    The primary has to carry the snow ground itself -- a map on this pair no
    longer has gTileset_SnowCinna to draw it from -- so the same ground set the
    snow village primary uses is rebuilt here.
    """
    S = Sheet(RAW + CHERRY_SRC)                     # native 1x, real alpha
    comps = list(enumerate(find_components(S.im)))
    print("== snowy_cherries ==  %dx%d, %d tree components" % (S.w, S.h, len(comps)))

    quantise_per_tree(S, comps)

    from build_snow_village import flatten_primary, GROUND_IDS, SNOW_CINNA
    gpng = os.path.join(tempfile.gettempdir(), "wot_cherry_ground.png")
    _, grows = flatten_primary(SNOW_CINNA, GROUND_IDS, gpng, cols=12)
    G = Sheet(gpng)

    # The base is snow_cinna metatile 0 -- the SAME ground Frostwood Town is
    # painted with (360 uses, its most-painted tile) -- so a tree sits on it
    # seamlessly in game.
    #
    # It does look like a blue-and-white checkerboard behind 34 trees in
    # porymap's selector, and that is worth understanding rather than
    # "fixing": the sparkle IS snow_cinna's snow texture. Nothing in that
    # primary is flatter -- the most uniform pale tile in all 384 of them is
    # 77% single-colour, and metatile 0 is already 75%. Baking something
    # flatter would trade a busy selector for tree-shaped flat patches on
    # textured snow in the actual map, which is the worse of the two.
    ground = G.metatile_px(0, 0)

    # Balance the two halves by tile cost rather than by count: the tall trees
    # are worth several of the small bushes.
    def tcost(bb):
        cols = math.ceil((bb[2]-bb[0])/16); rows = math.ceil((bb[3]-bb[1])/16)
        return cols * rows
    total = sum(tcost(bb) for _, bb in comps)
    half, acc, cut = total / 2.0, 0, len(comps)
    for k, (_, bb) in enumerate(comps):
        acc += tcost(bb)
        if acc >= half:
            cut = k + 1
            break
    first, second = comps[:cut], comps[cut:]

    p = Builder("SnowyCherries", sel_width=12, primary=True)
    p.terrain(G, 0, 0, 12, grows, label="snow", newline=True)
    place_trees(p, S, first, ground)
    p.pack(verbose=False)
    p.write(TS + "/primary/snowy_cherries")
    print("   gTileset_SnowyCherries       PRIMARY   %d/512 tiles  %d/512 metatiles  (%d trees + ground)"
          % (len(p.tiles), len(p.metas), len(first)))

    snow = PrimaryBase(TS + "/primary/snowy_cherries", 0).entries
    b = Builder("SnowyCherries2", sel_width=12)
    place_trees(b, S, second, snow)
    b.pack(verbose=False)
    b.write(TS + "/secondary/snowy_cherries_2")
    print("   gTileset_SnowyCherries2      secondary %d/512 tiles  %d/512 metatiles  (%d trees)"
          % (len(b.tiles), len(b.metas), len(second)))
    return p, b


if __name__ == "__main__":
    build_dojo()
    print()
    build_cherries()
