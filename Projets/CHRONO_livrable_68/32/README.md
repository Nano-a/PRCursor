# Tâche 32 — Fonctionnel : **poster** et **répondre**

**CHRONO N°32** | Branche : **`feature/etape1-proto-tcp`**

## Objectif

- **POST** (**N°24**) : tout **membre** du groupe peut publier du texte ; serveur attribue **`billet numb`** (0 pour le premier, 1 pour le deuxième, etc.).
- **REPLY** (**N°25**) : même membre (ou autre accepté dans le groupe) peut répondre sur un billet existant (**`reply <uid> <idg> <numb> <texte…>`**) ; première réponse sur ce billet a **`rep=1`** (affichée plus tard par **`feed`**).

## Base

Snapshot **fonctionnel = même pile que N°31** (**ans**, membres…) ; mise en valeur **POST / REPLY**.

## Livrables (`32/`)

| Fichier | Rôle |
|---------|------|
| `serveur_complet_etape_programmation_reseaux_chrono_N32.c` | **`handle_post`**, **`handle_reply`** (inchangées vs N°31). |
| `client_complet_etape_programmation_reseaux_chrono_N32.c` | **`cmd_post`**, **`cmd_reply`** (syntaxe ligne de commande). |
| **`extrait_scenario_post_reply.sh`** | Deux **`post`** (`b1`, `b2`) puis **`reply`** par **U3** sur le billet **numb=1** (texte **`r-b2`**) ; exige **`export U1 U3 IDG`** après un scénario type **tâche 31** jusqu’à **`ans … 1`** pour **U3**. |

### Numérotation billets (scénario du script)

- 1<sup>er</sup> `post` **`b1`** → **`numb = 0`**
- 2<sup>e</sup> `post` **`b2`** → **`numb = 1`** → le script répond avec **`reply "$U3" "$IDG" 1 …`**.

## Compilation (depuis ce répertoire, racine projet `PRCursor`)

**Sans** définition **`PAROLES_ACCEPT_REAL_CLE_113`** — pile TCP CHRONO (acceptation inscription avec clés nulles comme les dossiers précédents). Les appels utilisent **`readn(..., 30000)`** pour rester compatibles avec le **`net.h`** actuel (**délai 30 s par lecture**, cf. autres CHRONO mis à jour).

```bash
gcc -Wall -Wextra -O2 -g \
  -I ../../../include -I ../../../src \
  -o paroles_server serveur_complet_etape_programmation_reseaux_chrono_N32.c \
  ../../../src/wire.c ../../../src/net.c
gcc -Wall -Wextra -O2 -g \
  -I ../../../include -I ../../../src \
  -o paroles_client client_complet_etape_programmation_reseaux_chrono_N32.c \
  ../../../src/wire.c ../../../src/net.c
```

Les binaires doivent se trouver **dans ce dossier** pour que `./paroles_client` des scripts soit résolu tel quel.

## Scénarios manuels / extrait **`extrait_scenario_post_reply.sh`**

Le script ne démarre pas le serveur : le lancer ailleurs, puis depuis **`32/`**.

Il attend **`export U1 U3 IDG`** après un enchaînement type **31** jusqu’à **`ans … 1`** pour **U3** (membre accepté dans le groupe). **Important** : n’exécutez **pas** la ligne **`ans "$U3" "$IDG" 2`** du scénario d’origine (**31**) si elle « quit » le dernier flux — elle sort **U3** du groupe avant les **`post`** / **`reply`**.

Attendus côté client après **`post`** / **`reply`** : lignes **`OK billet numb=…`**, puis **`OK reply numb=… numr=…`**.

```bash
PORT=4245 ./extrait_scenario_post_reply.sh
```

## Commit exemple

```
CHRONO N°32 : snapshots post/reply + README + extrait scenario
```
