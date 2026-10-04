# Wishes of Tomorrow v1.2.6

**Your save carries over.** The save format is the same as every version before it, so patch a fresh base ROM and keep playing. A couple of controls behave differently on an existing save; they're under [Saves](#saves) below.

This covers everything since v1.2.4 on GitHub and this site. (v1.2.5 was a Hackdex-only hotfix; everything in it is here too.)

## Fixed

- **The crash after leaving the beach lifeguard shack or the yacht.** Stepping back onto Route 2 and then opening the Bag, or any other full-screen menu, could freeze or crash the game. The cause was one beach trainer, Tuber Coco, drawn with a FireRed sprite whose colours don't exist in this game. She's gone from the beach.
- **The yacht threw you straight back out.** Boarding it now works: stand on the gangway below the sign and press up. It stays locked until you have the Yacht Key.
- **Randomized trainers could only use Struggle.** With trainer randomization on, every swapped Pokémon went into battle with no moves. They now get their own species' moves for their level.
- **The map showed Kanto around Sennen.** Opening the map on the Sennen beach, the yacht, the lifeguard shack or Torii Route brought up the Kanto map, and the Sennen dojo and Ashlands hot spring brought up the Sevii Islands. They all show this region now, with you in the right place. Battles on the Sennen beach and Torii Route also play the same battle themes as everywhere else.
- **Fly works from the Sennen beach**, and an Escape Rope used on the yacht brings you back to it.
- **The FRAME option scrambled the Options menu** while you cycled through frames.

## New

- **The yacht's cargo.** Below deck the yacht's owner, the person designing the next generation of Pokémon, left a field-test case with three prototypes: **Browt, Pombon and Gecqua**. Each one shows its sprite before you choose, the way the starters do. Take one; the other two are spoken for. Read the papers on the desk, too.
- **Infinite Candy**: a Rare Candy that never runs out. It's in the Key Items pocket, so you can register it to SELECT. New games start with one.
- **Fast forward at 2x.** Options now offers OFF, 2x and 3x. 2x runs the overworld at double speed for anyone who found 3x too quick to steer, while battles go just as fast as at 3x.
- **AUTO SPRINT** in Options. TOGGLE (the default) runs by default, and a tap of B switches to walking and back. HOLD B is the classic hold-to-run.
- **A new title screen**: the Oni, over black.
- **General Edwards has a new look**, in the overworld and in battle.

## Changed

- **Character portraits are off.** The dialogue portraits were AI-assisted art, so they're switched off until they're redrawn by hand, and the option is gone from the menu for now. Boss intro cards still play in full, without the portrait.

## Saves

The save structure hasn't changed, so any earlier save loads. On an existing save:

- **AUTO SPRINT starts on TOGGLE.** You still run without holding anything, but B is now a tap to switch between walking and running, not a hold to walk. HOLD B in Options brings back hold-to-run.
- **Fast forward** keeps whatever you had: if it was on (3x), it's still 3x.
- **The Infinite Candy comes with new games only**, so an existing save won't have one.
- If you're coming from a version before 1.2.3, the two notes in [the v1.2.3 release](https://github.com/jmaloney95/Wishes-Game/releases/tag/v1.2.3) still apply.

## Getting it

Apply `wishes-of-tomorrow-1.2.6.bps` to a clean Pokémon Emerald (U) ROM — 16 MB, header code `BPEE`, CRC32 `1F1C08FB`. The patch stores that checksum, so a wrong dump is rejected before anything is written. Patched output is 32 MB, CRC32 `F1EEEA17`.

You can patch in the browser at [flatfootlabs.com](https://flatfootlabs.com) without installing anything, or use [Flips](https://github.com/Alcaro/Flips) locally.
