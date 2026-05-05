# Tâche 38 — Notifications **UDP** personnelles : port à l’inscription ; **22** ; **23**

**CHRONO N°38** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1** (émission), **M2** (réception)

## Format

Même paquet **6 octets** que le multicast : **code** (uint16 BE) + **IDG** (uint32 BE), envoyé en **UDP** vers **`u->reg_addr`** avec port **`u->udp_port`** (fourni à l’inscription).

## Codes

- **22** : invitation reçue (`PAROLES_NOTIF_INV_UDP`) — émis dans **`handle_invite`** pour chaque invité.
- **23** : quelqu’un a **récupéré** un billet vous concernant (`PAROLES_NOTIF_FETCH`) — **`handle_reply`** et **`handle_feed`** (notif auteur du billet).

## Fichier extrait (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_server_notif_udp_user.c` | `src/server.c` — `notif_udp_user` + rappels des constantes 22/23 dans `paroles_proto.h`. |

## Vérification

`tests/notif_codereq.sh` : écoute UDP sur le port d’inscription de N2 puis `invite` → ligne **`22 <IDG>`**.
