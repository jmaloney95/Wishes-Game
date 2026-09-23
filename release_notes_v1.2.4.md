# Wishes of Tomorrow v1.2.4

**Your save carries over.** The save format is identical to v1.2.3 and every version before it, so patch a fresh base ROM and keep playing. Nothing in this release touches a save slot.

A hotfix for the bullet train, plus the fight that was supposed to come with the Strength HM.

## Fixed

- **You can get off the Shinkansen again.** Boarding the bullet train and walking back to the door did nothing, which left you stuck inside the carriage with no way out but a reset. The door tile had never been told it was a door — the map declared the exit, but the tile the player stands on has to carry the warp behaviour for it to fire, and it didn't. **Fixed on both carriages:** the one you ride to Shin-Tokyo, and the one you board during the assault. The second had the same fault and nobody had hit it yet.

## New

- **Yiffer makes you earn the free sample.** He was handing over HM Strength in Munen Tunnel for nothing; now the free trial lesson is an actual fight — boss card, then Zangoose, Zeraora and a Machamp holding a Black Belt. Lose and you can walk back and try again. If your save already has Strength, he skips straight past it.
- **Walnut Woods redrawn** on the snow set, and Munen Village touched up alongside it.

## Credits

- **The space backdrop is by LibertyTwins**, not carchagui. Both drew for the FR Battle Backgrounds patch, and the whole set had been credited to one name. Corrected in the end credits, on the site and in the listings.

## Saves

The save structure is unchanged — SaveBlock2 3908 / SaveBlock1 15572, the same as v1.2.3, read out of the built ROM rather than assumed. An older save loads with no caveats this time. If you are coming from a version before 1.2.3, the two notes in [the v1.2.3 release](https://github.com/jmaloney95/Wishes-Game/releases/tag/v1.2.3) still apply.

## Getting it

Apply `wishes-of-tomorrow-1.2.4.bps` to a clean Pokémon Emerald (U) ROM — 16 MB, header code `BPEE`, CRC32 `1F1C08FB`. The patch stores that checksum, so a wrong dump is rejected before anything is written. Patched output is 32 MB, CRC32 `A8AA31E3`.

You can patch in the browser at [flatfootlabs.com](https://flatfootlabs.com) without installing anything, or use [Flips](https://github.com/Alcaro/Flips) locally.
