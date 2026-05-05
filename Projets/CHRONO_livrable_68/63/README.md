# Tâche 63 — Client : **TLS** + **auth** + **requête métier** sur la **même** connexion TCP

**CHRONO N°63** | Branche : **`feature/etape3-ed25519-auth`** | Qui : **M2**, **M3**

## Objectif

Sur une seule session :

1. **Handshake TLS** (`SSL_connect` après `tcp6_connect`).
2. Échange **CODEREQ 0** puis **AUTH_OK** signé (vérifié avec `--server-pub`).
3. **Sans fermer la socket** : envoi du paquet métier (`conn_writen` du message déjà construit) et lecture de la réponse (`conn_read_upto`).

C’est implémenté dans **`one_cmd_authed`** : pas de second `connect` entre auth et commande.

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `src/client.c` | `one_cmd_authed` : TLS → auth pkt → lecture AUTH_OK → `write_nonce` → `conn_writen(msg)` → `conn_read_upto`. |
| `tests/auth_full_smoke.sh` | TLS + `reg` (clair) + `newgroup` avec `--key` / `--server-pub`. |

## Fichiers extraits

| Fichier | Rôle |
|---------|------|
| `extrait_client_one_cmd_authed_tail.c` | Fin de `one_cmd_authed` : après auth, envoi métier sur le même `fd` / `g_io_ssl`. |
| `extrait_auth_full_smoke.sh` | Scénario complet (chemin `../../../PRCursor`). |

## Vérification

- `./extrait_auth_full_smoke.sh` depuis ce dossier ou `tests/auth_full_smoke.sh` dans `PRCursor`.
