#!/usr/bin/env python3
"""
Pokemon ZA trainer sprites from custom sprites/ZA Sprites/.

Two kinds of source art:

  * Overworld walk sheets (4x4): rows face S / W / E / N, columns are
    stand / step A / stand / step B. Each becomes the engine's standard 9-frame
    strip, the same way tools/build_wot_ow_sprites.py does it, and is registered
    as OBJ_EVENT_GFX_ZA_<NAME> so it shows up in porymap.

  * Standing battle figures: one full-body figure per trainer. Each is fitted
    into the 64x64 trainer-pic box and registered as TRAINER_PIC_FRONT_ZA_<NAME>
    so it shows up in porymap's trainer editor.

Everything is named with the ZA_ prefix so it is easy to find in porymap.

Sheets whose grid does not divide evenly (Emma's) are cut on the real cell
boundaries, then cropped to the art they share, so no frame loses pixels.

Run from the repo root:  python3 tools/build_za_sprites.py
"""

import os
import sys
from collections import Counter

from PIL import Image, ImageChops

SRC_DIR = os.path.join("..", "custom sprites", "ZA Sprites")
PIC_DIR = os.path.join("graphics", "object_events", "pics", "people")
PAL_DIR = os.path.join("graphics", "object_events", "palettes")
TRAINER_DIR = os.path.join("graphics", "trainers", "front_pics")

# (source relative to SRC_DIR, NAME)
OW_SPRITES = [
    ("detective emma/z_a_emma_overworld_look_by_exalted_by_limitdistortion_dkemkd2.png", "EMMA"),
    ("dyn4mo_canari_by_diegowt_dlrut3h/trTDYN4MO_Canari.png", "CANARI"),
    ("kalos_gym_leader_valerie_by_diegowt_dlrutv0.png", "VALERIE"),
    ("paldea_gym_leader_tulip_by_diegowt_dlruwfz.png", "TULIP"),
    ("researcher mable/researcher_mable_by_diegowt_dmf3d4p.png", "MABLE"),
    ("taunie/taunie_muni_from_pokemon_za_overworld_bw_style__by_tobalcr_dkknzhd.png", "TAUNIE"),
    ("team_mz_lida_by_diegowt_dlruxav/trTeamMZ_Lida.png", "LIDA"),
]

# (source relative to SRC_DIR, NAME)
BATTLE_SPRITES = [
    ("detective emma/detective_emma_vs_look_by_limitdistortion_dkemlah.png", "DETECTIVE_EMMA"),
    ("researcher mable/mable_pokemon_legends_za_by_imnothoro_dke09ln.png", "MABLE"),
    ("team_mz_lida_by_diegowt_dlruxav/lida_pokemon_legends_za_by_imnothoro_dke09dg.png", "LIDA"),
]

FIRST_GFX_ID = 433          # NUM_OBJ_EVENT_GFX before this script ran
FIRST_PAL_TAG = 0x1174      # 0x1174-0x1188 are unused; 0x1189 is taken
BASELINE = 29
TRANSPARENT = (255, 0, 255)
MAX_COLOURS = 15


def camel(name):
    return name.title().replace("_", "")


def measure_scale(img):
    """Largest pixel pitch (4 or 2) whose blocks are at least 97% uniform."""
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
        if 100.0 * uni / max(tot, 1) >= 97.0:
            return k
    return 1


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


def cell_boxes(w, h):
    """Real cell boundaries of a 4x4 grid, even when w or h is not a multiple of 4."""
    return [[(round(c * w / 4), round(r * h / 4), round((c + 1) * w / 4), round((r + 1) * h / 4))
             for c in range(4)] for r in range(4)]


def build_ow(fname, name):
    src = Image.open(os.path.join(SRC_DIR, fname)).convert("RGBA")
    # keep only fully opaque art; a baked drop shadow goes
    src.putdata([(c[0], c[1], c[2], 255) if c[3] >= 128 else (0, 0, 0, 0) for c in src.getdata()])

    k = measure_scale(src)
    art = shrink(src, k) if k > 1 else src
    fh = 32

    def window(art):
        """Shared content window across all 16 cells, so every frame keeps the same offsets."""
        boxes = cell_boxes(*art.size)
        x0 = y0 = 10 ** 6
        x1 = y1 = -10 ** 6
        for r in range(4):
            for c in range(4):
                bx = art.crop(boxes[r][c]).getbbox()
                if bx:
                    x0, y0 = min(x0, bx[0]), min(y0, bx[1])
                    x1, y1 = max(x1, bx[2]), max(y1, bx[3])
        if x1 < 0:
            sys.exit("%s: no art found" % name)
        return boxes, x0, y0, x1 - x0, y1 - y0

    boxes, x0, y0, cw, ch = window(art)
    if ch > fh:
        # taller than a 32px frame: shrink the sheet by its own pixel grid. Nearest
        # neighbour keeps the pixel edges hard, at the cost of dropping some rows.
        s = fh / ch
        art = art.resize((round(art.size[0] * s), round(art.size[1] * s)), Image.NEAREST)
        boxes, x0, y0, cw, ch = window(art)
        print("    %s: sheet resized x%.3f to fit a %d-tall frame" % (name, s, fh))
    fw = 16 if cw <= 16 else 32
    if cw > fw or ch > fh:
        sys.exit("%s: art %dx%d does not fit a %dx%d frame" % (name, cw, ch, fw, fh))

    def cell(row, col):
        bx = boxes[row][col]
        return art.crop((bx[0] + x0, bx[1] + y0, bx[0] + x0 + cw, bx[1] + y0 + ch))

    palette, remap, merges = fit_colours(art)
    index = {c: i + 1 for i, c in enumerate(palette)}

    ox = (fw - cw) // 2

    order = [(0, 0), (3, 0), (1, 0), (0, 1), (0, 3), (3, 1), (3, 3), (1, 1), (1, 3)]
    strip = Image.new("P", (fw * len(order), fh), 0)
    flat = list(TRANSPARENT)
    for c in palette:
        flat += list(c)
    flat += [0] * (48 - len(flat))
    strip.putpalette(flat)
    sp = strip.load()
    feet = []
    for f, (r, c) in enumerate(order):
        im = cell(r, c)
        bb = im.getbbox()
        if bb is None:
            continue
        # each frame's feet go onto the baseline. The source rows are not all
        # placed at the same height, so one shared offset would make heads bob.
        oy = (BASELINE + 1) - bb[3]
        oy = max(oy, -bb[1])
        oy = min(oy, fh - bb[3])
        feet.append(oy)
        ip = im.load()
        for y in range(ch):
            for x in range(cw):
                p = ip[x, y]
                if p[3]:
                    col = remap.get(p[:3], p[:3])
                    sp[f * fw + ox + x, oy + y] = index[col]
    oy = feet[0]

    lower = name.lower()
    strip.save(os.path.join(PIC_DIR, "za_%s.png" % lower), bits=4)
    with open(os.path.join(PAL_DIR, "za_%s.pal" % lower), "w", newline="\r\n") as f_:
        f_.write("JASC-PAL\n0100\n16\n")
        for c in [TRANSPARENT] + palette + [(0, 0, 0)] * (15 - len(palette)):
            f_.write("%d %d %d\n" % c)

    worst = max((m[2] for m in merges), default=0)
    print("OW  %-8s %dx scale, art %dx%d -> %dx%d frames, feet y=%d, %d colours%s" % (
        name, k, cw, ch, fw, fh, oy + BASELINE - BASELINE, len(palette),
        "" if not merges else ", %d merged (worst distance %.0f)" % (len(merges), worst)))
    return fw, fh


def build_front(fname, name):
    """Standing figure -> 64x64 trainer pic, <=15 colours plus a transparent key at index 0."""
    src = Image.open(os.path.join(SRC_DIR, fname)).convert("RGBA")
    r, g, b, a = src.split()
    # premultiply so transparent pixels don't bleed their colour into the edges
    pm = Image.merge("RGB", [ImageChops.multiply(ch, a) for ch in (r, g, b)])
    small = pm.resize((64, 64), Image.LANCZOS)
    small_a = a.resize((64, 64), Image.LANCZOS)
    sp, sa = small.load(), small_a.load()
    out = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    op = out.load()
    for y in range(64):
        for x in range(64):
            al = sa[x, y]
            if al >= 128:
                op[x, y] = tuple(min(255, sp[x, y][i] * 255 // al) for i in range(3)) + (255,)

    opaque = [p[:3] for p in out.getdata() if p[3]]
    common = Counter(opaque).most_common(1)[0][0]
    rgb = Image.new("RGB", (64, 64), common)
    rgb.putdata([p[:3] if p[3] else common for p in out.getdata()])
    q = rgb.quantize(colors=MAX_COLOURS, method=Image.Quantize.MEDIANCUT)
    qpal = q.getpalette()[: 3 * MAX_COLOURS]
    used = sorted(set(q.getdata()))
    pal = list(TRANSPARENT) + qpal
    pal += [0] * (48 - len(pal))

    final = Image.new("P", (64, 64), 0)
    final.putpalette(pal)
    fp, qp = final.load(), q.load()
    for y in range(64):
        for x in range(64):
            if out.getpixel((x, y))[3]:
                fp[x, y] = qp[x, y] + 1

    final.save(os.path.join(TRAINER_DIR, "za_%s.png" % name.lower()), bits=4)
    print("PIC %-16s 64x64, %d colours + transparent" % (name, len(used)))


def patch(path, anchor, addition, after=True):
    s = open(path, encoding="utf-8", newline="").read()
    if s.count(anchor) != 1:
        sys.exit("%s: anchor found %d times: %r" % (path, s.count(anchor), anchor[:60]))
    s = s.replace(anchor, anchor + addition if after else addition + anchor)
    open(path, "w", encoding="utf-8", newline="").write(s)


def register_ow(built):
    consts = "include/constants/event_objects.h"
    if "OBJ_EVENT_GFX_ZA_%s " % OW_SPRITES[0][1] in open(consts, encoding="utf-8").read():
        print("overworld sprites already registered; art rebuilt only")
        return

    n_old = "#define NUM_OBJ_EVENT_GFX                        %d\n" % FIRST_GFX_ID
    defs = "// Pokemon ZA trainers: walking sprites from custom sprites/ZA Sprites\n"
    for i, (_, name) in enumerate(OW_SPRITES):
        defs += "#define %-44s %d\n" % ("OBJ_EVENT_GFX_ZA_" + name, FIRST_GFX_ID + i)
    patch(consts, n_old, defs, after=False)
    s = open(consts, encoding="utf-8", newline="").read()
    s = s.replace(n_old, "#define NUM_OBJ_EVENT_GFX                        %d\n" % (FIRST_GFX_ID + len(OW_SPRITES)), 1)
    open(consts, "w", encoding="utf-8", newline="").write(s)

    tags = "// Pokemon ZA trainers\n"
    for i, (_, name) in enumerate(OW_SPRITES):
        tags += "#define %-55s 0x%04X\n" % ("OBJ_EVENT_PAL_TAG_ZA_" + name, FIRST_PAL_TAG + i)
    patch(consts, "#define OBJ_EVENT_PAL_TAG_WOT_VICTOR                            0x1173\n", tags)

    gfx, pics, infos, externs, ptrs, pals = "", "", "", "", "", ""
    for (_, name), (fw, fh) in zip(OW_SPRITES, built):
        c, lower = camel(name), name.lower()
        mw, mh = fw // 8, fh // 8
        gfx += ('const u32 gObjectEventPic_Za%s[] = INCGFX_U32("graphics/object_events/pics/people/za_%s.png", ".4bpp", "-mwidth %d -mheight %d");\n'
                'const u16 gObjectEventPal_Za%s[] = INCGFX_U16("graphics/object_events/palettes/za_%s.pal", ".gbapal");\n') % (c, lower, mw, mh, c, lower)
        pics += ("\nstatic const struct SpriteFrameImage sPicTable_Za%s[] = {\n"
                 "    overworld_ascending_frames(gObjectEventPic_Za%s, %d, %d),\n};\n") % (c, c, mw, mh)
        infos += ("\nconst struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Za%s = {\n"
                  "    .tileTag = TAG_NONE,\n"
                  "    .paletteTag = OBJ_EVENT_PAL_TAG_ZA_%s,\n"
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
                  "    .images = sPicTable_Za%s,\n"
                  "};\n") % (c, name, fw * fh // 2, fw, fh, fw, fh, fw, fh, c)
        externs += "extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Za%s;\n" % c
        ptrs += "    %-44s &gObjectEventGraphicsInfo_Za%s,\n" % ("[OBJ_EVENT_GFX_ZA_%s] =" % name, c)
        pals += "    %-40s OBJ_EVENT_PAL_TAG_ZA_%s},\n" % ("{gObjectEventPal_Za%s," % c, name)

    patch("src/data/object_events/object_event_graphics.h",
          'const u16 gObjectEventPal_WotVictor[] = INCGFX_U16("graphics/object_events/palettes/wot_victor.pal", ".gbapal");\n', gfx)
    patch("src/data/object_events/object_event_pic_tables.h",
          "    overworld_ascending_frames(gObjectEventPic_WotVictor, 2, 4),\n};\n", pics)
    patch("src/data/object_events/object_event_graphics_info.h",
          "    .images = sPicTable_WotVictor,\n};\n", infos)
    patch("src/data/object_events/object_event_graphics_info_pointers.h",
          "extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_WotVictor;\n", externs)
    patch("src/data/object_events/object_event_graphics_info_pointers.h",
          "    [OBJ_EVENT_GFX_WOT_VICTOR] =                 &gObjectEventGraphicsInfo_WotVictor,\n", ptrs)
    patch("src/event_object_movement.c",
          "    {gObjectEventPal_WotVictor,              OBJ_EVENT_PAL_TAG_WOT_VICTOR},\n", pals)
    print("registered OBJ_EVENT_GFX_ZA_%s .. ZA_%s (%d-%d), palette tags 0x%04X-0x%04X" % (
        OW_SPRITES[0][1], OW_SPRITES[-1][1], FIRST_GFX_ID, FIRST_GFX_ID + len(OW_SPRITES) - 1,
        FIRST_PAL_TAG, FIRST_PAL_TAG + len(OW_SPRITES) - 1))


def register_front(names):
    consts = "include/constants/trainers.h"
    if "TRAINER_PIC_FRONT_ZA_" in open(consts, encoding="utf-8").read():
        print("battle pics already registered; art rebuilt only")
        return

    enums = "    // Pokemon ZA trainers\n"
    for name in names:
        enums += "    TRAINER_PIC_FRONT_ZA_%s,\n" % name
    patch(consts, "    TRAINER_PIC_FRONT_WOT_BEACH_GIRL,\n", enums)

    data = "src/data/graphics/trainers.h"
    incs, rows = "", ""
    for name in names:
        lower = name.lower()
        incs += ('const u32 gTrainerFrontPic_Za%s[] = INCGFX_U32("graphics/trainers/front_pics/za_%s.png", ".4bpp.smol");\n'
                 'const u16 gTrainerPalette_Za%s[] = INCGFX_U16("graphics/trainers/front_pics/za_%s.png", ".gbapal");\n') % (
            camel(name), lower, camel(name), lower)
        rows += "    TRAINER_SPRITE(TRAINER_PIC_FRONT_ZA_%s, gTrainerFrontPic_Za%s, gTrainerPalette_Za%s),\n" % (
            name, camel(name), camel(name))
    s = open(data, encoding="utf-8", newline="").read()
    wot_inc = [l for l in s.splitlines(True) if l.startswith("const u16 gTrainerPalette_WotBeachGirl[]")]
    if len(wot_inc) != 1:
        sys.exit("expected one WotBeachGirl palette line in %s" % data)
    patch(data, wot_inc[0], incs)
    patch(data, "    TRAINER_SPRITE(TRAINER_PIC_FRONT_WOT_BEACH_GIRL, gTrainerFrontPic_WotBeachGirl, gTrainerPalette_WotBeachGirl),\n", rows)
    print("registered TRAINER_PIC_FRONT_ZA_%s" % ", ZA_".join(names))


if __name__ == "__main__":
    built = [build_ow(f, n) for f, n in OW_SPRITES]
    for f, n in BATTLE_SPRITES:
        build_front(f, n)
    if "--art-only" not in sys.argv:
        register_ow(built)
        register_front([n for _, n in BATTLE_SPRITES])
