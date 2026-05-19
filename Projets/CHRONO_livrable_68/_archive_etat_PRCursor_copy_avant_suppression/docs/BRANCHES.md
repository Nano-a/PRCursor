# Branches

| Branche | Rôle |
|---------|------|
| `main` | Version stable / rendu (tag `v1.0-sujet-complet` quand le sujet complet est mergé) |
| `develop` | Intégration |
| `feature/etape1-proto-tcp` | Protocole TCP IPv6 sans TLS (étape 1) |
| `feature/etape1-tests-notifs` | Tests CODEREQ / notifs |
| `feature/etape2-tls` | TLS OpenSSL |
| `feature/etape3-ed25519-auth` | CLE 113 PEM + AUTH 0 + session TLS+métier |
| `hotfix/<sujet>` | Correctifs depuis `main`, à remerger dans `develop` (voir `docs/HOTFIX.md`) |

Les merges `--no-ff` depuis les features vers `develop` permettent de voir l’historique sur GitLab.
