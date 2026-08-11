# Pokémon: Wishes of Tomorrow — Act 3 Canon
### "City of Tomorrow" — Shin-Tokyo & the Archipelago

**Status:** Canon as of this document. Items marked **[OPEN]** await co-director review.
**Purpose:** Portable context document. Hand this to a new chat or to Claude Code for implementation work.

---

## 1. Theme & Tone

The player arrives at the most advanced city in the region. Everything looks perfect — neon skyscrapers, drones, holographic ads, maglev trains, artificial weather. Team Mutrid already controls it and the citizens don't know, or won't say.

Shin-Tokyo is an **island city**, reached by Shinkansen at the end of Act 2. A surfable archipelago sits off its coast. The city is the seat of Mutrid power; the islands are where the region's resistance survives in exile.

Act 3 is an **infiltration act**, not a stealth gauntlet. The city is a hostile-state hub with two designed stealth set-pieces; the archipelago is the pressure release between them.

**Opening staging:** A train enters the city in the rain. The player remembers the allies lost at Sennen station. No music — only train sounds. The player steps off. No welcoming NPCs. The city feels wrong.

---

## 2. Core Systems Active in Act 3

### 2.1 Enemy Class / Disguise System
- **Goons** — fooled by the carved mask (Save Face, Act 1 hidden side quest). Masked, they ignore the player. Unmasked, they actively path toward the player and initiate battle.
- **Officers** — see through the carved mask. Their encounters fire regardless of disguise state.
- **Backup disguise:** a black market vendor in the Neon District sells a mask for players who skipped Save Face. Never mandatory, never hard-locking.
- The mask becomes **permanently useless at red alert** (Step 5). It is removed on-screen, not quietly expired.

### 2.2 Snag & Purification Loop
Taught by the defector at the rebel hideout (Step 7).

1. **Snag** a Shadow Pokémon from a Mutrid trainer in the city (Snag Machine, obtained in the jailbreak).
2. **Battle with it at least once** — this "opens" the Shadow. Single boolean flag per Pokémon; no heart gauge, no step counting.
3. **Surf to Celebi island**, clear/enter the shrine, **purify instantly**.

Power tiers (established canon): Shadow ≈ 20% stronger than Mega → Purified > Shadow → **Mega + Purified = endgame ceiling.**

Mutrid patrols must respawn for the loop to sustain (Expansion rematch infrastructure, or alert-state patrols as regenerating encounters).

### 2.3 The Research Flag (from Act 2's "His Life's Work")
Clarkson's research is the blueprint for Shadow conversion. Mutrid has corruption but not **control** — without the research they can shadow-ify weak Pokémon only. With it, pseudo-legendaries and high-power species stop resisting the process.

Three states carried into Act 3:

| State | Act 3 Enemy Tier | Clarkson's Voice |
|---|---|---|
| **Destroyed** | Capped — moderate Shadows only | State A (catastrophe averted) |
| **Kept, secure** | Capped — identical to destroyed | State B (dread of what it enables) |
| **Kept, then stolen** | **Escalated** — pseudo-legendary Shadows appear | State C (grief) |

Only two mechanical tiers to build: **capped** and **escalated**.

The **"His Life's Work" quest never completes** on any kept path. It sits unfinished in the log permanently. Snagging back the Shadows the research created does not close it — the knowledge stays loose.

---

## 3. Act 3 Sequence

### Step 1 — Arrival: Neon District (free roam, normal state)
No cutscene lock. The player explores street level and absorbs the wrongness independently.

**Available:**
- **Volten's Gym — Badge 5.** Electric. Still operating because Mutrid tolerates it as propaganda: proof that life is normal. Volten is charismatic, young, and secretly helping civilians escape. Puzzle: rotating laser mirrors to restore power. Team: Rotom-Wash, Bellibolt, Magnezone, Iron Hands, Luxray, **Ace: Mega Ampharos.** Post-victory line: *"You don't need strength to save the world. You need hope."*
- **Black market vendor** (replaces department store — that was Act 2). Inflated prices, scarce stock. Also sells the backup mask.
- **Battle Café** — repeatable battles for XP farming.
- **Subway**, **Missing Scientist** side quest, **Battle Veteran**, **in-game trades**, **Underground Arena**.
- Flatfoot Games arcade cabinet cameo.
- **NO Game Corner** (Act 2), **NO Department Store** (Act 2), **NO Pokémon Center**, **NO Hotel**.

**Environmental storytelling:** brainwashed crowds, propaganda screens, cheerful-and-wrong NPC one-liners. Citizens mention people disappearing, strong Pokémon turning violent, power outages, official denial.

**Building screen** displays the Mutrid Leader's face.

**Water is visible and locked.** Surf is unlocked (Distortion Badge, #4) but tides are too high. Island silhouettes on the horizon. A sign or NPC ties the high water to the tower drawing power. Goal visible, goal locked.

The mask/goon class system teaches itself here in safety.

---

### Step 2 — The Research Theft *(kept path only)*
Fires at an exploration trigger during free roam — not at the station door.

**The thief is the Mutrid Captain** (Layer 2 gym leader, Badge 8). She/he appears at street level as a public show of authority — an inspection, an authoritarian spectacle beat consistent with the city's tone. **Officer class: sees through the carved mask.** The player is stopped, the research is taken, and the Captain leaves.

Design notes:
- **Not Draco.** Draco already attempted the theft in Act 2 and failed; repeating him weakens both beats.
- Using the Captain makes **Badge 8 personal** — the player climbs to Layer 2 to face the person who took Clarkson's life's work.
- The theft is by **authority, not by force.** No forced loss, no unwinnable battle. Being outranked in an occupied city is its own kind of powerlessness.
- Flips the research flag to **stolen**. Gengar State C dialogue begins here. All downstream Shadow pools escalate.

**On the destroy path:** the Captain still stops the player, finds nothing, and moves on. Small, quiet satisfaction.

---

### Step 3 — Minister's Call
The prison is inaccessible until this fires. **Minister** (IT specialist, breached the train at the end of Act 2) calls: **Mikmanc has been compromised** and is holding the Snag Machine. Mutrid will find it.

Minister distracts the front-door guards remotely. The door opens. Main quest triggers, prison marked on map.

A call funnels the player without trapping them in a scene, and pays off Minister as a returning ally.

---

### Step 4 — The Dark Jailbreak (stealth set-piece #1)

**Concept:** near-total darkness. The player has no light. Patrolling guards each carry a Flash-style light radius — the only illumination on the map. The player navigates *by* the light of the things hunting them.

**Phase 1 — Infiltrate.** Cross the dark room to the cell block, timing movement against patrol routes. **Whiteout → respawn at map entry.**

**Phase 2 — Cell / Checkpoint.** Free **Mikmanc**. He has kept the Snag Machine hidden on his person through captivity — that's why Mutrid hasn't weaponized it, and why this rescue specifically matters. **The Snag Machine is handed over HERE**, at the cell. Checkpoint set.

**Phase 3 — Escort out.** A second exit, a fresh set of goons. **Double battles with Mikmanc as the player's partner** — mechanically expresses the escort, justifies the doubles format. These fights are the **snagging tutorial**: the player tests the Snag Machine on the goons' Shadow Pokémon. **Whiteout → respawn at cell checkpoint.**

**Resolution:** Mikmanc flees to safety. The partner slot goes empty.

**Design rules:**
- The **carved mask is irrelevant here** — the mechanic is light and timing, not disguise. Players who skipped Save Face are not disadvantaged in the one sequence that would punish them hardest, and the two stealth systems stay mechanically distinct.
- The Snag Machine is given at the cell so it can **never be lost.** The escort stakes are Mikmanc's safety, not the device.
- Level 40+ at this point. Difficulty should be flavor, not a wall — 1–2 attempts.
- **Research flag sets the Shadow tier** for every battle in this section.

**Technical spec (for Claude Code):**
- Darkened palette state applied on map entry (cave-without-Flash mechanism, scripted palette fade).
- Each guard carries a bright radial-gradient OBJ light sprite moving with them via the same `applymovement` driving their patrol loop.
- Detection = **coordinate check** against guard position, radius-based (all directions), not a forward-facing trainer cone.
- **Brief full reveal on map entry** (map lights for ~1 second, then goes dark) so the player gets one mental snapshot to plan from. Prevents "can't route what you can't see" frustration.
- Two checkpoints: map entry, and cell.
- GBA hardware window lighting is the visually truest option but too expensive for multiple simultaneous guards. Do not use.

---

### Step 5 — Nightfall & Red Alert *(permanent state change)*
On completing the jailbreak, the map **skips to night**. No clock-based tide system — a single scripted transition.

**Changes:**
- The city is **permanently hostile.** Every enemy engages regardless of disguise.
- The **carved mask becomes useless.**
- The **building screen swaps the Leader's face for the player's sprite and a bounty.** *(Technical: conditional graphic matching the player's chosen sprite, not one static image.)* This is the causal reason the mask dies — they know the player's face now.
- **Volten's gym is shut down** and he goes underground. The player's jailbreak cost him his cover. He reappears at the finale evacuation. **[OPEN]** — consider a hideout cameo mid-act to keep him present.
- **Tides drop.** Surf access to the archipelago becomes available (opens formally at Step 7 with the map).

---

### Step 6 — The Arrest
An **officer** stops the player in the alerted city and announces they'll escort the prisoner personally. Goons defer.

**She strips the carved mask herself.** This is the Save Face payoff — a demonstration, not a quiet expiry. The officer class rule the player learned in Step 1 is proven at their own expense.

In private, the **Zoroark illusion drops.** She is a hooded young woman with Zoroark-like hair, a **street operative** embedded inside Mutrid, working for the rebellion. She was hunting whoever pulled off the prison break; the manhunt is how she found them.

She brings the player to the hideout.

**She is NOT the defector** — two distinct characters.

---

### Step 7 — The Rebel Hideout *(safe hub)*
- **Blissey heal + Porygon PC box.** This is the only heal/box in Shin-Tokyo.
- **The defector** — a former Clarkson lab assistant, recruited by the Leader with a false-legacy pitch ("we'll build the world my father was too timid to build"), turned when they saw Jirachi corrupted. They explain **shadow science**, teach the **snag → battle → purify loop**, and give the purification quest.
- **The updated map** is handed over here: a new Shin-Tokyo region map showing the archipelago. The islands were invisible on the mainland map. The map itself is the reveal beat.
- Tides are down. **Surf opens.**

---

### Step 8 — The Loop (city ↔ archipelago)
Core gameplay for the middle of the act. Snag in the city, purify on the islands, return stronger, build toward Layer 2.

**City snag grounds (red alert):** office towers, subway tunnels, research labs, power plant.

**Systematized side content:**
- **Shadow Rescue Patrol** — city trainers using Shadows. Defeat, snag, purify. Accrues **Purification Points**. Rewards: Ability Patch, Gold Bottle Cap, Ability Machines, Paws. *(Master Ball removed from this reward pool — see Step 9.)*
- **Power Grid** — three corrupted substations inside the power plant. Shadow Magnezone, Shadow Electivire, Shadow Rotom. Framed as a rebellion objective, not a civic favor. Restoring power aids the resistance.
- **Underground Arena** — five consecutive battles, illegal Mutrid-run. Final opponent uses **Shadow Tyranitar** — **research-flag gated, only exists on the stolen path.** Rewards: Choice Specs / Band / Scarf.
- **Battle Veteran** — difficult repeatable battles. Gifts Life Orb or Expert Belt.
- **Missing Scientist** — a scientist's daughter asks for help; her father was forced to work on Shadow production. Rewards: TM Nasty Plot, upgrade items, Rare Candies. **Research notes REMOVED** (duplicates the defector's exposition). **[OPEN]** — flag variant where, on the *destroy* path, he was kidnapped specifically because Mutrid lacks Clarkson's blueprints and needs them reconstructed. Quietly reinforces that the choice mattered.
- **In-game trades:** Alolan Sandslash → Incineroar · Absol → Mamoswine · Golduck → Excadrill · Arcanine → Milotic · any shiny → Greninja (Battle Bond).

**The archipelago:**

| Island | Content |
|---|---|
| **Celebi Island** (closest) | Fortree/Pacifidlog hybrid — treehouses and rope bridges over shallow water, log-raft platforms. Sanctuary aesthetic, organic, the visual opposite of Shin-Tokyo's steel. **Badge 6** in the town. Uxie/Mesprit/Azelf trial gates the purification forest; **Celebi shrine** beyond it. |
| **Signal / Lighthouse Island** | **Badge 7.** Defeating the exiled leader is what lights the signal, coordinating the rebellion. |
| **Prototype Island** | Half-sunk offshore Mutrid outpost — the *first* lab, before Munen. **Deoxys** as the failed "perfect form" prototype. Environmental storytelling via logs and cracked containment. Post-game snag target that resists purification longest. |
| **Cannibal Island** | Tiny, purely comedic. Refined Hannibal-Lecter-styled NPC who eats Togepi eggs. Deadpan, elaborate table setting, chillingly polite. Content implied, never shown. Uses the approved delayed-textbox gag. Zero plot load — tonal whiplash stays contained. |

**Exiled gym leaders:** The mainland leaders fled to the islands when Mutrid took the region. The archipelago is the resistance-in-exile. Each defeated leader **joins the tower assault** — badges become the rebellion roster. Distance from shore doubles as the difficulty curve.

**The trio trial (Uxie / Mesprit / Azelf):** knowledge, emotion, willpower — the components of a wish. They gate the Celebi shrine. They test **consistency**, not correctness — no path is judged right. Uxie responds to the destroy-or-keep-research choice; Mesprit to Grandfather's Last Wish; Azelf to the Act 2 backward march through the occupied wastelands. Not catchable. **[OPEN]** — impacts endgame mechanic gating.

---

### Step 9 — Layer 2: The Mutrid Captain
**Badge 8.** Awards the **Master Ball.** Timing is deliberate — the final tool arrives immediately before the finale.

On the stolen path, this is the player facing the person who took Clarkson's research at street level in Step 2.

---

### Step 10 — Team Mutrid HQ *(floors above Layer 2)*
Not underground. The HQ occupies the floors directly above the Captain's gym, so the entire act reads as **one continuous ascent from street level to summit** and the "layers" stop being discrete zones.

**Sections:** Genetics Lab · AI Control Center · Shadow Chamber · Prototype Facility · Executive Offices.

**Admin battles throughout:** three normal Pokémon + three Shadows. Pool: Shadow Garchomp, Shadow Scizor, Shadow Salamence, Shadow Chandelure, Shadow Metagross.

**Three Executives, each with a Shadow Titan.** Each defeat disables part of the Shadow Generator.
- Executive 1 — **Shadow Goodra**
- Executive 2 — **Shadow Hydreigon**
- Executive 3 — **Shadow Kommo-o** or **Shadow Dragapult** *(avoid Metagross — already in the admin pool)*

---

### Step 11 — Tower Summit
**No disguise, no Zoroark illusion.** The player is wanted, publicly, with a bounty on a building-sized screen. They walk in as themselves. That is the point.

**The chamber:** **Shadow Jirachi** suspended in machinery, barely awake, powering the machine. Cannot battle.

The **Mutrid Leader** waits calmly beside it. He genuinely believes he is saving humanity.

**His argument:** People create war, greed, hatred, jealousy. Pokémon only amplify human weakness. Remove free will through Shadow conversion and the world finally achieves peace. This lands on the **Order/Chaos axis** — his position is Order taken to its end. **The player never responds with words. Only by battling.**

**The reckoning:** Mega Gengar. The father-son confrontation. **Music cuts to silence.** The signature staging moment of the game.

**Leader's team:** Shadow Houndoom, Hisuian Zoroark *(a deliberate mirror of the player's operative — keep this on purpose)*, Porygon-Z, Iron Valiant, Shadow Skarmory.
**Ace: Shadow Slaking** — custom ability **Unrelenting**: never skips turns, cannot have Attack lowered, ignores Intimidate. No Truant. A relentless physical wallbreaker embodying Mutrid's philosophy.

---

### Step 12 — Finale: Mend, Not Smash

**The machine is never destroyed.** The player uses the **Master Ball to snag Shadow Jirachi** — freeing it, not killing it. This honors Clarkson and pays off the destroy-or-keep-research choice thematically.

**The Shadow Core** was designed to run on a living soul of immense energy. Jirachi was forced to fill that role. The instant Jirachi is disconnected, the reactor enters catastrophic meltdown. Without a soul to stabilize it, the city is destroyed.

Clarkson scans the machine: *"Without a soul to stabilize it for a few minutes... this entire city disappears."*

The player moves to reconnect Jirachi. **Clarkson stops them: "No. It ends today."**

Because his soul is already detached from a human body and inhabiting Gengar, **he is the only other being capable of interfacing with the machine.** He steps into the chamber. The machine locks around him. The reactor stabilizes. The alarms stop. For a moment, everything is quiet.

**The Gengar resolution:** The **shiny Gengar remains in the player's party.** Still Mega-capable. Still theirs. **It never speaks again.** The voice that has accompanied the player since Act 2 simply stops. No mechanical cost to the player, maximum emotional cost — and it gives the post-game a reason to exist beyond collection.

**The escape:** descent through the collapsing tower carrying weakened Shadow Jirachi. Wild Shadow Pokémon attack while fleeing; the player must survive.

**Final corridor — Draco** as the Team Mutrid rival. His Act 2 flight to Shin-Tokyo pays off here. Team: starter, Shadow Staraptor, Shadow Gallade, Shadow Avalugg, Shadow Sneasler, Shadow Krookodile.

**Outside:** gym leaders and allies evacuating civilians. **Volten among them.** Allies are revived. The player reaches safety still carrying **Shadow Jirachi — unpurified.**

---

### Step 13 — The Capstone: Purifying Jirachi
The act does not end at the tower. The player must **surf back to Celebi island and purify Shadow Jirachi at the shrine.**

This is the true ending beat and the ultimate expression of the loop the whole act taught: the same three-step process used on every snagged Shadow, performed one last time on the being the entire story has been about. The wish is mended, not smashed. The region's thaw resolves.

---

### Step 14 — Post-Game
Freeing Jirachi and stabilizing the core scattered **Shadow legendaries** across the region.

Hunt and purify them all. **Purifying the complete set frees Clarkson's soul from the core — and the voice comes back.**

This converts a completionist checklist into the last thing he asked of the player. The deeper archipelago and the remaining islands hold the hunt. Deoxys on the prototype island is the hardest purification in the game.

---

## 4. Clarkson / Gengar Voice — Dialogue States

Short, telepathic-feeling interjections. Clarkson cannot speak normally in a Gengar body — the voice reads as thought pressed into the player's head. Clipped, uncanny, a father's cadence through a ghost's medium. GBA-length lines. Fires on map entry, before key battles, and at save points.

**Recurring motif:** *hands.* Clarkson cannot act — destroying, keeping, and protecting all route through the player's hands instead of his.

> **Note:** These lines are approved in *sentiment* and still need a polish pass on wording.

### State A — Research Destroyed (catastrophe averted)
- *On destroying:* "…Good. Without it, they'll never break the strong ones. It ends here."
- *Entering occupied map:* "They can shadow the weak. Never the giants. That door is shut now — because of you."
- *Before a hard Shadow battle:* "Whatever they throw at you, it has limits. I made sure of that when I let it burn."
- *Save point:* "I spent my life on those pages. Ending them was the only good use left in me."
- *Approaching the tower:* "His machine will run on what he has. Not on what he wanted. You saw to that."

### State B — Research Kept, Secure (dread of what it enables)
- *On keeping:* "…Then guard it with everything. You're holding the key to every monster he can't yet make."
- *Entering occupied map:* "If this reaches him, the strong ones fall too. Tyranitar. Salamence. Nothing would resist."
- *Before a hard Shadow battle:* "This is nothing to what he'd field with my work. Pray he never gets it."
- *Save point:* "I know why you kept it. The power's real. Just… don't let that be the reason he wins."
- *Approaching the tower:* "He needs my research to finish the machine. It's one room away from him. Don't let it close that gap."

### State C — Research Kept, Then Stolen (grief withheld)
- *After the theft:* "…It's gone. After everything. It's in his hands now."
- *First map after theft:* "Every Shadow you meet from here — that's mine. My work, wearing their faces."
- *Encountering an escalated Shadow:* "I made that possible. Snag it. Take back what I let loose."
- *After snagging one back:* "One saved. It doesn't undo it. But it's one."
- *Save point:* "I asked you to destroy it. I should have asked harder."
  *(alt, colder — optional, may be too harsh:)* "I won't blame you. …I want to. But I won't."
- *Approaching the tower:* "He has my work and my death both. Go up there and leave him nothing else."
- *Tower-top, into the music cut:* "My son. My research. Both turned. …Finish this."

**Mikmanc, stolen path only (during the jailbreak):** *"These aren't normal for them… someone gave them the key."*

---

## 5. Badge Distribution (Act 3)

| # | Badge | Location | Leader |
|---|---|---|---|
| 5 | — | Neon District, Shin-Tokyo | **Volten** (Electric, Mega Ampharos) — tolerated as propaganda, shut down at red alert |
| 6 | — | Celebi Island town | Exiled mainland leader **[OPEN — assign]** |
| 7 | — | Signal / Lighthouse Island | Exiled mainland leader **[OPEN — assign]** |
| 8 | — | Layer 2, Shin-Tokyo | **Mutrid Captain** — awards Master Ball; the research thief on the stolen path |

Badges 1–4 established in Acts 1–2 (Ember/Frostwood/Red Fatality; Lantern/Tradewind/Madam Tsuji; Distortion #4 grants Surf).

---

## 6. Cast Reference (Act 3)

| Character | Role |
|---|---|
| **Professor Clarkson** | Murdered in Act 1 by his own son. Soul inhabits a shiny Gengar. Mega-capable via Gengarite. Sacrifices himself to the Shadow Core; goes silent. |
| **The Mutrid Leader** | Clarkson's son. Believes removing free will brings peace. Tower summit. |
| **The Mutrid Captain** | Layer 2 gym, Badge 8, Master Ball. Steals the research at street level on the kept path. |
| **The Zoroark Operative** | Hooded young woman, Zoroark-like hair. Street operative embedded in Mutrid as an officer. Arrests the player, strips the mask, brings them to the hideout. |
| **The Defector** | Former Clarkson lab assistant, recruited via false-legacy pitch. Academic. At the hideout — teaches shadow science and the purification loop. |
| **Mikmanc** | Captured ally holding the Snag Machine. Freed in the jailbreak, fights as doubles partner, then flees to safety. |
| **Minister** | IT specialist from the Act 2 train breach. Calls with the Mikmanc intel, remotely distracts the prison door guards. |
| **Volten** | Neon District gym leader, Badge 5. Secretly evacuating civilians. Exposed and shut down at red alert. Reappears at the finale evacuation. |
| **Draco** | Failed to steal the research in Act 2, fled to Shin-Tokyo. Returns as the rival in the final escape corridor. |

---

## 7. Open Items

1. **[OPEN]** No heal or PC box exists between Step 1 (arrival) and Step 7 (hideout) — this window includes Volten's gym and the entire dark jailbreak. Hotel was cut for scope. Needs a resolution: black market healing items only, a single rebel-contact NPC heal in the Neon District, or accept the gap as intentional difficulty.
2. **[OPEN]** Assign the two exiled gym leaders (badges 6 and 7) — identities, types, and which mainland town each fled from.
3. **[OPEN]** Volten cameo at the hideout between his gym closing and the finale evacuation?
4. **[OPEN]** Missing Scientist destroy-path flag variant.
5. **[OPEN]** Trio trial gating of Celebi purification — impacts endgame mechanics, needs co-director sign-off.
6. **[OPEN]** Deoxys as failed prototype in the Munen lab / prototype island — impacts established Act 2 beats, needs co-director sign-off.
7. **[OPEN]** Gengar dialogue lines approved in sentiment, wording still needs a polish pass.
8. **[OPEN]** Does the red alert state need any decay, or is permanent hostility final? *(Current canon: permanent.)*
