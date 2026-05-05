# Tâche 33 — Fonctionnel : **liste des derniers billets** depuis **(n, r)**

**CHRONO N°33** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Objectif

Commande **feed** : **FEED** avec **IDG**, **NUMB**, **NUMR** ; affichage des entrées suivantes dans l’ordre global du fil (voir **N°26**).

## Convention pratique

**`(0, 0)`** : tout le fil depuis le début (extension alignée sur les tests `smoke` / PDF).

## Fichier extrait

| Fichier | Source |
|---------|--------|
| `extrait_scenario_feed.sh` | `feed U3 IDG 0 0` + grep — exige **`export U3 IDG`** et le fil produit par la tâche **32** (billets `b1`/`b2`, réponse `r-b2`). |

## Vérification

Après posts/réponses : `./paroles_client … feed <uid> <idg> 0 0 | grep billet=`
