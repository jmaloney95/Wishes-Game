# -*- coding: utf-8 -*-
"""Process the 2026-07-16 designer sprite drop into GBA assets.
- 7 female beach/swimmer trainer pics -> overwrite existing front_pics
- 3 Mutrid grunt battle pics (new) + Mutrid leader battle pic (overwrite)
- Armored Mewtwo front/back reprocess + NEW icon (stock icon palette)
- Shadow Jirachi new fb -> anim_front/back/pals + NEW icon + overworld
- Grunt OW (red) -> oni_goon.png/.pal ; Leader OW alt -> gold_oni.png/.pal
Trainer pics: 64x64, 4-bit indexed, index0 = transparent bg, <=16 colors.
"""
from PIL import Image
import numpy as np, os

CS = r"J:\ROM Hack Project\custom sprites"
GP = r"J:\ROM Hack Project\pokeemerald-expansion\graphics"

def load(path):
    im = Image.open(path)
    if getattr(im, "is_animated", False):
        im.seek(0)
    return im.convert("RGBA")

def key_bg(a, c=None, tol=35):
    """corner-color chroma key -> alpha 0"""
    if c is None:
        # most common corner color
        from collections import Counter
        corners = [tuple(a[0,0][:3]), tuple(a[0,-1][:3]), tuple(a[-1,0][:3]), tuple(a[-1,-1][:3])]
        c = Counter(corners).most_common(1)[0][0]
    m = (abs(a[...,0].astype(int)-c[0])<tol)&(abs(a[...,1].astype(int)-c[1])<tol)&(abs(a[...,2].astype(int)-c[2])<tol)
    a = a.copy(); a[m] = 0
    return a

def bbox(a):
    m = a[...,3] > 8
    ys, xs = np.where(m)
    return a[ys.min():ys.max()+1, xs.min():xs.max()+1]

def fit(a, W=64, H=64, bottom_margin=2):
    s = bbox(a)
    h, w = s.shape[:2]
    if w > W or h > H-bottom_margin:
        f = min(W/w, (H-bottom_margin)/h)
        im = Image.fromarray(s).resize((max(1,int(w*f)), max(1,int(h*f))), Image.LANCZOS)
        s = np.array(im); h, w = s.shape[:2]
    out = np.zeros((H, W, 4), np.uint8)
    ox = (W-w)//2; oy = H-h-bottom_margin
    out[oy:oy+h, ox:ox+w] = s
    return out

def quantize(a, maxc=15):
    op = a[...,3] > 128
    cols = {tuple(p) for p in a[op][:,:3]}
    if len(cols) > maxc:
        flat = Image.fromarray((a[...,:3]*op[...,None]).astype(np.uint8))
        q = np.array(flat.quantize(colors=maxc, method=Image.MEDIANCUT).convert("RGB"))
    else:
        q = a[...,:3]
    pal = sorted({tuple(p) for p in q[op]}, key=lambda c: 0.299*c[0]+0.587*c[1]+0.114*c[2])[:maxc]
    lut = {c:i+1 for i,c in enumerate(pal)}
    idx = np.zeros(a.shape[:2], np.uint8)
    ys, xs = np.where(op)
    for y,x in zip(ys,xs):
        c = tuple(q[y,x])
        if c not in lut:
            c = min(pal, key=lambda p: sum((int(p[k])-int(c[k]))**2 for k in range(3)))
        idx[y,x] = lut[tuple(c)]
    return idx, pal

def save_4bit(idx, pal, path, bg=(152,208,160)):
    im = Image.new("P", (idx.shape[1], idx.shape[0]))
    im.putdata(idx.flatten().tolist())
    full = [bg] + list(pal) + [(0,0,0)]*(15-len(pal))
    im.putpalette([v for c in full for v in c], rawmode="RGB")
    # 4-bit indexed save
    im.save(path, bits=4)

def write_jasc(path, colors16):
    with open(path, "wb") as f:
        f.write(b"JASC-PAL\r\n0100\r\n16\r\n")
        for c in colors16:
            f.write(("%d %d %d\r\n" % tuple(c[:3])).encode())

def trainer_pic(src, dst, keycol=None, tol=35):
    a = key_bg(np.array(load(src)), keycol, tol)
    f = fit(a)
    idx, pal = quantize(f)
    save_4bit(idx, pal, dst)
    print("trainer pic:", os.path.basename(dst), f"{len(pal)+1} colors")

FP = os.path.join(GP, "trainers", "front_pics")

# ---- 7 beach/swimmer trainers -> existing slots ----
TS = os.path.join(CS, "Trainer Sprites")
trainer_pic(os.path.join(TS,"hilda_bikini_sprite_by_flamejow_d4cuqvd.png"), os.path.join(FP,"swimmer_allison.png"))
trainer_pic(os.path.join(TS,"hilda_bikini_sprite_2_by_flamejow_d4cur18.png"), os.path.join(FP,"swimmer_skye.png"))
trainer_pic(os.path.join(TS,"_dream_girl__v2___og_pokemon_gen_3_trainer_sprite_by_ginzhio_dex6crd.gif"), os.path.join(FP,"swimmer_rosa.png"))
trainer_pic(os.path.join(TS,"cynthia_in_a_bikini_by_shaddyshad_d3dx9n7.png"), os.path.join(FP,"swimmer_marina.png"))
trainer_pic(os.path.join(TS,"lifeguard_trainer_sprite_by_acarreras_djqs48e.png"), os.path.join(FP,"swimmer_f.png"))
trainer_pic(os.path.join(TS,"poke_kid__eevee__by_ulithiumdragon_ddkpus7.png"), os.path.join(FP,"tuber_f.png"))
trainer_pic(os.path.join(TS,"you_re_challenged_by_swimmers_reina_and_vanessa_by_pandachick700_da3ssdd.png"), os.path.join(FP,"sr_and_jr.png"))

# ---- Mutrid battle pics ----
MG = os.path.join(CS, "Mutrid Grunt")
trainer_pic(os.path.join(MG,"battle sprite red.png"), os.path.join(FP,"mutrid_grunt.png"))
trainer_pic(os.path.join(MG,"battle sprite white.png"), os.path.join(FP,"mutrid_grunt_white.png"))
trainer_pic(os.path.join(MG,"battle sprite cat girl.png"), os.path.join(FP,"mutrid_grunt_f.png"))
trainer_pic(os.path.join(CS,"Mutrid Leader","battle sprite.png"), os.path.join(FP,"mutrid_leader.png"))

# ---- species helpers ----
def species_pair(front_rgba, back_rgba, outdir, anim2=False, shiny_same=True):
    f64 = fit(front_rgba, 64, 64, 4)
    b64 = fit(back_rgba, 64, 64, 4)
    combo = np.vstack([f64, b64])
    idx, pal = quantize(combo)
    fi, bi = idx[:64], idx[64:]
    full = [(255,0,255)] + list(pal) + [(0,0,0)]*(15-len(pal))
    def save_p(ix, path):
        im = Image.new("P", (ix.shape[1], ix.shape[0]))
        im.putdata(ix.flatten().tolist())
        im.putpalette([v for c in full for v in c])
        im.save(path)
    if anim2:
        save_p(np.vstack([fi, fi]), os.path.join(outdir, "anim_front.png"))
    else:
        save_p(fi, os.path.join(outdir, "front.png"))
    save_p(bi, os.path.join(outdir, "back.png"))
    write_jasc(os.path.join(outdir, "normal.pal"), full)
    if shiny_same and os.path.exists(os.path.join(outdir, "shiny.pal")):
        write_jasc(os.path.join(outdir, "shiny.pal"), full)
    print(os.path.basename(outdir), "front/back done (%d colors)" % (len(pal)+1))

def load_stock_icon_pals():
    pals = []
    for i in range(6):
        L = open(os.path.join(GP,"pokemon","icon_palettes","pal%d.pal"%i)).read().split("\n")[3:]
        pals.append([tuple(map(int,l.split())) for l in L if l.strip()][:16])
    return pals

def make_icon(src, outdir, keycol=None):
    """32x64 icon indexed against the best-fitting stock icon palette."""
    a = key_bg(np.array(load(src)), keycol)
    assert a.shape[:2] == (64,32), a.shape
    pals = load_stock_icon_pals()
    op = a[...,3] > 128
    px = a[op][:,:3].astype(int)
    def err(pal):
        cand = np.array(pal[1:], dtype=int)   # index0 = transparent slot
        d = ((px[:,None,:]-cand[None,:,:])**2).sum(2)
        return d.min(1).mean()
    best = min(range(6), key=lambda i: err(pals[i]))
    pal = pals[best]
    cand = np.array(pal[1:], dtype=int)
    idx = np.zeros(a.shape[:2], np.uint8)
    ys, xs = np.where(op)
    d = ((a[ys,xs][:,:3].astype(int)[:,None,:]-cand[None,:,:])**2).sum(2)
    idx[ys,xs] = d.argmin(1)+1
    im = Image.new("P",(32,64)); im.putdata(idx.flatten().tolist())
    im.putpalette([v for c in pal for v in c] + [0]*(768-48))
    im.save(os.path.join(outdir,"icon.png"))
    print(os.path.basename(outdir), "icon done, stock palette", best)
    return best

# ---- Armored Mewtwo: split fb sheet, reprocess, icon ----
MD = os.path.join(GP,"pokemon","mewtwo_armored")
fb = key_bg(np.array(load(os.path.join(CS,"Armored Mewtwo","armored_mewtwo_front_back.png"))), (118,225,60))
species_pair(fb[:, :64], fb[:, 75:139], MD, anim2=False, shiny_same=False)
# shiny aliases normal.pal via INCGFX; nothing else needed
mewtwo_pal = make_icon(os.path.join(CS,"Armored Mewtwo","armored mewtwo menu icon.png"), MD)

# ---- Shadow Jirachi: new fb (top=front,bottom=back), icon, overworld ----
JD = os.path.join(GP,"pokemon","jirachi_shadow")
jfb = key_bg(np.array(load(os.path.join(CS,"Shadow Jirachi","jirachi battle sprite.png"))), (36,216,0))
species_pair(jfb[:64], jfb[64:], JD, anim2=True, shiny_same=True)
jirachi_pal = make_icon(os.path.join(CS,"Shadow Jirachi","jirachi icon.png"), JD)
# overworld 192x27 -> pad to 192x32 bottom-aligned
fo = key_bg(np.array(load(os.path.join(CS,"Shadow Jirachi","jirachi overworld.png"))))
pad = np.zeros((32,192,4), np.uint8); pad[32-fo.shape[0]:] = fo
oidx, opal = quantize(pad)
fullo = [(255,0,255)] + list(opal) + [(0,0,0)]*(15-len(opal))
im = Image.new("P",(192,32)); im.putdata(oidx.flatten().tolist())
im.putpalette([v for c in fullo for v in c])
im.save(os.path.join(JD,"overworld.png"))
write_jasc(os.path.join(JD,"overworld_normal.pal"), fullo)
write_jasc(os.path.join(JD,"overworld_shiny.pal"), fullo)
print("jirachi_shadow overworld done")

# ---- OW people sheets: 144x32, 9 frames, index0 transparent ----
def people_ow(src, png_out, pal_out, keycol):
    a = key_bg(np.array(load(src)), keycol)
    assert a.shape[:2] == (32,144), a.shape
    idx, pal = quantize(a)
    full = [(115,197,164)] + list(pal) + [(0,0,0)]*(15-len(pal))
    im = Image.new("P",(144,32)); im.putdata(idx.flatten().tolist())
    im.putpalette([v for c in full for v in c])
    im.save(png_out)
    write_jasc(pal_out, full)
    print("OW:", os.path.basename(png_out), f"{len(pal)+1} colors")

OP = os.path.join(GP,"object_events")
people_ow(os.path.join(MG,"overworld red.png"), os.path.join(OP,"pics","people","oni_goon.png"),
          os.path.join(OP,"palettes","oni_goon.pal"), (153,229,80))
people_ow(os.path.join(CS,"Mutrid Leader","leader overworld alt.png"), os.path.join(OP,"pics","people","gold_oni.png"),
          os.path.join(OP,"palettes","gold_oni.pal"), None)

print("ICON PAL INDICES: mewtwo_armored=%d jirachi_shadow=%d" % (mewtwo_pal, jirachi_pal))
