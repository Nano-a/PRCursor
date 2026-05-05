# Tâche 35 — Réponse erreur **CODEREQ 31** + **ZEROS**

**CHRONO N°35** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Format

- **1 octet** : `PAROLES_CODEREQ_ERR` (= **31**).
- **15 octets** : **ZEROS** (`PAROLES_ERR_TAIL` dans `paroles_proto.h`).

Total **16 octets** pour la forme « standard » utilisée par `send_ack` / `send_err` (même taille de buffer : 1 + ERR_TAIL).

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_paroles_proto_err.h` | `include/paroles_proto.h` — `PAROLES_CODEREQ_ERR`, `PAROLES_ERR_TAIL`. |
| `extrait_server_send_err.c` | `src/server.c` — `send_err`. |

## Côté client

Lire le premier octet de la réponse : si **31**, traiter comme erreur (le client CLI retourne souvent code 1).

## Vérification

Déclencher un rejet (N°34) et capturer les octets reçus ou observer échec CLI.
