# Derive a shiny.pal for new Mega art from a reference normal/shiny palette pair.
# Each colour in the new normal.pal takes the normal->shiny shift of the nearest
# colour that the reference sprites actually use, so the Mega inherits the
# reference's shiny scheme. Reference = the stock Mega folder for a canon Mega,
# or the base species folder for a custom one.
#   python tools/build_mega_shiny.py <ref_dir> <mega_dir>
import sys, os
from PIL import Image

def rd(p): return [tuple(map(int, l.split())) for l in open(p).read().split('\n')[3:] if l.strip()][:16]
def dist(a, b):
    rm = (a[0]+b[0])/2; dr, dg, db = a[0]-b[0], a[1]-b[1], a[2]-b[2]
    return (2+rm/256)*dr*dr + 4*dg*dg + (2+(255-rm)/256)*db*db
def q(v): v = max(0, min(255, v)); return (v >> 3) << 3 | (v >> 5)

ref, mega = sys.argv[1], sys.argv[2]
sn, ss = rd(os.path.join(ref, 'normal.pal')), rd(os.path.join(ref, 'shiny.pal'))
used = set()
for k in ('front.png', 'anim_front.png', 'back.png'):
    p = os.path.join(ref, k)
    if os.path.exists(p): used |= set(Image.open(p).getdata())
used.discard(0)
used = [i for i in used if i < len(sn) and i < len(ss)]
newp = rd(os.path.join(mega, 'normal.pal'))
sh = [newp[0]]
for c in newp[1:]:
    i = min(used, key=lambda i: dist(c, sn[i]))
    sh.append(tuple(q(c[j] + ss[i][j] - sn[i][j]) for j in range(3)))
with open(os.path.join(mega, 'shiny.pal'), 'w', newline='\r\n') as f:
    f.write('JASC-PAL\n0100\n16\n' + ''.join(f'{c[0]} {c[1]} {c[2]}\n' for c in sh))
