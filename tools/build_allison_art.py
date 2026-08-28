#!/usr/bin/env python3
"""
Wishes of Tomorrow -- Siren Allison's battle sprite and portrait.

Battle sprite comes out of the artist's sheet (the tall figure on its left),
scaled to the 64x64 trainer-pic box with its aspect kept, so she is not
squashed. Portrait is rendered down from the 1408px character art.

Both are quantised to 16 colours with the first slot reserved for the
transparent key, which is what the 4bpp trainer-pic and portrait formats want.

Run from the repo root:  python3 tools/build_allison_art.py
"""

import os
import sys

from PIL import Image

SHEET = os.path.join("..", "custom sprites", "Trainer Sprites", "allison.png")
PORTRAIT_SRC = os.path.join("..", "custom sprites", "Key Trainer Sprites", "swimmer_allison.png")

BATTLE_OUT = os.path.join("graphics", "trainers", "front_pics", "wot_allison_land.png")
PORTRAIT_OUT = os.path.join("graphics", "portraits", "allison.png")

BG_INDEX = 25          # the sheet's background slot; index 0 is also black but is outline
TRANSPARENT = (255, 0, 255)
BOX = 64


def premultiplied_resize(rgba, size):
    """Downscale RGBA without letting the transparent pixels bleed in.

    Resampling RGBA directly averages the colour of fully transparent pixels
    into the edges, which is how a keyed image ends up with a halo of the key
    colour. Multiplying by alpha first, resampling both, then dividing back out
    keeps the edge colours honest.
    """
    r, g, b, a = rgba.split()
    pm = [Image.merge("L", [c]).point(lambda v, _a=a: v) for c in (r, g, b)]
    px_a = a.load()
    chans = []
    for ch in (r, g, b):
        src = ch.load()
        out = Image.new("L", rgba.size)
        dst = out.load()
        for y in range(rgba.height):
            for x in range(rgba.width):
                dst[x, y] = src[x, y] * px_a[x, y] // 255
        chans.append(out.resize(size, Image.LANCZOS))
    a_small = a.resize(size, Image.LANCZOS)
    pa = a_small.load()
    res = Image.new("RGBA", size)
    rp = res.load()
    cl = [c.load() for c in chans]
    for y in range(size[1]):
        for x in range(size[0]):
            av = pa[x, y]
            if av < 8:
                rp[x, y] = (0, 0, 0, 0)
            else:
                rp[x, y] = (min(255, cl[0][x, y] * 255 // av),
                            min(255, cl[1][x, y] * 255 // av),
                            min(255, cl[2][x, y] * 255 // av), 255)
    return res


def key_magenta(rgb, tol=60):
    """Alpha-key a magenta background, then eat one pixel of fringe.

    The portrait source is a JPEG, so its key is not one exact colour -- it
    smears around (239,0,224). Keying on distance catches the smear; eroding
    afterwards removes the half-magenta rim JPEG leaves behind, which would
    otherwise survive the downscale as a pink outline.
    """
    w, h = rgb.size
    px = rgb.load()
    out = rgb.convert("RGBA")
    op = out.load()
    keyed = [[False] * h for _ in range(w)]
    for y in range(h):
        for x in range(w):
            r, g, b = px[x, y]
            if r > 170 and g < 90 and b > 150 and abs(r - b) < 90:
                keyed[x][y] = True
    for y in range(h):
        for x in range(w):
            if keyed[x][y]:
                op[x, y] = (0, 0, 0, 0)
            elif any(0 <= x + dx < w and 0 <= y + dy < h and keyed[x + dx][y + dy]
                     for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                op[x, y] = (0, 0, 0, 0)   # erode the fringe
    return out


def content_bbox_indexed(im, bg):
    """Bounding box in INDEX space. Converting a P image to L reads palette
    COLOURS instead of indices, which silently mis-measures any art whose
    background colour also appears as real pixels -- this sheet's does."""
    px = im.load()
    w, h = im.size
    xs = [x for x in range(w) for y in range(h) if px[x, y] != bg]
    ys = [y for y in range(h) for x in range(w) if px[x, y] != bg]
    return min(xs), min(ys), max(xs) + 1, max(ys) + 1


def to_indexed(rgba, colours):
    """RGBA -> paletted, transparent key in slot 0."""
    flat = rgba.convert("RGB").quantize(colors=colours - 1, method=Image.MEDIANCUT)
    pal = flat.getpalette()[: (colours - 1) * 3]

    out = Image.new("P", rgba.size, 0)
    out.putpalette(list(TRANSPARENT) + pal + [0] * (16 * 3 - 3 - len(pal)))
    src, dst, alpha = flat.load(), out.load(), rgba.split()[3].load()
    for y in range(rgba.height):
        for x in range(rgba.width):
            dst[x, y] = 0 if alpha[x, y] < 128 else src[x, y] + 1
    return out


def build_battle():
    sheet = Image.open(SHEET)
    if sheet.mode != "P":
        sys.exit("expected a paletted sheet, got %s" % sheet.mode)

    # The figure lives to the left of the overworld grid (which starts at x=160).
    left = sheet.crop((0, 0, 160, sheet.height))
    box = content_bbox_indexed(left, BG_INDEX)
    fig = left.crop(box)

    # Cut the background to real transparency before resampling, or the key
    # colour bleeds into her edges on the way down.
    fig = fig.convert("RGBA")
    px = fig.load()
    src = left.crop(box).load()
    for y in range(fig.height):
        for x in range(fig.width):
            if src[x, y] == BG_INDEX:
                px[x, y] = (0, 0, 0, 0)

    # Fit the 64x64 trainer box, aspect preserved, standing on the bottom edge.
    scale = BOX / fig.height
    new = (max(1, round(fig.width * scale)), BOX)
    fig = premultiplied_resize(fig, new)

    canvas = Image.new("RGBA", (BOX, BOX), (0, 0, 0, 0))
    canvas.paste(fig, ((BOX - fig.width) // 2, BOX - fig.height), fig)

    out = to_indexed(canvas, 16)
    out.save(BATTLE_OUT)
    print("battle sprite  %s from %dx%d source -> %s" %
          ("%dx%d" % out.size, box[2] - box[0], box[3] - box[1], BATTLE_OUT))


def build_portrait():
    src = Image.open(PORTRAIT_SRC).convert("RGB")
    # The render sits on a magenta field. Key it -- otherwise the quantiser
    # spends palette slots on it and it shows up as an opaque pink background
    # in game, because portraits treat slot 0 as transparent (see edwards.png).
    rgba = key_magenta(src)
    box = rgba.getbbox()
    if box:
        rgba = rgba.crop(box)
    # Fit the square portrait box, aspect preserved.
    scale = min(BOX / rgba.width, BOX / rgba.height)
    new = (max(1, round(rgba.width * scale)), max(1, round(rgba.height * scale)))
    small = premultiplied_resize(rgba, new)
    canvas = Image.new("RGBA", (BOX, BOX), (0, 0, 0, 0))
    canvas.paste(small, ((BOX - new[0]) // 2, BOX - new[1]), small)
    out = to_indexed(canvas, 16)
    out.save(PORTRAIT_OUT)
    print("portrait       %s from %dx%d source -> %s" %
          ("%dx%d" % out.size, src.width, src.height, PORTRAIT_OUT))


if __name__ == "__main__":
    build_battle()
    build_portrait()
