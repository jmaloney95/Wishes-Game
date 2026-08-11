# Handoff: "Paw" Wish Items & Ability Machines (AMs)

**Project:** Wishes of Tomorrow (Flatfoot Games)
**Base:** pokeemerald-expansion (RHH), ~v1.16.x on WSL Ubuntu
**Owner:** Joe · **Implementer:** Claude Code
**Type:** Two custom features sharing one piece of infrastructure. Both are green-lit and understood; this doc scopes the plan, not just the idea.

> **Read this first.** Function/struct/file names below are anchored to what the current expansion actually uses, but the expansion renames things between versions (e.g. `base_stats.h` → `gSpeciesInfo`, `gBattleMoves` → `gMovesInfo`). Where a name is marked *(verify in current tree)*, grep for the concept before trusting the exact symbol. Do **not** invent offsets or field names — inspect the real headers.

---

## 0. Viability summary (why this is doable)

| Feature | Real engineering cost | Why it's not scary |
|---|---|---|
| **Paws** | Add a persistent per-mon flat stat modifier; hook it into stat calc; build a stat-choice sub-menu | Expansion already stores per-mon stat overrides for **Hyper Training**, so there's a working precedent for "per-mon data that alters `CalculateMonStats`." UI reuses existing party-menu + list-menu widgets. |
| **AMs** | Add a per-mon "override ability" field; one hook in the ability resolver; item modeled on TMs | **The abilities already exist in the engine.** An AM does *not* re-implement Intimidate/Guts/etc. — it just assigns an existing ability. A mon's ability is a slot index (`abilityNum`) resolved through `GetMonAbility()`; overriding that function is the whole trick. In battle, `gBattleMons[b].ability` is set from `GetMonAbility(mon)`, so the override "just works" once the resolver respects it. |

**Shared infrastructure:** both features need a couple of new persistent fields on the Pokémon data struct. Build that once (Section 1), then Features A and B slot on top.

**Difficulty:** moderate. The only genuinely delicate part is adding save-persistent fields (Section 1) — everything else is well-trodden decomp work.

---

## 1. Shared infrastructure — persistent per-mon fields

### 1.1 What to add

Two new pieces of per-mon data on `struct BoxPokemon` (`include/pokemon.h`):

| Field | Suggested type | Purpose | New `MON_DATA_*` accessor |
|---|---|---|---|
| Override ability | `u16` (0 = `ABILITY_NONE` = "no override") | Stores an arbitrary ability ID set by an AM | `MON_DATA_CUSTOM_ABILITY` |
| Paw stat modifiers | `s8 statMods[5]` (ATK, DEF, SPATK, SPDEF, SPEED) | Signed permanent stat deltas from Paws | `MON_DATA_STAT_MODS` (or one per stat) |

Total footprint ≈ 7 bytes. Ability IDs exceed 255 across all gens, so the override **must be `u16`**, not `u8`.

### 1.2 Save-compatibility decision — READ THIS

Adding fields to `BoxPokemon` can invalidate existing saves. Two paths, in order of preference:

1. **Consume existing reserved/padding bits** in the boxmon substructs (grep for `unused`, `padding`, `filler` in the substruct definitions). Expansion's "improved Pokémon data structure" left some slack. If the new fields fit in reserved space, **existing saves stay valid.** Prefer this.
2. **Grow the struct** and accept a **save reset.** Fine for active dev, but confirm with Joe first — if friends already have physical carts with progress, that data is lost. (Joe's PC project files are unaffected either way; this is only about in-game save files.)

**Action for Claude Code:** report which path you took and why in your summary. Do not silently break saves.

### 1.3 Wiring the accessors

- Add the enum entries to the `MON_DATA_*` list *(verify location — `include/pokemon.h`)*.
- Add read/write cases in `GetBoxMonData`/`SetBoxMonData` (and the `GetMonData`/`SetMonData` wrappers) in `src/pokemon.c` *(verify)*, following the exact pattern of an existing multi-byte field (Hyper-Training fields or `MON_DATA_HP_EV` are good models).
- Default values on a freshly generated mon: override ability = `ABILITY_NONE`; all stat mods = 0. Make sure `CreateBoxMon`/the generation path zero-inits them (they should if you reuse reserved space that was already zeroed, but verify).

---

## 2. Feature A — The Paws

### 2.1 Design spec

Three single-use bag items. Using one: open party menu → pick a Pokémon → a stat-choice menu appears (ATK / DEF / SP.ATK / SP.DEF / SPEED) → chosen stat rises by N → a **different** random stat falls by N → both reported in text → item consumed.

| Item | ID (suggest) | Magnitude (`.secondaryId`) |
|---|---|---|
| Mankey's Paw | `ITEM_MANKEYS_PAW` | 1 |
| Primeape's Paw | `ITEM_PRIMEAPES_PAW` | 2 |
| Annihilape's Paw | `ITEM_ANNIHILAPES_PAW` | 3 |

**Stat set (v1):** the 5 non-HP stats only. HP is excluded in v1 — permanently altering max HP means rescaling current HP and interacts with the summary/heal paths; leave it out unless Joe asks. (See Open Decision #2.)

**Semantics (v1, recommended):** a **permanent flat modifier** to the displayed stat, stored in `MON_DATA_STAT_MODS`, applied inside `CalculateMonStats`. (Alternative EV-based approach in Open Decision #1.)

**Clamping:**
- Each stat's cumulative modifier is clamped to `[-PAW_MOD_CAP, +PAW_MOD_CAP]` — add `#define PAW_MOD_CAP 12` in a config header so Joe can tune it.
- After applying mods in stat calc, floor every final stat at **1** (never 0 or negative).

**RNG for the decrease:** use `Random()` to pick a stat index in `0..4` that is **not** the chosen stat, then apply `-N`. Simplest v1: apply and clamp (if the rolled stat is already at floor, the curse "whiffs"). Optional polish: reroll among stats not already at `-PAW_MOD_CAP` / not already at floor so the downside always lands.

**Stacking:** using multiple Paws on the same mon accumulates modifiers (subject to the cap). This falls out naturally from the signed array.

### 2.2 Files to touch

| File | Change |
|---|---|
| `include/constants/items.h` | Add the 3 item ID `#define`s (or use expansion's enum item list — *verify how new items are added in current tree*, some versions use an enum) |
| `src/data/items.h` | Add 3 item definitions (see template below) |
| `src/item_use.c` | Add `ItemUseOutOfBattle_Paw` — sets `gItemUseCB` to the party-menu callback and opens the party menu (model on the vitamin path, `ItemUseOutOfBattle_Medicine` *(verify)*) |
| `src/party_menu.c` | Add `ItemUseCB_Paw` — runs the stat-choice menu, applies mods, recalculates, shows text, consumes item |
| `src/pokemon.c` | Hook Paw mods into `CalculateMonStats` (Section 2.4) |
| Config header (e.g. `include/config/...` *(verify)*) | `#define PAW_MOD_CAP 12` |
| Item graphics | Placeholder 24×24 indexed icons + palettes — see Section 5 |

### 2.3 Item definition template (adapt)

Modeled on the confirmed TM item structure:

```c
[ITEM_MANKEYS_PAW] = {
    .name = ITEM_NAME("Mankey Paw"),        // GBA name length is tight — verify char limit
    .price = 0,                              // not buyable; or set a value if it should be
    .description = COMPOUND_STRING(
        "A withered paw. Grants a wish\n"
        "for power, and takes one in\n"
        "return."),
    .pocket = POCKET_ITEMS,
    .type = ITEM_USE_PARTY_MENU,
    .fieldUseFunc = ItemUseOutOfBattle_Paw,
    .secondaryId = 1,                        // magnitude N; Primeape = 2, Annihilape = 3
    // .importance = 0  -> consumed on use (NOT reusable, unlike I_REUSABLE_TMS)
},
```

### 2.4 Stat-calc hook (illustrative — adapt to real code)

Inside `CalculateMonStats`, after each non-HP stat is computed and before it's written back:

```c
// pseudo — read the stored signed mod for this stat, add, floor at 1
s32 mod = GetBoxMonData(boxMon, MON_DATA_STAT_MODS + i, NULL); // however you index it
stat += mod;
if (stat < 1) stat = 1;
```

`CalculateMonStats` already preserves current HP by adjusting for the HP delta; since v1 doesn't touch HP, that logic is untouched. Confirm the stat is recalculated (call `CalculateMonStats`, not a partial path) after a Paw is applied so the change shows immediately.

### 2.5 Apply-on-use logic (illustrative)

In `ItemUseCB_Paw`, after the player picks a stat (`chosenIdx` in `0..4`) and `N` = magnitude from `.secondaryId`:

```c
s8 mods[5];
// read current mods for the selected mon into mods[]
mods[chosenIdx] += N;
CLAMP(mods[chosenIdx], -PAW_MOD_CAP, +PAW_MOD_CAP);

u32 r;
do { r = Random() % 5; } while (r == chosenIdx);   // a DIFFERENT stat
mods[r] -= N;
CLAMP(mods[r], -PAW_MOD_CAP, +PAW_MOD_CAP);

// write mods[] back to MON_DATA_STAT_MODS
CalculateMonStats(mon);
// buffer mon name, both stat names, and N; show the two text lines below
// consume the item
```

### 2.6 Text

Two lines, for the ironic beat. Buffer the mon nickname, the stat names, and N with `StringExpandPlaceholders`:

```
"{MON}'s {STAT_UP} rose by {N}!"
"But its {STAT_DOWN} fell by {N}\p"   // the pause sells the curse
```

Optional flourish that fits the WoT wish motif — a lead-in line when the paw is used:
```
"You make a wish upon the\nwithered paw..."
```

### 2.7 Acceptance criteria

- [ ] Using Mankey's Paw opens the party menu; selecting a mon opens a 5-stat choice menu.
- [ ] Chosen stat rises by 1 (visible in summary); a *different* random stat falls by 1; both reported in text; item consumed.
- [ ] Primeape's Paw = ±2, Annihilape's Paw = ±3.
- [ ] No stat ever drops below 1; cumulative mod per stat respects `PAW_MOD_CAP`.
- [ ] Modifiers persist across save/load, PC deposit/withdraw, and are reflected in **battle** stats (not just the summary display).
- [ ] Stacking multiple Paws accumulates correctly.

---

## 3. Feature B — Ability Machines (AMs)

### 3.1 Design spec

TM-style items ("discs"). Using one: open party menu → pick a Pokémon → its ability is **replaced** with the AM's ability → fanfare + "learned/gained" text. The abilities themselves are already coded in the engine — the AM only assigns them.

**How the override works:** store the chosen ability in `MON_DATA_CUSTOM_ABILITY`. Make `GetMonAbility()` return the override first if set. Because battle reads `gBattleMons[b].ability = GetMonAbility(mon)` at switch-in, the ability takes effect in battle with no further plumbing. In-battle ability-changing effects (Skill Swap, Gastro Acid, Neutralizing Gas, etc.) operate on `gBattleMons` and won't corrupt the stored override — the override is the mon's "true" ability; temporary battle changes are separate.

**Reusable vs single-use:** expansion supports both via `.importance` (e.g. `I_REUSABLE_TMS` makes an item reusable). Pick a default with Joe (Open Decision #4). Doc assumes **reusable** ("discs") unless told otherwise — just note that reusable infinite AMs are a stronger power source than single-use.

**Compatibility:** doc assumes **universal minus a blacklist** — any mon can receive any non-signature ability. Per-species restriction is possible but is a lot of data entry (Open Decision #5).

### 3.2 The blacklist (important)

Signature / form-locked / transform / pure-cosmetic-illusion abilities assume a specific species and will **break or do nothing** on the wrong mon. Do **not** offer AMs for these, and guard against setting them:

`Multitype, RKS System, Stance Change, Battle Bond, Power Construct, Schooling, Shields Down, Disguise, Gulp Missile, Ice Face, Hunger Switch, Zen Mode, Forecast, Flower Gift, Zero to Hero, Commander, As One, Illusion, Imposter, Comatose`

Implement as a `static const u16 sAMBlacklist[]` and reject any AM whose `.secondaryId` is in it (belt-and-suspenders — also just don't create item entries for them). Optionally also exclude pure-downside abilities (Truant, Slow Start, Defeatist, Klutz, Stall) — though a deliberately "cursed" downside AM could be a fun WoT gag given the wish theme; Joe's call.

### 3.3 The ability resolver hook (illustrative)

```c
u16 GetMonAbility(struct Pokemon *mon)   // (verify signature/return type in current tree)
{
    u16 override = GetMonData(mon, MON_DATA_CUSTOM_ABILITY, NULL);
    if (override != ABILITY_NONE)
        return override;
    // ...existing logic: resolve via species abilities + abilityNum (+ hidden)...
}
```

Mirror this in any **box-mon** ability getter used out of battle, and confirm the **summary screen** ability display routes through `GetMonAbility` (`src/pokemon_summary_screen.c` *(verify)*). If the summary uses its own species-lookup path, patch it too, or the override will battle-correctly but *display* the wrong ability on the status screen.

### 3.4 Files to touch

| File | Change |
|---|---|
| `include/constants/items.h` | Add one item ID per AM |
| `src/data/items.h` | Add one item definition per AM (template below) |
| `src/item_use.c` | Add `ItemUseOutOfBattle_AbilityMachine` — model directly on `ItemUseOutOfBattle_TMHM` |
| `src/party_menu.c` | Add `ItemUseCB_AbilityMachine` — reads `.secondaryId`, rejects blacklisted, sets `MON_DATA_CUSTOM_ABILITY`, plays fanfare, shows text, consumes item unless reusable |
| `src/pokemon.c` | The `GetMonAbility` override hook (Section 3.3) |
| `src/pokemon_summary_screen.c` | Ensure ability display honors the override *(verify)* |
| Item graphics | Placeholder 24×24 icons + palettes (Section 5) |

### 3.5 Item definition template (adapt — mirrors the TM pattern)

```c
[ITEM_AM_INTIMIDATE] = {
    .name = ITEM_NAME("AM Intimid"),        // verify GBA name length limit
    .price = 5000,
    .description = COMPOUND_STRING(
        "Teaches a POKMON the\n"
        "Intimidate Ability, replacing\n"
        "its current one."),
    .importance = I_REUSABLE_TMS,            // reusable "disc"; set 0 for single-use
    .pocket = POCKET_TM_HM,                  // or a dedicated AM pocket if Joe wants one
    .type = ITEM_USE_PARTY_MENU,
    .fieldUseFunc = ItemUseOutOfBattle_AbilityMachine,
    .secondaryId = ABILITY_INTIMIDATE,       // the ability to grant
},
```

### 3.6 AM roster to implement (all abilities already exist in-engine)

Joe's four + a curated set. Tier column is a gating suggestion, not balance law.

| AM | Ability | Effect (brief) | Tier |
|---|---|---|---|
| AM Intimidate | `ABILITY_INTIMIDATE` | Lowers foe's Attack 1 stage on entry | core |
| AM Guts | `ABILITY_GUTS` | +50% Attack when statused | core |
| AM Technician | `ABILITY_TECHNICIAN` | +50% power to moves ≤60 BP | core |
| AM Moxie | `ABILITY_MOXIE` | +1 Attack per KO | core |
| AM Adaptability | `ABILITY_ADAPTABILITY` | STAB ×2 instead of ×1.5 | common |
| AM Sheer Force | `ABILITY_SHEER_FORCE` | +30% to moves w/ secondary effects | common |
| AM Tough Claws | `ABILITY_TOUGH_CLAWS` | +30% to contact moves | common |
| AM Iron Fist | `ABILITY_IRON_FIST` | +20% to punching moves | common |
| AM Tinted Lens | `ABILITY_TINTED_LENS` | "Not very effective" hits do full dmg | common |
| AM Skill Link | `ABILITY_SKILL_LINK` | Multi-hit moves always hit 5× | common |
| AM Thick Fat | `ABILITY_THICK_FAT` | Halves Fire/Ice damage | common |
| AM Sturdy | `ABILITY_STURDY` | Survive an OHKO from full HP | common |
| AM Levitate | `ABILITY_LEVITATE` | Ground immunity | common |
| AM Unaware | `ABILITY_UNAWARE` | Ignores foe's stat boosts | uncommon |
| AM Serene Grace | `ABILITY_SERENE_GRACE` | Doubles secondary-effect chance | uncommon |
| AM Prankster | `ABILITY_PRANKSTER` | +1 priority to status moves | uncommon |
| AM Poison Heal | `ABILITY_POISON_HEAL` | Heal instead of taking poison dmg | uncommon |
| AM Multiscale | `ABILITY_MULTISCALE` | Halves damage at full HP | rare |
| AM Magic Guard | `ABILITY_MAGIC_GUARD` | Only direct attacks deal damage | rare |
| AM Regenerator | `ABILITY_REGENERATOR` | Heal 1/3 HP on switch-out | rare |
| AM Speed Boost | `ABILITY_SPEED_BOOST` | +1 Speed each turn | rare |
| AM Magic Bounce | `ABILITY_MAGIC_BOUNCE` | Reflects status moves | rare |

(Optional flavor packs: type-absorb abilities — Water Absorb / Volt Absorb / Flash Fire; weather/terrain setters — Drought / Drizzle / Sand Stream / Snow Warning + the four surges. Easy to add later since they follow the identical pattern.)

### 3.7 Acceptance criteria

- [ ] Using AM Intimidate opens the party menu; selecting a mon sets its ability to Intimidate.
- [ ] The new ability shows correctly on the **summary screen** AND triggers on entry in **battle**.
- [ ] The old ability is fully replaced; using another AM replaces again.
- [ ] Blacklisted abilities can't be applied (guarded even if an item somehow references one).
- [ ] Reusable vs single-use behaves per the chosen `.importance` default.
- [ ] The override persists across save/load and PC deposit/withdraw.
- [ ] In-battle ability swaps (if tested) don't corrupt the stored override after the battle ends.

---

## 4. Suggested build order

1. **Infra (Section 1)** — add fields + accessors + defaults; confirm save-compat path; build clean. Nothing user-visible yet.
2. **AMs first** — lower-risk (abilities already coded). Get `GetMonAbility` override + one AM (Intimidate) working end-to-end in battle and summary. Then bulk-add the roster (pure data entry) + blacklist.
3. **Paws** — stat-calc hook + one Paw (Mankey's) with the stat-choice menu, verify persistence and battle stats, then add Primeape/Annihilape (magnitude tweak only).
4. **Placeholder icons** for all items so they're testable in-bag.
5. **Polish** — optional summary-screen indicators for modified ability/stats; the wish-flavor lead-in text.

Commit per milestone so a regression is easy to bisect.

---

## 5. Item icons (placeholders OK for v1)

Item icons are **24×24 indexed** with the standard item-icon palette format *(verify current dimensions/format in `graphics/items/` and the item graphics table)*. Joe already runs a PIL/numpy asset pipeline, so GBA-spec placeholders are easy — a solid-color paw silhouette and a labeled disc are enough to test. Final art comes from Joe's artist later; keep the item entries pointing at placeholder icon/palette paths that can be swapped without touching code.

---

## 6. Testing checklist

- [ ] Builds clean on WSL (`make`), no warnings on the new files.
- [ ] mGBA: use each item from the bag, verify menus/text/consumption.
- [ ] mGBA: confirm stat changes and ability changes show in **summary** and behave in **battle**.
- [ ] Save, soft-reset, reload — confirm both features persist.
- [ ] PC deposit/withdraw — confirm both features survive the boxmon round-trip.
- [ ] Hardware pass on the EZ-Flash Omega DE + real cart once mGBA is green.

---

## 7. Open decisions for Joe (surface these, don't guess)

1. **Paw semantics** — permanent flat stat modifier *(doc's default, recommended)* vs EV-based (no new save data, but "+1" becomes fuzzy and won't be exactly +1 at low levels). If EV-based is chosen, skip the `MON_DATA_STAT_MODS` field and instead add/subtract EVs — but confirm the magnitudes (a literal +1 EV is meaningless; you'd want a tier like ±10/±20/±30 EVs).
2. **HP as a Paw target?** Doc excludes HP in v1. Including it requires rescaling current HP on apply.
3. **`PAW_MOD_CAP` value** — default 12. Higher = spicier, lower = safer.
4. **AMs reusable or single-use?** Doc default = reusable ("discs"). Reusable = a much stronger, repeatable power source.
5. **AM compatibility** — universal-minus-blacklist *(doc default)* vs per-species/per-ability restriction (more work, more control).
6. **Save reset acceptable?** Only relevant if the new fields can't fit in reserved boxmon space (Section 1.2). Confirm no friends are mid-playthrough on distributed carts before breaking saves.
7. **Summary-screen indicators?** Optional: color/flag a Paw-modified stat or an AM-overridden ability so players can tell at a glance.

---

## 8. Lore / naming notes (WoT)

- The Paws lean directly into the game's wish motif — a "be careful what you wish for" echo of Jirachi's Undying Wish. Purely mechanical, **not a canon change**, but the flavor lightly touches the central theme, so a courtesy heads-up to the co-director is polite (not required for approval).
- Keep item descriptions **GBA-lean** per house style — terse and ominous for the Paws, terse and functional for AMs (mirror TM phrasing).
- If Joe wants, "Ability Machine / AM" naming can mirror the TM/HM convention exactly (numbered AMs, or named as in the roster table). Named reads better than numbered for a friends-facing hack, but numbered is more TM-authentic.
