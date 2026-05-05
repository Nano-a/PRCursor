# Tâche 66 — Relecture finale **checklist** + démo **soutenance** (mode verbeux)

**CHRONO N°66** | Branche : **`main`** / **`develop`** | Qui : **Tous**

## Objectif

- Parcourir **`CHRONO_TABLEAU_PR6.md`** et **`checklist_projet_PR6.md`** : chaque exigence du sujet est couverte (Makefile, GitLab, protocole, erreurs 31, notifs, TLS, auth ED25519, etc.).
- Préparer la **démonstration orale** : scénario court (inscription → groupe → message → notif éventuelle) avec le **mode verbeux** pour expliquer les échanges au tableau.

## Mode verbeux (`PRCursor`)

| Binaire | Option | Effet |
|---------|--------|--------|
| `paroles_server` | **`-v`** en **premier** argument | Active `vlog(...)` (traces sur stderr). |
| `paroles_client` | **`-v`** en **premier** argument | Affiche taille des réponses et codes utiles au debug. |

Exemples d’usage (adapter port, chemins TLS/cert si besoin) :

```text
./paroles_server -v ::1 4242
./paroles_client -v ::1 4242 reg Demo
```

Avec TLS + auth (référence dossiers 54 et 63) : placer `-v` avant `--tls` / `--key`.

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_demo_soutenance_verbose.sh` | Rappel : lancer serveur et client en `-v` depuis `PRCursor` (chemin relatif depuis ce dossier). |

## Vérification

- `make` et `make test` OK ; démo répétée au moins une fois en équipe ; pas de régression évidente sur `main`.
