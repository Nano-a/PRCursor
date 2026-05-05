# Tâche 62 — Aligner les **signatures** sur le **cours**

**CHRONO N°62** | Branche : **`feature/etape3-ed25519-auth`** | Qui : **Tous**

## Objectif

Utiliser la **même** construction que le cours pour **ED25519** avec OpenSSL :

- **Pas de pré-hash** imposé par l’API : `EVP_DigestSignInit(ctx, NULL, NULL, NULL, sk)` et `EVP_DigestSign` sur le **message brut** (les 9 octets CODEREQ \| ID \| NUM ou AUTH_OK \| ID \| NUM).
- Même principe pour **`EVP_DigestVerify`** côté vérification.

Ne pas inventer un format « signe seulement ID+NUM » ou un hash SHA-256 manuel si le cours impose le schéma EVP « pur Ed25519 » sur le buffer complet.

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `src/auth_ed25519.c` | Commentaire d’en-tête + `paroles_ed25519_sign` / `paroles_ed25519_verify`. |

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_auth_ed25519_sign_verify.c` | Copie fidèle des fonctions sign/verify (+ en-tête fichier) depuis `PRCursor`. |

## Vérification

- Relecture avec le polycopié / TD ; tests `auth_full_smoke.sh` ; en cas d’écart avec le correcteur, ajuster **exactement** les appels EVP, pas le filaire Paroles.
