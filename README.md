# Wishes of Tomorrow — Development Workspace

This branch is the **working repository** behind [Wishes of Tomorrow](https://github.com/jmaloney95/Emerald-Rom-Hack/tree/master): source art, raw tilesets, ported music, design documents, and the build tooling used to turn all of it into the game.

> **Looking for the game?** The playable ROM hack and all of its source code live on the **[`master`](https://github.com/jmaloney95/Emerald-Rom-Hack/tree/master)** branch. This branch (`main`) is deliberately separate history — it is the workshop, not the product.

Everything here is published so other developers can take what is useful: the assets, the tileset pipeline, the design documents, or just a look at how a full-length ROM hack got put together. **Please read [Reusing this work](#reusing-this-work) first** — much of the art belongs to other artists and carries their terms, not mine.

## Layout

| Path | What's in it |
|---|---|
| [`custom sprites/`](custom%20sprites) | Source sprite art — trainers, Shadow Pokémon battle sheets, overworld sprites (139 files) |
| [`custom ui/`](custom%20ui) | UI mockups, HUD concepts, battle interface iterations (4,729 files) |
| [`custom music/`](custom%20music) | Sequenced tracks and samples ported from later-generation Pokémon titles (168 files) |
| [`tilesets_raw/`](tilesets_raw) | Raw tileset art plus [`build_scripts/`](tilesets_raw/build_scripts) — the Python pipeline that compiles it into GBA-legal primary/secondary tileset pairs (373 files) |
| [`region map/`](region%20map) | Region map art and layout data |
| [`previews/`](previews) | Rendered previews and concept images produced along the way |
| [`tools/`](tools) | Asset staging scripts (`build_assets.py`, `stage_nds_music.py`, `update_music.py`) |
| [`Claude Handoff/`](Claude%20Handoff) | Session handoff notes from AI-assisted development |
| `sprites_out/`, `act1_scripts/` | Pipeline output and early script drafts |

Submodules (`pokeemerald-expansion`, `agbcc`, `pokefirered`, `pokemonHnS`, `Team-Aquas-Asset-Repo`) are reference checkouts. Clone them with:

```bash
git submodule update --init --recursive
```

**Not in version control:** `Tools/` and `mGBA-0.10.5-win32/` — roughly 700 MB of third-party binaries. Every one of them, with its download source, is listed in [`Toolchain_and_Setup_Guide.md`](Toolchain_and_Setup_Guide.md). Nothing in the project references them by path, so a fresh clone builds without them.

## Design documents

**Heads up: these contain complete story spoilers, including the ending and post-game.**

| Document | Contents |
|---|---|
| [`Pokemon_Wishes_of_Tomorrow_GDD.docx`](Pokemon_Wishes_of_Tomorrow_GDD.docx) | The game design document |
| [`WoT_Act3_Canon.md`](WoT_Act3_Canon.md) | Act 3 story canon and set-piece specifications |
| [`WoT_Act3_Status_Handoff.md`](WoT_Act3_Status_Handoff.md) | Running development log — every feature, bug, and engine gotcha, newest first. The most useful document here if you want to know *how* something was built |
| [`Quest_Plan_Handoff.md`](Quest_Plan_Handoff.md) | Quest system design and the full quest list |
| [`Custom_Pokemon_List.md`](Custom_Pokemon_List.md) | Custom and modified species |
| [`Wishes_of_Tomorrow_Pokemon_by_Area.xlsx`](Wishes_of_Tomorrow_Pokemon_by_Area.xlsx) | Every encounter in the game by area, plus Shadow species with art but no encounter |
| [`Region_Map_Data.md`](Region_Map_Data.md) | Region map sections and layout |
| [`Toolchain_and_Setup_Guide.md`](Toolchain_and_Setup_Guide.md) · [`Build_Environment_Setup.md`](Build_Environment_Setup.md) | Toolchain, build environment, and where to get every external tool |
| [`NDS_Music_Registration_Recipe.md`](NDS_Music_Registration_Recipe.md) · [`NDS_Music_Stage_Checklist.md`](NDS_Music_Stage_Checklist.md) | The NDS-to-GBA music porting process, start to finish |
| [`boss_intro_card_handoff.md`](boss_intro_card_handoff.md) · [`wishes_of_tomorrow_ui_features_handoff.md`](wishes_of_tomorrow_ui_features_handoff.md) · [`HANDOFF_paws_and_ability_machines.md`](HANDOFF_paws_and_ability_machines.md) | Individual feature specifications |
| [`Pompeii_Market_Plan.md`](Pompeii_Market_Plan.md) | Area design notes |

## Reusing this work

Take what is useful — that is why this is public. But the contents have **different owners and different terms**, so please check before you reuse:

- **Original code, scripts, tooling, and design documents** (Joe Maloney / Flatfoot Games) — free to use and adapt. A credit is appreciated, not required.
- **Sprite, tileset, portrait, and UI art by other artists** — these belong to the artists credited in the [game's README](https://github.com/jmaloney95/Emerald-Rom-Hack/blob/master/README.md#credits), **not to this project**. Credit the original artist, and follow whatever terms they set for their own work. Do not treat their presence here as permission.
- **Emeiry / *Odisea* by ekat99** (the Shin-Tokyo tilesets) is licensed [CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/) — attribution, non-commercial, share-alike. Those obligations carry over to anything you build with it.
- **Engine code** originates from [RHH's `pokeemerald-expansion`](https://github.com/rh-hideout/pokeemerald-expansion) and [pret's `pokeemerald`](https://github.com/pret/pokeemerald); their terms apply to it.
- **Music** is arranged from official Pokémon soundtracks and is **not** cleared for commercial use.

*Pokémon* is a trademark of Nintendo / Creatures Inc. / GAME FREAK. This is a non-commercial fan project, unaffiliated with and unendorsed by them.
