# Tâche 23 — Formats **10–11** : liste membres (**IDG** ou **0** = tous inscrits) ; **admin en premier**

**CHRONO N°23** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Client → serveur (10)

- **LIST_MEM** : **ID** demandeur, **IDG** (4 BE) — **IDG = 0** : liste de **tous** les utilisateurs inscrits sur le serveur.

## Serveur → client (11)

- **LIST_MEM_OK**, **IDG** (0 si tous inscrits), **NB**, puis (**ID**, **NOM** 10) × NB.

## Admin en premier

Dans `PRCursor`, le créateur est ajouté en premier avec `group_add_member` ; les autres sont ajoutés à la fin. La boucle d’émission parcourt `g->mem` dans l’ordre : le **premier** élément est l’**admin**.

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_client_codereq_10_11.c` | `src/client.c` — `cmd_listmem`. |
| `extrait_server_codereq_10_11.c` | `src/server.c` — `handle_list_mem`. |

## Vérification

- `listmem <uid> 0` : tous les inscrits ; `listmem <uid> <idg>` : membres du groupe, première ligne = admin.
