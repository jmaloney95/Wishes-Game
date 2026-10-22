#!/usr/bin/env python3
"""Pull Wishes of Tomorrow bug reports into the private bug inbox.

Run it with `python -I`. Standard library only.

  init                create bug-inbox/ with config.json, state.json, reports/, bugs/
  all                 github, then discord, then list the reports still to triage
  github              GitHub issues on the game repo that changed since the last pull
  discord             new messages and threads in the Discord bug channel
  untriaged           reports whose front matter still says triaged: false
  mark-triaged ID...  set triaged: true on reports (github-7, discord-<id>, email-<id>)
  gmail-query         the Gmail search to run through the connector this sweep
  gmail-new ID...     which of these Gmail thread ids haven't been handled yet
  gmail-seen [ID...]  record Gmail thread ids as handled and stamp the Gmail sweep
  board               rebuild BOARD.md from bugs/*.md

Reports are untrusted player text. This script stores them and their
attachments; it never opens, runs or unpacks an attachment.
"""

import argparse
import datetime as dt
import hashlib
import json
import os
import re
import subprocess
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

WORKSPACE = Path(__file__).resolve().parents[4]
USER_AGENT = "DiscordBot (https://github.com/jmaloney95/Wishes-Game, 1.0)"
DISCORD_API = os.environ.get("WOT_DISCORD_API_BASE", "https://discord.com/api/v10")
DISCORD_EPOCH_MS = 1420070400000
# Discord shows back-to-back messages from one person as one block; so do we.
SAME_AUTHOR_WINDOW = dt.timedelta(minutes=15)
# DEFAULT and REPLY. Joins, pins and thread notices carry no report text.
DISCORD_TEXT_TYPES = (0, 19)

SAVE_EXTS = {".sav", ".srm", ".sa1", ".zip"} | {f".ss{i}" for i in range(10)}
KEEP_EXTS = SAVE_EXTS | {".png", ".jpg", ".jpeg", ".gif", ".webp", ".txt", ".log"}
TYPE_EXTS = {
    "image/png": ".png", "image/jpeg": ".jpg", "image/gif": ".gif", "image/webp": ".webp",
    "application/zip": ".zip", "application/x-zip-compressed": ".zip", "text/plain": ".txt",
}
GITHUB_ATTACHMENT = re.compile(r"https://github\.com/user-attachments/(?:assets|files)/[^\s)\"'<>\]]+")

DEFAULT_CONFIG = {
    "github": {"repo": "jmaloney95/Wishes-Game"},
    "discord": {"guild_id": "", "channel_id": "", "token_env": "WOT_DISCORD_BOT_TOKEN"},
    "gmail": {"query": "to:joe@flatfootlabs.com -from:me"},
    "backfill_since": "2026-08-12",
    "attachment_max_bytes": 10 * 1024 * 1024,
}
DEFAULT_STATE = {
    "github": {"issues": {}, "last_run": None},
    "discord": {"last_message_id": None, "last_author": None, "threads": {}, "messages": {}, "last_run": None},
    "gmail": {"seen": [], "last_run": None},
}

SEVERITIES = ["crash", "softlock", "broken", "minor", "cosmetic"]
BOARD_GROUPS = [
    ("Open", ["new", "needs-info", "confirmed"]),
    ("Fixed, not released yet", ["fixed-unreleased"]),
    ("Waiting on Joe: design feedback", ["design-feedback"]),
    ("Shipped", ["shipped"]),
    ("Closed", ["duplicate", "already-fixed", "not-a-bug", "wontfix"]),
]

INBOX_README = """\
# Bug inbox (private)

Player reports from GitHub, Discord and email, kept on this machine only. The
folder is in .gitignore because reports carry names, email addresses and save
files.

- `BOARD.md`: every bug by status. The sweep rebuilds it, so edit the bug files instead.
- `bugs/BUG-NNN-<slug>.md`: one bug each, with duplicate reports merged.
- `reports/<source>-<id>/report.md`: one raw report, word for word. Its `files/`
  folder holds the player's saves and screenshots.
- `config.json`: where the reports come from. It holds no secrets.
- `state.json`: where each source left off.

Run `/bug-sweep` to pull and triage new reports, and `/bug-fix BUG-NNN` to fix
one. Attachments are players' files: look at them, never run them.
"""


class PullError(Exception):
    """A source failed. The message is safe to print."""


class Skip(Exception):
    """A source isn't set up yet. The message says what's missing."""


# ---------------------------------------------------------------- files

def write_text(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    tmp = path.with_name(path.name + ".tmp")
    with open(tmp, "w", encoding="utf-8", newline="\n") as f:
        f.write(text)
    os.replace(tmp, path)


def write_json(path, data):
    write_text(path, json.dumps(data, indent=2, ensure_ascii=False) + "\n")


def load_json(path):
    return json.loads(path.read_text(encoding="utf-8"))


def merge(defaults, loaded):
    """Defaults with `loaded` on top, so keys added later get their default."""
    out = json.loads(json.dumps(defaults))
    for key, value in (loaded or {}).items():
        if isinstance(value, dict) and isinstance(out.get(key), dict):
            out[key] = merge(out[key], value)
        else:
            out[key] = value
    return out


def safe_name(name, fallback="file"):
    name = re.sub(r"[^A-Za-z0-9._-]+", "_", name).strip("._") or fallback
    return name[:120]


def now_iso():
    return dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def parse_ts(text):
    return dt.datetime.fromisoformat(text.replace("Z", "+00:00"))


def tidy(text):
    return (text or "").replace("\r\n", "\n").replace("\r", "\n").strip()


def first_line(text, limit=80):
    for line in tidy(text).splitlines():
        line = line.strip(" #*_>`")
        if line:
            return line if len(line) <= limit else line[:limit - 1].rstrip() + "…"
    return ""


def quote(text):
    return "\n".join("> " + line if line else ">" for line in tidy(text).splitlines())


class Inbox:
    def __init__(self, root):
        self.root = Path(root)
        self.reports = self.root / "reports"
        self.bugs = self.root / "bugs"
        self.config_path = self.root / "config.json"
        self.state_path = self.root / "state.json"
        self.config = self.state = None

    def load(self):
        if not self.config_path.exists():
            raise SystemExit(f"No bug inbox at {self.root}. Run: pull_reports.py init")
        self.config = merge(DEFAULT_CONFIG, load_json(self.config_path))
        saved = load_json(self.state_path) if self.state_path.exists() else {}
        self.state = merge(DEFAULT_STATE, saved)
        return self

    def save_state(self):
        write_json(self.state_path, self.state)


# ---------------------------------------------------------- front matter
# One `key: value` per line between --- fences. Lists are [a, b]. A value
# that starts with a double quote is a JSON string. Hand-written bug files use
# the same format, so the parser stays forgiving.

def fm_dump(value):
    if value is None:
        return ""
    if isinstance(value, bool):
        return "true" if value else "false"
    if isinstance(value, (list, tuple)):
        return "[" + ", ".join(fm_item(v) for v in value) + "]"
    text = str(value)
    if "\n" in text or text != text.strip() or text[:1] in ('"', "[") or text in ("true", "false"):
        return json.dumps(text, ensure_ascii=False)
    return text


def fm_item(value):
    text = str(value)
    return text if re.fullmatch(r"[^,\[\]\"\s]+", text) else json.dumps(text, ensure_ascii=False)


def fm_load(raw):
    if raw == "":
        return None
    if raw in ("true", "false"):
        return raw == "true"
    if raw.startswith('"'):
        try:
            return json.loads(raw)
        except ValueError:
            return raw.strip('"')
    if raw.startswith("[") and raw.endswith("]"):
        items = re.findall(r'"(?:[^"\\]|\\.)*"|[^,]+', raw[1:-1])
        return [json.loads(i) if i.startswith('"') else i.strip() for i in (x.strip() for x in items) if i]
    return raw


def read_front_matter(path):
    text = path.read_text(encoding="utf-8").replace("\r\n", "\n")
    if not text.startswith("---\n"):
        return {}, text
    end = text.find("\n---\n", 3)
    if end < 0:
        return {}, text
    meta = {}
    for line in text[4:end].splitlines():
        key, sep, raw = line.partition(":")
        if sep and key.strip() and not key.lstrip().startswith("#"):
            meta[key.strip()] = fm_load(raw.strip())
    return meta, text[end + 5:]


def render_report(meta, body):
    lines = ["---", *(f"{k}: {fm_dump(v)}" for k, v in meta.items()), "---", "", ""]
    return "\n".join(lines) + body.rstrip() + "\n"


def report_title(body):
    return next((line[2:].strip() for line in body.splitlines() if line.startswith("# ")), "")


# ------------------------------------------------------------ downloads

def download(url, dest_dir, name, cap, headers=None):
    """Save one attachment as plain data. Returns (path under the report, None) or (None, reason)."""
    request = urllib.request.Request(url, headers={"User-Agent": USER_AGENT, **(headers or {})})
    try:
        with urllib.request.urlopen(request, timeout=60) as resp:
            ext = Path(name).suffix.lower()
            if ext not in KEEP_EXTS:
                guessed = TYPE_EXTS.get(resp.headers.get_content_type(), "")
                if guessed:
                    name, ext = name + guessed, guessed
            if ext not in KEEP_EXTS:
                return None, f"not saved ({resp.headers.get_content_type()}); open the link to see it"
            size = resp.headers.get("Content-Length")
            if size and size.isdigit() and int(size) > cap:
                return None, f"not saved, too big ({int(size) // 1024} KB)"
            data = resp.read(cap + 1)
    except urllib.error.HTTPError as e:
        return None, f"not saved (HTTP {e.code})"
    except (urllib.error.URLError, TimeoutError, OSError) as e:
        return None, f"not saved ({getattr(e, 'reason', None) or e.__class__.__name__})"
    if len(data) > cap:
        return None, "not saved, over the size cap"
    path = dest_dir / safe_name(name)
    dest_dir.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)
    return f"files/{path.name}", None


# --------------------------------------------------------------- GitHub

def gh_json(args):
    try:
        out = subprocess.run(["gh", *args], capture_output=True, text=True, encoding="utf-8", errors="replace")
    except FileNotFoundError:
        raise PullError("the GitHub CLI (gh) isn't on PATH")
    if out.returncode != 0:
        raise PullError("gh failed: " + (out.stderr.strip().splitlines() or ["no output"])[-1])
    return json.loads(out.stdout)


def comment_key(comment):
    return comment.get("id") or comment.get("url") or comment.get("createdAt")


def issue_hash(issue):
    """Changes when the text does. Label edits and closing a duplicate don't requeue the report."""
    data = [issue.get("title"), issue.get("body"),
            [(comment_key(c), c.get("body")) for c in issue.get("comments") or []]]
    return hashlib.sha1(json.dumps(data, ensure_ascii=False).encode("utf-8")).hexdigest()


def github_attachment_name(url):
    # assets/<uuid> -> <uuid>; files/<id>/<name> -> <id>-<name>
    parts = urllib.parse.urlparse(url).path.strip("/").split("/")
    return "-".join(parts[2:]) or parts[-1]


def pull_github(inbox):
    repo = inbox.config["github"]["repo"]
    cap = int(inbox.config["attachment_max_bytes"])
    fields = "number,title,body,author,createdAt,updatedAt,labels,comments,url,state"
    issues = gh_json(["issue", "list", "-R", repo, "--state", "all", "--limit", "1000", "--json", fields])
    seen = inbox.state["github"]["issues"]
    changed = []
    for issue in sorted(issues, key=lambda i: i["number"]):
        key = str(issue["number"])
        prev = seen.get(key)
        digest = issue_hash(issue)
        if prev and prev.get("hash") == digest:
            prev["updated"] = issue["updatedAt"]
            continue
        rid = f"github-{key}"
        folder = inbox.reports / rid
        old = load_json(folder / "source.json") if (folder / "source.json").exists() else {}
        files = old.get("files", {})
        comment_ids = [comment_key(c) for c in issue.get("comments") or []]
        known = set(prev.get("comments", [])) if prev else None
        new_ids = [c for c in comment_ids if known is not None and c not in known]
        text = "\n".join([issue.get("body") or ""] + [c.get("body") or "" for c in issue.get("comments") or []])
        for url in dict.fromkeys(GITHUB_ATTACHMENT.findall(text)):
            got = files.get(url) or {}
            if got.get("path") and (folder / got["path"]).exists():
                continue
            name = github_attachment_name(url)
            path, err = download(url, folder / "files", name, cap)
            files[url] = {"name": name, "path": path, "error": err}
        write_json(folder / "source.json", {"issue": issue, "files": files, "new_comment_ids": new_ids})
        write_text(folder / "report.md", render_github(rid, issue, files, set(new_ids)))
        seen[key] = {"updated": issue["updatedAt"], "hash": digest, "comments": comment_ids}
        changed.append(("new" if not prev else "updated", rid, issue["title"]))
    inbox.state["github"]["last_run"] = now_iso()
    inbox.save_state()
    return changed


def render_github(rid, issue, files, new_ids):
    meta = {
        "source": "github",
        "id": rid,
        "url": issue["url"],
        "author": (issue.get("author") or {}).get("login") or "ghost",
        "created": issue["createdAt"],
        "updated": issue["updatedAt"],
        "state": (issue.get("state") or "").lower(),
        "labels": [label["name"] for label in issue.get("labels") or []],
        "reply_via": "a comment on the GitHub issue, with Joe's OK",
        "triaged": False,
    }
    parts = [f"# {issue['title']}", "", tidy(issue.get("body")) or "_(no description)_"]
    comments = issue.get("comments") or []
    if comments:
        parts += ["", "## Comments"]
        for c in comments:
            who = (c.get("author") or {}).get("login") or "ghost"
            flag = " · NEW since the last pull" if comment_key(c) in new_ids else ""
            parts += ["", f"### {who} · {c.get('createdAt')}{flag}", "", tidy(c.get("body")) or "_(empty)_"]
    if files:
        parts += ["", "## Files"]
        for url, f in files.items():
            parts.append(f"- `{f['path']}` (from {url})" if f.get("path") else f"- {url}: {f.get('error')}")
    return render_report(meta, "\n".join(parts))


# -------------------------------------------------------------- Discord

def snowflake_at(date_text):
    day = dt.datetime.fromisoformat(date_text[:10]).replace(tzinfo=dt.timezone.utc)
    return str((int(day.timestamp() * 1000) - DISCORD_EPOCH_MS) << 22)


class DiscordApi:
    def __init__(self, token):
        self._auth = "Bot " + token

    def get(self, path, params=None):
        url = DISCORD_API + path + ("?" + urllib.parse.urlencode(params) if params else "")
        for _ in range(6):
            request = urllib.request.Request(url, headers={"Authorization": self._auth, "User-Agent": USER_AGENT})
            try:
                with urllib.request.urlopen(request, timeout=30) as resp:
                    return json.loads(resp.read().decode("utf-8"))
            except urllib.error.HTTPError as e:
                body = e.read()
                if e.code == 429:
                    try:
                        wait = float(json.loads(body or b"{}").get("retry_after", 1))
                    except ValueError:
                        wait = 1.0
                    time.sleep(min(wait, 30) + 0.25)
                    continue
                if e.code == 401:
                    raise PullError("the bot token was rejected (401). Reset it in the developer portal "
                                    "and update the environment variable")
                if e.code == 403:
                    raise PullError(f"the bot can't read {path} (403). Give its role View Channels and "
                                    "Read Message History on the bug channel")
                if e.code == 404:
                    return None
                raise PullError(f"HTTP {e.code} from {path}")
            except (urllib.error.URLError, TimeoutError) as e:
                raise PullError(f"couldn't reach Discord ({getattr(e, 'reason', None) or e})")
        raise PullError("still rate limited after several tries; run it again in a minute")

    def messages(self, channel_id, after):
        """Every message after the `after` id, oldest first. Discord pages newest-first."""
        out = []
        while True:
            batch = self.get(f"/channels/{channel_id}/messages", {"after": after, "limit": 100}) or []
            if not batch:
                break
            batch.sort(key=lambda m: int(m["id"]))
            out.extend(batch)
            after = batch[-1]["id"]
            if len(batch) < 100:
                break
        return out

    def threads(self, guild_id, channel_id, since):
        """The channel's threads: every active one, and archived ones back to `since`."""
        found = {}
        active = self.get(f"/guilds/{guild_id}/threads/active") or {}
        for thread in active.get("threads", []):
            if thread.get("parent_id") == channel_id:
                found[thread["id"]] = thread
        before = None
        for _ in range(20):
            params = {"limit": 50, **({"before": before} if before else {})}
            page = self.get(f"/channels/{channel_id}/threads/archived/public", params) or {}
            batch = page.get("threads", [])
            for thread in batch:
                found.setdefault(thread["id"], thread)
            if not page.get("has_more") or not batch:
                break
            before = min(t.get("thread_metadata", {}).get("archive_timestamp", "") for t in batch)
            if not before or before < since:
                break
        return sorted(found.items(), key=lambda kv: int(kv[0]))


def trim_message(m):
    author = m.get("author") or {}
    return {
        "id": m["id"],
        "channel_id": m.get("channel_id"),
        "type": m.get("type", 0),
        "timestamp": m.get("timestamp"),
        "edited_timestamp": m.get("edited_timestamp"),
        "author": {k: author.get(k) for k in ("id", "username", "global_name", "bot")},
        "content": m.get("content") or "",
        "mentions": [{"id": u.get("id"), "name": u.get("global_name") or u.get("username")}
                     for u in m.get("mentions") or []],
        "attachments": [{k: a.get(k) for k in ("id", "filename", "size", "url", "content_type")}
                        for a in m.get("attachments") or []],
        "embeds": [{k: e.get(k) for k in ("title", "description", "url", "fields")} for e in m.get("embeds") or []],
        "reply_to": (m.get("message_reference") or {}).get("message_id") if m.get("type") == 19 else None,
    }


def discord_name(author):
    name = author.get("global_name") or author.get("username") or "unknown"
    user = author.get("username")
    return f"{name} (@{user})" if user and user != name else name


def jump(guild, channel, message):
    return f"https://discord.com/channels/{guild}/{channel}/{message}"


def expand_mentions(m):
    text = m.get("content") or ""
    for user in m.get("mentions") or []:
        text = re.sub(rf"<@!?{re.escape(str(user.get('id')))}>", "@" + (user.get("name") or "user"), text)
    return tidy(text)


def save_discord_attachment(att, dest_dir, cap):
    info = {"name": att.get("filename"), "path": None, "error": None}
    ext = Path(att.get("filename") or "").suffix.lower()
    if ext not in KEEP_EXTS:
        info["error"] = "not saved (file type); open the message to see it"
    elif (att.get("size") or 0) > cap:
        info["error"] = f"not saved, too big ({(att.get('size') or 0) // 1024} KB)"
    else:
        name = f"{att['id']}-{att.get('filename') or 'file'}"
        info["path"], info["error"] = download(att["url"], dest_dir, name, cap)
    return info


class DiscordStore:
    """Collects one run's messages per report, then writes each touched report once."""

    def __init__(self, inbox, guild):
        self.inbox, self.guild = inbox, guild
        self.sidecars = {}
        self.touched = {}

    def sidecar(self, rid):
        if rid not in self.sidecars:
            path = self.inbox.reports / rid / "source.json"
            if path.exists():
                data = load_json(path)
                data["new_ids"] = []
            else:
                data = {"guild_id": self.guild, "title": None, "messages": [], "files": {}, "new_ids": []}
            self.sidecars[rid] = data
        return self.sidecars[rid]

    def add(self, rid, message, title=None):
        data = self.sidecar(rid)
        kind = "updated" if (self.inbox.reports / rid / "report.md").exists() else "new"
        if title and not data.get("title"):
            data["title"] = title
            self.touched.setdefault(rid, kind)
        if any(m["id"] == message["id"] for m in data["messages"]):
            return
        data["messages"].append(trim_message(message))
        data["messages"].sort(key=lambda m: int(m["id"]))
        data["new_ids"].append(message["id"])
        self.touched.setdefault(rid, kind)
        self.inbox.state["discord"]["messages"][message["id"]] = rid

    def title(self, rid):
        data = self.sidecar(rid)
        first = data["messages"][0] if data["messages"] else {}
        return data.get("title") or first_line(expand_mentions(first)) or "Discord report"

    def flush(self):
        cap = int(self.inbox.config["attachment_max_bytes"])
        for rid in self.touched:
            data = self.sidecars[rid]
            if not data["messages"]:
                continue
            folder = self.inbox.reports / rid
            for m in data["messages"]:
                for att in m.get("attachments") or []:
                    if att["id"] not in data["files"]:
                        data["files"][att["id"]] = save_discord_attachment(att, folder / "files", cap)
            write_json(folder / "source.json", data)
            write_text(folder / "report.md", render_discord(rid, data, self.title(rid)))
        return [(kind, rid, self.title(rid)) for rid, kind in self.touched.items() if self.sidecars[rid]["messages"]]


def render_discord(rid, data, title):
    guild, messages = data["guild_id"], data["messages"]
    first = messages[0]
    meta = {
        "source": "discord",
        "id": rid,
        "url": jump(guild, first["channel_id"], first["id"]),
        "author": discord_name(first["author"]),
        "author_id": first["author"].get("id"),
        "created": first["timestamp"],
        "updated": messages[-1]["timestamp"],
        "messages": len(messages),
        "reply_via": "Joe replies in Discord; Claude drafts the text",
        "triaged": False,
    }
    new = set(data.get("new_ids") or [])
    flag_new = len(new) < len(messages)
    parts = [f"# {title}"]
    for m in messages:
        when = (m.get("timestamp") or "")[:16].replace("T", " ")
        head = f"### {discord_name(m['author'])} · {when} UTC · [message]({jump(guild, m['channel_id'], m['id'])})"
        if m.get("reply_to"):
            head += " · reply"
        if flag_new and m["id"] in new:
            head += " · NEW since the last pull"
        parts += ["", head, "", expand_mentions(m) or "_(no text)_"]
        for embed in m.get("embeds") or []:
            bits = [b for b in (embed.get("title"), embed.get("description")) if b]
            bits += [f"{f.get('name')}: {f.get('value')}" for f in embed.get("fields") or []]
            if bits:
                parts += ["", quote("embed: " + "\n".join(bits))]
        for att in m.get("attachments") or []:
            got = data["files"].get(att["id"]) or {}
            if got.get("path"):
                parts.append(f"- attachment `{got['path']}` ({att.get('filename')})")
            else:
                parts.append(f"- attachment {att.get('filename')}: {got.get('error') or 'pending'}")
    return render_report(meta, "\n".join(parts))


def channel_report_for(state, m):
    """Which report a channel message belongs to: its reply target, the author's block, or its own."""
    ref = (m.get("message_reference") or {}).get("message_id") if m.get("type") == 19 else None
    if ref:
        return state["messages"].get(ref) or f"discord-{m['id']}"
    last = state.get("last_author") or {}
    author = (m.get("author") or {}).get("id")
    if (last.get("id") == author and last.get("rid") and last.get("ts")
            and parse_ts(m["timestamp"]) - parse_ts(last["ts"]) <= SAME_AUTHOR_WINDOW):
        return last["rid"]
    return f"discord-{m['id']}"


def pull_discord(inbox):
    cfg = inbox.config["discord"]
    guild = str(cfg.get("guild_id") or "").strip()
    channel = str(cfg.get("channel_id") or "").strip()
    if not guild or not channel:
        raise Skip("not set up yet: put the server and channel IDs in bug-inbox/config.json")
    env = cfg.get("token_env") or "WOT_DISCORD_BOT_TOKEN"
    token = os.environ.get(env, "").strip()
    if not token:
        raise Skip(f"{env} isn't set here. Set it for your Windows account, then restart the Claude app")
    api = DiscordApi(token)
    state = inbox.state["discord"]
    since = snowflake_at(inbox.config["backfill_since"])
    store = DiscordStore(inbox, guild)

    for m in api.messages(channel, state.get("last_message_id") or since):
        state["last_message_id"] = m["id"]
        if m.get("type") not in DISCORD_TEXT_TYPES:
            continue
        rid = channel_report_for(state, m)
        store.add(rid, m)
        state["last_author"] = {"id": (m.get("author") or {}).get("id"), "ts": m["timestamp"], "rid": rid}

    for tid, thread in api.threads(guild, channel, inbox.config["backfill_since"]):
        # A thread opened on a message shares that message's id, so it joins its report.
        info = state["threads"].setdefault(
            tid, {"report": state["messages"].get(tid) or f"discord-{tid}", "last_id": since})
        for m in api.messages(tid, info["last_id"]):
            info["last_id"] = m["id"]
            if m.get("type") in DISCORD_TEXT_TYPES:
                store.add(info["report"], m, title=thread.get("name"))

    changed = store.flush()
    state["last_run"] = now_iso()
    inbox.save_state()
    return changed


# ---------------------------------------------------------------- Gmail

def gmail_query(inbox):
    start = (inbox.state["gmail"].get("last_run") or inbox.config["backfill_since"])[:10]
    day = dt.date.fromisoformat(start) - dt.timedelta(days=1)
    return f"{inbox.config['gmail']['query']} after:{day:%Y/%m/%d}"


# ---------------------------------------------------------------- board

def untriaged(inbox):
    rows = []
    for path in sorted(inbox.reports.glob("*/report.md")):
        meta, body = read_front_matter(path)
        if meta.get("triaged") is not True:
            rows.append((str(meta.get("created") or "")[:10], path.parent.name, report_title(body)))
    return sorted(rows)


def mark_triaged(inbox, ids):
    for rid in ids:
        path = inbox.reports / rid / "report.md"
        if not path.exists():
            print(f"  no report called {rid}")
            continue
        text = path.read_text(encoding="utf-8").replace("\r\n", "\n")
        end = text.find("\n---\n", 3)
        if not text.startswith("---\n") or end < 0:
            print(f"  {rid}: no front matter, left alone")
            continue
        head, rest = text[:end], text[end:]
        if re.search(r"^triaged:.*$", head, flags=re.M):
            head = re.sub(r"^triaged:.*$", "triaged: true", head, count=1, flags=re.M)
        else:
            head += "\ntriaged: true"
        write_text(path, head + rest)
        print(f"  {rid}: triaged")


def as_list(value):
    if value is None:
        return []
    return value if isinstance(value, list) else [value]


def cell(value):
    return str(value if value not in (None, "") else "—").replace("|", "\\|").replace("\n", " ")


def build_board(inbox):
    bugs = []
    for path in sorted(inbox.bugs.glob("*.md")):
        meta, body = read_front_matter(path)
        match = re.match(r"BUG-\d+", path.stem)
        meta.setdefault("id", match.group(0) if match else path.stem)
        meta["title"] = meta.get("title") or report_title(body) or path.stem
        meta["_file"] = f"bugs/{path.name}"
        bugs.append(meta)

    def severity_rank(bug):
        sev = bug.get("severity")
        return SEVERITIES.index(sev) if sev in SEVERITIES else len(SEVERITIES)

    groups, placed = [], set()
    for heading, statuses in BOARD_GROUPS:
        rows = [b for b in bugs if (b.get("status") or "new") in statuses]
        placed.update(b["_file"] for b in rows)
        if heading == "Open":
            rows.sort(key=lambda b: (severity_rank(b), b["id"]))
        groups.append((heading, rows))
    leftovers = [b for b in bugs if b["_file"] not in placed]
    if leftovers:
        groups.append(("Status not recognised", leftovers))

    waiting = untriaged(inbox)
    counts = {heading: len(rows) for heading, rows in groups}
    lines = [
        "# WoT bug board",
        "",
        f"Rebuilt {dt.date.today():%Y-%m-%d} by `pull_reports.py board`. Edit the bug files, not this page.",
        "",
        f"**{counts['Open']} open** · {counts['Fixed, not released yet']} fixed, not released · "
        f"{counts['Waiting on Joe: design feedback']} design notes for Joe · "
        f"{len(waiting)} reports still to triage",
    ]
    for heading, rows in groups:
        lines += ["", f"## {heading}", ""]
        if not rows:
            lines.append("_None._")
            continue
        lines += ["| Bug | Title | Severity | Status | Area | Reports | Seen on | Fixed in |",
                  "|---|---|---|---|---|---|---|---|"]
        for b in rows:
            lines.append("| " + " | ".join([
                f"[{b['id']}]({b['_file']})", cell(b.get("title")), cell(b.get("severity")),
                cell(b.get("status") or "new"), cell(b.get("area")), str(len(as_list(b.get("reports")))),
                cell(", ".join(map(str, as_list(b.get("versions"))))), cell(b.get("fixed_in")),
            ]) + " |")
    if waiting:
        lines += ["", "## Reports still to triage", ""]
        lines += [f"- `{rid}` ({day}) {title}" for day, rid, title in waiting]
    write_text(inbox.root / "BOARD.md", "\n".join(lines) + "\n")
    return counts, len(waiting)


# ------------------------------------------------------------------ CLI

def print_changed(label, changed):
    new = sum(1 for kind, _, _ in changed if kind == "new")
    print(f"{label}: {new} new, {len(changed) - new} updated")
    for kind, rid, title in changed:
        print(f"  {kind:<8} {rid:<28} {title}")


def print_untriaged(inbox):
    rows = untriaged(inbox)
    print(f"Still to triage: {len(rows)}")
    for day, rid, title in rows:
        print(f"  {rid:<28} {day}  {title}")


def run_source(label, pull, inbox):
    try:
        print_changed(label, pull(inbox))
        return True
    except Skip as e:
        print(f"{label}: skipped. {e}")
        return True
    except PullError as e:
        print(f"{label}: FAILED. {e}")
        return False


def main():
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    parser = argparse.ArgumentParser(description="Pull WoT bug reports into the private bug inbox.")
    parser.add_argument("--inbox", default=str(WORKSPACE / "bug-inbox"), help="inbox folder (default: %(default)s)")
    sub = parser.add_subparsers(dest="cmd", required=True)
    for name in ("init", "all", "github", "discord", "untriaged", "gmail-query", "board"):
        sub.add_parser(name)
    for name in ("mark-triaged", "gmail-new"):
        sub.add_parser(name).add_argument("ids", nargs="+")
    sub.add_parser("gmail-seen").add_argument("ids", nargs="*")
    args = parser.parse_args()

    inbox = Inbox(args.inbox)
    if args.cmd == "init":
        for folder in (inbox.root, inbox.reports, inbox.bugs):
            folder.mkdir(parents=True, exist_ok=True)
        if not inbox.config_path.exists():
            write_json(inbox.config_path, DEFAULT_CONFIG)
        if not inbox.state_path.exists():
            write_json(inbox.state_path, DEFAULT_STATE)
        if not (inbox.root / "README.md").exists():
            write_text(inbox.root / "README.md", INBOX_README)
        print(f"Bug inbox ready at {inbox.root}")
        return 0

    inbox.load()
    if args.cmd == "all":
        ok = run_source("GitHub", pull_github, inbox)
        ok = run_source("Discord", pull_discord, inbox) and ok
        print(f"Gmail: search through the connector for:  {gmail_query(inbox)}")
        print("  then record every thread you looked at:  pull_reports.py gmail-seen <thread ids>")
        print_untriaged(inbox)
        return 0 if ok else 1
    if args.cmd == "github":
        return 0 if run_source("GitHub", pull_github, inbox) else 1
    if args.cmd == "discord":
        return 0 if run_source("Discord", pull_discord, inbox) else 1
    if args.cmd == "untriaged":
        print_untriaged(inbox)
    elif args.cmd == "mark-triaged":
        mark_triaged(inbox, args.ids)
    elif args.cmd == "gmail-query":
        print(gmail_query(inbox))
    elif args.cmd == "gmail-new":
        seen = set(inbox.state["gmail"]["seen"])
        fresh = [i for i in args.ids if i not in seen]
        print("\n".join(fresh) if fresh else "(all of these were handled already)")
    elif args.cmd == "gmail-seen":
        gmail = inbox.state["gmail"]
        gmail["seen"] = list(dict.fromkeys(gmail["seen"] + args.ids))
        gmail["last_run"] = dt.date.today().isoformat()
        inbox.save_state()
        print(f"Gmail: {len(args.ids)} thread(s) recorded, sweep stamped {gmail['last_run']}")
    elif args.cmd == "board":
        counts, waiting = build_board(inbox)
        print(f"BOARD.md rebuilt: {counts['Open']} open, {waiting} reports still to triage")
    return 0


if __name__ == "__main__":
    sys.exit(main())
