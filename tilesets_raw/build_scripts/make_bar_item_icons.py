"""Item icons for the Frostwood bar's drinks, cut from gTileset_FrostwoodBar.

    Domestic  <- the beer mug   (metatile 0x248, top layer, palette 9)
    Craft     <- the beer bottle (metatile 0x21D, top layer, palette 12)

The top four metatile entries hold the drink on transparency (the bar counter
is the bottom layer), so the art is taken as-is: tile pixels keep their
indices, the tileset palette becomes the icon palette, index 0 stays
transparent. The art is cropped to its bounding box, pixel-doubled (at 1x a
10x10 mug reads tiny next to vanilla icons) and centred on the 24x24 canvas.
"""
import os, struct
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
G = os.path.normpath(os.path.join(HERE, "..", "..", "pokeemerald-expansion"))
BAR = os.path.join(G, "data/tilesets/secondary/frostwood_bar")

ICONS = [("domestic", 0x248, 9), ("craft", 0x21D, 12)]
SCALE = 2


def read_pal(i):
    lines = open(os.path.join(BAR, "palettes", "%02d.pal" % i)).read().split("\n")[3:]
    return [tuple(int(v) for v in l.split()) for l in lines if l.strip()][:16]


def main():
    tiles_im = Image.open(os.path.join(BAR, "tiles.png"))
    tpx = tiles_im.load()
    tw = tiles_im.size[0] // 8
    meta = open(os.path.join(BAR, "metatiles.bin"), "rb").read()

    for name, mid, want_pal in ICONS:
        entries = struct.unpack_from("<8H", meta, (mid - 512) * 16)
        art = [[0] * 16 for _ in range(16)]
        for q, v in enumerate(entries[4:]):
            tid, pal = v & 0x3FF, v >> 12
            if tid < 512:
                continue  # blank primary tile
            assert pal == want_pal, (name, hex(v))
            t = tid - 512
            hf, vf = (v >> 10) & 1, (v >> 11) & 1
            for y in range(8):
                for x in range(8):
                    sx, sy = (7 - x if hf else x), (7 - y if vf else y)
                    c = tpx[(t % tw) * 8 + sx, (t // tw) * 8 + sy] & 15
                    art[(q // 2) * 8 + y][(q % 2) * 8 + x] = c
        xs = [x for y in range(16) for x in range(16) if art[y][x]]
        ys = [y for y in range(16) for x in range(16) if art[y][x]]
        x0, x1, y0, y1 = min(xs), max(xs), min(ys), max(ys)
        w, h = (x1 - x0 + 1) * SCALE, (y1 - y0 + 1) * SCALE
        ox, oy = (24 - w) // 2, (24 - h) // 2

        icon = Image.new("P", (24, 24), 0)
        pal = read_pal(want_pal)
        flat = []
        for c in pal:
            flat += list(c)
        icon.putpalette(flat + [0] * (768 - len(flat)))
        ipx = icon.load()
        for y in range(h):
            for x in range(w):
                ipx[ox + x, oy + y] = art[y0 + y // SCALE][x0 + x // SCALE]
        icon.save(os.path.join(G, "graphics/items/icons/%s.png" % name))
        with open(os.path.join(G, "graphics/items/icon_palettes/%s.pal" % name), "w", newline="\r\n") as f:
            f.write("JASC-PAL\n0100\n16\n")
            for c in pal:
                f.write("%d %d %d\n" % c)
        print(name, "art %dx%d at (%d,%d)" % (w, h, ox, oy))


main()
