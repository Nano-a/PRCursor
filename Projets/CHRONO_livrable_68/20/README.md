# Tâche 20 — Formats **6–7** : liste **invitations en attente** + réponse (IDG, LEN, nom, admin 10 octets)

**CHRONO N°20** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Client → serveur (6)

- **LIST_INV** + **ID** utilisateur (4 octets BE) — corps après le code lu par `serve_business_switch` selon votre découpage (dans `PRCursor`, corps = 4 octets pour l’UID).

## Serveur → client (7)

- **LIST_INV_OK**, **NB** (4 BE), puis pour chaque invitation : **IDG**, **LEN** nom, **nom**, **NOM admin** (10 octets).

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_client_codereq_6_7.c` | `src/client.c` — `cmd_listinv`. |
| `extrait_server_codereq_6_7.c` | `src/server.c` — `handle_list_inv`. |

## Vérification

- Utilisateur invité : `listinv` affiche une ligne par groupe en attente avec idg, nom, admin.
