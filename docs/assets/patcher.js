/* ═══════════════════════════════════════════════════════════════════════
   In-browser ROM patcher — BPS, UPS and IPS.

   Everything here runs locally in the page. The base ROM is read with the
   File API, patched in memory, and handed back through a blob URL: it is
   never uploaded, and the site has no server to upload it to. That is the
   whole reason this exists rather than a link to an external patcher.

   We distribute a patch, never a built ROM. The player brings their own
   legally obtained base ROM.
   ═══════════════════════════════════════════════════════════════════ */
(function () {
  "use strict";

  /* ── the one block to edit per release ──────────────────────────────
     Drop the patch in docs/patches/ and point `url` at it. If the file
     isn't there yet the UI falls back to "choose the patch you
     downloaded" instead of showing a broken button. */
  var PATCH = {
    url: "patches/wishes-of-tomorrow-1.0.0.bps",
    outputName: "Wishes of Tomorrow 1.0.0.gba",
    base: {
      name: "Pokémon Emerald (U)",
      code: "BPEE",     // GBA header game code at 0xAC
      size: 16777216    // 16 MB
    }
  };

  /* ── CRC32 ──────────────────────────────────────────────────────── */
  var CRC_TABLE = (function () {
    var t = new Uint32Array(256);
    for (var n = 0; n < 256; n++) {
      var c = n;
      for (var k = 0; k < 8; k++) c = (c & 1) ? (0xedb88320 ^ (c >>> 1)) : (c >>> 1);
      t[n] = c >>> 0;
    }
    return t;
  })();

  function crc32(bytes, start, end) {
    start = start || 0;
    end = (end === undefined) ? bytes.length : end;
    var c = 0xffffffff;
    for (var i = start; i < end; i++) c = CRC_TABLE[(c ^ bytes[i]) & 0xff] ^ (c >>> 8);
    return (c ^ 0xffffffff) >>> 0;
  }

  function magic(bytes, len) {
    var s = "";
    for (var i = 0; i < len; i++) s += String.fromCharCode(bytes[i]);
    return s;
  }

  function hex8(n) { return n.toString(16).toUpperCase().padStart(8, "0"); }

  function PatchError(msg, detail) {
    var e = new Error(msg);
    e.detail = detail;
    return e;
  }

  /* ── shared reader ──────────────────────────────────────────────── */
  function Reader(bytes, pos) {
    this.b = bytes;
    this.pos = pos || 0;
  }
  // BPS and UPS share this variable-width number encoding: 7 bits a byte,
  // high bit marks the last byte, and each continuation adds the next
  // power so encodings stay unique. Multiplication, not shifts — these
  // values can exceed what a 32-bit shift holds.
  Reader.prototype.varint = function () {
    var data = 0, shift = 1, x;
    for (;;) {
      x = this.b[this.pos++];
      if (x === undefined) throw PatchError("The patch file is truncated.");
      data += (x & 0x7f) * shift;
      if (x & 0x80) break;
      shift *= 128;
      data += shift;
    }
    return data;
  };

  function readU32LE(b, o) {
    return ((b[o]) | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24)) >>> 0;
  }

  /* ── BPS ────────────────────────────────────────────────────────── */
  function applyBPS(source, patch) {
    var r = new Reader(patch, 4);
    var sourceSize = r.varint();
    var targetSize = r.varint();
    var metaSize = r.varint();
    r.pos += metaSize;

    var footer = patch.length - 12;
    if (footer < r.pos) throw PatchError("The patch file is truncated.");

    var srcCrc = readU32LE(patch, footer);
    var tgtCrc = readU32LE(patch, footer + 4);
    var ownCrc = readU32LE(patch, footer + 8);
    if (crc32(patch, 0, footer + 8) !== ownCrc) {
      throw PatchError("The patch file itself is corrupt — download it again.");
    }
    checkSource(source, sourceSize, srcCrc);

    var target = new Uint8Array(targetSize);
    var outPos = 0, srcRel = 0, tgtRel = 0;

    while (r.pos < footer) {
      var data = r.varint();
      var action = data % 4;
      var length = Math.floor(data / 4) + 1;

      if (action === 0) {                     // SourceRead
        while (length--) { target[outPos] = source[outPos]; outPos++; }
      } else if (action === 1) {              // TargetRead
        while (length--) target[outPos++] = patch[r.pos++];
      } else {                                // SourceCopy / TargetCopy
        var d = r.varint();
        var delta = (d % 2 ? -1 : 1) * Math.floor(d / 2);
        if (action === 2) {
          srcRel += delta;
          while (length--) target[outPos++] = source[srcRel++];
        } else {
          tgtRel += delta;
          // Byte at a time on purpose: the ranges are allowed to overlap,
          // which is how BPS encodes runs.
          while (length--) target[outPos++] = target[tgtRel++];
        }
      }
    }

    if (outPos !== targetSize || crc32(target) !== tgtCrc) {
      throw PatchError("The patched ROM failed its checksum. The patch may be damaged.");
    }
    return target;
  }

  /* ── UPS ────────────────────────────────────────────────────────── */
  function applyUPS(source, patch) {
    var r = new Reader(patch, 4);
    var inputSize = r.varint();
    var outputSize = r.varint();

    var footer = patch.length - 12;
    if (footer < r.pos) throw PatchError("The patch file is truncated.");

    var srcCrc = readU32LE(patch, footer);
    var tgtCrc = readU32LE(patch, footer + 4);
    var ownCrc = readU32LE(patch, footer + 8);
    if (crc32(patch, 0, footer + 8) !== ownCrc) {
      throw PatchError("The patch file itself is corrupt — download it again.");
    }
    checkSource(source, inputSize, srcCrc);

    var target = new Uint8Array(outputSize);
    target.set(source.subarray(0, Math.min(inputSize, outputSize)));

    var outPos = 0;
    while (r.pos < footer) {
      outPos += r.varint();
      for (;;) {
        var x = patch[r.pos++];
        if (x === 0) break;
        if (outPos < outputSize) target[outPos] ^= x;
        outPos++;
      }
      outPos++;
    }

    if (crc32(target) !== tgtCrc) {
      throw PatchError("The patched ROM failed its checksum. The patch may be damaged.");
    }
    return target;
  }

  /* ── IPS ────────────────────────────────────────────────────────── */
  function applyIPS(source, patch) {
    // IPS carries no checksums, so the base ROM can only be sanity-checked
    // against the GBA header. Two passes: size the output, then write it.
    var pos = 5, end = source.length;
    while (pos < patch.length) {
      if (magic(patch.subarray(pos, pos + 3), 3) === "EOF") break;
      var off = (patch[pos] << 16) | (patch[pos + 1] << 8) | patch[pos + 2];
      pos += 3;
      var size = (patch[pos] << 8) | patch[pos + 1];
      pos += 2;
      if (size === 0) {
        var run = (patch[pos] << 8) | patch[pos + 1];
        pos += 3;
        end = Math.max(end, off + run);
      } else {
        pos += size;
        end = Math.max(end, off + size);
      }
    }

    var target = new Uint8Array(end);
    target.set(source);

    pos = 5;
    while (pos < patch.length) {
      if (magic(patch.subarray(pos, pos + 3), 3) === "EOF") break;
      var o = (patch[pos] << 16) | (patch[pos + 1] << 8) | patch[pos + 2];
      pos += 3;
      var s = (patch[pos] << 8) | patch[pos + 1];
      pos += 2;
      if (s === 0) {
        var count = (patch[pos] << 8) | patch[pos + 1];
        pos += 2;
        var value = patch[pos++];
        target.fill(value, o, o + count);
      } else {
        target.set(patch.subarray(pos, pos + s), o);
        pos += s;
      }
    }
    return target;
  }

  /* ── base-ROM validation ────────────────────────────────────────── */
  function checkSource(source, expectedSize, expectedCrc) {
    if (source.length !== expectedSize) {
      throw PatchError(
        "That base ROM is the wrong size.",
        "The patch expects " + fmtBytes(expectedSize) + ", your file is " +
        fmtBytes(source.length) + ". A headered or trimmed dump won't work."
      );
    }
    if (crc32(source) !== expectedCrc) {
      throw PatchError(
        "That isn't the base ROM this patch expects.",
        "Needs CRC32 " + hex8(expectedCrc) + ", yours is " + hex8(crc32(source)) +
        ". You likely have a different region or revision."
      );
    }
  }

  function headerCode(bytes) {
    return bytes.length > 0xb0 ? magic(bytes.subarray(0xac, 0xb0), 4) : "";
  }

  function fmtBytes(n) {
    if (n >= 1048576) return (n / 1048576).toFixed(n % 1048576 ? 1 : 0) + " MB";
    if (n >= 1024) return Math.round(n / 1024) + " KB";
    return n + " bytes";
  }

  function detectAndApply(source, patch) {
    var m4 = magic(patch, 4);
    if (m4 === "BPS1") return applyBPS(source, patch);
    if (m4 === "UPS1") return applyUPS(source, patch);
    if (magic(patch, 5) === "PATCH") return applyIPS(source, patch);
    throw PatchError("That file isn't a patch.", "Expected a .bps, .ups or .ips file.");
  }

  /* ── UI ─────────────────────────────────────────────────────────── */
  var ui = {};
  var romBytes = null, romName = "";
  var patchBytes = null, patchName = "";
  var resultUrl = null;

  function $(sel) { return document.querySelector(sel); }

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

  function readFile(file) {
    return new Promise(function (resolve, reject) {
      var fr = new FileReader();
      fr.onload = function () { resolve(new Uint8Array(fr.result)); };
      fr.onerror = function () { reject(new Error("Couldn't read that file.")); };
      fr.readAsArrayBuffer(file);
    });
  }

  function refresh() {
    ui.apply.disabled = !(romBytes && patchBytes);
  }

  function setSlot(slot, name, note, ok) {
    slot.querySelector("[data-slot-name]").textContent = name;
    slot.querySelector("[data-slot-note]").textContent = note;
    slot.dataset.state = ok ? "ok" : "";
  }

  async function onRom(file) {
    if (!file) return;
    try {
      romBytes = await readFile(file);
    } catch (err) {
      say("bad", err.message);
      return;
    }
    romName = file.name;
    var code = headerCode(romBytes);
    var note = fmtBytes(romBytes.length) + (code ? " · " + code : "");
    setSlot(ui.romSlot, file.name, note, true);

    // A friendly nudge before the real checksum test runs on apply.
    if (code && PATCH.base.code && code !== PATCH.base.code) {
      say("warn", "That looks like the wrong game.",
        "The header says " + code + "; this patch is for " + PATCH.base.name +
        " (" + PATCH.base.code + "). You can still try.");
    } else {
      say("idle", "Ready when you are.");
    }
    refresh();
  }

  async function onPatch(file) {
    if (!file) return;
    try {
      patchBytes = await readFile(file);
    } catch (err) {
      say("bad", err.message);
      return;
    }
    patchName = file.name;
    setSlot(ui.patchSlot, file.name, fmtBytes(patchBytes.length), true);
    say("idle", "Ready when you are.");
    refresh();
  }

  function apply() {
    say("busy", "Patching…", "Large ROMs take a moment.");
    ui.apply.disabled = true;

    // Yield a frame so the status paints before the synchronous work.
    setTimeout(function () {
      var out;
      try {
        out = detectAndApply(romBytes, patchBytes);
      } catch (err) {
        say("bad", err.message, err.detail);
        ui.apply.disabled = false;
        return;
      }

      if (resultUrl) URL.revokeObjectURL(resultUrl);
      resultUrl = URL.createObjectURL(new Blob([out], { type: "application/octet-stream" }));
      ui.result.href = resultUrl;
      ui.result.download = PATCH.outputName;
      ui.result.hidden = false;
      ui.result.textContent = "Save " + PATCH.outputName + " (" + fmtBytes(out.length) + ")";
      say("ok", "Done — your patched ROM is ready.",
        "Nothing left this page; the file was built in your browser.");
      ui.apply.disabled = false;
    }, 30);
  }

  /* ── wiring ─────────────────────────────────────────────────────── */
  function init() {
    var root = $("[data-patcher]");
    if (!root) return;

    ui.status = root.querySelector("[data-patch-status]");
    ui.apply = root.querySelector("[data-patch-apply]");
    ui.result = root.querySelector("[data-patch-result]");
    ui.romSlot = root.querySelector("[data-slot=rom]");
    ui.patchSlot = root.querySelector("[data-slot=patch]");
    ui.romInput = root.querySelector("#rom-file");
    ui.patchInput = root.querySelector("#patch-file");

    ui.romInput.addEventListener("change", function () { onRom(this.files[0]); });
    ui.patchInput.addEventListener("change", function () { onPatch(this.files[0]); });
    ui.apply.addEventListener("click", apply);

    // Drag and drop onto either slot.
    [[ui.romSlot, onRom], [ui.patchSlot, onPatch]].forEach(function (pair) {
      var el = pair[0], handler = pair[1];
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
    });

    setSlot(ui.romSlot, "No file chosen",
      PATCH.base.name + " · " + fmtBytes(PATCH.base.size), false);

    // Try to load the published patch so step 2 is normally automatic.
    fetch(PATCH.url).then(function (res) {
      if (!res.ok) throw new Error(String(res.status));
      return res.arrayBuffer();
    }).then(function (buf) {
      patchBytes = new Uint8Array(buf);
      patchName = PATCH.url.split("/").pop();
      setSlot(ui.patchSlot, patchName, fmtBytes(patchBytes.length) + " · loaded from this page", true);
      root.dataset.patchReady = "1";
      document.querySelectorAll("[data-patch-link]").forEach(function (a) {
        a.href = PATCH.url;
        a.setAttribute("download", "");
        var meta = a.querySelector(".dl-meta");
        if (meta) meta.textContent = fmtBytes(patchBytes.length) + " ↓";
      });
      say("idle", "Choose your base ROM to begin.");
      refresh();
    }).catch(function () {
      // No published patch yet: fall back to letting the player supply one.
      setSlot(ui.patchSlot, "No file chosen", "Choose the .bps you downloaded", false);
      root.dataset.patchManual = "1";
      say("idle", "Choose your base ROM and the patch file to begin.");
      refresh();
    });

    refresh();
  }

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", init);
  } else {
    init();
  }

  // Exposed so the patch engine can be exercised directly against known
  // fixtures without driving the file inputs.
  window.WoTPatcher = {
    crc32: crc32,
    applyBPS: applyBPS,
    applyUPS: applyUPS,
    applyIPS: applyIPS,
    detectAndApply: detectAndApply
  };
})();
