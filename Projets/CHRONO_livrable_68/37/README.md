# Tâche 37 — Notifications **multicast** groupe : codes **18–21**, format **CODEREQ \| IDG** (BE)

**CHRONO N°37** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1**

## Émission

- Socket **UDP IPv6** vers **`g->mcast_ip`** et **`g->mcast_port`**.
- Paquet **6 octets** : **code** (uint16 BE) + **IDG** (uint32 BE).

## Codes (définitions)

| Code | Rôle (résumé) |
|------|----------------|
| 18 | Nouveau message / réponse sur le fil |
| 19 | Membre a rejoint le groupe |
| 20 | Membre a quitté le groupe |
| 21 | Groupe fermé (admin parti) |

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_paroles_proto_notif_mcast.h` | `include/paroles_proto.h` — constantes 18–21. |
| `extrait_server_notif_mcast.c` | `src/server.c` — `notif_mcast`. |

## Vérification

Test **`tests/notif_codereq.sh`** (post → notif **18** sur le multicast du groupe).
