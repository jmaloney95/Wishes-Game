"""Overlay a labelled 16x16-metatile grid on a source sheet so regions can be picked by (row,col)."""
import sys, os
from PIL import Image, ImageDraw

def gridify(src, out, half=False, zoom=3, rows_per_chunk=24, label_every=1):
    im = Image.open(src).convert("RGBA")
    if half:
        im = im.resize((im.width // 2, im.height // 2), Image.NEAREST)
    W, H = im.size
    cols, rows = W // 16, H // 16
    print(f"{os.path.basename(src)}: {W}x{H} -> {cols} cols x {rows} rows of 16px metatiles")
    n = (rows + rows_per_chunk - 1) // rows_per_chunk
    base = os.path.splitext(out)[0]
    for i in range(n):
        r0 = i * rows_per_chunk
        r1 = min(rows, r0 + rows_per_chunk)
        crop = im.crop((0, r0 * 16, W, r1 * 16))
        # checkerboard behind transparency
        bg = Image.new("RGBA", crop.size, (255, 255, 255, 255))
        d0 = ImageDraw.Draw(bg)
        for yy in range(0, crop.height, 8):
            for xx in range(0, crop.width, 8):
                if (xx // 8 + yy // 8) % 2:
                    d0.rectangle([xx, yy, xx + 7, yy + 7], fill=(215, 215, 215, 255))
        bg.alpha_composite(crop)
        big = bg.resize((crop.width * zoom, crop.height * zoom), Image.NEAREST)
        pad = 26 * zoom // 3
        canvas = Image.new("RGBA", (big.width + pad, big.height + pad), (255, 255, 255, 255))
        canvas.paste(big, (pad, pad))
        d = ImageDraw.Draw(canvas)
        for c in range(cols + 1):
            x = pad + c * 16 * zoom
            d.line([(x, pad), (x, canvas.height)], fill=(255, 0, 0, 160), width=1)
        for r in range(r1 - r0 + 1):
            y = pad + r * 16 * zoom
            d.line([(pad, y), (canvas.width, y)], fill=(255, 0, 0, 160), width=1)
        for c in range(cols):
            d.text((pad + c * 16 * zoom + 4, 2), str(c), fill=(0, 0, 200, 255))
        for r in range(r1 - r0):
            d.text((2, pad + r * 16 * zoom + 4), str(r0 + r), fill=(0, 128, 0, 255))
        p = f"{base}_{i}.png"
        canvas.save(p)
        print("  ", p, f"rows {r0}-{r1-1}")

if __name__ == "__main__":
    gridify(sys.argv[1], sys.argv[2],
            half=(len(sys.argv) > 3 and sys.argv[3] == "half"),
            zoom=int(sys.argv[4]) if len(sys.argv) > 4 else 3,
            rows_per_chunk=int(sys.argv[5]) if len(sys.argv) > 5 else 24)
