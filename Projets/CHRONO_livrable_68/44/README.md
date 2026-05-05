# Tâche 44 — **Merge** `feature/etape1-proto-tcp` → **`develop`**

**CHRONO N°44** | Branche : **`develop`** ← merge | Qui : **M3**, **Tous** (revue)

## Objectif

Intégrer tout le travail **étape 1** (protocole TCP IPv6, sans TLS obligatoire dans cette branche) dans **`develop`**, après **revue** et **tests** (smoke, scénarios CODEREQ).

## Commandes typiques (dépôt GitLab / local)

```bash
git fetch origin
git checkout develop
git pull origin develop
git merge --no-ff feature/etape1-proto-tcp -m "Merge branch 'feature/etape1-proto-tcp' into develop (CHRONO N°44)"
# Résoudre conflits si besoin, puis :
git push origin develop
```

## Pourquoi `--no-ff` ?

Conserve un **commit de merge** explicite dans l’historique (visible sur GitLab / `git log --graph`).

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_merge_n44.sh` | Script rappel (adapter noms de remote/branche). |

## Vérification

- `develop` contient l’état attendu de l’étape 1 ; `make` / `make test` passent sur une machine propre.
