# -*- coding: utf-8 -*-
import sys
sys.exit("SUPERSEDED -- do not run. it adds red OBJ_EVENT_GFX_ITEM_BALL objects, which were replaced by painted ball tiles; running it would put them back on top of the tiles. Use the painted-tile pickups in data/maps/MunenVillage/scripts.inc instead.")
"""Three item balls in Munen Village, as standard finditem pickups.

Standard pattern, same as MunenTunnel's: an OBJ_EVENT_GFX_ITEM_BALL object
event with a finditem script and a hide flag. The object itself is what blocks
the tile -- the ground underneath stays passable and needs no impassable
painting -- and the flag makes the ball vanish once taken and stay gone.

All three target tiles were checked: passable, and reachable from at least one
neighbouring tile.
"""
import io
import json
import os

ROOT = r"J:\ROM Hack Project\pokeemerald-expansion"

ITEMS = [
    # (x, y, elevation, item constant, flag suffix, script suffix, comment)
    (10, 20, 3, "ITEM_TM_DIG",     "TM_DIG",     "ItemTMDig",     "0x468"),
    (28,  9, 4, "ITEM_SACRED_ASH", "SACRED_ASH", "ItemSacredAsh", "0x470"),
    (18,  5, 4, "ITEM_LIGHT_BALL", "LIGHT_BALL", "ItemLightBall", "0x472"),
]


def read(p):
    return io.open(p, encoding="utf-8", newline="").read()


def write(p, s):
    io.open(p, "w", encoding="utf-8", newline="").write(s)


# ------------------------------------------------------------------ flags
p = os.path.join(ROOT, "include", "constants", "flags.h")
s = read(p)
anchor = "#define FLAG_ITEM_DISTORTION_WORLD_5_RIFT_HEART FLAG_UNUSED_0x284"
assert s.count(anchor) == 1
block = anchor + "\n\n" + "".join(
    "#define FLAG_ITEM_MUNEN_VILLAGE_%-15s FLAG_UNUSED_%s // item ball at (%d,%d)\n"
    % (flag, slot, x, y) for x, y, _, _, flag, _, slot in ITEMS)
if "FLAG_ITEM_MUNEN_VILLAGE_TM_DIG" not in s:
    write(p, s.replace(anchor + "\n", block, 1))
    print("  flags.h: 3 item flags added")
else:
    print("  flags.h: already present")

# ---------------------------------------------------------------- scripts
p = os.path.join(ROOT, "data", "maps", "MunenVillage", "scripts.inc")
s = read(p)
if "MunenVillage_EventScript_ItemTMDig" not in s:
    s = s.rstrip("\n") + "\n\n" + (
        "@ --------------------------------------------------------------------------\n"
        "@  Item balls\n"
        "@  Standard finditem pickups: the object event blocks the tile, the flag on\n"
        "@  it hides the ball once taken, and the ground underneath stays passable.\n"
        "@ --------------------------------------------------------------------------\n")
    for x, y, _, item, _, script, _ in ITEMS:
        s += ("MunenVillage_EventScript_%s::\n"
              "\tfinditem %s\n"
              "\tend\n\n" % (script, item))
    write(p, s)
    print("  scripts.inc: 3 finditem scripts appended")
else:
    print("  scripts.inc: already present")

# --------------------------------------------------------------- map.json
p = os.path.join(ROOT, "data", "maps", "MunenVillage", "map.json")
d = json.loads(read(p))
have = {e.get("script") for e in d["object_events"]}
added = 0
for x, y, elev, item, flag, script, _ in ITEMS:
    name = "MunenVillage_EventScript_%s" % script
    if name in have:
        continue
    d["object_events"].append({
        "graphics_id": "OBJ_EVENT_GFX_ITEM_BALL",
        "x": x, "y": y, "elevation": elev,
        "movement_type": "MOVEMENT_TYPE_NONE",
        "movement_range_x": 0, "movement_range_y": 0,
        "trainer_type": "TRAINER_TYPE_NONE",
        "trainer_sight_or_berry_tree_id": "0",
        "script": name,
        "flag": "FLAG_ITEM_MUNEN_VILLAGE_%s" % flag,
    })
    added += 1
if added:
    write(p, json.dumps(d, indent=2) + "\n")
print("  map.json: %d item-ball object events added (now %d objects)"
      % (added, len(d["object_events"])))
