#!/usr/bin/env python3
"""
Build the WoT red light sprites (LIGHT_TYPE_WOT_RED, LIGHT_TYPE_WOT_RED_SIGN)
used in Shin Tokyo.

An object event whose graphics id is OBJ_EVENT_GFX_LIGHT_SPRITE spawns a light
from gFieldEffectLightTemplates[trainerRange_berryTreeId] -- porymap's "sight
radius / berry tree id" field picks the light type. Vanilla ships three: the
warm 32x32 street-lamp ball (0) and the two 16x16 neon signs (1, 2). This adds
a red one for Shin Tokyo's lamp posts.

Two shapes, one palette (they share OBJ_EVENT_PAL_TAG_WOT_RED_LIGHT):

  wot_red_light.png       32x32  the lamp-post pool. The vanilla ball is a
                                 flat, hard-rimmed disc; this is a rounder
                                 pool with no rim, a hot core and a four-point
                                 glint, so it reads as neon bloom rather than
                                 a red version of the same lamp.
  wot_red_light_sign.png  64x64  the rectangle that hangs on the METRO sign
                                 and spills down onto the pavement: a bright
                                 bar across the top (the sign board itself),
                                 then four tile-rows of fall-off that widen as
                                 they drop, matching the light already painted
                                 into the tiles under the sign.

The sprite is drawn in ST_OAM_OBJ_BLEND over the night-darkened screen, so the
palette ramp is what does the lighting: index 15 is the brightest point of the
glow and index 1 is barely there. Index 0 is the transparent key (the same
green vanilla's light.png uses), and it is never drawn.

Run from the repo root:  python3 tools/build_wot_red_light.py
"""

import math

from PIL import Image

PNG = "graphics/object_events/pics/misc/wot_red_light.png"
SIGN_PNG = "graphics/object_events/pics/misc/wot_red_light_sign.png"
PAL = "graphics/object_events/palettes/wot_red_light.pal"

SIZE = 32
SIGN_W, SIGN_H = 64, 64

# 0 = transparent key, then deep ember -> hot pink-white.
COLORS = [
    (110, 198, 165),
    (40, 6, 14), (56, 8, 18), (74, 10, 22), (94, 12, 26), (114, 14, 30),
    (135, 17, 34), (156, 20, 38), (177, 24, 44), (197, 30, 52), (214, 40, 62),
    (228, 56, 76), (238, 78, 92), (246, 104, 110), (251, 134, 132), (255, 170, 160),
]


def brightness(x, y):
    """0..1 light intensity at a pixel centre."""
    cx = cy = (SIZE - 1) / 2.0
    dx, dy = (x - cx) / 14.5, (y - cy) / 11.5
    r = math.hypot(dx, dy)
    core = max(0.0, 1.0 - r) ** 1.45
    # four-point glint: a soft horizontal and vertical flare across the pool
    fx = math.exp(-((y - cy) / 2.0) ** 2) * max(0.0, 1.0 - abs(x - cx) / 15.5)
    fy = math.exp(-((x - cx) / 2.0) ** 2) * max(0.0, 1.0 - abs(y - cy) / 15.5)
    return min(1.0, core + 0.30 * max(fx, fy) ** 1.5)


def sign_brightness(x, y):
    """0..1 for the rectangular sign light.

    The light is cast DOWNWARD: the board itself only glows (rows 0-13 sit at
    a low, even level), the beam brightens as it falls, burns brightest where
    it lands on the pavement two tile-rows down, and fades out at the far edge
    of the pool. It also opens outwards the whole way down, so the lit patch on
    the street is wider than the board that throws it.
    """
    cx = (SIGN_W - 1) / 2.0
    board_bottom, pool_top, pool_bottom = 13.0, 30.0, 46.0
    if y <= board_bottom:                 # the board: a steady glow, no more
        v = 0.42 + 0.06 * (y / board_bottom)
    elif y < pool_top:                    # the beam on its way down
        v = 0.48 + 0.52 * ((y - board_bottom) / (pool_top - board_bottom)) ** 1.3
    elif y <= pool_bottom:                # where it lands
        v = 1.0
    else:                                 # the far edge of the pool
        v = max(0.0, 1.0 - (y - pool_bottom) / (SIGN_H - 3 - pool_bottom)) ** 1.2
    drop = y / float(SIGN_H - 1)
    half = 17.0 + 13.0 * drop             # opens outwards all the way down
    soft = 4.0 + 7.0 * drop               # and blurs at the edges
    h = min(1.0, max(0.0, (half - abs(x - cx)) / soft))
    return v * h


def render(path, w, h, fn):
    img = Image.new("P", (w, h), 0)
    flat = []
    for c in COLORS:
        flat += list(c)
    img.putpalette(flat + [0, 0, 0] * (256 - len(COLORS)))
    px = img.load()
    lit = 0
    for y in range(h):
        for x in range(w):
            v = int(round(fn(x, y) * 15))
            px[x, y] = v
            lit += v > 0
    img.save(path)
    print("%s  %dx%d, %d lit pixels" % (path, w, h, lit))


def main():
    render(PNG, SIZE, SIZE, brightness)
    render(SIGN_PNG, SIGN_W, SIGN_H, sign_brightness)

    with open(PAL, "w", newline="\n") as f:
        f.write("JASC-PAL\n0100\n16\n")
        for r, g, b in COLORS:
            f.write("%d %d %d\n" % (r, g, b))

    print("%s  %d colours (shared by both sprites)" % (PAL, len(COLORS)))


if __name__ == "__main__":
    main()
