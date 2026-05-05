# Tâche 29 — Fonctionnel : **inviter**

**CHRONO N°29** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Objectif

L’**admin** envoie **INVITE** avec **IDG** et la liste des **UID** invités ; réponse **ACK (24)** ; les invités reçoivent une **notif UDP 22** (côté serveur).

## Référence code

Tâche **N°19** (formats 5 et 24).

## Fichier extrait

| Fichier | Source |
|---------|--------|
| `extrait_scenario_invite.sh` | Chaîne reg ×2 → newgroup → invite (smoke). |

## Vérification

Après invite, `listinv` côté invité doit montrer le groupe (tâche **N°30**).
