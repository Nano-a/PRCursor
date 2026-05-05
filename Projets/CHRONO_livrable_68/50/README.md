# Tâche 50 — **Merge** `feature/etape1-tests-notifs` → **`develop`**

**CHRONO N°50** | Branche : **`develop`** ← merge | Qui : **M3**

## Objectif

Intégrer la branche des **tests et vérifications** (N°46–49) dans **`develop`** une fois les scripts verts en CI / localement (`make test` ou équivalent).

## Commandes typiques

```bash
git fetch origin
git checkout develop
git pull origin develop
git merge --no-ff feature/etape1-tests-notifs -m "Merge branch 'feature/etape1-tests-notifs' into develop (CHRONO N°50)"
# Conflits éventuels : résoudre, retester, puis :
git push origin develop
```

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_merge_n50.sh` | Rappel de commandes (adapter remote / noms de branche). |

## Vérification

- `develop` contient les commits de tests ; `make` / `make test` OK après merge.
