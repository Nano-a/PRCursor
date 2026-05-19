# Tâche 23 — Formats **10–11** : liste membres (**IDG** ou **0** = tous inscrits)

**CHRONO N°23** | Branche : **`feature/etape1-proto-tcp`**

## Base

- **Départ** : snapshot **N°22**.

## Livrables

| Fichier | Cible |
|---------|--------|
| `serveur_complet_etape_programmation_reseaux_chrono_N23.c` | `src/server.c` |
| `client_complet_etape_programmation_reseaux_chrono_N23.c` | `src/client.c` |

## Modifications depuis N°22

| Côté | Détail |
|------|--------|
| **Serveur** | **`handle_list_mem`** (comme `extrait_server_codereq_10_11.c`) : **LIST_MEM_OK (11)** + **IDG** + **NB** puis (**ID**, **nom** 10) × NB. **IDG = 0** : tous les **users.used** ; sinon membre du groupe uniquement (**`group_is_member`**). Ordre **`g->mem[]`** : **admin en premier** (cf. **`group_add_member`** à la création). |
| **Serveur** | **`serve_one_codereq`** : branche **LIST_MEM (10)**, corps **UID (4) + IDG (4)**. |
| **Client** | **`cmd_listmem`** + **`listmem <uid> <idg>`** (**argc = 6** ; **idg = 0** = tous les inscrits). Réponse lue jusqu’à fin TCP (**`recv` en boucle**). |

## Test rapide

```text
./paroles_client ::1 P listmem 1 0
./paroles_client ::1 P listmem 1 <idg>
```

## Commit exemple

```
CHRONO N°23 : LIST_MEM (10) et LIST_MEM_OK (11), listmem client
```
