# Tâche 25 — Formats **14–15** : **réponse** à billet + **NUMR** ; réponses **à partir de 1**

**CHRONO N°25** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Client → serveur (14)

- **REPLY** : **ID**, **IDG**, **NUMB** (billet cible), **LEN**, **texte**.

## Serveur → client (15)

- **REPLY_OK** : **IDG**, **NUMB**, **NUMR** (numéro de la réponse pour ce billet).

## Numérotation des réponses

Pour chaque billet, **`po->next_reply`** est incrémenté ; la première réponse a **NUMR = 1** (initialisation implicite du compteur côté post à 0 puis premier reply donne 1 — voir code : `po->next_reply++` puis `numr = po->next_reply`).

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_client_codereq_14_15.c` | `src/client.c` — `cmd_reply`. |
| `extrait_server_codereq_14_15.c` | `src/server.c` — `handle_reply`. |

## Vérification

- Réponse au billet 0 → `REPLY_OK` avec **numr** 1, puis 2, …
