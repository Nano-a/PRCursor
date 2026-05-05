# Tâche 5 — Créer et pousser la branche `develop` depuis `main`

**CHRONO N°5** | Branche : **`develop`** | Qui : **M3** (souvent)

## Prérequis

- `main` existe sur le dépôt GitLab et contient au moins le premier commit (**N°4**).

## Commandes (à lancer depuis le clone du dépôt fac)

```bash
git checkout main
git pull origin main
git checkout -b develop
git push -u origin develop
```

## Vérification « c’est fait »

```bash
git branch -a
# doit montrer origin/develop et develop en suivi
```

Sur GitLab : la branche **`develop`** est visible à côté de **`main`**.

## Suite logique

Travail de fonctionnalités sur des branches **`feature/...`** fusionnées dans **`develop`** (voir **N°9** et suivants).
