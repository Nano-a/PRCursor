# Tâche 54 — Adapter client / serveur + certificats / script génération ; tests **UFR**

**CHRONO N°54** | Branche : **`feature/etape2-tls`** | Qui : **M3**, **M1**, **M2**

## Objectif

- **CLI** : `paroles_server [--tls cert.pem key.pem] …` et `paroles_client [--tls ca.pem] host port …`.
- **Handshake** : `SSL_accept` côté serveur après `accept(2)` ; `SSL_connect` côté client avec SNI adapté (`::1` / `127.0.0.1` → `localhost`).
- **Certificats** : script OpenSSL pour une **CA de test** + certificat serveur signé (répertoire type `tests/fixtures/certs/`).
- **Non-régression** : `tests/tls_smoke.sh` (inscription TLS).

Sur les **machines UFR** : vérifier présence d’**OpenSSL** en ligne de commande et bibliothèques **libssl** pour l’édition de liens (`-lssl -lcrypto`).

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `scripts/gencerts.sh` | Génère `ca.pem`, `server.pem`, `server.key`. |
| `tests/tls_smoke.sh` | Smoke TLS (N°53–54). |
| `src/server.c` | `main` : `--tls` + `serve_client` : `SSL_accept` (~816–930). |
| `src/client.c` | `--tls` + `SSL_connect` dans `one_cmd_plain` (~26–82, 484–491). |

## Consigne de collage

- Ne pas écraser tout `server.c` / `client.c` sans relecture : **fusionner** les blocs TLS avec votre dispatch métier existant.
- Copier `gencerts.sh` dans `scripts/` du dépôt fac ; lancer depuis la racine du projet.

## Fichiers extraits

| Fichier | Rôle |
|---------|------|
| `extrait_gencerts.sh` | = `PRCursor/scripts/gencerts.sh`. |
| `extrait_tls_smoke.sh` | = `tests/tls_smoke.sh` avec `ROOT` → `../../../PRCursor`. |
| `extrait_server_tls_handshake.c` | `serve_client` + partie `main` (`--tls`). |
| `extrait_client_tls_handshake.c` | SNI + bloc TLS dans `one_cmd_plain` + parse `--tls` dans `main`. |

## Vérification

- `./extrait_tls_smoke.sh` depuis ce dossier **ou** `PORT=4248 tests/tls_smoke.sh` dans `PRCursor` après `make`.
