# NDS Music — Song-Registration Recipe for THIS tree

**Companion to** `Music_Import_NDS_Handoff.md` (§3) and `Music_Wiring_Spec.md`.
**Purpose:** the import handoff's biggest risk is build-system reconciliation — the
CyanSixFour resource was written against the *old* pokeemerald music pipeline, but this
repo (Wishes of Tomorrow / pokeemerald-expansion) uses the *modern* one. This doc is the
exact, verified way THIS tree registers a song, so dropping the NDS music in is mechanical.

All facts below were verified against the current tree.

---

## 1. The pipeline in this tree (modern — verified)

The build reads `sound/songs/midi/midi.cfg` and auto-generates the mid2agb rule for every
`.mid` (see `audio_rules.mk`). **There is NO `songs.mk` and NO `ld_script.txt`** in this
tree — so ignore every resource instruction that edits those.

Registering ONE song touches **4 source files** (5 if it needs a new instrument bank):

| # | File | What you add |
|---|------|--------------|
| 1 | `sound/songs/midi/<name>.mid` | the MIDI file itself |
| 2 | `sound/songs/midi/midi.cfg` | one line: the mid2agb flags (incl. its `-G` voicegroup) |
| 3 | `include/constants/songs.h` | `#define MUS_<NAME> <index>` |
| 4 | `sound/song_table.inc` | one `song` entry in `gSongTable`, **in index order** |
| 5 | `sound/voicegroups/` + `sound/voice_groups.inc` (+ `sound/direct_sound_samples/`) | only if the song needs a new instrument bank |

> **Critical rule:** a `MUS_` constant's number **is its position in `gSongTable`**.
> The 1st `song` entry = value 0, the 2nd = 1, … Verified: `mus_national_park` is the
> 611th entry (`song_table.inc` line 619) and `MUS_NATIONAL_PARK = 610`. So constants and
> table rows must stay in lockstep, and new songs **append to the end**.

---

## 2. Current numbers (where the imported songs go)

- `gSongTable` has **611 entries (indices 0–610)**. Highest real song = `MUS_NATIONAL_PARK = 610`.
- **Next free song index = `611`.** Number every imported track **611, 612, 613, …** and
  append its `song` row to `gSongTable` in that same order, right after `mus_national_park`.
- Highest existing numbered voicegroup = **`voicegroup183`**. Number any new imported
  voicegroups **184+** (or give them unique names) to avoid collisions.
- `MUS_ROUTE118 = 0x7FFF` and `MUS_NONE = 0xFFFF` are sentinels — not real indices, leave them.

---

## 3. The recipe — register one imported track

Say the track is DPPt "Mt. Coronet" and you'll call it `MUS_DP_MT_CORONET`:

1. **Drop the MIDI:** `sound/songs/midi/mus_dp_mt_coronet.mid`
2. **midi.cfg line** (match the existing format exactly — every `.mid` MUST have one or the
   build errors `"...does not have an associated entry in midi.cfg! It cannot be built"`):
   ```
   mus_dp_mt_coronet.mid:        -E -R50 -G_<its_voicegroup> -V080
   ```
   `-G_<voicegroup>` = the instrument bank, `-V0XX` = volume (vanilla uses 080–090),
   `-R50` = reverb, `-E` = standard. Copy the resource's per-song flags if it provides them.
3. **Voicegroup (only if new):** drop `sound/voicegroups/<name>.inc` and any new samples into
   `sound/direct_sound_samples/`, then register the bank with a line in
   `sound/voice_groups.inc`:
   ```
   .include "sound/voicegroups/<name>.inc"
   ```
   (Number 184+ or unique-name it; samples are additive — only watch for filename clashes.)
4. **Constant** in `include/constants/songs.h` — value = next free index:
   ```
   #define MUS_DP_MT_CORONET           611
   ```
5. **Song-table row** appended to `gSongTable` in `sound/song_table.inc`, AFTER
   `song mus_national_park, ...`, in the same order as the constants:
   ```
   song mus_dp_mt_coronet, MUSIC_PLAYER_BGM, 0
   ```
   (The label is the `.mid` name without extension. `MUSIC_PLAYER_BGM` for music.)

Repeat for each track, incrementing the index (611, 612, …) and appending the table row.

---

## 4. Conflict hot-spots when merging the resource (the §3 reconciliation)

In priority order — this is where the manual work is:

1. **MUS_ number collisions (biggest one).** The resource assigns its own `MUS_` numbers
   against the *old, smaller* song table. They WILL collide with this tree (vanilla + FRLG
   `MUS_RG_*` + your customs already run up to 610). **Fix:** renumber every imported track
   to **611+** and append to `gSongTable`. Never reuse a value ≤ 610.
2. **Song-table is positional.** Don't insert in the middle — that shifts every later
   constant. Append only, keeping `#define` value == table row position.
3. **Registration mechanism mismatch.** Resource edits `songs.mk` / `ld_script.txt` /
   hand-built song table the old way. This tree: translate all of that into **midi.cfg
   lines + `gSongTable` rows + `songs.h` defines**. The old files don't exist here.
4. **Voicegroup collisions.** If the resource ships numbered voicegroups that overlap
   `voicegroup001`–`voicegroup183`, renumber to 184+. The hack's one custom bank is
   *named* (`national_park`), so name-clashes are unlikely; number-clashes are the risk.
5. **Missing samples/voicegroups.** A voicegroup `.inc` won't assemble if a referenced
   `DirectSoundWaveData_*` sample isn't present. Bring all the resource's
   `direct_sound_samples/` over.

Build order to fix errors one class at a time (on your PC — Cowork's sandbox has no GBA
toolchain): missing sample → missing voicegroup → undefined `MUS_` constant → song-table
row/`#define` index mismatch.

---

## 5. The canonical worked example already in your tree

`MUS_NATIONAL_PARK` was added exactly this way — use it as the reference template:

- `include/constants/songs.h`: `#define MUS_NATIONAL_PARK  610`
- `sound/songs/midi/midi.cfg` line 531: `mus_national_park.mid:  -E -R50 -G_national_park -V080`
- `sound/song_table.inc` line 619: `song mus_national_park, MUSIC_PLAYER_BGM, 0`
- `sound/voicegroups/national_park.inc` registered in `sound/voice_groups.inc`

---

## 6. After the constants exist

Only once the `MUS_*` constants are in `songs.h` does `Music_Wiring_Spec.md` become
actionable: set each map's **Song** (`data/maps/<Map>/map.json` `"music"`) and script the
cues (`fadeoutbgm`/`fadenewbgm`/`savebgm`/`fadedefaultbgm`). Several of those beats also
need Act 2/3 maps that aren't built yet — wire the ones whose maps exist (Ashlands,
National Park gravesite) first; the rest as the maps land.
