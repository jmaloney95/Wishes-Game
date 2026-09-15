"""Correct the ice/snow metatile behaviours on gTileset_SnowStone.

WHAT WENT WRONG. The first pass read MB_ICE and MB_FOOTPRINTS out of a grep of
metatile_behaviors.h and used the LINE NUMBERS as the enum values. They are not
the same thing:

    MB_ICE          is 31, not 37 -- 37 is MB_THIN_ICE
    MB_FOOTPRINTS   is 36, not 42 -- 42 is MB_REFLECTION_UNDER_BRIDGE

which is exactly what Joe saw: the ice did not slide (MB_THIN_ICE only does
anything under the per-map cracked-ice task in Sootopolis Gym and Icefall Cave,
so on Munen Village it is inert), and the snow got a water reflection, hence
the underwater bubbles.

THE MASK WAS ALSO WRONG, though harmlessly. With metatile_attributes_size=2 the
Emerald layout is METATILE_ATTR_BEHAVIOR_MASK 0x00FF (bits 0-7) and
METATILE_ATTR_LAYER_MASK 0xF000 (bits 12-15). The first pass used 0x1FF, a
9-bit mask borrowed from the FRLG layout. Both values fit in 8 bits so nothing
was corrupted, but this uses 0x00FF.

Layer types are bits 12-15 and are NOT touched: Joe has since re-layered
metatiles by hand to fix what draws above the player, and that work must
survive. Only bits 0-7 are rewritten.
"""
import os
import re
import struct
import sys

ROOT = r"J:\ROM Hack Project\pokeemerald-expansion"
SNOW = os.path.join(ROOT, "data", "tilesets", "secondary", "snow_stone")
BEHAVIORS_H = os.path.join(ROOT, "include", "constants", "metatile_behaviors.h")

BEHAVIOR_MASK = 0x00FF          # bits 0-7; layer type at 12-15 is left alone

# From the earlier signature pass over the tileset. Ice is every metatile built
# from metatile 210's two cyans (98E8F8 / D0F8F8); snow is the pale, solid,
# walkable ground.
ICE = [201, 202, 203, 209, 210, 211, 217, 218, 219, 319, 327, 333, 334, 335,
       340, 341, 343]
SNOW_GROUND = [0, 4, 5, 9, 15, 83, 84, 96, 97, 112, 141, 226, 249, 251, 268, 345]

# What the first pass wrote, so this can report what it is correcting.
WRONG_ICE, WRONG_SNOW = 37, 42


def read_behaviour_enum():
    """Parse the real enum. Never take these from a grep's line numbers."""
    s = open(BEHAVIORS_H).read()
    body = s[s.index("{") + 1:s.index("}")]
    out, val = {}, 0
    for line in body.splitlines():
        # 2026-09-14: strip trailing comments first. Without this the line
        # "MB_INTERIOR_DEEP_WATER, // ..." was skipped and every later value
        # came out one low (ice 31, snow 36). See correct_snow_behaviours_off_by_one.py.
        line = line.split("//")[0].strip().rstrip(",").strip()
        if not line or line.startswith("//"):
            continue
        m = re.match(r"^(MB_[A-Z0-9_]+)\s*(?:=\s*(\S+))?$", line)
        if not m:
            continue
        if m.group(2):
            val = int(m.group(2), 0)
        out[m.group(1)] = val
        val += 1
    return out


def main():
    mbs = read_behaviour_enum()
    ice_v, foot_v = mbs["MB_ICE"], mbs["MB_FOOTPRINTS"]
    rev = {v: k for k, v in mbs.items()}
    print("  MB_ICE        = %d   (first pass wrote %d = %s)"
          % (ice_v, WRONG_ICE, rev.get(WRONG_ICE)))
    print("  MB_FOOTPRINTS = %d   (first pass wrote %d = %s)"
          % (foot_v, WRONG_SNOW, rev.get(WRONG_SNOW)))

    p = os.path.join(SNOW, "metatile_attributes.bin")
    ab = bytearray(open(p, "rb").read())
    n = len(ab) // 2
    nmeta = len(open(os.path.join(SNOW, "metatiles.bin"), "rb").read()) // 16
    assert n == nmeta, "attributes %d vs metatiles %d" % (n, nmeta)

    changed = 0
    layers_before = [struct.unpack_from("<H", ab, m * 2)[0] & 0xF000
                     for m in range(n)]
    for ids, want in ((ICE, ice_v), (SNOW_GROUND, foot_v)):
        for m in ids:
            if m >= n:
                print("  skipping metatile %d: past the end (%d)" % (m, n))
                continue
            v = struct.unpack_from("<H", ab, m * 2)[0]
            new = (v & ~BEHAVIOR_MASK) | want
            if new != v:
                struct.pack_into("<H", ab, m * 2, new)
                changed += 1

    layers_after = [struct.unpack_from("<H", ab, m * 2)[0] & 0xF000
                    for m in range(n)]
    assert layers_before == layers_after, "layer types changed -- aborting"
    print("  layer types (bits 12-15) verified untouched across all %d metatiles" % n)

    open(p, "wb").write(ab)
    print("  %d attributes corrected" % changed)

    got = {}
    for m in range(n):
        b = struct.unpack_from("<H", ab, m * 2)[0] & BEHAVIOR_MASK
        got.setdefault(b, []).append(m)
    for b in sorted(got):
        if b:
            print("     MB %-3d %-26s %d metatiles"
                  % (b, rev.get(b, "?"), len(got[b])))


if __name__ == "__main__":
    main()
