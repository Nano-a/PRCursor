# -*- coding: utf-8 -*-
"""Usage: python3 _gen_src_md.py  — regénère wire.md, net.md, auth_ed25519.md, tls_io.md, client.md"""
from __future__ import annotations

from _md_blocks_common import HERE, assert_pipe_width, rebuild_section
from md_parts_client import PARTS_CLIENT
from md_parts_wire_net_auth_tls import PARTS_AUTH, PARTS_NET, PARTS_TLS, PARTS_WIRE

INTRO_CADRE = (
    "Chaque **bloc** est un **même cadre ASCII** : d’abord **Lignes** (avec le nombre de lignes entre parenthèses), **Bloc**, **Rôle** ; "
    "puis **Explication simple** (**récit** : actions terminal / TP, **pourquoi** ce morceau **à ce moment**) ; "
    "si besoin un **sous-tableau à 3 colonnes** ; puis **Cmd**, **Effet**, **Fonct.** — comme `server.md`. "
    "**Entre deux blocs** : une ligne `---------------------------------------------------------------------------------`.\n\n"
)

DOCS: list[tuple[str, str, int, str, str, list]] = [
    (
        "wire.md",
        "`wire.c`",
        55,
        "Encodage/décodage **big-endian** des entiers et des zones de **zéros** utilisés dans tous les paquets Paroles (**serveur et client**).",
        "```mermaid\nflowchart LR\n  E[msg à envoyer] --> P[wire_put_*]\n  R[octets lus TCP] --> G[wire_get_*]\n```\n\n",
        PARTS_WIRE,
    ),
    (
        "net.md",
        "`net.c`",
        136,
        "**Transport IPv6** : sockets **TCP** (écouter, accepter, connecter), **écritures/ lectures bornées** (**`writen`/`readn`**) avec **timeouts `poll`**, et **UDP** + helpers **multicast** pour les notifications cours.",
        "```mermaid\nflowchart LR\n  S[tcp6_listen] --> A[accept serveur]\n  C[tcp6_connect] --> T[Flux TCP cliente]\n  U[UDP send/recv] --> N[Notifs cours]\n```\n\n",
        PARTS_NET,
    ),
    (
        "auth_ed25519.md",
        "`auth_ed25519.c`",
        61,
        "Charge **PEM Ed25519** et fournit **`sign` / `verify`** via **EVP OpenSSL** (**messages bruts**, alignement cours **PR6**). Branché derrière **`--signing-key`**, **`--key`**, **`--server-pub`**.",
        "```mermaid\nflowchart LR\n  PEM[charges PEM] --> K[EVP_PKEY]\n  M[octets AUTH] --> S[DigestSign]\n  M2[AUTH_OK] --> V[DigestVerify]\n```\n\n",
        PARTS_AUTH,
    ),
    (
        "tls_io.md",
        "`tls_io.c`",
        120,
        "Crée **`SSL_CTX` serveur ou client**, puis **`conn_readn` / `conn_read_upto` / `conn_writen`** qui **marchent aussi sans TLS** (délégation **`readn`/`writen`** depuis **`net.c`**).",
        "```mermaid\nflowchart LR\n  SSL[Connexion TLS] --> R[SSL_read/write]\n  CLR[TCP sans TLS] --> RN[readn/writen]\n```\n\n",
        PARTS_TLS,
    ),
    (
        "client.md",
        "`client.c`",
        648,
        "**Outil de TP** `./paroles_client` : construction des **CODEREQ** avec **`wire`**, transport **`net`/`tls_io`**, options **signature** facultative (**`auth_ed25519`**), et commandes **`listen_udp` / `listen_mcast`** pour voir les notifs UDP.",
        "```mermaid\nflowchart LR\n  argv[main parse] --> Cmd[cmd_*]\n  Cmd --> Oc[one_cmd]\n  Oc --> Tcp[tcp6 + TLS?]\n  Tcp --> CW[conn_read/write]\n```\n\n",
        PARTS_CLIENT,
    ),
]


def md_prefix(title_name: str, src_name: str, nlines: int, role: str, mermaid_block: str) -> str:
    return (
        f"# {title_name} — carte de lecture\n\n"
        f"Fichier source : **`src/{src_name}`** (**{nlines}** lignes).  \n"
        f"{INTRO_CADRE}"
        f"**Rôle dans le projet :** {role}\n\n"
        f"{mermaid_block}"
        f"---\n\n"
    )


def suffix_common() -> str:
    return (
        "\n---\n\n## Régénérer ces cadres\n\n"
        "```bash\n"
        'cd "$(git rev-parse --show-toplevel 2>/dev/null)/PRCursor/src md"\n'
        "python3 _gen_src_md.py\n"
        "```\n\n"
        "Voir aussi **`server.md`** (même style, script **`_gen_server_md_blocks.py`**).\n"
    )


def main() -> None:
    for fname, quoted, nlines, role, mer, parts in DOCS:
        src = fname.replace(".md", ".c")
        body = md_prefix(quoted.strip("`"), src, nlines, role, mer)
        doc = body + rebuild_section(parts) + suffix_common()
        assert_pipe_width(doc)
        (HERE / fname).write_text(doc, encoding="utf-8")
        print(f"Écrit {HERE / fname} ({len(parts)} blocs)")


if __name__ == "__main__":
    main()
