# Tâche 14 — Échange **inscription** : pseudo + clé ; serveur **ID**, **port UDP**, **clé**, **IP client** ; **fin de connexion**

**CHRONO N°14** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Côté serveur

- Lit **NOM + CLE** après le code **REG**.
- Enregistre l’utilisateur avec **`u->reg_addr = *peer`** (adresse IPv6 du client vue au moment du `accept`) et un **port UDP** dérivé de l’ID (convention du projet).
- Envoie **REG_OK** puis la session TCP se **ferme** (une requête par connexion pour l’inscription, comme les autres commandes du client CLI).

## Côté client

- Envoie **REG**, lit **REG_OK**, affiche ID et UDP ; la connexion est fermée par le serveur après la réponse.

## Fichiers extraits (`PRCursor`)

| Fichier ici | Source |
|-------------|--------|
| `extrait_server_reg_peer_udp.c` | `src/server.c` — dans `handle_reg` : `udp_port`, `reg_addr`, préparation réponse. |
| `extrait_server_serve_client_reg_fin.c` | `src/server.c` — dans `serve_client` : branche `REG` puis `goto end` (fermeture SSL + `close`). |

## Vérification

- Après `reg`, `netstat` ou trace : plus de TCP établi pour cette session ; le serveur a bien mémorisé l’IPv6 du client pour les **notifs UDP** ultérieures.
