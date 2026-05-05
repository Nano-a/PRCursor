# Tâche 59 — Session TCP : **sauf inscription**, échange **auth** puis réponse serveur (signatures)

**CHRONO N°59** | Branche : **`feature/etape3-ed25519-auth`** | Qui : **M1**, **M3**

## Objectif

Lorsque le serveur est lancé avec **`--signing-key priv_ed25519.pem`** (`g_srv_sign_key` non nul) :

1. La **première** requête d’une connexion peut rester **`CODEREQ_REG` (1)** : pas d’auth (inscription en clair sur le canal, éventuellement déjà sous TLS).
2. Toute autre session : le **premier octet** doit être **`CODEREQ_AUTH` (0)** ; le serveur vérifie la signature client (voir N°60), répond **`AUTH_OK` (25)** signé, puis lit le **vrai** premier octet métier.
3. **`dispatch`** : si `sess_uid` est fixé après auth, le corps des requêtes métier doit commencer par le **même UID** filaire (anti usurpation).

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `src/server.c` | `serve_client` : branche REG, puis `do_client_auth` / `send_auth_ok`, puis `serve_business_switch`. |
| `src/server.c` | `dispatch` : garde `wire_uid == sess_uid` si `sess_uid != 0`. |
| `src/client.c` | `one_cmd` : sans `--key`, `one_cmd_plain` ; avec `--key`, `one_cmd_authed` pour les commandes métier. |

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_server_session_auth.c` | Flux `serve_client` + en-tête `dispatch` (garde UID). |

## Vérification

- Sans `--signing-key` : comportement étape 1/2 sur TCP/TLS.
- Avec `--signing-key` + client `--key` / `--server-pub` : `tests/auth_full_smoke.sh` (dossier suivant / Makefile `test`).
