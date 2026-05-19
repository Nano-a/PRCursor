# Tâche 26 — Formats **16–17** : **FEED** depuis **(NUMB, NUMR)** et fil global

**CHRONO N°26** | Branche : **`feature/etape1-proto-tcp`**

## Base

- **Départ** : snapshot **N°25**.

## Livrables

| Fichier | Cible usuelle (`programmation_reseaux`) |
|---------|----------------------------------------|
| `serveur_complet_etape_programmation_reseaux_chrono_N26.c` | `src/server.c` |
| `client_complet_etape_programmation_reseaux_chrono_N26.c` | `src/client.c` |

## Contenu fonctionnel

| Élément | Détail |
|---------|--------|
| **`FeedItem`** | Une entrée du fil : **author**, **numb**, **numr**, **len**, **`data`** (copie dynamique). |
| **`feed_push`** | Appelé depuis **`handle_post`** (**numr = 0** pour le corps du billet) et **`handle_reply`** (texte + **numr** attribué). |
| **`feed_index_after`** | Première occurrence de **(`numb`, `numr`)** dans le fil. |
| **Convention `(0, 0)`** | Point de départ **dès le premier** événement : renvoyer la tranche **`i = 0 .. nfeed-1`**. Sinon **`from = index_trouvé + 1`**. |
| **`handle_feed`** | **MEMBRE** requis → **FEED_OK** (**IDG**, **NB**, puis lignes comme l’extrait). Repositionnement **`msp`** après **realloc** pour patcher **NB**. **NOTIF_FETCH** vers auteur billet comme l’extrait. |
| **Client** | **`feed <uid> <idg> <numb> <numr>`** (`argc == 8`) ; corps **FEED** = **UID + IDG + NUMB + NUMR** (**16 octets** après code). Lecture réponse jusqu’à fin TCP (**`recv` en boucle**). |

## Test rapide

```text
# Après inscription + groupe + post(s)/reply(s), historique depuis le début :
./paroles_client ::1 P feed 1 1 0 0
```

## Commit exemple

```
CHRONO N°26 : FEED (16) / FEED_OK (17), fil chronologique + anchor (0,0)
```
