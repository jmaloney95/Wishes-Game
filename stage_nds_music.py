#!/usr/bin/env python3
"""
stage_nds_music.py  —  Wishes of Tomorrow / pokeemerald-expansion
==================================================================
Turn-key, idempotent staging of the 13 mapped NDS Music Expansion tracks
(DPPt + HGSS + BW) from CyanSMP64/pokeemerald into the WoT expansion tree.

RUN IT ON YOUR BUILD PC (not inside Cowork): it copies ~500 asset files and the
tree must be buildable right after. It only copies files + edits text; it never
builds or downloads. Safe to re-run — every step skips what's already done.

USAGE
  Get the source first (FULL clone — sparse/partial omits the samples):
     git clone --branch music_expansion_dev https://github.com/CyanSMP64/pokeemerald nds-src
  Then:
     python3 stage_nds_music.py  ./nds-src  ./pokeemerald-expansion
  Then read NDS_Music_Stage_Checklist.md and `make -j$(nproc)`.

WHAT IT STAGES (the full dependency closure)
  1. 13 .mid files
  2. voicegroups 191-274 (the 3 soundfont chains) + every voicegroupInst*/Drum*
     sub-bank those chains reference (BW uses 29 of them)
  3. sound/keysplit_tables.inc  ->  keysplit_tables_nds.inc, registered in
     data/sound_data.s (the NDS voicegroups use numbered KeySplitTableN; the
     expansion's own keysplit file uses *named* tables, so there's no collision)
  4. every DirectSoundWaveData sample those voicegroups reference (AIFF .aif) +
     its INCBIN declaration in sound/direct_sound_data.inc
  5. the aif2pcm tool + a .aif build rule (this expansion is .wav-only and had
     aif2pcm removed; the NDS samples are .aif)
  6. midi.cfg lines, songs.h constants (611-623), song_table rows
  7. src/m4a_1.s  ->  src/m4a_1.s.nds-candidate  (do NOT auto-apply; see checklist)
"""

import os, re, sys, glob, shutil

# (mid basename, MUS_ constant, voicegroup, volume)  — indices assigned 611.. in order
TRACKS = [
    ("mus_dp_mt_coronet",        "MUS_DP_MT_CORONET",        191, 112),
    ("mus_dp_old_chateau",       "MUS_DP_OLD_CHATEAU",       191, 127),
    ("mus_pl_distortion_world",  "MUS_PL_DISTORTION_WORLD",  191, 105),
    ("mus_pl_vs_giratina",       "MUS_PL_VS_GIRATINA",       191, 105),
    ("mus_hg_ecruteak",          "MUS_HG_ECRUTEAK",          229,  54),
    ("mus_hg_bell_tower",        "MUS_HG_BELL_TOWER",        229,  80),
    ("mus_bw_anville",           "MUS_BW_ANVILLE",           274,  82),
    ("mus_bw_castelia",          "MUS_BW_CASTELIA",          274,  64),
    ("mus_bw_black_city",        "MUS_BW_BLACK_CITY",        274,  76),
    ("mus_bw_n_castle",          "MUS_BW_N_CASTLE",          274, 110),
    ("mus_bw_plasma",            "MUS_BW_PLASMA",            274,  73),
    ("mus_bw_vs_plasma",         "MUS_BW_VS_PLASMA",         274, 120),
    ("mus_bw_vs_ghetsis",        "MUS_BW_VS_GHETSIS",        274, 126),
]
FIRST_INDEX = 611
VG_LO, VG_HI = 191, 274
ANCHOR_DEFINE, ANCHOR_SONG = "MUS_NATIONAL_PARK", "mus_national_park"

def die(m): print("ERROR:", m); sys.exit(1)
def rd(p):  return open(p, encoding="utf-8", errors="ignore").read()

def main():
    if len(sys.argv) != 3: die("usage: python3 stage_nds_music.py <source-clone> <expansion-tree>")
    SRC, DST = os.path.abspath(sys.argv[1]), os.path.abspath(sys.argv[2])
    for p, w in [(SRC,"source clone"),(DST,"expansion tree")]:
        if not os.path.isdir(p): die(f"{w} not found: {p}")
    smp_dir_s = os.path.join(SRC,"sound","direct_sound_samples")
    if not os.path.isdir(smp_dir_s) or not os.listdir(smp_dir_s):
        die("source has no sound/direct_sound_samples — clone FULLY (not sparse).")

    midi_s, midi_d = os.path.join(SRC,"sound","songs","midi"), os.path.join(DST,"sound","songs","midi")
    vg_s,   vg_d   = os.path.join(SRC,"sound","voicegroups"),  os.path.join(DST,"sound","voicegroups")
    smp_d   = os.path.join(DST,"sound","direct_sound_samples")
    sample_inc = os.path.join(DST,"sound","direct_sound_data.inc")
    vgroups_inc= os.path.join(DST,"sound","voice_groups.inc")
    midicfg    = os.path.join(midi_d,"midi.cfg")
    songs_h    = os.path.join(DST,"include","constants","songs.h")
    songtable  = os.path.join(DST,"sound","song_table.inc")
    sound_data = os.path.join(DST,"data","sound_data.s")
    for p in [midi_d,vg_d,smp_d,sample_inc,vgroups_inc,midicfg,songs_h,songtable,sound_data]:
        if not os.path.exists(p): die(f"expected path missing in expansion tree: {p}")
    log = []

    # 1) MIDIs ----------------------------------------------------------------
    n=0
    for base,*_ in TRACKS:
        s,d = os.path.join(midi_s,base+".mid"), os.path.join(midi_d,base+".mid")
        if not os.path.exists(s): die(f"missing source midi: {s}")
        if not os.path.exists(d): shutil.copy2(s,d); n+=1
    log.append(f"midis: {n} copied / 13")

    # 2) voicegroups 191-274 --------------------------------------------------
    n=0
    for v in range(VG_LO,VG_HI+1):
        s,d = os.path.join(vg_s,f"voicegroup{v}.inc"), os.path.join(vg_d,f"voicegroup{v}.inc")
        if not os.path.exists(s): die(f"missing source voicegroup: {s}")
        if not os.path.exists(d): shutil.copy2(s,d); n+=1
    log.append(f"voicegroups {VG_LO}-{VG_HI}: {n} new")

    # 2b) Inst/Drum sub-bank closure (referenced by the numbered voicegroups) -
    subrefs = set()
    for v in range(VG_LO,VG_HI+1):
        subrefs |= set(re.findall(r"voicegroup(?:Inst|Drum)\d+", rd(os.path.join(vg_d,f"voicegroup{v}.inc"))))
    n=0; miss=[]
    for r in sorted(subrefs):
        s,d = os.path.join(vg_s,r+".inc"), os.path.join(vg_d,r+".inc")
        if os.path.exists(s):
            if not os.path.exists(d): shutil.copy2(s,d); n+=1
        else: miss.append(r)
    log.append(f"Inst/Drum sub-banks: {len(subrefs)} referenced, {n} copied" + (f", MISSING {miss}" if miss else ""))

    # 3) keysplit tables (numbered) -> separate file, registered after the existing one
    kst_s = os.path.join(SRC,"sound","keysplit_tables.inc")
    kst_d = os.path.join(DST,"sound","keysplit_tables_nds.inc")
    if os.path.exists(kst_s) and not os.path.exists(kst_d):
        shutil.copy2(kst_s,kst_d); log.append("keysplit_tables_nds.inc copied")
    sd = rd(sound_data)
    if "keysplit_tables_nds.inc" not in sd and '.include "sound/keysplit_tables.inc"' in sd:
        open(sound_data,"w",encoding="utf-8").write(sd.replace(
            '.include "sound/keysplit_tables.inc"',
            '.include "sound/keysplit_tables.inc"\n\t.include "sound/keysplit_tables_nds.inc"',1))
        log.append("data/sound_data.s: keysplit_tables_nds.inc registered")

    # 4) register ALL NDS voicegroups in voice_groups.inc ---------------------
    vg_txt = rd(vgroups_inc)
    names = [f"voicegroup{v}" for v in range(VG_LO,VG_HI+1)] + sorted(subrefs)
    add = [f'.include "sound/voicegroups/{nm}.inc"\n' for nm in names if f"voicegroups/{nm}.inc\"" not in vg_txt]
    if add:
        with open(vgroups_inc,"a",encoding="utf-8") as f: f.writelines(add)
    log.append(f"voice_groups.inc: {len(add)} includes added")

    # 5) sample closure over EVERY NDS voicegroup (numbered + Inst/Drum) ------
    have = set(re.findall(r"DirectSoundWaveData_(\w+)::", rd(sample_inc)))
    needed = set()
    for nm in names:
        p = os.path.join(vg_d,nm+".inc")
        if os.path.exists(p): needed |= set(re.findall(r"DirectSoundWaveData_(\w+)", rd(p)))
    copied=0; smiss=[]; decls=[]; addbytes=0
    for s in sorted(needed-have):
        srcs = glob.glob(os.path.join(smp_dir_s,s+".*"))
        if not srcs: smiss.append(s); continue
        for f in srcs:
            shutil.copy2(f, os.path.join(smp_d,os.path.basename(f)))
            if f.endswith(".aif"): addbytes += os.path.getsize(f)
        copied+=1
        decls.append(f'\n\t.align 2\nDirectSoundWaveData_{s}::\n\t.incbin "sound/direct_sound_samples/{s}.bin"\n')
    if decls:
        with open(sample_inc,"a",encoding="utf-8") as f: f.writelines(decls)
    log.append(f"samples: {len(needed)} referenced, {len(have&needed)} already present, {copied} copied"
               + (f" (~{addbytes/1048576:.1f}MB AIFF added; ROM .bin ~half)" if addbytes else "")
               + (f", MISSING {len(smiss)}" if smiss else ""))
    if smiss: log.append("  MISSING samples: " + ", ".join(smiss[:15]) + (" ..." if len(smiss)>15 else ""))

    # 6) aif2pcm tool + .aif build rule ---------------------------------------
    tool_s,tool_d = os.path.join(SRC,"tools","aif2pcm"), os.path.join(DST,"tools","aif2pcm")
    if os.path.isdir(tool_s):
        os.makedirs(tool_d,exist_ok=True)
        for fn in os.listdir(tool_s):
            sp=os.path.join(tool_s,fn)
            if os.path.isfile(sp): shutil.copy2(sp,os.path.join(tool_d,fn))
        log.append("aif2pcm tool copied")
    else: log.append("WARN: source tools/aif2pcm missing — copy it manually")
    mt=os.path.join(DST,"make_tools.mk"); s=rd(mt)
    if "aif2pcm" not in s and re.search(r"TOOL_NAMES\s*:=.*",s):
        open(mt,"w",encoding="utf-8").write(re.sub(r"(TOOL_NAMES\s*:=.*)",r"\1 aif2pcm",s,1)); log.append("make_tools.mk: +aif2pcm")
    mk=os.path.join(DST,"Makefile"); s=rd(mk)
    if re.search(r"^AIF\s*:=",s,re.M) is None and re.search(r"^MID\s*:=.*$",s,re.M):
        open(mk,"w",encoding="utf-8").write(re.sub(r"(^MID\s*:=.*$)",r"\1\nAIF          := $(TOOLS_DIR)/aif2pcm/aif2pcm$(EXE)",s,1,flags=re.M)); log.append("Makefile: AIF defined")
    ar=os.path.join(DST,"audio_rules.mk"); s=rd(ar)
    wav='$(SOUND_BIN_DIR)/%.bin: sound/%.wav\n\t$(WAV2AGB) -b $< $@'
    if "sound/%.aif" not in s and wav in s:
        open(ar,"w",encoding="utf-8").write(s.replace(wav, wav+
            '\n\n# Compressed AIFF drum-loop samples (NDS BW/B2W2) need aif2pcm --compress (more specific -> preferred)\n'
            'sound/direct_sound_samples/bw_drum_loop_%.bin: sound/direct_sound_samples/bw_drum_loop_%.aif\n\t$(AIF) $< $@ --compress\n'
            'sound/direct_sound_samples/b2_drum_loop_%.bin: sound/direct_sound_samples/b2_drum_loop_%.aif\n\t$(AIF) $< $@ --compress\n'
            '\n# Uncompressed sounds shipped as AIFF (imported NDS Music Expansion samples)\n'
            '$(SOUND_BIN_DIR)/%.bin: sound/%.aif\n\t$(AIF) $< $@',1)); log.append("audio_rules.mk: .aif + drum-loop rules added")
    # voice_directsound_comp macro (compressed DirectSound voice; NDS Drum banks use it)
    mv=os.path.join(DST,"asm","macros","music_voice.inc"); s=rd(mv)
    if "voice_directsound_comp" not in s:
        with open(mv,"a",encoding="utf-8") as f:
            f.write('\n\t@ compressed DirectSound sample (NDS Music Expansion); type byte 0x20\n'
                    '\t.macro voice_directsound_comp base_midi_key:req, pan:req, sample_data_pointer:req, attack:req, decay:req, sustain:req, release:req\n'
                    '\t.byte 0x20\n'
                    '\t_voice_directsound \\base_midi_key, \\pan, \\sample_data_pointer, \\attack, \\decay, \\sustain, \\release\n'
                    '\t.endm\n')
        log.append("music_voice.inc: voice_directsound_comp macro added")

    # 7) midi.cfg (no comments — parsed line-by-line into make rules) ----------
    cfg=rd(midicfg); add=[]
    for base,_c,vg,vol in TRACKS:
        if f"{base}.mid:" not in cfg: add.append(f"{(base+'.mid:').ljust(30)}-E -R0 -G{vg} -V{vol:03d}\n")
    if add:
        with open(midicfg,"a",encoding="utf-8") as f:
            if cfg and not cfg.endswith("\n"): f.write("\n")
            f.writelines(add)
    log.append(f"midi.cfg: {len(add)} lines")

    # 8) songs.h constants ----------------------------------------------------
    lines=rd(songs_h).splitlines(keepends=True)
    if not any(re.search(rf"#define\s+{TRACKS[0][1]}\b",l) for l in lines):
        i=next((k for k,l in enumerate(lines) if re.search(rf"#define\s+{ANCHOR_DEFINE}\b",l)),None)
        if i is None: die("can't find MUS_NATIONAL_PARK in songs.h")
        blk=["\n// ---- NDS Music Expansion ----\n"]+[f"#define {c.ljust(28)}{FIRST_INDEX+k}\n" for k,(_b,c,*_r) in enumerate(TRACKS)]
        lines[i+1:i+1]=blk; open(songs_h,"w",encoding="utf-8").writelines(lines); log.append("songs.h: 13 constants")
    else: log.append("songs.h: present")

    # 9) song_table rows ------------------------------------------------------
    st=rd(songtable).splitlines(keepends=True)
    if not any(f"song {TRACKS[0][0]}," in l for l in st):
        i=next((k for k,l in enumerate(st) if re.search(rf"song\s+{ANCHOR_SONG}\b",l)),None)
        if i is None: die("can't find mus_national_park in song_table.inc")
        blk=["\n\t@ ---- NDS Music Expansion ----\n"]+[f"\tsong {b}, MUSIC_PLAYER_BGM, 0\n" for b,*_ in TRACKS]
        st[i+1:i+1]=blk; open(songtable,"w",encoding="utf-8").writelines(st); log.append("song_table.inc: 13 rows")
    else: log.append("song_table.inc: present")

    # 10) m4a candidate -------------------------------------------------------
    m4=os.path.join(SRC,"src","m4a_1.s")
    if os.path.exists(m4): shutil.copy2(m4,os.path.join(DST,"src","m4a_1.s.nds-candidate")); log.append("m4a_1.s -> m4a_1.s.nds-candidate (do NOT auto-apply)")

    print("\n".join("  "+l for l in log))
    print("\nDONE. Next: `make -j$(nproc)`. Leave src/m4a_1.s unchanged for the first build;")
    print("only swap in the candidate if tracks play at wrong pitch/tempo or the engine crashes.")

if __name__ == "__main__":
    main()
