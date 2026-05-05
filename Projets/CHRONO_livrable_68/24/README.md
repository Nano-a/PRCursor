# Tâche 24 — Formats **12–13** : **post** + réponse **NUMB** ; billets **à partir de 0** par groupe

**CHRONO N°24** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Client → serveur (12)

- **POST** : **ID**, **IDG**, **LEN**, **données** texte.

## Serveur → client (13)

- **POST_OK** : **IDG**, **NUMB** (numéro du billet).

## Numérotation

Nouveau billet : **`po->numb = g->next_billet++`** ; compteur par groupe, premier billet **0**.

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_client_codereq_12_13.c` | `src/client.c` — `cmd_post`. |
| `extrait_server_codereq_12_13.c` | `src/server.c` — `handle_post` (création billet + réponse). |

## Vérification

- Premier `post` sur un groupe → `OK billet numb=0`, puis 1, 2, …
