# Hackdex listing copy — Pokémon Wishes of Tomorrow

## SHORT DESCRIPTION (15 words)

A darker Emerald hack: an occupied region, a Shadow Pokémon war, and a three-act story.

**Alternates**

- Three acts, an occupied region, and Shadow Pokémon to snag and purify. Built from source. (15)
- Original region, original cast, 8 badges, Shadow Pokémon, and a story with a real ending. (15)
- Emerald, rebuilt from source: a mountain shrine, a fallen region, and a neon tower. (14)

---

## LONG DESCRIPTION

# About this hack

## What is Pokémon Wishes of Tomorrow?

**Wishes of Tomorrow** is a full-length ROM hack of *Pokémon Emerald* with an original region, an original cast, and a three-act story that actually finishes — ending, credits roll, Hall of Fame, post-game and all. It isn't a patched binary; it's built from source on RHH's `pokeemerald-expansion`, so every map, system and asset compiles from scratch. That's roughly 82,000 lines across 3,400 files, 64 new maps, 34 new tilesets and a custom soundtrack, in a 32 MB cartridge.

It opens quietly. You're in Munen Village, a mountain shrine town under Mt. Fuji, and the thousand-year ice on the peak is breaking. Following that question leads you to Prof. Clarkson, to Star Summit, and to Team Mutrid — who are not spiritual cranks, and who are much further along than anyone in the village realises.

Then the game turns. Act II is the region **after** you lose: the towns you walked through in Act I under occupation, a rebellion running out of people, and a march back through your own hometown. Act III drops you off a train in Shin-Tokyo, a neon surveillance city where Mutrid already runs everything and the citizens either don't know or won't say. You arrive with a Snag Machine and a bounty on a building-sized screen.

The tone is closer to *Unbound* than to vanilla: loss, inherited guilt, and a villain who believes he's saving people. It's a Pokémon game about grief that still lets you fight a gym leader with a Mega Ampharos.

## Developer's Note

This is my first ROM hack, and it took over my life for the better part of a year.

I wanted to make the Pokémon game I kept waiting for someone else to make — one where the story has weight, where losing is part of the plot, and where the region remembers what happened to it. I don't know if I pulled it off, but it's finished, it's play-tested end to end, and it's free.

If you play it, thank you, sincerely. If something breaks or a quest strands you, tell me — I'd much rather hear it than not.

— Joe, Flatfoot Games

## About the game

**Story & world**

- Original region with an eight-badge campaign across three acts — mountain shrine village, occupied countryside, surveillance city — plus a post-game archipelago
- 64 new maps and 34 new tilesets, all compiled from source
- Character portraits during dialogue, animated boss intro cards, and scripted set-pieces with detached camera work
- A custom soundtrack sequenced from later-generation Pokémon titles and ported to the GBA's m4a engine, with the mixer expanded from 5 to 12 direct-sound channels

**The Shadow Pokémon system**

- Shadow Pokémon are marked with a purple flame beside their level. They hit harder, they run with reduced defenses, they can snap into Hyper Mode — and they gain no experience at all until they're purified
- **Snag Balls** let you steal a Shadow straight out of a trainer battle
- Battle a snagged Shadow once to *open* it, then purify it at the shrine on Celebi Island
- A **Shadow Log** in the START menu tracks every Shadow in the game as loose, snagged or purified, with completion metrics
- Purification isn't optional flavour — it's how your team grows, and it's how the story resolves

**Choices that carry**

- What you do with Clarkson's research at the end of Act II is a real fork. Destroy it and Mutrid's Act III patrols stay capped. Keep it and their Shadow pool escalates hard — Haxorus, Melmetal, Ursaluna, Rhyperior, Annihilape — and you're the only one who gets to read the dossier. Neither path locks content

**Quality of life**

- A **quest journal** in the START menu with 12 tracked questlines, live objectives, and on-screen advancement notifications
- **HM field-move menu** — no HM slave, no wasted party slot
- Curio shop for Mega Stones, Ability Machines, custom healing locations, fast travel, searchable bag
- Modern battle engine via `pokeemerald-expansion` 1.15.3: current species, moves, abilities, items and mechanics

## New 1.2.0 features

**Save-compatible with 1.1.0 and 1.0.0** — patch a fresh base ROM and keep playing your existing file. (Randomizer Mode is chosen at new game, so a randomized run needs a new save.)

- **Randomizer Mode.** A settings screen right after you name your character: four switches for starters, wilds and trainer Pokémon, independently. Levels never change, so the difficulty curve stays the one the campaign was designed with. Gen 1–9 base forms only, deterministic per save and seeded from your own Trainer ID — the same Pokémon always becomes the same replacement, so the world stays coherent and two runs randomize differently. Shadow Pokémon are never randomized, which keeps the Shadow Log completable and the story's set-pieces intact
- **A new starter trio — Gible, Frigibax and Axew**, with portraits at the choice, all three evolving on the same schedule (16 and 36) so no pick is punished
- **A post-game region chart for the archipelago.** Press R on the map to toggle mainland and islands; fly destinations work on both
- **The Fisherman's questline pays off** — bring him his Gyarados and the Route 2 yacht opens, where Treecko, Torchic and Mudkip are waiting and one can come with you
- **The Battle Frontier is reachable.** After the ending, the Shin-Tokyo ferry runs to it
- **The Game Corner has real prizes** — fifteen Pokémon from Gimmighoul up to Porygon, plus thirteen TMs and evolution items, nine of those TMs new to the game (Draco Meteor, Close Combat, Scale Shot, Play Rough, Swords Dance, Nasty Plot, Scorching Sands, Earth Power, Knock Off)
- **Mega Stones.** Three lie on the floor of the Distortion World; for anything else the Minister at the Shin-Tokyo docks will source the stone for whoever's at the front of your party
- Flying is now earned for purifying your *first* Shadow rather than for finishing the game, and purifying five earns a Gold Bottle Cap

**Fixed in 1.2.0:** permanently blank text boxes after warping during the map-name banner; randomizer settings being wiped by new-game init; a softlock on Celebi Island; misplaced fly destinations; two out-of-bounds surf landings; a truncated Snag Ball description.

## How to play

You'll need a legally obtained **Pokémon Emerald (U)** ROM — 16 MB, `BPEE`, CRC32 `1F1C08FB`.

1. Grab `wishes-of-tomorrow-1.2.0.bps` from the releases page
2. Apply it with the **in-browser patcher at [flatfootlabs.com](https://flatfootlabs.com)** — your ROM never leaves your machine — or with any BPS patcher (Flips, Rom Patcher JS)
3. Play the 32 MB output (`8AA8F352`) in mGBA or another accurate GBA emulator. flatfootlabs.com will also just run it in your browser
4. No account, no launcher, no cost

**A few things the game doesn't spell out:**

- **Shadows won't purify until they've been opened** — send a snagged Shadow into one battle first, then take it to the shrine
- **To reach the shrine**, wait for the tides to drop in Act III and **surf south**. You don't need Fly to get there the first time — Fly is the reward for your first purification, not a badge
- **The tower stays sealed** until you've finished the Celebi Island trial
- **The Catalpa Bow** has exactly one use spot: the grave in the National Park
- **When in doubt, open the quest log** in the START menu — every active quest carries its current objective and where it points

There's a full Player's Guide for Acts I–III if you get properly stuck.

## Support

The game is free and always will be. The best support you can give it is playing it and telling me what broke.

- **[flatfootlabs.com](https://flatfootlabs.com)** — patcher, browser play, downloads and FAQ
- **Bug reports:** email **joe@flatfootlabs.com** with your version number and your save file if you have it. Save files make bugs about ten times easier to chase down
- Story feedback, balance complaints and "this quest stranded me" reports are all equally welcome

## Future Plans

The main campaign is content complete — the work from here is the **post-game**.

Freeing what's in the tower scattered Shadow legendaries across the region, and I want that hunt to be a real chapter rather than a checklist: more of the deeper archipelago opened up, the remaining islands built out, and the hardest purifications in the game waiting on them. Alongside that, patches for whatever players turn up in 1.2.0.

## Credits

**Creator:** Joe Maloney — story, direction, game design, map design, event scripting

**Sprite Design:** Rafael Sanna, aveontrainer, baylorhernandez, biadoxaf, DarkusShadow, jaov46
**Character Art:** ToxShadows642
**Shadow Sprites:** pogokitten, WeeGeeDude, Quanyails
**Tileset Art:** pinkscales, Phyromatical, MagiScarf, PeekyChew, Elinthind, lo8jd, Dark Slayer
**Shin-Tokyo Art:** Emeiry; *Odisea* by ekat99 (CC BY-NC-SA)
**Battle Backdrops:** carchagui (open sea, cave, laboratory, space, town, interior); aveontrainer (Ashlands savanna, molten summit)
**Music:** arranged from official Pokémon soundtracks
**Playtesting:** Luke Devereux, Mike Mancuso, Kyle Clarkson

**Special Thanks:** the spriting community and every artist who shares their work. *Rest in Paradise, Connor.*

**Built with** RHH's `pokeemerald-expansion` 1.15.3 and the `pokeemerald` decompilation by pret, based on *Pokémon Emerald Version* (GAME FREAK / Nintendo / The Pokémon Company). A **Flatfoot Games** project — non-commercial, unaffiliated, and unendorsed.
