# Tâche 19 — Formats **5** et **24** : **INVITE** + **ACK**

**CHRONO N°19** | Branche suggérée : **`feature/etape1-proto-tcp`**

## Méthode « fichier complet »

- **Base** : snapshot **dossier 18** → `serveur_complet_etape_programmation_reseaux_chrono_N18.c`.
- **Résultat** : `serveur_complet_etape_programmation_reseaux_chrono_N19.c` + `client_complet_etape_programmation_reseaux_chrono_N19.c` **ici** — remplacer **les deux** `src/*.c` dans `programmation_reseaux`.

## Fichiers

| Fichier | Rôle |
|---------|------|
| `serveur_complet_etape_programmation_reseaux_chrono_N19.c` | Serveur complet **N°19** |
| `client_complet_etape_programmation_reseaux_chrono_N19.c` | Client avec sous-commande **`invite`** |
| `extrait_server_invite_ack.c` | Mémo (TLS + `conn_writen`) |
| `extrait_client_codereq_5.c` | Mémo (`one_cmd` → chez nous **TCP court**) |

## Modifications **serveur** vs dossier **18**

| Thème | Implémentation |
|-------|----------------|
| **`find_group(idg)`** | Recherche groupe actif par **`idg`**. |
| **Invitations en attente** | `Group` : **`pend[MAX_GM]`** + **`npend`**, `group_is_pending`, `group_add_pending`. |
| **`send_ack`** | Envoie **`PAROLES_CODEREQ_ACK`** (24) + **15 octets de zéros** (`PAROLES_ERR_TAIL`) via **`writen`**. |
| **`notif_udp_user`** | Paquet UDP 6 octets *(code 16b BE, idg 32b BE)* vers **`u->reg_addr`** / **`udp_port`** — notif **22** pour chaque invité valide. |
| **`handle_invite`** | Parse **IDG**, **NB**, boucle **ID + ZEROS** ; vérif admin ; membres utilisateurs existants ; enqueue pending + notif UDP. |
| **`serve_one_codereq`** | Branche **`PAROLES_CODEREQ_INVITE`** : lit **admin, idg, nb** puis la queue (**`nb * (4 + PAROLES_INV_PAD)` octets**) ; appelle **`handle_invite`**. |

## Modifications **client** vs dossier **18**

| Thème | Implémentation |
|-------|----------------|
| **`cmd_invite`** | Comme **`extrait_client_codereq_5.c`** : CODEREQ 5, puis IDs + **`PAROLES_INV_PAD`** octets à zéro par invité. |
| **Lecture réponse** | **`readn`** **1 + PAROLES_ERR_TAIL** octets ; succès si **`resp[0] == PAROLES_CODEREQ_ACK`**. |
| **CLI** | `invite <admin_uid> <idg_group> <n> <uid1> [uid2 …]` où **`argc == 7 + n`**. |

## Exemple Manuel

Alice (1) cree groupe→ idg **1**. Bob (**2**) inscrit. Alice invite Bob :

```
./paroles_client ::1 5555 invite 1 1 1 2
```

Attend **`OK invite ACK`** côté client ; UDP **22** côté Bob (sans auditeur udp ici facultatif).

## Commit exemple

```
CHRONO N°19 : INVITE (CODEREQ 5) + ACK 24 ; pending groupe + notification UDP invitation
```
