# Tâche 46 — Tests auto / manuels : scénarios complets + non-régression **CODEREQ**

**CHRONO N°46** | Branche : **`feature/etape1-tests-notifs`** | Qui : **M2**, **M1**

## Objectif

Couvrir les flux **étape 1** (inscription, groupes, invitations, post/reply/feed, erreurs métier) avec un script reproductible, en complément des scénarios par CODEREQ des dossiers **27–35**.

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `tests/regression_codereq.sh` | Scénario N°46 (source de vérité ; même logique que l’extrait ci-dessous). |
| `Makefile` | Cible `test` : enchaîne `verify_codereq_implemented`, `smoke`, **`regression_codereq`**, `feed_order_pdf`, `notif_codereq`, etc. |

## Consigne

- **Ne pas** remplacer tout le dépôt fac par l’extrait : copier le script dans `tests/regression_codereq.sh` **ou** lancer l’extrait depuis ce dossier livrable (il se place dans `PRCursor` via le chemin relatif).
- Prérequis : binaires `paroles_server` / `paroles_client` compilés (`make` à la racine de `PRCursor`).

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_regression_codereq.sh` | Même contenu que `PRCursor/tests/regression_codereq.sh` avec `ROOT` pointant vers `../../../PRCursor` depuis ce dossier. |

## Vérification

- Exécution sans erreur ; messages `regression_codereq OK` en fin de script.
- Sur le dépôt complet : `PORT=4245 make -C PRCursor test` (ou uniquement ce script après `make -s`).
