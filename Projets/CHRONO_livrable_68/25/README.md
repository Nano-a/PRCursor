# Tâche 25 — Formats **14–15** : **réponse** à billet + **NUMR** (réponses à partir de **1**)

**CHRONO N°25** | Branche : **`feature/etape1-proto-tcp`**

## Base

- **Départ** : snapshot **N°24**.

## Livrables

| Fichier | Cible usuelle |
|---------|----------------|
| `serveur_complet_etape_programmation_reseaux_chrono_N25.c` | `src/server.c` |
| `client_complet_etape_programmation_reseaux_chrono_N25.c` | `src/client.c` |

## Modifications depuis N°24

| Élément | Détail |
|---------|--------|
| **`Post.next_reply`** | Incrémenté à chaque **REPLY** ; **`numr = po->next_reply`** après **`++`** (première réponse **1** si le post est initialisé à **0**). |
| **Serveur** | **`find_post(g, numb)`** pour retrouver le billet. |
| **Serveur** | **`handle_reply`** : membre du groupe, **REPLY_OK (15)** + **IDG** + **NUMB** + **NUMR**, **`notif_mcast(NEW_MSG)`**, et si l’auteur du billet ≠ répondant → **`notif_udp_user(..., NOTIF_FETCH, idg)`** (comme l’extrait). Le texte de la réponse n’est pas stocké (hors périmètre simplifié). |
| **Serveur** | **`serve_one_codereq`** : **REPLY (14)**, corps **UID + IDG + NUMB (4×3) + LEN (2)** + texte. |
| **Client** | **`cmd_reply`** + **`reply <uid> <idg> <numb> texte [mots …]`** (**argc ≥ 8**). Affichage **`OK reply numb=… numr=…`**. |

## Test rapide

```text
./paroles_client ::1 P post 1 1 sujet
./paroles_client ::1 P reply 1 1 0 ma réponse
→ OK reply numb=0 numr=1
```

## Commit exemple

```
CHRONO N°25 : REPLY (14) et REPLY_OK (15), NOTIF_FETCH vers l’auteur du billet
```
