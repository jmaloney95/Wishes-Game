# -*- coding: utf-8 -*-
"""Strip the checkerboard backdrop off the snow village sheet.

The artist marked "empty" with a 16x16 checkerboard alternating (224,216,208)
and (248,248,248), aligned to the metatile grid. Neither can be keyed by colour
alone: (248,248,248) is pure white, which is also snow, and the sheet has no
alpha channel at all to fall back on.

So a pixel is only treated as backdrop when BOTH hold:
  * it matches the colour its own cell is supposed to be, given the phase
    ((x//16 + y//16) even -> beige, odd -> white), and
  * it is reachable from the sheet border through other backdrop pixels.

The phase test alone would eat white snow sitting in a white cell; the flood
alone would eat any white that touches the border. Together, art enclosed by
its own outline is safe, which is what every object here has.
"""
import sys
from collections import deque

from PIL import Image

SRC = "J:/ROM Hack Project/tilesets_raw/snow_village_tiles_by_carchagui_dcgwwcb.png"
BEIGE = (224, 216, 208)
WHITE = (248, 248, 248)


def snap(c):
    return ((c[0] >> 3) << 3, (c[1] >> 3) << 3, (c[2] >> 3) << 3)


def strip(src=SRC, out=None, verbose=True):
    im = Image.open(src).convert("RGBA")
    W, H = im.size
    px = im.load()

    def is_backdrop(x, y):
        want = BEIGE if ((x // 16) + (y // 16)) % 2 == 0 else WHITE
        return snap(px[x, y][:3]) == want

    seen = bytearray(W * H)
    q = deque()
    for x in range(W):
        for y in (0, H - 1):
            if is_backdrop(x, y) and not seen[y * W + x]:
                seen[y * W + x] = 1
                q.append((x, y))
    for y in range(H):
        for x in (0, W - 1):
            if is_backdrop(x, y) and not seen[y * W + x]:
                seen[y * W + x] = 1
                q.append((x, y))

    while q:
        x, y = q.popleft()
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nx, ny = x + dx, y + dy
            if 0 <= nx < W and 0 <= ny < H and not seen[ny * W + nx] and is_backdrop(nx, ny):
                seen[ny * W + nx] = 1
                q.append((nx, ny))

    # what the phase test alone would have taken, for comparison
    phase_only = sum(1 for y in range(H) for x in range(W) if is_backdrop(x, y))
    keyed = sum(seen)

    for y in range(H):
        for x in range(W):
            if seen[y * W + x]:
                px[x, y] = (0, 0, 0, 0)

    if verbose:
        print("sheet %dx%d = %d px" % (W, H, W * H))
        print("  phase test alone would key : %d" % phase_only)
        print("  flood-connected, so keyed  : %d" % keyed)
        print("  SAVED by requiring connectivity: %d px of art-coloured pixels"
              % (phase_only - keyed))
        left = {}
        for y in range(H):
            for x in range(W):
                if not seen[y * W + x]:
                    c = snap(px[x, y][:3])
                    if c in (BEIGE, WHITE):
                        left[c] = left.get(c, 0) + 1
        print("  backdrop colours surviving inside art:", left)
    if out:
        im.save(out)
    return im


if __name__ == "__main__":
    im = strip(out=sys.argv[1] if len(sys.argv) > 1 else None)
