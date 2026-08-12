#!/usr/bin/env python3
# update_music.py - Wishes of Tomorrow
# Swap 3 weak BW battle tracks; add 10 popular DPPt/HGSS tracks.
# Run:  python3 update_music.py ./nds-src ./pokeemerald-expansion  &&  make -j$(nproc)
# Idempotent. Soundfonts already imported, so only .mid + registration are needed.
# Indices derive from song_table.inc (truth: MUS_ value == row position). Aborts if
# song_table.inc and songs.h disagree, so a half-synced tree can't be corrupted.
import os, re, sys, shutil
REMOVE = ["mus_bw_plasma", "mus_bw_vs_plasma", "mus_bw_vs_ghetsis"]
ADD = [
    ("mus_dp_galactic_hq", 191, 86), ("mus_dp_vs_galactic", 191, 90),
    ("mus_dp_vs_galactic_boss", 191, 90), ("mus_dp_vs_champion", 191, 90),
    ("mus_hg_vs_rival", 229, 84), ("mus_dp_vs_gym_leader", 191, 88),
    ("mus_hg_vs_lugia", 229, 102), ("mus_dp_vs_dialga_palkia", 191, 90),
    ("mus_hg_vs_suicune", 229, 98), ("mus_dp_eterna_forest", 191, 88),
    ("mus_dp_snowpoint_day", 191, 100), ("mus_dp_hall_of_origin", 191, 127),
    ("mus_dp_sunyshore_day", 191, 90),
]
SONG = re.compile(r"^\s*song\s+(\w+)")
DEF = re.compile(r"^\s*#define\s+(MUS_\w+)\s+(\d+)\b")
def k(b): return "MUS_" + b[4:].upper()
def die(m): print("ERROR:", m); sys.exit(1)
def rd(p): return open(p, encoding="utf-8", errors="ignore").read()
def wr(p, s): open(p, "w", encoding="utf-8").write(s)
def main():
    if len(sys.argv) != 3: die("usage: python3 update_music.py <source-clone> <expansion-tree>")
    SRC, DST = os.path.abspath(sys.argv[1]), os.path.abspath(sys.argv[2])
    md_s = os.path.join(SRC, "sound", "songs", "midi"); md = os.path.join(DST, "sound", "songs", "midi")
    H = os.path.join(DST, "include", "constants", "songs.h"); ST = os.path.join(DST, "sound", "song_table.inc")
    CFG = os.path.join(md, "midi.cfg"); DBG = os.path.join(DST, "src", "debug.c")
    for p in [md, H, ST, CFG, DBG]:
        if not os.path.exists(p): die("missing path: " + p)
    log = []
    rows = [SONG.match(l).group(1) for l in rd(ST).splitlines() if SONG.match(l)]
    pos = {n: i for i, n in enumerate(rows)}
    defs = {m.group(1): int(m.group(2)) for l in rd(H).splitlines() for m in [DEF.match(l)] if m}
    for c in ("MUS_NATIONAL_PARK", "MUS_DP_MT_CORONET", "MUS_BW_VS_GHETSIS"):
        b = "mus_" + c[4:].lower()
        if c in defs and b in pos and defs[c] != pos[b]:
            die("out of sync: %s=%d but row %d. Re-sync songs.h/song_table.inc. Nothing changed." % (c, defs[c], pos[b]))
    if "MUS_DP_MT_CORONET" in defs and "mus_dp_mt_coronet" not in pos:
        die("songs.h has NDS constants but song_table.inc lacks their rows (out of date). Nothing changed.")
    rem = 0
    for b in REMOVE:
        c = k(b)
        present = any(re.search(r"\bsong\s+" + b + r"\b", l) for l in rd(ST).splitlines())
        h = [l for l in rd(H).splitlines(keepends=True) if not re.match(r"\s*#define\s+" + c + r"\b", l)]
        st = [l for l in rd(ST).splitlines(keepends=True) if not re.search(r"\bsong\s+" + b + r"\b", l)]
        cf = [l for l in rd(CFG).splitlines(keepends=True) if not l.lstrip().startswith(b + ".mid:")]
        wr(H, "".join(h)); wr(ST, "".join(st)); wr(CFG, "".join(cf))
        mp = os.path.join(md, b + ".mid")
        if os.path.exists(mp): os.remove(mp)
        if present: rem += 1
    log.append("removed %d BW rows" % rem)
    st_lines = rd(ST).splitlines(keepends=True)
    si = [i for i, l in enumerate(st_lines) if SONG.match(l)]
    idx = len(si); base_count = len(si)
    have = {m.group(1) for l in rd(H).splitlines() for m in [DEF.match(l)] if m}
    nd, nr, nc, added, miss = [], [], [], [], []
    for b, vg, vol in ADD:
        c = k(b)
        if c in have: continue
        s = os.path.join(md_s, b + ".mid")
        if os.path.exists(s): shutil.copy2(s, os.path.join(md, b + ".mid"))
        else: miss.append(b)
        nd.append("#define %s%d\n" % (c.ljust(28), idx))
        nr.append("\tsong %s, MUSIC_PLAYER_BGM, 0\n" % b)
        nc.append("%s-E -R0 -G%d -V%03d\n" % ((b + ".mid:").ljust(30), vg, vol))
        added.append(c); idx += 1
    if nr:
        ins = si[-1] + 1
        st_lines[ins:ins] = ["\n\t@ added music set\n"] + nr
        wr(ST, "".join(st_lines))
        hl = rd(H).splitlines(keepends=True)
        hi = max(range(len(hl)), key=lambda i: int(DEF.match(hl[i]).group(2)) if DEF.match(hl[i]) else -1)
        hl[hi + 1:hi + 1] = ["\n// added music set\n"] + nd
        wr(H, "".join(hl))
        cur = rd(CFG)
        with open(CFG, "a", encoding="utf-8") as f:
            if cur and not cur.endswith("\n"): f.write("\n")
            f.writelines(nc)
    log.append("added %d tracks at %d..%d" % (len(added), base_count, idx - 1) + ("; MISSING: " + ",".join(miss) if miss else ""))
    txt = rd(DBG)
    m = re.search(r"#define SOUND_LIST_BGM[ \t]*\\\n(.*?)\n[ \t]*\n#define SOUND_LIST_SE", txt, re.S)
    if m:
        names = re.findall(r"X\((MUS_\w+)\)", m.group(1))
        rmc = {k(b) for b in REMOVE}
        new = [n for n in names if n not in rmc] + [k(b) for (b, vg, vol) in ADD if k(b) not in names]
        out = ["#define SOUND_LIST_BGM              \\"]
        for i, n in enumerate(new):
            out.append("    X(%s)%s" % (n, "" if i == len(new) - 1 else "              \\"))
        txt = txt[:m.start()] + "\n".join(out) + "\n\n#define SOUND_LIST_SE" + txt[m.end():]
        wr(DBG, txt); log.append("debug.c list rebuilt (%d entries)" % len(new))
    else:
        log.append("WARN: SOUND_LIST_BGM not found; edit debug.c by hand")
    if added:
        last = k(ADD[-1][0]); t = rd(H)
        if re.search(r"#define\s+END_MUS\b", t):
            wr(H, re.sub(r"(#define\s+END_MUS\s+)\S+", r"\g<1>" + last, t, count=1)); log.append("END_MUS = " + last)
    print("\n".join("  " + x for x in log)); print("DONE. Run: make -j$(nproc)")
if __name__ == "__main__":
    main()
