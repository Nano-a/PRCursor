# Tâche 55 — **Merge** `feature/etape2-tls` → **`develop`**

**CHRONO N°55** | Branche : **`develop`** ← merge | Qui : **M3**

## Objectif

Intégrer l’**étape 2** (TLS OpenSSL, scripts certificats, tests smoke) dans **`develop`** après revue et **`make test`** (incluant `tls_smoke.sh`).

## Commandes typiques

```bash
git fetch origin
git checkout develop
git pull origin develop
git merge --no-ff feature/etape2-tls -m "Merge branch 'feature/etape2-tls' into develop (CHRONO N°55)"
git push origin develop
```

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_merge_n55.sh` | Rappel merge (adapter remote). |

## Vérification

- `develop` contient TLS ; non-régression étape 1 (sans `--tls`) si vous conservez le mode clair en parallèle ; tests verts.
