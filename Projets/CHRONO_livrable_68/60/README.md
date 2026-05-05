# Tâche 60 — Message **CODEREQ 0** : ID, NUM (BE), LSIG, SIG sur les **3 premiers champs**

**CHRONO N°60** | Branche : **`feature/etape3-ed25519-auth`** | Qui : **M3**, **M1**

## Objectif

Format **auth client → serveur** (après le premier octet `0`) :

| Champ | Taille | Contenu |
|-------|--------|---------|
| ID | 4 octets | big-endian |
| NUM | 4 octets | big-endian |
| LSIG | 2 octets | longueur signature (64 pour ED25519) |
| SIG | LSIG octets | signature **du message de 9 octets** : `CODEREQ` (0) \| ID \| NUM |

Le serveur reconstruit les **9 octets** (`wire_put_u8` + deux `wire_put_u32_be`), vérifie avec la clé publique dérivée des **32 premiers octets** de la CLE 113 utilisateur, puis incrémente le compteur attendu (**NUM**, voir N°61).

Réponse **AUTH_OK (25)** : même logique côté serveur (message 9 octets à signer), paquet `25 \| ID \| NUM \| LSIG \| SIG`.

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `include/paroles_proto.h` | `PAROLES_CODEREQ_AUTH 0`, `PAROLES_CODEREQ_AUTH_OK 25`, `PAROLES_ED25519_SIG_LEN 64`. |
| `src/server.c` | `do_client_auth`, `send_auth_ok`. |
| `src/client.c` | Construction `tbs[9]`, `authpkt`, lecture réponse dans `one_cmd_authed`. |

## Fichiers extraits

| Fichier | Rôle |
|---------|------|
| `extrait_paroles_proto_auth0.h` | Constantes 0 / 25 / 64. |
| `extrait_server_do_client_auth.c` | `do_client_auth` + `send_auth_ok`. |
| `extrait_client_codereq0_auth_pkt.c` | Signer et envoyer le paquet 0 ; vérifier AUTH_OK. |

## Vérification

- Cohérent avec le **cours** (EVP, message brut — voir N°62) ; tests `auth_full_smoke.sh` avec TLS + clés PEM.
