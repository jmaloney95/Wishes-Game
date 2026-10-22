#!/usr/bin/env python3
"""Rebuild a species' overworld_shiny.pal from its battle palettes.

Some species ship an all-gray overworld shiny palette, which reads as "not
shiny" (and vanishes in the Mega Dimension's gray world). For each colour of
overworld_normal.pal this takes the nearest colour of the battle normal.pal
and writes the same slot of the battle shiny.pal, so the overworld shiny
matches the battle shiny.

    python tools/build_ow_shiny_pal.py trapinch

(Milcery ships gray too, but its battle shiny is gray-white as well, so it
is correct and left alone.)
"""
import sys


def read_pal(path):
    lines = open(path).read().split('\n')
    count = int(lines[2])
    return [tuple(int(v) for v in line.split()) for line in lines[3:3 + count]]


def write_pal(path, cols):
    with open(path, 'w', newline='\r\n') as f:
        f.write('JASC-PAL\n0100\n%d\n' % len(cols))
        for c in cols:
            f.write('%d %d %d\n' % c)


def dist(a, b):
    return sum((x - y) ** 2 for x, y in zip(a, b))


for species in sys.argv[1:]:
    base = 'graphics/pokemon/%s/' % species
    ow = read_pal(base + 'overworld_normal.pal')
    normal = read_pal(base + 'normal.pal')
    shiny = read_pal(base + 'shiny.pal')
    out = [ow[0]]   # index 0 is transparency
    for c in ow[1:]:
        if c == (0, 0, 0) or c in ((16, 16, 16), (248, 248, 248), (40, 40, 40)):
            out.append(c)   # unused slots, outline, highlight and dark accents stay
            continue
        i = min(range(1, len(normal)), key=lambda j: dist(normal[j], c))
        out.append(shiny[i])
    write_pal(base + 'overworld_shiny.pal', out)
    print(species, out)
