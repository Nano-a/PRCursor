# Procédure hotfix depuis `main` (CHRONO N°68)

À utiliser si un **bug critique** est découvert sur la branche **`main`** alors que le développement continue sur **`develop`**.

## Étapes

1. Créer une branche depuis **`main`** :  
   `git checkout main && git pull && git checkout -b hotfix/<sujet-court>`
2. Corriger le problème, **compiler**, lancer **`make test`** (ou les tests pertinents).
3. Merger le hotfix dans **`main`** (MR ou merge local selon votre politique GitLab).
4. Merger **la même branche (ou le même commit)** dans **`develop`** pour ne pas réintroduire le bug :  
   `git checkout develop && git merge hotfix/<sujet-court>`
5. Pousser **`main`** et **`develop`**, tagger si nécessaire (`v1.0.1`, etc.).

## Règles

- Un hotfix doit rester **minimal** (pas de refactor gratuit).
- Toujours **fusionner dans les deux branches** (`main` et `develop`) sauf consigne contraire du responsable dépôt.
