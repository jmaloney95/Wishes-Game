# Wishes of Tomorrow v1.2.2

**Save-compatible with v1.2.1, v1.2.0, v1.1.0 and v1.0.0** — patch a fresh base ROM with this version and keep playing on your existing save. Nothing here changes the save format.

## New

- **Wild double battles.** They turn up about one encounter in five, and you can aim a Poké Ball at whichever of the two you actually want rather than being told you can't throw one at all. If you're down to a single usable Pokémon the game quietly gives you a normal battle instead.
- **Shinies at 1 in 256**, up from roughly 1 in 500. A shiny's HP/EXP box is gold, so one can't slip past you while you're mashing through an encounter. The health bar keeps its green/yellow/red, because that's a status readout and not decoration.
- **An Exp. Share in your bag from the first step**, switched on. Use it in the bag to toggle it off whenever you'd rather not share.
- **3x fast forward**, in Options, off by default. Battles run faster still. The music keeps its own time and pitch — nothing sounds like a chipmunk — because the sound mixer runs on the hardware's interrupt schedule rather than on the game loop.
- **The Options list scrolls now**, which is what made room for the new switches.
- **Name entry stays on capitals** instead of dropping to lowercase after the first letter.
- **Each starter learned something with personality**: Gible gets Thousand Arrows, Frigibax gets Glaciate, Axew gets Dragon Darts.
- **A new team for the Mutrid tower's captain**, built around a Mega Gyarados and a Shadow Salamence.

## Fixed

- **The tower guards crashed the game on sight.** Their overworld sprites were FireRed's, not Emerald's, and the game came apart the moment one of them started to move. Three NPCs were affected across the tower and Tradewind Gym.
- **The Minister's Mega Stone counter blue-screened**, and offered stones to Pokémon that have no Mega form at all. Both came from the same mistake in how his shop reported its result back to the game.
- **Battles could freeze mid-animation** with fast forward on.
- **Your starting bag was empty.** The Quest Log and everything else meant to be there was being cleared immediately after it was handed out.
- **Mega Pokémon showed as their base forms** on the Hall of Fame and continue screens.
- **NPCs walked out through walls.** Eight of them, Professor Clarkson most visibly — he crossed four separate walls leaving Walnut Woods. Every walking NPC's exit route has been re-checked against the actual map.
- Wild double battles could put the same Pokémon in both slots.

## Changed

- Dialogue no longer leans on `--` for its pauses, which had started to read like everyone in the region talks the same way.

## Getting it

Apply `wishes-of-tomorrow-1.2.2.bps` to a clean Pokémon Emerald (U) ROM — 16 MB, header code `BPEE`, CRC32 `1F1C08FB`. The patch stores that checksum, so a wrong dump is rejected before anything is written. Patched output is 32 MB, CRC32 `52866AB5`.

You can patch in the browser at [flatfootlabs.com](https://flatfootlabs.com) without installing anything, or use [Flips](https://github.com/Alcaro/Flips) locally.
