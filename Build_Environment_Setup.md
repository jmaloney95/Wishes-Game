# Build Environment Setup — Pokémon Wishes of Tomorrow

**Base: `pokeemerald-expansion` v1.15.3 (RH-Hideout).**
**Verified against the repo's own `docs/install/` (2026-05-25).**
**Goal:** get a working Emerald-expansion build, ending with `pokeemerald.gba` booting in mGBA.

> **Why the switch from FireRed:** Emerald is the stronger, best-supported decomp base, and
> the **expansion** ships every Pokémon through the latest generation *with finished sprites* —
> so your entire cross-gen roster needs zero insertion work. It also bundles modern mechanics
> (physical/special split, abilities, Fairy type).
>
> **Good news on effort:** the expansion setup is *simpler* than FireRed's. No `agbcc` to
> build, no devkitPro installer — the whole toolchain comes from Ubuntu's `apt`, and the build
> is a single `make`.

---

## What you already have (carries over)
You set all of this up for FireRed and it's reused as-is:
- ✅ WSL1 + Ubuntu
- ✅ Git, `build-essential`, `binutils-arm-none-eabi`, `libpng-dev`, `python3`
- ✅ The `J:\ROM Hack Project` working folder and the `metadata` drive-mount fix

**You can ignore/delete the old `pokefirered` and `agbcc` folders — we're not using them.**

---

## Stage 1 — Add the two new dependencies  *(in the Ubuntu terminal)*

The expansion's modern compiler needs two extra apt packages. This command installs them
(and re-confirms the ones you already have):

```bash
sudo apt update
sudo apt install -y build-essential binutils-arm-none-eabi gcc-arm-none-eabi libnewlib-arm-none-eabi git libpng-dev python3
```

✅ **Checkpoint 1:** installs with no errors. (`gcc-arm-none-eabi` and `libnewlib-arm-none-eabi`
are the only genuinely new ones.)

---

## Stage 2 — Make sure the project drive allows Git permissions

If you applied the permanent `/etc/wsl.conf` fix earlier, skip this. If not, run it once so the
clone doesn't hit the `core.filemode` error again:

```bash
printf '[automount]\noptions = "metadata,noatime"\n' | sudo tee /etc/wsl.conf
```
Then run `wsl --shutdown` in PowerShell, reopen Ubuntu. (Or, for just this session:
`cd ~ && sudo umount /mnt/j && sudo mount -t drvfs J: /mnt/j -o metadata,noatime`.)

---

## Stage 3 — Clone the expansion

```bash
cd "/mnt/d/ROM Hack Project"
git clone https://github.com/rh-hideout/pokeemerald-expansion
```

✅ **Checkpoint 2:** a `pokeemerald-expansion` folder exists, cloned with no `core.filemode` error.

---

## Stage 4 — Build it

```bash
cd pokeemerald-expansion
make -j$(nproc)
```

When it finishes you'll have **`pokeemerald.gba`** in `J:\ROM Hack Project\pokeemerald-expansion\`.

> **Heads-up on build time:** because your files live on the Windows `J:` drive under WSL1, the
> *first* build is I/O-heavy and can take a while (this is the trade-off for keeping everything
> on J: where Porymap can see it). Later builds are incremental and quick. If first-build time
> ever becomes painful, ask me about moving to WSL2 for a big speedup.

✅ **Checkpoint 3:** `pokeemerald.gba` exists in the folder.

---

## Stage 5 — Boot it in mGBA

Open `J:\ROM Hack Project\mGBA-0.10.5-win32\mGBA.exe` → File → Load ROM →
`J:\ROM Hack Project\pokeemerald-expansion\pokeemerald.gba`. You should reach the Emerald
title screen and be able to start a game.

✅ **Checkpoint 4:** Emerald runs. **Your toolchain works.** Every source change now recompiles
with `make`.

---

## Stage 6 — Install the editors (Windows apps)

- **Porymap** — https://github.com/huderlem/porymap/releases → unzip (e.g. `J:\ROM Hack Project\tools\`).
  First launch: **Open Project** → select `J:\ROM Hack Project\pokeemerald-expansion`.
- **Poryscript** — https://github.com/huderlem/poryscript/releases + the VS Code "Poryscript"
  extension. Turns our `.pory` files into the build.
- **VS Code** — https://code.visualstudio.com + the **WSL** extension; open the
  `pokeemerald-expansion` folder.

---

## Stage 7 — Where our drafted work plugs in (Emerald paths)

- `act1_scripts/munen_village.pory` → script files under
  `pokeemerald-expansion/data/maps/<MapName>/scripts.pory` once those maps exist in Porymap.
  *(Minor note: a few command/constant names differ between FireRed and Emerald — I'll adapt
  them when we wire it.)*
- Flags/vars from that file → `pokeemerald-expansion/include/constants/flags.h` and `vars.h`.
- **Your cross-gen Pokémon are already in the ROM** — Tyrunt, Amaura, the spirit lines, the
  Bone-Field fossils, the Aegislash line. We don't add them; we just place them (encounters,
  starters, trainers). See `Custom_Pokemon_List.md`.

Once Stage 5 passes, tell me and I'll start wiring Munen Village and your starters into the
actual project.

---

## Quick troubleshooting

| Symptom | Fix |
|---|---|
| `could not set 'core.filemode'` on clone | Stage 2 (metadata mount / wsl.conf) |
| `arm-none-eabi-gcc: command not found` | re-run Stage 1 (need `gcc-arm-none-eabi`) |
| `cannot find -lc` / newlib errors | install `libnewlib-arm-none-eabi` (Stage 1) |
| Build is very slow | expected on WSL1 + `/mnt/d`; ask about WSL2 migration |
| Porymap can't read project | confirm the repo is under `J:\` (not `\\wsl$`) |
