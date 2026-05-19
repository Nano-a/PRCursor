# Tâche 30 — Fonctionnel : **lister les invitations**

**CHRONO N°30** | Branche : **`feature/etape1-proto-tcp`**

## Objectif

L’utilisateur **invité** appelle **LIST_INV (6)** : le serveur renvoie **LIST_INV_OK (7)** avec, pour chaque invitation en attente : **IDG**, **nom du groupe** (longueur + UTF-8), **nom admin** (10 octets **PAROLES_NOM_LEN**).

## Base

Snapshot **fonctionnel = même pile que N°29** ; mise en valeur **exclusive** dans ce dossier pour **LIST_INV** (**CHRONO 20**, formats **6–7**).

## Livrables (`30/`)

| Fichier | Rôle |
|---------|------|
| `serveur_complet_etape_programmation_reseaux_chrono_N30.c` | **`handle_list_inv`** et branche **`LIST_INV`** (déjà fournis depuis la pile N°20). |
| `client_complet_etape_programmation_reseaux_chrono_N30.c` | **`cmd_listinv`** — CLI **`listinv <uid>`** (**`argc == 5`**). |
| **`extrait_scenario_listinv.sh`** | **reg ×2 → newgroup → invite (`n=1`) → listinv `$U2` + `grep idg=`**. |

## Vérification

```bash
./paroles_client ::1 4242 listinv 2
# 2 = UID invité après invite ; lignes invitations idg=… groupe=… admin=…
```

## Commit exemple

```
CHRONO N°30 : scénario listinv (LIST_INV 6–7) + script avec invite correct
```
