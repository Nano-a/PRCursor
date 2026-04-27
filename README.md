# PR6 — Paroles

Projet réseau **IPv6** (protocole Paroles) — client et serveur en C, étape 1 sans TLS.

## Compilation

```bash
make
```

Binaires : `paroles_server`, `paroles_client`.

## Exemple

Terminal 1 :

```bash
./paroles_server :: 4242
```

Terminal 2 :

```bash
./paroles_client ::1 4242 reg Alice
./paroles_client ::1 4242 newgroup 1 MonGroupe
```

Voir `paroles_client` sans arguments pour la liste des commandes (`invite`, `post`, `feed`, etc.).

## Test rapide

```bash
chmod +x tests/smoke.sh
./tests/smoke.sh
```

## Auteurs

[`authors.md`](authors.md) — à compléter par le trinôme.

## CREDITS (sources externes)

| Source | Nature | Date |
|--------|--------|------|
| *Aucune pour l’instant* | — | — |

Toute aide (web, IA, code tiers) doit être **ajoutée ici** avec URL ou référence précise (obligation du sujet).

## Branches Git

[`docs/BRANCHES.md`](docs/BRANCHES.md)
