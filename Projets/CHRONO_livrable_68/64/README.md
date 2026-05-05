# Tâche 64 — **Merge** `feature/etape3-ed25519-auth` → **`develop`**

**CHRONO N°64** | Branche : **`develop`** ← merge | Qui : **M3**

## Objectif

Intégrer l’**étape 3** (CLE 113 réelle, CODEREQ 0, signatures, compteur NUM) dans **`develop`** après revue et tests (`make test`, dont `auth_full_smoke.sh`).

## Commandes typiques

```bash
git fetch origin
git checkout develop
git pull origin develop
git merge --no-ff feature/etape3-ed25519-auth -m "Merge branch 'feature/etape3-ed25519-auth' into develop (CHRONO N°64)"
git push origin develop
```

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_merge_n64.sh` | Rappel merge. |

## Vérification

- `develop` contient TLS + auth ; tests verts sur machine propre / UFR.
