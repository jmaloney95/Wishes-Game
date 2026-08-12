# Wishes of Tomorrow

A full-length Pokémon ROM hack for the Game Boy Advance, with an original region, cast and three-act story, plus a post-game chapter. It's built from source on RHH's [pokeemerald-expansion](https://github.com/rh-hideout/pokeemerald-expansion) 1.15.3.

**[Play it](https://flatfootlabs.com)** · [Releases](https://github.com/jmaloney95/Wishes-Game/releases) · [Report a bug](https://github.com/jmaloney95/Wishes-Game/issues/new/choose)

The game is distributed as a patch. Apply it to your own legally obtained copy of *Pokémon Emerald*. This repository doesn't contain a ROM.

## Features

- **Shadow Pokémon.** Corrupted Pokémon hit harder and can slip into Hyper Mode mid-battle. Snag them from enemy trainers with the Snag Machine and Snag Balls, then purify them. The Shadow Log tracks every one in the game.
- **Quest journal.** Main and side quests in the start menu, with on-screen notifications as they advance.
- **Character portraits** during story dialogue.
- **Boss battles** with animated intro cards, custom teams and scripted set pieces.
- **Quality of life.** An HM menu that frees a party slot, custom heal locations and checkpoints, a curio shop for Mega Stones, and Ability Machines.
- **Custom soundtrack** ported from later Pokémon games into the GBA's sound engine.
- **Post-game chapter** with islands to surf and fish between, a legendary encounter, and an overworld that reflects the ending.
- **Modern battle engine** from pokeemerald-expansion: current-generation Pokémon, moves, abilities, items and battle gimmicks.

## Building

On Linux or WSL2:

```bash
sudo apt install build-essential git gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi libpng-dev python3
git clone https://github.com/jmaloney95/Wishes-Game.git
cd Wishes-Game
make -j$(nproc)
```

This builds `pokeemerald.gba`. Clone with git rather than downloading a ZIP, because the build stops if it can't find git history.

The engine is documented in the [pokeemerald-expansion repository](https://github.com/rh-hideout/pokeemerald-expansion).

## Credits

**Wishes of Tomorrow** is a game by **Joe Maloney**: story, direction, game design, map design and event scripting.

**Sprite Design**
Rafael Sanna · aveontrainer · baylorhernandez · biadoxaf · DarkusShadow · jaov46

**Character Art**
ToxShadows642

**Shadow Sprites**
pogokitten · WeeGeeDude · Quanyails

**Tileset Art**
pinkscales · Phyromatical · MagiScarf · PeekyChew · Elinthind · lo8jd · Dark Slayer

**Shin-Tokyo Art**
Emeiry · *Odisea* by ekat99, used under [CC BY-NC-SA](https://creativecommons.org/licenses/by-nc-sa/4.0/)

**Battle Backdrops**
carchagui (open sea, cave, and the FR-patch Lab/Town/Indoors) · LibertyTwins (space) · aveontrainer (Ashlands savanna, molten summit)

**Music**
Arranged from the Pokémon soundtracks.

**Playtesting**
Luke Devereux · Mike Mancuso · Kyle Clarkson

**Special Thanks**
The spriting community, and every artist who shares their work.

*Rest in Paradise, Connor.*

A **Flatfoot Labs** project.

### Built with

- [RHH's pokeemerald-expansion](https://github.com/rh-hideout/pokeemerald-expansion) and its [contributors](CREDITS.md)
- [pret's pokeemerald](https://github.com/pret/pokeemerald) decompilation
- Porymap, Porytiles, Poryscript, LibreSprite and VGMTrans

Third-party asset attributions carried over from the base are in [EXTRACTED_ASSETS_CREDITS.md](EXTRACTED_ASSETS_CREDITS.md).

*Pokémon* is a trademark of Nintendo, Creatures Inc. and GAME FREAK. This is a non-commercial fan project, not affiliated with or endorsed by them.
