# Tâche 20 — Formats **6–7** : liste des invitations en attente

**CHRONO N°20** | Branche : **`feature/etape1-proto-tcp`**

## Base

- **Serveur / client précédents** : fichiers complets du dossier **`19/`**.

## Fichiers « copier-coller » dans `programmation_reseaux`

| Fichier dans ce dossier | Cible |
|-------------------------|--------|
| `serveur_complet_etape_programmation_reseaux_chrono_N20.c` | `src/server.c` |
| `client_complet_etape_programmation_reseaux_chrono_N20.c` | `src/client.c` |

## Modifications **depuis le dossier 19** (résumé commit)

| Zone | Changement |
|------|------------|
| **`struct Group`** | Champ **`int closed`** (à **0** à la création ; utilisé pleinement en **21** pour masquer les groupes fermés via `find_group`). |
| **`find_group`** | Ignore les groupes avec **`closed != 0`**. |
| **Serveur** | **`handle_list_inv`** : construit **LIST_INV_OK (7)** avec **nb** comptée en **deux passes** (placeholder `u32` puis entrées : **idg**, **len**, **nom**, **10 octets admin**), comme `extrait_server_codereq_6_7.c` — **`writen`** à la place de **`conn_writen`**. |
| **`serve_one_codereq`** | Après **INVITE** : branche **LIST_INV (6)** — lecture de **4 octets UID** puis réponse (corps additionnel **vide** côté client). |
| **Client** | **`cmd_listinv`** + CLI **`listinv <uid>`** (argc = **5** : `prog host port listinv uid`). Lecture réponse par **boucle `recv`** jusqu’à fermeture TCP (une requête par connexion). |

## Test rapide

Après **reg** + **newgroup** + **invite** un utilisateur **B** :

```
./paroles_client ::1 PORT listinv <uid_B>
```

## Commit exemple

```
CHRONO N°20 : LIST_INV / LIST_INV_OK (6–7) + flag Group.closed (préparation 21)
```
