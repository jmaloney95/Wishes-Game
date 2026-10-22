#!/usr/bin/env python3
"""Placeholder art for the Mega Dimension portal (OBJ_EVENT_GFX_WOT_MEGA_PORTAL).

Writes a 32x128 indexed PNG: four 32x32 frames stacked vertically (the
layout the object pic table reads, like the jumbotron sheets), plus the
matching JASC palette. Index 0 is transparent.

The frames are a swirling rift: an elliptical ring of pink/violet/cyan bands
that rotate a quarter-turn's worth per frame, with a bright core. Replace
graphics/object_events/pics/misc/wot_mega_portal.png (and the .pal) with real
art whenever it exists, keeping the 32x128 / 16-colour layout, then delete
the generated .4bpp/.gbapal so make rebuilds them.

Usage: python tools/build_mega_portal_placeholder.py
"""
import math
import os

from PIL import Image

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PNG_OUT = os.path.join(ROOT, "graphics", "object_events", "pics", "misc", "wot_mega_portal.png")
PAL_OUT = os.path.join(ROOT, "graphics", "object_events", "palettes", "wot_mega_portal.pal")

FRAME = 32
FRAMES = 4

# Index 0 transparent (magenta key), then a dark outline, the swirl bands,
# and the bright core. All values sit on the GBA's 5-bit grid.
PALETTE = [
    (255, 0, 255),   # 0 transparent
    (40, 16, 64),    # 1 outline
    (96, 32, 136),   # 2 deep violet
    (144, 56, 192),  # 3 violet
    (200, 80, 208),  # 4 magenta
    (248, 112, 192), # 5 pink
    (248, 168, 216), # 6 light pink
    (64, 168, 232),  # 7 blue
    (96, 216, 248),  # 8 cyan
    (176, 240, 248), # 9 pale cyan
    (248, 248, 248), # 10 white core
    (232, 200, 248), # 11 lilac glow
    (0, 0, 0), (0, 0, 0), (0, 0, 0), (0, 0, 0),
]

BANDS = [2, 3, 4, 5, 6, 5, 4, 3, 7, 8, 9, 8, 7]


def snap(c):
    return tuple((v >> 3) << 3 for v in c)


def render_frame(phase):
    img = Image.new("P", (FRAME, FRAME), 0)
    px = img.load()
    cx, cy = 15.5, 17.0
    rx, ry = 11.5, 14.0  # a tall oval, like a doorway
    for y in range(FRAME):
        for x in range(FRAME):
            dx, dy = (x - cx) / rx, (y - cy) / ry
            r = math.hypot(dx, dy)
            if r > 1.0:
                continue
            if r > 0.9:
                px[x, y] = 1
                continue
            ang = math.atan2(dy, dx)
            # spiral: the band index advances with angle and with distance
            t = (ang / (2 * math.pi)) * len(BANDS) + r * 9.0 + phase * len(BANDS) / FRAMES
            idx = BANDS[int(math.floor(t)) % len(BANDS)]
            if r < 0.22:
                idx = 10
            elif r < 0.36:
                idx = 11 if (int(t) % 2) else 9
            px[x, y] = idx
    return img


def main():
    sheet = Image.new("P", (FRAME, FRAME * FRAMES), 0)
    flat = []
    for c in PALETTE:
        flat.extend(snap(c))
    sheet.putpalette(flat)
    for f in range(FRAMES):
        frame = render_frame(f)
        sheet.paste(frame, (0, f * FRAME))
    sheet.putpalette(flat)
    sheet.save(PNG_OUT, transparency=0)

    with open(PAL_OUT, "w", newline="\r\n") as fh:
        fh.write("JASC-PAL\n0100\n16\n")
        for c in PALETTE:
            r, g, b = snap(c)
            fh.write(f"{r} {g} {b}\n")
    print("wrote", PNG_OUT)
    print("wrote", PAL_OUT)


if __name__ == "__main__":
    main()
