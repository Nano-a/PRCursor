# Tâche 48 — Vérif : ordre fil **16 / 17** = exemple PDF (ordre de réception global)

**CHRONO N°48** | Branche : **`feature/etape1-tests-notifs`** | Qui : **M1**

## Objectif

Valider que la réponse **FEED** (`CODEREQ` 16 → 17) intercale **billets** et **réponses** selon l’**ordre d’arrivée global** sur le groupe (comme l’exemple du sujet / PDF : après deux posts, les réponses apparaissent dans l’ordre où elles ont été reçues par le serveur).

## Fichiers dans `PRCursor`

| Fichier | Rôle |
|---------|------|
| `tests/feed_order_pdf.sh` | Scénario : posts `b0`, `b1`, puis `reply` sur 0 et 1 ; contrôle les triplets `(billet, rep)` extraits du fil. |
| `src/server.c` | Logique d’émission du fil (à comparer si le test échoue). |

## Consigne

- Utiliser l’extrait ci-dessous **ou** copier le script dans `tests/feed_order_pdf.sh` du dépôt fac.
- Le parsing du texte client (`billet=… rep=…`) doit rester aligné sur la sortie verbose de **votre** client.

## Fichier extrait

| Fichier | Rôle |
|---------|------|
| `extrait_feed_order_pdf.sh` | Adapté de `PRCursor/tests/feed_order_pdf.sh` avec chemin `PRCursor`. |

## Vérification

- Message `feed_order_pdf OK (N°48)` ; sinon ajuster l’implémentation feed côté serveur (file globale par groupe).
