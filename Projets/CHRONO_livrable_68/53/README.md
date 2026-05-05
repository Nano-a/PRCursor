# Tâche 53 — **Étape 2** : TCP sécurisé avec **TLS** (OpenSSL)

**CHRONO N°53** | Branche : **`feature/etape2-tls`** | Qui : **M3**, **M1**

## Objectif

Chiffrer le canal **TCP** avec **TLS 1.2+** via OpenSSL : contexte serveur (cert + clé), contexte client (CA pour vérifier le serveur), et **lecture/écriture** unifiées (`conn_readn` / `conn_writen` / `conn_read_upto`) avec `SSL *` ou TCP clair si `ssl == NULL`.

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `include/tls_io.h` | API publique TLS + I/O. |
| `src/tls_io.c` | Implémentation (lignes 1–120 environ). |
| `src/server.c` | `SSL_accept` après accept TCP (`serve_client`, ~816–830). |
| `src/client.c` | `SSL_connect` dans `one_cmd_plain` / `one_cmd_authed`. |
| `Makefile` | `-lssl -lcrypto`, compilation de `tls_io.o`. |

## Consigne de collage

- **`extrait_tls_io.c`** : fusionner dans `src/tls_io.c` (fichier entier aligné sur le dépôt de référence), ou comparer ligne à ligne.
- Côté **`server.c` / `client.c`**, voir dossier **54** pour les extraits d’intégration CLI et handshake.

## Fichiers extraits

| Fichier | Rôle |
|---------|------|
| `extrait_tls_io.h` | = `PRCursor/include/tls_io.h`. |
| `extrait_tls_io.c` | = `PRCursor/src/tls_io.c`. |

## Vérification

- `make` ; démarrage serveur avec `--tls cert key` et client avec `--tls ca.pem` (smoke N°54).
