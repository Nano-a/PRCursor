# Tâche 22 — Règle : **admin quitte** → **fermeture définitive** du groupe

**CHRONO N°22** | Branche : **`feature/etape1-proto-tcp`**

## Base

- **Départ** : snapshot **N°21** (`21/serveur_complet_…_N21.c`, `21/client_…_N21.c`).
- **Correction** : dans les snapshots **N22+**, le `#endif` du bloc `#ifndef PAROLES_ACCEPT_REAL_CLE_113` est placé **après** tout le code jusqu’à `main` (+ branche `#else` stub si la macro est définie), pour que `serve_one_codereq` voie bien les handlers (évite erreurs « implicit declaration »).

## Livrables (copier vers `programmation_reseaux`)

| Fichier | Cible |
|---------|--------|
| `serveur_complet_etape_programmation_reseaux_chrono_N22.c` | `src/server.c` |
| `client_complet_etape_programmation_reseaux_chrono_N22.c` | `src/client.c` |

## Fonctionnalité (déjà alignée avec l’extrait du dossier 22)

Dans **`handle_inv_ans`**, **AN = 2** et **`g->admin_id == uid`** → **`close_group(g)`**, **ACK**. Membre non admin + **AN = 2** → retrait du groupe, **ACK**, notification **LEAVE**.

## Test rapide

```text
./paroles_client ::1 P ans <admin_uid> <idg> 2
```
→ groupe fermé (`find_group` l’ignore) ; multicast **CLOSE** (21).

## Commit exemple

```
CHRONO N°22 : snapshot étape invitation (INV_ANS) + fermeture si admin quitte ; fix ifndef serveur
```
