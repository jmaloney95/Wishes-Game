# Pompeii Night Market — Spirited Away Direction

Goal: evolve Pompeii (the gate town below Mt. Munen) into a veil-market like the
Spirited Away spirit town — warm red lanterns, food stalls, noren curtains, and
"other customers" who only appear once the lanterns are lit. This doc gets us
from today's map to the custom-tileset version in safe, testable phases.

## Where things stand (2026-07-02)

- Town renders in-game as **POMPEII** (MAPSEC_POMPEII); tunnel announces
  **MUNEN TUNNEL**. Fly lands at the Pilgrim's Rest once visited.
- Market **scripts are already written** at the bottom of
  `pokeemerald-expansion/data/maps/NewMap/scripts.inc` — lantern lighter, food
  stall (treats mart), trinket stall (wards mart), kimono dancer, masked patron
  (reacts to the Carved Mask), and a market sign. They just need objects
  attached in Porymap.
- The lore already points here: the tunnel inscription, Gen's "two kinds of
  festival," the Tradewind ghost-market precedent.

## Phase 1 — Market with stock assets (no new tiles)

1. In Porymap, place along the riverfront / plaza:
   | Object gfx | Script label |
   |---|---|
   | OBJ_EVENT_GFX_OLD_WOMAN (facing a lamp) | NewMap_EventScript_LanternLighter |
   | OBJ_EVENT_GFX_COOK or POKEFAN_M | NewMap_EventScript_FoodStall |
   | OBJ_EVENT_GFX_WOMAN_5 or GENTLEMAN | NewMap_EventScript_TrinketStall |
   | OBJ_EVENT_GFX_KIMONO_GIRL | NewMap_EventScript_KimonoDancer |
   | OBJ_EVENT_GFX_MAGMA_MEMBER_M | NewMap_EventScript_MaskedPatron |
   | bg sign | NewMap_EventScript_MarketSign |
2. Fake the stalls with existing tiles for now: benches + signs + NPC behind
   them reads as a stall at GBA scale.
3. Palette caution: Poochyena/Skitty/Wingull already eat 3 OW palette slots on
   this map. Add human NPCs (shared palettes) and check nothing turns
   invisible (Ashlands lesson).
4. Mood pass to test in-game: try `MUS_HG_ECRUTEAK` or `MUS_BW_ANVILLE` as town
   music, and `WEATHER_SHADE` for dusk. Both are one-line map.json changes —
   evaluate, keep what feels right.

## Phase 2 — Custom tileset spec (the real look)

Build as a **secondary tileset** (`gTileset_PompeiiMarket`) paired with the
current primary, replacing gTileset_AshlandTrees-style usage on LAYOUT_NEW_MAP.

Tile shopping list (roughly 120–180 tiles):
- Red paper lanterns: post-mounted, string-hung (2 frames if animated later)
- Noren curtains (half-height doorway drapes, 2–3 colors, wind variant)
- Stall kit: wooden counter, awning roofs (red/gold), hanging menu boards
- Food props: steam baskets, skewer racks, sake barrels, stacked bowls
- Bathhouse-style facade pieces for one landmark building (balcony rails,
  ornate ridge caps, big round window)
- Red arched bridge tiles (the river is already center-map — perfect)
- Paper talismans/ofuda to paste near the tunnel door
- Stone lanterns (tōrō) for the shrine path up to the torii exit

Hard rules learned on this project (do not skip):
- tiles.png must be **4bpp indexed** or Porymap shows noise
- a primary tileset folder must have **exactly 6 .pal files**
- **tile 0 must be fully blank** or every metatile renders as filler
- new palettes go in secondary slots (10/11 worked for Mt Moon Village)
- Porymap caches tileset images — **reload the project** after edits

## Phase 3 — Repaint + day/night identity

- Repaint the market blocks in Porymap with the new metatiles; keep the
  existing footprint (buildings/river/bridges stay where they are).
- Two-mood option: default map = ordinary sleepy town; after a story flag
  (lanterns lit), swap to the market look. Cheapest implementations:
  a) palette swap of the secondary tileset (day/dusk pals), or
  b) duplicate layout + `setmaplayoutindex` (vanilla-supported) — full visual
  swap, one line of script, no new maps.
  Option (b) is the Spirited Away moment: cross the bridge at the wrong hour
  and the town has changed.
- The masked patron, lantern lighter, and market sign are already written to
  support "two kinds of guests" — more veil-guest NPCs can appear via
  flag-gated objects once the market look lands.

## Phase 4 — Later hooks

- Act 2 disguise is live: any oni goon script can start with the shared
  Carved Mask check (`data/scripts/oni_mask_disguise.inc` documents the
  pattern + shared fooled/nod texts).
- Gym pass idea from Tradewind ("veil market" precedent) could migrate here
  if Pompeii ever gets a gym or trial.
- Candidate landmark: the inn (double-door, top-right) is the natural
  "bathhouse" — its facade gets the Phase 2 ornate pieces.
