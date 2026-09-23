/* Wishes of Tomorrow — landing page behaviour.
   Everything here is progressive enhancement: with JS off the page is still
   complete, just without the starfield, the tilt and the reveal-on-scroll. */
(function () {
  "use strict";

  /* ── the one thing to edit per release ──────────────────────────── */
  var RELEASE = {
    version: "1.2.4",
    date: "12 August 2026"
  };

  /* ── download counter ───────────────────────────────────────────────
     There is no server here, so the count is GitHub's own tally of release
     asset downloads, read back from the public API. The download button
     therefore points at the release asset rather than the copy this site
     serves — GitHub only counts the former.

     Two things this number does not include: people who patch with the
     in-page patcher (it reads the same-origin copy, because the release
     asset host sends no CORS headers), and repeat downloads GitHub
     de-duplicates. It is a floor, not a click count. */
  var COUNTER = {
    api: "https://api.github.com/repos/jmaloney95/Wishes-Game/releases",
    asset: /\.bps$/i,
    since: RELEASE.date,
    minDigits: 4,
    cacheKey: "wot:downloads",
    cacheMs: 10 * 60 * 1000   // be kind to the 60-requests-an-hour limit
  };

  var reduced = window.matchMedia("(prefers-reduced-motion: reduce)").matches;

  function $(sel) { return document.querySelector(sel); }

  /* ── release labels ─────────────────────────────────────────────── */
  function paintRelease() {
    var v = $("[data-version]");
    if (v) v.textContent = "v" + RELEASE.version;
  }

  function renderDigits(host, text) {
    host.textContent = "";
    text.split("").forEach(function (ch) {
      var cell = document.createElement("span");
      if (ch === "." || ch === ",") cell.className = "dot";
      cell.textContent = ch;
      host.appendChild(cell);
    });
  }

  function group(n) {
    return String(n).replace(/\B(?=(\d{3})+(?!\d))/g, ",");
  }

  function padded(n, minDigits) {
    var s = group(n);
    var digits = s.replace(/,/g, "").length;
    var min = minDigits || COUNTER.minDigits;
    while (digits < min) { s = "0" + s; digits++; }
    return s;
  }

  /* Fall back to what the chip used to show if the count can't be had. */
  function showVersionInstead() {
    var host = $("[data-count-digits]");
    if (!host) return;
    renderDigits(host, RELEASE.version);
    var title = $("[data-count-title]"), sub = $("[data-count-sub]"), sr = $("[data-count-sr]");
    if (title) title.textContent = "Current version";
    if (sub) sub.textContent = "released " + RELEASE.date;
    if (sr) sr.textContent = "Version " + RELEASE.version;
  }

  function cached() {
    try {
      var raw = window.localStorage.getItem(COUNTER.cacheKey);
      if (!raw) return null;
      var hit = JSON.parse(raw);
      if (!hit || typeof hit.n !== "number") return null;
      return (Date.now() - hit.t < COUNTER.cacheMs) ? hit.n : null;
    } catch (e) { return null; }
  }

  function remember(n) {
    try {
      window.localStorage.setItem(COUNTER.cacheKey, JSON.stringify({ n: n, t: Date.now() }));
    } catch (e) { /* private mode; not worth caring about */ }
  }

  function fetchCount() {
    var hit = cached();
    if (hit !== null) return Promise.resolve(hit);
    if (!window.fetch) return Promise.reject();

    return fetch(COUNTER.api, { headers: { Accept: "application/vnd.github+json" } })
      .then(function (res) {
        if (!res.ok) throw new Error(String(res.status));
        return res.json();
      })
      .then(function (releases) {
        // Sum every published patch across every release, so the number keeps
        // climbing rather than resetting when a new version ships.
        var total = 0;
        (releases || []).forEach(function (rel) {
          (rel.assets || []).forEach(function (a) {
            if (COUNTER.asset.test(a.name || "")) total += (a.download_count || 0);
          });
        });
        remember(total);
        return total;
      });
  }

  // srSuffix names the source in the screen-reader text, so the two chips
  // are not both read out as a bare "N downloads".
  function countUp(host, sr, target, srSuffix) {
    var paint = function (n) {
      renderDigits(host, padded(n));
      if (sr) sr.textContent = group(target) + (target === 1 ? " download" : " downloads") + (srSuffix || "");
    };
    if (reduced || !target || !window.requestAnimationFrame) { paint(target); return; }

    var t0 = null, dur = 1500, done = false;
    var tick = function (now) {
      if (t0 === null) t0 = now;
      var p = Math.min(1, (now - t0) / dur);
      paint(Math.round(target * (1 - Math.pow(1 - p, 3))));
      if (p < 1) requestAnimationFrame(tick); else done = true;
    };
    requestAnimationFrame(tick);

    // Background tabs throttle rAF to a standstill. The animation is a nicety;
    // showing the right number is not, so settle it either way.
    setTimeout(function () { if (!done) paint(target); }, dur + 500);
  }

  function wireCounter() {
    var chip = $("[data-counter]");
    var host = $("[data-count-digits]");
    if (!chip || !host) return;

    renderDigits(host, padded(0));

    fetchCount().then(function (total) {
      var sr = $("[data-count-sr]"), sub = $("[data-count-sub]");
      if (sub) sub.textContent = "since " + COUNTER.since;

      var started = false;
      var run = function () {
        if (started) return;
        started = true;
        countUp(host, sr, total, " from flatfootlabs.com");
      };

      if (!reduced && "IntersectionObserver" in window) {
        var io = new IntersectionObserver(function (entries) {
          if (entries.some(function (e) { return e.isIntersecting; })) { io.disconnect(); run(); }
        }, { threshold: 0.25 });
        io.observe(chip);
        // Don't let a throttled or never-delivered observer leave the counter
        // reading zero — the count-up is decoration, the number is not.
        setTimeout(run, 3000);
      } else {
        run();
      }
    }).catch(showVersionInstead);
  }

  /* -- Hackdex counter --
     Hackdex's own tally. A DIFFERENT number from the GitHub one above it, not
     a subset and not a superset, so the two are shown side by side and are
     never added together.

     It is READ FROM A FILE HERE, not scraped, because it cannot be scraped.
     hackdex.app sends no Access-Control-Allow-Origin, so a browser fetch is
     refused outright; and every path on that origin answers 429 with
     `x-vercel-mitigated: challenge` to anything that is not a real browser, so
     a scheduled server-side fetch gets the challenge page instead of the
     count. Both were tested against the live host. Update data/hackdex.json by
     hand.

     Deliberately NOT requested through the ?v= token: this figure changes far
     more often than the assets do, and coupling it to the token would mean
     bumping the token just to publish a number. */
  function wireHackdexCounter() {
    var chip = $("[data-hd-counter]");
    var host = $("[data-hd-digits]");
    if (!chip || !host || !window.fetch) return;

    // Cache-buster rather than the shared token, so the figure can be edited on
    // its own; no-store keeps a proxy from pinning yesterday's number.
    fetch("data/hackdex.json?t=" + Date.now(), { cache: "no-store" })
      .then(function (res) {
        if (!res.ok) throw new Error(String(res.status));
        return res.json();
      })
      .then(function (data) {
        var n = data && data.downloads;
        if (typeof n !== "number" || !(n > 0)) throw new Error("no count");

        var sub = $("[data-hd-sub]"), sr = $("[data-hd-sr]");
        if (sub) sub.textContent = data.asOf ? "as of " + data.asOf : "";
        chip.hidden = false;

        var started = false;
        var run = function () {
          if (started) return;
          started = true;
          countUp(host, sr, n, " on Hackdex");
        };

        if (!reduced && "IntersectionObserver" in window) {
          var io = new IntersectionObserver(function (entries) {
            if (entries.some(function (e) { return e.isIntersecting; })) { io.disconnect(); run(); }
          }, { threshold: 0.25 });
          io.observe(chip);
          setTimeout(run, 3000);
        } else {
          run();
        }
      })
      .catch(function () {
        // No file, no number: leave the chip hidden rather than show a zero.
        chip.hidden = true;
      });
  }

  /* ── starfield ──────────────────────────────────────────────────── */
  function paintStars() {
    var host = document.getElementById("stars");
    if (!host || host.childElementCount) return;
    var frag = document.createDocumentFragment();
    for (var i = 0; i < 40; i++) {
      var s = document.createElement("span");
      var size = Math.random() < 0.2 ? 3 : 2;
      s.style.width = size + "px";
      s.style.height = size + "px";
      s.style.left = (Math.random() * 100).toFixed(2) + "%";
      s.style.top = (Math.random() * 100).toFixed(2) + "%";
      s.style.opacity = ".3";
      if (!reduced) {
        s.style.animation = "twinkle " + (2 + Math.random() * 5).toFixed(1) + "s ease-in-out infinite";
        s.style.animationDelay = "-" + (Math.random() * 6).toFixed(1) + "s";
      }
      frag.appendChild(s);
    }
    host.appendChild(frag);
  }

  /* ── horizontal cast rail: let a vertical wheel scroll it sideways ── */
  function wireCastRail() {
    // Every [data-rail] gets this, not just the cast: the screenshots are a
    // rail too now, and a page with two of them should behave the same on both.
    Array.prototype.forEach.call(document.querySelectorAll("[data-rail]"), function (rail) {
      rail.addEventListener("wheel", function (e) {
        if (Math.abs(e.deltaX) > Math.abs(e.deltaY)) return;
        var max = rail.scrollWidth - rail.clientWidth;
        // Only hijack the wheel while the rail still has somewhere to go,
        // so the page keeps scrolling normally at either end.
        if ((e.deltaY < 0 && rail.scrollLeft > 0) || (e.deltaY > 0 && rail.scrollLeft < max)) {
          e.preventDefault();
          rail.scrollLeft += e.deltaY;
        }
      }, { passive: false });
    });
  }

  /* ── reveal on scroll ───────────────────────────────────────────── */
  function wireReveal() {
    var targets = document.querySelectorAll("[data-reveal]");
    if (reduced || !("IntersectionObserver" in window)) return;

    var io = new IntersectionObserver(function (entries) {
      var shown = 0;
      entries.forEach(function (e) {
        if (!e.isIntersecting) return;
        // Stagger, but cap it: a tall section can put a dozen targets on
        // screen at once, and an uncapped i*60 leaves the last one blank
        // for the best part of a second.
        e.target.style.transitionDelay = Math.min(shown++ * 60, 360) + "ms";
        e.target.style.opacity = "1";
        e.target.style.transform = "none";
        io.unobserve(e.target);
      });
    }, { threshold: 0.1, rootMargin: "0px 0px -6% 0px" });

    targets.forEach(function (el) {
      el.style.opacity = "0";
      el.style.transform = "translateY(20px)";
      el.style.transition = "opacity .7s cubic-bezier(.2,.8,.3,1), transform .7s cubic-bezier(.2,.8,.3,1)";
      io.observe(el);
    });
  }

  /* ── trailer: load YouTube only when asked ────────────────
     The markup is a link to the video wrapped round a poster, so it works
     with JS off and with the embed blocked. Here we intercept the click and
     swap the link for a nocookie iframe, which is the first moment anything
     is requested from YouTube. Modified and middle clicks fall through to
     the link, because that is what they are for. */
  function wireTrailer() {
    var link = $("[data-trailer]");
    if (!link) return;
    link.addEventListener("click", function (ev) {
      if (ev.button !== 0 || ev.metaKey || ev.ctrlKey || ev.shiftKey || ev.altKey) return;
      ev.preventDefault();

      var frame = document.createElement("iframe");
      frame.src = "https://www.youtube-nocookie.com/embed/" +
                  link.getAttribute("data-trailer") +
                  "?autoplay=1&rel=0&modestbranding=1";
      frame.title = "Wishes of Tomorrow \u2014 official trailer";
      frame.allow = "accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture";
      frame.allowFullscreen = true;

      var stage = document.createElement("div");
      stage.className = "trailer";
      stage.appendChild(frame);
      link.parentNode.replaceChild(stage, link);
    });
  }

  /* ── pointer tilt on the hero art ───────────────────────────────── */
  function wireTilt() {
    if (reduced) return;
    document.querySelectorAll("[data-tilt]").forEach(function (el) {
      el.addEventListener("pointermove", function (ev) {
        if (ev.pointerType === "touch") return;
        var r = el.getBoundingClientRect();
        var x = (ev.clientX - r.left) / r.width - 0.5;
        var y = (ev.clientY - r.top) / r.height - 0.5;
        el.style.transform =
          "perspective(900px) rotateX(" + (-y * 4).toFixed(2) + "deg) rotateY(" + (x * 4.5).toFixed(2) + "deg)";
      });
      el.addEventListener("pointerleave", function () { el.style.transform = "none"; });
    });
  }

  paintRelease();
  wireCounter();
  wireHackdexCounter();
  paintStars();
  wireCastRail();
  wireReveal();
  wireTilt();
  wireTrailer();
})();
