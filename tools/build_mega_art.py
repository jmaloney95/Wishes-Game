# Convert 80x80 RGBA Mega art pairs to GBA 64x64 4bpp-ready indexed PNGs + JASC pal.
# Usage: python -I tools/build_mega_art.py <srcdir> <outdir>
# <srcdir> holds 'M-NAME.png' + 'M-NAME BACK.png' pairs; writes <outdir>/<species>/{front,back}.png + normal.pal,
# ready to copy into graphics/pokemon/<species>/mega/ (then run tools/build_mega_shiny.py for shiny.pal).
import sys, os
from PIL import Image
src, out = sys.argv[1], sys.argv[2]
FRONT_WEIGHT = 2

def q555(c): return tuple((v >> 3) << 3 | (v >> 5) for v in c[:3])

def dist(a, b):
    rm = (a[0] + b[0]) / 2
    dr, dg, db = a[0]-b[0], a[1]-b[1], a[2]-b[2]
    return (2 + rm/256)*dr*dr + 4*dg*dg + (2 + (255-rm)/256)*db*db

def premul_resize(im, size):
    r, g, b, a = im.split()
    from PIL import ImageChops
    pm = Image.merge('RGBA', [ImageChops.multiply(c, a) for c in (r, g, b)] + [a])
    pm = pm.resize(size, Image.LANCZOS)
    px = []
    for (R, G, B, A) in pm.getdata():
        if A == 0: px.append((0, 0, 0, 0))
        else: px.append((min(255, R*255//A), min(255, G*255//A), min(255, B*255//A), A))
    o = Image.new('RGBA', size); o.putdata(px); return o

def reduce(counts, n=15):
    cols = dict(counts)
    while len(cols) > n:
        keys = list(cols)
        best = None
        for i in range(len(keys)):
            for j in range(i+1, len(keys)):
                a, b = keys[i], keys[j]
                cost = dist(a, b) * min(cols[a], cols[b])
                if best is None or cost < best[0]: best = (cost, a, b)
        _, a, b = best
        keep, drop = (a, b) if cols[a] >= cols[b] else (b, a)
        cols[keep] += cols.pop(drop)
    return list(cols)

def nearest(c, pal): return min(range(len(pal)), key=lambda i: dist(c, pal[i]))

names = sorted(set(f[:-4].replace(' BACK', '').replace(' back', '') for f in os.listdir(src) if f.endswith('.png') and f != 'ALL.png'))
report = []
for n in names:
    fr = os.path.join(src, n + '.png')
    bk = [os.path.join(src, n + s + '.png') for s in (' BACK', ' back') if os.path.exists(os.path.join(src, n + s + '.png'))][0]
    imgs = {'front': Image.open(fr).convert('RGBA'), 'back': Image.open(bk).convert('RGBA')}
    counts = {}
    for k, im in imgs.items():
        w = FRONT_WEIGHT if k == 'front' else 1
        for p in im.getdata():
            if p[3] >= 128:
                c = q555(p); counts[c] = counts.get(c, 0) + w
    orig = len(counts)
    pal = reduce(counts)
    pal.sort(key=lambda c: -(0.299*c[0] + 0.587*c[1] + 0.114*c[2]))  # light -> dark
    bg = (152, 208, 160)
    while bg in pal: bg = (bg[0], bg[1], (bg[2] + 8) % 256)
    full = [bg] + pal + [(0, 0, 0)] * (15 - len(pal))
    # the pack's file names misspell two species
    key = {'amoongus': 'amoonguss', 'typlhosion': 'typhlosion'}.get(n[2:].lower(), n[2:].lower())
    od = os.path.join(out, key); os.makedirs(od, exist_ok=True)
    for k, im in imgs.items():
        sm = premul_resize(im, (64, 64))
        idx = [0 if p[3] < 128 else 1 + nearest(q555(p), pal) for p in sm.getdata()]
        o = Image.new('P', (64, 64)); o.putpalette([v for c in full for v in c]); o.putdata(idx)
        o.save(os.path.join(od, k + '.png'), transparency=0)
    with open(os.path.join(od, 'normal.pal'), 'w', newline='\r\n') as f:
        f.write('JASC-PAL\n0100\n16\n' + ''.join(f'{c[0]} {c[1]} {c[2]}\n' for c in full))
    report.append(f'{key:12s} colors {orig:2d} -> {len(pal)}')
print('\n'.join(report))
