#!/usr/bin/env python3
"""
Report flag constants that resolve to the same value.

WoT names are layered onto vanilla's FLAG_UNUSED_* slots by hand, and the
battle/overworld configs claim some of those slots too, so two unrelated names
can quietly end up on one bit. That is not a theoretical problem: until round
88, B_FLAG_NO_WHITEOUT and FLAG_WOT_SNAG_STAGE1 were both 0x035, which meant
that from the first snag reward onwards the player could never white out of a
trainer battle again -- a loss handed control back to the script instead, so
bosses re-challenged forever or paid out their rewards anyway.

Names are collected with a regex; VALUES COME FROM THE COMPILER. Most flags are
(BASE + 0xNN) expressions, and eyeballing or regexing those is how the mistake
gets made in the first place.

Run from the repo root (needs a host cc; under WSL that is just gcc):
    python3 tools/check_flag_collisions.py
Exit status is 1 if an unexplained collision is found.
"""

import collections
import os
import re
import subprocess
import sys
import tempfile

CONFIG_HEADERS = ("general.h", "battle.h", "overworld.h", "pokemon.h",
                  "item.h", "debug.h", "save.h")

# Aliases that are meant to share a value: the config knobs that are documented
# as "point this at the flag you want", and vanilla's own START/TEMP aliases.
EXPECTED_ALIAS_PREFIXES = ("B_FLAG_BADGE_BOOST", "FLAG_BADGE", "FLAG_TEMP",
                           "FLAG_HIDDEN_ITEMS_START", "FLAG_HIDDEN_ITEM_LAVARIDGE")
EXPECTED_ALIAS_PAIRS = (
    frozenset(("FLAG_WOT_EXP_SHARE_ON", "I_EXP_SHARE_FLAG")),
    frozenset(("FLAG_WOT_NO_WHITEOUT", "B_FLAG_NO_WHITEOUT")),
)


def collect_names():
    names, seen = [], set()
    for line in open("include/constants/flags.h", encoding="utf-8"):
        m = re.match(r"\s*#define\s+(FLAG_[A-Za-z0-9_]+)\s+\S", line)
        if m and m.group(1) not in seen:
            seen.add(m.group(1))
            names.append(m.group(1))
    for cfg in CONFIG_HEADERS:
        path = os.path.join("include/config", cfg)
        if not os.path.exists(path):
            continue
        for line in open(path, encoding="utf-8"):
            m = re.match(r"\s*#define\s+([A-Z0-9_]*FLAG[A-Za-z0-9_]*)\s+(FLAG_|0x)", line)
            if m and m.group(1) not in seen:
                seen.add(m.group(1))
                names.append(m.group(1))
    return names


def flag_values(names):
    src = ['#include "constants/flags.h"']
    src += ['#include "config/%s"' % c for c in CONFIG_HEADERS
            if os.path.exists(os.path.join("include/config", c))]
    src += ["#include <stdio.h>", "int main(void){"]
    src += ['    printf("%s=%d\\n", "{0}", (int)({0}));'.format(n) for n in names]
    src += ["    return 0;", "}"]

    with tempfile.TemporaryDirectory() as tmp:
        c_file = os.path.join(tmp, "flagprobe.c")
        exe = os.path.join(tmp, "flagprobe")
        open(c_file, "w", newline="\n").write("\n".join(src) + "\n")
        cc = os.environ.get("CC", "cc")
        build = subprocess.run([cc, "-I", "include", "-x", "c", c_file, "-o", exe],
                               capture_output=True, text=True)
        if build.returncode != 0:
            sys.exit("could not compile the probe:\n" + build.stderr[:2000])
        out = subprocess.run([exe], capture_output=True, text=True).stdout

    values = collections.defaultdict(list)
    for line in out.split():
        name, _, value = line.rpartition("=")
        values[int(value)].append(name)
    return values


def main():
    if not os.path.exists("include/constants/flags.h"):
        sys.exit("run me from the repo root")

    values = flag_values(collect_names())
    bad = 0
    for value in sorted(values):
        named = [n for n in values[value] if "UNUSED" not in n]
        if value == 0 or len(named) < 2:
            continue
        if all(n.startswith(EXPECTED_ALIAS_PREFIXES) for n in named):
            continue
        if frozenset(named) in EXPECTED_ALIAS_PAIRS:
            continue
        print("0x%03X: %s" % (value, ", ".join(named)))
        bad += 1

    # Only the general-purpose region: past FLAG_DEFEATED_RUSTBORO_GYM (0x4F0)
    # come the gym/system/daily blocks, which are not ours to hand out.
    free = [v for v in range(0x20, 0x4F0)
            if not values.get(v) or all("UNUSED" in n for n in values[v])]
    runs, start = [], None
    for v in free + [None]:
        if start is None:
            start = prev = v
        elif v is not None and v == prev + 1:
            prev = v
        else:
            runs.append((start, prev))
            start, prev = v, v
    longest = max(runs, key=lambda r: r[1] - r[0]) if runs else None

    print("collisions: %d" % bad)
    if longest:
        print("longest free run: 0x%03X-0x%03X (%d slots)"
              % (longest[0], longest[1], longest[1] - longest[0] + 1))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
