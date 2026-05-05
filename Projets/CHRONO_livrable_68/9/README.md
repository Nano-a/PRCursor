# Tâche 9 — Créer la branche `feature/etape1-proto-tcp` depuis `develop`

**CHRONO N°9** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M3**

## Prérequis

- Branche **`develop`** existe et est à jour (**N°5**).

## Commandes

```bash
git checkout develop
git pull origin develop
git checkout -b feature/etape1-proto-tcp
git push -u origin feature/etape1-proto-tcp
```

## Ensuite

Tout le développement **étape 1** (protocole TCP IPv6 sans TLS, CLE nulle, pas d’auth 0) se fait sur cette branche jusqu’au merge vers `develop` (**N°44**).

## Vérification

```bash
git branch -vv
# feature/etape1-proto-tcp suit origin/feature/etape1-proto-tcp
```
