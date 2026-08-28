# Wishes of Tomorrow v1.2.1

**Save-compatible with v1.2.0, v1.1.0 and v1.0.0** — patch a fresh base ROM with this version and keep playing on your existing save. Nothing in this release touches the save format.

A hotfix for the issues found in v1.2.0, plus a new title screen.

## Fixed

- **Shadow Pokémon can now Mega Evolve.** Holding the right stone was being ignored for any Pokémon still in its Shadow state, so a Shadow Gyarados with a Gyaradosite simply never transformed.
- **The Jirachi at the summit is the right one.** The tower's final encounter was spawning the ordinary Jirachi rather than the Shadow one the story is about — so it could not be snagged, and the Shadow Log could not be completed.
- **Mega forms now show on the Hall of Fame and resume screens.** A Pokémon holding its stone was drawn in its base form on the champion roll and on the continue screen, which made a Mega-heavy team look like it had reverted.
- **The ship at the end of the story sails again.** It appeared at the dock after the ending but could not be interacted with; it now runs to the post-game Battle Frontier as intended.
- **Siren Allison behaves like a Trainer.** She could not see you at all if you stayed on land — a water-bound Trainer can never sight across a waterline — so she was simple to walk past. She now spots you from open water, swims in, and comes ashore to battle, with splash audio on the approach and the same banner shimmer as General Edwards. Her overworld sprite, battle sprite and portrait have all been redrawn from the artist's sheet at the correct scale; she was previously appearing at double size next to every other NPC.

## Changed

- **New title screen.** Rebuilt from the artist's layered artwork at 256 colours: the nebula, the peak, the logo, the stylised "Wishes of Tomorrow" lockup and Shadow Jirachi, with PRESS START blinking over the top. The earlier version was assembled from a single flattened image and carried visible artifacts from it — a canvas grid, speckling in the rocks and logo, and a washed band behind the title.

## Getting it

Apply `wishes-of-tomorrow-1.2.1.bps` to a clean Pokémon Emerald (U) ROM — 16 MB, header code `BPEE`, CRC32 `1F1C08FB`. The patch stores that checksum, so a wrong dump is rejected before anything is written. Patched output is 32 MB, CRC32 `81AF35BA`.

You can patch in the browser at [flatfootlabs.com](https://flatfootlabs.com) without installing anything, or use [Flips](https://github.com/Alcaro/Flips) locally.
