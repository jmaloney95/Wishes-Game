/* Wishes of Tomorrow — landing page behaviour.
   Everything here is progressive enhancement: with JS off the page is still
   complete, just without the starfield, the tilt and the reveal-on-scroll. */
(function () {
  "use strict";

  /* ── the one thing to edit per release ──────────────────────────────
     The download buttons point at /releases/latest and never need touching.
     These two strings are only the labels shown on the page. */
  var RELEASE = {
    version: "1.0.0",
    date: "12 August 2026"
  };

  var reduced = window.matchMedia("(prefers-reduced-motion: reduce)").matches;

  /* ── release labels ─────────────────────────────────────────────── */
  function paintRelease() {
    var v = document.querySelector("[data-version]");
    if (v) v.textContent = "v" + RELEASE.version;

    var d = document.querySelector("[data-release-date]");
    if (d) d.textContent = "released " + RELEASE.date;

    var digits = document.querySelector("[data-version-digits]");
    if (!digits) return;
    digits.textContent = "";
    RELEASE.version.split("").forEach(function (ch) {
      var cell = document.createElement("span");
      if (ch === ".") cell.className = "dot";
      cell.textContent = ch;
      digits.appendChild(cell);
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
    var rail = document.querySelector("[data-cast-rail]");
    if (!rail) return;
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
  paintStars();
  wireCastRail();
  wireReveal();
  wireTilt();
})();
