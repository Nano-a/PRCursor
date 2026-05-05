# Tâche 31 — Fonctionnel : **accepter / refuser / quitter** groupe

**CHRONO N°31** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Objectif

- **AN = 1** : accepter une invitation → **JOIN_OK (9)** + liste membres / mcast.  
- **AN = 0** : refuser → **ACK (24)**.  
- **AN = 2** : quitter le groupe (membre) → **ACK** + notif leave ; si **admin** → **close_group** (N°22).

## Référence code

Tâche **N°21** (`cmd_ans`, `handle_inv_ans`).

## Fichier extrait

| Fichier | Source |
|---------|--------|
| `extrait_scenario_ans.sh` | `PRCursor/tests/regression_codereq.sh` — refus U2 (`ans 0`), accept U3 (`ans 1`), quit U3 (`ans 2`). |

## Vérification

Enchaîner `invite` → `listinv` → `ans … 0|1|2` et contrôler `OK ack` / `OK join`.
