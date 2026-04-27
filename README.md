# PR6 — Paroles

Projet réseau **IPv6** (protocole Paroles) — client et serveur en C : TCP, **TLS** (OpenSSL), authentification **ED25519** (CODEREQ 0 / AUTH_OK 25) sur la même session que le métier après l’inscription.

## Compilation

```bash
make
```

Binaires : `paroles_server`, `paroles_client`.

## Exemple (plain TCP, sans TLS)

Terminal 1 :

```bash
./paroles_server :: 4242
```

Terminal 2 :

```bash
./paroles_client ::1 4242 reg Alice
./paroles_client ::1 4242 newgroup 1 MonGroupe
```

## TLS + auth (étapes 2–3)

Serveur (certificat + clé TLS seulement ; la CA sert au **client**) :

```bash
./paroles_server --tls server.pem server.key :: 4242 --signing-key srv_priv.pem
```

Client après inscription : même connexion TLS, message AUTH (0) puis commande métier ; clés utilisateur PEM :

```bash
./paroles_client --tls ca.pem ::1 4242 --key user_priv.pem --server-pub srv_pub.pem newgroup 1 MonGroupe
```

Voir `paroles_client` / `paroles_server` sans arguments pour les options (`--verbose`, ports UDP/multicast, etc.).

## Documentation (CHRONO 66–68)

- [`docs/GIT_ARBRE.md`](docs/GIT_ARBRE.md) — **graphe d’historique** identique à `git log --graph --all` (la page *Network* GitHub ne le reproduit pas à l’identique)  
- [`docs/VERIFICATION_TESTS.md`](docs/VERIFICATION_TESTS.md) — **comment lancer et interpréter les tests** (terminaux, commandes, TLS/auth)  
- [`docs/SOUTENANCE.md`](docs/SOUTENANCE.md) — démo et relecture finale  
- [`docs/GIT_EQUIPE.md`](docs/GIT_EQUIPE.md) — historique visible pour chaque membre  
- [`docs/HOTFIX.md`](docs/HOTFIX.md) — procédure `hotfix/*` depuis `main`

## Test rapide

```bash
make test
```

Smoke seul : `chmod +x tests/smoke.sh && ./tests/smoke.sh`

## Auteurs

[`authors.md`](authors.md) — à compléter par le trinôme.

## CREDITS (sources externes)

| Source | Nature | Date |
|--------|--------|------|
| *Aucune pour l’instant* | — | — |

Toute aide (web, IA, code tiers) doit être **ajoutée ici** avec URL ou référence précise (obligation du sujet).

## Branches Git

[`docs/BRANCHES.md`](docs/BRANCHES.md)
