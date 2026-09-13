"""Stop Munen Village's snow behaving like water, without touching other maps.

THE PROBLEM. Joe painted parts of the village with gTileset_General's water
metatiles. They carry water behaviours, so the snow reflects the player
(MB_POND_WATER is in MetatileBehavior_IsReflective) and ripples underfoot
(MB_SHALLOW_WATER).

WHY THE OBVIOUS FIX IS WRONG. Changing those behaviours in gTileset_General
would fix Munen and break everywhere else: 14 layouts paint metatile 209 alone
-- Routes 2, 117 and 120, Safari Zone, Faraway Island, Southern Island, Tower
Top. Their water would stop being water.

WHAT THIS DOES INSTEAD. Each offending General metatile is CLONED into
gTileset_SnowStone with MB_FOOTPRINTS, and Munen Village's map.bin is repointed
at the clones. The clone is byte-identical in appearance -- a metatile is just
eight (tile, palette) entries, and those tiles are the PRIMARY's, which
SnowStone shares -- so it costs **zero tiles** and the map looks the same.
Collision and elevation bits in map.bin are preserved.
"""
import json
import os
import struct

ROOT = r"J:\ROM Hack Project\pokeemerald-expansion"
SNOW = os.path.join(ROOT, "data", "tilesets", "secondary", "snow_stone")
GEN = os.path.join(ROOT, "data", "tilesets", "primary", "general")
LAYOUTS = os.path.join(ROOT, "data", "layouts", "layouts.json")

MB_FOOTPRINTS = 36
BEHAVIOR_MASK = 0x00FF
LAYER_MASK = 0xF000
MARKER = ".declawed"

# behaviours that make snow act like water: reflective, or rippling underfoot
WATER_BEHAVIOURS = {
    16,   # MB_POND_WATER      -- reflective
    23,   # MB_SHALLOW_WATER   -- ripples / wading
    22,   # MB_PUDDLE          -- reflective
}


def main(force=False):
    marker = os.path.join(SNOW, MARKER)
    if os.path.exists(marker) and not force:
        print("  already done (%s: %s)" % (MARKER, open(marker).read().strip()))
        return

    gm = open(os.path.join(GEN, "metatiles.bin"), "rb").read()
    ga = open(os.path.join(GEN, "metatile_attributes.bin"), "rb").read()
    sm = bytearray(open(os.path.join(SNOW, "metatiles.bin"), "rb").read())
    sa = bytearray(open(os.path.join(SNOW, "metatile_attributes.bin"), "rb").read())
    n_snow = len(sm) // 16
    assert len(sa) // 2 == n_snow, "attributes/metatiles mismatch"

    lay = {l["id"]: l for l in json.load(open(LAYOUTS))["layouts"]}
    l = lay["LAYOUT_MUNEN_VILLAGE"]
    w, h = int(l["width"]), int(l["height"])
    # layouts.json paths are relative to the repo root, not to this script
    mp = os.path.join(ROOT, l["blockdata_filepath"].replace("/", os.sep))
    b = bytearray(open(mp, "rb").read())

    # which General metatiles does this map paint that behave like water?
    offenders = {}
    for i in range(0, len(b), 2):
        v = struct.unpack_from("<H", b, i)[0]
        mt = v & 0x3FF
        if mt >= 512:
            continue
        beh = struct.unpack_from("<H", ga, mt * 2)[0] & BEHAVIOR_MASK
        if beh in WATER_BEHAVIOURS:
            offenders.setdefault(mt, [0, beh])
            offenders[mt][0] += 1
    if not offenders:
        print("  nothing to do -- no water-behaviour General metatiles painted")
        return

    print("  General metatiles painted here that behave like water:")
    for mt, (cnt, beh) in sorted(offenders.items()):
        print("     metatile %3d  behaviour %2d  x%d" % (mt, beh, cnt))

    if n_snow + len(offenders) > 512:
        raise SystemExit("no room: %d + %d clones > 512" % (n_snow, len(offenders)))

    # clone each into SnowStone, appearance identical, behaviour MB_FOOTPRINTS
    remap = {}
    for k, mt in enumerate(sorted(offenders)):
        new = n_snow + k
        sm += gm[mt * 16:mt * 16 + 16]                 # same eight entries
        layer = struct.unpack_from("<H", ga, mt * 2)[0] & LAYER_MASK
        sa += struct.pack("<H", layer | MB_FOOTPRINTS)
        remap[mt] = 512 + new
        print("     General %3d -> SnowStone %3d (id %d)" % (mt, new, 512 + new))

    # repoint the map, keeping collision and elevation
    swapped = 0
    for i in range(0, len(b), 2):
        v = struct.unpack_from("<H", b, i)[0]
        mt = v & 0x3FF
        if mt in remap:
            struct.pack_into("<H", b, i, (v & ~0x3FF) | (remap[mt] & 0x3FF))
            swapped += 1

    open(os.path.join(SNOW, "metatiles.bin"), "wb").write(sm)
    open(os.path.join(SNOW, "metatile_attributes.bin"), "wb").write(sa)
    open(mp, "wb").write(b)
    for junk in ("tiles.4bpp", "tiles.4bpp.lz"):
        p = os.path.join(SNOW, junk)
        if os.path.exists(p):
            os.remove(p)

    open(marker, "w").write("%d General water metatiles cloned, %d map tiles "
                            "repointed\n" % (len(remap), swapped))
    print("  %d map tiles repointed; SnowStone now %d metatiles (no new TILES)"
          % (swapped, len(sm) // 16))


if __name__ == "__main__":
    import sys
    main(force="--force" in sys.argv)
