# Tâche 40 — Qualité : erreurs syscall, messages mal formés, **déconnexion** du pair

**CHRONO N°40** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Côté serveur / client

- Les lectures TCP utilisent **`conn_readn`** / **`readn`** avec **timeout** (`PAROLES_TCP_TIMEOUT_MS`, ex. 30 s dans `net.h`) : pas de blocage infini si le pair ne répond pas.
- **`poll`** : timeout ou **`POLLHUP` / `POLLERR`** → échec de lecture → fermeture propre de la session (`serve_client` : `goto end`, `close`).
- Messages **mal formés** : garde sur **`blen`**, **`wire_get_*`**, **`return -1`** puis **31** (N°34–35).

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_net_timeout.h` | `src/net.h` — `PAROLES_TCP_TIMEOUT_MS`. |
| `extrait_net_readn.c` | `src/net.c` — `readn` (poll, `POLLHUP`, `read <= 0`). |
| `extrait_tls_io_conn_readn.c` | `src/tls_io.c` — `conn_readn` (SSL + timeout / erreur SSL). |

## Vérification

- Client fermé au milieu d’une requête : le serveur ne reste pas bloqué indéfiniment.
- `tests/notif_recv.py tcp_bad` : envoi d’un octet invalide → réponse **31** puis fin de connexion.
