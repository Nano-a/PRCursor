<!-- CHRONO N°4 — bloc d’intro (extrait_README.md, lignes 1–26) retiré — voir Projets/CHRONO_livrable_68/4/ -->

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

## Branches Git

[`docs/BRANCHES.md`](docs/BRANCHES.md)
