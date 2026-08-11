# Handoff — Boss Intro Card System (`boss_intro`)

**Project:** Wishes of Tomorrow (pokeemerald-expansion)
**First implementation target:** General Edwards, Sennen Station, Act 2 climax
**Author:** Joe (Flatfoot Games) — spec written for Claude Code implementation
**Status:** New system. Nothing exists yet.

---

## 0. Read this first

This spec was written from outside the repo. **Every symbol name, file path, and function
signature below is a strong hint, not gospel.** Before writing code, grep the actual tree and
confirm. Where I flag `[VERIFY]`, the name has changed across pret revisions and you must check
which variant this project uses.

Build with placeholders first, art last (see §7 Build Order). Do not block on assets.

---

## 1. Goal

Implement a reusable **boss intro card** — the "Boss Subtitles" trope (Zelda boss nameplates,
Dave the Diver creature cards, Kill la Kill name slams). One data-driven system, called from any
map script, so it can be reused for the Mutrid Captain, the Layer 2 gym, and the tower-top
Leader rematch later.

Desired beat at Sennen Station:

1. Edwards delivers **one line** of dialogue through the existing namebox + portrait system.
2. Message closes. Field **freezes**.
3. A **filter** washes over the frozen overworld (darken + color tint). The character portrait
   stays unfiltered and pops forward — it's the only bright thing on screen.
4. A **banner slides in** from the left with his name (and subtitle line).
5. **Sound effect** hits on the slam.
6. Card holds ~1.5s, exits, filter lifts, battle starts.

Total runtime target: **~2.2–2.5 seconds**. It must feel like a punch, not a cutscene. If it
drags, cut hold frames, not the slam.

---

## 2. Reference material already in this repo

Mirror these patterns rather than inventing new ones:

| Existing thing | What to steal from it |
|---|---|
| `src/map_name_popup.c` | The canonical "banner slides in, holds, slides out" task. Read this first — it is the closest existing analogue in the whole codebase. Note it slides via BG offset; we are **not** doing that (see §4.2). |
| **Custom NPC portrait system** (64×64 OBJ sprites, built for this project) | Reuse wholesale for the card portrait. Find the loader and the portrait ID enum; the card should take a portrait ID, not its own art. |
| **Gen 5-style NPC namebox** (this project) | The one-liner in step 1 uses this as-is. No changes needed. |
| **Quest toast notification** (this project) | Reference for non-blocking task lifecycle, sprite alloc/free hygiene, and tag management. |
| `src/title_screen.c` | Reference for `LoadCompressedSpriteSheet` / palette loading of custom art. |

---

## 3. Timeline (60 fps, frame-accurate storyboard)

Implement as a state machine on a single task. Frame numbers are from task start.

| Frame | Event |
|---|---|
| 0 | `FreezeObjectEvents()`; optional `FadeOutBGM(4)` if `cutMusic` set in data |
| 0 | **SE #1** — low impact/cue |
| 0–10 | Field filter fades in: `BeginNormalPaletteFade(fieldMask, 0, 0, 10, tintColor)` |
| 6–16 | Portrait slides in from right edge, ease-out, settles right-of-center |
| 16–28 | Banner slides in from left edge, ease-out, settles across lower third |
| 26 | **SE #2** — the slam. Name graphic appears same frame with a 2-frame white flash |
| 28–118 | **Hold** (90 frames, per-boss configurable) |
| 118–130 | Exit: banner slides back left, portrait slides right, name goes with the banner |
| 126–136 | Filter lifts: `BeginNormalPaletteFade(fieldMask, 0, 10, 0, tintColor)` |
| 138 | Free all sprites/tags, `UnfreezeObjectEvents()`, `ScriptContext_Enable()` |

**Skip:** A or B after frame 30 jumps straight to frame 118. Players will replay this map;
make it skippable.

**Easing:** no affine math needed. Use a fixed-point position with a per-frame velocity that
decays (`vel = vel * 3 / 4`, clamp minimum 1) — cheap, reads as ease-out, no lookup table.

---

## 4. Architecture

### 4.1 New files

```
src/boss_intro.c
include/boss_intro.h
include/constants/boss_intro.h        # BOSS_INTRO_* enum
graphics/boss_intro/banner.png        # 4bpp indexed
graphics/boss_intro/banner.pal
graphics/boss_intro/name_edwards.png  # 4bpp indexed, shares banner.pal
```

Register the gfx in `graphics_file_rules.mk` and add `boss_intro.o` wherever the build collects
objects. **Do not** route this art through Porytiles — that's for map tilesets only. Standard
`gbagfx` rules, `.4bpp.lz`.

### 4.2 Banner rendering — use OBJ sub-sprites, not BG

`map_name_popup.c` slides its banner by writing `REG_OFFSET_BG0VOFS`. **Don't copy that here.**
BG0 carries text windows, and shifting the whole layer will fight the message system and any
weather/field effects on the other layers.

Instead: build the banner from **OBJ sub-sprites** via a subsprite table
(`SetSubspriteTables`, see existing uses in `src/` for the pattern). A 256×32 banner = 4× 64×32
sprite pieces bound to one sprite with a subsprite table. This makes the card fully independent
of BG layer usage, which matters because this system needs to work on any map.

### 4.3 The filter

`BeginNormalPaletteFade(u32 selectedPalettes, s8 delay, u8 startY, u8 targetY, u16 color)`
handles the whole ramp for you. Two things to get right:

**Exclude the card's own palettes from the mask**, or the portrait and banner will get filtered
along with the field:

```c
u32 mask = PALETTES_ALL;
mask &= ~(1 << (16 + IndexOfSpritePaletteTag(TAG_BOSS_INTRO_BANNER)));
mask &= ~(1 << (16 + IndexOfSpritePaletteTag(TAG_BOSS_INTRO_PORTRAIT)));
```

OBJ palette *n* occupies bit `16 + n` in the `selectedPalettes` bitmask. Allocate the card's
palettes **before** computing the mask.

**Watch for fade conflicts.** The field runs its own palette fades (weather, transitions,
`gPaletteFade`). Assert/guard that no fade is active when the card starts, and don't start the
card from inside a warp or a fade-out. If `gPaletteFade.active` is set on entry, wait a frame.

Suggested tint for Mutrid encounters: deep desaturated purple, roughly `RGB(6, 2, 12)`, target
`y = 10`. Tune in mGBA; on real hardware (EZ-Flash Omega DE) it will read darker, so err bright.

### 4.4 Data table

Everything per-boss lives in one table so future cards are data-only:

```c
struct BossIntroData
{
    u16 portraitId;        // ID into the existing NPC portrait system
    const u32 *nameGfx;    // compressed 4bpp name graphic
    const u8 *subtitle;    // optional second line, printed to a window; NULL to omit
    u16 seCue;             // SE #1
    u16 seSlam;            // SE #2
    u8 holdFrames;         // default 90
    u16 tintColor;
    bool8 cutMusic;        // fade BGM out at frame 0
};

static const struct BossIntroData sBossIntroData[BOSS_INTRO_COUNT] = { ... };
```

`BOSS_INTRO_EDWARDS` is index 0 and the only populated entry for now.

### 4.5 Script interface

Macro in `asm/macros/event.inc`:

```
	.macro bossintro id:req
	callnative BossIntro_StartFromScript
	.4byte \id
	.endm
```

C side:

```c
void BossIntro_StartFromScript(struct ScriptContext *ctx)
{
    u32 id = ScriptReadWord(ctx);
    BossIntro_Start(id);
    ScriptContext_Stop();   // [VERIFY] older revisions: StopScript / different name
}
```

The task calls `ScriptContext_Enable()` on completion. `[VERIFY]` — older pret revisions name
this `EnableBothScriptContexts()`. Grep for whichever exists; the script must resume exactly
once, from the task's final state, never from the start function.

---

## 5. Sennen Station script integration

Target: the Act 2 resistance assault on Sennen Station. Edwards is the officer holding the
platform when the player slips through alone with Gengar.

```
SennenStation_EventScript_EdwardsIntro::
	lockall
	applymovement LOCALID_EDWARDS, Common_Movement_FaceDown
	waitmovement 0
	msgbox SennenStation_Text_EdwardsOneLiner, MSGBOX_DEFAULT
	closemessage
	bossintro BOSS_INTRO_EDWARDS
	waitstate
	trainerbattle_no_intro TRAINER_EDWARDS, SennenStation_Text_EdwardsDefeat
	msgbox SennenStation_Text_EdwardsPostBattle, MSGBOX_DEFAULT
	closemessage
	releaseall
	end
```

Notes:
- `lockall` handles the player freeze; the task's `FreezeObjectEvents()` covers NPC animation
  during the card. Confirm they don't double-free on exit.
- `trainerbattle_no_intro` because the one-liner already served as the intro — we don't want the
  default trainer intro text firing after the card.
- **Placeholder text is fine.** Use `"You've come far enough."` for the one-liner until Joe
  writes the real line. Flag it with a `// TODO(joe): final dialogue` comment.

---

## 6. Asset specification

| Asset | Size | Format | Notes |
|---|---|---|---|
| `banner.png` | 256×32 | 4bpp indexed | 4× 64×32 subsprite pieces. Index 0 transparent. |
| `name_edwards.png` | 192×32 | 4bpp indexed | Shares `banner.pal` to save an OBJ palette slot. Pre-rendered display type — the built-in font can't do the slanted/heavy look. |
| Portrait | 64×64 | existing system | Reuse the project's portrait pipeline unmodified. |

Constraints: 15 colors + transparency per 4bpp palette. Quantize to BGR555 before export (the
existing PIL/numpy pipeline handles this). JASC-PAL alongside each sheet. Compress with LZ77 and
load via `LoadCompressedSpriteSheet`.

VRAM budget: banner 4KB + name 3KB + portrait 2KB ≈ 9KB of the 32KB OBJ region. Fine, but the
overworld already has sprites resident — **free every tag on exit**
(`FreeSpriteTilesByTag`, `FreeSpritePaletteByTag`, `DestroySprite`) and verify with mGBA's
sprite viewer that a second trigger doesn't leak.

### Sound

Pick from `include/constants/songs.h` — grep, don't guess. Candidates to audition:
`SE_MUGSHOT` (if present in this base), `SE_M_DETECT`, `SE_M_SKY_UPPERCUT`, `SE_FAILURE`,
`SE_THUNDER`. Custom SE can be added later; leave the constant in the data table so swapping is
a one-line change.

Optional and recommended for Edwards: set `cutMusic = TRUE` so the resistance assault theme
drops out at frame 0 and the card plays over near-silence, with the battle theme kicking in on
`trainerbattle`. This rhymes with the planned music-cut beat in the Act 3 tower rematch — if
you use it here, it needs to feel like a *smaller* version of that, not the same size.

---

## 7. Build order

Ship each phase working before moving on.

1. **Skeleton** — `bossintro` macro, `callnative`, task that waits 120 frames and resumes the
   script. Prove the script blocks and resumes exactly once. No graphics.
2. **Filter** — palette fade in/out with the correct mask. Field darkens, card palettes exempt.
3. **Placeholder card** — solid-color rectangle sprite as the banner, name printed to a window
   with the standard font. Full slide-in/hold/slide-out timing. Tune the easing here.
4. **Portrait** — hook the existing portrait system in.
5. **Sound** — both SEs, optional music cut.
6. **Real art** — swap banner and name graphics.
7. **Skip handling + polish** — A/B skip, white flash on slam.

---

## 8. Acceptance criteria

- [ ] Card triggers once on first approach, gated by a flag; does not re-fire on re-entry.
- [ ] Script resumes exactly once; no soft-lock if the player mashes A during the card.
- [ ] Field is fully frozen — no NPC animation, no player input leak.
- [ ] Portrait and banner are unfiltered; everything else is tinted.
- [ ] Filter fully lifts on exit; no residual tint after the battle.
- [ ] All sprite tags freed — trigger the card 5× in a row via a debug script with no VRAM or
      palette leak.
- [ ] Skippable with A/B after frame 30.
- [ ] Adding a second boss requires only a new `BOSS_INTRO_*` constant, a table row, and art.
- [ ] Works on hardware (EZ-Flash Omega DE), not just mGBA — check tint brightness and timing.

---

## 9. Known pitfalls

- **Fade collision.** The field's own palette fades will stomp the filter. Guard on
  `gPaletteFade.active` at start; never trigger the card adjacent to a warp or map transition.
- **BG0 is spoken for.** Don't slide anything via BG offsets. See §4.2.
- **Battle context.** This system is overworld-only. Do not call it from a battle script — the
  Act 3 in-battle variant is a separate job and a different hook.
- **Weather.** Sennen Station's weather state (if any) also touches palettes. Test with the
  map's actual weather set, not on a clean test map.
- **Freeze double-handling.** `lockall` and `FreezeObjectEvents()` overlap. Make sure the exit
  path doesn't unfreeze twice or unfreeze before `releaseall`.

---

## 10. Open questions for Joe

1. **Edwards' one-liner** — placeholder in for now. Needs the real line.
2. **Subtitle text** — the trope usually carries a title under the name. Something like
   `MUTRID VANGUARD` or a rank. Currently a nullable field; leave NULL if you'd rather the card
   be name-only.
3. **Is Edwards a returning name or new?** If he appears earlier in Act 2, the card should fire
   at his *first* appearance, not here — check against the Act 2 canon doc before wiring the
   trigger flag.
4. **Banner color language** — should Mutrid bosses have a fixed palette (red/black) distinct
   from neutral bosses? Worth deciding now since it's per-boss data either way.
