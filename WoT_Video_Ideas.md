# Wishes of Tomorrow: Video Ideas & Research

Running list of channel content. Update the status column as videos ship.
Numbers in the research sections were checked against the v1.2.4 source
(`src/data/trainers.party`, map scripts, species files), not the community guide.

**Status key:** `idea` · `researching` · `scripted` · `recorded` · `posted` · `on hold`

**Platforms:** long-form on YouTube (the income channel). Shorts are exported
clean (no watermark) and posted to YouTube Shorts, TikTok and Instagram Reels
on the same day. Download link goes in bios and pinned comments, never in the
video itself.

**Standing rules:** royalty-free or original music only (Nintendo/Game Freak
tracks get claimed). Label spoiler videos in the title. Test every battle claim
in-game before recording.

**Format rule:** no camera. Shorts can be silent, with on-screen text.
Long-form gets a voiceover: silent long gameplay loses viewers, and it risks
YouTube's "inauthentic/repetitive content" check at YPP review.

**Hybrid videos (real gameplay + generated graphics, like the Oni trailer):** the
footage map, problems found in existing clips, and a six-video slate are in
`Youtube/Hybrid_Video_Plan.md`.

## Easiest first (start here)

1. **Capture and caption (under 30 min each):** S20 1% montage, S3 mapping time-lapse,
   S25 region zoom-out, S4 before/after, S5 sprite showcase, S6 boss teaser,
   S15 snag clip, S16 Hyper Mode, S17 randomizer roulette, S23 stat card
2. **Text-driven (on-screen facts, a little editing):** S18 cursed starter,
   S19 one type beats this boss, S21 Mega Stone locations, S28 evolve during the day,
   S7 guess the location, S8 would you catch it
3. **Needs the community first:** S1 Hall of Fame slideshow, L13 20k video
4. **Easiest long-form (voiceover):** L1 boss counters and L2 best team (research done),
   L11 patch notes, L10 playthrough

## TikTok playbook (researched 2026-09-29)

- **The niche's dominant format is the ranked list:** "Top 10 completed ROM hacks for 2026",
  "ROM hacks with Nuzlocke mode", "underrated GBA hacks". Use the same keywords in captions:
  *completed, GBA, 2026, new region, Nuzlocke*. Pitch WoT to list channels
  (e.g. PokeCoders) with a ready-made clip pack.
- **Single-game hooks work better than feature lists:** "This GBA hack lets you steal legendaries."
- **Carousels (photo mode) are for comments and saves:** tier lists, boss cheat sheets,
  Mega Stone locations, Hall of Fame. **Video is for reach:** the data is mixed, and one large
  study found carousels got about 1/5 of video views.
- **Sounds:** phonk (Brazilian phonk especially) for hype and gameplay edits. A trending
  sound for carousels. The in-game shiny sparkle for shiny and 1% clips.
  Check trending sounds in-app each week before posting.
- **Export clips without music.** Add the sound inside each app. TikTok library sounds
  get claimed on YouTube, and business accounts can only use the Commercial Music Library.

## CapCut templates

- **Template types that fit the niche:** "Who's that Pokémon" silhouette reveal (custom species, S5/S7),
  beat-synced speed edit (battle and snag clips), Top-N countdown (L3/S13), before/after
  swipe (S4), "wait for it" zoom (shinies, 1% encounters), photo slideshow (Hall of Fame; on
  Shorts, run 1–2 s per slide).
- **Before export:** delete CapCut's end card, replace template music that isn't from the CapCut
  library, and turn off blur/glow effects that smear the pixel art.
- **Framing the GBA screen in 9:16:** fill the top and bottom with a blurred copy of the clip, or
  crop to the action. Text goes in the top third (the Shorts UI covers the bottom).

---

## Long-form (YouTube)

| # | Idea | Format | Status | Notes |
|---|------|--------|--------|-------|
| L1 | **Best counters for every boss** | Voiceover over battle footage | researching | Data in *Research: boss teams & counters*. One video, or split by Act. |
| L2 | **The best team in Wishes of Tomorrow** | Voiceover, one segment per pick | researching | Data in *Research: best team*. |
| L3 | **10 unique features** | Listicle, voiceover | idea | Shiny starters & wild shinies, Curio exchange → Mega Stones, Carved Mask, Catalpa Bow, Ability machines/Paws, rare captures (Delta Dodrio, Hellraiser, 1% spawns), Shadow Dossier, Snag Machine. Need 2 more. Candidates: Randomizer Mode, the Star Summit impossible battle, Celebi purification, boss intro cards. Lead with the strangest one. |
| L4 | **Keep or destroy Clarkson's research** | Lore / consequences, spoilers | idea | Choice is in `MunenLabAct2` (keep vs destroy). Show both branches and where the pools of Shadow Pokémon come from. Strongest lore idea. |
| L5 | **Full story explained** | Spoiler recap | idea | Title it clearly as spoilers. Release later, once most players have finished. |
| L6 | **Lore explainers** (series) | 5–10 min, one topic each | idea | Characters, factions, mysteries. Canon docs are the script bank. |
| L7 | **Behind the story** | Voiceover | idea | What was cut, what changed, why. |
| L8 | **Devlogs** | Screen capture + voiceover | idea | How a town, mechanic or scene was built. |
| L9 | **ROM hacking tutorials** | Screen capture + voiceover | idea | Porymap, RMXP → GBA tilesets (halve the art, 512 tiles / 7 palettes), scripting events. Long search tail. |
| L10 | **Playthrough / walkthrough series** | Gameplay + light commentary | idea | Fastest route to YPP watch hours. |
| L11 | **Patch notes per release** | Changelog over footage | idea | v1.2.4 onward. |
| L12 | **Tier list** | Long-form or Short | idea | Pokémon, routes, characters, bosses, or the custom species. Run a community poll first for a follow-up video. |
| L13 | **20,000 downloads milestone** | See the plan below | idea | |
| L14 | **AI in the game** | Calm, factual | on hold | Only if you decide you want that conversation. The release page already has an AI disclosure (portraits toggle off in the options menu, dialogue drafts edited by hand, human cover art, automated scripting). Stick to "what I used and why", not a debate. |
| L15 | **THE ONI villain trailer** | Motion graphics, 60 s, original sound design | recorded | Done 2026-10-08 in `Youtube/Oni Trailer/`: video, 3 thumbnails for Test & compare, and title/description/pinned comment/tags in `youtube_upload.md`. Re-render with `oni_trailer.py`; thumbnails with `thumbnail.py`. A 9:16 cut for Shorts/TikTok isn't made yet. |

### L13: the 20k downloads video

**Long-form (3–5 min), "20,000 of you played Wishes of Tomorrow"**
1. **Cold open:** a counter climbs from 0 to 20,000 over fast cuts from the trailer.
2. **The story of the release:** what the first week looked like, the trailer
   passing 1M views, Hackdex numbers, and the first bug reports.
3. **Community wall:** players' Hall of Fame screenshots, fan art and best
   comments, all credited. Ask for submissions a week before recording.
   The same material becomes the Hall of Fame slideshow Short (S1).
4. **Thank-yous:** playtesters, sprite and tileset artists, LibertyTwins,
   the pokeemerald-expansion team. The credits already exist.
5. **What's next:** a roadmap teaser (next patch, post-game, anything you can
   show). End on a question for the comments.

**Short (under 20 s):** a counter ticks to 20,000 over the best trailer shot, then "Thank you."
**Community post:** a poll such as "Which starter did you pick?". It drives comments and gives the next video data.

Optional milestone gift: a small thank-you patch (for example an NPC who
names the community, or a hidden item) that is announced in the video. It gives the
video news value. Keep it free, and keep donation links out of it.

---

## Shorts / TikTok / Reels (faceless, no script)

| # | Idea | Status | Notes |
|---|------|--------|-------|
| S1 | **Hall of Fame slideshow** (swipe format) | idea | Community-submitted lineups, credited. |
| S2 | Best moments (a battle finish, a rare encounter) | idea | 10–20 s with a one-line caption. |
| S3 | Mapping time-lapse, Porymap to the finished map in-game | idea | |
| S4 | Before/after: RMXP art vs the GBA tileset, or vanilla vs redesign | idea | Wipe transition. |
| S5 | Sprite showcase / custom species reveal | idea | One creature per Short (there are 7). |
| S6 | Boss teaser: intro card and sprite, "Can you beat them?" | idea | The boss intro cards are ready-made hooks. |
| S7 | Guess the location (cropped tile, reveal at the end) | idea | |
| S8 | "Would you catch it?" | idea | |
| S9 | Bug and glitch blooper reel | idea | |
| S10 | Day in dev: "fixing a softlock" | idea | |
| S11 | Update announcement | idea | |
| S12 | Progress counter ("Act 3: X% done") | idea | |
| S13 | Each of the 10 features as its own Short | idea | Cut from L3. |
| S14 | **"Can you win the impossible battle?"** | idea | Star Summit ??? (Lv50 Typhlosion + Feraligatr) is scripted to be lost. Beating it makes a great clip. |
| S15 | **Snagging a legendary** | idea | Shadow Ho-Oh, Moltres, Raikou or Salamence, from the throw to the catch. |
| S16 | **Hyper Mode moments** | idea | A Shadow Pokémon losing control mid-battle. |
| S17 | **Randomizer Mode roulette** | idea | "Randomizer gave me ___ as a starter." Easy to repeat. |
| S18 | **"Your starter is cursed"** | idea | All three starters are Dragons, the first gym is Ice, and Gible takes 4×. |
| S19 | **One type beats this boss** | idea | Mutrid Alpha: Fairy hits all six. Text on screen, then the sweep. |
| S20 | **1% encounter montage** | idea | Politoed, Gyarados Mega, Treecko, Chi-Yu, Litten, Sobble… |
| S21 | **Where to get every Mega Stone** | idea | Quick cuts, one stone per beat. Data in *Research: Mega Stones*. |
| S22 | **Secret spots** | idea | The hidden lab behind the bookcase, Celebi Island, Distortion World layers. |
| S23 | **Boss stat card** (e.g. Hellraiser: 125 Sp. Atk, 105 Speed) | idea | Static graphic, then the fight. |
| S24 | **Replies to comments** | idea | Answer a real question with footage. |
| S25 | **Region zoom-out** | idea | Pan across the full Porymap region. Loops well. |
| S26 | **16-color challenge** | idea | Artwork quantized down to the GBA palette limit, step by step. |
| S27 | **Nuzlocke moments** | idea | Clip a close call. Invite viewers to post their runs. |
| S28 | **"Evolve it during the day"** | idea | Riolu and Rockruff evolve based on the in-game clock (see best team). A useful tip that people will look up. |

---

## Research: best team (for L2)

The goal is a team that is available early, covers the endgame, and uses Mega Stones the game actually hands out.
The Mega Ring comes from beating Tsuji (`TradewindTown/scripts.inc:175`).

| Slot | Pick | Where / when | Why |
|------|------|--------------|-----|
| 1 | **Garchomp @ Garchompite** (Gible start) | Starter. Gabite at 24, Garchomp at 36. Garchompite is on the ground in Distortion World 5. | Ground STAB walls all of Volten's Electric gym. Dragon STAB covers dragon-heavy late bosses. Ground handles Metagross and Ceruledge on The Oni. **Tradeoff:** Gible takes 4× from Ice, so it sits out Red Fatality. |
| 2 | **Grimmsnarl** | Impidimp, Torii (11%). Morgrem at 32, Grimmsnarl at 42. | Dark/Fairy is the best single pick in the game. It beats Tsuji's and Vesper's ghosts (Dark resists Ghost). Fairy is the only type that hits Spiritomb super-effectively. It's immune to Psystrike, which matters for Armored Mewtwo. Fairy hits all of Mutrid Alpha's team. |
| 3 | **Lucario** | Riolu, Melting Mile (6%) or Walnut Woods (4%). Evolves with high friendship **when it isn't night**. | Useful from the first gym, where Riolu handles three Ice types. 4× on Draco's Sneasel, Edwards' Mega Tyranitar and White Tiger's Chien-Pao. Fighting beats White Tiger's Dark team. |
| 4 | **Houndoom @ Houndoominite** | Houndour, Ashlands (10%). Evolves at 24. Stone: buy the Bottled Warmth curio in Tradewind and trade it at the Pawnbroker after the Lantern Badge. | Fire/Dark hits both of Silas's Pokémon and Kiyo's Hellraiser (Dark). Fire covers Skarmory, Corviknight, Bronzong and Metagross. Flash Fire makes it immune to Chandelure's Fire moves. |
| 5 | **Lycanroc** | Rockruff, Route 2 (5%). Evolves at 25. Midday form if it isn't night, Midnight form at night. | The early-game MVP: Rock Throw beats Red Fatality's whole team. Late game, Rock does 4× to Suzaku's Talonflame, Moltres and Ho-Oh, 4× to Volcarona, and 4× to Tatsuko's Charizard. |
| 6 | **Luxray** | Shinx, Melting Mile (25%, the most common early catch). Luxio at 15, Luxray at 30. | Easy to get and useful all game. Beats Golbat at Torii, Skarmory, Corviknight, Blastoise, all the birds, and Mega Gyarados. Intimidate helps against physical bosses. |

**Swaps and honorable mentions**
- **Gardevoir @ Gardevoirite** (Ralts, Melting Mile 10%; stone from the Folded Goodbye curio). A second Fairy for the endgame dragons, Yiffer's Machamp and Ryujin. Swap in for Luxray after Act II.
- **Frigibax start:** Baxcalibur @ Baxcalibrite (Distortion World 4). Ice does 4× to Garchomp, Dragonite and Rayquaza. This is the best starter for the Ryujin double battle.
- **Axew start:** Haxorus has no Mega Stone here. Pair it with **Krookodile** (Sandile, Ashlands 10%) as the Ground answer to Volten.
- **Talonflame** (Fletchling, Melting Mile 20%) covers Edwards' Corviknight, Lucario and Falinks.

**Weak spots to mention honestly:** only one Mega per battle. The team has no Water type, so Walking Wake and Blastoise go to the Fairies and Luxray. Gible's early game is rough.

---

## Research: Mega Stones (for S21, L3)

| Stone | How |
|-------|-----|
| Mega Ring + Gengarite | Beat Tsuji (Lantern Badge) |
| Houndoominite, Banettite, Sablenite, Absolite, Gardevoirite, Mawilite, Blazikenite | Buy a curio at the Tradewind Curio Mart and trade it at the Pawnbroker (opens after the Lantern Badge) |
| Venusaurite, Charizardite X, Blastoisinite | Hidden lab behind the bookcase in the Tradewind apartment |
| Gyaradosite, Baxcalibrite, Garchompite | On the ground in Distortion World 3, 4 and 5 |

---

## Research: boss teams & counters (for L1)

Source: v1.2.4 `trainers.party` plus the map scripts. Counters are type-based **drafts**:
verify each one in-game before filming. Most bosses have no written moves, so they use
the last four moves they learned by level.

### Corrections to romhackguides.com (v1.2.3 guide)
- **The rival's starter mapping is wrong on the guide.** Actual:
  Gible → Draco has Axew (Fraxure at Torii, Haxorus in the lab) ·
  Frigibax → Gible (Gabite, Garchomp) · Axew → Frigibax (Arctibax, Baxcalibur).
- **Yiffer** (Munen Tunnel, v1.2.4, boss card) is missing: Zangoose 19, Zeraora 19, Machamp @ Black Belt 20, IV 20.
- **Ryujin is a double battle with Tatsuko** (Charizard 45, Dragapult 50, Dracovish 50, IV 28).
- **Star Summit ???** is scripted to be lost. **Armored Mewtwo** is Draco's post-game fight, and winning it gives you the Mewtwo.
- Not shown on the guide: Red Fatality has 2 Super Potions, The Oni has 2 Full Restores, White Tiger's Incineroar is shiny.

### Fights in story order

| Fight | Team (level, IV) | Counters (draft) | Call-outs |
|-------|------------------|------------------|-----------|
| Draco, Munen Village | His starter, Lv5 | Frigibax's Icy Wind/Glaciate do 4× to Gible. Gible starts with Thousand Arrows. | |
| Red Fatality, Frostwood | Snorunt 15, Sealeo 16, Snover 16, Froslass 17, Arcanine 18 @ Sitrus (IV 18–28) | Rockruff (Rock Throw on all four Fire/Ice), Riolu (not on Froslass), Psyduck or Wingull special attacks on Arcanine | Every starter is Dragon; Gible takes 4× from Ice. Arcanine has Intimidate. |
| Yiffer, Munen Tunnel | Zangoose 19, Zeraora 19, Machamp 20 @ Black Belt | Ralts/Kirlia (Machamp), Diglett, Drilbur or Gible (Zeraora), Riolu (Zangoose) | New in v1.2.4 |
| Draco, Torii | His dragon 21, Golbat 19, Sneasel 20 | Riolu (4× Sneasel, 2× Arctibax), Shinx (Golbat), Kirlia or Comfey (immune to Dragon) | |
| ???, Star Summit | Typhlosion 50, Feraligatr 50 | Meant to be lost | Short S14 |
| Tsuji, Tradewind | Drifblim 40, Cofagrigus 41, Mismagius 41, Chandelure 42, Sableye 44 @ Sablenite | **Grimmsnarl/Morgrem** (Dark/Fairy), Houndoom, Krookodile, Bisharp | Aftermath & Mummy punish contact moves. Flash Fire Chandelure. Mega Sableye's Magic Bounce reflects status moves. |
| Silas, Distortion World | Bronzong 35, Dusclops 36 (IV 24) | Houndoom (Fire + Dark hit both) | Levitate blocks Ground |
| Kiyo, Distortion World | Hellraiser 38 (Ghost/Fire, Levitate, IV 24) | Water, Rock, Dark (Krookodile, a Surf user) | Base 125 Sp. Atk, 105 Speed |
| Vesper, Distortion World | Mismagius 37, Spiritomb 38, Dusknoir 38, Golurk 39, Giratina 40 @ Sitrus (IV 26/31) | Grimmsnarl (Fairy is the only hit on Spiritomb) | Don't bring Electric vs Golurk. Golurk: Shadow Punch/EQ/Hammer Arm/Phantom Force |
| Draco, lab | Skarmory 38, Sneasel 38, ace 40 | Luxray/Houndoom (Skarmory), Lucario (Sneasel). Ace: Ice 4× Garchomp; Fairy/Ice/Dragon on Haxorus; Fighting/Rock/Steel on Baxcalibur | |
| General Edwards, Sennen | Tyranitar 41 @ Tyranitarite, Corviknight 36, Blastoise 38, Lucario 37, Volcarona 39, Falinks 35 (IV 24) | Lucario (4× Tyranitar), Luxray (Corviknight, Blastoise), Talonflame (Corviknight, Lucario, Falinks), any Rock (4× Volcarona) | Sand Stream chip |
| Volten, Shin-Tokyo Underground | Manectric 49, Magnezone 49, Luxray 50, Ampharos 51 @ Ampharosite (IV 31) | Garchomp, Excadrill, Krookodile | **Magnet Pull traps Steel types.** Leave Skarmory, Lucario and Bisharp home. |
| White Tiger, Tokyo Tower Street | Incineroar 44 (shiny), Chien-Pao 44, Absol 45 @ Absolite, Landorus-T 45, Meowscarada 45, **Shadow Raikou 46** (IV 26) | Lucario (4× Chien-Pao), Fairy moves, Ice (4× Landorus-T) | Snag Raikou |
| Suzaku, Tokyo Tower Mid | Talonflame 49, Altaria 49 @ Altarianite, Fezandipiti 50, Blaziken 50, **Shadow Moltres 50**, **Shadow Ho-Oh 51** (IV 28) | Rock (4× Talonflame, Moltres, Ho-Oh): Lycanroc, Tyranitar, Glimmora | Blaziken does 4× to Tyranitar. **Weaken and snag the birds, don't KO them.** |
| Mutrid Alpha, Tower Command | Gyarados 55 @ Gyaradosite, Dragonair 55 @ Eviolite, Walking Wake 55, Latios 56, Miraidon 56, **Shadow Salamence 57** (IV 31) | **Fairy hits all six** (Clefable, Gardevoir, Grimmsnarl) | Snag Salamence. Short S19. |
| The Oni, Tokyo Tower Top | Iron Valiant 58 @ Booster Energy, Grimmsnarl 57 @ Light Clay, Metagross 57 @ Weakness Policy, Ceruledge 57, Haxorus 58, Greninja-Bond 60 (IV 31, 2 Full Restores) | Fairy (Valiant, Grimmsnarl, Haxorus, Greninja), Ground (Metagross, Ceruledge) | Weakness Policy: only hit Metagross super-effectively if it's a knockout. Bring Brick Break for the screens. |
| Ryujin + Tatsuko, Ryuden Island (double) | Naganadel 50, Dragonite 55, Rayquaza 60 + Charizard 45, Dragapult 50, Dracovish 50 | Baxcalibur (4× Dragonite, Rayquaza), Rock (4× Charizard), Fairy (Dracovish, Dragapult) | Naganadel's Poison hits Fairy types. Rayquaza has EQ, Extreme Speed, Dragon Ascent and Dragon Dance. |
| Draco, post-game | Armored Mewtwo 80 (Psystrike, Aura Sphere, Shadow Ball, Recover; IV 31) | Grimmsnarl (immune to Psystrike, resists Shadow Ball), Krookodile | Winning gives you the Mewtwo |
