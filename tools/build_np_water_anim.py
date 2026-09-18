#!/usr/bin/env python3
"""
Build the National Park water animation frames (Route 224 secondary tileset).

The park's pond art is a run of twelve water tiles (secondary locals 70-81)
with diagonal wave streaks, and it never moved. Rather than draw new frames in
a style that would not match, each frame is the same art scrolled one pixel
along the streaks -- left and down -- so after eight frames it wraps exactly
back to the source and the loop is seamless.

Frames are written one tile per row (8 x 96) because INCGFX serialises a .4bpp
linearly: a single column means the tiles come out in local order, 70 first.

Run from the repo root:  python3 tools/build_np_water_anim.py
"""

import os

from PIL import Image

TILES = "data/tilesets/secondary/route_224/tiles.png"
OUT = "data/tilesets/secondary/route_224/anim/water"
FIRST, COUNT, FRAMES = 70, 12, 8


def main():
    sheet = Image.open(TILES).convert("P")
    per_row = sheet.width // 8
    src = sheet.load()
    os.makedirs(OUT, exist_ok=True)

    for frame in range(FRAMES):
        img = Image.new("P", (8, 8 * COUNT), 0)
        img.putpalette(sheet.getpalette())
        px = img.load()
        for i in range(COUNT):
            t = FIRST + i
            ox, oy = (t % per_row) * 8, (t // per_row) * 8
            for y in range(8):
                for x in range(8):
                    # one pixel per frame along the streaks: left and down
                    px[x, i * 8 + y] = src[ox + (x + frame) % 8,
                                           oy + (y - frame) % 8]
        img.save("%s/%d.png" % (OUT, frame))

    print("%s/0-%d.png: %d tiles (locals %d-%d), %d frames"
          % (OUT, FRAMES - 1, COUNT, FIRST, FIRST + COUNT - 1, FRAMES))


if __name__ == "__main__":
    main()
