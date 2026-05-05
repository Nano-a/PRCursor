# Tâche 22 — Règle : **admin quitte** → **fermeture définitive** du groupe

**CHRONO N°22** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1**

## Comportement

Dans **`handle_inv_ans`**, si **AN = 2** (quitter) et l’utilisateur est **admin** (`g->admin_id == uid`), le serveur appelle **`close_group(g)`** (notif multicast **CLOSE**, groupe marqué fermé) puis envoie **ACK**.

Un membre **non-admin** qui quitte avec AN=2 est simplement retiré du groupe (notif **LEAVE**).

## Fichier extrait (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_server_admin_quitte_close.c` | `src/server.c` — branche `an == 2` + admin dans `handle_inv_ans`. |

## Vérification

- Admin envoie `ans <admin> <idg> 2` → groupe inexistant pour les commandes suivantes ; notif 21 côté sujet.
