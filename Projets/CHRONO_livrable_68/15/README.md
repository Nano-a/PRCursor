# Tâche 15 — Règle : **même pseudo** possible, **IDs distincts**

**CHRONO N°15** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1**

## Règle métier

Deux inscriptions avec le **même nom** (mêmes 10 octets NOM) sont autorisées : le serveur attribue à chaque fois un **nouvel ID** (`next_uid++`). Il n’y a **pas** de contrainte d’unicité sur le pseudo dans l’implémentation de référence.

## Fichier extrait (`PRCursor`)

| Fichier ici | Source |
|-------------|--------|
| `extrait_server_meme_pseudo_ids_distincts.c` | `src/server.c` — `handle_reg` : `memcpy(u->nom, body, PAROLES_NOM_LEN)` + `u->id = next_uid++` (pas de recherche par nom). |

## Vérification

- Deux `./paroles_client … reg Alice` → deux lignes `OK id=…` avec **id** différents et le même pseudo affiché côté utilisateur.

## Test auto de référence

Le script `tests/regression_codereq.sh` du dépôt `PRCursor` inclut l’inscription triple « Alice » avec IDs distincts.
