# Tâche 21 — CODEREQ **8**, **9**, **24** : réponse invitation (accepter / refuser / quitter)

**CHRONO N°21** | Branche : **`feature/etape1-proto-tcp`**

*(Le tableau PR6 mentionne aussi la règle **admin quitte → fermeture** en ligne **22** : elle est **déjà implémentée** ici via **`an == 2`** + **`g->admin_id == uid`** → **`close_group`**, comme dans `extrait_server_handle_inv_ans.c`.)*

## Base

- **Départ** : fichiers complets du dossier **`20/`**.

## Fichiers « copier-coller »

| Fichier | Cible |
|---------|--------|
| `serveur_complet_etape_programmation_reseaux_chrono_N21.c` | `src/server.c` |
| `client_complet_etape_programmation_reseaux_chrono_N21.c` | `src/client.c` |

## Modifications **depuis le dossier 20**

### Serveur

| Élément | Rôle |
|---------|------|
| **`group_remove_pending` / `group_remove_member`** | Retrait propre des tableaux **`pend`** / **`mem`**. |
| **`notif_mcast`** | Paquet UDP **6 octets** vers **`(g->mcast_ip, g->mcast_port)`** (code 16b + idg 32b). |
| **`close_group`** | **`g->closed = 1`** + notif **CLOSE (21)**. |
| **`handle_inv_ans`** | **AN=0** : refuse (retire pending, **ACK**). **AN=1** : accepte → **JOIN_OK (9)** + membres + **`notif_mcast(JOIN)`**. **AN=2** : quitte — si **admin** → **`close_group`** + **ACK** ; sinon retire membre + **ACK** + **`notif_mcast(LEAVE)`**. |
| **`serve_one_codereq`** | Branche **INV_ANS (8)** : lit **UID** (4 o) puis corps **idg (4) + an (1)**. |

### Client

| Élément | Rôle |
|---------|------|
| **`cmd_ans`** | Envoie **INV_ANS** ; lit la réponse jusqu’à fin TCP ; **ACK** → `OK ack` ; **JOIN_OK** → `OK join`. |
| **CLI** | `ans <uid> <idg> <0|1|2>` (**argc = 7**). |

## Exemples

```text
# Refus
./paroles_client ::1 P ans 2 1 0

# Acceptation
./paroles_client ::1 P ans 2 1 1

# Quitter (membre) / fermer (si admin)
./paroles_client ::1 P ans 1 1 2
```

## Commit exemple

```
CHRONO N°21 : INV_ANS (8), JOIN_OK (9), ACK (24) ; notif mcast ; admin quitte close_group
```
