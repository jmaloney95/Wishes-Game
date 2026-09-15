#!/usr/bin/env python3
"""
Regenerate SennenVillage_EventScript_ClearTrain from the two Sennen layouts.

SennenVillage is the train-present map; SennenVillage_2 is the reference with
the tracks cleared. Once the Ember Badge is won, ON_LOAD repaints every tile in
the train band that differs between the two. The list is baked into
data/maps/SennenVillage/scripts.inc as setmetatile lines, so it goes stale
whenever either map is resized or the train is repainted -- re-run this then.

Only the train band is copied. The two maps also differ further down (the
beach and treeline below row 17); those differences predate this script and
were never applied, so they are left out on purpose.

setmetatile writes the metatile id and the collision bit; elevation is not
settable and keeps SennenVillage's value (as it always has).

Run from the repo root:  python3 tools/gen_sennen_train_clear.py
"""

import json
import re
import struct
import sys

TRAIN_ROWS = range(10, 18)   # rows 10-17 of the 40x27 layout (was 9-15 at 35x26)
SCRIPT = "data/maps/SennenVillage/scripts.inc"
LABEL = "SennenVillage_EventScript_ClearTrain::\n"


def layout(layout_id):
    for l in json.load(open("data/layouts/layouts.json"))["layouts"]:
        if l and l.get("id") == layout_id:
            data = open(l["blockdata_filepath"], "rb").read()
            vals = struct.unpack("<%dH" % (len(data) // 2), data)
            return l["width"], l["height"], vals
    sys.exit("no layout " + layout_id)


def main():
    w1, h1, train = layout("LAYOUT_SENNEN_VILLAGE")
    w2, h2, clear = layout("LAYOUT_SENNEN_VILLAGE_2")
    if (w1, h1) != (w2, h2):
        sys.exit("the two Sennen layouts must be the same size: %dx%d vs %dx%d" % (w1, h1, w2, h2))

    lines = []
    for y in TRAIN_ROWS:
        for x in range(w1):
            a, b = train[y * w1 + x], clear[y * w1 + x]
            if a != b:
                lines.append("\tsetmetatile %d, %d, %d, %d\n" % (x, y, b & 0x3FF, (b >> 10) & 3 and 1))

    s = open(SCRIPT, encoding="utf-8", newline="").read()
    start = s.index(LABEL) + len(LABEL)
    end = s.index("\tend\n", start)
    body = s[start:end]
    if any(not l.startswith("\tsetmetatile ") and l.strip() for l in body.splitlines(True)):
        sys.exit("unexpected lines inside ClearTrain; not rewriting")
    s = s[:start] + "".join(lines) + s[end:]
    open(SCRIPT, "w", encoding="utf-8", newline="").write(s)
    print("%dx%d layouts, rows %d-%d: %d tile swaps written" % (w1, h1, TRAIN_ROWS[0], TRAIN_ROWS[-1], len(lines)))


if __name__ == "__main__":
    main()
