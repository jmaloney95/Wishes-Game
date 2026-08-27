#!/usr/bin/env python3
"""
Wishes of Tomorrow -- build 16x16 overworld pickup sprites for the mega stones
that lie on the floor in the Distortion World.

Each mega stone's 24x24 bag icon carries its art inside a 14x14 box, so this
just crops that box and centres it on a 16x16 canvas. Nothing is scaled and no
colour is remapped, which means:

  * the sprite is pixel-identical to the icon the player sees in the bag, and
  * the object event can point straight at the icon's OWN palette file
    (graphics/items/icon_palettes/<stone>.pal) instead of needing a new one.

That last point matters: the shared OBJ_EVENT_PAL_TAG_GROUND_ITEMS palette used
by the Potion / Carved Mask / Catalpa Bow pickups has all 16 slots spoken for,
and the stones are blue-heavy, so squeezing them in would have meant recolouring
the existing three. The stones never share a map, so a palette each is free.

Run from the repo root:  python3 tools/build_mega_stone_ow.py
"""

import os
import sys

from PIL import Image

STONES = ["garchompite", "baxcalibrite", "gyaradosite"]

ICON_DIR = os.path.join("graphics", "items", "icons")
OUT_DIR = os.path.join("graphics", "object_events", "pics", "misc")

CANVAS = 16
TRANSPARENT_INDEX = 0


def content_bbox(image):
    """Bounding box of non-transparent pixels, in INDEX space.

    Deliberately reads raw palette indices. Converting a P-mode image to L (or
    calling .point() on it) reads the palette COLOURS instead, which silently
    gives the wrong answer whenever a real colour happens to be dark.
    """
    px = image.load()
    w, h = image.size
    xs = [x for x in range(w) for y in range(h) if px[x, y] != TRANSPARENT_INDEX]
    ys = [y for y in range(h) for x in range(w) if px[x, y] != TRANSPARENT_INDEX]
    if not xs:
        raise ValueError("image is entirely transparent")
    return min(xs), min(ys), max(xs) + 1, max(ys) + 1


def build(stone):
    icon = Image.open(os.path.join(ICON_DIR, stone + ".png"))
    if icon.mode != "P":
        raise ValueError("%s: expected a paletted icon, got %s" % (stone, icon.mode))

    left, top, right, bottom = content_bbox(icon)
    w, h = right - left, bottom - top
    if w > CANVAS or h > CANVAS:
        raise ValueError("%s: art is %dx%d, will not fit %dx%d without scaling"
                         % (stone, w, h, CANVAS, CANVAS))

    out = Image.new("P", (CANVAS, CANVAS), TRANSPARENT_INDEX)
    out.putpalette(icon.getpalette())
    out.paste(icon.crop((left, top, right, bottom)),
              ((CANVAS - w) // 2, (CANVAS - h) // 2))

    path = os.path.join(OUT_DIR, "item_%s.png" % stone)
    out.save(path)
    return path, w, h, sorted(set(out.getdata()))


def main():
    for stone in STONES:
        path, w, h, used = build(stone)
        print("%-14s %dx%d art -> %s (%d colours)" % (stone, w, h, path, len(used)))
        if max(used) > 15:
            sys.exit("%s: index %d exceeds the 16-colour 4bpp limit" % (stone, max(used)))


if __name__ == "__main__":
    main()
