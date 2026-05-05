# Tâche 56 — **Merge** `develop` → **`main`** ; tag **`v0.2-tls`**

**CHRONO N°56** | Branche : **`main`** ← `develop` | Qui : **M3**

## Objectif

Publier sur **`main`** la version **TLS** (étape 2 mergée dans `develop` après N°55) et poser le tag **`v0.2-tls`** sur GitLab.

## Commandes typiques

```bash
git fetch origin
git checkout main
git pull origin main
git merge --no-ff develop -m "Merge develop into main — étape 2 TLS (CHRONO N°56)"
git push origin main
git tag -a v0.2-tls -m "PR6 étape 2 — TCP + TLS OpenSSL"
git push origin v0.2-tls
```

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_merge_tag_n56.sh` | Rappel merge + tag. |

## Vérification

- `main` aligné avec `develop` post-TLS ; tag `v0.2-tls` visible ; tests passent.
