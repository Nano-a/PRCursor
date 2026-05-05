# Tâche 28 — Fonctionnel : **créer groupe** (admin)

**CHRONO N°28** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Objectif

Un utilisateur inscrit crée un groupe : **NEW_GROUP** → **NEW_GROUP_OK** avec **IDG**, multicast, port ; il devient **admin**.

## Référence code

Tâches **N°16–18** (formats 3–4, IDG, mcast, admin).

## Fichier extrait

| Fichier | Source |
|---------|--------|
| `extrait_scenario_newgroup.sh` | Suite logique après `reg` (smoke). |

## Vérification

```bash
./paroles_client ::1 4242 newgroup 1 MonGroupe
# id 1 = UID renvoyé par reg ; attendu : OK idg=...
```
