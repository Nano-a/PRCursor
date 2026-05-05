# Tâche 49 — Vérif : **18–23** et **31** selon rôle client / serveur

**CHRONO N°49** | Branche : **`feature/etape1-tests-notifs`** | Qui : **M1**, **M2**

## Objectif

- **Multicast / UDP** : codes **18** (nouveau message) et **22** (invitation UDP) avec le format attendu (début du paquet : code + `IDG` en grand boutiste, comme dans le sujet).
- **TCP** : premier octet de réponse **31** (`CODEREQ_ERR`) sur une requête **mal formée** (ici : envoi d’un octet invalide `99` sans en-tête complet).

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `tests/notif_codereq.sh` | Orchestration N°49. |
| `tests/notif_recv.py` | `udp` / `mcast` / `tcp_bad` (réception ou sonde TCP). |

## Consigne

- **`python3`** requis. Le script extrait utilise **`$ROOT/tests/notif_recv.py`** avec `ROOT` = `PRCursor` (chemin relatif depuis ce dossier).
- Pour un dépôt **étape 1 sans TLS**, adapter si votre stack impose encore du TLS sur le port TCP (sinon le test `tcp_bad` doit rester valide).

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_notif_codereq_n49.sh` | Adapté de `PRCursor/tests/notif_codereq.sh`. |

## Vérification

- `notif_codereq OK (N°49 — échantillon 18, 22, 31)`.
