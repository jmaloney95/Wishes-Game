# Render in-game mockups of every Mega from the repo's real assets:
# battle background (BG3 tiles/tilemap/palette), battle textbox (BG0), the Gen-4 enemy
# shadow, the Mega's front (enemy side) and back (player side) at the engine's own
# coordinates, plus party icon and Mega Stone icon. Values come from the species_info
# source so the picture matches what the build will draw.
#   python -I tools/render_battle_mockup.py <repo> <outdir> SPECIES_KEY,... StoneName,...
#   e.g. ... out AMOONGUSS_MEGA,MEGANIUM_MEGA Amoongussite,Meganiumite
import os, re, glob, sys, json, struct
from PIL import Image

R, OUT = sys.argv[1], sys.argv[2]
os.makedirs(OUT, exist_ok=True)

def jasc(p): return [tuple(map(int, l.split())) for l in open(p).read().split('\n')[3:] if l.strip()]
def gbapal(p):
    raw = open(p, 'rb').read()
    out = []
    for (v,) in struct.iter_unpack('<H', raw):
        r, g, b = v & 31, (v >> 5) & 31, (v >> 10) & 31
        out.append((r << 3 | r >> 2, g << 3 | g >> 2, b << 3 | b >> 2))
    return out
def c555(c): return tuple((v >> 3) << 3 | (v >> 3) >> 2 for v in c)   # what the GBA actually shows

def tiles_of(png):
    im = Image.open(png); w, h = im.size; px = im.load()
    return [[[px[tx * 8 + x, ty * 8 + y] for x in range(8)] for y in range(8)] for ty in range(h // 8) for tx in range(w // 8)]

def draw_bg(canvas, tiles, mapbin, palfn, rows=20, cols=30):
    raw = open(mapbin, 'rb').read()
    ents = [v for (v,) in struct.iter_unpack('<H', raw)]
    px = canvas.load()
    for ty in range(rows):
        for tx in range(cols):
            e = ents[ty * 32 + tx]
            t = tiles[e & 0x3FF] if (e & 0x3FF) < len(tiles) else None
            if t is None: continue
            hf, vf, bank = (e >> 10) & 1, (e >> 11) & 1, e >> 12
            for y in range(8):
                for x in range(8):
                    i = t[7 - y if vf else y][7 - x if hf else x]
                    if i:
                        px[tx * 8 + x, ty * 8 + y] = palfn(bank, i)

def sprite(png, pal):
    im = Image.open(png)
    if im.size[1] > 64: im = im.crop((0, 0, 64, 64))
    rgba = Image.new('RGBA', im.size, (0, 0, 0, 0)); p = rgba.load(); s = im.load()
    for y in range(im.size[1]):
        for x in range(im.size[0]):
            i = s[x, y]
            if i: p[x, y] = c555(pal[i]) + (255,)
    return rgba

env_tiles = tiles_of(f'{R}/graphics/battle_environment/tall_grass/tiles.png')
env_pal = jasc(f'{R}/graphics/battle_environment/tall_grass/palette.pal')
tb_tiles = tiles_of(f'{R}/graphics/battle_interface/textbox.png')
tb_pal = gbapal(f'{R}/graphics/battle_interface/textbox.gbapal')
shadow_sheet = Image.open(f'{R}/graphics/battle_interface/enemy_mon_shadows_sized.png')
shadow_pal = Image.open(f'{R}/graphics/battle_interface/ball_status_bar.png').getpalette()
shadow_pal = [tuple(shadow_pal[i * 3:i * 3 + 3]) for i in range(16)]
icon_pals = [jasc(f'{R}/graphics/pokemon/icon_palettes/pal{i}.pal') for i in range(6)]

def background():
    c = Image.new('RGB', (240, 160), c555(tb_pal[0]))
    draw_bg(c, env_tiles, f'{R}/graphics/battle_environment/tall_grass/map.bin', lambda b, i: c555(env_pal[(b - 2) * 16 + i]))
    return c
BG = background()
def textbox(canvas):
    draw_bg(canvas, tb_tiles, f'{R}/graphics/battle_interface/textbox_map.bin', lambda b, i: c555(tb_pal[b * 16 + i]))

# ---- species values straight from the source
src = ''.join(open(f, encoding='utf-8').read() for f in sorted(glob.glob(f'{R}/src/data/pokemon/species_info/*.h')))
gfx = open(f'{R}/src/data/graphics/pokemon.h', encoding='utf-8').read()
def entry(key):
    m = re.search(r'\[SPECIES_%s\] =\n    \{\n(.*?)\n    \},' % key, src, re.S)
    return m.group(1)
def f(b, k, default=None):
    m = re.search(r'^        \.%s = (.*?),$' % k, b, re.M)
    return m.group(1) if m else default
def num(expr):  # take the non-GBA branch of "P_GBA_STYLE_... ? a : b"
    expr = str(expr)
    return int(expr.split(':')[-1].strip()) if '?' in expr else int(expr)
def gpath(sym):
    m = re.search(r'%s\[\] = INCGFX_\w+\("([^"]+)"' % re.escape(sym), gfx)
    return f'{R}/{m.group(1)}'

def render(key):
    b = entry(key)
    front = gpath(f(b, 'frontPic')); back = gpath(f(b, 'backPic'))
    pal_n = jasc(gpath(f(b, 'palette'))); pal_s = jasc(gpath(f(b, 'shinyPalette')))
    off = num(f(b, 'frontPicYOffset', 0)); boff = num(f(b, 'backPicYOffset', 0)); elev = num(f(b, 'enemyMonElevation', 0))
    sh = re.search(r'SHADOW\((-?\d+), (-?\d+), (SHADOW_SIZE_\w+)\)', b)
    shx, shy = int(sh.group(1)), int(sh.group(2))
    size = {'SHADOW_SIZE_S': 0, 'SHADOW_SIZE_M': 1, 'SHADOW_SIZE_L': 2, 'SHADOW_SIZE_XL_BATTLE_ONLY': 3}[sh.group(3)]
    res = {}
    for tag, pal in (('normal', pal_n), ('shiny', pal_s)):
        c = BG.copy().convert('RGBA')
        ey = 40 + off - elev
        band = shadow_sheet.crop((0, size * 8, 64, size * 8 + 8))
        sp = Image.new('RGBA', (64, 8), (0, 0, 0, 0)); bp = band.load(); spp = sp.load()
        for y in range(8):
            for x in range(64):
                if bp[x, y]: spp[x, y] = c555(shadow_pal[bp[x, y]]) + (255,)
        c.alpha_composite(sp, (176 + shx - 32, ey + shy + 16 - 4)) if 0 <= ey + shy + 12 < 160 else None
        fs = sprite(front, pal)
        _paste(c, fs, 144, ey - 32)
        bs = sprite(back, pal)
        _paste(c, bs, 40, 48 + boff)
        c = c.convert('RGB'); textbox(c)
        c.save(f'{OUT}/{key.lower()}_battle_{tag}.png')
        fs.save(f'{OUT}/{key.lower()}_front_{tag}.png'); bs.save(f'{OUT}/{key.lower()}_back_{tag}.png')
    # party icon, first frame
    ip = num(f(b, 'iconPalIndex'))
    ic = sprite(gpath(f(b, 'iconSprite')), icon_pals[ip]).crop((0, 0, 32, 32))
    ic.save(f'{OUT}/{key.lower()}_icon.png')
    return dict(off=off, boff=boff, elev=elev, shadow=[shx, shy, sh.group(3)], icon=f(b, 'iconSprite'))

def _paste(c, s, x, y):
    # clip to the screen like the hardware does
    c.alpha_composite(s, (max(x, 0), max(y, 0)), (max(-x, 0), max(-y, 0)))

def stone(name):
    n = name.lower()
    pal = jasc(f'{R}/graphics/items/icon_palettes/{n}.pal')
    sprite(f'{R}/graphics/items/icons/{n}.png', pal).save(f'{OUT}/stone_{n}.png')

if __name__ == '__main__':
    keys = sys.argv[3].split(',')
    stones = sys.argv[4].split(',')
    meta = {k: render(k) for k in keys}
    for s in stones: stone(s)
    json.dump(meta, open(f'{OUT}/meta.json', 'w'), indent=1)
    print('rendered', len(keys), 'species,', len(stones), 'stones')
