# Tâche 26 — Formats **16–17** : fil depuis **(n, r)** + ordre de **réception globale**

**CHRONO N°26** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Principe

Le serveur maintient un fil **`g->feed`** : enchaînement des **billets** et **réponses** dans l’ordre où ils arrivent. La requête **FEED (16)** précise **IDG**, **NUMB**, **NUMR** : on renvoie tout ce qui suit **après** cette position dans le fil (`feed_index_after` puis boucle `i = from .. nfeed-1`).

La réponse **FEED_OK (17)** contient **NB** puis pour chaque entrée : auteur, billet, numéro réponse, longueur, texte.

## Convention `(0, 0)`

Point de départ « début du fil » pour récupérer l’historique depuis le début (voir `feed_order_pdf` / sujet).

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_client_codereq_16_17.c` | `src/client.c` — `cmd_feed`. |
| `extrait_server_codereq_16_17.c` | `src/server.c` — `feed_index_after` + `handle_feed`. |

## Vérification

- `tests/feed_order_pdf.sh` et scénario PDF (ordre b2, b3, réponse b1, etc.).
