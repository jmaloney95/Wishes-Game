# Pokémon Wishes of Tomorrow — Toolchain & Setup Guide

**Build path:** Decompilation (`pokefirered`) + Porymap
**Companion to:** `Pokemon_Wishes_of_Tomorrow_GDD.docx`
**Last updated:** 2026-05-25

---

## Why this path (and how it differs from the GDD)

The GDD recommends the binary route (Advance Map + XSE + CFRU). We've switched to the
**decompilation** route for two reasons tied to your goals:

1. **You want vanilla mechanics.** CFRU's main value is *modern* mechanics
   (physical/special split, Gen 8 moves, Fairy type). You don't want those, so CFRU's
   setup complexity buys you nothing here.
2. **You want to lean on AI heavily.** In decomp, the entire game is source code, JSON,
   and PNG files. That means Claude can build large parts of the game *directly* —
   scripts, story, data tables, map wiring — instead of only advising while you click
   through Windows GUIs.

**Bonus:** you build the ROM straight from source, so you are not blocked on dumping a
physical cartridge to get started. (For sharing your finished hack you still distribute a
patch, and owning the game remains the right thing to do.)

---

## The toolset

### 1. Build environment (one-time setup)
> **See `Build_Environment_Setup.md` for the exact, verified step-by-step.** Summary below.

| Tool | Purpose | Where |
|------|---------|-------|
| WSL1 + Ubuntu | Linux layer on Windows where the game compiles (WSL1 recommended by pret for Porymap compatibility) | dism + Microsoft Store |
| binutils-arm-none-eabi | ARM assembler/linker for GBA | `apt install` inside WSL |
| agbcc | The GBA C compiler the default build uses | github.com/pret/agbcc |
| build deps | `build-essential`, `git`, `libpng-dev` | `apt install` inside WSL |
| devkitARM | *Optional* — only for the `make modern` target (not needed for vanilla mechanics) | skip for now |

### 2. The project
| Tool | Purpose | Where |
|------|---------|-------|
| pokefirered | The disassembled FireRed source — your game's base | github.com/pret/pokefirered |
| Git (+ GitHub acct, optional) | Version control + backups; revert any change safely | git-scm.com |

### 3. Editors
| Tool | Purpose | Where |
|------|---------|-------|
| Porymap | Maps, tilesets/metatiles, events, warps, wild encounters | github.com/huderlem/porymap |
| Poryscript | Friendly event-scripting language (replaces XSE) | github.com/huderlem/poryscript |
| VS Code (+ WSL ext.) | Browse and edit the source | code.visualstudio.com |

### 4. Graphics
| Tool | Purpose | Where |
|------|---------|-------|
| GIMP (free) or Aseprite (paid) | Process tilesets/art into indexed 16-color PNGs | gimp.org / aseprite.org |
| Paint.NET (free, optional) | Lightweight Windows alternative | getpaint.net |

### 5. Audio (later phase)
| Tool | Purpose | Where |
|------|---------|-------|
| mid2agb (ships with toolchain) | Convert MIDI tracks into the decomp's song format | included |
| Sappy / MIDI sources | Port atmospheric tracks (Golden Sun, etc.) | community |

### 6. The `Tools/` directory (not in version control)

`Tools/` and `mGBA-0.10.5-win32/` are **gitignored** — they are ~700 MB of
third-party binaries that would otherwise dominate the repository. Nothing in
the project references them by path, so a fresh clone builds without them;
they are only needed for the authoring workflows below. Re-create the folder
by downloading each of these:

| In `Tools/` | Purpose | Where |
|-------------|---------|-------|
| `porymap.exe` | Map, event, and wild-encounter authoring | github.com/huderlem/porymap/releases |
| `poryscript-windows` | Event-script language front end | github.com/huderlem/poryscript/releases |
| `porytiles-linux-amd64` | Tileset compilation (run under WSL) | github.com/grunt-lucas/porytiles/releases |
| `libresprite` | Pixel art and sprite editing | libresprite.github.io |
| `VGMTrans-v1.3` | Extract NDS sequences and samples for music ports | github.com/vgmtrans/vgmtrans/releases |
| `Gen 3 Sprite Pack` | Reference sprite sheets | community asset packs |
| `mGBA-0.10.5-win32/` (repo root) | Run and debug the compiled `.gba` | mgba.io/downloads.html |

The custom tileset build scripts in `tilesets_raw/build_scripts/` are pure
Python + Pillow and do **not** depend on Porytiles — they are the pipeline
actually used to produce the game's tilesets.

### 6. Testing
| Tool | Purpose | Where |
|------|---------|-------|
| mGBA | Run and debug the compiled `.gba` | already installed ✓ |

---

## Who drives what

**Claude can build directly (writing files):**
- Opening cutscene + all five-act story scripting (Poryscript)
- NPC dialogue and narration text
- Map data wiring, warps, connections (JSON)
- Wild encounter tables (the spirit/fossil Pokémon per route)
- Trainer parties and gym leaders
- Starter / fossil Pokémon data edits
- Region map data, build config, and debugging build errors

**You drive (GUI + creative):**
- Painting maps visually in Porymap (the fun part)
- Pixel-art tweaks in GIMP/Aseprite
- Final playtesting in mGBA
- All the creative calls

**Together:**
- Converting the community tilesets into GBA-ready indexed PNGs
- Overall structure and milestone planning

---

## Recommended setup order

1. **Install WSL2 + Ubuntu** — run `wsl --install` in an admin PowerShell, reboot.
2. **Install the toolchain inside WSL** — devkitARM, agbcc, and build deps.
   Follow the `INSTALL.md` in the pokefirered repo for the exact current commands
   (they're the authoritative source and change occasionally).
3. **Clone pokefirered** and do a first clean build to confirm the environment works
   (you should get a `.gba` you can open in mGBA).
4. **Put the project under Git** — make your first commit so you have a clean baseline.
5. **Install Porymap and Poryscript**, point Porymap at the project, open the maps.
6. **Install VS Code** + the WSL extension; open the project folder.
7. **Install GIMP or Aseprite** for graphics work.

Once step 3 produces a working ROM, the project is ready for real content work.

---

## Project-specific notes

- **Custom tilesets:** Your linked "Gen 3 Ultimate Tileset Collection" is non-commercial
  and credit-based. The Munen Village set requires crediting **Ekat99, Magiscarf,
  KingTapir, and J-Treecko252**. Keep a running credits list as you add art.
- **Ruined-timeline maps (Act 3):** These are duplicate maps with altered tilesets —
  conceptually simple, just volume. Decomp handles map duplication cleanly.
- **Seven-day clock (Act 2):** This is the one notable *custom mechanic* in an otherwise
  vanilla game. It's scriptable in decomp but is a real piece of work — flag it as its
  own milestone, not a day-one task.
- **Title screen animation:** Still a later-phase polish item, exactly as the GDD says.

---

## First milestone

Per the GDD: **build Act 1 only** — Munen Village, Route 1 (the Mist Path), and the
Archive City — as a playable demo before touching any other act.
