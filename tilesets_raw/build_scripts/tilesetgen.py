"""Generic RPG-Maker/sheet -> pokeemerald secondary tileset converter.

Handles:
  * source sheets drawn at 2x (RMXP style) -> `half=True` downscales losslessly
  * magenta / alpha transparency
  * grid-aligned terrain extraction and free-floating object extraction
  * 5-bit colour snapping, palette packing into <=7 x 15 colours
  * 8x8 tile dedup with X/Y flips, 512-tile budget
  * two-layer metatiles (base fill under transparent object art)

Outputs tiles.png + metatiles.bin + metatile_attributes.bin + palettes/NN.pal,
which the pokeemerald makefile turns into .4bpp.lz / .gbapal.
"""
import os, struct, math
from PIL import Image

TRANS = (-1, -1, -1)
MAGENTA = (248, 0, 248)

NUM_TILES_IN_PRIMARY = 512
MAX_SECONDARY_TILES = 512
MAX_SECONDARY_METATILES = 512
SEC_PAL_SLOTS = list(range(6, 13))       # palettes 6..12 belong to the secondary
MAX_PALS = len(SEC_PAL_SLOTS)
COLORS_PER_PAL = 15                      # index 0 is transparent

LAYER_NORMAL = 0x0000                    # bottom->BG2, top->BG1 (art draws over player)
LAYER_COVERED = 0x1000                   # bottom->BG3, top->BG2 (art draws under player)


def snap(c):
    return ((c[0] >> 3) << 3, (c[1] >> 3) << 3, (c[2] >> 3) << 3)


class Sheet:
    """A source image, addressed in *destination* pixels (post-downscale)."""

    def __init__(self, path, half=False, shift=(0, 0), magenta_key=True,
                 white_key=False, extra_keys=()):
        im = Image.open(path).convert("RGBA")
        if shift != (0, 0):
            dx, dy = shift
            nim = Image.new("RGBA", im.size, (0, 0, 0, 0))
            nim.paste(im.crop((max(0, -dx), max(0, -dy), im.width, im.height)),
                      (max(0, dx), max(0, dy)))
            im = nim
        if half:
            # Majority-vote downsample: for 2x-drawn art whose 2x2 blocks are
            # not perfectly uniform (stray-pixel texture), NEAREST keeps the
            # top-left pixel and produces crunchy noise. Taking the block's
            # majority colour recovers the intended 1x pixel; ties fall back
            # to the top-left.
            W2, H2 = im.width // 2, im.height // 2
            nim = Image.new("RGBA", (W2, H2))
            sp, dp = im.load(), nim.load()
            for y in range(H2):
                for x in range(W2):
                    q = [sp[2 * x, 2 * y], sp[2 * x + 1, 2 * y],
                         sp[2 * x, 2 * y + 1], sp[2 * x + 1, 2 * y + 1]]
                    tl = q[0]
                    if q.count(tl) >= 2:
                        dp[x, y] = tl
                    else:
                        dp[x, y] = max(q, key=q.count)
            im = nim
        self.im = im
        self.px = im.load()
        self.w, self.h = im.size
        self.keys = set()
        if magenta_key:
            self.keys |= {(248, 0, 248), (255, 0, 255)}
        if white_key:
            self.keys |= {(255, 255, 255), (248, 248, 248)}
        self.keys |= {snap(k) for k in extra_keys}

    def pixel(self, x, y, clip=None):
        """Return a snapped RGB, or TRANS. `clip` = (x0,y0,x1,y1) mask rect."""
        if x < 0 or y < 0 or x >= self.w or y >= self.h:
            return TRANS
        if clip and not (clip[0] <= x < clip[2] and clip[1] <= y < clip[3]):
            return TRANS
        p = self.px[x, y]
        if p[3] < 128:
            return TRANS
        c = snap(p[:3])
        if c in self.keys:
            return TRANS
        return c

    def tile(self, tx, ty, clip=None):
        """8x8 tile at tile coords."""
        return tuple(self.pixel(tx * 8 + x, ty * 8 + y, clip)
                     for y in range(8) for x in range(8))

    def tile_px(self, x0, y0, clip=None):
        return tuple(self.pixel(x0 + x, y0 + y, clip)
                     for y in range(8) for x in range(8))

    def metatile_px(self, x0, y0, clip=None):
        """4 tiles (TL,TR,BL,BR) of the 16x16 block whose top-left is (x0,y0)."""
        return [self.tile_px(x0 + dx, y0 + dy, clip)
                for dy, dx in ((0, 0), (0, 8), (8, 0), (8, 8))]

    def bbox(self, x0, y0, x1, y1):
        """Tight opaque bbox inside the given rect (dest px), or None."""
        xs, ys = [], []
        for y in range(max(0, y0), min(self.h, y1)):
            for x in range(max(0, x0), min(self.w, x1)):
                if self.pixel(x, y) != TRANS:
                    xs.append(x); ys.append(y)
        if not xs:
            return None
        return (min(xs), min(ys), max(xs) + 1, max(ys) + 1)


def flips(t):
    fx = tuple(t[r * 8 + (7 - c)] for r in range(8) for c in range(8))
    fy = tuple(t[(7 - r) * 8 + c] for r in range(8) for c in range(8))
    fxy = tuple(fx[(7 - r) * 8 + c] for r in range(8) for c in range(8))
    return {t: (0, 0), fx: (1, 0), fy: (0, 1), fxy: (1, 1)}


def canon(t):
    return min(flips(t))


def colors(t):
    return {c for c in t if c != TRANS}


def _dist(a, b):
    return (a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2 + (a[2] - b[2]) ** 2


def quantize(cs, limit=COLORS_PER_PAL):
    """Merge nearest colours until <= limit. Returns (newset, remap)."""
    cols = list(cs)
    rmap = {c: c for c in cols}
    while len(cols) > limit:
        bi = bj = None; bd = None
        for i in range(len(cols)):
            for j in range(i + 1, len(cols)):
                d = _dist(cols[i], cols[j])
                if bd is None or d < bd:
                    bd = d; bi, bj = i, j
        a, b = cols[bi], cols[bj]
        m = snap(((a[0] + b[0]) // 2, (a[1] + b[1]) // 2, (a[2] + b[2]) // 2))
        for k, v in list(rmap.items()):
            if v in (a, b):
                rmap[k] = m
        cols = [m if c in (a, b) else c for c in cols]
        cols = list(dict.fromkeys(cols))
    return set(cols), rmap


def load_pal_files(tileset_dir, slots):
    """Read palettes/NN.pal for the given slots as ordered colour lists,
    stripping index 0 (transparent) and trailing black padding. sorted()
    palettes never END with (0,0,0), so trailing blacks are always padding."""
    out = []
    for slot in slots:
        lines = open(os.path.join(tileset_dir, "palettes", f"{slot:02d}.pal")).read().splitlines()[3:]
        cols = []
        for ln in lines:
            ln = ln.strip()
            if not ln:
                continue
            r, g, b = (int(x) for x in ln.split())
            cols.append((r, g, b))
        cols = cols[1:16]
        while cols and cols[-1] == (0, 0, 0):
            cols.pop()
        out.append(cols)
    return out


class Ref:
    """A ready-made tile reference (e.g. borrowed from the primary tileset)."""
    __slots__ = ("v",)

    def __init__(self, tid, xf, yf, slot):
        self.v = (tid, xf, yf, slot)


class PrimaryBase:
    """Pull a base-fill metatile out of an already-built primary tileset."""

    def __init__(self, primary_dir, metatile_id):
        mb = open(os.path.join(primary_dir, "metatiles.bin"), "rb").read()
        off = metatile_id * 16
        self.entries = []
        for q in range(4):                       # bottom layer only
            v = struct.unpack("<H", mb[off + q * 2: off + q * 2 + 2])[0]
            self.entries.append(Ref(v & 0x3FF, (v >> 10) & 1, (v >> 11) & 1, (v >> 12) & 0xF))


class Builder:
    """Lays content out on a `sel_width`-wide grid so porymap's metatile
    selector shows each object as an assembled rectangle.

    primary=True builds a PRIMARY tileset instead: tile ids start at 0 (no
    +512 offset), palettes go in slots 0-5 (six of them), and callers
    register it with .isSecondary = FALSE."""

    def __init__(self, name, sel_width=8, primary=False):
        self.name = name
        self.sel_width = sel_width
        self.primary = primary
        self.pal_slots = list(range(0, 6)) if primary else SEC_PAL_SLOTS
        self.max_pals = len(self.pal_slots)
        self.tile_base = 0 if primary else NUM_TILES_IN_PRIMARY
        self.cells = {}        # (row,col) -> (label, bottom4|None, top4|None, attr)
        self.shelf_row = 0     # first row of the current shelf
        self.shelf_h = 0
        self.shelf_x = 0

    # ---------------- placement ----------------
    def _alloc(self, w, h, newline=False):
        w = min(w, self.sel_width)
        if newline or self.shelf_x + w > self.sel_width:
            self.shelf_row += self.shelf_h
            self.shelf_h, self.shelf_x = 0, 0
        r0, c0 = self.shelf_row, self.shelf_x
        self.shelf_x += w
        self.shelf_h = max(self.shelf_h, h)
        return r0, c0

    def newline(self):
        if self.shelf_h:
            self.shelf_row += self.shelf_h
        self.shelf_h, self.shelf_x = 0, 0

    def _put(self, r, c, item):
        self.cells[(r, c)] = item

    # ---------------- content collection ----------------
    def terrain(self, sheet, x0, y0, cols, rows, label="terrain", attr=0,
                base=None, attr_over=LAYER_COVERED, newline=False):
        """Grid-aligned metatiles from dest-pixel (x0,y0).

        Fully opaque cells become cheap single-layer metatiles. Cells with
        transparency are stacked over `base` (if given) so no backdrop shows
        through; those get `attr_over`.
        """
        r0, c0 = self._alloc(cols, rows, newline)
        n = 0
        for r in range(rows):
            for c in range(cols):
                if c >= self.sel_width:
                    continue
                sub = sheet.metatile_px(x0 + c * 16, y0 + r * 16)
                if all(not colors(t) for t in sub):
                    continue
                opaque = all(TRANS not in t for t in sub)
                if base is None or opaque:
                    self._put(r0 + r, c0 + c, (f"{label}[{r},{c}]", sub, None, attr))
                else:
                    self._put(r0 + r, c0 + c, (f"{label}[{r},{c}]", list(base), sub, attr_over))
                n += 1
        return n

    def obj(self, sheet, bbox, base, cols=None, rows=None, label="obj",
            attr=LAYER_COVERED, halign="center", valign="bottom",
            top_rows=0, top_attr=LAYER_NORMAL, newline=False):
        """Extract a free-floating object, snapping it into a clean cell grid.

        The topmost `top_rows` metatile rows get `top_attr`; use LAYER_NORMAL
        there so the player walks *behind* tree canopies / building storeys.
        """
        x0, y0, x1, y1 = bbox
        w, h = x1 - x0, y1 - y0
        cols = cols or math.ceil(w / 16)
        rows = rows or math.ceil(h / 16)
        W, H = cols * 16, rows * 16
        if halign == "center":
            ox = x0 - (W - w) // 2
        elif halign == "right":
            ox = x1 - W
        else:
            ox = x0
        if valign == "bottom":
            oy = y1 - H
        elif valign == "center":
            oy = y0 - (H - h) // 2
        else:
            oy = y0
        r0, c0 = self._alloc(cols, rows, newline)
        n = 0
        for r in range(rows):
            for c in range(min(cols, self.sel_width)):
                sub = sheet.metatile_px(ox + c * 16, oy + r * 16, clip=bbox)
                if all(not colors(t) for t in sub):
                    continue
                a = top_attr if r < top_rows else attr
                self._put(r0 + r, c0 + c, (f"{label}[{r},{c}]", list(base), sub, a))
                n += 1
        return n

    # ---------------- packing ----------------
    def _linearize(self):
        """Row-major flatten of the placement grid, padding gaps with blanks."""
        if not self.cells:
            return []
        nrows = max(r for r, _ in self.cells) + 1
        return [self.cells.get((r, c), ("blank", None, None, 0))
                for r in range(nrows) for c in range(self.sel_width)]

    def _all_tiles(self):
        for _, bot, top, _ in self.items:
            for grp in (bot, top):
                if grp is None:
                    continue
                for t in grp:
                    if not isinstance(t, Ref):
                        yield t

    def pack_append(self, prev_dir, n_prev_metas, verbose=True,
                    extra_pal_colors=None, kill_labels=()):
        """Append-only rebuild: the previous build's palettes, tiles and first
        n_prev_metas metatiles are carried over VERBATIM (painted maps keep
        rendering identically); only items beyond that count are newly
        assigned, reusing previous tiles where they match and appending new
        tile ids after the previous tile count."""
        self.items = self._linearize()
        assert len(self.items) >= n_prev_metas,             f"placement shrank: {len(self.items)} < {n_prev_metas}"
        slots = self.pal_slots
        self.remap = {}
        self.pal_lists = load_pal_files(prev_dir, slots)
        if extra_pal_colors:
            # append NEW colours into a palette's free slots -- existing
            # entries (and thus painted tiles) keep their exact indices
            for pi, cols in extra_pal_colors.items():
                for c in cols:
                    if c not in self.pal_lists[pi]:
                        self.pal_lists[pi].append(c)
                assert len(self.pal_lists[pi]) <= COLORS_PER_PAL,                     f"palette {pi} overflow: {len(self.pal_lists[pi])}"
        self.pals = [set(p) for p in self.pal_lists]
        # seed tiles + dedup index from the previous tiles.png
        img = Image.open(os.path.join(prev_dir, "tiles.png")).convert("P")
        pxl = img.load()
        tw = img.width // 8
        prev_meta = open(os.path.join(prev_dir, "metatiles.bin"), "rb").read()
        prev_attr = open(os.path.join(prev_dir, "metatile_attributes.bin"), "rb").read()
        # recover each previous tile's palette from the metatile entries
        img_capacity = (img.width // 8) * (img.height // 8)
        tile_pal = {0: 0}
        for off in range(0, n_prev_metas * 16, 2):
            v = struct.unpack("<H", prev_meta[off:off + 2])[0]
            tid = v & 0x3FF
            local = tid - self.tile_base
            # skip refs outside this tileset's own tile space (cross-tileset
            # refs are legal in the binary; they're not ours to seed)
            if 0 <= local < img_capacity:
                tile_pal.setdefault(local, (v >> 12) - slots[0])
        n_prev_tiles = 0
        for local, _ in tile_pal.items():
            n_prev_tiles = max(n_prev_tiles, local + 1)
        self.tiles = []
        self.tindex = {}
        for tid in range(n_prev_tiles):
            ox, oy = (tid % tw) * 8, (tid // tw) * 8
            idxs = tuple(pxl[ox + x, oy + y] for y in range(8) for x in range(8))
            pi = tile_pal.get(tid, 0)
            self.tiles.append((pi, idxs))
            pal = self.pal_lists[pi]
            rgb = tuple(TRANS if k == 0 else pal[k - 1] for k in idxs)
            self.tindex.setdefault((canon(rgb), pi), tid)
        # optionally retire prev metatiles by label: blank their entries and
        # recycle tile slots referenced ONLY by them (painted cells showing a
        # killed metatile go blank -- the caller repaints those few cells)
        prev_meta = bytearray(prev_meta)
        killed = set()
        if kill_labels:
            for i in range(min(n_prev_metas, len(self.items))):
                lbl = self.items[i][0]
                if any(lbl.startswith(k) for k in kill_labels):
                    killed.add(i)
            refs_alive = {}
            for mi in range(n_prev_metas):
                if mi in killed:
                    continue
                for q in range(8):
                    v = struct.unpack("<H", prev_meta[mi*16+q*2:mi*16+q*2+2])[0]
                    local = (v & 0x3FF) - self.tile_base
                    if 0 <= local < img_capacity:
                        refs_alive[local] = True
            self.free_slots = []
            for mi in killed:
                for q in range(8):
                    v = struct.unpack("<H", prev_meta[mi*16+q*2:mi*16+q*2+2])[0]
                    local = (v & 0x3FF) - self.tile_base
                    if 0 < local < img_capacity and local not in refs_alive:
                        refs_alive[local] = True   # free once
                        self.free_slots.append(local)
                prev_meta[mi*16:(mi+1)*16] = bytes(16)
                prev_attr = prev_attr[:mi*2] + bytes(2) + prev_attr[mi*2+2:]
            # freed slots must not dedup-match: drop them from the seed index
            self.tindex = {k: v for k, v in self.tindex.items()
                           if v not in set(self.free_slots)}
            if verbose and killed:
                print(f"  KILLED {len(killed)} metatiles ({sorted(killed)[0]}-{sorted(killed)[-1]}), "
                      f"recycled {len(self.free_slots)} tile slots")
        self.metas = []
        self._prev_meta_bytes = bytes(prev_meta[:n_prev_metas * 16])
        self._prev_attr_bytes = prev_attr[:n_prev_metas * 2]
        self._n_prev_metas = n_prev_metas
        for label, bot, top, attr in self.items[n_prev_metas:]:
            ent = []
            for grp in (bot, top):
                if grp is None:
                    ent += [(0, 0, 0, 0)] * 4
                    continue
                for tt in grp:
                    ent.append(tt.v if isinstance(tt, Ref) else self._tile_ref(tt))
            self.metas.append((label, ent, attr))
        if verbose:
            print(f"  APPEND: prev {n_prev_metas} metas/{n_prev_tiles} tiles verbatim; "
                  f"now tiles={len(self.tiles)}/{MAX_SECONDARY_TILES} "
                  f"metatiles={n_prev_metas + len(self.metas)}/{MAX_SECONDARY_METATILES}")
        assert len(self.tiles) <= MAX_SECONDARY_TILES
        assert n_prev_metas + len(self.metas) <= MAX_SECONDARY_METATILES

    def pack(self, verbose=True, frozen_palettes=None):
        """frozen_palettes: list of ordered colour lists (from load_pal_files).
        When given, palette derivation is SKIPPED entirely -- palettes stay
        byte-identical to the previous build and every tile maps into them
        (exact match or nearest colour). Use for append-only rebuilds of a
        tileset that painted maps already reference."""
        self.items = self._linearize()
        if frozen_palettes is not None:
            self.remap = {}
            self.pal_lists = [list(p) for p in frozen_palettes]
            self.pals = [set(p) for p in self.pal_lists]
            assert len(self.pals) <= self.max_pals
            if verbose:
                print(f"  FROZEN palettes: {len(self.pals)} sizes={[len(p) for p in self.pals]}")
            self._assign_tiles(verbose)
            return
        # 1. per-tile colour sets, deduped by canonical form
        seen, sets = set(), []
        for t in self._all_tiles():
            cs = colors(t)
            if not cs:
                continue
            k = canon(t)
            if k in seen:
                continue
            seen.add(k)
            sets.append(cs)
        remap = {}
        # any single tile with >15 colours must be quantized first
        for i, cs in enumerate(sets):
            if len(cs) > COLORS_PER_PAL:
                ns, rm = quantize(cs)
                remap.update(rm)
                sets[i] = ns
        if remap:
            sets = [{remap.get(c, c) for c in s} for s in sets]

        # 2. greedy first-fit-best palette assembly
        pals = []
        for cs in sorted(sets, key=len, reverse=True):
            best, bsz = None, None
            for i, p in enumerate(pals):
                u = len(p | cs)
                if u <= COLORS_PER_PAL and (bsz is None or u < bsz):
                    bsz, best = u, i
            if best is None:
                pals.append(set(cs))
            else:
                pals[best] |= cs
        if verbose:
            print(f"  greedy palettes: {len(pals)} sizes={sorted(len(p) for p in pals)}")

        # 3. squeeze down to MAX_PALS, quantizing when a merge overflows
        while len(pals) > self.max_pals:
            best, bl = None, None
            for i in range(len(pals)):
                for j in range(i + 1, len(pals)):
                    u = len(pals[i] | pals[j])
                    if bl is None or u < bl:
                        bl, best = u, (i, j)
            i, j = best
            u = pals[i] | pals[j]
            if len(u) > COLORS_PER_PAL:
                u, rm = quantize(u)
                remap.update(rm)
            pals[i] = u
            pals.pop(j)
        # resolve remap chains
        for k in list(remap):
            v = remap[k]; guard = set()
            while v in remap and remap[v] != v and v not in guard:
                guard.add(v); v = remap[v]
            remap[k] = v
        pals = [{remap.get(c, c) for c in p} for p in pals]
        assert all(len(p) <= COLORS_PER_PAL for p in pals), [len(p) for p in pals]
        if verbose:
            print(f"  final palettes: {len(pals)} sizes={[len(p) for p in pals]}")

        self.remap = remap
        self.pals = pals
        self.pal_lists = [sorted(p) for p in pals]
        self._assign_tiles(verbose)

    def _assign_tiles(self, verbose=True):
        # tile dedup + id assignment against self.pal_lists/self.pals
        self.tiles = [(0, tuple(0 for _ in range(64)))]   # slot 0 = blank
        self.tindex = {}
        out = []
        for label, bot, top, attr in self.items:
            ent = []
            for grp in (bot, top):
                if grp is None:
                    ent += [(0, 0, 0, 0)] * 4
                    continue
                for t in grp:
                    ent.append(t.v if isinstance(t, Ref) else self._tile_ref(t))
            out.append((label, ent, attr))
        self.metas = out
        if verbose:
            print(f"  tiles={len(self.tiles)}/{MAX_SECONDARY_TILES}  "
                  f"metatiles={len(self.metas)}/{MAX_SECONDARY_METATILES}")
        assert len(self.tiles) <= MAX_SECONDARY_TILES, f"too many tiles: {len(self.tiles)}"
        assert len(self.metas) <= MAX_SECONDARY_METATILES, f"too many metatiles: {len(self.metas)}"

    def _rc(self, c):
        return self.remap.get(c, c)

    def _find_pal(self, cs):
        """Palette that reproduces this tile's colours most faithfully.

        An exact match wins outright; otherwise score by how far every colour
        would have to move to its nearest entry. Counting shared colours alone
        picks palettes that render the *unshared* ones badly wrong.
        """
        cs = {self._rc(c) for c in cs}
        for i, p in enumerate(self.pals):
            if cs <= p:
                return i
        best, bp = None, 0
        for i, p in enumerate(self.pals):
            err = sum(min(_dist(c, q) for q in p) for c in cs)
            if best is None or err < best:
                best, bp = err, i
        return bp

    def _nearest(self, col, pi):
        pal = self.pal_lists[pi]
        best, bi = None, 0
        for k, q in enumerate(pal):
            d = _dist(col, q)
            if best is None or d < best:
                best, bi = d, k
        return bi + 1                      # index 0 is transparent

    def _tile_ref(self, t):
        cs = colors(t)
        if not cs:
            return (0, 0, 0, 0)
        pi = self._find_pal(cs)
        c = canon(t)
        key = (c, pi)
        if key not in self.tindex:
            free = getattr(self, "free_slots", None)
            if free:
                slot = free.pop(0)
                self.tindex[key] = slot
                cm = {col: k + 1 for k, col in enumerate(self.pal_lists[pi])}
                idxs = []
                for col in c:
                    if col == TRANS:
                        idxs.append(0)
                        continue
                    rc = self._rc(col)
                    idxs.append(cm[rc] if rc in cm else self._nearest(rc, pi))
                self.tiles[slot] = (pi, tuple(idxs))
                tid = slot
                xf, yf = flips(c)[t]
                return (tid + self.tile_base, xf, yf, self.pal_slots[pi])
            self.tindex[key] = len(self.tiles)
            cm = {col: k + 1 for k, col in enumerate(self.pal_lists[pi])}
            idxs = []
            for col in c:
                if col == TRANS:
                    idxs.append(0)
                    continue
                rc = self._rc(col)
                # fall back to the closest entry, never to slot 1 (the darkest)
                idxs.append(cm[rc] if rc in cm else self._nearest(rc, pi))
            self.tiles.append((pi, tuple(idxs)))
        tid = self.tindex[key]
        xf, yf = flips(c)[t]
        return (tid + self.tile_base, xf, yf, self.pal_slots[pi])

    # ---------------- output ----------------
    def write(self, outdir, tiles_per_row=16):
        os.makedirs(os.path.join(outdir, "palettes"), exist_ok=True)
        n = len(self.tiles)
        rows = (n + tiles_per_row - 1) // tiles_per_row
        img = Image.new("P", (tiles_per_row * 8, rows * 8), 0)
        flat = [0, 0, 0]
        for c in self.pal_lists[0]:
            flat += list(c)
        flat += [0, 0, 0] * (256 - len(flat) // 3)
        img.putpalette(flat[:768])
        px = img.load()
        for tid, (pi, idxs) in enumerate(self.tiles):
            ox, oy = (tid % tiles_per_row) * 8, (tid // tiles_per_row) * 8
            for y in range(8):
                for x in range(8):
                    px[ox + x, oy + y] = idxs[y * 8 + x]
        img.save(os.path.join(outdir, "tiles.png"), bits=4)

        mb = bytearray(getattr(self, "_prev_meta_bytes", b""))
        for _, ent, _ in self.metas:
            for (tid, xf, yf, sl) in ent:
                mb += struct.pack("<H", (sl << 12) | (yf << 11) | (xf << 10) | (tid & 0x3FF))
        open(os.path.join(outdir, "metatiles.bin"), "wb").write(mb)

        ab = bytearray(getattr(self, "_prev_attr_bytes", b""))
        for _, _, attr in self.metas:
            ab += struct.pack("<H", attr)
        open(os.path.join(outdir, "metatile_attributes.bin"), "wb").write(ab)

        for slot in range(16):
            pi = slot - self.pal_slots[0]
            cols = self.pal_lists[pi] if 0 <= pi < len(self.pal_lists) else []
            full = [(0, 0, 0)] + cols
            full += [(0, 0, 0)] * (16 - len(full))
            with open(os.path.join(outdir, f"palettes/{slot:02d}.pal"), "w") as f:
                f.write("\n".join(["JASC-PAL", "0100", "16"] +
                                  [f"{c[0]} {c[1]} {c[2]}" for c in full[:16]]) + "\n")
        # stale build products must go so make regenerates them
        for junk in ("tiles.4bpp", "tiles.4bpp.lz"):
            p = os.path.join(outdir, junk)
            if os.path.exists(p):
                os.remove(p)
        for slot in range(16):
            p = os.path.join(outdir, f"palettes/{slot:02d}.gbapal")
            if os.path.exists(p):
                os.remove(p)
        print(f"  wrote {outdir}: {n} tiles, {len(self.metas)} metatiles")
        return n, len(self.metas)

    def manifest(self):
        """Metatile-id range per content group, in placement order."""
        spans = {}
        for i, (label, _, _, _) in enumerate(self.items):
            if label == "blank":
                continue
            g = label.split("[")[0]
            lo, hi = spans.get(g, (i, i))
            spans[g] = (min(lo, i), max(hi, i))
        for g, (lo, hi) in sorted(spans.items(), key=lambda kv: kv[1][0]):
            print(f"    0x{lo:03X}-0x{hi:03X}  ({lo:3d}-{hi:3d})  {g}")
