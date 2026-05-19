# Tâche 27 — Fonctionnel : **inscription** (`reg` → **REG_OK**)

**CHRONO N°27** | Branche : **`feature/etape1-proto-tcp`**

## Base

Le **code exécutable** est le **même snapshot que le CHRONO N°26** (pile déjà disponible jusqu’aux **LIST_MEM**, posts, replies, feed). Ce numéro sert à **tracabiliser la consigne inscription** (**tâches 11–13–14**) et les **tests**.

## Livrables (dossier `/27/`)

| Fichier | Rôle |
|---------|------|
| `serveur_complet_etape_programmation_reseaux_chrono_N27.c` | Copie **alignée sur N°26** (entête différent uniquement si besoin). |
| `client_complet_etape_programmation_reseaux_chrono_N27.c` | Idem — **`cmd_reg`** déjà présent (**CLE** nulle étape 1). |
| **`extrait_scenario_reg.sh`** | Scénario minimal : `./paroles_client … reg Alice` puis **Bob**. |

Pour un dépôt de code unique, utilise plutôt **N°26** dans `src/` ; **N°27** sert au **parcours README + script**.

## Test manuel (cf. fichier extrait)

```bash
PORT=4242 ./paroles_server ::1 "$PORT"
# autre terminal
./paroles_client ::1 "$PORT" reg Alice
# Attendu : OK id=1 udp=…
```

Ou : `chmod +x extrait_scenario_reg.sh && PORT=4242 ./extrait_scenario_reg.sh` (avec binaires sur le `$PATH` courant ou adapter les chemins).

## Commit exemple

```
CHRONO N°27 : scénario inscription (REG déjà présent dans snapshot N°26)
```
