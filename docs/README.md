# Flatfoot Games / Flatfoot Labs — site

Static site, no build step, no dependencies. GitHub Pages serves this folder.

```
docs/
├── index.html                Wishes of Tomorrow — the landing page
├── labs.html                 Flatfoot Labs — the studio hub
├── .nojekyll                 stops Pages running Jekyll over it
├── patches/                  release patch files (see patches/README.md)
└── assets/
    ├── styles.css            Nocturne tokens + both pages' styles
    ├── site.js               starfield, reveal-on-scroll, tilt, cast rail
    ├── patcher.js            in-browser BPS/UPS/IPS patcher
    ├── art/                  cast portraits + favicon
    └── media/                title screen, town map, battle backdrops
```

`index.html` is the entry point — visitors land on the game. `labs.html` is the
studio hub; the "← Flatfoot Labs" link in the header goes back to it.

## Publishing

Pages can only serve from the repo root or `/docs`, which is why the site lives
here. Once this is committed and pushed to `main`:

**Settings → Pages → Source: Deploy from a branch → `main` / `/docs`**

The site then appears at `https://jmaloney95.github.io/Wishes-Game/`.

`Wishes-Game` is currently **private**, so Pages needs a paid plan and every
GitHub link on the site 404s for visitors. Make the repo public before
announcing.

## The patcher

`assets/patcher.js` applies BPS, UPS and IPS patches in the browser. The
player's ROM is read with the File API, patched in memory, and returned via a
blob URL — it is never uploaded, and the site has no server that could receive
it. That is the point: we distribute a patch, never a built ROM.

BPS and UPS both embed a CRC32 of the base ROM they were built against, so a
player with the wrong region or revision gets a clear error rather than a
broken file. IPS carries no checksums, so only the GBA header is sanity-checked.

The engine is also exposed as `window.WoTPatcher` for testing against known
fixtures.

## Play in browser

After a successful patch, a **Play in browser** button boots the patched bytes
straight from memory — nothing is re-downloaded and the ROM still never leaves
the machine.

The emulator is [EmulatorJS](https://emulatorjs.org) (mGBA core), loaded from
its CDN **only when the button is pressed** — about 1.5 MB, so visitors who
just want the patch never pay for it. Keyboard on desktop, on-screen pad on
touch devices, both handled by EmulatorJS. Saves and save states live in the
browser's own storage.

This is the site's one third-party runtime dependency. If the CDN is
unreachable the button reports it and the patch download is unaffected. To
drop the dependency, vendor `emulator.min.js`, `emulator.css` and the mGBA
core into `assets/` and point `PLAYER.data` in `patcher.js` at them.

To publish a patch, see [`patches/README.md`](patches/README.md).

## Updating for a new release

Four places:

| What | Where |
| --- | --- |
| Version + date on the page | `RELEASE` at the top of `assets/site.js` |
| Patch filename the patcher loads | `PATCH.url` at the top of `assets/patcher.js` |
| Patch download button | `data-patch-link` in `index.html` — **version-pinned**, see below |
| Changelog entry | `index.html`, search `data-changelog` |
| Asset cache stamp | `?v=` on the css/js tags in both HTML files — bump on any `assets/` change |

That last one matters: Pages serves `assets/` with a four-hour `max-age`, so
without a fresh stamp a returning visitor can load new HTML against cached old
JS. Editing a script without bumping it is how the counter renders empty.

"Release notes" and "Source & issues" point at `/releases/latest` and the repo,
so they never need editing.

## The download counter

There is no server here, so the number on the download panel is GitHub's own
tally of release-asset downloads, read from the public API
(`/repos/:owner/:repo/releases`, no auth, CORS-enabled, 60 requests an hour per
visitor, cached in `localStorage` for ten minutes). It sums every `.bps` asset
across every release, so it keeps climbing when a new version ships.

**This is why the download button points at the release asset and not at
`patches/…`.** GitHub only counts the former. Repointing that button at the
local copy would peg the counter at zero for ever — which is also why the
button URL is pinned to a specific version and has to be updated per release.

Two things the number does not include: people who use the in-page patcher
(it reads the same-origin copy, because the release asset host sends no CORS
headers), and repeat downloads GitHub de-duplicates. It's a floor, not a click
count. If the API call fails, the chip falls back to showing the version.

## Local preview

```sh
python -m http.server 8000 --directory docs
```

Then open <http://localhost:8000>. Use a server rather than opening the file
directly — the patcher fetches the patch over HTTP.

## Art pipeline

Cast portraits come from `custom sprites/Key Trainer Sprites/` with the magenta
chroma-key background flood-filled to transparency, resized to 560×560, saved
as WebP. Media tiles are cropped from the sheets in `previews/`. Both were
produced by throwaway scripts — the files in `assets/` are the artefact to
keep.
