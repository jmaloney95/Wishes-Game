# Hand-editing the NPC dialogue portraits

Working copies of `clarkson.png` and `edwards.png` for editing in Aseprite.
Edit them here, hand them back, and they get validated and installed into
`pokeemerald-expansion/graphics/portraits/`.

---

## Hard limits

These are enforced by the hardware and by `src/npc_portrait.c`. Break one and
the art either fails to convert or renders wrong.

| Limit | Value | Why |
|---|---|---|
| Canvas | exactly **64 x 64** | `PORTRAIT_SIZE_4BPP` is 0x800 = 64 tiles of 4bpp |
| Colour mode | **Indexed** | converted to 4bpp; RGB mode will not convert |
| Palette | **16 entries, including transparent** | one OBJ palette slot, 4bpp |
| Index 0 | **transparent** | anything on index 0 does not draw |
| Channel depth | **5 bits per channel** | keep every R/G/B a **multiple of 8** |

So you have **15 drawable colours**. There is no per-tile palette limit — it is
a single 64x64 sprite, so all 64 tiles share the one palette. Any pixel can be
any of the 15.

### The multiple-of-8 rule

The GBA stores 5 bits per channel, so it only has 32 levels, not 256. `gbagfx`
truncates each channel with `>> 3`. Two colours that differ by less than 8 in
every channel collapse into the *same* on-screen colour and you have silently
wasted one of your 15 slots — Edwards had exactly this before, with two
near-blacks that were identical on hardware.

Both `.gpl` files here are already snapped to multiples of 8. If you mix new
colours, round them: `248, 240, 232, 224 …` not `250, 243, 231`.

---

## What is actually visible

The portrait is a sprite at screen x 8–71, y 52–115 (left-hand side), drawn at
OAM priority 1, which puts it **behind** the nameplate and the dialogue frame.

`reference/occlusion_guide.png` is a 64x64 overlay of the dead zone:

- **red** — rows 52–63, columns 8–63: behind the nameplate (`tilemapTop = 13`)
- **orange** — rows 60–63, all columns: behind the dialogue frame (`tilemapTop = 15`)

**The bottom ~12 rows are almost entirely hidden.** Do not spend effort there.
The red zone's left edge depends on the speaker name's length, so treat columns
0–7 of those rows as the only reliably visible part.

Everything above row 52 — the whole head, shoulders and upper chest — is what
the player sees. That is where detail pays.

---

## Setting up in Aseprite

1. Open `clarkson.png`. Aseprite reads the indexed palette and keeps index 0 as
   the transparent slot. Confirm with **Sprite > Color Mode** — it should
   already say *Indexed*. Do not convert to RGB; converting back re-quantises
   and undoes your work.
2. **Window > Palette**, then the hamburger menu > **Load Palette** >
   `clarkson.gpl`. The swatch order matches the file's indices.
3. Add the source art as a tracing layer:
   **Layer > New > New Layer from File** > `reference/clarkson_source_64.png`,
   then drag it *below* the portrait layer and drop its opacity to ~40%. Toggle
   it to check shapes. **Delete this layer before saving.**
4. Same again with `reference/occlusion_guide.png` as a top layer at ~50% so
   you can see the dead zone. **Delete before saving.**
5. Keep `reference/*_source_512.png` open in a second tab for reading detail —
   how the eye actually sits, where the collar seam runs.

### Saving

**File > Save As**, keep it as `.png`, overwrite `clarkson.png` in this folder.
Flatten or delete the reference layers first — a saved PNG of a 40%-opacity
reference composited under the art is not recoverable.

If Aseprite warns about the colour mode on export, stop and check you are still
in Indexed.

---

## What to actually fix

From comparing the current files against the source at 8x, the things that read
as "AI slop" at 64x64 are, in priority order:

1. **The silhouette edge.** Any pixel that is neither clearly inside nor
   clearly outside reads as fuzz. Every edge pixel should be a deliberate dark
   outline pixel or nothing. Edwards' head outline was the worst offender.
2. **Scattered single pixels in flat areas.** A lone off-shade pixel in the
   middle of the coat or the camo is dither residue, not detail. Flood it.
3. **Facial features.** At this size an eye is 1–2 pixels. They need to be
   placed by hand, not averaged into existence. Clarkson's eyes and Edwards'
   eyes are the single highest-leverage pixels in each portrait.
4. **Hair.** Rendered strand-by-strand it becomes noise. Read it as 2–3 flat
   masses with a clean edge between them.

Ignore anything below row 52.

---

## Handing it back

Just say the files are ready. They get checked for canvas size, colour mode,
index-0 transparency, palette count, multiple-of-8 channels, and 15-bit
collisions, run through the real `tools/gbagfx` to confirm they produce a
2048-byte tile block and a 32-byte palette, and then installed.

`reference/*_previous.png` is the older dithered version if you want to compare
or start from it instead.
