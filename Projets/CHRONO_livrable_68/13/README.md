# Tâche 13 — Formats **CODEREQ 1–2** : inscription (NOM 10, CLE 113) + réponse ID, PORTUDP, CLE

**CHRONO N°13** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Message client → serveur (1)

- **1 octet** : `PAROLES_CODEREQ_REG` (= 1)
- **10 octets** : pseudo (padding `\0`) — `PAROLES_NOM_LEN`
- **113 octets** : CLE

## Réponse serveur → client (2)

- **1 octet** : `PAROLES_CODEREQ_REG_OK` (= 2)
- **4 octets BE** : ID utilisateur
- **2 octets BE** : port UDP notifications
- **113 octets** : CLE (étape 1 : zéros)

## Fichiers extraits (`PRCursor`)

| Fichier ici | Source |
|-------------|--------|
| `extrait_client_codereq_1_2.c` | `src/client.c` — construction requête + lecture réponse `REG_OK`. |
| `extrait_server_codereq_1_2.c` | `src/server.c` — `handle_reg` : corps de la réponse. |

## Vérification

- Tailles strictes : lecture côté serveur `NOM + CLE` ; côté client réponse `1+4+2+113` minimum.
