# -*- coding: utf-8 -*-
"""Infrastructure partagée : cadres 110 cols, sous-tableaux, section ## Blocs détaillés."""
import textwrap
from pathlib import Path

HERE = Path(__file__).resolve().parent

W = 110
inner = W - 2
TAG_SIMPLE = " Explication simple :"


def border() -> str:
    return "|" + "-" * inner + "|"


def line_txt(s: str) -> str:
    s = s.replace("\n", " ").strip()
    if not s.startswith(" "):
        s = " " + s
    if len(s) > inner:
        s = s[:inner]
    return "|" + s.ljust(inner) + "|"


def wrap_stanza(tag: str, text: str) -> list[str]:
    text = text.strip()
    if not tag.startswith(" "):
        tag = " " + tag.strip()
    indent_len = min(len(tag) + 1, 32)
    indent = " " * indent_len
    tw = textwrap.TextWrapper(
        width=inner,
        initial_indent=tag + " ",
        subsequent_indent=indent,
        break_long_words=False,
        break_on_hyphens=True,
    )
    out = []
    for ln in tw.fill(text).split("\n"):
        if len(ln) > inner:
            ln = ln[:inner]
        if not ln.startswith(" "):
            ln = " " + ln.lstrip()
        if len(ln) > inner:
            ln = ln[:inner]
        out.append("|" + ln.ljust(inner) + "|")
    return out


W1, W2, W3 = 22, 40, 43


def row_triple(a: str, b: str, c: str) -> str:
    body = " " + a[:W1].ljust(W1) + "│" + b[:W2].ljust(W2) + "│" + c[:W3].ljust(W3)
    assert len(body) == inner, len(body)
    return "|" + body + "|"


def wrap_cell(s: str, w: int) -> list[str]:
    if not s.strip():
        return [""]
    return textwrap.wrap(s.strip(), width=w, break_long_words=False, break_on_hyphens=True)


def render_subgrid(subs: list[tuple[str, str, str]]) -> list[str]:
    lines: list[str] = []
    lines.append(line_txt(" — Sous-tableau : Fonction │ Ce qu'elle fait │ Comment — "))
    lines.append(row_triple("Fonction", "Ce qu'elle fait", "Comment"))
    lines.append(row_triple("─" * W1, "─" * W2, "─" * W3))
    for nom, fait, comme in subs:
        cl1 = wrap_cell(nom, W1)
        cl2 = wrap_cell(fait, W2)
        cl3 = wrap_cell(comme, W3)
        nmax = max(len(cl1), len(cl2), len(cl3))
        cl1.extend([""] * (nmax - len(cl1)))
        cl2.extend([""] * (nmax - len(cl2)))
        cl3.extend([""] * (nmax - len(cl3)))
        for i in range(nmax):
            lines.append(row_triple(cl1[i], cl2[i], cl3[i]))
    return lines


def build_block(meta, simple, cmd, effet, fonct, subs=None) -> str:
    lr, n, bname, role = meta
    lines = []
    lines.append(border())
    lines.append(line_txt(f"Lignes : {lr} ({n})"))
    lines.append(line_txt(f"Bloc   : {bname}"))
    lines.append(line_txt(f"Rôle   : {role}"))
    lines.append(border())
    lines.extend(wrap_stanza(TAG_SIMPLE, simple))
    if subs:
        lines.extend(render_subgrid(subs))
    lines.extend(wrap_stanza(" Cmd :", cmd))
    lines.extend(wrap_stanza(" Effet :", effet))
    lines.extend(wrap_stanza(" Fonct. :", fonct))
    lines.append(border())
    return "\n".join(lines)


def rebuild_section_intro() -> str:
    return (
        "Chaque cadre : **Lignes** / **Bloc** / **Rôle**, puis **Explication simple** (**chronologie TP** : **quand**, **depuis quel terminal**, **après quelle action cliente** — peu de jargon si possible) ; si plusieurs fonctions/étapes, un **sous-tableau à 3 colonnes** (**Fonction** | **Ce qu'elle fait** | **Comment**) ; puis **Cmd**, **Effet**, **Fonct.** — lignes × 110 caractères. Entre blocs : tirets.\n\n"
    )


def rebuild_section(parts) -> str:
    section = "## Blocs détaillés\n\n"
    section += rebuild_section_intro()
    for i, p in enumerate(parts):
        if len(p) == 5:
            meta, simple, cmd, effet, fonct = p
            subs = None
        else:
            meta, simple, cmd, effet, fonct, subs = p
        section += "```\n" + build_block(meta, simple, cmd, effet, fonct, subs) + "\n```\n\n"
        if i < len(parts) - 1:
            section += "---------------------------------------------------------------------------------\n\n"
    return section


def assert_pipe_width(txt: str) -> None:
    bad = [(i + 1, len(l)) for i, l in enumerate(txt.splitlines()) if l.startswith("|") and len(l) != W]
    assert not bad, bad[:10]


def full_doc(prefix: str, parts, suffix: str = "") -> str:
    doc = prefix + rebuild_section(parts) + suffix
    assert_pipe_width(doc)
    return doc
