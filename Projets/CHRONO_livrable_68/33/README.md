# Tâche 33 — Fonctionnel : **liste du fil** (**feed**) depuis **(n, r)**

**CHRONO N°33** | Branche : **`feature/etape1-proto-tcp`**

## Objectif

Commande **`feed`** : **FEED** avec **IDG**, **NUMB**, **NUMR** ; le client imprime les entrées suivantes dans l’ordre global du groupe (**N°26**), au format :

```text
  [<uid_author> billet=<numb> rep=<nr>] <texte>
```

## Convention pratique

**`(NUMB=0, NUMR=0)`** : depuis le tout premier billet du groupe (extension alignée sur les tests **`smoke`** / PDF).

## Base

Snapshot **fonctionnel = même pile que N°32**.

## Livrables (`33/`)

| Fichier | Rôle |
|---------|------|
| `serveur_complet_etape_programmation_reseaux_chrono_N33.c` | **`handle_feed`**, même pile que N°32. |
| `client_complet_etape_programmation_reseaux_chrono_N33.c` | **`cmd_feed`** ; syntaxe **`feed <uid> <idg> <numb> <numr>`** (**`argc == 8`**). |
| **`extrait_scenario_feed.sh`** | **`feed … 0 0`** puis vérifie la présence de **`billet=1`** (billet **`b2`**) et du texte **`r-b2`** (issue du script **32**) ; exige **`export U3 IDG`** (et un fil préalable produit avec **32**). |

## Compilation (**sans** `PAROLES_ACCEPT_REAL_CLE_113`)

Même justification que **`32/`** : pile CHRONO + **`readn(..., 30000)`**.

Depuis **`33/`** :

```bash
gcc -Wall -Wextra -O2 -g \
  -I ../../../include -I ../../../src \
  -o paroles_server serveur_complet_etape_programmation_reseaux_chrono_N33.c \
  ../../../src/wire.c ../../../src/net.c
gcc -Wall -Wextra -O2 -g \
  -I ../../../include -I ../../../src \
  -o paroles_client client_complet_etape_programmation_reseaux_chrono_N33.c \
  ../../../src/wire.c ../../../src/net.c
```

## Vérification

Après un scénario **32**, :

```bash
./paroles_client ::1 4245 feed "$U3" "$IDG" 0 0 | grep billet=
```

Ou : **`chmod +x extrait_scenario_feed.sh`** puis **`PORT=4245 ./extrait_scenario_feed.sh`** avec **`export U3 IDG`**.

## Commit exemple

```
CHRONO N°33 : snapshots feed + README + extrait scenario
```
