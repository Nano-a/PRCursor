# Tâche 31 — Fonctionnel : **répondre à une invitation (ans)**

**CHRONO N°31** | Branche : **`feature/etape1-proto-tcp`**

## Objectif

- **`ans … 1`** → accepter → **JOIN_OK (9)** (mcast, membres…)  
- **`ans … 0`** → refus → **ACK (24)**  
- **`ans … 2`** → quitter groupe ; si **admin** → **close_group** + fermeture (**CHRONO 22**) ; sinon **LEAVE**.

## Base

Snapshot **fonctionnel = même pile que N°30** ; mise en valeur **INV_ANS / handle_inv_ans / cmd_ans** (**CHRONO 21**).

## Livrables (`31/`)

| Fichier | Rôle |
|---------|------|
| `serveur_complet_etape_programmation_reseaux_chrono_N31.c` | **`handle_inv_ans`**, **`close_group`** (déjà présents). |
| `client_complet_etape_programmation_reseaux_chrono_N31.c` | **`cmd_ans`** — **`ans <uid> <idg> <0\|1\|2>`** (**`argc == 7`**). |
| **`extrait_scenario_ans.sh`** | **3 inscrits** ; **invite 2 destinataires** (`n=2`) → refus **`U2`**, accept **`U3`**, puis quit **`U3`**. |

### Syntaxe `invite`

**`invite <admin> <idg> <n> <uid₁> … <uidₙ>`**

Exemple deux invités : **`invite "$U1" "$IDG" 2 "$U2" "$U3"`**.

## Vérification

Attendus côté client : **`OK ack`** (refus / quit sans join), **`OK join`** (accept).

## Commit exemple

```
CHRONO N°31 : scénario ans (0 / 1 / 2), script avec invite n=2
```
