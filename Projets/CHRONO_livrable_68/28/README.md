# Tâche 28 — Fonctionnel : **créer un groupe** (admin)

**CHRONO N°28** | Branche : **`feature/etape1-proto-tcp`**

## Objectif

Après inscription (**REG → REG_OK**), un utilisateur peut créer un groupe :  
**NEW_GROUP (3)** → **NEW_GROUP_OK (4)** avec **IDG**, adresse multicast, port ; créateur ajouté comme **membre admin**.

## Base

Snapshot **fonctionnel = N°26** (fil FEED inclus) ; cet incrément correspond à la **mise sous contrôle fonctionnel « newgroup »** des **CHRONO 16–18**.

## Livrables (dossier `28/`)

| Fichier | Rôle |
|---------|------|
| `serveur_complet_etape_programmation_reseaux_chrono_N28.c` | Copie pile complète (**même comportement que 26–27**), entête orientée NEW_GROUP. |
| `client_complet_etape_programmation_reseaux_chrono_N28.c` | Idem (**`cmd_newgroup`** déjà présente). |
| **`extrait_scenario_newgroup.sh`** | Enchaîne **2 × reg** puis **`newgroup <U1> MonGroupe`** (pour extraire **`U1`** depuis la sortie de `reg`). |

## Référence technique

Formats **3–4**, IDG dynamique / nom groupe / **mcast** + **handle_new_group_tcp** (**`GROUP_OK`** conforme **`paroles_proto.h`**).

## Vérification

```bash
# Terminal 1
./paroles_server ::1 4242

# Terminal 2
./paroles_client ::1 4242 reg Alice
./paroles_client ::1 4242 newgroup 1 MonGroupe
# où 1 = UID affiché par reg ; attendu : OK idg=… mcast=…
```

Ou : `chmod +x extrait_scenario_newgroup.sh && PORT=4242 ./extrait_scenario_newgroup.sh` depuis le répertoire contenant `paroles_client`.

## Commit exemple

```
CHRONO N°28 : scénario newgroup — snapshot pile complète inchangée
```
