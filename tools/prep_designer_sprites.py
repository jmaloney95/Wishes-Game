# -*- coding: utf-8 -*-
"""Designer sprite drop v2 -- fidelity-first reprocessing.
Fixes vs v1: exact palettes when <=15 colors (v1's median-cut ate the Mutrid
leader's red eyes/orb); accent-preserving quantization otherwise (farthest-
point x frequency keeps small distinct hues); lossless NEAREST /2 for true 2x
art; palette-snap after unavoidable downscales (>62px content); leader golds
brightened per Joe; Mewtwo back flush to the textbox; red Mewtwo OW; Jirachi
galaxy front; white-grunt OW recolored from the red walk cycle.
"""
from PIL import Image
import numpy as np, os, colorsys
from collections import Counter

CS = r"J:\ROM Hack Project\custom sprites"
GP = r"J:\ROM Hack Project\pokeemerald-expansion\graphics"
FP = os.path.join(GP, "trainers", "front_pics")

def load(path):
    im = Image.open(path)
    if getattr(im, "is_animated", False):
        im.seek(0)
    return im.convert("RGBA")

def key_bg(a, c=None, tol=35):
    if c is None:
        corners = [tuple(a[0,0][:3]), tuple(a[0,-1][:3]), tuple(a[-1,0][:3]), tuple(a[-1,-1][:3])]
        c = Counter(corners).most_common(1)[0][0]
    m = (abs(a[...,0].astype(int)-c[0])<tol)&(abs(a[...,1].astype(int)-c[1])<tol)&(abs(a[...,2].astype(int)-c[2])<tol)
    a = a.copy(); a[m] = 0
    return a

def bbox(a):
    m = a[...,3] > 8
    ys, xs = np.where(m)
    return a[ys.min():ys.max()+1, xs.min():xs.max()+1]

def is_2x(a):
    h, w = a.shape[:2]
    c = a[:(h//2)*2, :(w//2)*2, :3]
    u = np.mean((c[0::2,0::2]==c[0::2,1::2]).all(-1) & (c[0::2,0::2]==c[1::2,0::2]).all(-1) & (c[0::2,0::2]==c[1::2,1::2]).all(-1))
    return u > 0.97

def snap_palette(a, pal):
    """map every opaque pixel to nearest color in pal (kills resample mush)"""
    op = a[...,3] > 128
    px = a[op][:,:3].astype(int)
    cand = np.array(pal, dtype=int)
    d = ((px[:,None,:]-cand[None,:,:])**2).sum(2)
    a = a.copy()
    a[op] = np.concatenate([cand[d.argmin(1)], np.full((len(px),1),255,int)], axis=1).astype(np.uint8)
    a[~op] = 0
    return a

def accent_quantize_pal(a, k=15):
    """exact colors if <=k; else farthest-point x frequency selection."""
    op = a[...,3] > 128
    cnt = Counter(tuple(p) for p in a[op][:,:3])
    cols = list(cnt.keys())
    if len(cols) <= k:
        return cols
    kept = [max(cols, key=lambda c: cnt[c])]
    arr = np.array(cols, dtype=int)
    while len(kept) < k:
        ka = np.array(kept, dtype=int)
        dmin = ((arr[:,None,:]-ka[None,:,:])**2).sum(2).min(1).astype(float)
        w = np.array([np.log2(cnt[tuple(c)]+2) for c in cols])
        pick = int((dmin*w).argmax())
        kept.append(cols[pick])
    return kept

def to_indexed(a, pal):
    op = a[...,3] > 128
    cand = np.array(pal, dtype=int)
    idx = np.zeros(a.shape[:2], np.uint8)
    ys, xs = np.where(op)
    d = ((a[ys,xs][:,:3].astype(int)[:,None,:]-cand[None,:,:])**2).sum(2)
    idx[ys,xs] = d.argmin(1)+1
    return idx

def save_4bit(idx, pal, path, bg=(152,208,160)):
    im = Image.new("P", (idx.shape[1], idx.shape[0]))
    im.putdata(idx.flatten().tolist())
    full = [bg] + list(pal) + [(0,0,0)]*(15-len(pal))
    im.putpalette([v for c in full for v in c], rawmode="RGB")
    im.save(path, bits=4)

def write_jasc(path, colors16):
    with open(path, "wb") as f:
        f.write(b"JASC-PAL\r\n0100\r\n16\r\n")
        for c in colors16:
            f.write(("%d %d %d\r\n" % tuple(c[:3])).encode())

def prep_sprite(a, W=64, H=64, margin=1):
    """key must be done. Crop; NEAREST /2 if true 2x. NEVER resample art that
    already fits WxH -- the margin shrinks to fit instead (a 64->63 LANCZOS
    pass blurred away single-pixel details like the leader's red eyes)."""
    s = bbox(a)
    if is_2x(s):
        im = Image.fromarray(s)
        s = np.array(im.resize((s.shape[1]//2, s.shape[0]//2), Image.NEAREST))
    src_pal = list({tuple(p) for p in s[s[...,3]>128][:,:3]})
    h, w = s.shape[:2]
    if w > W or h > H:
        f = min(W/w, H/h)
        im = Image.fromarray(s).resize((max(1,int(w*f)), max(1,int(h*f))), Image.LANCZOS)
        s = snap_palette(np.array(im), src_pal)
        h, w = s.shape[:2]
    eff = min(margin, H-h)
    out = np.zeros((H, W, 4), np.uint8)
    out[H-eff-h:H-eff, (W-w)//2:(W-w)//2+w] = s
    return out

def brighten_golds(a, factor=1.18):
    a = a.copy()
    op = a[...,3] > 128
    ys, xs = np.where(op)
    for y, x in zip(ys, xs):
        r, g, b = a[y,x,:3].astype(float)/255
        h, s, v = colorsys.rgb_to_hsv(r, g, b)
        if 0.07 <= h <= 0.17 and s > 0.3 and v > 0.25:   # golds
            v = min(1.0, v*factor); s = max(0, s*0.97)
            r, g, b = colorsys.hsv_to_rgb(h, s, v)
            a[y,x,:3] = [int(r*255), int(g*255), int(b*255)]
    return a

def trainer_pic(src, dst, keycol=None, tol=35, gold=False, margin=1):
    a = key_bg(np.array(load(src)), keycol, tol)
    if gold:
        a = brighten_golds(a)
    f = prep_sprite(a, margin=margin)
    pal = accent_quantize_pal(f)
    f = snap_palette(f, pal)
    idx = to_indexed(f, pal)
    save_4bit(idx, pal, dst)
    # red survival check
    reds = sum(1 for c in pal if c[0] > 120 and c[0] > c[1]*1.6 and c[0] > c[2]*1.6)
    print(f"{os.path.basename(dst):26s} {len(pal)+1:2d} colors, red-hue entries: {reds}")

TS = os.path.join(CS, "Trainer Sprites")
trainer_pic(os.path.join(TS,"hilda_bikini_sprite_by_flamejow_d4cuqvd.png"), os.path.join(FP,"swimmer_allison.png"))
trainer_pic(os.path.join(TS,"hilda_bikini_sprite_2_by_flamejow_d4cur18.png"), os.path.join(FP,"swimmer_skye.png"))
trainer_pic(os.path.join(TS,"_dream_girl__v2___og_pokemon_gen_3_trainer_sprite_by_ginzhio_dex6crd.gif"), os.path.join(FP,"swimmer_rosa.png"))
trainer_pic(os.path.join(TS,"cynthia_in_a_bikini_by_shaddyshad_d3dx9n7.png"), os.path.join(FP,"swimmer_marina.png"))
trainer_pic(os.path.join(TS,"lifeguard_trainer_sprite_by_acarreras_djqs48e.png"), os.path.join(FP,"swimmer_f.png"))
trainer_pic(os.path.join(TS,"poke_kid__eevee__by_ulithiumdragon_ddkpus7.png"), os.path.join(FP,"tuber_f.png"))
trainer_pic(os.path.join(TS,"you_re_challenged_by_swimmers_reina_and_vanessa_by_pandachick700_da3ssdd.png"), os.path.join(FP,"sr_and_jr.png"))

MG = os.path.join(CS, "Mutrid Grunt")
trainer_pic(os.path.join(MG,"battle sprite red.png"), os.path.join(FP,"mutrid_grunt.png"))
trainer_pic(os.path.join(MG,"battle sprite white.png"), os.path.join(FP,"mutrid_grunt_white.png"))
trainer_pic(os.path.join(MG,"battle sprite cat girl.png"), os.path.join(FP,"mutrid_grunt_f.png"))
trainer_pic(os.path.join(CS,"Mutrid Leader","battle sprite.png"), os.path.join(FP,"mutrid_leader.png"), gold=True)

# ---------- species ----------
def save_p(ix, full, path):
    im = Image.new("P", (ix.shape[1], ix.shape[0]))
    im.putdata(ix.flatten().tolist())
    im.putpalette([v for c in full for v in c])
    im.save(path)

def species_pair(front, back, outdir, anim2, front_margin=4, back_margin=0, shiny_same=True):
    f64 = prep_sprite(front, 64, 64, margin=front_margin)
    b64 = prep_sprite(back, 64, 64, margin=back_margin)
    combo = np.vstack([f64, b64])
    pal = accent_quantize_pal(combo)
    f64 = snap_palette(f64, pal); b64 = snap_palette(b64, pal)
    fi = to_indexed(f64, pal); bi = to_indexed(b64, pal)
    full = [(255,0,255)] + list(pal) + [(0,0,0)]*(15-len(pal))
    save_p(np.vstack([fi,fi]) if anim2 else fi, full, os.path.join(outdir, "anim_front.png" if anim2 else "front.png"))
    save_p(bi, full, os.path.join(outdir, "back.png"))
    write_jasc(os.path.join(outdir,"normal.pal"), full)
    if shiny_same and os.path.exists(os.path.join(outdir,"shiny.pal")):
        write_jasc(os.path.join(outdir,"shiny.pal"), full)
    print(os.path.basename(outdir), f"front/back: {len(pal)+1} colors")

# Mewtwo: same fb sheet, back flush to bottom (fixes 'floats above textbox')
MD = os.path.join(GP,"pokemon","mewtwo_armored")
fb = key_bg(np.array(load(os.path.join(CS,"Armored Mewtwo","armored_mewtwo_front_back.png"))), (118,225,60))
species_pair(fb[:, :64], fb[:, 75:139], MD, anim2=False, shiny_same=False)
# red overworld variant
ow = key_bg(np.array(load(os.path.join(CS,"Armored Mewtwo","armored_mewtwo_overworld_red.png"))), (118,225,60))
assert ow.shape[:2] == (32,192), ow.shape
opal = accent_quantize_pal(ow)
ow = snap_palette(ow, opal)
oidx = to_indexed(ow, opal)
fullo = [(255,0,255)] + list(opal) + [(0,0,0)]*(15-len(opal))
save_p(oidx, fullo, os.path.join(MD,"overworld.png"))
write_jasc(os.path.join(MD,"overworld_normal.pal"), fullo)
write_jasc(os.path.join(MD,"overworld_shiny.pal"), fullo)
print("mewtwo_armored overworld = RED variant")

# Jirachi: galaxy alt as FRONT, back from the fb sheet
JD = os.path.join(GP,"pokemon","jirachi_shadow")
gal = key_bg(np.array(load(os.path.join(CS,"Shadow Jirachi","jirachi galaxy alt.png"))))
jfb = key_bg(np.array(load(os.path.join(CS,"Shadow Jirachi","jirachi battle sprite.png"))), (36,216,0))
species_pair(gal, jfb[64:], JD, anim2=True, front_margin=4, back_margin=0, shiny_same=True)

# ---------- white grunt OW: recolor the red walk cycle ----------
def recolor_red_to_white(a):
    a = a.copy()
    op = a[...,3] > 128
    ys, xs = np.where(op)
    for y, x in zip(ys, xs):
        r, g, b = a[y,x,:3].astype(float)/255
        h, s, v = colorsys.rgb_to_hsv(r, g, b)
        hdeg = h*360
        if s > 0.30 and (hdeg <= 25 or hdeg >= 335):     # reds -> white cloth
            nv = min(1.0, 0.45 + v*0.62)                  # lift toward white
            r2, g2, b2 = colorsys.hsv_to_rgb(0.083, 0.06, nv)  # near-neutral warm
            a[y,x,:3] = [int(r2*255), int(g2*255), int(b2*255)]
    return a

OP = os.path.join(GP,"object_events")
red_ow = key_bg(np.array(load(os.path.join(MG,"overworld red.png"))), (153,229,80))
assert red_ow.shape[:2] == (32,144)
white_ow = recolor_red_to_white(red_ow)
for name, art in (("oni_goon", red_ow), ("mutrid_grunt_white", white_ow)):
    pal = accent_quantize_pal(art)
    art2 = snap_palette(art, pal)
    idx = to_indexed(art2, pal)
    full = [(115,197,164)] + list(pal) + [(0,0,0)]*(15-len(pal))
    save_p(idx, full, os.path.join(OP,"pics","people",name+".png"))
    write_jasc(os.path.join(OP,"palettes",name+".pal"), full)
    print("OW:", name, f"{len(pal)+1} colors")
print("done")
