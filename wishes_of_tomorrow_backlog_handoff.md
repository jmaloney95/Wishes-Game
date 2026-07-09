# Wishes of Tomorrow — Feature & Bug Backlog (Handoff #2)

**For:** Claude Code
**Project:** *Wishes of Tomorrow* (Flatfoot Games) — Pokémon Emerald ROM hack
**Base:** RHH **pokeemerald-expansion**
**Companion doc:** `wishes_of_tomorrow_ui_features_handoff.md` (UI features 1–4). Ground rules from that doc **apply here too** — especially: verify all function/constant names against the installed expansion version before editing, work on feature branches, gate new behavior behind config defines where practical, and match the project's existing asset-declaration conventions.

---

## How to read this doc

Each item has a **Complexity** rating and a **Verdict**.

| Rating | Meaning |
|---|---|
| **Trivial** | Minutes. Text/data change, no new systems. |
| **Low** | Standard scripting or a small flag/var addition. Well-trodden. |
| **Medium** | New code path or custom item/menu logic. Touches several files. |
| **High** | New subsystem, or significant engine-adjacent work with real risk of regressions/resource pressure. |

**Verdicts:** `DO NOW` · `DO NOW (blocked on asset)` · `DEFER` · `NEEDS DECISION FIRST`

---

## Recommended order of work

1. **§1 first, alone.** It is game-breaking and progression-blocking. Do not start anything else until it's fixed.
2. **§2** — trivial, clear it out of the way.
3. **Story blockers** (§3, §4) — needed for Act 2 to play correctly end to end.
4. **Item/menu systems** (§5, §6) — these two share machinery; build them together.
5. **Overworld item icons** (§9, scoped to three items) and **Gengar follower animation** (§7) — art-facing, independent of the above.
6. **Armored Mewtwo** (§8) — **last, on its own branch.** It touches species tables; keeping it isolated makes any save/Pokédex regression trivially bisectable.

Nothing on this list is now recommended for outright cutting. The only deferred item is the *unscoped* "every item shows its own icon" system (see §9, Out of scope).

---

# BUGS

## §1 — GAME-BREAKING: opponent Aurorus flees mid-battle and ends the trainer battle

**Complexity: Medium (diagnosis) → Low (fix)**
**Verdict: DO NOW — priority 1, before anything else on this list**

### Symptom (confirmed)
During a **trainer battle**, the opponent's **Aurorus flees and the battle ends on the spot.** This is the opponent leaving the field, not the player pressing RUN. A trainer's Pokémon should never be able to flee, and a flee should never terminate a trainer battle.

### Why this is priority one
This is progression-breaking. If the battle can end early, the player skips a story-critical fight — and in Act 2 the rival battle at the Munen lab is what gates the shadow dossier beat (§4 below). Fix this before building anything on top of it.

### Root-cause checklist (work in this order — most likely first)

**1. Battle type flags — check this first.**
A trainer's Pokémon fleeing and *ending the battle* is the signature behavior of a **wild** battle. Strongly suspect the battle is being started without `BATTLE_TYPE_TRAINER`, or with wild-battle flags set. Find the script that starts this battle and confirm it goes through the trainer-battle path (`trainerbattle` / the appropriate expansion macro) and not a wild/scripted-wild path. If Aurorus is on a trainer's party this is the #1 candidate; if this battle was ever prototyped as a scripted wild encounter, it's almost certainly the cause.

**2. Held item.**
Check Aurorus's held item for **Smoke Ball** (guarantees escape) or **Eject Button**. Smoke Ball on a trainer's mon in a battle mis-flagged as wild would produce exactly this.

**3. Ability.**
Check Aurorus's assigned ability. Aurorus's legitimate abilities (Refrigerate / Snow Warning) don't cause fleeing, so **if it has anything else, that's the bug.** Watch for a misassigned **Run Away** (wild-escape only), or an **Emergency Exit / Wimp Out**-style switch-out ability — those switch in trainer battles but *end* wild battles.

**4. Moveset.**
Check for switch-out moves: **Teleport** (Gen 8+ switches the user out; in a wild battle it flees), **U-turn / Volt Switch / Flip Turn / Parting Shot / Baton Pass**. Also check whether the *player* is using **Roar / Whirlwind** — a forced switch against a trainer's **last remaining Pokémon** is a classic edge case that can be mishandled into an early battle end.

**5. Party size.**
If Aurorus is the trainer's **only** Pokémon, any switch-out effect has nowhere to switch to. Verify the engine blocks the effect rather than resolving it as an escape.

**6. Expansion config / effect gating.**
Confirm whichever effect is implicated correctly no-ops or converts to a switch when `BATTLE_TYPE_TRAINER` is set. This is where a genuine engine-level bug would live, but check 1–5 first — a data/scripting mistake is far more likely than an expansion battle-engine bug.

### Diagnostic instructions
- Reproduce in mGBA and **record the exact on-screen message** when the battle ends ("Got away safely!" vs "…fled!" vs a switch message vs nothing). The message text identifies which code path fired and will collapse the checklist immediately.
- Note the exact turn/trigger: does it happen on a specific move, at a specific HP threshold (≈50% would implicate Emergency Exit / Wimp Out), or immediately?
- Dump Aurorus's species, ability, held item, and full moveset from the trainer party data.

### Tasks
1. Diagnose via the checklist above. **Identify the actual root cause before changing anything.**
2. Fix at the root. Do **not** patch by suppressing the flee message or force-disabling escape — that hides the bug and may leave the battle in a bad state.
3. Determine whether this affects **only Aurorus** or any mon with the same item/ability/move. Fix the general case if so.
4. Regression-test: every trainer battle in Acts 1–2 starts and ends correctly; wild battles can still be fled; switch-out moves/abilities still switch correctly in trainer battles.

### Acceptance criteria
- Aurorus cannot flee; the trainer battle runs to a normal conclusion.
- No trainer's Pokémon can end a trainer battle by escaping.
- Wild-battle fleeing is unaffected.
- Switch-out moves and abilities still behave correctly in both battle types.
- Root cause is documented in the commit message.

### Open question for Joe
Is Aurorus on the **rival's** team, or a different trainer? (Earlier report mentioned "Aurorus **or** rival's starter" — confirm whether the rival's starter also exhibits this, or whether Aurorus is the only case.)

---

## §2 — Goon name in Shinkansen interior

**Complexity: Trivial**
**Verdict: DO NOW**

### Symptom
A goon on the Shinkansen interior map displays as `Team Mutrid ???`. It should read `??? Goon`.

### Tasks
1. Determine whether this string comes from the **trainer class name**, the **trainer name**, or a combination the game concatenates for display (trainer class + name is the usual pattern — which is exactly why it currently reads "Team Mutrid ???").
2. Fix so it displays `??? Goon`. Depending on how the display string is assembled, this may mean creating/using a trainer class of `???` with the name `Goon`, rather than editing a single string.
3. Check whether other Mutrid goons on other maps share the same class/name and would be affected. Scope the change so only the intended NPC changes, unless the same fix is desired everywhere (**confirm with Joe if it's shared**).

### Acceptance criteria
- The NPC's namebox / battle intro reads `??? Goon`.
- No other trainers' names regress.

---

# STORY BLOCKERS

## §3 — Tradewind curio: limit to one purchase each

**Complexity: Low**
**Verdict: DO NOW**

### Symptom
In Tradewind Town, the "impossible goods" curio stall currently has unlimited stock. The player should only be able to purchase **1 of each curio**.

### Context
This stall sells impossible items (bottled warmth, borrowed years, jar of unspoken last words, etc.) at 1,000 each, sells back at 1,000 with no risk, and buying one unlocks the next. That progression logic implies flags already exist — extend that pattern rather than inventing a new one.

### Approach
**Do not use a vanilla Poké Mart.** Vanilla marts have unlimited stock by design and fighting that is more work than the alternative. Implement (or convert to) an **NPC script per curio**:
- Check the "already purchased" flag for that curio. If set, the NPC gives a "sold out / that was the only one" line.
- Otherwise check money ≥ 1,000, take money, give item, set the purchased flag, and set the "next curio unlocked" flag.

If the stall is *already* an NPC script and not a mart, this is just adding the purchased-flag check.

### Tasks
1. Locate the existing Tradewind curio stall script and determine whether it's a mart or an NPC script.
2. Add a `FLAG_CURIO_<NAME>_PURCHASED` per curio (check for free flags in the project's flag list; do not reuse a flag that's already live).
3. Guard the purchase on that flag; add a sold-out dialogue branch.
4. Preserve the existing "buying one unlocks the next" behavior.
5. **Interaction with sell-back:** decide what happens if the player sells a curio back and returns. Recommended: the purchased flag stays set (they had their chance — it's a one-of-a-kind curio, which fits the fiction). **Confirm with Joe.**

### Acceptance criteria
- Each curio can be bought exactly once.
- The sold-out line fires on a second attempt.
- The unlock-the-next-curio chain still works.
- The Mega Stone conversion stall (post-Lantern Badge) is unaffected.

---

## §4 — Gengar gate: can't leave the lab without the shadow dossier

**Complexity: Low**
**Verdict: DO NOW**

### Symptom / desired behavior
In Act 2, after the player defeats the rival at the Munen lab, Gengar should speak a line and set a trigger that prevents the player from leaving the lab until they've interacted with the **shadow dossier**.

### Approach
Standard gating pattern, three pieces:
1. **On rival defeat:** set a var/flag (e.g. `FLAG_LAB_DOSSIER_GATE_ACTIVE`), and fire Gengar's line.
2. **On the exit:** a script trigger tile in front of the lab exit (or a script attached to the warp) that, while the gate flag is set and the dossier flag is *not* set, blocks the player, plays a "wait —" beat, and optionally repeats a short Gengar nudge line.
3. **On dossier interaction:** set `FLAG_SHADOW_DOSSIER_READ`, clear the gate, allow exit.

Use a script trigger tile rather than modifying warp behavior — it's the idiomatic pattern and easier to reason about.

### Tasks
1. Add the two flags (find free flags; don't reuse).
2. Hook the rival-defeat script to set the gate flag and deliver Gengar's line.
3. Place the blocking script trigger at the lab exit; write the blocked-exit dialogue.
4. Hook the dossier object's interaction script to set the read flag and clear the gate.
5. Ensure the gate can't persist after the beat is complete (clear it on dossier read, and defensively on the Act 2 progression var advancing).

### Content needed from Joe
- Gengar's line on rival defeat.
- Gengar's nudge line when the player tries to leave early.

### Acceptance criteria
- After beating the rival, Gengar speaks; the exit is blocked.
- Attempting to leave replays a short nudge and returns the player to the room (no soft-lock, no getting stuck in the trigger).
- Reading the dossier releases the gate; the player can leave.
- Saving and reloading mid-gate preserves the gate state correctly.
- The gate does not re-arm on a later visit to the lab.

---

# ITEM & MENU SYSTEMS

> **Build §5 and §6 together.** Both are "key item, when used from the bag, opens a small choice menu, and each choice runs a different script." That is one reusable pattern. Implement the pattern once and use it twice.

## §5 — Catalpa Bow: Teleport to Lab / Go to Distortion World

**Complexity: Medium**
**Verdict: NEEDS DECISION FIRST, then DO NOW**

### Desired behavior
In Act 2's distortion world, Gengar / Prof. Clarkson says a line along the lines of "talk to me when you're ready to teleport home." Selecting the **Catalpa Bow** item then presents a choice: **Teleport to the Lab** or **Go to the Distortion World**.

### Open question — resolve before implementing
The description has an ambiguity that changes the design:

- Gengar says *"talk to me if you are ready to teleport"*, but the menu is triggered by **using the item**, not by talking to Gengar. Which is the actual interaction?
  - **(a)** Gengar's line is just a hint; the real mechanism is always the bag item. (Simplest, and matches the rest of the description.)
  - **(b)** Talking to Gengar opens the choice menu, and the item is a separate/alternate route.
  - **(c)** Talking to Gengar is what *enables* the item's menu (before that, using the item does nothing / gives a "not yet" message).

**Recommendation: (a) or (c).** (a) is cleanest; (c) is nicer design because the item stays inert until the story says it's live. Also confirm: is the Catalpa Bow **repeatable** (a permanent fast-travel between the lab and the distortion world), or a **one-shot** exit? That changes the acceptance criteria substantially.

### Approach
1. Register the Catalpa Bow as a **key item with a field-use effect** (`ItemUseOutOfBattle_*` style function, registered in the item's data). Confirm how this version of expansion declares item field-use callbacks — item data structures were reorganized in recent versions (icon pointers moved into `gItemsInfo`), so verify before copying an old tutorial.
2. On use, close the bag and run a script that presents a **multichoice** menu: `Teleport to the Lab` / `Go to the Distortion World` / `Cancel`.
3. Each choice runs a warp script to the appropriate destination (with fade + any SFX).
4. Gate availability: if the item shouldn't work everywhere, guard it on the Act 2 progression var and/or current map, with a "nothing happens here" message otherwise.

### Content needed from Joe
- Gengar's "ready to teleport home" line.
- The exact warp destinations (map + coordinates) for both options.
- Whether the item is repeatable or one-shot.

### Acceptance criteria
- Using the Catalpa Bow in the valid context opens the three-option menu.
- Each option warps correctly; Cancel closes cleanly and returns to the field (not a stuck bag state).
- Using the item outside the valid context gives a sensible message and does not warp.
- Using it does not break the follower (Gengar) — verify the follower re-spawns correctly on the destination map.

---

## §6 — Clarkson's research: Read / Destroy from the bag

**Complexity: Medium** (Low if §5's pattern already exists)
**Verdict: DO NOW — after §5**

### Desired behavior
If the player chooses to **keep** Clarkson's research in Act 2, selecting the research item in the bag presents a menu with **Read** or **Destroy**.

### Approach — important scoping note
There are two ways to do this, and one is much cheaper:

- **Recommended:** the item's **field-use effect** opens a multichoice: `Read` / `Destroy` / `Cancel`. This reuses §5's pattern exactly, requires no changes to bag internals, and is what the player experiences as "select the item, get options."
- **Not recommended:** modifying the bag's context menu (the `Use / Give / Toss / Cancel` list) to add custom verbs for this one item. This touches bag UI internals, is more fragile, and risks regressions across the whole bag. Only do this if Joe specifically wants the verbs to appear in the context menu itself.

**Default to the recommended approach unless told otherwise.**

### Tasks
1. Register the research as a key item with a field-use callback (reuse §5's helper).
2. `Read` → display the research text (a standard message sequence; can be multiple boxes). Repeatable.
3. `Destroy` → confirmation prompt (`Are you sure? This cannot be undone.`), then on yes: remove the item from the bag, set a `FLAG_RESEARCH_DESTROYED`, and play an appropriate SFX/beat.
4. Ensure `FLAG_RESEARCH_DESTROYED` and the "kept research" state feed the **Act 3 Shadow Jirachi finale branch** — this choice is tied to the "mend not smash" path. Verify whatever var/flag Act 3 reads is the one being set here. **Do not introduce a second, parallel state variable.**

### Content needed from Joe
- The research text (what `Read` displays).
- The destroy confirmation + aftermath lines.

### Acceptance criteria
- Selecting the item shows `Read / Destroy / Cancel`.
- `Read` is repeatable and doesn't consume the item.
- `Destroy` prompts for confirmation; cancel at the prompt leaves the item intact.
- Destroying removes the item and sets exactly one canonical state flag that Act 3 reads.
- Save/reload preserves the state.

---

# CONTENT ADDITIONS

## §7 — Gengar follower custom animation

**Complexity: Medium** (possibly Low — depends on the asset)
**Verdict: DO NOW — verify the asset path first (see below)**

### Desired behavior
Apply a custom movement animation to Gengar when it is the active follower.

### ⚠️ Asset access
The source is at `J:\ROM Hack Project\custom sprites\gengar_new_movement` on Joe's **Windows** filesystem. Claude Code runs in **WSL** — this path must be accessed as `/mnt/j/ROM Hack Project/custom sprites/gengar_new_movement` (mount point may differ; verify). **Inspect the asset before estimating.** The complexity swings entirely on what it contains:

- **If it's additional frames for the existing walk cycle** → Low. Extend the sprite sheet and the animation table's frame count/timing.
- **If it's a new animation type** (e.g. a float/hover idle replacing the walk, a unique appear/disappear) → Medium. Requires new anim table entries and possibly hooking where the follower's animation is selected.

### Approach
1. Inspect the asset: dimensions, frame count, layout (rows = directions?), and whether it's already indexed.
2. Confirm the follower graphics path for this expansion version. As of 1.9.0, follower graphics live at per-species paths like `graphics/pokemon/<species>/follower.png`.
3. **Shiny palette:** the story Gengar is shiny (white). Expansion supports shiny overworld sprites via `OBJ_EVENT_GFX_SPECIES_SHINY()` and ships shiny follower palettes. Any new frames must be authored against the **same 16-color palette** as the existing shiny Gengar follower, or the shiny render will break.
4. Update the pic table + animation table to match the new frame count. **Frame count in the sheet and the anim table must agree** or you get garbage/crashes.
5. Note `OW_FOLLOWERS_BOBBING` already applies an idle/walk bob. Check whether the custom animation is meant to **replace** the bob or **stack** with it — stacking may look wrong. **Confirm with Joe.**

### Gotchas
- `OW_GFX_COMPRESS` compresses follower graphics and is **incompatible with non-power-of-two sprite sizes**. If the new sheet isn't a power-of-two frame size, either resize or disable compression.
- Compressed gfx are loaded into VRAM; more frames = more VRAM. Watch this if the map has many overworld Pokémon.

### Acceptance criteria
- Gengar-as-follower plays the new animation.
- Shiny (white) palette renders correctly.
- No VRAM/palette corruption when other object events are on the map.
- Follower still behaves correctly through warps, script movement, and Poké Ball return.

---

## §8 — Armored Mewtwo replaces shiny Mewtwo (developer gift NPC)

**Complexity: Medium** (was Medium–High; art being complete removes the hard part)
**Verdict: DO — but last, after §1–§7. Own branch.**

### Status update
**Art is complete and in the repo.** The artist delivered front, back, overworld, and follower sprites, now in the custom sprites directory. This unblocks the item and drops it from "defer" to "doable." What remains is data entry, not art: stats, moveset/learnset, Pokédex entry, and the gift script.

### Desired behavior
The NPC in the player's starter house who gifts a **shiny Mewtwo** to developers should instead gift **Armored Mewtwo**.

### Key design decision — make this a FORM, not a new species
This single choice determines whether it's Medium or High.

- **Recommended: a cosmetic form of Mewtwo** (`SPECIES_MEWTWO_ARMORED`, form of `SPECIES_MEWTWO`). It inherits Mewtwo's dex number, learnset, and cry. No new Pokédex slot, no new national dex entry, no learnset table to author. This is the cheap, low-risk path, and for a developer easter egg it's almost certainly the right one. Armored Mewtwo has no official stat spread — it's an anime/movie design — so inventing one buys nothing.
- **Heavier alternative: a fully distinct species** with its own stats, learnset, and dex entry. Choose this only if the armor is meant to *play* differently (e.g. redistributed bulk: more Def/SpDef, less Speed). It's a lot more table surface and much more risk.

**Default to the cosmetic form unless Joe says the stats should differ.** A cosmetic form still lets you set a distinct name string and dex flavor text if desired.

> You cannot simply swap Mewtwo's sprite — that changes *every* Mewtwo in the game. It must be a form/species entry either way.

### Approach
Follow the expansion's own tutorial: `docs/tutorials/how_to_new_pokemon.md` in the repo. Do not improvise from an older PokéCommunity tutorial — species/item data structures have been reorganized across recent expansion versions.

### Tasks
1. Read `docs/tutorials/how_to_new_pokemon.md` for the installed version.
2. Add `SPECIES_MEWTWO_ARMORED` as a form of Mewtwo. Register it in the form species table so form-aware code resolves it correctly.
3. Wire the delivered sprites: front, back, overworld, follower. Match the project's asset-declaration convention.
4. Set the species name string and (if desired) dex flavor text.
5. Update the developer gift NPC's script to give Armored Mewtwo instead of shiny Mewtwo. **Confirm whether it should still be shiny** — the old gift was a shiny Mewtwo; Armored may be intended as the "special" quality on its own. **Ask Joe.**
6. Verify it works as a follower (§7's system) and appears correctly in the party menu, PC, and summary screen.

### Assets still needed (verify before starting)
The delivered set may not be complete. Check for:
- **Party/box icon** — separate from the front sprite; easy to overlook.
- **Shiny palettes** — for front, back, and follower. If Armored Mewtwo can be shiny (or if the gift stays shiny), these are required.
- **Cry** — a cosmetic form inherits Mewtwo's automatically. Nothing to do unless a custom cry is wanted.

### Risk notes
- Species tables are wide-reaching. A mistake here can corrupt save data or the Pokédex.
- **Do this on its own branch.** Test specifically: save → reload → the Pokémon is intact; the Pokédex is not corrupted; boxing/withdrawing works; trading/battle facilities don't choke on the form.
- Do it **after** §1–§7 are stable, so a save-corruption regression is trivially bisectable.

### Acceptance criteria
- The developer NPC gifts Armored Mewtwo.
- It displays correctly: battle (front/back), party icon, summary, PC, and as a follower.
- Save/reload preserves it.
- Pokédex integrity is unaffected; base Mewtwo is unchanged everywhere else in the game.

---

## §9 — Overworld item icons instead of Poké Balls — SCOPED TO THREE ITEMS

**Complexity: Low–Medium** (was High at unlimited scope)
**Verdict: DO NOW — the scoped version is comfortably within hardware budget**

### Desired behavior
Ground item pickups display the actual item's icon in the overworld rather than the generic Poké Ball object — scoped to exactly **three** items: the **Carved Mask**, the **Catalpa Bow**, and a **Potion**.

### Why the scoped version is cheap (and the unscoped version wasn't)
**Cost scales with the number of distinct graphics, not the number of instances.** Ten Potions scattered across a map share one graphic, one tile set, and one palette. So three item graphics is three graphics, period — regardless of how many pickups exist in the game.

The unscoped version was High-complexity because per-item icons meant *hundreds* of distinct graphics, each with its own 16-color palette, against only 16 OBJ palette slots shared with the player, NPCs, and the Gengar follower. Three items sidesteps that entirely.

**Budget for three 16×16 items:**
- **Tiles:** a 16×16 sprite is 4 tiles (128 bytes) per frame. Three of them, static, is ~12 tiles. Negligible.
- **Palettes:** worst case 3 OBJ palette slots if all three are visible on one map simultaneously. Almost certainly never happens — but see the optimization below to make it 1.
- **Object event graphics table:** three new entries. Expansion has expanded overworld IDs (past the old 255 limit), so there's no pressure here.

**Nothing about this is too costly for the hardware.** The scoped version is safe.

### Recommended optimization: one shared palette
Author all three sprites against a **single shared 16-color palette** (a common palette that covers the mask, the bow, and the potion). This drops the cost to **one OBJ palette slot total**, no matter how many of the three appear on a map at once, and removes palette pressure as a concern permanently. It constrains the art slightly — 15 colors across all three items — but at 16×16 that's plenty.

If the artist needs more color freedom, fall back to one palette per item (3 slots). Still fine; just less headroom for other sprites.

### Approach
Use **approach (a): a distinct object event graphic per item**, hand-authored. Do **not** build the dynamic "load the bag icon at spawn" system — that's the High-complexity path and it buys nothing at this scope.

Mechanically, ground items are object events using `OBJ_EVENT_GFX_ITEM_BALL`. To change one, you change that object's **graphics ID** in Porymap. The existing pickup script (`Common_EventScript_FindItem` or equivalent) is unaffected — it reads which item to give from the object's script/var, not from the graphic. So this is mostly art + table entries + repointing a few object events.

### Tasks
1. Confirm the existing ground-item pickup script and how the object's graphics ID is assigned.
2. Add three object event graphics: `OBJ_EVENT_GFX_ITEM_CARVED_MASK`, `OBJ_EVENT_GFX_ITEM_CATALPA_BOW`, `OBJ_EVENT_GFX_ITEM_POTION` (match the project's naming convention). Register tiles + palette per the version's asset-declaration pattern.
3. In Porymap, repoint the relevant ground-item object events to the new graphics IDs. Leave all other ground items as Poké Balls.
4. Verify the pickup script still fires correctly and the object disappears + sets its flag on pickup.
5. Confirm the item still can't be re-collected after save/reload.

### Art spec
- **Size:** 16×16 px (one metatile — matches the Poké Ball object and sits correctly on the ground). Bag icons are drawn at a larger size and a different style; **do not** downscale bag icons directly, they will read poorly. These want a small overworld art pass.
- **Format:** indexed PNG, transparent index 0.
- **Palette:** ideally one shared 16-color palette across all three (see above).
- **Frames:** static is fine. A subtle 2-frame shimmer is affordable if wanted.

### Design note worth raising
The Poké Ball ground object is a strong, learned visual signal for "this is an item, press A." A Potion lying on the ground is less immediately readable as interactive. Consider keeping a small sparkle/shine animation, or reserving the custom icons for **key/story items** (Carved Mask, Catalpa Bow) where the "oh, that's *the* mask" moment carries real weight, while ordinary consumables like Potions stay as balls. **Joe's call** — the Potion is included here as specified, but it's the one of the three whose payoff is least certain.

### Acceptance criteria
- The Carved Mask, Catalpa Bow, and Potion ground items render as their own icons.
- All other ground items still render as Poké Balls.
- Pickup, flag-setting, and disappearance all work unchanged.
- No palette corruption with the player, NPCs, and a Gengar follower on the same map — **test this case specifically.**

### Out of scope (still deferred)
The general "every item shows its own icon" system. If that's ever wanted, it needs the dynamic `gItemsInfo` icon-loading approach, a palette-sharing strategy, and its own branch and budget.

---

# Summary table

| § | Item | Complexity | Verdict |
|---|---|---|---|
| 1 | **Aurorus flees, ends trainer battle** | Medium → Low | **DO NOW — priority 1, game-breaking** |
| 2 | `??? Goon` name fix | Trivial | **DO NOW** |
| 3 | Tradewind curio: 1 per purchase | Low | **DO NOW** |
| 4 | Gengar lab gate / shadow dossier | Low | **DO NOW** |
| 5 | Catalpa Bow teleport menu | Medium | **NEEDS DECISION** → DO NOW |
| 6 | Research: Read / Destroy | Medium (Low after §5) | **DO NOW** (after §5) |
| 7 | Gengar follower animation | Medium | **DO NOW** (verify asset path) |
| 9 | Overworld item icons (3 items only) | Low–Medium | **DO NOW** (scoped) |
| 8 | Armored Mewtwo | Medium | **DO LAST** (own branch) |

---

# Open questions for Joe (blocking)

1. **§1:** Is Aurorus on the **rival's** team or a different trainer's? Does the rival's starter also flee, or is Aurorus the only case?
2. **§2:** Should the `??? Goon` rename apply to all Mutrid goons, or only the Shinkansen interior NPC?
3. **§3:** If the player sells a curio back, can they re-buy it?
4. **§5:** Does the Catalpa Bow menu open from **using the item**, from **talking to Gengar**, or does talking to Gengar **enable** the item? And is the item repeatable fast-travel or a one-shot?
5. **§5:** Exact warp destinations (map + coords) for both menu options.
6. **§6:** Read/Destroy as an **item-use multichoice** (recommended) or as custom verbs in the bag's context menu (more invasive)?
7. **§6:** Confirm which existing var/flag Act 3 reads for the destroy-or-keep-research branch, so §6 sets that one and not a duplicate.
8. **§7:** Does the new Gengar animation **replace** the follower bob (`OW_FOLLOWERS_BOBBING`) or play alongside it?
9. **§8:** Cosmetic form (recommended) or fully distinct species with its own stats? And should the gift still be **shiny**, or is Armored the "special" quality on its own?
10. **§9:** Should the **Potion** get a custom overworld icon, or keep custom icons for key/story items only (Carved Mask, Catalpa Bow)? See the design note in §9.

# Content needed from Joe (non-blocking, can use placeholders)

- Gengar's line on rival defeat, and his "don't leave yet" nudge line (§4).
- Gengar's "ready to teleport home" line (§5).
- The research text shown by `Read`, plus destroy confirmation/aftermath lines (§6).
- Sold-out line for the curio stall (§3).
