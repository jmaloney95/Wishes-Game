#!/usr/bin/env python3
"""
Wishes of Tomorrow -- cut Siren Allison's overworld sheets out of the artist's
sprite sheet.

Source: custom sprites/Trainer Sprites/allison.png

That sheet is one image holding everything: a tall battle sprite on the left,
and on the right an 8x4 grid of 32x48 cells. The grid's LEFT four columns are
her swimming (head and shoulders above the water), the RIGHT four are her
walking; the four rows are down / left / right / up, and each group of four
columns is a stand-step-stand-step cycle, so the two step poses are columns
1 and 3 of the group.

pokeemerald wants nine frames in a fixed order, and the right-facing row is not
needed -- the engine mirrors the left-facing frames.

THE SHEET IS DRAWN AT 2x AND MUST BE HALVED. Verified rather than assumed:
on the (0,0) phase, 98.9%% of its 2x2 blocks are a single flat colour, so it is
genuinely double-scale art and halving it is all but lossless. Emitted at full
size she towered over every other NPC on the route. Halved, a 32x48 cell
becomes 16x24 and her walking figure becomes 16x19, which sits comfortably in
the standard 16x32 NPC frame -- the same size as the player.

Blocks that are NOT uniform (the artist's own anti-aliasing) are resolved by
majority vote of the four pixels rather than by sampling one corner, which
keeps her outline from breaking up.

Each halved cell is placed into its frame at a FIXED offset rather than being
individually centred, so the artist's own frame-to-frame bob is preserved
instead of being flattened.

Run from the repo root:  python3 tools/build_allison_sprites.py
"""

import os
import sys

from PIL import Image

SRC = os.path.join("..", "custom sprites", "Trainer Sprites", "allison.png")

GRID_X, CELL_W, CELL_H = 160, 32, 48    # source cell, at 2x
FRAME_W, FRAME_H = 16, 32               # the standard NPC sprite, after halving
HALF_W, HALF_H = CELL_W // 2, CELL_H // 2
# Bottom-align the cell in the frame, matching how the other overworld sheets
# in this repo are authored (content sits on the tile).
OFF_X, OFF_Y = (FRAME_W - HALF_W) // 2, FRAME_H - HALF_H

ROW_DOWN, ROW_LEFT, ROW_UP = 0, 1, 3
STAND, STEP_A, STEP_B = 0, 1, 3

# pokeemerald's nine-frame order.
FRAMES = [
    (ROW_DOWN, STAND),   # 0 face down
    (ROW_UP,   STAND),   # 1 face up
    (ROW_LEFT, STAND),   # 2 face left
    (ROW_DOWN, STEP_A),  # 3 walk down
    (ROW_DOWN, STEP_B),  # 4
    (ROW_UP,   STEP_A),  # 5 walk up
    (ROW_UP,   STEP_B),  # 6
    (ROW_LEFT, STEP_A),  # 7 walk left
    (ROW_LEFT, STEP_B),  # 8
]

SHEETS = [
    ("swimmer_allison", 0),   # swimming: the left four columns
    ("wot_allison_land", 4),  # walking:  the right four columns
]

OUT_PIC = os.path.join("graphics", "object_events", "pics", "people")
OUT_PAL = os.path.join("graphics", "object_events", "palettes")

# The sheet's background is a palette INDEX, not simply "black" -- index 0 is
# also black and is real outline. Keying on the colour would eat her outlines.
BG_INDEX = 25


def halve(cell):
    """2x -> 1x by majority vote of each 2x2 block."""
    px = cell.load()
    out = Image.new("P", (HALF_W, HALF_H), BG_INDEX)
    out.putpalette(cell.getpalette())
    dst = out.load()
    for y in range(HALF_H):
        for x in range(HALF_W):
            quad = [px[x * 2, y * 2], px[x * 2 + 1, y * 2],
                    px[x * 2, y * 2 + 1], px[x * 2 + 1, y * 2 + 1]]
            dst[x, y] = max(set(quad), key=lambda v: (quad.count(v), v != BG_INDEX))
    return out


def build(src, name, col_base):
    frames = []
    for row, col in FRAMES:
        x = GRID_X + (col_base + col) * CELL_W
        frames.append(halve(src.crop((x, row * CELL_H, x + CELL_W, row * CELL_H + CELL_H))))

    # Collect the colours actually used, transparent first.
    used = []
    for f in frames:
        for p in f.getdata():
            if p != BG_INDEX and p not in used:
                used.append(p)
    if len(used) > 15:
        sys.exit("%s: %d colours, over the 15 + transparent limit" % (name, len(used)))

    src_pal = src.getpalette()
    palette = [(255, 0, 255)] + [tuple(src_pal[i * 3:i * 3 + 3]) for i in used]
    palette += [(0, 0, 0)] * (16 - len(palette))
    remap = {old: new + 1 for new, old in enumerate(used)}

    sheet = Image.new("P", (FRAME_W * len(frames), FRAME_H), 0)
    flat = []
    for rgb in palette:
        flat += list(rgb)
    sheet.putpalette(flat)

    dst = sheet.load()
    for n, f in enumerate(frames):
        px = f.load()
        for y in range(HALF_H):
            for x in range(HALF_W):
                v = px[x, y]
                if v != BG_INDEX:
                    dst[n * FRAME_W + OFF_X + x, OFF_Y + y] = remap[v]

    pic = os.path.join(OUT_PIC, name + ".png")
    sheet.save(pic)

    pal = os.path.join(OUT_PAL, name + ".pal")
    with open(pal, "w", newline="\r\n") as fh:
        fh.write("JASC-PAL\n0100\n16\n")
        for r, g, b in palette:
            fh.write("%d %d %d\n" % (r, g, b))

    print("%-18s %s  %d frames, %d colours -> %s" %
          (name, "%dx%d" % sheet.size, len(frames), len(used) + 1, pic))


def main():
    src = Image.open(SRC)
    if src.mode != "P":
        sys.exit("expected a paletted source sheet, got %s" % src.mode)
    for name, col_base in SHEETS:
        build(src, name, col_base)


if __name__ == "__main__":
    main()
