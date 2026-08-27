# Patch files

The release patch lives here. The site serves it from this folder, so the
in-browser patcher works without the player leaving the page.

## Current patch

`wishes-of-tomorrow-1.2.0.bps` — 22,709,339 bytes.

| | CRC32 |
| --- | --- |
| Base ROM — Pokémon Emerald (U), 16 MB, `BPEE` | `1F1C08FB` |
| Patched output — 32 MB | `8AA8F352` |

`wishes-of-tomorrow-1.1.0.bps` (output CRC32 `47253C41`) and
`wishes-of-tomorrow-1.0.0.bps` (output CRC32 `4056030C`) stay in this folder as
archived releases; their GitHub release assets keep working.

Both checksums are embedded in the patch, so the patcher rejects a wrong base
ROM before doing any work. Verified byte-exact against the built ROM.

The page does **not** download this file on load — it sends a HEAD request for
the size and fetches the bytes only when the player presses Apply.

**Only patches belong here. Never a built `.gba`** — that would distribute the
game itself. `.gitignore` blocks `docs/**/*.gba` as a backstop, and neither the
base ROM nor the built ROM is in this repo.

## Expected filename

`assets/patcher.js` looks for exactly one path:

```js
url: "patches/wishes-of-tomorrow-1.2.0.bps"
```

Name the file to match, or change that line. If the file is absent the page
degrades gracefully: the patcher asks the player to choose a patch file, and
the "Patch file" button falls back to the GitHub releases page.

## Creating the patch

The patch is the difference between a clean base ROM and the built ROM. Both
files stay on your machine; only the patch is published.

```sh
flips --create --bps \
  "Pokemon Emerald (U).gba" \
  "pokeemerald-expansion/pokeemerald.gba" \
  "docs/patches/wishes-of-tomorrow-<version>.bps"
```

(v1.1.0 and v1.2.0 were generated with an equivalent in-repo Python BPS
encoder and verified by independently re-applying the patch with a separate
applier written from the spec: the output is byte-identical to the built ROM
and all three embedded CRCs check.)

- **Base ROM** — Pokémon Emerald (U), 16 MB, header code `BPEE`.
- **Built ROM** — `pokeemerald-expansion/pokeemerald.gba`, 32 MB after expansion.
- **Format** — BPS. It stores a CRC32 of the base ROM, which is what lets the
  patcher tell a player they've got the wrong dump instead of handing them a
  broken file. UPS also carries checksums; IPS carries none, so prefer BPS.

Flips: <https://github.com/Alcaro/Flips>

## After building a new patch

1. Drop the `.bps` in this folder.
2. Update `url` in `assets/patcher.js` and `RELEASE` in `assets/site.js`.
3. Add a changelog entry in `index.html` (search for `data-changelog`).
4. Attach the same `.bps` to the GitHub release so the release page matches.
