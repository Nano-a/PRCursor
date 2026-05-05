# Tâche 51 — **Merge** `develop` → **`main`** ; tag optionnel `v0.1-etape1`

**CHRONO N°51** | Branche : **`main`** ← `develop` | Qui : **M3**

## Objectif

Figurer sur **`main`** l’état validé **étape 1** (protocole + tests notifs mergés dans `develop` après N°50). Le tag **`v0.1-etape1`** est **recommandé** pour marquer ce jalon sur GitLab.

## Commandes typiques

```bash
git fetch origin
git checkout main
git pull origin main
git merge --no-ff develop -m "Merge develop into main — étape 1 complète (CHRONO N°51)"
git push origin main
git tag -a v0.1-etape1 -m "PR6 étape 1 — proto TCP/IPv6, tests CODEREQ/notifs"
git push origin v0.1-etape1
```

Si votre équipe préfère **sans tag**, supprimez les deux dernières lignes (le tableau indique « tag optionnel »).

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_merge_tag_n51.sh` | Rappel merge + tag (adapter messages). |

## Vérification

- `main` à jour avec `develop` ; tag visible sur GitLab si poussé ; `make` / `make test` sur une branche propre.
