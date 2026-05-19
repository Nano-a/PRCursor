# Tâche 29 — Fonctionnel : **inviter** des membres

**CHRONO N°29** | Branche : **`feature/etape1-proto-tcp`**

## Objectif

L’**admin** (**UID** du groupe) envoie **INVITE (5)** avec **IDG** et la liste des **UID** invités ; le serveur répond **ACK (24)** et envoie une **notification UDP PAROLES_NOTIF_INV_UDP (22)** à chaque invité (pour l’instant implémenté côté serveur comme dans le snapshot N°19+).

## Base

Snapshot **fonctionnel = même pile que N°28** (**FEED**, posts, replies, etc.) ; cet incrément **documente** la chaîne **`invite`** (CHRONO **19**).

## Livrables (dossier `29/`)

| Fichier | Rôle |
|---------|------|
| `serveur_complet_etape_programmation_reseaux_chrono_N29.c` | Idem fonctionnel (**`handle_invite`**, **`notif_udp_user`**), entête INVITE. |
| `client_complet_etape_programmation_reseaux_chrono_N29.c` | **`cmd_invite`** : syntaxe **`invite <admin_uid> <idg> <n> <uid1> [<uid2> …]`**. |
| **`extrait_scenario_invite.sh`** | **reg** ×2 → **newgroup** → **invite avec `n=1`**. |

### Syntaxe importante (client CHRONO)

Le **3ᵉ paramètre** après **`<idg>`** est **`n`** (= nombre de **UID** invités), pas immédiatement le premier UID. Exemple :

```text
invite 1 2 1 3
```
= admin **1**, groupe **2**, **1** invité, UID **3**.

Le script extrait prend en charge ce format.

## Vérification succès ACK

Après `./paroles_client … invite …` : sortie **`OK invite ACK`** côté client.

Voir aussi **liste des invitations** (CHRONO suivants / **`listinv`**) pour l’invité.

## Commit exemple

```
CHRONO N°29 : scénario invite (ACK + NOTIF_UDP) — script corrigé n=invités
```
