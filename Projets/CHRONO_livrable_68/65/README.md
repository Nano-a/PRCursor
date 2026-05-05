# Tâche 65 — **Merge** `develop` → **`main`** ; tag **`v1.0-sujet-complet`** (ou équivalent)

**CHRONO N°65** | Branche : **`main`** ← `develop` | Qui : **M3**

## Objectif

Publier sur **`main`** la version **complète** du sujet (étapes 1–3 : proto, TLS, ED25519/auth) et poser un tag de release **`v1.0-sujet-complet`** (nom exact selon consigne équipe / enseignant).

## Commandes typiques

```bash
git fetch origin
git checkout main
git pull origin main
git merge --no-ff develop -m "Merge develop into main — sujet PR6 complet (CHRONO N°65)"
git push origin main
git tag -a v1.0-sujet-complet -m "PR6 — Paroles : étapes 1–3 complètes"
git push origin v1.0-sujet-complet
```

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_merge_tag_n65.sh` | Rappel merge + tag. |

## Vérification

- `main` à jour ; tag visible ; `make test` OK ; prêt pour soutenance (N°66).
