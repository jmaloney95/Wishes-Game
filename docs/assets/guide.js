/* Wishes of Tomorrow — the field guide.

   One map at a time, chosen by the address's hash (#m1 … #a3_15), so a map can be
   linked to and the back button walks back through the maps you looked at. The page
   ships with the first map drawn in; this keeps the title, the map, its route and
   pins, the stop cards, the act tabs, the select and the previous/next links in step
   with the hash. The maps' data is the JSON in #guide-data, baked from the guide
   editor's layout file. */
(function () {
  "use strict";

  var dataEl = document.getElementById("guide-data");
  if (!dataEl) return;
  var MAPS = JSON.parse(dataEl.textContent);
  var index = {};
  MAPS.forEach(function (m, i) { index[m.id] = i; });

  var $ = function (sel) { return document.querySelector(sel); };
  var $$ = function (sel) { return Array.prototype.slice.call(document.querySelectorAll(sel)); };

  var head = $("[data-guide-head]");
  var kicker = $("[data-guide-kicker]");
  var title = $("[data-guide-title]");
  var fig = $("[data-guide-fig]");
  var img = $("[data-guide-img]");
  var routeKey = $("[data-guide-routekey]");
  var count = $("[data-guide-count]");
  var stops = $("[data-guide-stops]");
  var empty = $("[data-guide-empty]");
  var select = $("[data-guide-select]");
  var prevBtn = $("[data-guide-prev]");
  var nextBtn = $("[data-guide-next]");
  var prevLink = $("[data-guide-prevlink]");
  var nextLink = $("[data-guide-nextlink]");
  var tabs = $$("[data-guide-acts] [data-act]");
  var logs = $$("[data-guide-log]");
  var baseTitle = document.title;
  var reduced = window.matchMedia && window.matchMedia("(prefers-reduced-motion: reduce)").matches;
  var current = -1;

  function el(tag, cls, text) {
    var e = document.createElement(tag);
    if (cls) e.className = cls;
    if (text != null) e.textContent = text;
    return e;
  }

  // The map log shows one act at a time; the tabs only change which act is listed.
  function showAct(act) {
    act = String(act);
    tabs.forEach(function (t) { t.setAttribute("aria-pressed", String(t.getAttribute("data-act") === act)); });
    logs.forEach(function (l) { l.hidden = l.getAttribute("data-act") !== act; });
  }

  function setNav(link, m) {
    link.hidden = !m;
    if (!m) return;
    link.href = "#" + m.id;
    link.querySelector(".ff-mapnav-name").textContent = m.name;
  }

  function render(i, navigated) {
    var m = MAPS[i];
    var inAct = MAPS.filter(function (x) { return x.act === m.act; });
    kicker.textContent = "Field guide · Act " + m.act + " · Map " + (inAct.indexOf(m) + 1) + " of " + inAct.length;

    // The map's name in two colours: the last word in violet, like every headline.
    var words = m.name.split(" ");
    var last = words.pop();
    title.textContent = words.length ? words.join(" ") + " " : "";
    title.appendChild(el("em", null, last + "."));

    img.src = m.src;
    img.width = m.w;
    img.height = m.h;
    img.alt = "Map of " + m.name;

    fig.querySelectorAll("polyline").forEach(function (p) { p.setAttribute("points", m.path); });
    routeKey.hidden = !m.path;

    // Pins sit over the SVG as HTML so they stay square on stretched maps.
    Array.prototype.slice.call(fig.querySelectorAll(".ff-pin")).forEach(function (p) { p.remove(); });
    stops.textContent = "";
    m.stops.forEach(function (s, n) {
      var pin = el("span", "ff-pin", String(n + 1));
      pin.setAttribute("aria-hidden", "true");
      pin.style.left = s.x + "%";
      pin.style.top = s.y + "%";
      fig.appendChild(pin);

      var card = el("li", "ff-stop");
      card.appendChild(el("span", "ff-stop-num", String(n + 1)));
      var body = el("div");
      body.appendChild(el("h3", null, s.t || "Stop " + (n + 1)));
      if (s.d) body.appendChild(el("p", null, s.d));
      card.appendChild(body);
      stops.appendChild(card);
    });
    stops.hidden = !m.stops.length;
    empty.hidden = !!m.stops.length;
    count.textContent = m.stops.length === 1 ? "1 stop" : m.stops.length + " stops";

    $$("[data-guide-log] a").forEach(function (a) {
      if (a.getAttribute("href") === "#" + m.id) a.setAttribute("aria-current", "true");
      else a.removeAttribute("aria-current");
    });
    showAct(m.act);

    select.value = m.id;
    var prev = MAPS[i - 1], next = MAPS[i + 1];
    prevBtn.disabled = !prev;
    nextBtn.disabled = !next;
    setNav(prevLink, prev);
    setNav(nextLink, next);

    document.title = m.name + " — " + baseTitle;
    // Picked from further down the page (the previous/next links): bring the new map's name into view.
    if (navigated && head.getBoundingClientRect().top < 0) {
      head.scrollIntoView({ behavior: reduced ? "auto" : "smooth", block: "start" });
    }
    current = i;
  }

  function fromHash() {
    var id = decodeURIComponent(location.hash.slice(1));
    return Object.prototype.hasOwnProperty.call(index, id) ? index[id] : 0;
  }

  window.addEventListener("hashchange", function () { render(fromHash(), true); });
  tabs.forEach(function (t) {
    t.addEventListener("click", function () { showAct(t.getAttribute("data-act")); });
  });
  select.addEventListener("change", function () { location.hash = select.value; });
  prevBtn.addEventListener("click", function () { if (current > 0) location.hash = MAPS[current - 1].id; });
  nextBtn.addEventListener("click", function () { if (current < MAPS.length - 1) location.hash = MAPS[current + 1].id; });

  render(fromHash(), false);
})();
