#!/usr/bin/env python3
r"""
Pokemon Wishes of Tomorrow - Asset build helper.

Run from WSL:
    cd /mnt/d/ROM\ Hack\ Project
    sudo apt install -y python3-pil    # Pillow without pip

    # DIAGNOSTICS (run these first, share output)
    python3 build_assets.py inspect-attrs > attrs_before.txt
    python3 build_assets.py diag-signs

    # FIXES (one at a time)
    python3 build_assets.py set-walkbehind <slot,slot,slot...>
    python3 build_assets.py body-raw           # uses kit body PNG raw, no compositing
    python3 build_assets.py snow-roofs         # appends snow-rooftop variants past slot 471
    python3 build_assets.py torii-merge        # appends torii tiles past snow-roof slots
    python3 build_assets.py install-sprite     # wires ninja PNG into engine source

    # THEN ALWAYS:
    cd /mnt/d/ROM\ Hack\ Project/pokeemerald-expansion
    make -j$(nproc)

Every operation backs up files to *.bak first.
"""

import argparse
import json
import os
import shutil
import struct
import sys
from pathlib import Path

SCRIPT_DIR  = Path(__file__).resolve().parent
PROJ        = SCRIPT_DIR / "pokeemerald-expansion"
SNOW        = PROJ / "data" / "tilesets" / "secondary" / "snow_and_stone"
TORII_DIR   = PROJ / "data" / "tilesets" / "secondary" / "processed_torii_magiscarf"
KIT         = SCRIPT_DIR / "Tools" / "Gen 3 Sprite Pack" / "bases"
RAW_SS_PNG  = SCRIPT_DIR / "tilesets_raw" / "snow_and_stone_by_magiscarf_dcjgz10.png"
RAW_TORII   = SCRIPT_DIR / "tilesets_raw" / "torii_by_magiscarf.png"
PROC_TORII  = SCRIPT_DIR / "tilesets_raw" / "processed_torii_magiscarf"

ATTR_BIN  = SNOW / "metatile_attributes.bin"
META_BIN  = SNOW / "metatiles.bin"
TILES_PNG = SNOW / "tiles.png"

MUNEN_MAP = PROJ / "data" / "maps" / "Munen_village_2" / "map.json"

NUM_METATILES = 512
WARP_DOOR_SLOTS = {114, 283}
DOOR_ATTR = 0x1069

def backup(p: Path):
    bak = p.with_suffix(p.suffix + ".bak")
    if not bak.exists():
        shutil.copy2(p, bak)
        print(f"  backup: {bak.name}")

def fail(msg):
    print(f"ERROR: {msg}", file=sys.stderr)
    sys.exit(1)

def need_pil():
    try:
        from PIL import Image
        return Image
    except ImportError:
        fail("Pillow not installed.  Run: sudo apt install -y python3-pil")

# =====================================================================
# inspect-attrs  -- print every slot's current attribute
# =====================================================================
def cmd_inspect_attrs(args):
    if not ATTR_BIN.exists():
        fail(f"missing {ATTR_BIN}")
    raw = ATTR_BIN.read_bytes()
    print(f"# {ATTR_BIN}")
    print(f"# size = {len(raw)} bytes  ({len(raw)//2} u16 entries)")
    print(f"# slot | attr   | behavior | layer_type")
    LAYER = {0: "NORMAL ", 1: "COVERED", 2: "SPLIT  "}
    nonzero_groups = []
    cur_group = None
    for i in range(min(NUM_METATILES, len(raw)//2)):
        attr = struct.unpack_from("<H", raw, i*2)[0]
        beh = attr & 0xFF
        layer = (attr >> 12) & 0xF
        # only print non-default rows (default=0x1000), plus boundaries
        if attr != 0x1000 or i in WARP_DOOR_SLOTS:
            print(f"  {i:4d} | 0x{attr:04X} |   0x{beh:02X}   | {LAYER.get(layer, hex(layer))}")
    print()
    print("# (slots not listed above are 0x1000 = COVERED, behavior 0x00)")

# =====================================================================
# set-walkbehind <comma slot list>  -- explicit slot flip
# =====================================================================
def cmd_set_walkbehind(args):
    if not args.slots:
        fail("provide --slots 'a,b,c-d,e' (ranges allowed with -)")
    if not ATTR_BIN.exists():
        fail(f"missing {ATTR_BIN}")
    slots = parse_slot_list(args.slots)
    print(f"[set-walkbehind] flipping {len(slots)} slots to NORMAL (walk-behind)")
    backup(ATTR_BIN)

    raw = bytearray(ATTR_BIN.read_bytes())
    if len(raw) < NUM_METATILES * 2:
        raw.extend(b"\x00" * (NUM_METATILES * 2 - len(raw)))

    changed = 0
    for slot in slots:
        if slot in WARP_DOOR_SLOTS:
            print(f"  skip slot {slot} (warp door — keeping 0x1069)")
            continue
        if not 0 <= slot < NUM_METATILES:
            print(f"  skip slot {slot} (out of range)")
            continue
        off = slot * 2
        attr = struct.unpack_from("<H", raw, off)[0]
        new = attr & 0x00FF              # zero layer_type + terrain, keep behavior
        if new != attr:
            struct.pack_into("<H", raw, off, new)
            changed += 1
    if args.dry_run:
        print(f"  DRY RUN: would change {changed} slots; not writing.")
        return
    ATTR_BIN.write_bytes(bytes(raw))
    print(f"  wrote {ATTR_BIN}  ({changed} slots flipped)")

def parse_slot_list(s):
    out = set()
    for tok in s.split(","):
        tok = tok.strip()
        if "-" in tok:
            a, b = tok.split("-")
            out.update(range(int(a), int(b)+1))
        elif tok:
            out.add(int(tok))
    return sorted(out)

# =====================================================================
# diag-signs  -- print Munen sign positions + nearby tile info
# =====================================================================
def cmd_diag_signs(args):
    if not MUNEN_MAP.exists():
        fail(f"missing {MUNEN_MAP}")
    data = json.loads(MUNEN_MAP.read_text())
    bgs = data.get("bg_events", [])
    print(f"[diag-signs] {len(bgs)} bg_events in {MUNEN_MAP.name}")
    for ev in bgs:
        print(f"  {ev.get('type','?'):5s} at ({ev['x']:>2},{ev['y']:>2})  "
              f"facing={ev.get('player_facing_dir','?')}  script={ev.get('script','?')}")
    print()
    print("How sign interaction works in pokeemerald:")
    print("  Player must be standing on a walkable tile ADJACENT to the sign tile,")
    print("  facing toward the sign. The sign tile itself MUST have collision (be impassable).")
    print("  If the player can walk onto the sign tile, pressing A walks them forward instead.")
    print()
    print("FIX: in Porymap, open Munen_village_2 -> Map tab -> Collision view.")
    print("     Verify each (x,y) above is on a tile with collision = 1 (impassable),")
    print("     and that the south-adjacent tile (x, y+1) is walkable so the player")
    print("     can stand south of the sign facing north toward it.")

# =====================================================================
# probe-sprite  -- dump the kit body PNG four ways so we can see what works
# =====================================================================
def cmd_probe_sprite(args):
    Image = need_pil()
    print("[probe-sprite] Diagnostic dump of kit body PNG")
    src = KIT / "body" / "default-male.png"
    if not src.exists():
        fail(f"missing {src}")

    out_dir = SCRIPT_DIR / "sprites_out"
    out_dir.mkdir(exist_ok=True)

    # 1. Raw byte-for-byte copy (no PIL processing at all)
    raw_copy = out_dir / "01_raw_copy.png"
    shutil.copy2(src, raw_copy)
    print(f"  [1] wrote {raw_copy.name}  (raw byte copy, no processing)")

    im = Image.open(src)
    print(f"      source: size={im.size} mode={im.mode}")

    # Print top 8 distinct colors (sample every 2 px)
    im_rgba = im.convert("RGBA")
    px = im_rgba.load()
    from collections import Counter
    c = Counter()
    for y in range(0, im_rgba.height, 2):
        for x in range(0, im_rgba.width, 2):
            c[px[x, y]] += 1
    print("      top 8 colors (R,G,B,A): count")
    for col, n in c.most_common(8):
        print(f"        {col} : {n}")

    # 2. RGBA convert, save as-is
    rgba_out = out_dir / "02_rgba_convert.png"
    im_rgba.save(rgba_out)
    print(f"  [2] wrote {rgba_out.name}  (PIL RGBA convert, no key)")

    # 3. Magenta keyed to alpha
    im3 = im_rgba.copy()
    px3 = im3.load()
    keyed = 0
    for y in range(im3.height):
        for x in range(im3.width):
            r, g, b, a = px3[x, y]
            if r > 200 and g < 80 and b > 200:
                px3[x, y] = (0, 0, 0, 0)
                keyed += 1
    magenta_out = out_dir / "03_magenta_keyed.png"
    im3.save(magenta_out)
    print(f"  [3] wrote {magenta_out.name}  (magenta keyed: {keyed} px)")

    # 4. Composite onto bright red bg so character outline is unmistakable
    red_bg = Image.new("RGBA", im_rgba.size, (255, 0, 0, 255))
    im4 = Image.alpha_composite(red_bg, im_rgba)
    red_out = out_dir / "04_on_red_bg.png"
    im4.save(red_out)
    print(f"  [4] wrote {red_out.name}  (placed on solid red, char should be visible)")

    print()
    print("Open all 4 in your image viewer (Windows Photos or similar).")
    print("Tell Claude which ones show a recognizable trainer character.")
    print(f"  Folder: {out_dir}")

# =====================================================================
# composite-fixed  -- proper layered composite with magenta-as-transparent
# =====================================================================
RECIPE = [
    ("body",       "body/default-male.png"),
    ("outfit",     "dm-outfit/gi.png"),
    ("hair_back",  "hair-back/warrior.png"),
    ("hair",       "hair/warrior.png"),
    ("hair_front", "hair-front/warrior.png"),
    ("eyes",       "eyes/intense.png"),
]

def magenta_to_alpha(im):
    """Convert magenta-background PNG to alpha-transparent."""
    im = im.convert("RGBA")
    px = im.load()
    w, h = im.size
    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]
            if r > 200 and g < 80 and b > 200:
                px[x, y] = (0, 0, 0, 0)
    return im

def cmd_composite_fixed(args):
    Image = need_pil()
    print("[composite-fixed] Layered composite w/ magenta keyed to alpha")
    base = None
    for name, rel in RECIPE:
        p = KIT / rel
        if not p.exists():
            print(f"  skip {name} (not found): {rel}")
            continue
        im = magenta_to_alpha(Image.open(p))
        print(f"  {name:10s} size={im.size}")
        if base is None:
            base = im.copy()
        else:
            if im.size != base.size:
                pad = Image.new("RGBA", base.size, (0,0,0,0))
                pad.paste(im, (0, 0), im)
                im = pad
            base = Image.alpha_composite(base, im)
    out_dir = SCRIPT_DIR / "sprites_out"
    out_dir.mkdir(exist_ok=True)
    out = out_dir / "ninja_player_composite_v2.png"
    if args.dry_run:
        print(f"  DRY RUN: would write {out}  ({base.size})")
        return
    base.save(out)
    print(f"  wrote {out}")

# =====================================================================
# snow-roofs  -- import snow-capped variants from source PNG past slot 471
# =====================================================================
def cmd_snow_roofs(args):
    Image = need_pil()
    print("[snow-roofs] Import snow-capped roof variants past slot 471")
    if not RAW_SS_PNG.exists():
        fail(f"missing {RAW_SS_PNG}")
    if not TILES_PNG.exists():
        fail(f"missing {TILES_PNG}")
    if not META_BIN.exists():
        fail(f"missing {META_BIN}")

    src = Image.open(RAW_SS_PNG).convert("RGBA")
    tiles = Image.open(TILES_PNG).convert("RGBA")
    print(f"  source size: {src.size}")
    print(f"  current tiles.png: {tiles.size}")

    # Source PNG layout (visually inspected):
    #   Row 3 (y ~ 96..143) holds snow-capped variants of 4 houses.
    #   Each house tile in the source is roughly 64x48 pixels.
    # We slice 8x8 tile strips from row 3 (y range chosen below) and append
    # them to tiles.png as additional 8x8 tiles. Then we add new metatiles
    # in metatiles.bin that compose those tiles (4 tile indices per metatile
    # for a single-layer 16x16 metatile; secondary uses dual-layer).
    #
    # For safety this command runs in DRY-RUN by default and emits a slice
    # plan you can verify against the source PNG. Re-run with --apply once
    # the slice coordinates look right.
    plan = []
    # ROUGH cut: y=96 to y=144 is the snow-roof band; x range 0..256 (4 houses)
    # Each house roof is 64 px wide. We export 64x48 = 8 x 6 = 48 tiles per house.
    for house_idx in range(4):
        x0 = house_idx * 64
        plan.append({"house": house_idx, "x": x0, "y": 96, "w": 64, "h": 48})

    print("  slice plan (verify against source PNG visually before --apply):")
    for p in plan:
        print(f"    house {p['house']}: x={p['x']} y={p['y']} w={p['w']} h={p['h']}")

    if not args.apply:
        print()
        print("  DRY RUN — not modifying tiles.png or metatiles.bin.")
        print("  Re-run with --apply to commit, after confirming the slice plan looks right.")
        return

    # APPLY MODE — back up everything first
    backup(TILES_PNG)
    backup(META_BIN)
    backup(ATTR_BIN)

    # 1. Append slices to tiles.png
    # tiles.png is a 128-wide GBA-format tile sheet, 8-bit indexed normally;
    # for a secondary tileset it's appended below the existing tiles.
    # For visual correctness we paste-append at the bottom.
    appended_h = 0
    for p in plan:
        crop = src.crop((p["x"], p["y"], p["x"]+p["w"], p["y"]+p["h"]))
        # paste row-by-row 64x48 = 8 tiles wide x 6 tiles tall = 48 tiles
        appended_h += p["h"]
    new_tiles = Image.new("RGBA", (tiles.width, tiles.height + appended_h), (0,0,0,0))
    new_tiles.paste(tiles, (0,0))
    cur_y = tiles.height
    for p in plan:
        crop = src.crop((p["x"], p["y"], p["x"]+p["w"], p["y"]+p["h"]))
        new_tiles.paste(crop, (0, cur_y))
        cur_y += p["h"]
    new_tiles.save(TILES_PNG)
    print(f"  wrote {TILES_PNG}  ({new_tiles.size})")

    # 2. New metatiles + attributes
    # NOTE: This stub writes a placeholder. Wiring up metatiles.bin entries
    # requires knowing the tile indices the new tile rows occupy. The user
    # is expected to assemble the new metatiles in Porymap's Tileset Editor
    # using the newly imported tiles. This script just appends the tile data.
    print()
    print("  Tile DATA imported. Now open Porymap:")
    print("    Tools -> Tileset Editor -> snow_and_stone")
    print("    The new snowy-roof tiles will appear at the bottom of the tile selector.")
    print("    Drag them into empty metatile slots (472+) to build the snow-roof metatiles.")
    print("    Save the tileset. Then rebuild the ROM.")

# =====================================================================
# torii-merge  -- append torii tile data + install torii palette as slot 12
# =====================================================================
# Snow_and_stone palette slots 09, 10, 11, 12 are all empty (verified).
# We install the torii palette into slot 12 (highest unused secondary slot).
# The torii TILE pixel data is appended below the existing snow tiles so they
# show up at the bottom of Porymap's tile selector.
#
# Important: tiles.png in pokeemerald is a 4bpp INDEXED PNG. Pixel values are
# 0-15 (palette index within whatever 16-color slot the metatile assigns).
# To preserve indexing across the merge, both images are opened in P mode and
# raw index bytes are copied — not paste-via-RGB which would mangle indices.
# The merged PNG carries snow's global palette, so torii tiles will LOOK wrong
# in Porymap until you set their metatile palette to 12 — at which point they
# render in correct red/black/grey torii colors.
def cmd_torii_merge(args):
    Image = need_pil()
    print("[torii-merge] Append torii tiles + install torii palette as snow slot 12")

    src_dir = SCRIPT_DIR / "tilesets_raw" / "processed_torii_magiscarf"
    src_tiles = src_dir / "tiles.png"
    src_pal   = src_dir / "palette.pal"
    if not src_tiles.exists():
        fail(f"missing {src_tiles}")
    if not src_pal.exists():
        fail(f"missing {src_pal}")
    if not TILES_PNG.exists():
        fail(f"missing {TILES_PNG}")

    snow = Image.open(TILES_PNG)
    torii = Image.open(src_tiles)
    print(f"  snow tiles.png:  {snow.size}  mode={snow.mode}")
    print(f"  torii tiles.png: {torii.size}  mode={torii.mode}")

    target_pal12    = SNOW / "palettes" / "12.pal"
    target_gbapal12 = SNOW / "palettes" / "12.gbapal"

    if not args.apply:
        new_h = snow.height + torii.height
        print()
        print("  DRY RUN — would do:")
        print(f"    1. Append torii pixels below snow ({snow.size} -> {snow.width}x{new_h})")
        print(f"    2. Backup + replace palettes/12.pal with torii palette")
        print(f"    3. Delete palettes/12.gbapal (so make regenerates from .pal)")
        print()
        print("  Re-run with --apply to commit.")
        return

    backup(TILES_PNG)
    if target_pal12.exists():
        backup(target_pal12)
    if target_gbapal12.exists():
        backup(target_gbapal12)

    # Force P (indexed) mode for both
    snow_p  = snow if snow.mode == "P" else snow.convert("P")
    torii_p = torii if torii.mode == "P" else torii.convert("P")

    width = max(snow_p.width, torii_p.width)
    new_h = snow_p.height + torii_p.height

    # Build merged image: snow palette, snow indices on top, torii indices below.
    # We copy RAW palette indices (not RGB), so the torii tile shapes are
    # preserved at indices 0-15 — they'll render with snow's slot-12 colors,
    # which we are simultaneously replacing with the torii palette below.
    snow_bytes  = bytes(snow_p.tobytes())
    torii_bytes = bytes(torii_p.tobytes())

    merged_bytes = bytearray(width * new_h)
    for y in range(snow_p.height):
        for x in range(snow_p.width):
            merged_bytes[y * width + x] = snow_bytes[y * snow_p.width + x]
    for y in range(torii_p.height):
        for x in range(torii_p.width):
            merged_bytes[(snow_p.height + y) * width + x] = torii_bytes[y * torii_p.width + x]

    merged = Image.frombytes("P", (width, new_h), bytes(merged_bytes))
    merged.putpalette(snow_p.getpalette())
    merged.save(TILES_PNG)
    print(f"  wrote {TILES_PNG}  ({merged.size}, P mode)")

    # Install torii palette into slot 12
    shutil.copy2(src_pal, target_pal12)
    print(f"  copied {src_pal.name} -> palettes/12.pal")
    if target_gbapal12.exists():
        target_gbapal12.unlink()
        print(f"  deleted palettes/12.gbapal (make will regenerate)")

    print()
    print("=== Next steps in Porymap ===")
    print("  1. If Porymap is open, reload the project (File -> Reload Project)")
    print("     so it picks up the new palette 12.")
    print("  2. Open Munen_village_2 -> Tools -> Tileset Editor -> snow_and_stone")
    print("  3. Scroll to the bottom of the TILE selector -- new torii tiles are there.")
    print("     They will render with WRONG colors at first (because the global PNG")
    print("     preview uses palette 0 colors). That's expected.")
    print("  4. Click an empty METATILE slot (e.g. 480) to start a new metatile.")
    print("     Drag a torii tile into the metatile layer. Set the tile's palette")
    print("     dropdown to 12 -- it will now render in the proper red/black/grey.")
    print("  5. Repeat for each torii piece (typically 4 pieces: top-left, top-right,")
    print("     pillar-left, pillar-right -- or however your prefab is laid out).")
    print("  6. File -> Save Tileset. Then paint torii gates in Munen.")
    print("  7. Rebuild ROM:  cd pokeemerald-expansion && make -j$(nproc)")

# =====================================================================
# install-sprite  -- wire ninja_player.png into engine source
# =====================================================================
def cmd_install_sprite(args):
    Image = need_pil()
    print("[install-sprite] Installing ninja sprite into engine source files")
    src = SCRIPT_DIR / "sprites_out" / "ninja_player_bodyraw.png"
    if not src.exists():
        fail(f"missing {src} — run `body-raw` first")

    # Stub: prints what needs to change. Implementation pending visual approval.
    print("  pending — once you confirm body-raw output looks correct, Claude will")
    print("  do the engine wiring (event_objects.h, object_event_graphics.h, etc.)")

# =====================================================================
# torii-compose  -- the BIG one: copy fully-built torii metatiles from
# torii_magiscarf INTO snow_and_stone so they can be painted in Munen.
# =====================================================================
# What this does, in order:
#   1. Reads torii_magiscarf's tiles.png (the 8x8 source tile data).
#   2. Appends those tiles to snow_and_stone's tiles.png  (idempotent — skips
#      if already merged based on file size).
#   3. Installs torii_magiscarf's palette 7 (the torii red/grey palette) into
#      snow_and_stone's palette slot 12 (which was verified empty).
#   4. Parses prefabs.json to find which metatile IDs make up "torii-3" (the
#      smallest single torii gate in the prefab list).
#   5. Reads each of those metatiles from torii_magiscarf/metatiles.bin.
#   6. Re-indexes every tile reference inside the metatile:
#        - tile_id += snow's pre-merge tile count  (so the references point
#          into the appended torii tiles inside snow_and_stone/tiles.png)
#        - palette = 12  (so colors come from the installed torii palette)
#   7. Writes the re-indexed metatile entries into snow_and_stone/metatiles.bin
#      starting at slot 480 (free slot per inspect-attrs).
#   8. Copies the matching attribute bytes from torii_magiscarf into
#      snow_and_stone/metatile_attributes.bin at the same slot range.
#   9. Prints a layout map: "paint this metatile ID at this (x,y) offset."
#
# Result: snow_and_stone now has a torii gate built into it. You open Munen
# in Porymap, scroll the metatile selector to slot 480+, and paint those
# metatiles as a block at whatever position in Munen you want a torii.
def cmd_torii_compose(args):
    Image = need_pil()
    print("[torii-compose] Building torii metatiles directly into snow_and_stone")

    TORII_DIR    = PROJ / "data" / "tilesets" / "secondary" / "torii_magiscarf"
    TORII_TILES  = TORII_DIR / "tiles.png"
    TORII_META   = TORII_DIR / "metatiles.bin"
    TORII_ATTRS  = TORII_DIR / "metatile_attributes.bin"
    TORII_PAL07  = TORII_DIR / "palettes" / "07.pal"

    PREFABS_JSON = PROJ / "prefabs.json"

    for p in (TORII_TILES, TORII_META, TORII_ATTRS, TORII_PAL07, PREFABS_JSON,
              TILES_PNG, META_BIN, ATTR_BIN):
        if not p.exists():
            fail(f"missing {p}")

    TILES_PER_ROW   = 16             # tiles.png is 128 px wide / 8 = 16 tiles
    NUM_PRIMARY     = 512            # primary tileset tile slot count
    BYTES_PER_META  = 16             # 2 layers x 4 tiles x 2 bytes (no triple-layer)
    NUM_METATILES   = 512
    DEST_START_SLOT = 480            # where to write torii metatiles in snow

    # --- 1. Read both tile sheets ---
    snow_im  = Image.open(TILES_PNG)
    torii_im = Image.open(TORII_TILES)
    print(f"  snow tiles.png: {snow_im.size}  mode={snow_im.mode}")
    print(f"  torii tiles.png: {torii_im.size}  mode={torii_im.mode}")

    snow_tile_rows_orig = snow_im.height // 8   # number of 8-px tile rows
    snow_tile_count     = snow_tile_rows_orig * TILES_PER_ROW
    torii_tile_rows     = torii_im.height // 8
    torii_tile_count    = torii_tile_rows * TILES_PER_ROW
    print(f"  snow has {snow_tile_count} tiles  ({snow_tile_rows_orig} rows)")
    print(f"  torii has {torii_tile_count} tiles ({torii_tile_rows} rows)")

    # --- 2. Parse prefab to find torii metatile IDs ---
    prefabs = json.loads(PREFABS_JSON.read_text())
    target_prefab = None
    for p in prefabs:
        if p["name"] == "torii-3":      # smallest single torii (8x8)
            target_prefab = p
            break
    if target_prefab is None:
        # fall back to first torii prefab found
        for p in prefabs:
            if p["name"].startswith("torii"):
                target_prefab = p
                break
    if target_prefab is None:
        fail("no torii prefab found in prefabs.json")
    print(f"  using prefab: {target_prefab['name']}  "
          f"({target_prefab['width']}x{target_prefab['height']} metatiles)")

    # Collect ORIGINAL metatile IDs (in torii_magiscarf) that the prefab uses.
    # mid >= NUM_PRIMARY means it lives in the secondary tileset; mid==1 is blank.
    orig_mids_in_use = []
    for cell in target_prefab["metatiles"]:
        mid = cell["metatile_id"]
        if mid > NUM_PRIMARY and mid not in orig_mids_in_use:
            orig_mids_in_use.append(mid)
    print(f"  prefab references {len(orig_mids_in_use)} unique secondary metatile IDs")

    # Free slot count in snow_and_stone
    free_slots = NUM_METATILES - DEST_START_SLOT
    if len(orig_mids_in_use) > free_slots:
        print(f"  WARN: prefab uses {len(orig_mids_in_use)} metatiles but only "
              f"{free_slots} slots free in snow at {DEST_START_SLOT}+. Truncating.")
        orig_mids_in_use = orig_mids_in_use[:free_slots]

    # Map: original mid -> new slot in snow_and_stone
    mid_remap = {}
    for i, orig in enumerate(orig_mids_in_use):
        mid_remap[orig] = DEST_START_SLOT + i
    # Blanks stay as mid=1
    mid_remap[1] = 1

    if not args.apply:
        print()
        print("  DRY RUN — would do all of the following:")
        print(f"    A. Append {torii_im.height}px ({torii_tile_count} tiles) of torii "
              f"tile data below snow's tile sheet")
        print(f"       (skipped if snow tiles.png is already merged)")
        print(f"    B. Install torii_magiscarf/palettes/07.pal -> snow/palettes/12.pal")
        print(f"    C. Copy {len(orig_mids_in_use)} torii metatile entries into "
              f"snow_and_stone/metatiles.bin slots {DEST_START_SLOT}-"
              f"{DEST_START_SLOT+len(orig_mids_in_use)-1}")
        print(f"    D. Re-index tile refs (+{snow_tile_count}) and force palette=12")
        print(f"    E. Copy matching metatile_attributes")
        print(f"    F. Print paint-map for placing the gate in Munen")
        print()
        print("  Re-run with --apply to commit.")
        return

    # ===================== APPLY =====================
    backup(TILES_PNG)
    backup(META_BIN)
    backup(ATTR_BIN)

    # --- A. Append torii tiles to snow tiles.png (if not already done) ---
    # Heuristic: if snow's height already includes the torii rows we'd add,
    # assume it's already merged.
    expected_merged_h = (snow_tile_rows_orig + torii_tile_rows) * 8
    if snow_im.height >= expected_merged_h:
        print(f"  A. snow tiles.png already at >= {expected_merged_h}px; skipping append")
    else:
        snow_p  = snow_im  if snow_im.mode  == "P" else snow_im.convert("P")
        torii_p = torii_im if torii_im.mode == "P" else torii_im.convert("P")
        new_h = snow_p.height + torii_p.height
        width = max(snow_p.width, torii_p.width)
        merged_bytes = bytearray(width * new_h)
        snow_bytes  = bytes(snow_p.tobytes())
        torii_bytes = bytes(torii_p.tobytes())
        for y in range(snow_p.height):
            merged_bytes[y*width : y*width + snow_p.width] = \
                snow_bytes[y*snow_p.width : (y+1)*snow_p.width]
        for y in range(torii_p.height):
            dy = snow_p.height + y
            merged_bytes[dy*width : dy*width + torii_p.width] = \
                torii_bytes[y*torii_p.width : (y+1)*torii_p.width]
        merged = Image.frombytes("P", (width, new_h), bytes(merged_bytes))
        merged.putpalette(snow_p.getpalette())
        merged.save(TILES_PNG)
        print(f"  A. wrote merged tiles.png ({merged.size})")

    # --- B. Install torii palette as snow slot 12 ---
    snow_pal12     = SNOW / "palettes" / "12.pal"
    snow_gbapal12  = SNOW / "palettes" / "12.gbapal"
    if snow_pal12.exists():
        backup(snow_pal12)
    if snow_gbapal12.exists():
        backup(snow_gbapal12)
    shutil.copy2(TORII_PAL07, snow_pal12)
    if snow_gbapal12.exists():
        snow_gbapal12.unlink()
    print(f"  B. installed torii palette -> palettes/12.pal (gbapal will regen on next make)")

    # --- C+D. Read torii metatiles and re-index ---
    torii_meta_bytes = TORII_META.read_bytes()
    torii_attr_bytes = TORII_ATTRS.read_bytes()
    snow_meta_bytes  = bytearray(META_BIN.read_bytes())
    snow_attr_bytes  = bytearray(ATTR_BIN.read_bytes())

    # Ensure files are big enough
    if len(snow_meta_bytes) < NUM_METATILES * BYTES_PER_META:
        snow_meta_bytes.extend(b"\x00" * (NUM_METATILES * BYTES_PER_META - len(snow_meta_bytes)))
    if len(snow_attr_bytes) < NUM_METATILES * 2:
        snow_attr_bytes.extend(b"\x00" * (NUM_METATILES * 2 - len(snow_attr_bytes)))

    TILE_OFFSET = snow_tile_count

    for orig_mid, new_slot in mid_remap.items():
        if orig_mid == 1:
            continue
        orig_idx = orig_mid - NUM_PRIMARY        # index inside torii_magiscarf
        src_off = orig_idx * BYTES_PER_META
        if src_off + BYTES_PER_META > len(torii_meta_bytes):
            print(f"     skip mid {orig_mid}: out of torii bin range")
            continue

        new_entry = bytearray(BYTES_PER_META)
        for tile_idx in range(8):
            word = struct.unpack_from("<H", torii_meta_bytes, src_off + tile_idx * 2)[0]
            tile_id = word & 0x3FF
            xflip   = (word >> 10) & 1
            yflip   = (word >> 11) & 1
            # NOTE: original palette intentionally discarded — forced to 12
            if tile_id >= NUM_PRIMARY:
                new_tile_id = tile_id + TILE_OFFSET
                if new_tile_id > 0x3FF:
                    print(f"     WARN: tile_id {new_tile_id} overflows 10-bit field")
                    new_tile_id &= 0x3FF
            else:
                new_tile_id = tile_id     # primary tileset reference, leave alone
            new_palette = 12
            new_word = (new_tile_id & 0x3FF) | (xflip << 10) | (yflip << 11) | (new_palette << 12)
            struct.pack_into("<H", new_entry, tile_idx * 2, new_word)

        dst_off = new_slot * BYTES_PER_META
        snow_meta_bytes[dst_off : dst_off + BYTES_PER_META] = new_entry

        # Copy attribute too
        src_attr = torii_attr_bytes[orig_idx * 2 : orig_idx * 2 + 2]
        snow_attr_bytes[new_slot * 2 : new_slot * 2 + 2] = src_attr

    META_BIN.write_bytes(bytes(snow_meta_bytes))
    ATTR_BIN.write_bytes(bytes(snow_attr_bytes))
    print(f"  C+D+E. wrote {len(orig_mids_in_use)} metatiles to slots "
          f"{DEST_START_SLOT}-{DEST_START_SLOT+len(orig_mids_in_use)-1}")

    # --- F. Print paint-map ---
    print()
    print("=== TORII PAINT MAP ===")
    print(f"Open Munen_village_2 in Porymap.")
    print(f"Pick where you want the torii. Paint each cell of the grid below at")
    print(f"that anchor + (dx, dy). 'blank' means leave the existing map metatile.")
    print()
    width  = target_prefab["width"]
    height = target_prefab["height"]
    grid = [["blank"] * width for _ in range(height)]
    for cell in target_prefab["metatiles"]:
        orig = cell["metatile_id"]
        new_mid = mid_remap.get(orig)
        if new_mid is not None and new_mid != 1:
            grid[cell["y"]][cell["x"]] = f"{NUM_PRIMARY + new_mid:4d}"
        else:
            grid[cell["y"]][cell["x"]] = "    "
    print(f"  (paint these metatile IDs in a {width}x{height} block)")
    print(f"     " + " ".join(f"x={i:>2}" for i in range(width)))
    for y, row in enumerate(grid):
        print(f"  y={y:>2}  " + " ".join(row))
    print()
    print("In Porymap:")
    print("  1. File -> Reload Project (so it sees the new palette + metatiles)")
    print("  2. Open Munen_village_2 -> Map tab")
    print("  3. In the metatile selector at the right, scroll to find the new")
    print(f"     metatiles starting at slot {NUM_PRIMARY + DEST_START_SLOT}.")
    print("  4. Click each cell to pick it, then click in the map to paint.")
    print("  5. Or use Tools -> Region Map / Multi-select to paint the whole")
    print("     grid at once if you select all the new metatiles as a group.")
    print("  6. Save the map.")
    print()
    print("Then: cd pokeemerald-expansion && make -j$(nproc)")

# =====================================================================
# add-snow-roofs-copy -- append the user's cropped snow rooftops PNG
# =====================================================================
# Source PNG: tilesets_raw/snow_and_stone_by_magiscarf_dcjgz10 - Copy.png
#   This is a 95x51 RGBA crop of the magiscarf source containing just the
#   snowy-roof tile variants the user wants. The 95-wide crop isn't tile-
#   aligned; we pad to 128 wide and the next multiple of 8 tall before
#   appending below snow_and_stone/tiles.png.
#
# No palette install: this is the SAME source image as the existing snow
# tiles so palettes already match.
def cmd_add_snow_roofs_copy(args):
    Image = need_pil()
    print("[add-snow-roofs-copy] Appending Copy.png to snow_and_stone/tiles.png")

    src_png = SCRIPT_DIR / "tilesets_raw" / "snow_and_stone_by_magiscarf_dcjgz10 - Copy.png"
    if not src_png.exists():
        fail(f"missing {src_png}")
    if not TILES_PNG.exists():
        fail(f"missing {TILES_PNG}")

    src = Image.open(src_png)
    snow = Image.open(TILES_PNG)
    print(f"  source: {src.size}  mode={src.mode}")
    print(f"  snow:   {snow.size}  mode={snow.mode}")

    # Pad source to 128 wide and next 8-tall boundary
    TARGET_W = snow.width
    pad_h = ((src.height + 7) // 8) * 8
    src_rgba = src.convert("RGBA")
    padded = Image.new("RGBA", (TARGET_W, pad_h), (0, 0, 0, 0))
    padded.paste(src_rgba, (0, 0))
    print(f"  padded source to {padded.size}")

    if not args.apply:
        new_h = snow.height + pad_h
        print()
        print(f"  DRY RUN — would append {pad_h}px to snow tiles.png")
        print(f"  new snow tiles.png size: {snow.width}x{new_h}  "
              f"({(snow.width//8)*(new_h//8)} tiles)")
        print("  Re-run with --apply to commit.")
        return

    backup(TILES_PNG)
    # Convert padded source to P mode using snow's palette, then concat as bytes.
    snow_p = snow if snow.mode == "P" else snow.convert("P")
    padded_p = padded.convert("P", palette=Image.Palette.ADAPTIVE, colors=16)
    # Re-quantize padded to use snow's palette via remap_palette won't always work
    # cleanly; use quantize with snow's palette as reference
    try:
        padded_p = padded_p.quantize(palette=snow_p)
    except Exception as e:
        print(f"  WARN: palette remap failed ({e}); using raw indexed instead")
        padded_p = padded.convert("P")

    new_h = snow_p.height + padded_p.height
    merged_bytes = bytearray(TARGET_W * new_h)
    snow_bytes = bytes(snow_p.tobytes())
    pad_bytes  = bytes(padded_p.tobytes())
    for y in range(snow_p.height):
        merged_bytes[y*TARGET_W : (y+1)*TARGET_W] = snow_bytes[y*TARGET_W : (y+1)*TARGET_W]
    for y in range(padded_p.height):
        dy = snow_p.height + y
        merged_bytes[dy*TARGET_W : (dy+1)*TARGET_W] = pad_bytes[y*TARGET_W : (y+1)*TARGET_W]
    merged = Image.frombytes("P", (TARGET_W, new_h), bytes(merged_bytes))
    merged.putpalette(snow_p.getpalette())
    merged.save(TILES_PNG)
    print(f"  wrote {TILES_PNG}  ({merged.size}, "
          f"{(TARGET_W//8)*(new_h//8)} total tiles)")
    print()
    print("Next: open Porymap, the new snow-roof tiles are at the BOTTOM of")
    print("the tile selector. Build metatiles in empty slots, or replace")
    print("unused stock slots if you're tight on metatile space.")

# =====================================================================
# add-halcyon-trees -- crop snow-tree band from Halcyon source, append
# =====================================================================
# Source PNG: tilesets_raw/pokemon_halcyon___snowy_train_outdoors_by_ekat99
#   _dfbfwgc.png (256x1216, mode P)
# Crop: x=0..128 (left half), y=384..576 (snow-tree band)
#   = 128 x 192 = 384 tiles
# Install Halcyon's extracted 16-color palette as snow's palette slot 11
def cmd_add_halcyon_trees(args):
    Image = need_pil()
    print("[add-halcyon-trees] Cropping snow-tree band from Halcyon, appending to snow")

    src_png = SCRIPT_DIR / "tilesets_raw" / \
              "pokemon_halcyon___snowy_train_outdoors_by_ekat99_dfbfwgc.png"
    if not src_png.exists():
        fail(f"missing {src_png}")
    if not TILES_PNG.exists():
        fail(f"missing {TILES_PNG}")

    src = Image.open(src_png)
    snow = Image.open(TILES_PNG)
    print(f"  source: {src.size}  mode={src.mode}")
    print(f"  snow:   {snow.size}  mode={snow.mode}")

    # Crop the snow tree band — left half, y=384 to y=576
    CROP_X0, CROP_Y0 = 0, 384
    CROP_X1, CROP_Y1 = 128, 576
    cropped = src.crop((CROP_X0, CROP_Y0, CROP_X1, CROP_Y1))
    print(f"  cropped to {cropped.size} (x={CROP_X0}-{CROP_X1}, y={CROP_Y0}-{CROP_Y1})")
    crop_tile_count = (cropped.width // 8) * (cropped.height // 8)
    print(f"  that's {crop_tile_count} new tiles")

    snow_pal11    = SNOW / "palettes" / "11.pal"
    snow_gbapal11 = SNOW / "palettes" / "11.gbapal"

    if not args.apply:
        new_h = snow.height + cropped.height
        total_tiles = (snow.width // 8) * (new_h // 8)
        print()
        print(f"  DRY RUN — would:")
        print(f"    1. Append cropped Halcyon to snow tiles.png "
              f"({snow.width}x{snow.height} -> {snow.width}x{new_h})")
        print(f"    2. Total tiles after merge: {total_tiles}")
        print(f"    3. Extract 16-color palette from cropped Halcyon")
        print(f"    4. Install as snow_and_stone/palettes/11.pal")
        print()
        if total_tiles > 1024:
            print(f"  WARNING: {total_tiles} > 1024 may exceed engine cap. Build risk.")
        print("  Re-run with --apply to commit.")
        return

    backup(TILES_PNG)
    if snow_pal11.exists():
        backup(snow_pal11)
    if snow_gbapal11.exists():
        backup(snow_gbapal11)

    # Extract 16-color palette from the cropped section
    cropped_quant = cropped.convert("RGB").quantize(colors=16, dither=Image.Dither.NONE)
    pal = cropped_quant.getpalette()[:48]  # 16 colors * 3 bytes RGB

    # Write JASC-PAL format
    pal_lines = ["JASC-PAL", "0100", "16"]
    for i in range(16):
        r = pal[i*3] if i*3 < len(pal) else 0
        g = pal[i*3+1] if i*3+1 < len(pal) else 0
        b = pal[i*3+2] if i*3+2 < len(pal) else 0
        pal_lines.append(f"{r} {g} {b}")
    snow_pal11.write_text("\n".join(pal_lines) + "\n")
    print(f"  wrote {snow_pal11}")
    if snow_gbapal11.exists():
        snow_gbapal11.unlink()
        print(f"  deleted {snow_gbapal11} (make will regen)")

    # Append cropped to snow tiles.png in P mode
    snow_p = snow if snow.mode == "P" else snow.convert("P")
    cropped_p = cropped_quant  # already P mode after quantize

    new_h = snow_p.height + cropped_p.height
    width = snow_p.width
    merged_bytes = bytearray(width * new_h)
    snow_bytes = bytes(snow_p.tobytes())
    crop_bytes = bytes(cropped_p.tobytes())
    for y in range(snow_p.height):
        merged_bytes[y*width : (y+1)*width] = snow_bytes[y*width : (y+1)*width]
    for y in range(cropped_p.height):
        dy = snow_p.height + y
        merged_bytes[dy*width : (dy+1)*width] = crop_bytes[y*width : (y+1)*width]
    merged = Image.frombytes("P", (width, new_h), bytes(merged_bytes))
    merged.putpalette(snow_p.getpalette())
    merged.save(TILES_PNG)
    total_tiles = (width//8) * (new_h//8)
    print(f"  wrote {TILES_PNG}  ({merged.size}, {total_tiles} total tiles)")
    print()
    if total_tiles > 1024:
        print(f"  *** WARNING: {total_tiles} tiles > 1024 engine cap.")
        print(f"  *** If make fails or tiles render as garbage, revert tiles.png from .bak")
    print()
    print("Next steps:")
    print("  1. cd pokeemerald-expansion && make -j$(nproc)")
    print("  2. In Porymap: open Munen, scroll the tile selector to the bottom.")
    print("     The Halcyon snow-tree tiles are there. Set tile palette to 11")
    print("     when building metatiles to get the proper Halcyon colors.")

# =====================================================================
# cleanup-and-add-snow-roofs -- the right way: overwrite UNUSED tile
# slots in the addressable range, then auto-compose metatiles
# =====================================================================
# Why this is necessary:
#   - pokeemerald has NUM_TILES_TOTAL=1024, primary=512 -> secondary cap=512
#     tile_ids (range 512-1023). PNG content past PNG row 31 is unaddressable.
#   - snow_and_stone tiles.png currently has 736 tiles (46 rows). The bottom
#     ~14 rows are physically present but the engine can't reach them.
#   - The user's metatiles reference 291 of the 512 secondary tiles.
#     ~221 tile slots in the addressable range are UNUSED.
#   - The right way to add new tile content: overwrite unused addressable
#     slots in-place. The PNG doesn't grow; the engine sees them.
#
# What this step does:
#   1. Restore tiles.png + metatiles.bin + metatile_attributes.bin from .bak
#      (wipes any torii-compose garbage from previous runs).
#   2. Parse the restored metatiles.bin to find which secondary tile_ids
#      (512-1023) are unused.
#   3. Quantize Copy.png to snow_and_stone's existing palette.
#   4. Pick the first N unused tile_ids (N = number of tiles in Copy.png),
#      overwrite tiles.png at those slot positions with Copy.png tile data.
#   5. Auto-compose 2x2 metatiles from the new tile data, placed in unused
#      metatile slots starting at slot 480 (one metatile = 4 new tiles).
#   6. Set metatile attributes: palette 7 (snow palette), layer COVERED.
#   7. Print the list of new paintable metatile IDs for Munen.
def cmd_cleanup_and_add_snow_roofs(args):
    Image = need_pil()
    print("[cleanup-and-add-snow-roofs] Cleaning torii garbage + adding snow roofs")

    src_png = SCRIPT_DIR / "tilesets_raw" / "snow_and_stone_by_magiscarf_dcjgz10 - Copy.png"
    if not src_png.exists():
        fail(f"missing {src_png}")
    if not TILES_PNG.exists() or not META_BIN.exists() or not ATTR_BIN.exists():
        fail(f"missing snow_and_stone files")

    NUM_PRIMARY     = 512
    NUM_SECONDARY   = 512                # tile slot capacity
    BYTES_PER_META  = 16
    NUM_METATILES   = 512
    DEST_MT_START   = 480
    SNOW_PALETTE    = 7                  # palette slot with snow colors
    TILES_PER_ROW   = 16

    # --- Step 1: restore from .bak if present ---
    print()
    print("Step 1: restoring tileset files from .bak (removes torii garbage)")
    for f in (TILES_PNG, META_BIN, ATTR_BIN):
        bak = f.with_suffix(f.suffix + ".bak")
        if bak.exists() and not args.skip_restore:
            shutil.copy2(bak, f)
            print(f"  restored {f.name} from .bak")
        else:
            print(f"  {f.name}: no .bak or skip-restore set, leaving as-is")

    # --- Step 2: parse referenced tile_ids ---
    snow_meta_bytes = bytearray(META_BIN.read_bytes())
    if len(snow_meta_bytes) < NUM_METATILES * BYTES_PER_META:
        snow_meta_bytes.extend(b"\x00" * (NUM_METATILES * BYTES_PER_META - len(snow_meta_bytes)))

    referenced_secondary = set()
    for mid in range(NUM_METATILES):
        off = mid * BYTES_PER_META
        for ti in range(8):
            word = struct.unpack_from("<H", snow_meta_bytes, off + ti*2)[0]
            tid = word & 0x3FF
            if NUM_PRIMARY <= tid < NUM_PRIMARY + NUM_SECONDARY:
                referenced_secondary.add(tid)

    unused_tile_ids = sorted(set(range(NUM_PRIMARY, NUM_PRIMARY + NUM_SECONDARY))
                             - referenced_secondary)
    print(f"\nStep 2: tile usage analysis")
    print(f"  referenced secondary tiles: {len(referenced_secondary)}")
    print(f"  unused (writable) tile slots: {len(unused_tile_ids)}")

    # Find unused metatile slots (from DEST_MT_START up)
    unused_mt_slots = []
    for mid in range(DEST_MT_START, NUM_METATILES):
        off = mid * BYTES_PER_META
        chunk = snow_meta_bytes[off : off + BYTES_PER_META]
        if chunk == b"\x00" * BYTES_PER_META or all(struct.unpack_from("<H", chunk, i*2)[0] == 0 for i in range(8)):
            unused_mt_slots.append(mid)
    print(f"  unused metatile slots {DEST_MT_START}+: {len(unused_mt_slots)}")

    # --- Step 3: load Copy.png, quantize to snow's palette ---
    print(f"\nStep 3: loading and quantizing Copy.png")
    copy_im = Image.open(src_png).convert("RGBA")
    snow_im = Image.open(TILES_PNG)
    snow_p  = snow_im if snow_im.mode == "P" else snow_im.convert("P")
    print(f"  Copy.png: {copy_im.size}  -> will quantize to snow palette")
    print(f"  snow tiles.png: {snow_im.size}  mode={snow_im.mode}")

    # Pad Copy to multiples of 8 in both dimensions
    pad_w = ((copy_im.width + 7) // 8) * 8
    pad_h = ((copy_im.height + 7) // 8) * 8
    padded_rgba = Image.new("RGBA", (pad_w, pad_h), (0, 0, 0, 0))
    padded_rgba.paste(copy_im, (0, 0))

    # Quantize to snow's palette via PIL
    snow_pal_ref = snow_p.copy()
    try:
        padded_quant = padded_rgba.convert("RGB").quantize(palette=snow_pal_ref, dither=Image.Dither.NONE)
    except Exception as e:
        print(f"  WARN: palette remap fell back ({e})")
        padded_quant = padded_rgba.convert("P")
    print(f"  padded + quantized: {padded_quant.size}")

    # --- Step 4: pick non-blank tiles from Copy + overwrite unused snow slots ---
    print(f"\nStep 4: choosing which Copy tiles to import")
    new_tile_data = []   # list of (8x8 tile bytes, original (tx, ty))
    pad_bytes = bytes(padded_quant.tobytes())
    for ty in range(pad_h // 8):
        for tx in range(pad_w // 8):
            tile = bytearray(64)
            blank = True
            for py in range(8):
                row_src = (ty*8 + py) * pad_w + tx*8
                for px in range(8):
                    v = pad_bytes[row_src + px]
                    tile[py*8 + px] = v
                    if v != 0:
                        blank = False
            if not blank:
                new_tile_data.append((bytes(tile), tx, ty))

    print(f"  found {len(new_tile_data)} non-blank tiles in Copy.png")
    if len(new_tile_data) > len(unused_tile_ids):
        print(f"  WARN: only {len(unused_tile_ids)} unused slots, truncating")
        new_tile_data = new_tile_data[:len(unused_tile_ids)]

    if not args.apply:
        print()
        print("DRY RUN — would do:")
        print(f"  - restore tiles.png/.bin from .bak ({'available' if (TILES_PNG.with_suffix('.png.bak')).exists() else 'none'})")
        print(f"  - overwrite {len(new_tile_data)} unused tile slots with Copy.png content")
        print(f"  - compose ~{len(new_tile_data)//4} new 2x2 metatiles starting at slot {DEST_MT_START}")
        print(f"  - paintable metatile IDs: {NUM_PRIMARY + DEST_MT_START} - {NUM_PRIMARY + DEST_MT_START + len(new_tile_data)//4 - 1}")
        print()
        print("Re-run with --apply to commit.")
        return

    # APPLY ---------
    backup(TILES_PNG)
    backup(META_BIN)
    backup(ATTR_BIN)

    # Overwrite tile bytes in snow tiles.png
    snow_bytes = bytearray(snow_p.tobytes())
    snow_w = snow_p.width
    assigned_tile_ids = []
    for (tile_bytes, _, _), tid in zip(new_tile_data, unused_tile_ids):
        rel = tid - NUM_PRIMARY
        tx = (rel % TILES_PER_ROW) * 8
        ty = (rel // TILES_PER_ROW) * 8
        for py in range(8):
            dst = (ty + py) * snow_w + tx
            snow_bytes[dst : dst + 8] = tile_bytes[py*8 : (py+1)*8]
        assigned_tile_ids.append(tid)

    new_snow = Image.frombytes("P", (snow_w, snow_p.height), bytes(snow_bytes))
    new_snow.putpalette(snow_p.getpalette())
    new_snow.save(TILES_PNG)
    print(f"\n  wrote {TILES_PNG} ({len(assigned_tile_ids)} tile slots overwritten)")

    # --- Step 5: auto-compose 2x2 metatiles ---
    snow_attr_bytes = bytearray(ATTR_BIN.read_bytes())
    if len(snow_attr_bytes) < NUM_METATILES * 2:
        snow_attr_bytes.extend(b"\x00" * (NUM_METATILES * 2 - len(snow_attr_bytes)))

    composed_mt_ids = []
    DEST_LAYER = 0x1000      # COVERED layer_type
    DEST_BEH   = 0x00        # MB_NORMAL behavior
    # Sort assigned tile_ids by their original (tx, ty) position so the
    # composed metatiles preserve visual continuity (top-left of source
    # becomes top-left of metatile).
    pairs = list(zip(assigned_tile_ids,
                     [(tx, ty) for (_, tx, ty) in new_tile_data]))
    # Group into 2x2 by source position: find pairs at (tx, ty), (tx+1, ty),
    # (tx, ty+1), (tx+1, ty+1) for each source 16x16 block.
    by_pos = {(tx, ty): tid for tid, (tx, ty) in pairs}
    used = set()
    src_blocks = []
    for tid, (tx, ty) in pairs:
        if (tx, ty) in used:
            continue
        if tx % 2 != 0 or ty % 2 != 0:
            continue  # only start at even positions for clean 2x2
        block = []
        for dy in (0, 1):
            for dx in (0, 1):
                tt = by_pos.get((tx + dx, ty + dy))
                block.append(tt)
                if tt is not None:
                    used.add((tx + dx, ty + dy))
        if any(t is None for t in block):
            continue  # incomplete 2x2 block
        src_blocks.append(block)

    mt_slot = DEST_MT_START
    for block in src_blocks:
        if mt_slot >= NUM_METATILES:
            break
        off = mt_slot * BYTES_PER_META
        # Bottom layer: the 4 tiles (top-left, top-right, bottom-left, bottom-right)
        for ti, tid in enumerate(block):
            word = (tid & 0x3FF) | (SNOW_PALETTE << 12)
            struct.pack_into("<H", snow_meta_bytes, off + ti*2, word)
        # Top layer: all transparent (tile_id=0, no draw)
        for ti in range(4, 8):
            struct.pack_into("<H", snow_meta_bytes, off + ti*2, 0)
        struct.pack_into("<H", snow_attr_bytes, mt_slot * 2, DEST_LAYER | DEST_BEH)
        composed_mt_ids.append(NUM_PRIMARY + mt_slot)
        mt_slot += 1

    META_BIN.write_bytes(bytes(snow_meta_bytes))
    ATTR_BIN.write_bytes(bytes(snow_attr_bytes))
    print(f"  composed {len(composed_mt_ids)} metatiles in slots "
          f"{DEST_MT_START}-{DEST_MT_START + len(composed_mt_ids) - 1}")
    print(f"  paintable metatile IDs in Munen: {composed_mt_ids}")

    print()
    print("Next steps:")
    print("  1. cd pokeemerald-expansion && make -j$(nproc)")
    print("  2. In Porymap, reload project (File -> Reload Project)")
    print("  3. Open Munen, the new snow-roof metatiles are at slot 992+")
    print(f"     (i.e. metatile IDs {composed_mt_ids[0] if composed_mt_ids else 'N/A'}+)")
    print("  4. Paint them where you want snowy roofs")

# =====================================================================
# add-halcyon-trees-overwrite -- snow trees via overwrite-unused-slots
# =====================================================================
# Same pattern as cleanup-and-add-snow-roofs, but for Halcyon snow trees:
#   - Does NOT restore from .bak (preserves snow roof additions)
#   - Source: pokemon_halcyon___snowy_train_outdoors_by_ekat99_dfbfwgc.png
#   - Crop region: x=0..128, y=432..480 (left half, snow tree band)
#     = 16 wide x 6 tall = 96 source tiles
#   - Extracts a 16-color palette from the crop, installs as snow slot 11
#   - Overwrites unused secondary tile slots with Halcyon tile data
#   - Auto-composes 2x2 metatiles using palette 11 in free metatile slots
def cmd_add_halcyon_trees_overwrite(args):
    Image = need_pil()
    print("[add-halcyon-trees-overwrite] Adding Halcyon snow trees via overwrite-unused-slots")

    src_png = SCRIPT_DIR / "tilesets_raw" / \
              "pokemon_halcyon___snowy_train_outdoors_by_ekat99_dfbfwgc.png"
    if not src_png.exists():
        fail(f"missing {src_png}")
    if not TILES_PNG.exists() or not META_BIN.exists() or not ATTR_BIN.exists():
        fail("missing snow_and_stone files")

    NUM_PRIMARY    = 512
    NUM_SECONDARY  = 512
    BYTES_PER_META = 16
    NUM_METATILES  = 512
    TILES_PER_ROW  = 16
    HALCYON_PAL    = 11           # install Halcyon palette here (was empty)
    CROP_X0, CROP_Y0 = 0, 432
    CROP_X1, CROP_Y1 = 128, 480   # 128 x 48 = 96 tiles
    MT_SEARCH_START  = 256        # search for free metatile slots from here

    # --- find unused tile slots ---
    snow_meta_bytes = bytearray(META_BIN.read_bytes())
    if len(snow_meta_bytes) < NUM_METATILES * BYTES_PER_META:
        snow_meta_bytes.extend(b"\x00" * (NUM_METATILES * BYTES_PER_META - len(snow_meta_bytes)))

    referenced_secondary = set()
    for mid in range(NUM_METATILES):
        off = mid * BYTES_PER_META
        for ti in range(8):
            word = struct.unpack_from("<H", snow_meta_bytes, off + ti*2)[0]
            tid = word & 0x3FF
            if NUM_PRIMARY <= tid < NUM_PRIMARY + NUM_SECONDARY:
                referenced_secondary.add(tid)
    unused_tile_ids = sorted(set(range(NUM_PRIMARY, NUM_PRIMARY + NUM_SECONDARY))
                             - referenced_secondary)
    print(f"  unused tile slots: {len(unused_tile_ids)}")

    # --- find unused metatile slots ---
    unused_mt_slots = []
    for mid in range(MT_SEARCH_START, NUM_METATILES):
        off = mid * BYTES_PER_META
        if all(struct.unpack_from("<H", snow_meta_bytes, off + i*2)[0] == 0 for i in range(8)):
            unused_mt_slots.append(mid)
    print(f"  unused metatile slots ({MT_SEARCH_START}+): {len(unused_mt_slots)}")

    # --- crop Halcyon, quantize to 16 colors ---
    halcyon = Image.open(src_png)
    print(f"  Halcyon source: {halcyon.size} mode={halcyon.mode}")
    cropped = halcyon.crop((CROP_X0, CROP_Y0, CROP_X1, CROP_Y1))
    print(f"  cropped to ({CROP_X0},{CROP_Y0}) - ({CROP_X1},{CROP_Y1}) = {cropped.size}")

    cropped_rgb = cropped.convert("RGB")
    cropped_quant = cropped_rgb.quantize(colors=16, dither=Image.Dither.NONE)
    extracted_pal = cropped_quant.getpalette()[:48]
    print(f"  quantized to 16 colors")

    # Extract non-blank tiles
    crop_w = cropped_quant.width
    crop_h = cropped_quant.height
    crop_bytes = bytes(cropped_quant.tobytes())
    new_tile_data = []   # list of (tile_bytes, src_tx, src_ty)
    for ty in range(crop_h // 8):
        for tx in range(crop_w // 8):
            tile = bytearray(64)
            blank = True
            for py in range(8):
                row_src = (ty*8 + py) * crop_w + tx*8
                for px in range(8):
                    v = crop_bytes[row_src + px]
                    tile[py*8 + px] = v
                    if v != 0:
                        blank = False
            if not blank:
                new_tile_data.append((bytes(tile), tx, ty))
    print(f"  non-blank tiles in crop: {len(new_tile_data)}")

    if len(new_tile_data) > len(unused_tile_ids):
        print(f"  WARN: truncating to {len(unused_tile_ids)} tiles (budget limit)")
        new_tile_data = new_tile_data[:len(unused_tile_ids)]

    # Predict metatile composition count
    by_pos_preview = {(tx, ty): True for (_, tx, ty) in new_tile_data}
    pred_blocks = 0
    seen_preview = set()
    for (_, tx, ty) in new_tile_data:
        if (tx, ty) in seen_preview or tx % 2 != 0 or ty % 2 != 0:
            continue
        if all((tx+dx, ty+dy) in by_pos_preview for dx in (0,1) for dy in (0,1)):
            pred_blocks += 1
            for dx in (0,1):
                for dy in (0,1):
                    seen_preview.add((tx+dx, ty+dy))
    pred_metatiles = min(pred_blocks, len(unused_mt_slots))

    if not args.apply:
        print()
        print("DRY RUN — would do:")
        print(f"  - Install Halcyon palette as snow_and_stone/palettes/11.pal")
        print(f"  - Overwrite {len(new_tile_data)} unused tile slots with Halcyon snow trees")
        print(f"  - Auto-compose ~{pred_metatiles} 2x2 metatiles in free slots")
        print(f"  - Palette for new metatiles: {HALCYON_PAL}")
        print(f"  - Layer type: COVERED (0x1000). User can change to NORMAL")
        print(f"    in Porymap for walk-behind on tree tops.")
        print()
        print("Re-run with --apply to commit.")
        return

    # APPLY ---
    backup(TILES_PNG)
    backup(META_BIN)
    backup(ATTR_BIN)
    snow_pal11    = SNOW / "palettes" / "11.pal"
    snow_gbapal11 = SNOW / "palettes" / "11.gbapal"
    if snow_pal11.exists():
        backup(snow_pal11)
    if snow_gbapal11.exists():
        backup(snow_gbapal11)

    # Install palette
    pal_lines = ["JASC-PAL", "0100", "16"]
    for i in range(16):
        r = extracted_pal[i*3] if i*3 < len(extracted_pal) else 0
        g = extracted_pal[i*3+1] if i*3+1 < len(extracted_pal) else 0
        b = extracted_pal[i*3+2] if i*3+2 < len(extracted_pal) else 0
        pal_lines.append(f"{r} {g} {b}")
    snow_pal11.write_text("\n".join(pal_lines) + "\n")
    if snow_gbapal11.exists():
        snow_gbapal11.unlink()
    print(f"  wrote {snow_pal11.name}, deleted {snow_gbapal11.name} (make will regen)")

    # Overwrite tile slots
    snow_im = Image.open(TILES_PNG)
    snow_p  = snow_im if snow_im.mode == "P" else snow_im.convert("P")
    snow_bytes = bytearray(snow_p.tobytes())
    snow_w = snow_p.width

    assigned = []
    for (tile_bytes, _, _), tid in zip(new_tile_data, unused_tile_ids):
        rel = tid - NUM_PRIMARY
        tx = (rel % TILES_PER_ROW) * 8
        ty = (rel // TILES_PER_ROW) * 8
        for py in range(8):
            dst = (ty + py) * snow_w + tx
            snow_bytes[dst : dst + 8] = tile_bytes[py*8 : (py+1)*8]
        assigned.append(tid)
    new_snow = Image.frombytes("P", (snow_w, snow_p.height), bytes(snow_bytes))
    new_snow.putpalette(snow_p.getpalette())
    new_snow.save(TILES_PNG)
    print(f"  wrote {TILES_PNG} ({len(assigned)} tile slots overwritten)")

    # Auto-compose 2x2 metatiles
    snow_attr_bytes = bytearray(ATTR_BIN.read_bytes())
    if len(snow_attr_bytes) < NUM_METATILES * 2:
        snow_attr_bytes.extend(b"\x00" * (NUM_METATILES * 2 - len(snow_attr_bytes)))

    by_pos = {(tx, ty): tid for (_, tx, ty), tid in zip(new_tile_data, assigned)}
    used = set()
    blocks = []
    # Iterate in source order so visual continuity is preserved
    for (_, tx, ty) in new_tile_data:
        if (tx, ty) in used or tx % 2 != 0 or ty % 2 != 0:
            continue
        block = []
        for dy in (0, 1):
            for dx in (0, 1):
                tt = by_pos.get((tx + dx, ty + dy))
                block.append(tt)
                if tt is not None:
                    used.add((tx + dx, ty + dy))
        if all(t is not None for t in block):
            blocks.append(block)

    composed = []
    mt_iter = iter(unused_mt_slots)
    for block in blocks:
        try:
            slot = next(mt_iter)
        except StopIteration:
            break
        off = slot * BYTES_PER_META
        for ti, tid in enumerate(block):
            word = (tid & 0x3FF) | (HALCYON_PAL << 12)
            struct.pack_into("<H", snow_meta_bytes, off + ti*2, word)
        for ti in range(4, 8):
            struct.pack_into("<H", snow_meta_bytes, off + ti*2, 0)
        struct.pack_into("<H", snow_attr_bytes, slot * 2, 0x1000)   # COVERED
        composed.append(NUM_PRIMARY + slot)

    META_BIN.write_bytes(bytes(snow_meta_bytes))
    ATTR_BIN.write_bytes(bytes(snow_attr_bytes))
    print(f"  composed {len(composed)} snow-tree metatiles")
    if composed:
        print(f"  paintable metatile IDs in Munen: {composed}")

    print()
    print("Next:")
    print("  cd pokeemerald-expansion && make -j$(nproc)")
    print("  Reload Porymap (File -> Reload Project)")
    print("  Open Munen, scroll metatile selector to find the new snow-tree")
    print(f"  metatiles (IDs {composed[0] if composed else 'N/A'}+)")
    print("  Paint them where you want snow-capped trees!")
    print()
    print("  In Porymap's Tileset Editor you can change layer type to NORMAL")
    print("  on the upper tile rows so the player walks BEHIND the tree tops.")

# =====================================================================
# main
# =====================================================================
def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("step", choices=[
        "inspect-attrs", "set-walkbehind",
        "diag-signs",
        "probe-sprite", "composite-fixed", "install-sprite",
        "snow-roofs", "torii-merge", "torii-compose",
        "add-snow-roofs-copy", "add-halcyon-trees",
        "cleanup-and-add-snow-roofs",
        "add-halcyon-trees-overwrite",
    ])
    ap.add_argument("--slots", help="for set-walkbehind: comma list (ranges allowed: '4,5,12-15')")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--apply", action="store_true", help="for snow-roofs/torii-merge: actually write changes")
    ap.add_argument("--skip-restore", action="store_true", help="cleanup-and-add-snow-roofs: don't restore .bak files first")
    args = ap.parse_args()

    if not PROJ.exists():
        fail(f"pokeemerald-expansion not found at {PROJ}")

    {
        "inspect-attrs":   cmd_inspect_attrs,
        "set-walkbehind":  cmd_set_walkbehind,
        "diag-signs":      cmd_diag_signs,
        "probe-sprite":    cmd_probe_sprite,
        "composite-fixed": cmd_composite_fixed,
        "install-sprite":  cmd_install_sprite,
        "snow-roofs":      cmd_snow_roofs,
        "torii-merge":     cmd_torii_merge,
        "torii-compose":   cmd_torii_compose,
        "add-snow-roofs-copy": cmd_add_snow_roofs_copy,
        "add-halcyon-trees":   cmd_add_halcyon_trees,
        "cleanup-and-add-snow-roofs": cmd_cleanup_and_add_snow_roofs,
        "add-halcyon-trees-overwrite": cmd_add_halcyon_trees_overwrite,
    }[args.step](args)

if __name__ == "__main__":
    main()
