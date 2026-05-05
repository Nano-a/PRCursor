# Tâche 36 — (Option sujet) Erreur **31** avec **longueur + message** texte (debug)

**CHRONO N°36** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1**

## Extension du format erreur

En plus du mode **31 + 15 octets ZEROS** (`send_err`), le serveur peut envoyer :

- **1 octet** : `PAROLES_CODEREQ_ERR` (31)  
- **2 octets BE** : longueur du message (max tronquée à 200 côté `PRCursor`)  
- **message** : octets UTF-8 (pas de padding ZEROS obligatoire dans cette variante).

## Quand c’est utilisé

Dans `PRCursor`, **`serve_client`** appelle **`send_err_msg(fd, "erreur requete")`** si **`verbose`** (`-v`) et traitement en erreur ; sinon **`send_err`** (ZEROS seuls).

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_server_send_err_msg.c` | `src/server.c` — `send_err_msg`. |

## Côté client (si vous affichez l’erreur)

Après le premier octet 31, si la réponse a **plus de 16 octets**, lire **uint16 BE** puis **len** octets de texte.

## Vérification

Lancer le serveur avec **`-v`**, provoquer une requête invalide ; observer une réponse plus longue qu’un simple 31+ZEROS.
