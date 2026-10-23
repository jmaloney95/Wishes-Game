/* Wishes of Tomorrow — the Update tab.

   The player drops in the WoT ROM they already play. Its CRC32 says which
   release it is; patches/updates/manifest.json maps that to a small
   old→latest BPS patch (built by Tools/make_update_patches.py). The updated
   ROM downloads under the player's own filename, because that is how mGBA,
   RetroArch, MyBoy, Pizza Boy and friends find the matching save.

   The save checker reads a .sav/.srm without changing it: finds the newest
   save slot, checks it, shows whose save it is, and offers the same bytes
   back under a filename that matches the game file.

   Patching reuses assets/patcher.js (window.WoTPatcher). Nothing is uploaded. */
(function () {
  "use strict";

  var MANIFEST = "patches/updates/manifest.json";

  /* ── save format (pokeemerald src/save.c, include/save.h) ─────────── */
  var SECTOR = 0x1000, SECTORS_PER_SLOT = 14, FLASH_SIZE = 0x20000;
  var SIGNATURE = 0x08012025;
  var SB2_SIZE = 3908; // sizeof(struct SaveBlock2), from the ROM's GF header

  var ui = {};
  var manifest = null, manifestLoad = null;
  var oldRom = null, oldName = "", update = null;
  var resultUrl = null, saveUrls = [];
  var saveBytes = null, saveNameEdited = false;

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

  function readFile(file) {
    return new Promise(function (resolve, reject) {
      var fr = new FileReader();
      fr.onload = function () { resolve(new Uint8Array(fr.result)); };
      fr.onerror = function () { reject(new Error("Couldn't read that file.")); };
      fr.readAsArrayBuffer(file);
    });
  }

  function baseName(name) { return name.replace(/\.[^.]*$/, ""); }

  function loadManifest() {
    if (!manifestLoad) {
      manifestLoad = fetch(MANIFEST + "?t=" + Date.now(), { cache: "no-store" }).then(function (res) {
        if (!res.ok) throw new Error(String(res.status));
        return res.json();
      }).then(function (m) {
        manifest = m;
        if (ui.latest) ui.latest.textContent = "v" + m.latest;
        return m;
      }).catch(function (err) {
        manifestLoad = null; // let the next try fetch again
        throw err;
      });
    }
    return manifestLoad;
  }

  /* ── step 2: the player's current game file ───────────────────────── */
  async function onOldRom(file) {
    if (!file) return;
    var P = window.WoTPatcher;
    update = null;
    oldRom = null;
    ui.apply.disabled = true;
    ui.apply.hidden = false;
    ui.result.hidden = true;
    ui.play.hidden = true;
    ui.fallback.hidden = true;

    var bytes;
    try {
      bytes = await readFile(file);
    } catch (err) {
      say("bad", err.message);
      return;
    }
    // Played from a zip, the emulator names the save after the zip, so the
    // updated .gba takes the zip's name.
    oldName = /\.zip$/i.test(file.name) ? baseName(file.name) + ".gba" : file.name;
    if (P.isZip(bytes)) {
      say("busy", "Unzipping…");
      try {
        bytes = (await P.unzipRom(bytes)).bytes;
      } catch (err) {
        setSlot(ui.romSlot, file.name, "couldn't unzip", false);
        say("bad", err.message, err.detail);
        return;
      }
    }

    if (bytes.length <= FLASH_SIZE + 64 && bytes.length >= FLASH_SIZE / 2) {
      setSlot(ui.romSlot, file.name, fmtBytes(bytes.length), false);
      say("warn", "That looks like a save file, not the game.",
        "Choose the .gba here. Saves go in step 4.");
      return;
    }

    say("busy", "Checking which version this is…");
    await new Promise(function (r) { setTimeout(r, 30); });
    var crc = hex(P.crc32(bytes));
    var m;
    try {
      m = await loadManifest();
    } catch (err) {
      setSlot(ui.romSlot, file.name, fmtBytes(bytes.length), false);
      say("bad", "Couldn't load the update list.", "Check your connection and choose the file again.");
      return;
    }

    if (crc === m.latestCrc) {
      setSlot(ui.romSlot, file.name, "v" + m.latest + " · already the latest", true);
      say("ok", "You're already on v" + m.latest + ".",
        "Nothing to update. If your save won't load, check it in step 4.");
      return;
    }
    var from = m.from[crc];
    if (!from) {
      setSlot(ui.romSlot, file.name, "not recognised", false);
      var base = crc === "1F1C08FB";
      say("warn", base ? "That's the original Emerald ROM, not Wishes of Tomorrow."
                       : "We don't recognise this file.",
        base ? "Use it in Patch & play — we'll name the result " + oldName + " for you."
             : "It isn't any released version of Wishes of Tomorrow (it may be trimmed, edited or from another hack). You can patch a fresh copy from your base Emerald ROM instead; it'll be named " + oldName + ".");
      ui.fallback.hidden = false;
      return;
    }

    oldRom = bytes;
    update = from;
    setSlot(ui.romSlot, file.name, "v" + from.version + " → v" + m.latest, true);
    ui.apply.disabled = false;
    say("idle", "Recognised: v" + from.version + ".",
      "Press Update. The update is " + fmtBytes(from.size) + ".");
  }

  /* ── step 3: update ───────────────────────────────────────────────── */
  function apply() {
    if (!oldRom || !update) return;
    var P = window.WoTPatcher;
    ui.apply.disabled = true;
    say("busy", "Downloading the update…", fmtBytes(update.size));
    fetch(update.url).then(function (res) {
      if (!res.ok) throw { message: "Couldn't download the update.", detail: "The server returned " + res.status + "." };
      return res.arrayBuffer();
    }).then(function (buf) {
      say("busy", "Updating…", "Large ROMs take a moment.");
      return new Promise(function (r) { setTimeout(function () { r(buf); }, 30); });
    }).then(function (buf) {
      var out = P.applyBPS(oldRom, new Uint8Array(buf)); // checks both CRCs
      if (resultUrl) URL.revokeObjectURL(resultUrl);
      resultUrl = URL.createObjectURL(new Blob([out], { type: "application/octet-stream" }));
      ui.result.href = resultUrl;
      ui.result.download = oldName;
      ui.result.title = "Save " + oldName;
      ui.result.textContent = "Save (" + fmtBytes(out.length) + ")";
      ui.result.hidden = false;
      ui.apply.hidden = true;
      ui.play.hidden = false;
      ui.play.onclick = function () { P.play(out, ui.play); };
      say("ok", "Updated to v" + manifest.latest + " — saved as " + oldName + ".",
        "Replace your old file with this one and press Continue in your emulator.");
    }).catch(function (err) {
      say("bad", err.message || "Something went wrong.", err.detail);
      ui.apply.disabled = false;
    });
  }

  function fallback() {
    var tab = document.getElementById("tab-patch");
    if (window.WoTPatcher.setOutputName) window.WoTPatcher.setOutputName(oldName);
    if (tab) tab.click();
    var patch = document.getElementById("patch");
    if (patch) patch.scrollIntoView({ block: "start", behavior: "smooth" });
  }

  /* ── step 4: the save checker ─────────────────────────────────────── */
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
    var best = slots[0], o = best.sb2;
    return {
      kind: "ok",
      checksumOk: checksum(b, o, SB2_SIZE) === u16(b, o + 0xff6),
      backup: slots.length > 1,
      name: decodeName(b, o, 7) || "(no name)",
      gender: b[o + 0x08] === 1 ? "♀" : "♂",
      id: ("0000" + u16(b, o + 0x0a)).slice(-5),
      hours: u16(b, o + 0x0e),
      minutes: b[o + 0x10],
      saves: best.counter
    };
  }

  async function onSave(file) {
    if (!file) return;
    var bytes;
    try {
      bytes = await readFile(file);
    } catch (err) {
      say("bad", err.message);
      return;
    }
    ui.card.hidden = true;
    saveBytes = null;

    if (bytes.length > 4 * 1048576) {
      setSlot(ui.saveSlot, file.name, fmtBytes(bytes.length), false);
      say("warn", "That looks like a game file, not a save.", "Choose the .gba in step 2. Saves are about 128 KB.");
      return;
    }

    var info = parseSave(bytes);
    if (info.kind !== "ok") {
      setSlot(ui.saveSlot, file.name, fmtBytes(bytes.length), false);
      var why = {
        small: ["This save is too small.",
          "Wishes of Tomorrow saves are 128 KB; this one is " + fmtBytes(bytes.length) + ". Set your emulator's save type to Flash 128K (1M), and check you picked the right file."],
        blank: ["This save is empty.", "The game was never saved into it. Try the other .sav, or look for a newer one."],
        damaged: ["This save looks damaged.", "Only part of a save slot is readable. If you have another copy, try that one."],
        foreign: ["This doesn't look like a Pokémon Emerald save.", "Check you picked the save for Wishes of Tomorrow, not a save state."]
      }[info.kind];
      say("bad", why[0], why[1]);
      return;
    }

    saveBytes = bytes;
    setSlot(ui.saveSlot, file.name, fmtBytes(bytes.length) + " · readable", true);
    ui.who.textContent = info.name + " " + info.gender + " · ID " + info.id;
    ui.meta.textContent = info.hours + "h " + ("0" + info.minutes).slice(-2) + "m played · saved " +
      info.saves.toLocaleString() + " time" + (info.saves === 1 ? "" : "s") +
      (info.backup ? " · backup slot intact" : "");
    ui.card.classList.toggle("is-warn", !info.checksumOk);
    if (!saveNameEdited) ui.saveName.value = oldName ? baseName(oldName) : baseName(file.name);
    paintSaveLinks();
    ui.card.hidden = false;
    if (info.checksumOk) {
      say("ok", "Your save is readable.",
        "Keep it next to the updated game under the same name. Want a copy named to match? Download it below.");
    } else {
      say("warn", "Your save is readable, but its newest slot didn't check out.",
        "The game will load the backup slot, which can be one save older. Keep the original file safe.");
    }
  }

  function paintSaveLinks() {
    if (!saveBytes) return;
    var name = ui.saveName.value.replace(/\.(gba|sav|srm|sa1|zip)$/i, "").replace(/[\\/:*?"<>|]/g, "").trim() || "Wishes of Tomorrow";
    saveUrls.forEach(URL.revokeObjectURL);
    saveUrls = [];
    [[ui.sav, ".sav"], [ui.srm, ".srm"]].forEach(function (pair) {
      var url = URL.createObjectURL(new Blob([saveBytes], { type: "application/octet-stream" }));
      saveUrls.push(url);
      pair[0].href = url;
      pair[0].download = name + pair[1];
    });
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
      if (e.dataTransfer.files.length) handler(e.dataTransfer.files[0]);
    });
  }

  function init() {
    var root = $("[data-updater]");
    if (!root || !window.WoTPatcher) return;
    ui.status = $("[data-update-status]", root);
    ui.romSlot = $("[data-slot=old-rom]", root);
    ui.saveSlot = $("[data-slot=save]", root);
    ui.apply = $("[data-update-apply]", root);
    ui.result = $("[data-update-result]", root);
    ui.play = $("[data-update-play]", root);
    ui.latest = $("[data-update-latest]", root);
    ui.fallback = $("[data-update-fallback]", root);
    ui.card = $("[data-save-card]", root);
    ui.who = $("[data-save-who]", root);
    ui.meta = $("[data-save-meta]", root);
    ui.saveName = $("[data-save-name]", root);
    ui.sav = $("[data-save-sav]", root);
    ui.srm = $("[data-save-srm]", root);

    $("#old-rom-file", root).addEventListener("change", function () { onOldRom(this.files[0]); this.value = ""; });
    $("#save-file", root).addEventListener("change", function () { onSave(this.files[0]); this.value = ""; });
    dropTarget(ui.romSlot, onOldRom);
    dropTarget(ui.saveSlot, onSave);
    ui.apply.addEventListener("click", apply);
    $("button", ui.fallback).addEventListener("click", fallback);
    ui.saveName.addEventListener("input", function () { saveNameEdited = true; paintSaveLinks(); });

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
