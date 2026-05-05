# Tâche 19 — Formats **5** et **24** : **invitation** (IDG, NB, IDs + ZEROS) + **acquittement**

**CHRONO N°19** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Client → serveur (5)

- **INVITE** : ID admin, **IDG**, **NB**, puis pour chaque invité : **ID** (4 BE) + **ZEROS** (`PAROLES_INV_PAD` = 4 octets).

## Serveur → client (24)

- **ACK** (`PAROLES_CODEREQ_ACK`) avec queue d’erreur en zéros (format sujet).

## Fichiers extraits (`PRCursor`)

| Fichier | Source |
|---------|--------|
| `extrait_client_codereq_5.c` | `src/client.c` — `cmd_invite`. |
| `extrait_server_invite_ack.c` | `src/server.c` — `send_ack` + cœur de `handle_invite`. |

## Vérification

- Admin envoie invite ; réponse premier octet **24** ; client CLI considère succès si `resp[0] == ACK`.
