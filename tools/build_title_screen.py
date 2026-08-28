#!/usr/bin/env python3
"""
Wishes of Tomorrow -- build the title screen by COMPOSITING separate layers.

This used to work the other way round: one flattened mock-up came in, and the
tool tried to cut the lettering and Jirachi back out of it so they could be
replaced -- masking them by colour, then inpainting the holes left behind. That
is an unwinnable job on this art. Jirachi's silver is the same neutral grey as
the mountain's shaded snow, so no colour test separates them; and a hole in a
starfield cannot be reconstructed, because nothing you interpolate contains
stars. Every fix traded one artifact for another.

The artist now supplies the layers separately, so none of that is needed. There
is nothing to detect, nothing to cut out and no hole to fill: each layer is
simply drawn over the one below it, in order.

    bottom  custom ui/start menu full bg.jpg   sky, mountain, rocks
            custom ui/pokemon-logo.png         the logo, already keyed
            custom ui/wishes text.jpg          the stylised lockup
    top     custom ui/jirachi.png              Shadow Jirachi, already keyed

PRESS START is NOT composited. It blinks, so it is a sprite, and a sprite must
not also be sitting in the picture underneath or a permanent copy shows through
the blink. Its lettering is lifted from the old flattened mock-up, which is the
only place the artist ever drew it.

The picture goes on a 256-colour (8bpp) background. The nebula is one big smooth
gradient, and 4bpp -- sixteen colours per 8x8 tile -- visibly bands it. At 8bpp
the whole screen shares one 256-entry palette and the gradient survives. It
costs 64 bytes per tile instead of 32; the screen is 30x20 tiles, so the worst
case is 600 x 64 = 38,400 bytes, which fits BG VRAM with the tilemap above it.

Run from the repo root:  python3 tools/build_title_screen.py
"""

import os
import struct
import sys

import numpy as np
from PIL import Image

ART = os.path.join("..", "custom ui")

# The layers, bottom to top.
BG_SRC = os.path.join(ART, "start menu full bg.jpg")
LOGO_SRC = os.path.join(ART, "pokemon-logo.png")
TITLE_SRC = os.path.join(ART, "wishes text.jpg")
JIRACHI_SRC = os.path.join(ART, "jirachi.png")

# Not a layer: the source for the blinking sprite's lettering.
PRESS_SRC = os.path.join(ART, "start menu revised.jpg")

OUT = os.path.join("graphics", "title_screen")

W, H = 240, 160
TILE = 8
COLS, ROWS = W // TILE, H // TILE      # 30 x 20

# --- placement ---------------------------------------------------------------
# Measured off the approved mock-up so the composite lands where the artist put
# things: its logo occupied y 8..51, and the mountain's snow cap starts at y 93
# with its apex near x 120. The stack below keeps that layout and leaves each
# layer clear of the next.

LOGO_WIDTH = 140                  # the lockup is 2.725:1, so this is 51 tall
LOGO_TOP = 1

TITLE_HEIGHT = 16                 # glyph height in px
TITLE_STRETCH = 1.18              # >1 widens past true aspect, to fill the span
TITLE_CENTRE_Y = 61
# White with a dark stroke. The stroke is the only thing holding the lettering
# off a busy nebula -- there is no box behind it any more.
TITLE_FILL = (252, 246, 252)
TITLE_STROKE = (10, 6, 14)

JIRACHI_HEIGHT = 34
JIRACHI_CENTRE = (120, 88)        # hovering just above the peak at y 93

# PRESS START's box in the old mock-up. Matches the C: three 32x16 sprites at
# x 88/120/152 (centres), y 139, so together they cover x 72..168.
BOX_PRESS = (72, 134, 168, 144)
SPR_W, SPR_H, SPR_FRAMES = 32, 16, 4


# --- getting the art down to 240x160 -----------------------------------------
# The backgrounds are drawn on a visible canvas grid and the grid is baked into
# the JPEG. Working out where it sits took four wrong models: it is not a line
# every 21 source pixels (that is four lines -- the true period is one drawn
# pixel, 5.25 source px); it is not on the cell boundaries (measured, the
# darkest phase is about 44% INTO each cell, so sampling "cell interiors" to
# dodge it aimed straight at it); and it is not a constant darkening but a
# semi-transparent overlay, so bright sky loses more levels than dark rock.
#
# The fix in the end was not to model it at all. Each drawn pixel is a 5.25 x
# 5.25 block of source pixels and the grid line crosses roughly one row and one
# column of that block -- a MINORITY of the samples. A median throws minority
# samples away by construction, so it drops the grid without needing to know its
# period, phase, colour or opacity, none of which had to be guessed correctly.
# It also returns a flat block exactly, which is what makes the logo and the
# rocks read as solid rather than as speckle.

def cell_median(im):
    """One output pixel per drawn pixel, taken as a grid-rejecting median."""
    a = np.asarray(im).astype(np.int16)
    sh, sw = a.shape[0] / H, a.shape[1] / W
    out = np.zeros((H, W, 3), dtype=np.uint8)
    for Y in range(H):
        y0, y1 = int(Y * sh), max(int(Y * sh) + 1, int((Y + 1) * sh))
        for X in range(W):
            x0, x1 = int(X * sw), max(int(X * sw) + 1, int((X + 1) * sw))
            block = a[y0:y1, x0:x1].reshape(-1, 3)
            med = np.median(block, axis=0)
            # Snap to a colour that genuinely appears in the block, so every
            # output pixel is one the artist used rather than an average of
            # three of them.
            out[Y, X] = block[np.argmin(((block - med) ** 2).sum(axis=1))]
    return Image.fromarray(out)


def load_flat(path):
    """A flattened JPEG -> the 240x160 frame, cropped to 3:2 and grid-rejected."""
    im = Image.open(path).convert("RGB")
    tw = im.width
    th = int(round(tw / 1.5))
    if th > im.height:
        th = im.height
        tw = int(round(th * 1.5))
    x0, y0 = (im.width - tw) // 2, (im.height - th) // 2
    return cell_median(im.crop((x0, y0, x0 + tw, y0 + th)))


# --- compositing --------------------------------------------------------------

def content_crop(rgba):
    """Trim to the pixels that are actually drawn, so placement is by artwork
    rather than by whatever canvas the artist happened to export."""
    a = np.asarray(rgba)
    ys, xs = np.nonzero(a[..., 3] > 40)
    return rgba.crop((int(xs.min()), int(ys.min()),
                      int(xs.max()) + 1, int(ys.max()) + 1))


def premultiplied_resize(rgba, size):
    """Downscale RGBA without letting transparent pixels bleed into the edges.

    Resampling RGBA directly averages the COLOUR of fully transparent pixels
    into the rim -- which is how a keyed image ends up fringed in whatever was
    sitting in its unused channels. Multiplying by alpha first, resampling both,
    then dividing back out keeps the edge colours honest.
    """
    a = np.asarray(rgba).astype(np.float32)
    al = a[..., 3] / 255.0
    chans = [np.asarray(Image.fromarray(a[..., c] * al)
                        .resize(size, Image.LANCZOS)) for c in range(3)]
    a_small = np.asarray(Image.fromarray(al * 255.0)
                         .resize(size, Image.LANCZOS)).clip(0, 255)

    out = np.zeros((size[1], size[0], 4), dtype=np.uint8)
    safe = np.maximum(a_small / 255.0, 1e-6)
    for c in range(3):
        out[..., c] = np.clip(chans[c] / safe, 0, 255).astype(np.uint8)
    out[..., 3] = a_small.astype(np.uint8)
    out[a_small < 8] = 0
    return Image.fromarray(out)


def paste_rgba(base, layer, ox, oy, label):
    """Alpha-composite one layer onto the frame at a top-left offset.

    The edges are blended rather than keyed hard: at 255 colours there is room
    for the handful of in-between tones, and they are what stop the lockup and
    Jirachi from looking cut out with scissors.
    """
    b = np.asarray(base).astype(np.float32)
    l = np.asarray(layer).astype(np.float32)
    lh, lw = l.shape[:2]

    x0, y0 = max(0, ox), max(0, oy)
    x1, y1 = min(W, ox + lw), min(H, oy + lh)
    if x0 >= x1 or y0 >= y1:
        sys.exit("%s: placed entirely off-screen at (%d,%d)" % (label, ox, oy))
    if (x1 - x0, y1 - y0) != (lw, lh):
        print("  %s: clipped to the frame" % label)

    sub = l[y0 - oy:y1 - oy, x0 - ox:x1 - ox]
    al = sub[..., 3:4] / 255.0
    b[y0:y1, x0:x1] = sub[..., :3] * al + b[y0:y1, x0:x1] * (1.0 - al)
    print("  %-9s %3dx%-3d at (%3d,%3d) -> x %3d..%3d, y %3d..%3d"
          % (label, lw, lh, ox, oy, x0, x1 - 1, y0, y1 - 1))
    return Image.fromarray(b.clip(0, 255).astype(np.uint8))


def make_title_layer():
    """The stylised lockup, as white glyphs with a one-pixel dark stroke.

    The source is white-on-black, so it keys on luminance alone -- no mask, no
    guessing. It is resized in GREYSCALE and thresholded afterwards rather than
    resized as a bitmap, which keeps the diagonal strokes clean.
    """
    art = Image.open(TITLE_SRC).convert("L")
    a = np.asarray(art)
    ys, xs = np.nonzero(a > 100)
    art = art.crop((int(xs.min()), int(ys.min()),
                    int(xs.max()) + 1, int(ys.max()) + 1))

    h = TITLE_HEIGHT
    w = int(round(art.width * h / art.height * TITLE_STRETCH))
    g = np.asarray(art.resize((w, h), Image.LANCZOS)) > 110

    pad = 1
    fill = np.zeros((h + 2 * pad, w + 2 * pad), bool)
    fill[pad:pad + h, pad:pad + w] = g
    stroke = np.zeros_like(fill)
    for dy in (-1, 0, 1):
        for dx in (-1, 0, 1):
            stroke |= np.roll(np.roll(fill, dy, 0), dx, 1)

    out = np.zeros(fill.shape + (4,), dtype=np.uint8)
    out[stroke] = TITLE_STROKE + (255,)
    out[fill] = TITLE_FILL + (255,)
    return Image.fromarray(out)


def compose():
    """Draw the four layers, bottom to top."""
    print("compositing:")
    frame = load_flat(BG_SRC)
    print("  %-9s %3dx%-3d (grid-rejected median downscale)" % ("bg", W, H))

    logo = content_crop(Image.open(LOGO_SRC).convert("RGBA"))
    lh = max(1, round(logo.height * LOGO_WIDTH / logo.width))
    logo = premultiplied_resize(logo, (LOGO_WIDTH, lh))
    frame = paste_rgba(frame, logo, (W - LOGO_WIDTH) // 2, LOGO_TOP, "logo")

    title = make_title_layer()
    frame = paste_rgba(frame, title, (W - title.width) // 2,
                       TITLE_CENTRE_Y - title.height // 2, "title")

    jir = content_crop(Image.open(JIRACHI_SRC).convert("RGBA"))
    jw = max(1, round(jir.width * JIRACHI_HEIGHT / jir.height))
    jir = premultiplied_resize(jir, (jw, JIRACHI_HEIGHT))
    cx, cy = JIRACHI_CENTRE
    frame = paste_rgba(frame, jir, cx - jw // 2, cy - JIRACHI_HEIGHT // 2,
                       "jirachi")

    return frame


# --- the blinking PRESS START sprite ------------------------------------------

def is_text(px):
    """The glyphs are neutral -- near-black outline, near-white fill -- and sit
    on blue rock. Blue has a big blue-minus-red; neutral does not."""
    r, g, b = px
    return abs(r - b) < 46 and abs(r - g) < 46


def drop_specks(mask, minimum=9):
    """Discard tiny connected blobs. The neutral test also catches the odd grey
    speck of rock inside the box; a real glyph stroke is never this small."""
    seen, keep = set(), set()
    for start in mask:
        if start in seen:
            continue
        stack, blob = [start], []
        seen.add(start)
        while stack:
            x, y = stack.pop()
            blob.append((x, y))
            for dx in (-1, 0, 1):
                for dy in (-1, 0, 1):
                    n = (x + dx, y + dy)
                    if n in mask and n not in seen:
                        seen.add(n)
                        stack.append(n)
        if len(blob) >= minimum:
            keep.update(blob)
    return keep


def build_text_sheet():
    """Lift PRESS START out of the old mock-up into a four-frame sprite sheet.

    Frames are STACKED rather than laid out in a row: that is what makes each
    frame's tiles contiguous, which 1D OBJ mapping requires. 32x16 because the
    GBA has no 64x16 object -- the legal horizontal sizes are 16x8, 32x8, 32x16
    and 64x32. Frame 3 is left empty; the version number it used to hold is
    gone, and the C only ever creates three sprites.
    """
    src = load_flat(PRESS_SRC)
    px = src.load()
    x0, y0, x1, y1 = BOX_PRESS
    mask = drop_specks({(x, y) for y in range(y0, y1) for x in range(x0, x1)
                        if is_text(px[x, y])})
    press = {(x, y): px[x, y] for (x, y) in mask}
    if not press:
        sys.exit("PRESS START: nothing found in %s" % (BOX_PRESS,))

    tones = sorted(set(press.values()), key=sum)
    if len(tones) > 15:
        step = len(tones) / 15.0
        tones = [tones[int(i * step)] for i in range(15)]
    idx = {c: 1 + min(range(len(tones)),
                      key=lambda i: sum((a - b) ** 2 for a, b in zip(tones[i], c)))
           for c in set(press.values())}

    sheet = Image.new("P", (SPR_W, SPR_H * SPR_FRAMES), 0)
    sheet.putpalette([255, 0, 255] + [v for c in tones for v in c]
                     + [0] * (48 - 3 - 3 * len(tones)))
    dst = sheet.load()
    for (x, y), c in press.items():
        frame, fx = divmod(x - x0, SPR_W)
        fy = y - y0 + 2
        if 0 <= frame < 3 and 0 <= fx < SPR_W and 0 <= fy < SPR_H:
            dst[fx, frame * SPR_H + fy] = idx[c]

    sheet.save(os.path.join(OUT, "wot_title_text.png"))
    print("press start: %d px lifted, %d tones -> %dx%d sheet (blinks as 3 sprites)"
          % (len(press), len(tones), sheet.width, sheet.height))


# --- emitting the background --------------------------------------------------

def build_background(img):
    # NO DITHERING. Floyd-Steinberg scatters two colours in a chequer to fake a
    # third, which on this art turned the logo and the rocks into visible
    # speckle -- and speckle on a static screen reads as flicker. With 255
    # colours available the flat areas do not need faking; they just need to be
    # left alone.
    q = img.quantize(colors=255, method=Image.MEDIANCUT, dither=Image.NONE)
    # Shift every index up by one so slot 0 is free. Slot 0 of a BG palette is
    # the backdrop colour and is never drawn from, so leaving art in it would
    # lose a colour.
    src = q.load()
    pal = q.getpalette()[: 255 * 3]

    tiles, order, tilemap = {}, [], []
    for ty in range(ROWS):
        for tx in range(COLS):
            key = tuple(src[tx * TILE + x, ty * TILE + y] + 1
                        for y in range(TILE) for x in range(TILE))
            if key not in tiles:
                tiles[key] = len(order)
                order.append(key)
            tilemap.append(tiles[key])
    if len(order) > 1024:
        sys.exit("%d unique tiles -- over what the tilemap can index" % len(order))

    # Tileset image: 8 tiles per row, read back in the same order by gbagfx.
    tw = 8
    th = (len(order) + tw - 1) // tw
    sheet = Image.new("P", (tw * TILE, th * TILE), 0)
    sheet.putpalette([0, 0, 0] + pal + [0] * (768 - 3 - len(pal)))
    dst = sheet.load()
    for n, key in enumerate(order):
        ox, oy = (n % tw) * TILE, (n // tw) * TILE
        for y in range(TILE):
            for x in range(TILE):
                dst[ox + x, oy + y] = key[y * TILE + x]
    sheet.save(os.path.join(OUT, "wot_title_bg.png"))

    with open(os.path.join(OUT, "wot_title_bg.pal"), "w", newline="\r\n") as fh:
        fh.write("JASC-PAL\n0100\n256\n0 0 0\n")
        for i in range(255):
            fh.write("%d %d %d\n" % tuple(pal[i * 3:i * 3 + 3]))

    # 32x32 screen. Only 30x20 is on-screen; the rest stays tile 0.
    with open(os.path.join(OUT, "wot_title_bg.bin"), "wb") as fh:
        for ty in range(32):
            for tx in range(32):
                v = tilemap[ty * COLS + tx] if (tx < COLS and ty < ROWS) else 0
                fh.write(struct.pack("<H", v))

    print("background: %d unique tiles (%s bytes at 8bpp), tileset %dx%d"
          % (len(order), format(len(order) * 64, ","), sheet.width, sheet.height))


def main():
    frame = compose()
    frame.save(os.path.join(OUT, "wot_title_preview.png"))
    build_text_sheet()
    build_background(frame)


if __name__ == "__main__":
    main()
