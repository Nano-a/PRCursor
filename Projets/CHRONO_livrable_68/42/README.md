# Tâche 42 — Qualité : lisibilité, commentaires, **mode verbeux**, mutualisation, pas de fuites

**CHRONO N°42** | Branche : **`feature/etape1-proto-tcp`** | Qui : **Tous**

## Mode verbeux

- **Serveur** : `-v` en **premier** argument → traces **`vlog`** sur stderr (inscription, groupes, jointures, erreurs auth, etc.).
- **Client** : `-v` en premier argument → taille et **code** de chaque réponse TCP lue.

## Mutualisation

Exemples dans le projet : **`wire.c`** (sérialisation), **`net.c` / `tls_io.c`** (E/S), **`notif_mcast` / `notif_udp_user`** (paquets 6 octets).

## Fuites mémoire / FD

À chaque **`malloc`**/`realloc` raté : **`return -1`** ; après **`socket`** : **`close`** en chemin d’erreur ; en fin de handler : libérer buffers alloués (`handle_list_inv` → `free(out)`).

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_server_vlog.c` | `src/server.c` — `verbose`, `vlog`, usage dans handlers. |
| `extrait_client_verbose.c` | `src/client.c` — `verbose`, traces `reponse … octets`. |

## Vérification

Soutenance : démonstration avec **`./paroles_server -v …`** et **`./paroles_client -v …`** ; outils **valgrind**/sanitizers si demandés par le sujet.
