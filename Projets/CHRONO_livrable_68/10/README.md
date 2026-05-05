# Tâche 10 — Objectif : serveur + client **IPv6**, protocole Paroles

**CHRONO N°10** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## But

Implémenter un **serveur** et un **client** communiquant en **TCP sur IPv6**, selon le protocole **Paroles** décrit dans le sujet (voir `checklist_projet_PR6.md`).

## Fichiers extraits (projet complet `PRCursor`)

| Fichier ici | Source dans PRCursor | Contenu |
|-------------|------------------------|---------|
| `extrait_paroles_proto.h` | `include/paroles_proto.h` (l. 1–44) | Constantes de format : longueurs, CODEREQ principaux, notifications — **cœur du vocabulaire protocole**. |
| `extrait_net_tcp6.c` | `src/net.c` (l. 1–53) | `socket(AF_INET6, …)`, `inet_pton(AF_INET6, …)` pour **écoute** et **connexion** TCP en IPv6. |

## Comment utiliser les extraits

- `extrait_paroles_proto.h` : à rapprocher du fichier **`include/paroles_proto.h`** du dépôt fac (en général **remplacer** ou compléter le header du même nom).
- `extrait_net_tcp6.c` : extraire les **fonctions** `tcp6_listen`, `tcp6_accept`, `tcp6_connect` dans votre **`src/net.c`** (ou équivalent), en gardant cohérence avec `net.h`.

Ce ne sont **pas** des fichiers compilables seuls : ils illustrent l’**objectif IPv6 + squelette protocole**. Le reste du serveur et du client vit dans `server.c`, `client.c`, etc. (tâches **N°11** et suivantes).

## Vérification « c’est fait » (niveau objectif)

- Le serveur **écoute** sur une adresse IPv6 (`::`, `::1`, etc.).
- Le client **se connecte** en IPv6 au serveur.
- Les échanges ultérieurs respectent les CODEREQ du sujet (détaillés dans les tâches suivantes).
