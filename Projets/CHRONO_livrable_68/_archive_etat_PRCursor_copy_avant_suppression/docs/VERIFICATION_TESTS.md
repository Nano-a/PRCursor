# Comment vérifier le projet Paroles (tests automatiques et manuels)

Ce guide décrit **où taper les commandes**, **combien de terminaux** ouvrir, et **ce que chaque vérification couvre**.  
L’alignement sur le sujet officiel repose sur **`Projets/checklist_projet_PR6.md`** (référence au PDF *projet PR6*). Le PDF lui-même n’est pas copié dans ce dépôt : placez-le ou ouvrez-le depuis les fichiers fournis par l’enseignante.

---

## Prérequis

- Être dans le répertoire du dépôt compilable, par exemple :
  ```bash
  cd /chemin/vers/PRCursor
  ```
- Dépendances : compilateur C, **OpenSSL** (headers + libs), `make`, `bash`.

---

## 1. Tests automatiques (recommandé en premier)

### Combien de terminaux ?

**Un seul** terminal suffit : tout est enchaîné par `make`, les serveurs de test sont lancés en arrière-plan puis arrêtés par les scripts.

### Commande

```bash
cd /chemin/vers/PRCursor
make test
```

### Résultat attendu

À la fin, chaque étape affiche une ligne du type `… OK (…)` et la commande se termine **sans erreur** (code de sortie 0). Si une étape échoue, `make` s’arrête et affiche une erreur.

### Ce qui est couvert (lien avec la checklist / le sujet)

| Étape | Script / cible | Rôle |
|--------|----------------|------|
| CODEREQ | `tests/verify_codereq_implemented.sh` | Présence des codes **0, 1–17, 24–25, 31** et branches serveur associées (reprend la synthèse « formats TCP » du sujet). |
| Scénario de base | `tests/smoke.sh` (port 4242) | TCP sans TLS : **reg**, **newgroup**, **invite**, **listinv**, **ans** (join), **post**, **reply**, **feed** — flux minimal du protocole. |
| Non-régression large | `tests/regression_codereq.sh` (4245) | Même pseudo **IDs distincts**, invitations refus / accept, **listmem** (idg=0 et groupe), posts, erreur métier, **quitter** groupe. |
| Ordre du fil 16/17 | `tests/feed_order_pdf.sh` (4246) | Ordre global des entrées **feed** (comme les exemples du PDF). |
| Notifications | `tests/notif_codereq.sh` (4247) | Échantillon **multicast / UDP** (codes du sujet : 18–23, 31 côté erreur). |
| TLS | `tests/tls_smoke.sh` (4248) | **Étape 2** : session **TLS**, inscription. |
| CLE 113 réelle | `tests/stage3_cle_smoke.sh` (4249) | **Étape 3** : inscription avec clé **ED25519** dans le champ CLE (PEM → 113 octets). |
| TLS + auth + métier | `tests/auth_full_smoke.sh` (4251) | **Étape 3** : après **REG**, même connexion : **AUTH (0)**, **AUTH_OK (25)**, puis **newgroup** sur la même session TLS. |

### Lancer un script seul (débogage)

Les scripts utilisent la variable **`PORT`** si elle est définie :

```bash
cd /chemin/vers/PRCursor
make -s
chmod +x tests/smoke.sh
PORT=4242 tests/smoke.sh
```

Même principe pour `regression_codereq.sh`, `tls_smoke.sh`, etc. (voir `Makefile`, cible `test`).

---

## 2. Tests manuels « comme en soutenance »

### Combien de terminaux ?

En général **deux** :

1. **Terminal A** : serveur (reste ouvert, affiche les logs si `-v`).
2. **Terminal B** : client (une commande = une connexion TCP, sauf précision).

Pour observer les **notifications UDP** ou rejoindre le **multicast**, vous pouvez ouvrir un **troisième** terminal ou des outils (`tcpdump`, etc.) — ce n’est pas obligatoire pour valider le cœur du protocole.

### 2.1 TCP clair, sans TLS (proche étape 1)

**Terminal A :**

```bash
cd /chemin/vers/PRCursor
./paroles_server -v ::1 4242
```

**Terminal B — enchaînement typique :**

```bash
cd /chemin/vers/PRCursor
./paroles_client -v ::1 4242 reg Alice
./paroles_client -v ::1 4242 newgroup 1 MonGroupe
# Remplacer 1 par l’ID affiché après reg ; idem pour les commandes suivantes.
./paroles_client -v ::1 4242 invite 1 1 2
./paroles_client -v ::1 4242 listinv 2
./paroles_client -v ::1 4242 ans 2 1 1
./paroles_client -v ::1 4242 post 1 1 "Hello"
./paroles_client -v ::1 4242 reply 2 1 0 "Réponse"
./paroles_client -v ::1 4242 feed 2 1 0 0
```

Aide sur les arguments :

```bash
./paroles_client
./paroles_server
```

### 2.2 TLS sans authentification ED25519 sur chaque commande

Générer des certificats (une fois) :

```bash
cd /chemin/vers/PRCursor
chmod +x scripts/gencerts.sh
scripts/gencerts.sh "$(pwd)/tests/fixtures/certs"
CERT="$(pwd)/tests/fixtures/certs"
```

**Terminal A :**

```bash
./paroles_server -v --tls "$CERT/server.pem" "$CERT/server.key" ::1 4248
```

**Terminal B :**

```bash
./paroles_client -v --tls "$CERT/ca.pem" ::1 4248 reg Bob
./paroles_client -v --tls "$CERT/ca.pem" ::1 4248 newgroup 1 Gtls
```

### 2.3 TLS + auth ED25519 (sujet complet)

Il faut des paires de clés PEM utilisateur et serveur. Exemple avec les scripts du dépôt :

```bash
cd /chemin/vers/PRCursor
chmod +x scripts/gen_ed25519.sh
U="$(pwd)/tests/fixtures/ed25519_u"
S="$(pwd)/tests/fixtures/ed25519_srv"
scripts/gen_ed25519.sh "$U"
scripts/gen_ed25519.sh "$S"
scripts/gencerts.sh "$(pwd)/tests/fixtures/certs"
CERT="$(pwd)/tests/fixtures/certs"
```

**Terminal A :**

```bash
./paroles_server -v --tls "$CERT/server.pem" "$CERT/server.key" --signing-key "$S/key.pem" ::1 4251
```

**Terminal B — inscription** (sans `--key`, toujours en « plain » après TLS) :

```bash
./paroles_client -v --tls "$CERT/ca.pem" ::1 4251 reg Carol "$U/pub.pem"
```

**Terminal B — chaque autre commande** : ajouter **`--key`** (privé utilisateur) et **`--server-pub`** (public serveur pour vérifier AUTH_OK) :

```bash
./paroles_client -v --tls "$CERT/ca.pem" --key "$U/key.pem" --server-pub "$S/pub.pem" ::1 4251 newgroup 1 Gauth
```

Le client enregistre un compteur **NUM** par utilisateur dans `/tmp/paroles_nonce_<port>_<uid>`. Si vous réinitialisez le serveur ou refaites des tests bizarres, supprimez ce fichier pour repartir de **NUM = 0** :

```bash
rm -f /tmp/paroles_nonce_4251_1
```

---

## 3. Synthèse « est-ce que tout fonctionne ? »

1. **`make test` tout vert** → les scénarios automatisés du dépôt passent ; c’est la base la plus fiable avant la soutenance.  
2. **Relecture checklist** : ouvrir `Projets/checklist_projet_PR6.md` et parcourir les points (notifications reçues côté client, détails du PDF, etc.) : certains points sont **partiellement** couverts seulement par les scripts (voir la checklist elle-même).  
3. **Machine UFR** : retester `make` et `make test` sur l’environnement de rendu, comme demandé dans le sujet.

---

## 4. Dépannage rapide

| Problème | Piste |
|----------|--------|
| `make test` échoue sur un port | Un autre processus utilise le port ; changez `PORT=…` ou tuez l’ancien `paroles_server`. |
| Auth TLS échoue après plusieurs essais | Supprimer `/tmp/paroles_nonce_<port>_<uid>` et réaligner serveur + client (même utilisateur / même NUM). |
| Certificats manquants | Relancer `scripts/gencerts.sh` vers `tests/fixtures/certs`. |
