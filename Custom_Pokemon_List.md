# Cross-Gen Pokémon — Placement Plan
### Pokémon Wishes of Tomorrow

**Decision (2026-05-25):** base is now **pokeemerald-expansion**. Every species below is
**already in the ROM, with finished sprites, stats, learnsets, abilities, and modern
evolution methods.** So this is no longer a "build these" list — it's a "**place these**" plan:
where each one shows up in the game.

**Locked working set:** Core + Recommended + Relic = **19 species** (with potential to change later).

> **What changed vs. the old FireRed plan:**
> - No manual species insertion. No sprite sourcing. No adding evolution items.
> - Evolutions (incl. Dusk Stone / Dawn Stone / trade lines) work out of the box.
> - We just decide encounters, the starter trio, and any special/static catches.
> - Optional later: tweak a species' stats/learnset if we want it different from standard —
>   but the expansion's defaults are balanced and fine to ship.

**Status key:** ☐ not placed yet · ☑ placed in game

---

## A. Starters — the fossil trio (FIRST TO PLACE)
The shrine ice-block selection event hands out one of these at Lv 5.

| Line | Type | Role | Evo (works OOTB) | Status |
|---|---|---|---|---|
| **Tyrunt → Tyrantrum** | Rock/Dragon | Jaw Fossil — proud/regal | Lv 39 (day) | ☐ |
| **Amaura → Aurorus** | Rock/Ice | Sail Fossil — gentle/melancholic | Lv 39 (night) | ☐ |
| **Anorith → Armaldo** | Rock/Bug | Claw Fossil — scrappy | Lv 40 | ☐ |

---

## B. Lantern ghosts — the wisps made real (CORE)
| Line | Type | Where it's placed | Evo | Status |
|---|---|---|---|---|
| **Litwick → Lampent → Chandelure** | Ghost/Fire | Shrine grounds, Route 1 wisps, spirit areas | Lv 41, Dusk Stone | ☐ |

## C. The bound spirit — mirror of the seal (CORE)
| Line | Type | Where it's placed | Evo | Status |
|---|---|---|---|---|
| **Spiritomb** | Ghost/Dark | Special/static catch tied to the seal or Team Wish lore | — | ☐ |

## D. Snow spirit — the yuki-onna (CORE)
| Line | Type | Where it's placed | Evo | Status |
|---|---|---|---|---|
| **Snorunt → Froslass** | Ice → Ice/Ghost | High mountain / late Act-1 cold areas | Dawn Stone (♀) | ☐ |

## E. Forest tree spirits (CORE)
| Line | Type | Where it's placed | Evo | Status |
|---|---|---|---|---|
| **Phantump → Trevenant** | Ghost/Grass | Route 1 Ivy/Haunted Woods | Trade | ☐ |

## F. Misdreavus' evolution (CORE)
| Line | Type | Where it's placed | Evo | Status |
|---|---|---|---|---|
| **Misdreavus → Mismagius** | Ghost | Spirit-charged shrine areas | Dusk Stone | ☐ |

---

## G. Bone Fields fossils (RECOMMENDED — Route 2)
| Line | Type | Where it's placed | Evo | Status |
|---|---|---|---|---|
| **Tirtouga → Carracosta** | Water/Rock | Route 2 Bone Fields | Lv 37 | ☐ |
| **Archen → Archeops** | Rock/Flying | Route 2 Bone Fields (rare) | Lv 37 | ☐ |

## H. The haunted relic (RELIC LINE — in the locked set)
| Line | Type | Where it's placed | Evo | Status |
|---|---|---|---|---|
| **Honedge → Doublade → Aegislash** | Steel/Ghost | Ancient shrine blade / Archive City relic / late game | Lv 35, Dusk Stone | ☐ |

---

## Stretch options (not in the locked set — easy to add anytime)
All already in the ROM; say the word to slot any in:
Sinistea/Polteageist, Sandygast/Palossand, Yamask/Cofagrigus, Mimikyu, Drifloon/Drifblim,
Duskull/Dusknoir.

---

## Locked set (19 species)
Tyrunt, Tyrantrum, Amaura, Aurorus, Anorith, Armaldo, Litwick, Lampent, Chandelure, Spiritomb,
Froslass (+ Snorunt), Phantump, Trevenant, Mismagius (+ Misdreavus), Tirtouga, Carracosta,
Archen, Archeops, Honedge, Doublade, Aegislash.
*(Snorunt, Anorith, Misdreavus already appear in the GDD's base encounter lists.)*

---

## Next step
First placement target is **Group A** — wire the three starter lines into the shrine ice-block
selection event. No sprite hunt, no data entry: I point the event at `SPECIES_TYRUNT` /
`SPECIES_AMAURA` / `SPECIES_ANORITH` and they just work.
