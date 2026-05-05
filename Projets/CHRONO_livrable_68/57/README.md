# Tâche 57 — Créer **`feature/etape3-ed25519-auth`** depuis `develop`

**CHRONO N°57** | Branche : **`feature/etape3-ed25519-auth`** | Qui : **M3**

## Objectif

Isoler l’**étape 3** : clés **ED25519** en PEM, champ **CLE 113** réel à l’inscription, **CODEREQ 0** et réponses signées, sans mélanger avec le travail TLS déjà sur `main`/`develop`.

## Commandes typiques

```bash
git fetch origin
git checkout develop
git pull origin develop
git checkout -b feature/etape3-ed25519-auth
git push -u origin feature/etape3-ed25519-auth
```

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_branch_ed25519_n57.sh` | Rappel création + push de la branche. |

## Vérification

- Branche sur GitLab ; point de départ = `develop` après merge TLS (N°55).
