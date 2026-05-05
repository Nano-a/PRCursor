# Tâche 34 — Rejet serveur : utilisateur inconnu, mal formé, pas membre, pas admin, etc.

**CHRONO N°34** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1**

## Comportement

Les handlers métier retournent **-1** si :

- utilisateur **inconnu** (`find_user` NULL) ;
- message **mal formé** (tailles `blen`, `wire_get_*`, corps invité sans padding, etc.) ;
- **pas membre** du groupe quand requis (`group_is_member`) ;
- **pas admin** pour **INVITE** (`g->admin_id != uid`) ;
- **pas invité** pour répondre à une invitation (`!group_is_pending`) ;
- **groupe absent** (`find_group` NULL) ;
- curseur **feed** introuvable (`feed_index_after` &lt; 0).

Le `dispatch` de `serve_client` appelle ensuite **`send_err`** ou **`send_err_msg`** (mode verbeux).

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_server_garde_rejets.c` | Plusieurs **gardes** typiques dans `server.c` (voir commentaires). |
| `extrait_scenario_rejet_newgroup_uid_inconnu.sh` | `regression_codereq.sh` : `newgroup 99999` doit échouer. |

## Vérification

- Client quitte avec code ≠ 0 ; octets reçus : **31** + ZEROS (N°35).
