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

# The hand-made plates are SLANTED, and leaving that out is why THE ONI and RED
# FATALITY read as a different typeface from SIREN ALLISON and GENERAL EDWARDS.
# Both shipped plates measure the same slant: each row sits 0.19px further right
# than the one above it -- a backslant of about 10.8 degrees, not a forward
# italic. Measured by finding the shear that makes their stems stack into
# columns, and confirmed by reading the two L stems of ALLISON directly (their
# left edge runs x=15 at the top to x=18 at the bottom over 19 rows).
SHEAR = 0.19

# The shipped plates are set very tight -- adjacent letters all but touch, which
# is most of why they read as heavier than a default Impact string. Measured as
# ink inside the text's own bounding box: SIREN ALLISON 83%, GENERAL EDWARDS
# 92%, against 60% for an untracked render. This pulls each letter back toward
# the one before it to close that gap.
TRACK = -2

# Cap height. The script used to allow H-6, but SIREN ALLISON is 29 rows tall in
# a 32-row plate, so that cap alone kept generated plates visibly smaller than
# the hand-made ones. One row is left for the drop shadow.
MAX_GLYPH_H = H - 3

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


def build_mask(text, font):
    """Upright text, sheared, then centred in the plate.

    The shear is applied to the glyph mask BEFORE the bevel and shadow passes,
    so the one-pixel edges follow the slanted outline the way they do on the
    hand-made plates. Shearing the finished plate instead would tear the bevel
    into steps.

    Rows pivot about the middle of the plate rather than the top, which keeps
    the text centred and costs half as much width as pivoting on an edge.
    """
    pad = 64
    wide = Image.new("1", (W + 2 * pad, H), 0)
    d = ImageDraw.Draw(wide)
    box = font.getbbox(text)

    # Drawn letter by letter so TRACK can pull them together; PIL's text() would
    # lay the string out at the font's own spacing.
    x = pad + (W - (box[2] - box[0])) // 2 - box[0]
    y = (H - 2 - (box[3] - box[1])) // 2 - box[1]
    for ch in text:
        d.text((x, y), ch, font=font, fill=1)
        x += font.getlength(ch) + (TRACK if ch != " " else 0)

    sheared = Image.new("1", wide.size, 0)
    pivot = (H - 1) / 2.0
    for y in range(H):
        dx = int(round(SHEAR * (y - pivot)))
        sheared.paste(wide.crop((0, y, wide.size[0], y + 1)), (dx, y))

    bbox = sheared.getbbox()
    if bbox is None:
        sys.exit("nothing rendered for: " + text)
    glyphs = sheared.crop(bbox)

    mask = Image.new("1", (W, H), 0)
    mask.paste(glyphs, ((W - glyphs.width) // 2, bbox[1]))
    return mask, glyphs.size


def pick_font(text):
    path = next((f for f in FONTS if os.path.exists(f)), None)
    if path is None:
        sys.exit("no heavy font found; point FONTS at one")
    # Grow until it fills the plate. The fit is tested on the SHEARED mask --
    # slanting widens the text, so sizing on the upright box overflows.
    best = None
    for size in range(12, 48):
        font = ImageFont.truetype(path, size)
        _, (w, h) = build_mask(text, font)
        if w <= W - 8 and h <= MAX_GLYPH_H:
            best = font
        else:
            break
    if best is None:
        sys.exit("text does not fit: " + text)
    return best


def main():
    text, out = sys.argv[1].upper(), sys.argv[2]
    font = pick_font(text)
    mask, _ = build_mask(text, font)
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
