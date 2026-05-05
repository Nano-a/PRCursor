# Tâche 45 — Créer **`feature/etape1-tests-notifs`** depuis **`develop`**

**CHRONO N°45** | Branche : **`feature/etape1-tests-notifs`** | Qui : **M3**

## Objectif

Ouvrir une branche dédiée aux **tests automatisés / manuels** (non-régression CODEREQ, notifs, scripts `tests/*.sh`) sans mélanger avec le développement feature déjà mergé.

## Commandes

```bash
git checkout develop
git pull origin develop
git checkout -b feature/etape1-tests-notifs
git push -u origin feature/etape1-tests-notifs
```

## Suite logique (CHRONO)

Tâches **46–49** : scénarios de test, vérif CODEREQ, ordre fil, notifs ; puis **50** merge de cette branche dans **`develop`**.

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_branch_tests_n45.sh` | Rappel des commandes. |

## Vérification

```bash
git branch -vv
# feature/etape1-tests-notifs suit origin/feature/etape1-tests-notifs
```
