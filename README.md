# PRCursor — Projet PR6 « Paroles »

Implémentation **IPv6** du protocole **Paroles** (serveur + client en C).

## État (étape 1)

- TCP en clair (pas de TLS), pas d’auth par signature ; **CLE** = 113 octets nuls à l’inscription.
- Notifications **UDP** et **multidiffusion** IPv6.

Étapes **2** (TLS) et **3** (ED25519, CODEREQ 0) : branches `feature/etape2-tls` et `feature/etape3-ed25519-auth` (à venir).

## Compilation

```bash
make
```

Binaires : `paroles_server`, `paroles_client`, `paroles_smoke`, `test_feed_order`.

## Lancement

Serveur : `./paroles_server [::] 4242`  
Client : `./paroles_client [-v] ::1 4242 [if_multicast]` (ex. `lo`, `eth0`)

## Tests

```bash
make test
```

## Crédits (sources externes)

À compléter par l’équipe (URL, date, nature de l’aide) — obligation du sujet.

## Branches

Voir [`docs/BRANCHES.md`](docs/BRANCHES.md).

## Schéma Git

[`docs/git-workflow.svg`](docs/git-workflow.svg)
