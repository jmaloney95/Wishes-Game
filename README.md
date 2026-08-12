# Wishes of Tomorrow

<!-- PLACEHOLDER FOOTAGE: these three gifs are upstream pokeemerald-expansion
     engine captures, kept only until Wishes of Tomorrow gameplay gifs are
     recorded. Replace the three image URLs below and delete this comment. -->

![Gif that shows debugging functionality that is unique to pokeemerald-expansion such as rerolling Trainer ID, Cheat Start, PC from Debug Menu, Debug PC Fill, Pokémon Sprite Visualizer, Debug Warp to Map, and Battle Debug Menu](https://github.com/user-attachments/assets/cf9dfbee-4c6b-4bca-8e0a-07f116ef891c) ![Gif that shows overworld functionality that is unique to pokeemerald-expansion such as indoor running, BW2 style map popups, overworld followers, DNA Splicers, Gen 1 style fishing, OW Item descriptions, Quick Run from Battle, Use Last Ball, Wild Double Battles, and Catch from EXP](https://github.com/user-attachments/assets/383af243-0904-4d41-bced-721492fbc48e) ![Gif that shows off a number of modern Pokémon battle mechanics happening in the pokeemerald-expansion engine: 2 vs 1 battles, modern Pokémon, items, moves, abilities, fully customizable opponents and partners, Trainer Slides, and generational gimmicks](https://github.com/user-attachments/assets/50c576bc-415e-4d66-a38f-ad712f3316be)

# About

**Wishes of Tomorrow** is a full-length Pokémon ROM hack for the Game Boy Advance — an original region, cast, and three-act campaign built at source level on RHH's [`pokeemerald-expansion`](https://github.com/rh-hideout/pokeemerald-expansion) 1.15.3, itself a C and ARM decompilation of *Pokémon Emerald*. Nothing here is patched into a prebuilt binary: every map, system, and asset is compiled from source, so the engine is part of the deliverable rather than a black box the game sits on top of.

**Status — content complete and play-tested.** All three acts run end to end, followed by a scripted finale, an original credits sequence, Hall-of-Fame save integration, and a post-game chapter. The full campaign and post-game have been played through and verified.

## Engineering summary

| | |
|---|---|
| **Languages** | C (ARM7TDMI / GBA), ARMv4T assembly, GBA event-script assembly, Python 3, JSON, GNU Make |
| **Toolchain** | `arm-none-eabi-gcc` 14.2.1 and `agbcc` under WSL2; parallel Make builds; the `gbagfx` / `mapjson` / `jsonproc` / `aif2pcm` asset pipeline; mGBA 0.10.5 for instrumented testing |
| **Content tools** | Porymap (maps and event authoring), Porytiles, Poryscript, LibreSprite, VGMTrans (NDS sequence extraction) |
| **AI-assisted development** | Claude Code as the primary implementation surface — engine work, cutscene scripting, asset pipelines, and build verification — with Claude Design and Claude Cowork for planning and iteration |

**Scale, measured as a diff against the upstream base:** roughly **82,000 inserted lines across 3,434 files**. That breaks down to ~9,000 lines of C and ~4,900 lines of headers implementing engine and gameplay systems, ~33,500 lines of event-script assembly driving cutscenes, dialogue, and quest logic, ~15,800 lines of JSON map and encounter data, and ~2,400 lines of Python tooling. The result builds to a 32 MB cartridge image.

**Custom engine systems** written for this project include a Shadow Pokémon capture-and-purification framework — snag mechanics layered onto the ball-throw path, per-species persistent logging with idempotent save-forward backfill across party and PC boxes, battle-side stat modifiers, and summary-screen status indicators — plus a quest journal with deferred toast notifications, a visual-novel character portrait system bound to a speaker-name table, an HM field-move menu that removes the need for dedicated HM carriers, animated boss intro cards, and a set of scripted-cutscene primitives (detached camera objects, audio-suppressed screen shake, weather and SE muting, pooled particle effects).

**Custom tooling: 30 purpose-built Python utilities.** The largest is a tileset compiler that converts raw RMXP-format art into GBA-legal primary/secondary tileset pairs within the hardware's 512-tile, 7-palette budget. Alongside it: a sprite-import pipeline that colour-merges, quantizes, ground-line-aligns, and indexes 64×64 battle art against vanilla frame geometry, and an NDS-to-GBA music staging pipeline used to import 25 sequenced tracks and 461 direct-sound samples.

**Content shipped:** 64 new maps, 34 new tilesets, 277 new graphics assets, 12 tracked quests, and original trainer rosters and wild-encounter tables spanning the region.

# Features

- **An original region and a three-act story campaign**, from a quiet mountain village through a surveillance city to a finale atop a tower — with an archipelago of post-game islands to surf and fish between.
- **The Shadow Pokémon system.** Corrupted Pokémon fight harder and defend worse, and can slip into Hyper Mode mid-battle. Steal them from enemy trainers with the Snag Machine, then purify them to keep the offensive edge without the drawbacks.
- **Snag Balls** — a custom ball with a heavy multiplier against Shadow Pokémon, sold on the black market.
- **The Shadow Log**, a dedicated field menu tracking every Shadow Pokémon in the game as loose, snagged, or purified, with completion counters for post-game collection.
- **Quest journal** with a start-menu interface, main and side quest tracking, and on-screen notifications as objectives advance.
- **Character portraits** during story dialogue, driven by a speaker table so any named NPC can be given a face without touching a script.
- **Boss encounters** with animated intro cards, custom teams, and scripted set-piece choreography.
- **Quality-of-life systems:** an HM menu that frees a party slot, custom heal locations and checkpoints, a curio shop for Mega Stones, and Ability Machines.
- **Custom soundtrack**, sequenced and ported from later-generation Pokémon titles into the GBA's m4a engine, with the mixer widened from 5 to 12 direct-sound channels so field effects survive a busy score.
- **A post-game chapter** with a legendary encounter, a personal transport system, and a liberated overworld that reflects the ending.
- Built on **`pokeemerald-expansion`**, so the modern battle engine comes along with it: current-generation Pokémon, moves, abilities, items, and battle gimmicks. See [`FEATURES.md`](FEATURES.md) for that full upstream list.

# Credits

**Wishes of Tomorrow** — a game by **Joe Maloney**. Story, direction, game design, map design, and event scripting.

**Sprite Design**
Rafael Sanna · aveontrainer · baylorhernandez · biadoxaf · DarkusShadow · jaov46

**Character Art**
ToxShadows642

**Shadow Sprites**
pogokitten · WeeGeeDude · Quanyails

**Tileset Art**
pinkscales · Phyromatical · MagiScarf · PeekyChew · Elinthind · lo8jd · Dark Slayer

**Shin-Tokyo Art**
Emeiry · *Odisea* by ekat99 — used under [CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/)

**Music**
Arranged from the Pokémon soundtracks.

**Playtesting**
Luke Devereux · Mike Mancuso · Kyle Clarkson

**Special Thanks**
The spriting community, and every artist who shares their work.

*Rest in Paradise, Connor.*

A **Flatfoot Games** project.

## Built with

This game is built on **RHH (ROM Hacking Hideout)**'s `pokeemerald-expansion`, and would not exist without it or the decompilation it stands on.

```
Based off RHH's pokeemerald-expansion 1.15.3 https://github.com/rh-hideout/pokeemerald-expansion/
```

- [**RHH — `pokeemerald-expansion`**](https://github.com/rh-hideout/pokeemerald-expansion) and its [full contributor list](CREDITS.md)
- [**pret**](https://github.com/pret/pokeemerald) — the `pokeemerald` decompilation project
- *Pokémon Emerald Version* — GAME FREAK · Nintendo · The Pokémon Company

Third-party asset attributions carried over from the base are listed in [`EXTRACTED_ASSETS_CREDITS.md`](EXTRACTED_ASSETS_CREDITS.md).

# Source assets and design documents

This branch holds the game. The **[`main`](https://github.com/jmaloney95/Emerald-Rom-Hack/tree/main)** branch is the development workspace behind it — separate history, published alongside it:

- **Source art** — sprite sheets, Shadow Pokémon battle art, UI mockups, raw tilesets before they were compiled
- **The tileset pipeline** — the Python that converts RMXP-format art into GBA-legal primary/secondary tileset pairs
- **Design documents** — the GDD, story canon, quest plans, the full encounter spreadsheet, and a running development log covering every feature and engine gotcha (*contains story spoilers*)
- **The music porting process** — how later-generation tracks were sequenced into the GBA's m4a engine

Reuse terms differ by asset — most of the art belongs to the artists credited above, not to this project. See [Reusing this work](https://github.com/jmaloney95/Emerald-Rom-Hack/blob/main/README.md#reusing-this-work) before taking anything.

# Building

❗ Do not use GitHub's "Download ZIP" option — it omits commit history, which is required to merge upstream updates.

Build instructions, toolchain setup, and update guidance live in [`INSTALL.md`](INSTALL.md). In short, from a WSL2 or Linux shell with devkitARM installed:

```bash
make -j8
```

This produces `pokeemerald.gba`, playable in mGBA or any accurate GBA emulator.
