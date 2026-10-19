# Make a Mega Stone icon in the Z-A stone shape (graphics/items/icons/meganiumite.png),
# recoloured with three colours picked from the Mega's own palette.
#   python -I tools/build_mega_stone_icon.py <StoneName> <mega normal.pal> <shell> <band> <accent>
# shell/band/accent are palette indices (1-15) into the Mega's normal.pal. Writes
# graphics/items/icons/<stone>.png and graphics/items/icon_palettes/<stone>.pal (run from the repo root).
# Template slots: 4/10/11 shell (main/light/shade), 9/5/1 band (main/light/dark),
# 2/6/12 accent (main/light/dark), 3 and 7 the thin dividing lines, 8 highlight, 13 outline.
import sys
from PIL import Image

name, palfile = sys.argv[1], sys.argv[2]
picks = [int(v) for v in sys.argv[3:6]]
mp = [tuple(map(int, l.split())) for l in open(palfile).read().split('\n')[3:] if l.strip()][:16]
shell, band, acc = (mp[i] for i in picks)
W, K = (255, 255, 255), (0, 0, 0)
def mix(a, b, t): return tuple(a[i] + (b[i] - a[i]) * t for i in range(3))
def q(c): return tuple(((max(0, min(255, int(round(v)))) >> 3) << 3) | (max(0, min(255, int(round(v)))) >> 5) for v in c)
pal = [(255, 0, 128)] + [K] * 15
pal[4] = shell;  pal[10] = mix(shell, W, 0.45); pal[11] = mix(mix(shell, acc, 0.3), K, 0.15)
pal[9] = band;   pal[5] = mix(band, W, 0.40);   pal[1] = mix(band, K, 0.35)
pal[2] = acc;    pal[6] = mix(acc, W, 0.40);    pal[12] = mix(acc, K, 0.35)
pal[3] = mix(shell, K, 0.45); pal[7] = mix(mix(shell, band, 0.5), K, 0.2)
pal[8] = (248, 248, 248); pal[13] = (49, 49, 49)
pal = [pal[0]] + [q(c) for c in pal[1:]]
n = name.lower()
icon = Image.open('graphics/items/icons/meganiumite.png').copy()
icon.putpalette([v for c in pal for v in c])
icon.save(f'graphics/items/icons/{n}.png', transparency=0)
with open(f'graphics/items/icon_palettes/{n}.pal', 'w', newline='\r\n') as f:
    f.write('JASC-PAL\n0100\n16\n' + ''.join(f'{c[0]} {c[1]} {c[2]}\n' for c in pal))
