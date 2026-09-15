#!/usr/bin/env python3
"""
Wishes of Tomorrow -- overworld walking sprites from
custom sprites/Trainer Sprites/WoT sprites/.

Every source is a 4x4 walk sheet: rows face S / W / E / N, columns are
stand / step A / stand / step B. The artists drew them upscaled (Cyberstryke7
at 4x, the others at 2x), so each sheet is shrunk by its measured scale with a
per-block majority vote -- lossless where the blocks are uniform, which is
checked and reported.

Each sheet becomes the engine's standard 9-frame strip:
    S stand, N stand, W stand, S stepA, S stepB, N stepA, N stepB, W stepA, W stepB
East is the engine's h-flip of west, so the sheet's east row goes unused
(vanilla convention). Frames are 16x32, or 32x32 for the Let's Go-style
sheets whose art is wider than 16px. Art keeps its position inside the cell;
the whole sheet is shifted so the standing feet sit on y=29, the WOT_DRACO
baseline. Semi-transparent pixels (a baked drop shadow) are dropped -- the
engine draws its own shadow.

Sprites use 4bpp, so colours are fitted to 15 by repeatedly merging the
closest pair (the rarer colour takes the commoner one). Every merge is printed.

Then registers each as OBJ_EVENT_GFX_WOT_<NAME> (so porymap lists them
together under WOT_), with its own palette tag. Registration is skipped for
any sprite already registered, so the script can be re-run to rebuild art.

Run from the repo root:  python3 tools/build_wot_ow_sprites.py
"""

import os
import re
import sys
from collections import Counter

from PIL import Image

SRC_DIR = os.path.join("..", "custom sprites", "Trainer Sprites", "WoT sprites")
PIC_DIR = os.path.join("graphics", "object_events", "pics", "people")
PAL_DIR = os.path.join("graphics", "object_events", "palettes")

# (source file, NAME, artist)
SPRITES = [
    ("gloria_by_cyberstryke7_dhzkmoh.png", "GLORIA", "Cyberstryke7"),
    ("iono_by_cyberstryke7_dhzkr19.png", "IONO", "Cyberstryke7"),
    ("kanto_aldo_let_s_go__by_wergan_dm1why9.png", "ALDO", "Wergan"),
    ("leon_by_aveontrainer_dfyujwm.png", "LEON", "aveontrainer"),
    ("melony_by_cyberstryke7_dhzkmns.png", "MELONY", "Cyberstryke7"),
    ("misty_sprite_by_fersure4_desotnz.png", "MISTY", "fersure4"),
    ("oleana_by_aveontrainer_dh3x85r.png", "OLEANA", "aveontrainer"),
    ("sbc_jacinthe_by_diegowt_dmf3cl7.png", "JACINTHE", "DiegoWT"),
    ("victor_by_cyberstryke7_dhzkmmn.png", "VICTOR", "Cyberstryke7"),
]

FIRST_GFX_ID = 423
FIRST_PAL_TAG = 0x116B      # 0x114F is the only other gap; 0x1150-0x116A are follower balls
BASELINE = 29
TRANSPARENT = (255, 0, 255)
MAX_COLOURS = 15


def camel(name):
    return name.capitalize()


def measure_scale(img):
    px = img.load()
    w, h = img.size
    for k in (4, 2):
        tot = uni = 0
        for y in range(0, h - k + 1, k):
            for x in range(0, w - k + 1, k):
                blk = [px[x + i, y + j] for j in range(k) for i in range(k)]
                if any(c[3] for c in blk):
                    tot += 1
                    uni += all(c == blk[0] for c in blk)
        pct = 100.0 * uni / max(tot, 1)
        if pct >= 97.0:
            return k, pct
    return 1, 100.0


def shrink(img, k):
    """Majority vote per k*k block; transparent unless most of the block is art."""
    px = img.load()
    w, h = img.size[0] // k, img.size[1] // k
    out = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    op = out.load()
    for y in range(h):
        for x in range(w):
            blk = [px[x * k + i, y * k + j] for j in range(k) for i in range(k)]
            solid = [c for c in blk if c[3]]
            if len(solid) * 2 >= len(blk):
                op[x, y] = Counter(solid).most_common(1)[0][0]
    return out


def dist(a, b):
    r = (a[0] + b[0]) / 2
    dr, dg, db = a[0] - b[0], a[1] - b[1], a[2] - b[2]
    return ((2 + r / 256) * dr * dr + 4 * dg * dg + (2 + (255 - r) / 256) * db * db) ** 0.5


def fit_colours(img):
    counts = Counter(c[:3] for c in img.getdata() if c[3])
    remap, merges = {}, []
    while len(counts) > MAX_COLOURS:
        cols = list(counts)
        best = None
        for i in range(len(cols)):
            for j in range(i + 1, len(cols)):
                d = dist(cols[i], cols[j])
                if best is None or d < best[0]:
                    best = (d, cols[i], cols[j])
        d, a, b = best
        keep, drop = (a, b) if counts[a] >= counts[b] else (b, a)
        merges.append((drop, keep, d, counts[drop]))
        counts[keep] += counts.pop(drop)
        for k, v in list(remap.items()):
            if v == drop:
                remap[k] = keep
        remap[drop] = keep
    return sorted(counts, key=lambda c: -counts[c]), remap, merges


def build_one(fname, name):
    src = Image.open(os.path.join(SRC_DIR, fname)).convert("RGBA")
    # keep only fully opaque art; the baked 30% drop shadow goes
    src.putdata([c if c[3] >= 128 else (0, 0, 0, 0) for c in src.getdata()])
    src.putdata([(c[0], c[1], c[2], 255) if c[3] else c for c in src.getdata()])

    k, pct = measure_scale(src)
    art = shrink(src, k)
    cw, ch = art.size[0] // 4, art.size[1] // 4
    fw = 16 if cw <= 16 else 32
    fh = 32
    if cw > fw or ch > fh:
        sys.exit("%s: cell %dx%d does not fit a %dx%d frame" % (name, cw, ch, fw, fh))

    palette, remap, merges = fit_colours(art)
    index = {c: i + 1 for i, c in enumerate(palette)}

    def cell(row, col):
        return art.crop((col * cw, row * ch, (col + 1) * cw, (row + 1) * ch))

    def bbox(im):
        return im.getbbox()  # alpha-aware for RGBA

    # vertical offset: standing-south feet onto the baseline, clamped to the frame
    stand = bbox(cell(0, 0))
    tops = [bbox(cell(r, c))[1] for r in range(4) for c in range(4) if bbox(cell(r, c))]
    bots = [bbox(cell(r, c))[3] for r in range(4) for c in range(4) if bbox(cell(r, c))]
    oy = (BASELINE + 1) - stand[3]
    oy = max(oy, -min(tops))
    oy = min(oy, fh - max(bots))
    ox = (fw - cw) // 2

    order = [(0, 0), (3, 0), (1, 0), (0, 1), (0, 3), (3, 1), (3, 3), (1, 1), (1, 3)]
    strip = Image.new("P", (fw * len(order), fh), 0)
    flat = list(TRANSPARENT)
    for c in palette:
        flat += list(c)
    flat += [0] * (48 - len(flat))
    strip.putpalette(flat)
    sp = strip.load()
    for f, (r, c) in enumerate(order):
        im = cell(r, c)
        ip = im.load()
        for y in range(ch):
            for x in range(cw):
                p = ip[x, y]
                if p[3]:
                    col = p[:3]
                    col = remap.get(col, col)
                    sp[f * fw + ox + x, oy + y] = index[col]

    lower = name.lower()
    strip.save(os.path.join(PIC_DIR, "wot_%s.png" % lower), bits=4)
    with open(os.path.join(PAL_DIR, "wot_%s.pal" % lower), "w", newline="\r\n") as fh_:
        fh_.write("JASC-PAL\n0100\n16\n")
        for c in [TRANSPARENT] + palette + [(0, 0, 0)] * (15 - len(palette)):
            fh_.write("%d %d %d\n" % c)

    worst = max((m[2] for m in merges), default=0)
    print("%-9s %dx scale (%.1f%% uniform), cell %dx%d -> %dx%d frames, feet y=%d, %d colours%s" % (
        name, k, pct, cw, ch, fw, fh, stand[3] - 1 + oy, len(palette),
        "" if not merges else ", %d merged (worst distance %.0f)" % (len(merges), worst)))
    for drop, keep, d, n in merges:
        print("            %02X%02X%02X -> %02X%02X%02X  (%d px, distance %.0f)" % (drop + keep + (n, d)))
    return fw, fh


def patch(path, anchor, addition, after=True):
    s = open(path, encoding="utf-8", newline="").read()
    if s.count(anchor) != 1:
        sys.exit("%s: anchor found %d times: %r" % (path, s.count(anchor), anchor[:60]))
    s = s.replace(anchor, anchor + addition if after else addition + anchor)
    open(path, "w", encoding="utf-8", newline="").write(s)


def register(built):
    consts = "include/constants/event_objects.h"
    if "OBJ_EVENT_GFX_WOT_%s " % SPRITES[0][1] in open(consts, encoding="utf-8").read():
        print("already registered; art rebuilt only")
        return

    n_old = "#define NUM_OBJ_EVENT_GFX                        %d\n" % FIRST_GFX_ID
    defs = "// Wishes of Tomorrow: walking sprites from custom sprites/Trainer Sprites/WoT sprites\n"
    for i, (_, name, artist) in enumerate(SPRITES):
        defs += "#define %-44s %d // %s\n" % ("OBJ_EVENT_GFX_WOT_" + name, FIRST_GFX_ID + i, artist)
    patch(consts, n_old, defs, after=False)
    patch(consts, n_old, "", after=True)
    s = open(consts, encoding="utf-8", newline="").read()
    s = s.replace(n_old, "#define NUM_OBJ_EVENT_GFX                        %d\n" % (FIRST_GFX_ID + len(SPRITES)), 1)
    open(consts, "w", encoding="utf-8", newline="").write(s)

    tags = ""
    for i, (_, name, _a) in enumerate(SPRITES):
        tags += "#define %-55s 0x%04X\n" % ("OBJ_EVENT_PAL_TAG_WOT_" + name, FIRST_PAL_TAG + i)
    patch(consts, "#define OBJ_EVENT_PAL_TAG_WOT_EDWARDS_IMPACT                    0x114E\n", tags)

    gfx, pics, infos, externs, ptrs, pals = "", "", "", "", "", ""
    for (_, name, _a), (fw, fh) in zip(SPRITES, built):
        c, lower = camel(name), name.lower()
        mw, mh = fw // 8, fh // 8
        gfx += ('const u32 gObjectEventPic_Wot%s[] = INCGFX_U32("graphics/object_events/pics/people/wot_%s.png", ".4bpp", "-mwidth %d -mheight %d");\n'
                'const u16 gObjectEventPal_Wot%s[] = INCGFX_U16("graphics/object_events/palettes/wot_%s.pal", ".gbapal");\n') % (c, lower, mw, mh, c, lower)
        pics += ("\nstatic const struct SpriteFrameImage sPicTable_Wot%s[] = {\n"
                 "    overworld_ascending_frames(gObjectEventPic_Wot%s, %d, %d),\n};\n") % (c, c, mw, mh)
        infos += ("\nconst struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Wot%s = {\n"
                  "    .tileTag = TAG_NONE,\n"
                  "    .paletteTag = OBJ_EVENT_PAL_TAG_WOT_%s,\n"
                  "    .reflectionPaletteTag = OBJ_EVENT_PAL_TAG_NONE,\n"
                  "    .size = %d,\n"
                  "    .width = %d,\n"
                  "    .height = %d,\n"
                  "    .paletteSlot = PALSLOT_NPC_SPECIAL,\n"
                  "    .shadowSize = SHADOW_SIZE_M,\n"
                  "    .inanimate = FALSE,\n"
                  "    .compressed = FALSE,\n"
                  "    .tracks = TRACKS_FOOT,\n"
                  "    .oam = &gObjectEventBaseOam_%dx%d,\n"
                  "    .subspriteTables = sOamTables_%dx%d,\n"
                  "    .anims = sAnimTable_Standard,\n"
                  "    .images = sPicTable_Wot%s,\n"
                  "};\n") % (c, name, fw * fh // 2, fw, fh, fw, fh, fw, fh, c)
        externs += "extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Wot%s;\n" % c
        ptrs += "    %-44s &gObjectEventGraphicsInfo_Wot%s,\n" % ("[OBJ_EVENT_GFX_WOT_%s] =" % name, c)
        pals += "    %-40s OBJ_EVENT_PAL_TAG_WOT_%s},\n" % ("{gObjectEventPal_Wot%s," % c, name)

    patch("src/data/object_events/object_event_graphics.h",
          'const u16 gObjectEventPal_WotEdwardsImpact[] = INCGFX_U16("graphics/object_events/palettes/wot_edwards_impact.pal", ".gbapal");\n', gfx)
    patch("src/data/object_events/object_event_pic_tables.h",
          "static const struct SpriteFrameImage sPicTable_WotEdwardsImpact[] = {\n    obj_frame_tiles(gObjectEventPic_WotEdwardsImpact),\n};\n", pics)
    patch("src/data/object_events/object_event_graphics_info.h",
          "    .images = sPicTable_WotEdwardsImpact,\n};\n", infos)
    patch("src/data/object_events/object_event_graphics_info_pointers.h",
          "extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_WotEdwardsImpact;\n", externs)
    patch("src/data/object_events/object_event_graphics_info_pointers.h",
          "    [OBJ_EVENT_GFX_WOT_EDWARDS_IMPACT] =         &gObjectEventGraphicsInfo_WotEdwardsImpact,\n", ptrs)
    patch("src/event_object_movement.c",
          "    {gObjectEventPal_WotEdwardsImpact,       OBJ_EVENT_PAL_TAG_WOT_EDWARDS_IMPACT},\n", pals)
    print("registered OBJ_EVENT_GFX_WOT_%s .. WOT_%s (%d-%d), palette tags 0x%04X-0x%04X" % (
        SPRITES[0][1], SPRITES[-1][1], FIRST_GFX_ID, FIRST_GFX_ID + len(SPRITES) - 1,
        FIRST_PAL_TAG, FIRST_PAL_TAG + len(SPRITES) - 1))


if __name__ == "__main__":
    built = [build_one(f, n) for f, n, _ in SPRITES]
    if "--art-only" not in sys.argv:
        register(built)
