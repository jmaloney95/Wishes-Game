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

To publish a patch, see [`patches/README.md`](patches/README.md).

## Updating for a new release

Three places, all obvious:

| What | Where |
| --- | --- |
| Version + date on the page | `RELEASE` at the top of `assets/site.js` |
| Patch filename | `PATCH.url` at the top of `assets/patcher.js` |
| Changelog entry | `index.html`, search `data-changelog` |

The GitHub buttons point at `/releases/latest` and never need editing.

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
