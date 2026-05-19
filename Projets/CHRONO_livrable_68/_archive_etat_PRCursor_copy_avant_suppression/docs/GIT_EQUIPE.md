# Historique Git et répartition du travail (CHRONO N°67)

## Objectif

Les enseignants doivent voir des **commits attribuables** à chaque membre du trinôme sur le dépôt **GitLab UFR** (éviter une note basée sur un historique « invisible »).

## Bonnes pratiques

- **Configurer** `user.name` et `user.email` sur chaque machine utilisée pour pousser du code.
- **Alterner** les auteurs de commits sur les tâches réelles (serveur, client, tests, Makefile) plutôt qu’un seul compte pour tout le projet.
- Utiliser des messages de commit **explicites** (fonctionnalité, correctif, référence CHRONO si vous suivez le tableau projet).
- Les **fusions** (`develop`, `main`) et les **tags** de version peuvent être assurés par une personne (souvent « M3 »), mais le code des features doit montrer plusieurs contributeurs.

## Vérification rapide

```bash
git shortlog -sne --all
git log --oneline --all --graph -20
```

Chaque membre devrait apparaître sur des commits **de code** (pas seulement sur un README initial).
