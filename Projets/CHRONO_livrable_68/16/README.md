# Tâche 16 — Formats **3–4** : création groupe (LEN + nom) + réponse **IDG**, **PORTMDIFF**, **IPMDIFF** (16 octets)

**CHRONO N°16** | Branche : **`feature/etape1-proto-tcp`** | Qui : tableau PR6  

## Référencés dans ce dossier

| Élément | Rôle |
|---------|------|
| `extrait_server_codereq_3_4.c` | Séquence **`wire_put` → réponse CODEREQ 4** (dans réf TLS : `conn_writen` → ici **`writen`**) |
| `extrait_client_codereq_3_4.c` | Fonction **`cmd_newgroup`** côté client |
| **`serveur_complet_etape_programmation_reseaux_chrono_N16.c`** | **Fichier entier à coller dans `programmation_reseaux/src/server.c`** après les commits **14–15** |
| **`client_complet_etape_programmation_reseaux_chrono_N16.c`** | **Fichier entier à coller dans `programmation_reseaux/src/client.c`** |

## Méthode (équipe Discord)

1. Replacer **`src/server.c`** par le contenu de **`serveur_complet_etape_programmation_reseaux_chrono_N16.c`** → `make`.
2. Replacer **`src/client.c`** par **`client_complet_etape_programmation_reseaux_chrono_N16.c`** → `make`.

## Ce qui change par rapport aux commits **≤ 15** (`server.c`)

En résumé (pour ouvrir le fichier et retrouver vite les ajouts majeurs sans « puzzle » avec les vieux extraits TLS) :

| Sujet | Modification concrète |
|-------|------------------------|
| **Structure `Group` + tableau** | `typedef struct { … } Group`, **`groups[MAX_GROUPS]`**, **`static uint32_t next_gid = 1;`** |
| **`find_user`** | Déjà utilisé pour vérifier l’ **`uid`** admin avant création groupe |
| **Lecture CODEREQ 3 sur la socket** | **`serve_one_codereq`** : premier octet = code ; pas seulement `REG` comme avant |
| **`handle_new_group_…`** | Parse **`LEN`** + **`nom`** (voir `extrait_server_codereq_3_4` pour la partie réponse après construction de `Group`) ; **`malloc` pour `g->name`** |
| **Adresse multicast** | Génération **`ff0e::1:<idg>`** + **`mcast_port`** — suffit déjà aux octets CODEREQ 4 ; lignes tableau **18+** préciseront encore la règle « unique » côté sujet si besoin |
| **Écriture CODEREQ 4** | Identique aux extraits (remplacer `conn_writen` par **`writen`**) |

## Ce qui change côté **`client.c`**

| Avant (≈ ligne 13) | Après (dossier 16) |
|--------------------|--------------------|
| Sous-commande **`reg`** seule | **`newgroup`** : `CMD` + **`uid`** + **`strlen`/`wire_put`** du nom puis TCP **connexion courte** (**`connect`**, **`writen`**, **`readn`**, **`close`**) comme pour `cmd_reg` |

## Protocole (rappel)

- **Codereq 3** : **`NEW_GROUP`** (1 octet), **`UID`** admin (4 BE), **`LEN`** (2 BE), **`nom`** (`LEN`).
- **Codereq 4** : **`NEW_GROUP_OK`**, **`IDG`** 4 octets BE, **`PORTMDIFF`** 2 BE, **`IP`** 16 octets binaires IPv6.

## Petit test manual

```text
terminal A : ./paroles_server ::1 …
terminal B : ./paroles_client ::1 … reg Alice
terminal B : ./paroles_client ::1 … newgroup <id_d’Alice> NomDuGroupe
```

La sortie du client doit contenir **`OK idg=`** + multicast + port.

## Commit suggestion

```
CHRONO N°16 : CODEREQ 3–4 (newgroup + NEW_GROUP_OK, TCP sans TLS)
```

---

## Fichiers extraits courts (historique livrables)

| Fichier | Source réf |
|---------|-------------|
| `extrait_client_codereq_3_4.c` | moule Cursor / TLS |
| `extrait_server_codereq_3_4.c` | idem (**`conn_writen`**) |

Les **fichiers `*_chrono_N16.c`** au-dessus sont la variante **`programmation_reseaux` complète**.
