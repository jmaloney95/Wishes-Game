# Mega sprites, converted to GBA format (rounds 112 and 113, 2026-10-19)

Source: `../MEGA SPRITES/` (24 front+back pairs, 80x80 RGBA; `ALL.png` is just a contact sheet).
Converter: `pokeemerald-expansion/tools/build_mega_art.py` (80x80 -> 64x64 premultiplied LANCZOS,
front+back share ONE 15-colour palette + transparent idx0, RGB555-quantized).

**All 24 are now in the ROM** (round 113 made the 19 fan Megas real species). The copies in this
folder are the round-112 staging output and are superseded by the files under
`graphics/pokemon/<species>/mega/` (note: the staging folder `amoongus` is the species `amoonguss`).

- Canon Megas re-skinned (round 112): darkrai, excadrill, feraligatr, meganium, pyroar.
- New Mega species (round 113): amoonguss, azumarill, butterfree, conkeldurr, dugtrio, electivire,
  flygon, golem, jumpluff, kabutops, magmortar, milotic, omastar, poliwrath, shiftry, snorlax,
  torkoal, typhlosion, whimsicott. Data: `src/data/pokemon/species_info/wot_megas.h`.
- Shiny palettes: `tools/build_mega_shiny.py`. Mega Stone icons: `tools/build_mega_stone_icon.py`.
- In-game battle mockups: `tools/render_battle_mockup.py`.
