# Tâche 39 — Fonctionnel : **recevoir** les notifs (client : joindre mcast + écouter UDP)

**CHRONO N°39** | Branche de référence : **`feature/etape1-proto-tcp`** (évolué sur `main`) | Qui : **M2**, **M1**

## Contexte

Le polycopié exige que le **client** puisse **recevoir** les notifications (**UDP** personnelles, **multicast** de groupe).  
En plus de **`tests/notif_recv.py`** (Python), le binaire **`paroles_client`** du dépôt **`PRCursor`** expose désormais deux sous-commandes :

- **`listen_udp <port_udp> [sec]`** — écoute sur **`::`**, port = **PORTUDP** annoncé à l’inscription (**notif 22**, etc.).
- **`listen_mcast <ipv6> <port> [sec]`** — joint le groupe (**`IPV6_JOIN_GROUP`**) et reçoit le flux (**18**, **19**, …).

Chaque datagramme utile (≥ **6** octets) est affiché comme **`CODE IDG`** en décimal (**BE**), comme le script Python.

## Fichiers dans `PRCursor` (référence complète)

| Fichier | Rôle |
|---------|------|
| `src/client.c` | `write_notif_line`, `cmd_listen_udp`, `cmd_listen_mcast`, branchement dans `main`. |
| `src/net.c` / `net.h` | `udp6_bind_any`, `udp6_mcast_recv_socket` (bind + `join_mcast`). |

## Fichiers extraits (ce dossier)

| Fichier | Rôle |
|---------|------|
| `extrait_net_udp6_notif_recv.c` | Helpers réseau ajoutés pour la réception notifs. |
| `extrait_client_listen_notifs.c` | Commandes `listen_*` + affichage `code idg`. |

**Python** (inchangé utile pour les scripts) : `PRCursor/tests/notif_recv.py` — copie historique possible sous `extrait_notif_recv.py` si besoin de rendu sans recompiler le client.

## Vérification

- **`make test`** dans **`PRCursor`** (inclut **`notif_codereq.sh`**, **N°49**).
- Manuel (exemple) :  
  `./paroles_client ::1 4242 listen_udp 20002 15`  
  `./paroles_client ::1 4242 listen_mcast ff0e::1:1 30001 15`  
  (adapter **port TCP**, **PORTUDP**, **mcast**/**mport** du **`newgroup`**.)

## Chemin dépôt

`../../../PRCursor` depuis ce répertoire (`Projets/CHRONO_livrable_68/39` → `Projets/PRCursor`).
