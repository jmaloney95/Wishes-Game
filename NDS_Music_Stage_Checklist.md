# NDS Music — Stage & Build Checklist (13 tracks)

Companion to `stage_nds_music.py`. Run on your build machine. Everything here happens
**outside Cowork** (Cowork has no GBA toolchain and can't verify audio). Work on a branch:
`git checkout -b feature/nds-music`.

## What's being added

13 tracks from CyanSMP64/pokeemerald → song indices **611–623**. They pull **3 soundfonts =
voicegroups 191–274** (84, contiguous) + the **~343 samples** those reference. All voicegroup
numbers are ≥184 and all `MUS_` indices are ≥611, so **nothing collides** with your existing
songs (≤610) or voicegroups (≤183). Samples you already have (shared sc88pro set) are skipped.

| # | Index | Constant | Voicegroup | Beat (from the wiring spec) |
|---|---|---|---|---|
| 1 | 611 | `MUS_DP_MT_CORONET` | 191 | The Ashlands |
| 2 | 612 | `MUS_DP_OLD_CHATEAU` | 191 | Madam Tsuji's gym |
| 3 | 613 | `MUS_PL_DISTORTION_WORLD` | 191 | Veil crossing (boat) |
| 4 | 614 | `MUS_PL_VS_GIRATINA` | 191 | Shadow Jirachi / the machine |
| 5 | 615 | `MUS_HG_ECRUTEAK` | 229 | National Park gravesite |
| 6 | 616 | `MUS_HG_BELL_TOWER` | 229 | Gravesite alt / Celebi shrine stand-in |
| 7 | 617 | `MUS_BW_ANVILLE` | 274 | The safe house |
| 8 | 618 | `MUS_BW_CASTELIA` | 274 | Tradewind ghost-market |
| 9 | 619 | `MUS_BW_BLACK_CITY` | 274 | Shin-Tokyo (city) |
| 10 | 620 | `MUS_BW_N_CASTLE` | 274 | Shin-Tokyo tower / Mutrid HQ |
| 11 | 621 | `MUS_BW_PLASMA` | 274 | Occupied Munen / Frostwood / goons |
| 12 | 622 | `MUS_BW_VS_PLASMA` | 274 | Sennen station assault |
| 13 | 623 | `MUS_BW_VS_GHETSIS` | 274 | Leader rematch (tower top) |

> **Gap:** HGSS *Ilex Forest* (Celebi's purification shrine) is **not in the source**. Use
> `MUS_HG_BELL_TOWER` or `MUS_HG_ECRUTEAK` as the shrine stand-in, or do a fuller import later.

---

## Step 0 — Get the source (full clone, not sparse)

```
git clone --branch music_expansion_dev https://github.com/CyanSMP64/pokeemerald nds-src
```

A sparse/partial clone won't have `sound/direct_sound_samples/` and the script will refuse to
run. Confirm `nds-src/sound/direct_sound_samples/` is populated.

## Step 1 — Run the staging script

```
python3 stage_nds_music.py  ./nds-src  ./pokeemerald-expansion
```

Read its summary. It's idempotent (safe to re-run). If it prints **MISSING samples**, those
symbols are referenced by a voicegroup but the `.bin` wasn't in the source — note them; the
build will fail on them at step 3 until resolved (usually means a wrong source branch).

### Step 1b — the `aif2pcm` tool (handled automatically by the script)

The NDS samples ship as **AIFF (`.aif`)**, but this expansion builds samples from `.wav`
(wav2agb) and has **no `aif2pcm`** tool — so a plain copy of the `.aif` files won't build
(`wav2agb ... No such file: X.wav`). The script now also:

- copies `tools/aif2pcm/` from the source into your tree,
- adds `aif2pcm` to `TOOL_NAMES` in `make_tools.mk`,
- defines `AIF := $(TOOLS_DIR)/aif2pcm/aif2pcm$(EXE)` in `Makefile` (next to `WAV2AGB`),
- adds a `sound/%.bin: sound/%.aif` rule (using `$(AIF)`) to `audio_rules.mk`.

`make` then builds `aif2pcm` and converts the `.aif` samples to `.bin` with correct loop
points. If the script logs a `WARN` for any of these (anchor not found in a newer tree
version), apply that one change by hand. `.bin` files in `sound/direct_sound_samples/` are
gitignored build artifacts — safe to delete; they rebuild.

### Step 1c — the voicegroup closure (handled automatically by the script)

The 3 soundfonts aren't self-contained. voicegroup191/229/274 reference, transitively:
the numbered sub-voicegroups (192-273), **`KeySplitTableN`** key-split tables, and — for
BW (voicegroup274) — **29 `voicegroupInst*`/`voicegroupDrum*`** instrument sub-banks, which
pull in **~154 more samples**. If any layer is missing you get linker errors like
`undefined reference to KeySplitTable6` / `voicegroupInst1`. The script brings the whole
closure:

- copies `sound/keysplit_tables.inc` → `keysplit_tables_nds.inc` and registers it in
  `data/sound_data.s` after the existing keysplit include. No collision: the NDS file uses
  *numbered* `KeySplitTableN` (`.set`), the expansion's own uses *named* tables (`keysplit`
  macro).
- copies every referenced `voicegroupInst*/Drum*.inc` and `.include`s them in
  `voice_groups.inc`,
- traces samples across **all** NDS voicegroups (191-274 **and** the Inst/Drum banks), so
  those 154 extra samples are copied + declared.

ROM impact: ~1.3 MB of extra BW instrument samples → about **29.5 MB / 32 MB**. Fits, but
it's the reason the BW tracks are the heaviest; if you ever need space, dropping the BW
tracks (617-623) reclaims the most.

## Step 2 — First build, mixer UNCHANGED

```
make -j$(nproc)
```

Do **not** apply the m4a candidate yet. Many of these tracks play acceptably on the expansion
mixer. Fix build errors one class at a time, in this order (the doc's reconciliation order):

1. **missing sample** → `DirectSoundWaveData_X undefined`: the `.bin` didn't copy or its INCBIN
   line is missing. Confirm `sound/direct_sound_samples/X.bin` exists and there's a
   `DirectSoundWaveData_X::` block in `sound/direct_sound_data.inc`.
2. **missing voicegroup** → `voicegroupNNN undefined`: confirm `sound/voicegroups/voicegroupNNN.inc`
   exists and is `.include`d in `sound/voice_groups.inc`.
3. **undefined `MUS_` constant**: confirm it's in `include/constants/songs.h` (611–623).
4. **song-table / index mismatch**: the `song` row order in `sound/song_table.inc` must match
   the constant values. The script appends them right after `mus_national_park` in order — don't
   reorder.

## Step 3 — Boot test (mGBA)

Boot the ROM. Audition one DPPt (611), one HGSS (615), one BW (619). Listen for:

- **correct pitch/tempo** — if everything plays sharp/fast or flat/slow, the tracks expect the
  source's higher-rate mixer → go to step 4.
- **no audio-engine crash / hang** on a track start → also step 4.
- if they sound right: **skip step 4 entirely.** You're done with engine work.

## Step 4 — (only if step 3 failed) Reconcile the HQ-Mixer — the hard part

The source ships an upgraded `src/m4a_1.s` (ipatix HQ-Mixer, higher sample rate) the music was
tuned for. The script saved it as **`src/m4a_1.s.nds-candidate`** — it did NOT overwrite yours,
because expansion has its own m4a changes and a blind copy can break all sound.

```
diff src/m4a_1.s src/m4a_1.s.nds-candidate
```

Reconcile **manually**, not wholesale:
- Port the mixer/sample-rate routines the music needs, keeping any expansion-specific symbols.
- Watch the buffer symbol (`SoundMainRAM` vs `SoundMainRAM_Buffer`) and the sample-rate / frame
  config `.equ`s — these are the usual breakage points.
- Also check `include/gba/m4a_internal.h` / `sound/` config for a matching sample-rate define.
- Rebuild, reboot, re-audition. If a track still crashes, it's almost always a sample-rate or
  buffer mismatch in this file.

Delete `src/m4a_1.s.nds-candidate` once resolved (don't ship it).

## Step 5 — Wire tracks to maps (the payoff)

Now the constants exist, set them where the maps are built today:
- **Map BGM:** `data/maps/<Map>/map.json` → `"music": "MUS_XXX"` (or Porymap → Header → Song).
  - Ashlands → `MUS_DP_MT_CORONET` (currently `MUS_SEALED_CHAMBER`).
  - National Park gravesite area → `MUS_HG_ECRUTEAK`.
- **Scripted cues:** `playbgm` / `savebgm` / `fadenewbgm MUS_XXX` / `fadeoutbgm` / `fadedefaultbgm`.
- Beats whose maps aren't built yet (Shin-Tokyo, safe house, station assault, etc.) — wire as
  those maps land. See `Music_Wiring_Spec.md`.

## Step 6 — ROM size & trim

`make` reports the `.gba` size. Ceiling is **32 MB**; your EZ-Flash Omega DE handles up to 32 MB.
These 13 + 3 soundfonts are far lighter than the full import. If you ever go bigger and need
space back: delete the unused `.mid`, drop its midi.cfg line + songs.h define + replace its
song-table row with `mus_dummy` (keep the define so references compile).

## Step 7 — Credits (required) & legal

Add to credits:
- **Compositions/arrangements:** Junichi Masuda, Go Ichinose, Morikazu Aoki, Hitomi Sato,
  Shota Kageyama, Takuto Kitsuta, Minako Adachi.
- **GBA port (NDS Music Expansion):** CyanSixFour.
- Keep the existing RHH / pokeemerald-expansion credit.

> Nintendo/Game Freak copyrighted music. Fine for a non-commercial fan hack — **never monetize**,
> don't distribute it as an official-looking product.

## Final verification (before merging the branch)

- [ ] `make` clean; ROM < 32 MB.
- [ ] Boots in mGBA; no audio crash.
- [ ] One DPPt + one HGSS + one BW track plays at correct pitch/tempo.
- [ ] No existing WoT track broke.
- [ ] All 13 `MUS_*` constants resolve (611–623).
- [ ] Credits updated.
- [ ] Committed as its own revertible commit.
