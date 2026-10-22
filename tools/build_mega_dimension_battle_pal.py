#!/usr/bin/env python3
"""Battle background palette for the Mega Dimension.

The Mega Dimension battles on the starfield backdrop (graphics/battle_environment/space)
recoloured to black and white: every colour goes to its luma, the contrast is pushed,
and the result is inverted, so the night sky becomes a white void and the bright floor
a black pit. Only the palette is new; the tiles and tilemap are the space backdrop's.

Usage: python tools/build_mega_dimension_battle_pal.py
"""
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "graphics", "battle_environment", "space", "palette.pal")
OUT_DIR = os.path.join(ROOT, "graphics", "battle_environment", "mega_dimension")
CONTRAST = 1.3


def main():
    lines = open(SRC).read().split("\n")
    count = int(lines[2])
    colours = [tuple(int(v) for v in lines[3 + i].split()) for i in range(count)]
    out = []
    for r, g, b in colours:
        luma = (r * 76 + g * 151 + b * 29) / (256 * 255)
        luma = min(1.0, max(0.0, 0.5 + (luma - 0.5) * CONTRAST))
        v = (int((1.0 - luma) * 255) >> 3) << 3
        out.append((v, v, v))
    os.makedirs(OUT_DIR, exist_ok=True)
    with open(os.path.join(OUT_DIR, "palette.pal"), "w", newline="\r\n") as fh:
        fh.write("JASC-PAL\n0100\n%d\n" % count)
        for c in out:
            fh.write("%d %d %d\n" % c)
    print("wrote", os.path.join(OUT_DIR, "palette.pal"))


if __name__ == "__main__":
    main()
