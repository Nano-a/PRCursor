# Tâche 58 — **Étape 3** : paires clés **ED25519** PEM ; **CLE 113** à l’inscription (vraies clés)

**CHRONO N°58** | Branche : **`feature/etape3-ed25519-auth`** | Qui : **M3**, **M1**

## Objectif

- Générer des paires **ED25519** au format **PEM** (`openssl genpkey` / `pkey -pubout`).
- **Client** : à `reg`, encoder les **32 octets** de la clé publique brute en tête du champ **CLE 113** (complété par des zéros), ou envoyer **113 zéros** si pas de PEM (mode étape 1).
- **Serveur** : accepter et **stocker** la CLE reçue lorsque le build définit **`PAROLES_ACCEPT_REAL_CLE_113`** (voir `Makefile` : `-DPAROLES_ACCEPT_REAL_CLE_113` dans `CFLAGS`).
- Réponse **REG_OK** : renvoyer la **CLE** telle qu’enregistrée (sujet étape 3).

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `scripts/gen_ed25519.sh` | Génère `key.pem` / `pub.pem`. |
| `src/auth_ed25519.c` | `paroles_ed25519_pubkey_from_cle` (32 premiers octets de CLE). |
| `src/server.c` | `handle_reg` : `#ifdef PAROLES_ACCEPT_REAL_CLE_113` (copie `u->cle`, echo dans réponse). |
| `src/client.c` | `fill_ed25519_cle_from_pem`, `cmd_reg` avec argument PEM optionnel. |
| `tests/stage3_cle_smoke.sh` | Inscription avec `pub.pem`. |

## Fichiers extraits

| Fichier | Rôle |
|---------|------|
| `extrait_gen_ed25519.sh` | = `PRCursor/scripts/gen_ed25519.sh`. |
| `extrait_auth_ed25519.h` | En-tête chargement PEM / sign / verify. |
| `extrait_auth_ed25519.c` | Implémentation (réf. cours : EVP, message brut). |
| `extrait_server_handle_reg_cle113.c` | `handle_reg` (zones `#ifdef PAROLES_ACCEPT_REAL_CLE_113`). |
| `extrait_client_reg_cle113.c` | `fill_ed25519_cle_from_pem` + `cmd_reg`. |
| `extrait_stage3_cle_smoke.sh` | Smoke avec `ROOT` → `../../../PRCursor`. |

## Vérification

- `./extrait_stage3_cle_smoke.sh` ou `PORT=4249 tests/stage3_cle_smoke.sh` dans `PRCursor`.
