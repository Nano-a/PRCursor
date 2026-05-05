# Tâche 21 — Formats **8**, **9**, **24** : réponse invitation (**AN** 0 / 1 / 2) + **JOIN_OK** ou **ACK**

**CHRONO N°21** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Client → serveur (8)

- **INV_ANS** : **ID** utilisateur, **IDG**, **AN** (1 octet) — accepter (1), refuser (0), ou **quitter** (2) selon contexte.

## Serveur → client

- **ACK (24)** : refus d’invitation, ou quitter (non-admin), ou fermeture groupe par admin (voir N°22).
- **JOIN_OK (9)** : acceptation — **IDG**, port/IP multicast, **NB** membres, puis (**ID**, **NOM** 10) × NB (admin en tête dans `PRCursor` car ajouté en premier dans `g->mem`).

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_client_codereq_8.c` | `src/client.c` — `cmd_ans`. |
| `extrait_server_handle_inv_ans.c` | `src/server.c` — `handle_inv_ans` (branches 0, 1, 2). |

## Vérification

- `ans <uid> <idg> 0` → `OK ack` ; `ans … 1` (invité) → `OK join` ; `ans … 2` (membre quitte) → ack ou fermeture si admin.
