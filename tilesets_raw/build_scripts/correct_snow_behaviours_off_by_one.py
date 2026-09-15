"""One-off correction (2026-09-14) for gTileset_SnowStone behaviours.

fix_snow_behaviours.py (round 67) parsed metatile_behaviors.h with a regex that
skipped any enum line carrying a trailing comment. MB_INTERIOR_DEEP_WATER has
one, so every later value came out ONE LOW:
    ice   written 31 = MB_POKEMART_SIGN (mart-sign text, no slide)  -> MB_ICE 32
    snow  written 36 = MB_ASHGRASS      (wild encounters!)          -> MB_FOOTPRINTS 37
declaw_munen_water.py hardcoded the same wrong 36 for clones 400-406.

Only entries still holding the wrong value are changed. Joe has since re-set
some of these tiles in porymap (ice already 32, some snow to MB_NORMAL, two
clones back to MB_PUDDLE); those are left exactly as he made them. Behaviour
byte only; layer bits asserted unchanged. Values are checked against the enum
parsed with comments stripped.
"""
import os, re, struct

HERE = os.path.dirname(os.path.abspath(__file__))
G = os.path.normpath(os.path.join(HERE, "..", "..", "pokeemerald-expansion"))
ATTR = os.path.join(G, "data/tilesets/secondary/snow_stone/metatile_attributes.bin")

ICE = [201, 202, 203, 209, 210, 211, 217, 218, 219, 319, 327, 333, 334, 335, 340, 341, 343]
SNOW_GROUND = [0, 4, 5, 9, 15, 83, 84, 96, 97, 112, 141, 226, 249, 251, 268, 345]
CLONES = list(range(400, 407))


def behaviour_enum():
    s = open(os.path.join(G, "include/constants/metatile_behaviors.h")).read()
    body = s[s.index("{") + 1:s.index("}")]
    out, val = {}, 0
    for line in body.splitlines():
        line = line.split("//")[0].strip().rstrip(",").strip()   # drop trailing comments FIRST
        m = re.match(r"^(MB_[A-Z0-9_]+)\s*(?:=\s*(\S+))?$", line)
        if not m:
            continue
        if m.group(2):
            val = int(m.group(2), 0)
        out[m.group(1)] = val
        val += 1
    return out


mb = behaviour_enum()
# cross-checked against a compiled probe of the header on 2026-09-14
assert (mb["MB_POKEMART_SIGN"], mb["MB_ICE"], mb["MB_ASHGRASS"], mb["MB_FOOTPRINTS"]) == (31, 32, 36, 37), mb
WRONG_ICE, RIGHT_ICE = mb["MB_POKEMART_SIGN"], mb["MB_ICE"]
WRONG_SNOW, RIGHT_SNOW = mb["MB_ASHGRASS"], mb["MB_FOOTPRINTS"]

ab = bytearray(open(ATTR, "rb").read())
layers = [struct.unpack_from("<H", ab, i * 2)[0] & 0xF000 for i in range(len(ab) // 2)]
changed, kept = [], []
for ids, wrong, right in ((ICE, WRONG_ICE, RIGHT_ICE), (SNOW_GROUND + CLONES, WRONG_SNOW, RIGHT_SNOW)):
    for m in ids:
        v = struct.unpack_from("<H", ab, m * 2)[0]
        if (v & 0xFF) == wrong:
            struct.pack_into("<H", ab, m * 2, (v & 0xFF00) | right)
            changed.append(m)
        else:
            kept.append((m, v & 0xFF))
assert layers == [struct.unpack_from("<H", ab, i * 2)[0] & 0xF000 for i in range(len(ab) // 2)]
open(ATTR, "wb").write(ab)
print("corrected %d: %s" % (len(changed), ["0x%03X" % (512 + m) for m in changed]))
print("left as Joe set them: %s" % [("0x%03X" % (512 + m), b) for m, b in kept])
