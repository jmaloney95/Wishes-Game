# Extracted Asset Credits — Wishes of Tomorrow

Running attribution log for third-party assets brought into this project. Keep this
accurate so credit is correct if the project's distribution status ever changes.

## Overworld NPC sprites — from Pokémon Heart & Soul (HnS)
Source repo: PokemonHnS-Development/pokemonHnS (Modern Emerald base). Art is
base-agnostic and was re-registered against pokeemerald-expansion's tables.

| Asset | Our constant | Source file (HnS) | Artist |
|-------|--------------|-------------------|--------|
| Kimono Girl OW | `OBJ_EVENT_GFX_KIMONO_GIRL` | `pics/people/special/kimono.png` + `palettes/kimono.pal` | HnS team — specific artist TBD (Cesare_CBass / AveonTrainer / PurpleZaffre / BatimaTheBat) |
| Eusine OW | `OBJ_EVENT_GFX_EUSINE` | `pics/people/special/eusine.png` + `palettes/eusine.pal` | HnS team — specific artist TBD |
| Sage OW | `OBJ_EVENT_GFX_SAGE` | `pics/people/sage.png` + `palettes/sage.pal` | HnS team — specific artist TBD |

> ACTION (Joe): pin the exact artist per sprite from HnS's own credits before any release.

## Town tile ART — from Pokémon Heart & Soul (HnS)
Brought over **art-only** (tiles.png + palettes); metatiles rebuilt fresh in Porymap
against our 512-tile / 6-primary-palette base (HnS uses 640/7, so its metatiles.bin
could not be reused directly).

| Tileset | Our struct | Source (HnS) | Artists |
|---------|-----------|--------------|---------|
| Ecruteak City (pagodas / Tin Tower) | `gTileset_EcruteakCity` | `secondary/ecruteak_city/` | Crystal Advance (Kertra), Ekat99, TheDeadHeroAlistair, Johto Redrawn Team |
| Violet City (Sprout Tower / tile-roof town) | `gTileset_VioletCity` | `secondary/violet_city/` | (same Johto tileset teams) |

## Pending / planned extractions (not yet pulled)
- More Johto tilesets if wanted (azalea, cherrygrove, burned_tower, mt_silver_snow, cave_ice, johto_general_fall_spring…) — same credit teams.
- Surfing Pokémon OW sprites — credit: slawter666, wally-217.
