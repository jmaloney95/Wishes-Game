# Wishes of Tomorrow — Region Map Data (extracted from map files)

Source: `pokeemerald-expansion/data/maps/*/map.json` + `data/layouts/layouts.json`
Interior maps (houses, Pokémon Center, Mart, Gym, Shinkansen interior, Hot Spring) excluded.
Coordinates are tile units, origin top-left of each map, x→east, y→south.

---

## 1. Per-map data

### Munen_village_2 — MAPSEC_MUNEN_VILLAGE — w:49 h:29
| Exit | Type | Tiles (this map) | Destination | Landing tiles |
|---|---|---|---|---|
| North | **Connection** (seamless), offset 16 | exit triggers x:22–27 y:1 | MeltingMile | enters MM bottom edge (MM x = Munen x − 16) |

Single exit — confirmed (matters for Act 2 canon §6.7).

### MeltingMile (Route 1) — MAPSEC_ROUTE_ONE — w:30 h:25
| Exit | Type | Tiles | Destination | Landing tiles |
|---|---|---|---|---|
| South | Connection, offset −16 | bottom edge | Munen_village_2 | Munen top, door zone x:22–27 |
| East | Warp | x:29 y:6–7 | FrostwoodTown | x:0 y:26–27 (west edge) |
| East | Warp | x:29 y:16–17 | MunenLake | x:0 y:4–5 (west edge) |

### MunenLake — MAPSEC_LAKE_MUNEN — w:26 h:22
| Exit | Type | Tiles | Destination | Landing tiles |
|---|---|---|---|---|
| West | Warp | x:0 y:4–5 | MeltingMile | x:29 y:16–17 |
| North | Warp | x:14–15 y:0 | FrostwoodTown | x:24–25 y:29 (south edge) |

### FrostwoodTown — MAPSEC_FROSTWOOD_TOWN — w:50 h:30
| Exit | Type | Tiles | Destination | Landing tiles |
|---|---|---|---|---|
| West | Warp | x:0 y:26–27 | MeltingMile | x:29 y:6–7 |
| South | Warp | x:24–25 y:29 | MunenLake | x:14–15 y:0 |
| North | Warp | x:18–19 y:0 | Route_2 | x:13–14 y:45 |
(Interior warps: Pokémon Center x:10 y:25, Mart x:16 y:25, Gym x:39 y:15, House x:9 y:12 — excluded.)

### Route_2 — MAPSEC_ROUTE_2 — w:24 h:49
| Exit | Type | Tiles | Destination | Landing tiles |
|---|---|---|---|---|
| South | Warp | x:13–15 y:45 | FrostwoodTown | x:18–19 y:0 |
| North | Warp | x:5–7 y:0 | SennenVillage | x:5–7 y:19 |

### SennenVillage — w:35 h:20 (SennenVillage_2 identical: 35×20)
| Exit | Type | Tiles | Destination | Landing tiles |
|---|---|---|---|---|
| South | Warp | x:5–7 y:19 | Route_2 | x:5–7 y:0 |
| North | Warp | x:14–15 y:0 | Torii | x:31 y:35–36 |
| Station | Warp | x:32 y:6 | ShinkansenInterior (interior, excluded) | — |

Note: Torii's return warps target **SennenVillage_2** (the post-event copy) at the same x:14–15 y:0 — geometry unchanged. `region_map_section` is still placeholder `MAPSEC_ABANDONED_SHIP`.

### Torii (Route 3, the Torii Route) — MAPSEC_ROUTE_3 — w:40 h:50
| Exit | Type | Tiles | Destination | Landing tiles |
|---|---|---|---|---|
| Southeast | Warp | x:31 y:35–36 | SennenVillage_2 | x:14–15 y:0 |
| North | Warp | x:12–14 y:3 | StarSummit | x:15 y:23 |
| Internal | Warp pair | x:8 y:16 ↔ x:25 y:28 | Torii (shortcut within route) | — |

### StarSummit — w:30 h:29 (placeholder MAPSEC_MT_PYRE)
| Exit | Type | Tiles | Destination | Landing tiles |
|---|---|---|---|---|
| South | Warp | x:15–16 y:23 | Torii | x:12 y:3 |
| Script | `warp MAP_HOT_SPRING, 15, 13` (Act 1 climax → dream) | — | HotSpring (interior) | — |

### Ashlands — w:32 h:31 (placeholder MAPSEC_TREASURE_BEACH)
| Exit | Type | Tiles | Destination | Landing tiles |
|---|---|---|---|---|
| Warp | Warp | x:27 y:12 | HotSpring (interior) | x:15 y:31 |

No overworld connection — reached only via the Star Summit → Hot Spring dream sequence. Placed on the region map per canon §0: **west slope of the mountain**, descending toward Tradewind Town.

---

## 2. Stitched world coordinates (to scale)

Anchor: Munen Village top-left = (0,0). x→east, y→south (negative y = north).
Derived from the seamless connection (exact) and by aligning warp door tiles across shared edges (exact where doors are edge-to-edge).

| Map | Global X | Global Y | Size | Basis |
|---|---|---|---|---|
| Munen Village | 0 … 48 | 0 … 28 | 49×29 | anchor |
| Melting Mile | 16 … 45 | −25 … −1 | 30×25 | connection offset 16 (exact) |
| Frostwood Town | 46 … 95 | −45 … −16 | 50×30 | MM(29,6/7)↔FW(0,26/27) row-exact |
| Munen Lake | 49 … 74 † | −13 … 8 | 26×22 | MM(29,16/17)↔Lake(0,4/5) row-exact |
| Route 2 | 51 … 74 | −94 … −46 | 24×49 | door x aligned (R2 x13↔FW x18), edge-abutted |
| Sennen Village | 51 … 85 | −114 … −95 | 35×20 | door x aligned (Sen x5↔R2 x5), edge-abutted |
| Torii Route | 34 … 73 | −164 … −115 | 40×50 | door x aligned (Torii x31↔Sen x14/15), edge-abutted |
| Star Summit | 31 … 60 | −193 … −165 | 30×29 | door x aligned (Sum x15↔Torii x12), edge-abutted |
| Ashlands | −6 … 25 | −170 … −140 | 32×31 | thematic (canon: west slope, no overworld link) |

† Exact warp alignment puts the Lake at x:46, which clips Munen Village's NE corner by 3 columns (the maps are never loaded together, so it's harmless in game). Nudged +3 east on the visual map.

Region footprint (built maps): **~102 × 222 tiles** — a tall N–S spine, Munen at the foot, Star Summit at the crown. Matches the canon thaw gradient south→north.

---

## 3. Data issues found while cross-checking warps

1. **MunenLake (15,0) → FrostwoodTown `dest_warp_id: 10`** — Frostwood only has warps 0–9. Intended target is warp **7** (x:25 y:29).
2. **MunenLake (0,5) → MeltingMile `dest_warp_id: 4`** — Melting Mile only has warps 0–3. Intended target is warp **3** (x:29 y:17).

Both out-of-range ids will misplace the player on warp. Worth fixing in `MunenLake/map.json`.

3. Placeholder `region_map_section` values still in use: SennenVillage → MAPSEC_ABANDONED_SHIP, StarSummit → MAPSEC_MT_PYRE, Ashlands → MAPSEC_TREASURE_BEACH, Torii → MAPSEC_ROUTE_3, Route_2 → MAPSEC_ROUTE_2. (Layout grid already defines MAPSEC_STAR_SUMMIT, MAPSEC_SENNEN_LINE, MAPSEC_MT_FUJI, MAPSEC_SHIN_TOKYO.)

---

## 4. Suggested in-game town-map grid (28×15, to scale at 16 tiles/square)

Scale: 1 map square = 16×16 tiles. The Act 1 spine is centered (cols 10–16); the Sennen Line rail row carries Tradewind west and Shin-Tokyo east, matching the story canon and your concept art composition.

```
col→  10   11   12   13   14   15        (rows ↓)
r0              [STAR SUMMIT ]
r1              [STAR SUMMIT ]
r2   [ASHLANDS][TORII ROUTE  ]
r3   [ASHLANDS][TORII ROUTE  ]
r4             [TORII ROUTE  ]
r5   ══rail══════[ SENNEN ]══════rail══   (Tradewind c2–3 ← → Shin-Tokyo c24–25)
r6                [ROUTE 2]
r7                [ROUTE 2]
r8                [ROUTE 2]
r9                [FROSTWOOD     ]
r10       [MELT.MILE][FROSTWOOD  ]
r11       [MELT.MILE][LAKE MUNEN ]
r12  [MUNEN VILLAGE     ]
r13  [MUNEN VILLAGE     ]
```

`region_map_sections.json` values (x, y, width, height):

| MAPSEC | x | y | w | h |
|---|---|---|---|---|
| MAPSEC_STAR_SUMMIT | 12 | 0 | 2 | 2 |
| MAPSEC_ROUTE_3 (Torii) | 12 | 2 | 2 | 3 |
| MAPSEC_ASHLANDS | 10 | 2 | 2 | 2 |
| MAPSEC_SENNEN (new) | 13 | 5 | 2 | 1 |
| MAPSEC_ROUTE_2 | 13 | 6 | 1 | 3 |
| MAPSEC_FROSTWOOD_TOWN | 13 | 9 | 3 | 2 |
| MAPSEC_ROUTE_ONE (Melting Mile) | 11 | 10 | 2 | 2 |
| MAPSEC_LAKE_MUNEN | 13 | 11 | 2 | 1 |
| MAPSEC_MUNEN_VILLAGE | 10 | 12 | 3 | 2 |
| MAPSEC_TRADEWIND_TOWN (Act 2) | 2 | 5 | 2 | 1 |
| MAPSEC_SHIN_TOKYO (Act 2) | 24 | 5 | 2 | 1 |
| MAPSEC_SENNEN_LINE (rail) | 4 | 5 | 20 | 1 |

Adjacencies preserved: Munen↑Melting Mile, MM→Frostwood (east), Frostwood↓Lake & ↑Route 2, Route 2↑Sennen, Sennen↑Torii, Torii↑Star Summit, Ashlands west slope. Cursor walk order on the Pokénav matches actual travel order.

---

## 5. INSTALLED: Gemini art region map (graphics/pokenav/region_map/)

The AI-generated art was quantized and installed as `map.png` (tileset, 198/233 tiles), `map.bin` (64×64 affine tilemap), `map.pal` (48 colors, banks 7–9). Vanilla files backed up in `graphics/pokenav/region_map/vanilla_backup/`. The §4 grid above applies to the schematic PNG; the table below matches the **installed art** (read from `region_map_gemini_grid_4x.png`):

**APPLIED 2026-06-09** to `region_map_sections.json` + `region_map_layout.h` (final coordinates per Joe, all 1×1 cells):

| MAPSEC | Name | x | y |
|---|---|---|---|
| MAPSEC_MUNEN_VILLAGE | MUNEN VILLAGE | 13 | 14 |
| MAPSEC_ROUTE_ONE | **MELTING MILE** (renamed) | 17 | 12 |
| MAPSEC_LAKE_MUNEN | MUNEN LAKE | 19 | 11 |
| MAPSEC_FROSTWOOD_TOWN | FROSTWOOD TOWN | 17 | 9 |
| MAPSEC_ROUTE_2 | ROUTE 2 | 16 | 8 |
| MAPSEC_SENNEN_LINE | **SENNEN STATION** (renamed) | 15 | 7 |
| MAPSEC_ROUTE_3 | **TORII ROUTE** (renamed) | 12 | 4 |
| MAPSEC_MT_FUJI | **MOUNT MUNEN** (renamed) | 13 | 1 |
| MAPSEC_STAR_SUMMIT | STAR SUMMIT | 14 | 0 |
| MAPSEC_ONSEN_SPRINGS | ONSEN SPRINGS (new) | 10 | 0 |
| MAPSEC_ASHLANDS | ASHLANDS (new) | 9 | 1 |
| MAPSEC_TRADEWIND_TOWN | TRADEWIND TOWN | 9 | 8 |
| MAPSEC_SHIN_TOKYO | SHIN-TOKYO | 19 | 4 |

Also fixed placeholder `region_map_section` in map.json: StarSummit MT_PYRE→STAR_SUMMIT, Ashlands TREASURE_BEACH→ASHLANDS, SennenVillage(+_2) ABANDONED_SHIP→SENNEN_LINE. Note: MAPSEC_ROUTE_2/ROUTE_3 are the vanilla Kanto ids reused by your maps — their renames/coords affect any Kanto map screen too (not used by this hack). Verification overlay: `region_map_poi_check_4x.png`.
