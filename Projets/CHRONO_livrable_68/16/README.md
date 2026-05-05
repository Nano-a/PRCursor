# Tâche 16 — Formats **3–4** : création groupe (LEN + nom) + réponse **IDG**, **PORTMDIFF**, **IPMDIFF** 16 octets

**CHRONO N°16** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Client → serveur (3)

- **CODEREQ_NEW_GROUP** (3), **ID** admin (4 octets BE), **LEN** (2 octets BE), **nom** (LEN octets).

## Serveur → client (4)

- **CODEREQ_NEW_GROUP_OK** (4), **IDG** (4 BE), **PORTMDIFF** (2 BE), **IPMDIFF** (16 octets IPv6 binaire).

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_client_codereq_3_4.c` | `src/client.c` — `cmd_newgroup`. |
| `extrait_server_codereq_3_4.c` | `src/server.c` — `handle_new_group` (réponse). |

## Vérification

- `newgroup` affiche `OK idg=… mcast=… port=…` ; octets fil conformes aux tailles ci-dessus.
