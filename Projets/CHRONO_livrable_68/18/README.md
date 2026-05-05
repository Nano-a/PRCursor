# Tâche 18 — Création groupe : **multidiffusion unique** ; créateur = **admin**

**CHRONO N°18** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Implémentation de référence

- Adresse multicast dérivée de **IDG** : `ff0e::1:<idg>` (chaîne puis `inet_pton`).
- Port multicast : `30000 + (idg % 30000)` (convention projet).
- **`g->admin_id = uid`** : l’utilisateur qui envoie **NEW_GROUP** est admin ; il est ajouté comme membre via `group_add_member`.

## Fichier extrait (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_server_mcast_admin.c` | `src/server.c` — `handle_new_group` : admin, construction IP/port mcast. |

## Vérification

- Deux groupes → deux couples (IP mcast, port) distincts en pratique ; seul l’admin peut **inviter** (vérifié ailleurs dans `handle_invite`).
