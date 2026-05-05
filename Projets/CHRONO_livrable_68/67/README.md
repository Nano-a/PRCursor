# Tâche 67 — Vérifier **l’historique Git** : commits **visibles** pour **chaque** membre

**CHRONO N°67** | Branche : **toutes** | Qui : **Tous**

## Objectif

Le sujet exige une **participation réelle** de chaque étudiant du trinôme. Sur **GitLab** (ou en local), contrôler que **chaque** personne apparaît comme **auteur** de commits sur le dépôt du projet — sinon risque de **note très basse** ou **0** pour l’implication.

## Vérifications utiles (local)

- Auteurs distincts sur la branche principale :

  ```bash
  git shortlog -sne main
  ```

- Commits récents avec nom d’auteur :

  ```bash
  git log main --oneline --no-walk=sorted --format='%h %an %s' | head -30
  ```

- Sur GitLab : onglet **Repository → Contributors** (selon version) ou historique des **merge requests**.

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_verif_contributeurs.sh` | Commandes de contrôle rapide (adapter `main` / `develop`). |

## Que faire si un membre n’a aucun commit ?

- Lui attribuer des tâches **commitées** (code, tests, doc, fix) **avant** le rendu final ; éviter un dépôt où un seul auteur domine tout l’historique.
