# Wishes of Tomorrow v1.2.0

**Save-compatible with v1.1.0 and v1.0.0** — patch a fresh base ROM with this version and keep playing on your existing save. Randomizer Mode is chosen when you start a new game, so a randomized run needs a new file.

## Randomizer Mode

Start a new game and, right after you name your character, a settings screen asks how you want to play.

- **Four switches.** Randomizer on or off, then Starter Pokémon, Wild Pokémon and Trainer Pokémon independently. Turn the master switch on and all three come with it; leave any of them off to randomize only what you want.
- **Levels never change.** Only species are swapped, so the difficulty curve of the campaign is exactly the one it was designed with.
- **Gen 1–9, base forms only.** No megas, no regional forms, nothing outside the National Dex.
- **Deterministic per save.** The same Pokémon is always replaced by the same one for the whole of that playthrough, seeded from your own Trainer ID — so the world stays coherent and two runs randomize differently.
- **Your starter's portrait is the Pokémon you get.** The three sealed choices show whatever they actually rolled into.
- **Shadow Pokémon are never randomized.** Every Shadow keeps its identity, which keeps the Shadow Log completable and the story's set-pieces intact. The Oni's ace and Draco's Armored Mewtwo are left alone too — that Mewtwo is handed to you after the fight, and rolling it would mean fighting one thing and receiving another.
- **Changed your mind?** The PC in your bedroom reopens the same settings screen any time before you take your starter.

## New

- **A new starter trio — Gible, Frigibax and Axew** — with their portraits shown at the choice. All three lines now evolve on the same schedule, at 16 and 36, so no choice is punished by a late second or third stage.
- **A post-game region chart for the archipelago**, given by Nessa after the jailbreak. Press R on the map to toggle between the mainland and the islands; fly destinations work on both.
- **The Fisherman's questline pays off.** Hand him the Gyarados he wants and you get what you need to take the Route 2 yacht — where Treecko, Torchic and Mudkip are waiting, and you may bring one along.
- **The Battle Frontier is reachable.** After the ending, the ferry docks in Shin-Tokyo and runs to the Frontier.
- **The Game Corner has real prizes.** Two counters, both scrolling lists with prices shown: fifteen Pokémon from Gimmighoul at 999 coins up to Porygon at 9,999 (holding a Dubious Disc), and thirteen TMs and evolution items. Nine of those TMs are new to the game — Draco Meteor, Close Combat, Scale Shot, Play Rough, Swords Dance, Nasty Plot, Scorching Sands, Earth Power and Knock Off.
- **Mega Stones.** Three lie on the floors of the Distortion World, drawn as the stones themselves rather than as item balls: Garchompite, Baxcalibrite and Gyaradosite. For everything else, the Minister at the Shin-Tokyo docks will source the stone for whichever Pokémon is at the front of your party, for ₽10,000 — and tells you plainly when that Pokémon has no Mega form.
- Purifying five Shadow Pokémon for the Professor's assistant now rewards a **Gold Bottle Cap**.
- Flying is now the reward for purifying your first Shadow rather than for finishing the game, and the three guardians teach you how purification actually works.

## Fixed

- **Text boxes and the start menu could go permanently blank.** Warping while the map-name banner was still on screen left the text layer scrolled off the display, and nothing put it back — every message box and menu stayed empty from then on, across maps. Most reproducible walking straight into the Shin-Tokyo arcade.
- **Randomizer settings did nothing.** The choice was being erased by new-game initialisation moments after it was made, so both on and off produced a standard game.
- **A softlock on Celebi Island.** Beating the three trainers and approaching the lake trio could leave no way to trigger Celebi's arrival.
- Fly destinations on the main region map were scattered across the wrong locations.
- Surfing off the south of Shin-Tokyo, and returning north from Celebi Island, both landed outside the playable water.
- The Snag Ball's description was cut off after one line.

## Checksums

| | CRC32 |
| --- | --- |
| Base ROM — Pokémon Emerald (U), 16 MB, `BPEE` | `1F1C08FB` |
| Patched output — 32 MB | `8AA8F352` |

Apply `wishes-of-tomorrow-1.2.0.bps` to a clean Pokémon Emerald (U) dump with the in-browser patcher at [flatfootlabs.com](https://flatfootlabs.com), or any BPS patcher.
