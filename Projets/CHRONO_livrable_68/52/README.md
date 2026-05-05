# Tâche 52 — Créer **`feature/etape2-tls`** depuis `develop`

**CHRONO N°52** | Branche : **`feature/etape2-tls`** | Qui : **M3**

## Objectif

Isoler tout le travail **TLS / OpenSSL** (étape 2) sur une branche dédiée, partie de l’état actuel de **`develop`** (post–étape 1).

## Commandes typiques

```bash
git fetch origin
git checkout develop
git pull origin develop
git checkout -b feature/etape2-tls
git push -u origin feature/etape2-tls
```

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_git_branch_tls_n52.sh` | Rappel des commandes. |

## Vérification

- Branche visible sur GitLab ; base = dernier `develop` avant les commits TLS.
