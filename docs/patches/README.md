# Patch files

The release patch lives here. The site serves it from this folder, so the
in-browser patcher works without the player leaving the page.

## Current patch

`wishes-of-tomorrow-1.2.6.bps` — 22,864,704 bytes.

| | CRC32 |
| --- | --- |
| Base ROM — Pokémon Emerald (U), 16 MB, `BPEE` | `1F1C08FB` |
| Patched output — 32 MB | `F1EEEA17` |

`wishes-of-tomorrow-1.2.4.bps` (output CRC32 `A8AA31E3`),
`wishes-of-tomorrow-1.2.3.bps` (output CRC32 `E5FDE65B`),
`wishes-of-tomorrow-1.2.2.bps` (output CRC32 `52866AB5`),
`wishes-of-tomorrow-1.2.1.bps` (output CRC32 `81AF35BA`),
`wishes-of-tomorrow-1.2.0.bps` (output CRC32 `8AA8F352`),
`wishes-of-tomorrow-1.1.0.bps` (output CRC32 `47253C41`) and
`wishes-of-tomorrow-1.0.0.bps` (output CRC32 `4056030C`) stay in this folder as
archived releases; their GitHub release assets keep working.

Every CRC above is read out of the patch's own 12-byte footer rather than
copied forward from the last release. The 1.2.3 bump copied 1.2.2's output
CRC into this table by hand and nobody noticed.

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
url: "patches/wishes-of-tomorrow-1.2.6.bps"
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

(v1.1.0 onward were generated with an equivalent in-repo Python BPS encoder,
`Tools/make_bps_patch.py <version>`, and verified by independently re-applying the patch with a separate
applier written from the spec: the output is byte-identical to the built ROM
and all three embedded CRCs check.)

- **Base ROM** — Pokémon Emerald (U), 16 MB, header code `BPEE`.
- **Built ROM** — `pokeemerald-expansion/pokeemerald.gba`, 32 MB after expansion.
- **Format** — BPS. It stores a CRC32 of the base ROM, which is what lets the
  patcher tell a player they've got the wrong dump instead of handing them a
  broken file. UPS also carries checksums; IPS carries none, so prefer BPS.

Flips: <https://github.com/Alcaro/Flips>

## Update patches (`updates/`)

The site's **Update my game** tab turns a player's current WoT ROM straight
into the latest one, so they don't need their base ROM and the file keeps its
name (and so its save). Each `updates/wot-<old>-to-<new>.bps` is the diff from
one archived release's output to the new release's output, about 2–3 MB.

```sh
python Tools/make_update_patches.py <new_version>        # every older release
python Tools/make_update_patches.py <new_version> --keep 4   # only the last 4
```

The script rebuilds every release in memory from the base ROM and the patches
in this folder (the new release too, never the build tree), re-applies each
update patch with the independent applier and requires a byte-exact match. It
writes `updates/manifest.json` (CRC32 of each old release → its patch, CRCs
read from patch footers) and deletes update patches that target an older
release. Releases it doesn't cover fall back to the base-ROM route on the page.

## After building a new patch

1. Drop the `.bps` in this folder, then run `Tools/make_update_patches.py <version>`.
2. Update `url` in `assets/patcher.js` and `RELEASE` in `assets/site.js`.
3. Add a changelog entry in `index.html` (search for `data-changelog`).
4. Attach the same `.bps` to the GitHub release so the release page matches.
