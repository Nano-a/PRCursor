# Tâche 41 — Qualité : **timeouts** (pas de blocage infini si client idle)

**CHRONO N°41** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1**

## Principe

Toute attente sur une lecture TCP (clair ou TLS) doit avoir une **borne de temps** : si le client ne envoie rien ou se déconnecte, le serveur **sort** de la lecture et **ferme** la connexion au lieu de bloquer indéfiniment.

## Dans `PRCursor`

- **`PAROLES_TCP_TIMEOUT_MS`** dans `net.h` (ex. 30 s).
- **`readn`** / **`conn_readn`** : `poll` avec ce délai ; SSL **`WANT_READ`** / **`WANT_WRITE`** repoll avec le même timeout.
- **`serve_client`** : chaque **`conn_readn(..., PAROLES_TCP_TIMEOUT_MS)`** ; échec → `goto end` → fermeture.

## Fichiers extraits

| Fichier | Source |
|---------|--------|
| `extrait_serve_client_lecture_timeout.c` | `src/server.c` — première lecture de requête avec timeout. |

## Voir aussi

Tâche **40** (`readn`, `conn_readn`).

## Vérification

Client connecté sans envoyer de paquet : au bout du délai, le serveur abandonne la lecture (pas de blocage infini).
