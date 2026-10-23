/* Wishes of Tomorrow — the Update tab.

   The player brings whatever they have: the WoT ROM they play (any version),
   their save, or both. One file input takes either; the size says which.

   - A WoT ROM: its CRC32 picks a small old→latest patch from
     patches/updates/manifest.json (built by Tools/make_update_patches.py).
     The result keeps the player's filename, because that is how mGBA,
     RetroArch, MyBoy, Pizza Boy and friends find the matching save.
   - A save alone: read (newest slot, whose save it is), then a second row
     asks for the base ROM or any WoT ROM, since the site only ships patches.
     The result is named after the save so the two pair up.

   Either way the last row offers Download and Play in browser, which loads
   the player's save when they gave one. Patching reuses assets/patcher.js
   (window.WoTPatcher). Nothing is uploaded. */
(function () {
  "use strict";

  var MANIFEST = "patches/updates/manifest.json";
  var BASE_CRC = "1F1C08FB"; // Pokémon Emerald (U), what the full patch expects

  /* ── save format (pokeemerald src/save.c, include/save.h) ─────────── */
  var SECTOR = 0x1000, SECTORS_PER_SLOT = 14, FLASH_SIZE = 0x20000;
  var SIGNATURE = 0x08012025;
  var SB2_SIZE = 3908; // sizeof(struct SaveBlock2), from the ROM's GF header

  var ui = {};
  var manifest = null, manifestLoad = null;
  var mine = null;   // { bytes, name, crc } — the player's own WoT ROM
  var base = null;   // { bytes, name, rom, inRow1 } — a base ROM, or a WoT ROM from row 2
  var unknown = "";  // name of an unrecognised ROM chosen in row 1
  var save = null;   // { bytes, name, info }
  var built = null, resultUrl = null, job = 0;

  function $(sel, root) { return (root || document).querySelector(sel); }

  function say(kind, msg, detail) {
    ui.status.className = "patch-status is-" + kind;
    ui.status.innerHTML = "";
    var strong = document.createElement("strong");
    strong.textContent = msg;
    ui.status.appendChild(strong);
    if (detail) {
      var small = document.createElement("span");
      small.textContent = detail;
      ui.status.appendChild(small);
    }
  }

  function setSlot(slot, name, note, ok) {
    $("[data-slot-name]", slot).textContent = name;
    $("[data-slot-note]", slot).textContent = note;
    slot.dataset.state = ok ? "ok" : "";
  }

  function fmtBytes(n) {
    if (n >= 1048576) return (n / 1048576).toFixed(n % 1048576 ? 1 : 0) + " MB";
    if (n >= 1024) return Math.round(n / 1024) + " KB";
    return n + " bytes";
  }

  function hex(n) { return ("0000000" + n.toString(16).toUpperCase()).slice(-8); }
  function u16(b, o) { return b[o] | (b[o + 1] << 8); }
  function u32(b, o) { return (b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0; }
  function baseName(name) { return name.replace(/\.[^.]*$/, ""); }
  function tick() { return new Promise(function (r) { setTimeout(r, 30); }); }

  function readFile(file) {
    return new Promise(function (resolve, reject) {
      var fr = new FileReader();
      fr.onload = function () { resolve(new Uint8Array(fr.result)); };
      fr.onerror = function () { reject(new Error("Couldn't read that file.")); };
      fr.readAsArrayBuffer(file);
    });
  }

  function loadManifest() {
    if (!manifestLoad) {
      manifestLoad = fetch(MANIFEST + "?t=" + Date.now(), { cache: "no-store" }).then(function (res) {
        if (!res.ok) throw new Error(String(res.status));
        return res.json();
      }).then(function (m) {
        manifest = m;
        ui.latest.textContent = "v" + m.latest;
        return m;
      }).catch(function (err) {
        manifestLoad = null; // let the next try fetch again
        throw err;
      });
    }
    return manifestLoad;
  }

  /* ── reading saves ────────────────────────────────────────────────── */
  // Gen 3 text encoding, the parts a player name can use.
  var CHARS = (function () {
    var t = {};
    var i;
    for (i = 0; i < 26; i++) { t[0xbb + i] = String.fromCharCode(65 + i); t[0xd5 + i] = String.fromCharCode(97 + i); }
    for (i = 0; i < 10; i++) t[0xa1 + i] = String(i);
    var extra = { 0x00: " ", 0xab: "!", 0xac: "?", 0xad: ".", 0xae: "-", 0xb0: "…", 0xb1: "“", 0xb2: "”",
      0xb3: "‘", 0xb4: "’", 0xb5: "♂", 0xb6: "♀", 0xb8: ",", 0xba: "/" };
    for (var k in extra) t[k] = extra[k];
    return t;
  })();

  function decodeName(b, off, len) {
    var s = "";
    for (var i = 0; i < len && b[off + i] !== 0xff; i++) s += CHARS[b[off + i]] || "?";
    return s.trim();
  }

  // Same sums as CalculateChecksum in src/save.c.
  function checksum(b, off, size) {
    var sum = 0;
    for (var i = 0; i < size; i += 4) sum = (sum + u32(b, off + i)) >>> 0;
    return ((sum >>> 16) + sum) & 0xffff;
  }

  function readSlot(b, slot) {
    var ids = {}, count = 0, counter = 0;
    for (var s = 0; s < SECTORS_PER_SLOT; s++) {
      var off = (slot * SECTORS_PER_SLOT + s) * SECTOR;
      if (u32(b, off + 0xff8) !== SIGNATURE) continue;
      var id = u16(b, off + 0xff4);
      if (id >= SECTORS_PER_SLOT || ids[id] !== undefined) continue;
      ids[id] = off;
      count++;
      if (id === 0) counter = u32(b, off + 0xffc);
    }
    return { complete: count === SECTORS_PER_SLOT, any: count > 0, counter: counter, sb2: ids[0] };
  }

  function parseSave(b) {
    if (b.length < FLASH_SIZE) return { kind: "small" };
    var first = b[0], blank = first === 0xff || first === 0x00;
    for (var i = 1; blank && i < FLASH_SIZE; i++) if (b[i] !== first) blank = false;
    if (blank) return { kind: "blank" };

    var slots = [readSlot(b, 0), readSlot(b, 1)].filter(function (s) { return s.complete; });
    if (!slots.length) {
      var partial = readSlot(b, 0).any || readSlot(b, 1).any;
      return { kind: partial ? "damaged" : "foreign" };
    }
    // Newest slot wins; a signed difference copes with the counter wrapping.
    slots.sort(function (a, c) { return ((c.counter - a.counter) | 0); });
    var o = slots[0].sb2;
    return {
      kind: "ok",
      checksumOk: checksum(b, o, SB2_SIZE) === u16(b, o + 0xff6),
      name: decodeName(b, o, 7) || "(no name)",
      hours: u16(b, o + 0x0e),
      minutes: b[o + 0x10]
    };
  }

  var SAVE_PROBLEMS = {
    small: ["This save is too small.", "Set your emulator's save type to Flash 128K."],
    blank: ["This save is empty.", "The game was never saved into it."],
    damaged: ["This save looks damaged.", "Try another copy if you have one."],
    foreign: ["That isn't a Wishes of Tomorrow save.", "Save states won't work — use the .sav."]
  };

  /* ── choosing files ───────────────────────────────────────────────── */
  // Unzips if needed; returns { bytes, name } or null after reporting why.
  async function readRomOrSave(file, slot) {
    var bytes;
    try {
      bytes = await readFile(file);
    } catch (err) {
      say("bad", err.message);
      return null;
    }
    var P = window.WoTPatcher;
    if (P.isZip(bytes)) {
      say("busy", "Unzipping…");
      try {
        bytes = (await P.unzipRom(bytes)).bytes;
      } catch (err) {
        setSlot(slot, file.name, "couldn't unzip", false);
        say("bad", err.message, err.detail);
        return null;
      }
    }
    // Played from a zip, the emulator names the save after the zip.
    var name = /\.zip$/i.test(file.name) ? baseName(file.name) + ".gba" : file.name;
    return { bytes: bytes, name: name };
  }

  async function romInfo(bytes) {
    say("busy", "Checking your game…");
    await tick();
    var crc = hex(window.WoTPatcher.crc32(bytes));
    await loadManifest();
    if (crc === manifest.latestCrc) return { crc: crc, version: manifest.latest };
    if (manifest.from[crc]) return { crc: crc, version: manifest.from[crc].version, update: manifest.from[crc] };
    if (crc === BASE_CRC) return { crc: crc, base: true };
    return { crc: crc };
  }

  async function onMine(file) {
    if (!file) return;
    var got = await readRomOrSave(file, ui.mineSlot);
    if (!got) return;

    if (got.bytes.length <= 4 * FLASH_SIZE) {
      var info = parseSave(got.bytes);
      if (info.kind !== "ok") {
        setSlot(ui.mineSlot, file.name, "", false);
        say("bad", SAVE_PROBLEMS[info.kind][0], SAVE_PROBLEMS[info.kind][1]);
        return;
      }
      save = { bytes: got.bytes, name: got.name, info: info };
    } else {
      var rom;
      try {
        rom = await romInfo(got.bytes);
      } catch (err) {
        say("bad", "Couldn't load the update list.", "Check your connection and try again.");
        return;
      }
      unknown = "";
      if (rom.version) {
        mine = { bytes: got.bytes, name: got.name, rom: rom };
      } else if (rom.base) {
        mine = null;
        base = { bytes: got.bytes, name: got.name, rom: rom, inRow1: true };
      } else {
        unknown = got.name;
        paint();
        say("warn", "We don't recognise this game.", "Choose your Pokémon Emerald ROM instead.");
        return;
      }
    }
    paint();
    build();
  }

  async function onBase(file) {
    if (!file) return;
    var got = await readRomOrSave(file, ui.baseSlot);
    if (!got) return;
    var rom;
    try {
      rom = await romInfo(got.bytes);
    } catch (err) {
      say("bad", "Couldn't load the update list.", "Check your connection and try again.");
      return;
    }
    if (!rom.version && !rom.base) {
      setSlot(ui.baseSlot, file.name, "not recognised", false);
      say("bad", "That isn't Pokémon Emerald (U) or Wishes of Tomorrow.");
      return;
    }
    base = { bytes: got.bytes, name: got.name, rom: rom, inRow1: false };
    setSlot(ui.baseSlot, file.name, rom.base ? "Pokémon Emerald" : "v" + rom.version, true);
    build();
  }

  // Row 1 lists what the player chose; row 2 (a ROM to patch) only shows
  // when they haven't given a game we can use.
  function paint() {
    var names = [], notes = [], ok = true;
    if (mine) { names.push(mine.name); notes.push("v" + mine.rom.version); }
    else if (base && base.inRow1) { names.push(base.name); notes.push("Pokémon Emerald"); }
    else if (unknown) { names.push(unknown); notes.push("not recognised"); ok = false; }
    if (save) {
      names.push(save.name);
      notes.push(save.info.name + ", " + save.info.hours + "h " + ("0" + save.info.minutes).slice(-2) + "m");
    }
    setSlot(ui.mineSlot, names.join(" + "), notes.join(" · "), ok);
    ui.baseSlot.hidden = !!(mine || (base && base.inRow1)) || !(save || unknown || base);
    ui.getNum.textContent = ui.baseSlot.hidden ? "2" : "3";
  }

  /* ── building the latest ROM ──────────────────────────────────────── */
  function outputName() {
    if (mine) return mine.name;
    if (save) return baseName(save.name) + ".gba";
    if (base && !base.rom.base) return base.name;
    return "Wishes of Tomorrow " + manifest.latest + ".gba";
  }

  function fetchBytes(url) {
    return fetch(url).then(function (res) {
      if (!res.ok) throw { message: "Couldn't download the update.", detail: "The server returned " + res.status + "." };
      return res.arrayBuffer();
    }).then(function (buf) { return new Uint8Array(buf); });
  }

  async function build() {
    var src = mine || base;
    ui.result.hidden = true;
    ui.play.disabled = true;
    built = null;
    if (!src) {
      say("idle", "Your save is ready.", "Now choose your Emerald ROM, or any Wishes of Tomorrow .gba.");
      return;
    }
    var me = ++job, rom = src.rom, out;
    try {
      if (rom.version === manifest.latest) {
        out = src.bytes;
      } else {
        var url = rom.update ? rom.update.url : window.WoTPatcher.patchUrl;
        say("busy", "Downloading the update…", rom.update ? fmtBytes(rom.update.size) : "");
        var patch = await fetchBytes(url);
        if (me !== job) return;
        say("busy", "Updating…");
        await tick();
        out = window.WoTPatcher.applyBPS(src.bytes, patch); // checks both CRCs
      }
    } catch (err) {
      if (me === job) say("bad", err.message || "Something went wrong.", err.detail);
      return;
    }
    if (me !== job) return;

    built = out;
    var name = outputName();
    if (resultUrl) URL.revokeObjectURL(resultUrl);
    resultUrl = URL.createObjectURL(new Blob([out], { type: "application/octet-stream" }));
    ui.result.href = resultUrl;
    ui.result.download = name;
    ui.result.title = "Save " + name;
    ui.result.hidden = rom.version === manifest.latest && src === mine;
    ui.play.disabled = false;
    if (ui.result.hidden) {
      say("ok", "You're already on v" + manifest.latest + ".", save ? "Press Play to load your save." : "");
    } else {
      say("ok", "Ready: " + name, save ? "Play loads your save." : "");
    }
  }

  /* ── wiring ───────────────────────────────────────────────────────── */
  function dropTarget(el, handler) {
    ["dragenter", "dragover"].forEach(function (ev) {
      el.addEventListener(ev, function (e) { e.preventDefault(); el.dataset.drop = "1"; });
    });
    ["dragleave", "drop"].forEach(function (ev) {
      el.addEventListener(ev, function () { delete el.dataset.drop; });
    });
    el.addEventListener("drop", function (e) {
      e.preventDefault();
      Array.prototype.forEach.call(e.dataTransfer.files, function (f) { handler(f); });
    });
  }

  function init() {
    var root = $("[data-updater]");
    if (!root || !window.WoTPatcher) return;
    ui.status = $("[data-update-status]", root);
    ui.mineSlot = $("[data-slot=mine]", root);
    ui.baseSlot = $("[data-slot=base]", root);
    ui.getNum = $("[data-get-num]", root);
    ui.result = $("[data-update-result]", root);
    ui.play = $("[data-update-play]", root);
    ui.latest = $("[data-update-latest]", root);

    $("#mine-file", root).addEventListener("change", function () { onMine(this.files[0]); this.value = ""; });
    $("#base-file", root).addEventListener("change", function () { onBase(this.files[0]); this.value = ""; });
    dropTarget(ui.mineSlot, onMine);
    dropTarget(ui.baseSlot, onBase);
    ui.play.addEventListener("click", function () {
      if (built) window.WoTPatcher.play(built, ui.play, save && save.bytes);
    });

    // The list is a few hundred bytes; fetch it once the tab is opened.
    var tab = document.getElementById("tab-update");
    if (tab) tab.addEventListener("click", function () { loadManifest().catch(function () {}); });
    if (!root.hidden) loadManifest().catch(function () {});
  }

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", init);
  } else {
    init();
  }
})();
