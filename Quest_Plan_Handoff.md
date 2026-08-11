# Quest Plan Handoff — Wishes of Tomorrow

**Type:** Cowork implementation handoff (RESERVE)
**Status:** Joe is testing the existing **lightweight quest system** first. Implement
this only **if** he decides to switch to the refined **PSF Unbound Quest Menu** (see
`PSF_Quest_Menu_Porting_Handoff` for the port itself). This doc = the **content + plan**.
**Depends on (if adopted):** the PSF quest menu ported into the expansion build.

---

## 0. Global design rules (apply to every quest)

1. **Hidden until picked up.** No quest is visible in the menu — not even as "locked" —
   before its trigger fires. **Only call `startquest` at the trigger point.** Do NOT
   pre-register quests as visible/locked. The player should never see a quest they
   haven't yet encountered.
2. **Main vs Side.** Categorized below. If the menu supports a type/filter, tag them so
   main-story quests read distinctly from optional side quests.
3. **Multi-stage quests** (the ones with an "update" step) can be modeled two ways —
   Cowork's choice per the PSF docs:
   - as a **parent quest with sequential subquests** (stage = subquest), or
   - as a **single quest whose active objective text is swapped** at the update.
   The stage text below is written to work either way.
4. **Use the exact command names from the PSF scripting reference.** Below I use
   `startquest` / `completequest` for the core flow and note update points; confirm the
   real macro names (and any subquest commands) against the PSF wiki.
5. **Purify / shadow-legendary quests are intentionally PARKED** (see §4) — do not build
   them yet.

---

## 1. Quest roster

| ID | Type | Name | Triggers when | Completes when |
|---|---|---|---|---|
| `QUEST_WAKING_MOUNTAIN` | MAIN | **The Waking Mountain** | early Act 1 | (continues into Act 2) |
| `SIDE_QUEST_ONE_THAT_GOT_AWAY` | Side | **The One That Got Away** | fisherman's-son dialogue (Munen) | reunite at the boy's home |
| `SIDE_QUEST_FROZEN_TRACKS` | Side | **Frozen in Its Tracks** | board train + beat the goon | earn the Ember Badge |
| `SIDE_QUEST_EIGHT_WAYS` | Side | **Eight Ways to Grow** | talk to Eevee trainer (park) | acquire Eevee |
| `SIDE_QUEST_HEAVY_METAL` | Side | **Heavy Metal** | talk to trade NPC (park) | trade a starter for Scyther |
| `SIDE_QUEST_SAVE_FACE` | Side | **Save Face** | interact w/ MaskMaker (NewMap) | show the mask to the MaskMaker |
| `QUEST_GRANDFATHERS_WISH` | MAIN | **Grandfather's Last Wish** | receive the Catalpa Bow (Ashlands) | play the rite at Clarkson's grave |
| `QUEST_LIFES_WORK` | MAIN | **His Life's Work** | start of Act 2 (OnsenSprings) | destroy the research (Munen lab) |

---

## 2. Main quests

### `QUEST_WAKING_MOUNTAIN` — "The Waking Mountain"
- **Trigger:** early Act 1 (leaving Munen / townsfolk mention Clarkson). `startquest`.
- **Stage 1 — active:**
  > *The thousand-year ice is breaking, and no one knows why. Only Professor Clarkson
  > seems to understand it. Earn your footing and seek him out.*
- **Update — Clarkson's call after the Ember Badge:** advance to Stage 2.
- **Stage 2 — active:**
  > *Clarkson has called from the peak. Ascend Mt. Munen to the summit and meet him
  > where the ice finally ends.*
- **Continues** into the summit disaster and Act 2 (further stages TBD — see §5).
- *(Name is a placeholder for the Act 1 main line — confirm, and confirm the exact start
  point.)*

### `QUEST_GRANDFATHERS_WISH` — "Grandfather's Last Wish"  *(MAIN, per Joe)*
- **Trigger:** receiving the Catalpa Bow from the dying grandfather (Ashlands). `startquest`.
- **Active:**
  > *A dying man in the Ashlands pressed his family's Catalpa bow into your hands. Cross
  > the water to the park where the dead are remembered, and play the mourning rite at
  > his son's grave.*
- **Complete:** the player plucks the bow at Clarkson's grave. `completequest`.
  *(The Clarkson dream follows — but the quest text never promises the dead will answer.
  Keep the séance a surprise.)*
- ⚠️ *Note: this sits on the critical path (completing it leads to Clarkson-as-Gengar and
  onward progress). Confirm it's genuinely required, not skippable, now that it's MAIN —
  resolves the earlier optional-vs-gate question in favor of "required."*

### `QUEST_LIFES_WORK` — "His Life's Work"
- **Trigger:** start of Act 2, in OnsenSprings. `startquest`.
- **Active:**
  > *Professor Clarkson's final words were to destroy his research before it falls into
  > the wrong hands. Reach his lab in Munen — and undo everything he built.*
- **Complete:** the player resolves the research at the Munen lab. `completequest`.
- ⚠️ *Branch: canon lets the player DESTROY or KEEP the research. Decide whether "keep"
  completes the quest differently (e.g., a distinct completion line) or leaves it
  unresolved. Flag for Joe.*
- *(Name alt: "The Professor's Last Request.")*

---

## 3. Side quests

### `SIDE_QUEST_ONE_THAT_GOT_AWAY` — "The One That Got Away"
- **Trigger:** end of the fisherman's-son dialogue in Munen. `startquest`.
- **Stage 1 — active:**
  > *A boy in Munen waits by the lake for a father who never came back from the ice.
  > Find out what became of him.*
- **Update:** the father is found (end of the Red Gyarados cutscene, frozen far shore of
  Munen Lake — Act 2). Advance to Stage 2.
- **Stage 2 — active:**
  > *His father is alive. Return to the boy's home in Munen and end his long wait.*
- **Complete:** the reunion at the boy's home. `completequest`.

### `SIDE_QUEST_FROZEN_TRACKS` — "Frozen in Its Tracks"
- **Trigger:** board the Shinkansen interior and defeat the goon (who ices the gears as
  he's driven off). `startquest`. **Hidden entirely if the player never boards the train.**
- **Active:**
  > *A Mutrid saboteur froze the Sennen Line's gears solid before you drove him off. The
  > train won't budge until someone finds a way to thaw it.*
- **Complete:** the player earns the Ember Badge (Red Fatality's Arcanine thaws/powers
  the line). `completequest`.

### `SIDE_QUEST_EIGHT_WAYS` — "Eight Ways to Grow"
- **Trigger:** talking to the Eevee trainer in the national park. `startquest`.
- **Active:**
  > *A trainer in the park raises Eevee -- the Pokémon that could grow into almost
  > anything. Show her you're worth it, and one may choose you.*
- **Complete:** the player acquires Eevee. `completequest`.
- *(Name nods to Eevee's eight evolutions. Alt: "A World of Options.")*

### `SIDE_QUEST_HEAVY_METAL` — "Heavy Metal"  *(name per Joe)*
- **Trigger:** talking to the trade NPC in the national park. `startquest`.
- **Active:**
  > *A collector in the park will trade a Scyther for one of your starters. He swears the
  > right coat of metal could turn it into something unstoppable.*
- **Complete:** the player trades a starter for the Scyther. `completequest`.
- *(Description seeds the Scizor / Metal Coat payoff behind the "Heavy Metal" name.)*

### `SIDE_QUEST_SAVE_FACE` — "Save Face"
- **Trigger:** interacting with the MaskMaker in NewMap. `startquest`.
- **Stage 1 — active:**
  > *The MaskMaker's apprentice lost a finished mask deep in the cave. Recover it before
  > the old craftsman loses face -- and the apprentice loses their place.*
- **Update:** the mask is acquired in the cave. Advance to Stage 2.
- **Stage 2 — active:**
  > *You have the missing mask. Bring it back to the MaskMaker.*
- **Complete:** showing the mask to the MaskMaker. `completequest`.
- *(The oni-mask motif ties the MaskMaker loosely to Team Mutrid's masks — room for later
  foreshadowing.)*

---

## 4. Parked (do NOT build yet)

- **Purification / Shadow-Legendary quests** — deliberately deferred at Joe's request.
  When added, they'll suit the **subquest** structure best: a parent like *"Cleanse the
  Corrupted"* with **one subquest per shadow legendary**, each completing when that
  legendary is purified at Celebi's shrine. Left out entirely for now.

---

## 5. Implementation notes

- **Quest constants:** add the IDs above to the quest-definitions array; wire each `name`
  + `description`(s) + type. Keep the array order stable once shipped (states are indexed).
- **Hidden state:** ensure the default state is hidden/locked-and-invisible; the quest
  only surfaces on `startquest`. Verify this behaves as "not shown" (not "shown as ???")
  if that's the desired feel — Joe wants them fully unknown.
- **Multi-stage** (`ONE_THAT_GOT_AWAY`, `SAVE_FACE`, `WAKING_MOUNTAIN`): pick subquests
  vs objective-text-swap per §0.3.
- **Expansion reminder:** if porting PSF's menu, apply the `CreateMonIcon` one-argument
  fix and mind the global list-menu wrapping + saveblock cost (see the porting handoff).
- **Sprites:** each quest can show an item/NPC/Pokémon icon — natural picks: Red Gyarados
  or a fishing-rod item (`ONE_THAT_GOT_AWAY`), a train/ice tile (`FROZEN_TRACKS`), Eevee
  (`EIGHT_WAYS`), Scyther (`HEAVY_METAL`), a mask item (`SAVE_FACE`), the Catalpa Bow item
  (`GRANDFATHERS_WISH`), Jirachi or the summit (`WAKING_MOUNTAIN`).

---

## 6. Open inputs from Joe
- Confirm/rename **"The Waking Mountain"** (Act 1 main) and its **start point**.
- **His Life's Work:** does KEEPING the research complete the quest differently?
- **Grandfather's Last Wish** confirmed required (MAIN), not skippable?
- Final calls on the clever names (**Eight Ways to Grow**, **Save Face**, **The One That
  Got Away**, **Frozen in Its Tracks**) — swap any you don't love.
- Later: unpark the purification subquest line when you're ready.
