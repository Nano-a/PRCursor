# Tâche 24 — Formats **12–13** : **post** + réponse **NUMB** (billets à partir de **0**)

**CHRONO N°24** | Branche : **`feature/etape1-proto-tcp`**

## Base

- **Départ** : snapshot **N°23** (`/serveur_…_N23.c`, `/client_…_N23.c`).

## Livrables (dans ce dossier, copie vers `programmation_reseaux/src/` si besoin)

| Fichier | Cible usuelle |
|---------|----------------|
| `serveur_complet_etape_programmation_reseaux_chrono_N24.c` | `src/server.c` |
| `client_complet_etape_programmation_reseaux_chrono_N24.c` | `src/client.c` |

## Modifications depuis N°23

| Élément | Détail |
|---------|--------|
| **`Post` / `Group`** | Tableau **`posts[MAX_POSTS_PER_GROUP]`**, **`nposts`**, **`next_billet`** : chaque nouveau billet **`numb = g->next_billet++`** après assignation (premier billet **0**). |
| **Serveur** | **`handle_post`** : membre du groupe, copie du corps (`malloc`), **POST_OK (13)** + **IDG** + **NUMB**, **`notif_mcast(NEW_MSG)`**. |
| **Serveur** | **`serve_one_codereq`** : **POST (12)**, corps **UID (4) + IDG (4) + LEN (2)** + données (**LEN** ≤ **`PAROLES_MAX_BODY`**). |
| **Client** | **`cmd_post`** + ligne de commande **`post <uid> <idg> texte [mots …]`** (**argc ≥ 7** ; texte = reste d’`argv` concaténé avec des espaces). |

## Test rapide

```text
./paroles_client ::1 P post 1 1 Bonjour
→ OK billet numb=0
```

## Commit exemple

```
CHRONO N°24 : POST (12) et POST_OK (13), numérotation billets par groupe
```
