# Wishes of Tomorrow — UI Features Implementation Handoff

**For:** Claude Code
**Project:** *Wishes of Tomorrow* (Flatfoot Games) — Pokémon Emerald ROM hack
**Base:** RHH **pokeemerald-expansion** (NOT vanilla pokeemerald — this matters, see Ground Rules)
**Goal:** Implement/extend four UI features: (1) a graphical/horizontal start menu, (2) an NPC speaker name label above the message box, (3) a custom character-portrait system for important NPC dialogue, and (4) a non-blocking quest toast notification.

**Status (update):** Features 1–3 **and** the quest **menu** system are already implemented in this project. The active task is **Feature 4 below — a modification to the existing quest system, not a rebuild.** Treat Features 1–3 as the original build record; treat Feature 4 as the current work.

---

## Ground rules (read first)

1. **This is pokeemerald-expansion, not vanilla.** Several referenced techniques and one referenced feature branch (`namebox`, below) were written against vanilla pret/pokeemerald. Expansion tracks pret's documentation but diverges structurally, so expect that any drop-in/cherry-pick will need conflicts resolved by hand. Port, don't blindly merge.

2. **Verify names against the actual installed code.** Function names, struct names, and file layouts below are the *likely* targets, but the exact identifiers vary by expansion version. Before editing, `grep`/read the real source to confirm. Do not trust the exact signatures in this doc over what's in the repo. Check the installed version first (e.g. look for a version file or `git describe --tags`).

3. **Work on a feature branch.** Create `feature/ui-overhaul` (or one branch per feature). Commit each feature separately with a clear message so it can be reverted independently.

4. **Gate behind config defines where practical.** Expansion has an `include/config/` directory. Add config toggles (e.g. `USE_GRAPHICAL_START_MENU`, `USE_SPEAKER_NAMEBOX`, `USE_NPC_PORTRAITS`, `USE_QUEST_TOAST`) so features can be turned off and so future `git pull RHH` updates cause less pain.

5. **Placeholders are expected.** Final art for the start-menu icons and the character portraits does not exist yet. Build everything so it is fully functional with placeholder graphics, and so swapping in real art later is a drop-in (replace a PNG + repoint a table entry) with **no code or script changes required**.

6. **Follow the project's existing asset conventions.** Expansion has changed how some graphics are declared over versions. Before adding any PNG, grep an existing interface/overworld sprite to see exactly how sheets and palettes are declared and incbin'd in this version, and match that pattern.

7. **Build/test:** `make -j$(nproc)` (fall back to `make` if the parallel build errors). Test in mGBA. After each feature, confirm a clean build and no regressions to normal dialogue/menus.

---

## Feature 1 — Graphical start menu

### What we want
Replace the vanilla vertical text list (Pokédex / Pokémon / Bag / Player / Save / Options / Exit) with a horizontal, icon-based bar: an icon per action laid out along the bottom, a cursor/selector, left/right navigation, and a small text label showing the currently-selected action's name (like the "Pokédex" label that appears when that entry is highlighted).

### Approach (important — do it this way)
**Reskin the presentation only; keep the existing action list and callbacks intact.** Do NOT rewrite what each menu entry does. The vanilla start menu already builds an action list and dispatches to callbacks (e.g. functions along the lines of `StartMenuPokedexCallback`, `StartMenuPokemonCallback`, `StartMenuBagCallback`, `StartMenuPlayerNameCallback`, `StartMenuSaveCallback`, `StartMenuOptionCallback`, `StartMenuExitCallback` — confirm names). Expansion also adds/removes entries conditionally (debug menu on R+Start, link/union-room variants, etc.). **All of that logic must be preserved.** We are only swapping *how the list is drawn and navigated*.

### Files
- `src/start_menu.c` (primary) and `include/start_menu.h`
- New graphics under `graphics/interface/start_menu/` (or wherever this version keeps interface gfx — match convention)
- A new config toggle in `include/config/` (e.g. `include/config/overworld.h` or a new `include/config/ui.h`)

### Tasks
1. Locate the start-menu action list builder and the functions that (a) draw the menu window and (b) handle input. In vanilla these are around `BuildStartMenuActions`, `ShowStartMenu` / the window-draw, and `HandleStartMenuInput`. Confirm the expansion equivalents.
2. Replace the **window/text draw** with a sprite-based draw:
   - `LoadStartMenuIcons()` — load the icon spritesheet + palette, then spawn one OAM sprite per active action, positioned along the bottom of the screen (compute x from index so it stays centered for a variable number of entries).
   - `CreateStartMenuCursor()` — a cursor/highlight sprite (or a palette-swap/scale on the selected icon).
   - `PrintSelectedActionName()` — a small text window that prints the highlighted action's display name; refreshed whenever the cursor moves.
3. Replace the **input handler**: D-pad Left/Right moves the cursor (wrap around), A invokes the existing callback for the selected action, B/Start closes the menu. Keep whatever "close menu" cleanup already exists.
4. `DestroyStartMenuIcons()` — free sprites/palette on close.
5. Map each action in the action list to an icon index. Keep this mapping in one small table so it stays in sync if entries are added/removed.
6. Wrap the whole graphical path in the config toggle so the vanilla menu can be restored by flipping the define.

### Placeholder art strategy
Ship a single placeholder icon sheet so the menu is testable immediately: one row of simple, distinct 32×32 tiles (e.g. a flat color per action with the first letter of the action name, or basic monochrome glyphs). Real icons drop in later by replacing the sheet PNG and keeping the same frame order. Document the required frame order (index → action) in a comment at the top of the sheet's declaration.

### Icon spec (for when real art is dropped in)
- **Size:** 32×32 px per icon (or 24×24 if the bar needs more entries; pick one and be consistent). GBA OBJ sizes are fixed — 32×32 is a clean single-sprite size.
- **Palette:** one shared 16-color palette for the whole icon sheet if possible (keeps it to a single OBJ palette slot). If icons need more colors, split across two palettes and document it.
- **Format:** indexed PNG, transparent index 0, matching the project's other interface sprites.
- **Frame order (proposed):** `0 Pokédex, 1 Pokémon, 2 Bag, 3 Trainer Card, 4 Save, 5 Options, 6 Exit`. Add slots for any expansion-conditional entries (e.g. debug) at the end.

### Acceptance criteria
- Pressing Start opens the icon bar; icons render along the bottom with the correct count for the current context.
- Left/Right cycles the cursor with wraparound; the selected action's name prints and updates.
- A opens the correct screen for **every** entry (Pokédex, Party, Bag, Trainer Card, Save, Options, Exit) — verified one by one.
- B/Start closes cleanly with no leftover sprites or palette corruption.
- Expansion's conditional entries (debug menu, link contexts) still appear/behave correctly.
- Flipping the config toggle off restores the vanilla menu.

---

## Feature 2 — NPC speaker name label (namebox)

### What we want
A small name plate rendered above the main message box showing who's speaking (like the "Rich Boy" plate in the reference), styled to match a Gen-5/BW blue speaker box — **not** the poor default colors that the reference feature branch ships with. Trainers should auto-populate their name; non-trainer NPCs get their name set via a script command. Plain narration / signs must show **no** plate.

### Reference
tustin2121's `feature/namebox` branch for pokeemerald is the mechanism reference (name plate above the message box; auto-uses a trainer's name when you talk to or are spotted by them). It targets **vanilla** pokeemerald, so use it as a guide and port the concept into expansion's message-box code rather than cherry-picking directly. Its author explicitly notes the default name-plate palette looks bad and must be changed — we are changing it (see palette below).

### Files
- The field message-box code — likely `src/field_message_box.c` plus the window/text-window code (`src/text_window.c`) and `src/menu.c`. Confirm where expansion draws the standard dialogue box.
- Script command layer for the "set speaker name" command: `src/scrcmd.c`, `data/script_cmd_table.inc`, and `asm/macros/event.inc`.
- New name-plate graphic + palette (small window frame for the plate).
- Config toggle in `include/config/`.

### Tasks
1. Add a second small window positioned directly above the main message box that renders the speaker name with its own frame. It should appear only when a speaker name is set for the current message and hide otherwise.
2. **Auto-name trainers:** when a trainer sight/talk battle dialogue triggers, populate the plate with the trainer's name (this is what the reference branch does — replicate it against expansion's trainer-talk flow).
3. **Manual name for NPCs:** add a script command, e.g. `setspeakername <text pointer>` (or `speakername`), that sets the name for the next message box(es), plus a `clearspeakername`. Provide a convenience macro so scripts can do something like:
   ```
   setspeakername Text_MadamTsuji_Name
   msgbox Text_MadamTsuji_Line, MSGBOX_DEFAULT
   clearspeakername
   ```
   (Match the actual macro/command conventions in this repo.)
4. **Poryscript check:** determine whether this project uses Poryscript (look for `.pory` files under `data/` and a `tools/poryscript` setup). If yes, register the new command(s) in the Poryscript command config so they're callable from `.pory`; if no, the asm-macro + `scrcmd.c` registration is enough.
5. Ensure ordinary narration, signposts, and system messages render with the plate hidden (no empty box).
6. Wrap in the config toggle.

### Palette (use these, not the default)
Starting palette approximating the BW/Gen-5 blue speaker box in the reference. Tune to taste; provide it as an editable `.pal`:

- Transparent: index 0
- Fill (dark navy): `#0A1E3C`
- Fill (mid navy): `#14345E`
- Border (bright blue): `#3C7CDC`
- Border (light cyan highlight): `#7CC0F4`
- Name text: `#DAF0FF` (near-white cyan) with shadow `#204878`

Keep the name-plate palette in its own slot so it doesn't disturb the main message-box palette. If the main dialogue box palette also needs a matching retune to look cohesive, do that too (the reference branch warns the stock combination looks off).

### Acceptance criteria
- Talking to / being spotted by a trainer shows that trainer's name on the plate.
- A scripted NPC with `setspeakername` shows the set name; `clearspeakername` removes it for subsequent boxes.
- Signs, narration, and system messages show **no** plate.
- Colors match the specified palette (or the tuned version), and the plate reads cleanly against the dialogue box.
- Works from Poryscript if the project uses it.
- Config toggle disables the feature.

---

## Feature 3 — Character portrait system (the "good path")

### What we want
A custom portrait system: during important NPC dialogue, show a detailed character portrait beside the text box (like a visual-novel speaker portrait), cleared when the conversation moves on. This is a **new, purpose-built system** — do NOT use the vanilla `showmonpic`/`showpokepic` command, which forces an ugly bordered box and reads as unpolished for character art.

**Key design goal:** we don't have portrait art yet. Build the whole system now against a single placeholder portrait, wired so that adding real art later means dropping in a PNG and repointing one table entry — **no script or code changes.**

### Design decisions (build to these)
- **Size:** 64×64 px portraits. (64×64 is the max single GBA OBJ size, so a portrait is one hardware sprite — simplest and clean.) Larger/framed portraits via a BG layer are possible later, but start with 64×64 OBJ.
- **Rendering:** sprite-based (OAM), one 64×64 sprite. Loads into a dedicated OBJ palette slot.
- **Palette:** one 16-color palette per portrait. Portraits must be designed within this limit (stylized, GBA-appropriate — the hardware can't do a full-res render).
- **Position:** configurable Left or Right, above/over the text box (default Right, matching the reference). Expose it as an argument so a scene can put the speaker on the correct side.
- **Lifecycle:** shown by an explicit script command, persists across message boxes until hidden, cleared by an explicit hide command (and defensively cleared on script end / map change).

### Files
- New module: `src/npc_portrait.c` + `include/npc_portrait.h`
- Portrait constants: `include/constants/portraits.h`
- Portrait graphics: `graphics/portraits/` (placeholder PNG + palette to start)
- Script command layer: `src/scrcmd.c`, `data/script_cmd_table.inc`, `asm/macros/event.inc`
- Config toggle in `include/config/`

### Tasks
1. Define a portrait ID enum in `include/constants/portraits.h`, seeded with our known important characters plus a placeholder:
   ```
   PORTRAIT_PLACEHOLDER = 0,
   PORTRAIT_CLARKSON_GENGAR,
   PORTRAIT_MADAM_TSUJI,
   PORTRAIT_MUTRID_LEADER,
   PORTRAIT_RED_FATALITY,
   PORTRAIT_DRACO,
   PORTRAIT_COBRA,          // the Dave-the-Diver-style contact NPC
   PORTRAIT_COUNT
   ```
   (Adjust/add as needed; leave it easy to extend.)
2. Build a portrait table mapping each ID → `{ tiles pointer, palette pointer }`. **Every ID initially points at the placeholder graphic/palette.** Real art later = add the PNG, point that ID's row at it. That's the only change.
3. Implement the module:
   - `ShowNpcPortrait(portraitId, side)` — load the portrait's tiles+palette, spawn the 64×64 sprite on the given side, above the message box.
   - `HideNpcPortrait()` — despawn and free.
   - Guard against double-loads (showing a new portrait while one is up should swap cleanly).
4. Add script commands + macros:
   - `showportrait <PORTRAIT_ID>, <PORTRAIT_LEFT|PORTRAIT_RIGHT>`
   - `hideportrait`
   Example intended usage:
   ```
   showportrait PORTRAIT_MADAM_TSUJI, PORTRAIT_RIGHT
   setspeakername Text_MadamTsuji_Name
   msgbox Text_MadamTsuji_Intro, MSGBOX_DEFAULT
   hideportrait
   ```
   (This composes with the namebox from Feature 2.)
5. Poryscript: same detection/registration step as Feature 2 if the project uses `.pory`.
6. Defensive cleanup: ensure the portrait is cleared if a script ends or the player warps while one is displayed, so it can't get stuck on screen.
7. Wrap in the config toggle.

### Placeholder art
Provide one `graphics/portraits/placeholder.png` — a 64×64 indexed PNG, 16-color, transparent index 0. Suggest a neutral silhouette or a simple "?" bust, or the Flatfoot Games mark, so it's obviously a stand-in during testing. All enum IDs point at it until real art exists.

### Acceptance criteria
- `showportrait PORTRAIT_PLACEHOLDER, PORTRAIT_RIGHT` inside a scripted `msgbox` shows the placeholder on the right, beside the text.
- `PORTRAIT_LEFT` mirrors it to the left.
- Sequential `showportrait` calls swap the portrait cleanly (no ghosting, no palette corruption of other graphics).
- `hideportrait` clears it; warping/ending the script mid-display also clears it.
- Composes correctly with the namebox (portrait + name plate + message together).
- **Swapping a real portrait in requires only: add the PNG, repoint that enum's table row.** Confirm this by wiring one test ID to a second placeholder and showing both work with no script edits.
- Config toggle disables the feature.

---

## Feature 4 — Quest toast notification (UPDATE to the existing quest menu system)

> **This is a modification to code that already exists, not a new system.** The quest menu system is already implemented in this project — hook into it, do not rebuild it. All choices below are the recommended defaults and are locked in.

### What we want
Replace the current quest notification — which appears as a **blocking dialogue window** that requires an A press and stops the player — with a non-blocking **toast**: a small popup that slides into a **top corner**, holds briefly, plays a short chime, and slides out on its own, without interrupting movement or requiring input. It fires on quest **start**, **update**, and **completion**.

### Approach (recommended defaults, all baked in)
- **Render it as a background task, not a message box.** The blocking behavior comes from routing through the field-message/script path (`lockall` + wait for A). The toast runs as its own task that ticks each frame, reads **no input**, and manages its own lifecycle (slide in → hold → slide out → destroy). Do **not** `lockall`/`lock`, and do **not** route through the field-message wait — that is precisely what reintroduces blocking.
- **Clone the map-name popup as the template.** `src/map_name_popup.c` (the "Route 103" banner that slides in on map entry) already implements this exact UX: non-blocking display, auto-dismiss, slide animation, and cleanup on menu/transition. Base the toast on it — `ShowMapNamePopup` plus the `Task_MapNamePopUpWindow` state machine (**confirm the exact names in the installed expansion version**) — then adapt: reposition to a top corner, shrink the window, swap the contents to quest text, add the sound, and trigger it from the quest system instead of on map entry.
- **Window-based rendering** (recommended over sprite-based): a small window on a BG layer in the top corner; animate by scrolling that BG's offset.
- **Slide in/out animation** (the map-popup default). Avoid an abrupt pop; a fade is an acceptable alternative but slide is the pick.

### Integration point — the "update to existing" part
Trigger the toast directly from the existing quest system's code, at the three moments it already detects state changes. Find where the quest system currently advances quest state **and where it currently pops the blocking window**, and replace that call with the toast call — then remove or gate the old blocking notification so both don't fire:

| Event | Toast text (example) | Sound |
|---|---|---|
| Quest start | `New Quest: <name>` | `PlaySE(SE_PIN)` |
| Quest update / objective progress | `<name> — updated` | `PlaySE(SE_PIN)` |
| Quest complete | `Quest Complete: <name>` | `PlaySE(SE_SUCCESS)` |

Optionally also expose a non-locking script command (e.g. `questpopup <QUEST_ID>, <EVENT>`) if any scripts need to trigger a toast directly — but the **primary** path is the existing quest-system hooks above, not scripts.

### Sound mapping (locked)
- Quest **start** → `SE_PIN`
- Quest **update** → `SE_PIN`
- Quest **complete** → `SE_SUCCESS`

Play the SE **once**, on the frame the toast spawns. Both are short, light built-ins; confirm the constant names in `include/constants/songs.h` for this version.

### Files
- New toast module: `src/quest_toast.c` + `include/quest_toast.h` (a trimmed clone of the map-popup) — **or** fold it into the existing quest-system source if that's cleaner in this codebase.
- The **existing** quest-system source file(s): modify the start/update/complete paths to call the toast and stop firing the old blocking window.
- New toast graphic + palette (small corner frame) under the project's interface gfx folder, matching the existing asset-declaration convention.
- Config toggle in `include/config/` (`USE_QUEST_TOAST`) so it can be turned off and reverted to the old window.

### Behavior details to handle
- **Stacking (FIFO queue):** quests can update back-to-back — most commonly a completion that immediately starts the next quest. Use a small **FIFO queue** so toasts play one after another rather than overlapping, so the player gets both the `SE_SUCCESS` and the following `SE_PIN` in order.
- **Cleanup on context change:** hide and free the toast (and clear/park the queue as appropriate) if a battle starts, the Start menu opens, or the player warps mid-display, so it can't get stuck on screen or fight other UI for its window/BG/palette slot. Cloning the map popup covers much of this — verify it holds for the queued case.
- **Resource budget:** it needs a window slot, a BG priority above the overworld, a palette slot, and a few tiles. Pick ones that don't collide with the start menu (Feature 1), namebox (Feature 2), portrait system (Feature 3), or the quest menu itself.

### Acceptance criteria
- Starting a quest shows a corner toast that slides in, holds, and slides out — with **no** `lockall`, **no** A press, and the player able to keep walking the whole time. `SE_PIN` plays once on appear.
- A quest update behaves the same, with `SE_PIN`.
- Completing a quest shows the toast with `SE_SUCCESS`.
- A complete-then-immediately-start sequence shows **both** toasts in order via the queue, with both sounds and no overlap.
- The old blocking dialogue-window notification no longer appears.
- Opening the Start menu / entering a battle / warping mid-toast cleans it up with no leftover graphics or palette corruption.
- Config toggle off restores the previous (blocking) notification.

---

## Suggested implementation order
1. **Namebox (Feature 2)** — fastest to stand up and immediately useful; also the smallest surface area.
2. **Portraits (Feature 3)** — highest story impact; composes with the namebox.
3. **Graphical start menu (Feature 1)** — most involved (UI rewrite), do last.

Commit each independently. Confirm a clean `make` and no dialogue/menu regressions after each.

Features 1–3 are already implemented; **Feature 4 (quest toast) is the current, standalone task.** It modifies the existing quest system and can be done independently of the above.

---

## Open questions for the implementing agent to confirm in-repo
- Exact expansion version and whether it uses Poryscript.
- The precise current names of the start-menu build/draw/input functions and the message-box draw path (grep before editing).
- The project's current graphics-declaration convention (how sheets/palettes are incbin'd) — match it for all new PNGs.
- Whether the project already has a `include/config/ui.h` or where new UI config defines should live.
- **(Feature 4)** Where the existing quest system advances quest state and where it currently triggers the blocking notification window — that call site is the integration point for the toast.

---

## Asset sourcing notes (for Joe — not blocking; placeholders cover the build)

**Start-menu icons.** Options, roughly in order of licensing cleanliness:
- **itch.io Pokémon-style UI kits** — e.g. Tavare's "Pokemon Style UI Kit / Asset Pack (PixelArt)", Anima_nel's "GBC Modern UI Assets", and the broader `pokemon` + `User Interface` asset tag. Clearest licensing (each listing states its terms; some free, some a few dollars). Most will need recoloring/resizing to 32×32 and quantizing to a 16-color palette — same pipeline you already run.
- **PokéCommunity "ROM Hacking Patches Pack"** (LibertyTwins) — includes BW / DPPt / ORAS / SwSh-style menu graphics, incl. a "BW Start Menu". Useful as **art/visual reference and source sprites** (the code itself we're writing fresh for expansion; those patches are largely FireRed binary patches, not decomp drop-ins). These are community recreations of official UI — credit the creators, and note it's the usual fangame gray area licensing-wise.
- **Custom icons** — best for originality and your Flatfoot Games identity. Given the placeholder system, you can ship and playtest now and commission/draw finals later.

**Character portraits.** For a GBA-appropriate 64×64 stylized bust:
- **PMD sprite collab** (sprites.pmdcollab.org) — huge free portrait library (Mystery Dungeon style, credit required per artist). Good if a PMD-ish portrait aesthetic fits, or as style reference.
- **itch.io character-portrait packs** — several "Pokémon style RPG characters with character portraits" packs exist under the pokemon asset tag; check each pack's license.
- **Commission / custom** — for named story characters (Clarkson's Gengar, Madam Tsuji, the Mutrid leader, Cobra), custom art will land best. The placeholder wiring means art can arrive one character at a time without touching scripts.

Whatever the source: index to 16 colors, transparent index 0, and credit per each asset's terms in the Flatfoot Games credits doc.
