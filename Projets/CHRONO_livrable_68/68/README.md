# Tâche 68 — Bug sur **`main`** : branche **`hotfix/<sujet>`**, correction, merge **`main`** **et** **`develop`**

**CHRONO N°68** | Branche : **`hotfix/*`** | Qui : **Auteur du correctif** + **M3**

## Objectif

Si une erreur critique apparaît **après** la release sur `main` (soutenance, rendu, prod de démo), la corriger **sans** casser l’alignement des branches :

1. Partir de **`main`** à jour.
2. Créer **`hotfix/<description-courte>`** (ex. `hotfix/segfault-feed`).
3. Commits de correction, tests.
4. Merger le hotfix dans **`main`** et **pousser**.
5. Merger **`main`** (ou le hotfix) dans **`develop`** pour que la correction soit aussi présente en intégration continue — **pas** seulement sur `main`.

Sinon, le prochain travail sur `develop` réintroduira le bug au merge suivant.

## Séquence typique

```bash
git fetch origin
git checkout main
git pull origin main
git checkout -b hotfix/crash-udp-notif
# … corrections, commit …
git checkout main
git merge --no-ff hotfix/crash-udp-notif -m "hotfix: crash UDP notif (CHRONO N°68)"
git push origin main
git checkout develop
git pull origin develop
git merge --no-ff main -m "sync develop after hotfix (N°68)"
git push origin develop
```

Variante équivalente : merger `hotfix/...` dans `main` **puis** merger `hotfix/...` dans `develop` (évite un merge commit `main`→`develop` si vous préférez).

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_hotfix_n68.sh` | Après création de `hotfix/...` et **commits** : `./extrait_git_hotfix_n68.sh hotfix/nom` fusionne dans `main` puis `develop`. |

## Vérification

- Bug reproduit avant fix, plus reproduit après ; `main` et `develop` contiennent le même correctif ; tag éventuel **patch** (`v1.0.1`) si votre équipe versionne ainsi.
