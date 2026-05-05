# Tâche 61 — Compteur **NUM** : **0** puis **+1** à chaque connexion authentifiée réussie

**CHRONO N°61** | Branche : **`feature/etape3-ed25519-auth`** | Qui : **M1**

## Objectif

- **Serveur** : pour chaque utilisateur, `auth_nonce` vaut **0** après l’inscription (`memset(u, 0, sizeof *u)` dans `handle_reg`). À chaque **auth réussie** (`do_client_auth`), le client doit envoyer **NUM == auth_nonce** ; le serveur vérifie puis fait **`auth_nonce++`** avant de traiter la suite.
- **Client** : sans fichier persistant, `read_nonce` renvoie **0** (première auth). Après une auth OK, **`write_nonce(port, uid, num + 1)`** enregistre le prochain NUM attendu (fichier `/tmp/paroles_nonce_<port>_<uid>`), pour que la **prochaine** connexion utilise **1**, puis **2**, etc.

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `src/server.c` | `handle_reg` : init user à zéro ; `do_client_auth` : comparaison `num` / `u->auth_nonce`, puis `u->auth_nonce++`. |
| `src/client.c` | `read_nonce` / `write_nonce` / `nonce_path`. |

## Fichiers extraits

| Fichier | Rôle |
|---------|------|
| `extrait_server_auth_nonce.c` | Vérification + incrément côté serveur. |
| `extrait_client_nonce_persist.c` | Persistance côté client + appel après AUTH_OK. |

## Vérification

- Deux `newgroup` successifs avec la même identité sur le **même** client : le second doit utiliser **NUM=1** (fichier nonce mis à jour). En pratique : `tests/auth_full_smoke.sh` + test manuel ou script qui enchaîne deux commandes authentifiées.
