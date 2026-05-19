# Tâche 18 — Création groupe : multidiffusion **contrôlée** ; créateur = **admin** + **membre**

**CHRONO N°18** | Branche suggérée : **`feature/etape1-proto-tcp`**

## Méthode « fichier complet » (pas de placements au millimètre)

1. Tu considères comme **vérité précédente** le fichier **`../17/serveur_complet_etape_programmation_reseaux_chrono_N17.c`**.
2. Le fichier **`serveur_complet_etape_programmation_reseaux_chrono_N18.c` de CE dossier** est la version **après ligne tableau 18** : copier **en entier** → `programmation_reseaux/src/server.c`.
3. **`client_*_chrono_N18.c`** = même usage **reg / newgroup** que le dossier **17** (copie courte pour que les numéros de dossiers aient tous client+serveur sur disque).

## Fichiers utiles

| Fichier |
|---------|
| `serveur_complet_etape_programmation_reseaux_chrono_N18.c` |
| `client_complet_etape_programmation_reseaux_chrono_N18.c` |
| `extrait_server_mcast_admin.c` (mémo moule Cursor / TLS) |

## Changements codés depuis **dossier 17**

| Élément | Détail |
|---------|--------|
| **`Group`** | Ajout tableau **`mem[MAX_GM]`** + **`nmem`** pour stocker les **UID membres**. |
| **Helpers groupe** | `group_is_member`, `group_add_member`. |
| **Après `NEW_GROUP`** | Appel **`group_add_member(g, uid)`** : le créateur est **également premier membre**, comme **`extrait_server_mcast_admin.c`**. |
| **Multicast tableau « unique »** | Fonction **`mcast_pair_conflict`** : refus si un autre groupe actif utilise **déjà** le même couple **`(mcast_ip, mcast_port)`**. Avec `ff0e::1:<idg>` conventionnel ça marche comme **contrôle explicite** du tableau. |

Pas encore présent avant le dossier suivant (**19**) : **`find_group`**, invitations **pending**, etc.

## Test minimal

```
./paroles_server ::1 PORT
./paroles_client ::1 PORT reg Alice
./paroles_client ::1 PORT newgroup 1 Salon
```

## Commit exemple

```
CHRONO N°18 : NEW_GROUP ajoute créateur comme membre + contrôle multicast
```
