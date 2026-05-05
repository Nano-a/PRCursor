# Tâche 17 — Création groupe : **IDG** unique **> 0** ; **même nom** → **IDG** distincts

**CHRONO N°17** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1**

## Règles

- `next_gid` démarre à **1** : le premier groupe a **IDG ≥ 1**.
- Chaque nouveau groupe reçoit **`g->idg = next_gid++`** : pas de réutilisation d’ID.
- **Aucune** contrainte d’unicité sur le **nom** : deux groupes peuvent avoir le même libellé avec des **IDG** différents.

## Fichier extrait (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_server_idg_et_nom.c` | `src/server.c` — initialisation `next_gid`, attribution `idg`, copie du nom. |

## Vérification

- Deux `newgroup` avec le même nom de groupe → deux **idg** différents dans les réponses.
