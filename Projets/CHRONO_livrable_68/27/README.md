# Tâche 27 — Fonctionnel : **inscription**

**CHRONO N°27** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Objectif

Un client peut s’**inscrire** : envoi **REG**, réception **REG_OK** avec **ID** et **port UDP** (et CLE selon étape).

## Référence code (détail des messages)

Tâches **N°13–14** (formats 1–2) et **N°11** (CLE nulle étape 1).

## Fichier extrait — scénario de test

| Fichier | Source |
|---------|--------|
| `extrait_scenario_reg.sh` | Adapté de `PRCursor/tests/smoke.sh` (inscriptions). |

## Vérification manuelle

```bash
# Terminal 1 : serveur
./paroles_server ::1 4242

# Terminal 2
./paroles_client ::1 4242 reg Alice
# Attendu : OK id=1 udp=...
```

Automatique : `PORT=4242 tests/smoke.sh` (début du script).
