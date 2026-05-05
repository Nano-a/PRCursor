# Tâche 30 — Fonctionnel : **recevoir / lister invitations**

**CHRONO N°30** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Objectif

L’utilisateur invité appelle **LIST_INV** : réponse **LIST_INV_OK** avec pour chaque invitation **IDG**, nom du groupe, nom admin (10 octets).

## Référence code

Tâche **N°20** (formats 6–7).

## Fichier extrait

| Fichier | Source |
|---------|--------|
| `extrait_scenario_listinv.sh` | Smoke : après `invite`, `listinv` pour U2 + grep `idg=`. |

## Vérification

```bash
./paroles_client ::1 4242 listinv 2
# 2 = UID de l’invité ; attendu : lignes idg=... groupe=... admin=...
```

Automatique : `./paroles_client ... listinv "$U2" | grep -q idg=` (voir script).
