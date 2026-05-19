# Tâche 17 — IDG unique **strictement positif** ; **même nom** de groupe → **IDG** distincts

**CHRONO N°17** | Branche : **`feature/etape1-proto-tcp`** | Qui : tableau PR6  

## Fichiers livrés (copier-coller entier comme dossier **16**, avec doc N°17 intégrée au serveur)

| Fichier | Action |
|---------|--------|
| **`serveur_complet_etape_programmation_reseaux_chrono_N17.c`** | Remplace **`programmation_reseaux/src/server.c`** après commit **≤ 15** *(ou après le dossier 16 si tu recommits)* |
| **`client_complet_etape_programmation_reseaux_chrono_N17.c`** | Remplace **`src/client.c`** — **sans changement comportement CODEREQ** par rapport au client du dossier **16** (seul l’entête fichier est mis à jour) |

### Si tu arrives **juste après** le dossier 16 avec le même binaire fonctionnel  
Tu peux te contenter uniquement **`serveur_complet_etape_programmation_reseaux_chrono_N17.c`** puis **regarder le diff suivant**.

## Diff précis **`server.c`** : dossier **16 → 17**

| Dans le fichier N°17 | Lecture |
|----------------------|---------|
| Ligne **`static uint32_t next_gid = 1;`** précédée d’un bloc **CHRONO N°17** qui relie à **`extrait_server_idg_et_nom.c`** |
| Dans **`handle_new_group_tcp`** (nom renommé par rapport au N°16, même logique) : commentaire **long** après **`g->used = 1;`** avant **`g->idg = next_gid++`** expliquant : **pas d’unicité sur le nom** ; **`next_gid++`** donne des **IDG toujours plus grands**, donc deux groupes peuvent avoir le même `g->name` |

**Pas de nouveau `strcmp`/`search` doublons de nom.** C’est bien **ça** qui matérialise **`extrait_server_idg_et_nom.c`**.

### Code identique dossier **16 ↔ 17** hors commentaires  

- Fonctionnement machine et fil réseau : **identique** au fichier N°16 (les règles N°17 étaient **déjà vraies** si `next_gid++` était en place sans contrainte d’unicité sur le nom).  
- Le dossier **17** sert à ce que quelqu’un ouvrant **`serveur_*_N17.c`** **voie** ce qui doit être attesté comme **CHRONO ligne 17** sans chercher où coller trois lignes fragmentées dans un diff.

## Test tableau **17**

Après avoir un **`uid`** connu (Alice) :

```text
./paroles_client … newgroup … AliceID MonGroupe
./paroles_client … newgroup … AliceID MonGroupe
```

Deux lignes **`OK idg=`** avec deux **`idg` différents** et le **même** nom affiché côté humain pour le groupe.

## Commit suggestion  

```
CHRONO N°17 : IDG monotone (next_gid), pas d’unicité sur le nom de groupe (doc livrable)
```

## Fichier extrait référencé (historique)

| Fichier | Contenu livrable |
|---------|-------------------|
| `extrait_server_idg_et_nom.c` | **`next_gid`**, **`g->idg = next_gid++`**, **`malloc` + nom** |
