#!/usr/bin/env python3
"""
Build a boss-intro card name plate (graphics/boss_intro/name_<x>.png).

The card is three sprites: the banner, a 64x64 portrait and a 192x32 name
plate. The name plates that already shipped (SIREN ALLISON, EDWARDS) are
hand-made, and this rebuilds that exact treatment for new ones:

  * heavy condensed capitals, centred in 192x32;
  * a hard two-tone fill -- white above the split row, grey below;
  * a one-pixel bevel, gold on the top/left edges and dark gold on the
    bottom/right ones;
  * a one-pixel black drop shadow down and right.

Indices come from the shared card palette (graphics/boss_intro/banner.pal),
because the name sprite is drawn with the banner's palette (TAG_BI_BANNER) --
0 transparent, 1 black, 7 gold, 8 dark gold, 9 white, 10 grey.

Usage:  python3 tools/build_boss_name.py "THE ONI" graphics/boss_intro/name_oni.png
"""

import os
import sys

from PIL import Image, ImageDraw, ImageFont

W, H = 192, 32
SPLIT = 19          # first grey row, measured off name_allison.png
TRANSPARENT, BLACK, GOLD, GOLD_DARK, WHITE, GREY = 0, 1, 7, 8, 9, 10

FONTS = ["C:/Windows/Fonts/impact.ttf", "/mnt/c/Windows/Fonts/impact.ttf",
         "C:/Windows/Fonts/arialbd.ttf", "/mnt/c/Windows/Fonts/arialbd.ttf"]
PAL = "graphics/boss_intro/banner.pal"


def load_palette():
    lines = [l.strip() for l in open(PAL) if l.strip()][3:]
    flat = []
    for line in lines:
        flat += [int(v) for v in line.split()]
    return flat + [0] * (768 - len(flat))


def pick_font(text):
    path = next((f for f in FONTS if os.path.exists(f)), None)
    if path is None:
        sys.exit("no heavy font found; point FONTS at one")
    # grow until the text fills the plate, leaving room for bevel + shadow
    best = None
    for size in range(12, 48):
        font = ImageFont.truetype(path, size)
        box = font.getbbox(text)
        w, h = box[2] - box[0], box[3] - box[1]
        if w <= W - 8 and h <= H - 6:
            best = (font, box)
        else:
            break
    if best is None:
        sys.exit("text does not fit: " + text)
    return best


def main():
    text, out = sys.argv[1].upper(), sys.argv[2]
    font, box = pick_font(text)

    mask = Image.new("1", (W, H), 0)
    d = ImageDraw.Draw(mask)
    d.text(((W - (box[2] - box[0])) // 2 - box[0],
            (H - 2 - (box[3] - box[1])) // 2 - box[1]), text, font=font, fill=1)
    m = mask.load()

    def on(x, y):
        return 0 <= x < W and 0 <= y < H and m[x, y]

    img = Image.new("P", (W, H), TRANSPARENT)
    img.putpalette(load_palette())
    px = img.load()

    for y in range(H):
        for x in range(W):
            if on(x, y):
                continue
            if on(x - 1, y - 1) and not on(x - 1, y) and not on(x, y - 1):
                continue  # diagonal-only contact: leave it open
            if on(x - 1, y) or on(x, y - 1) or on(x - 1, y - 1):
                px[x, y] = BLACK  # the drop shadow sits down and right

    for y in range(H):
        for x in range(W):
            if not on(x, y):
                continue
            if not on(x, y - 1) or not on(x - 1, y):
                px[x, y] = GOLD
            elif not on(x, y + 1) or not on(x + 1, y):
                px[x, y] = GOLD_DARK
            else:
                px[x, y] = WHITE if y < SPLIT else GREY

    img.save(out)
    lit = sum(1 for y in range(H) for x in range(W) if px[x, y])
    print("%s: %r at %dpx, %d lit pixels" % (out, text, font.size, lit))


if __name__ == "__main__":
    main()
