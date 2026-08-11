"""Render a built pokeemerald tileset (primary+secondary) metatile sheet to PNG for eyeballing."""
import sys, os, struct
from PIL import Image

ROOT = "J:/ROM Hack Project/pokeemerald-expansion/data/tilesets"

def load_pals(d):
    pals = []
    for i in range(16):
        p = os.path.join(d, "palettes", f"{i:02d}.pal")
        if not os.path.exists(p):
            pals.append([(0, 0, 0)] * 16); continue
        lines = open(p).read().split("\n")[3:]
        cols = []
        for ln in lines:
            ln = ln.strip()
            if not ln: continue
            r, g, b = (int(x) for x in ln.split())
            cols.append((r, g, b))
        cols += [(0, 0, 0)] * (16 - len(cols))
        pals.append(cols[:16])
    return pals

def load_tiles(d):
    """Return list of 64-length index lists."""
    png = os.path.join(d, "tiles.png")
    im = Image.open(png).convert("P")
    W, H = im.size
    px = im.load()
    tw = W // 8
    n = (W // 8) * (H // 8)
    out = []
    for t in range(n):
        ox, oy = (t % tw) * 8, (t // tw) * 8
        out.append([px[ox + x, oy + y] for y in range(8) for x in range(8)])
    return out

def render(primary, secondary, outpath, cols=8):
    pd = os.path.join(ROOT, "primary", primary)
    sd = os.path.join(ROOT, "secondary", secondary)
    ptiles, stiles = load_tiles(pd), load_tiles(sd)
    ppals, spals = load_pals(pd), load_pals(sd)
    pals = ppals[:6] + spals[6:]
    mb = open(os.path.join(sd, "metatiles.bin"), "rb").read()
    nm = len(mb) // 16
    rows = (nm + cols - 1) // cols
    img = Image.new("RGB", (cols * 16, rows * 16), (255, 0, 255))
    px = img.load()
    for m in range(nm):
        base = m * 16
        mx, my = (m % cols) * 16, (m // cols) * 16
        for layer in range(2):
            for q in range(4):
                v = struct.unpack("<H", mb[base + (layer * 4 + q) * 2: base + (layer * 4 + q) * 2 + 2])[0]
                tid = v & 0x3FF
                xf = (v >> 10) & 1; yf = (v >> 11) & 1; sl = (v >> 12) & 0xF
                if tid < 512:
                    tile = ptiles[tid] if tid < len(ptiles) else None
                else:
                    j = tid - 512
                    tile = stiles[j] if j < len(stiles) else None
                if tile is None: continue
                pal = pals[sl]
                qx, qy = (q % 2) * 8, (q // 2) * 8
                for y in range(8):
                    for x in range(8):
                        sx = 7 - x if xf else x
                        sy = 7 - y if yf else y
                        idx = tile[sy * 8 + sx]
                        if layer == 1 and idx == 0: continue
                        px[mx + qx + x, my + qy + y] = pal[idx]
    img.resize((img.width * 2, img.height * 2), Image.NEAREST).save(outpath)
    print(outpath, nm, "metatiles", img.size)

if __name__ == "__main__":
    render(sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]) if len(sys.argv) > 4 else 8)
