# Tâche 11 — Étape 1 : sans TLS, sans auth 0 ; **CLE** = 113 octets **nuls** à l’inscription

**CHRONO N°11** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Contexte

À l’étape 1 du sujet, l’inscription envoie une clé **factice** : **113 octets à zéro**. Pas de TLS, pas de message **CODEREQ 0** (auth) avant le métier.

Dans `PRCursor` complet, le Makefile définit souvent `-DPAROLES_ACCEPT_REAL_CLE_113` pour l’étape 3 ; **sans** cette macro, le comportement ci-dessous correspond à l’étape 1.

## Fichiers extraits (`PRCursor`)

| Fichier ici | Source | Rôle |
|-------------|--------|------|
| `extrait_client_reg_cle_zero.c` | `src/client.c` | Branche **sans** `pub_pem` : `wire_put_zeros` sur **113** octets pour le champ CLE. |
| `extrait_server_reg_cle_zero.c` | `src/server.c` | `cle_is_zero`, refus si CLE non nulle ; `memset` / `wire_put_zeros` côté réponse **REG_OK**. |

## Copier dans le dépôt fac

- Fusionner les extraits dans `client.c` / `server.c` (ou fichiers équivalents), en respectant les `#ifdef` / `#ifndef` du projet si vous compilez étape 1 et étape 3 avec le même arbre.

## Vérification

- Inscription avec client **sans** fichier PEM : CLE = zéros ; serveur accepte ; réponse avec CLE zéros dans **REG_OK**.
