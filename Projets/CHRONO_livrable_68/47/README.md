# Tâche 47 — Vérif : **tous les CODEREQ** 1–17, 24, 31 (0 réservé à l’étape 3)

**CHRONO N°47** | Branche : **`feature/etape1-tests-notifs`** | Qui : **M2**

## Objectif

Contrôle **statique** (sans réseau) que :

- Les constantes dans `include/paroles_proto.h` ont les **valeurs attendues** (1–17, 24, 31 ; **0** uniquement pour `PAROLES_CODEREQ_AUTH` en étape 3).
- Le serveur déclare un `case` pour chaque requête métier **REG … FEED** dans le dispatch principal.
- La chaîne **auth** (`do_client_auth`) est présente (projet complet étape 3).

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `tests/verify_codereq_implemented.sh` | Script N°47 (source de vérité). |
| `include/paroles_proto.h` | Définitions numériques CODEREQ / notifs. |
| `src/server.c` | `case PAROLES_CODEREQ_*` + auth. |

## Consigne

- Lancer depuis la racine **`PRCursor`** : `./tests/verify_codereq_implemented.sh`, ou utiliser **`extrait_verify_codereq_implemented.sh`** depuis ce dossier (résout le chemin vers `PRCursor`).

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_verify_codereq_implemented.sh` | Copie adaptée de `PRCursor/tests/verify_codereq_implemented.sh`. |

## Vérification

- Message final : `verify_codereq_implemented OK (N°47)`.
