# Guide de test complet — Projet **Paroles** (PRCursor)

Version pédagogique : tout ce que couvrent les scripts automatiques (`make test`) et plus, **à refaire à la main** avec le terminal.  
Adapte le chemin si ton dépôt n’est pas au même endroit.

**Chemin de référence :** `/home/ajinou/Bureau/Perso/L3/S6/Reseau/PRCursor`

Un guide plus court (démarrage rapide) existe : `GUIDE_TEST_MANUEL_PAROLES.md` dans le même dossier `Projets/`.

---

## Table des matières

1. [Ce que tu dois avoir installé](#1-prérequis)
2. [Comment lire ce document (terminaux, variables)](#2-conventions--rôles-des-terminaux)
3. [Préparation : aller dans le projet et compiler](#3-préparation-unique)
4. [La batterie automatique `make test` (explication)](#4-la-batterie-automatique-make-test)
5. [Scénario manuel complet — du « smoke » au feed](#5-scénario-manuel-a--le-parcours-type-smoke)
6. [Scénario avancé — régression (plusieurs Alice, refus, listmem, erreur)](#6-scénario-manuel-b--régression-comme-regressionsh)
7. [Ordre du fil de discussion (N°48, `feed_order_pdf`)](#7-scénario-manuel-c--ordre-du-fil-feed)
8. [Notifications UDP, multicast et erreur TCP 31](#8-notifications-n49)
9. [Paramètres de `feed` : `numb` et `numr`](#9-feed--derniers-billets-depuis-numb--numr)
10. [Cas d’erreur et codes utiles](#10-erreurs-codes--comportements)
11. [Quitter un groupe, admin qui ferme le groupe](#11-quitter-groupe--fermeture)
12. [Optionnel : TLS et authentification forte](#12-tls--auth-optionnel)
13. [Annexes : tableau des CODEREQ / notifs](#13-annexes)
14. [Dépannage étendu](#14-dépannage)
15. [Référence : chaque sous-commande `paroles_client`](#15-référence--chaque-commande-du-client)
16. [Lecture ligne à ligne du script `smoke.sh`](#16-décomposer-smokesh-script-par-script)
17. [Liste de contrôle exhaustive (papier ou copier-coller)](#17-checklist-maison-tout-cocher)
18. [Scénarios d’erreur supplémentaires à provoquer](#18-scénarios-derreur-supplémentaires)
19. [Multiplexage temporel pour les notifications (qui quand où)](#19-chronologie-recommandée-des-notifs)
20. [`stage3_cle_smoke` et inscription avec clé publique](#20-clés-113--stage3)
21. [Comment interpréter `echo $?`](#21-code-de-sortie-shell)
22. [`make test` fichier par fichier (pour débuguer)](#22-lancer-les-scripts-un-par-un)
23. [Parcours « marathon » (blocs K0–K8)](#23-parcours-marathon-blocs-k0k8)
24. [FAQ](#24-questions-fréquentes-faq)
25. [Tableau récap express](#25-tableau-récap--tout-le-cycle-sans-relier-les-20-pages-précédentes)
26. [Alignement avec `checklist_projet_PR6.md` (vérification honnête)](#26-alignement-avec-checklist_projet_pr6md-vérification-honnête)

---

## 1. Prérequis

### But de cette section

**Poser ton environnement** pour que **`make`** et **`make test`** fonctionnent : **`bash`**, **`make` / GCC** (**`build-essential`**), **`libssl-dev`** si le projet lie OpenSSL, **`python3`** pour **`tests/notif_recv.py`**. Sans ces prérequis, tu peux être bloqué avant tout TP réseau ; le bloc **APT** ci-dessous permet de **vérifier** puis **installer** ce qui manque.

| Besoin | Pourquoi | Vérification rapide |
|--------|----------|---------------------|
| **Terminal shell** (`bash`) | Compiler et lancer client/serveur | `echo "$SHELL"` — tu dois pouvoir exécuter les commandes ci-dessous sans erreur |
| **`make`** + **GCC** (`build-essential` sur Debian/Ubuntu) | Compiler les binaires C | `command -v make gcc` && `make --version` && `gcc --version` |
| **Bibliothèques OpenSSL** (`libssl-dev`) | Lien `-lssl -lcrypto` si le `Makefile` lie OpenSSL | `dpkg -s libssl-dev` (voir `Status: install ok`) ou `test -r /usr/include/openssl/ssl.h && echo OK` |
| **`python3`** | Script `tests/notif_recv.py` pour certaines notifs | `command -v python3` && `python3 --version` |

Tu n’as **pas besoin** de Docker ou de VM pour les tests décrits ici si tout compile sur ta machine Linux.

### 1.1 Vérification sous **Kubuntu**, Ubuntu, Debian (APT)

**Kubuntu** utilise les mêmes paquets **`apt`** qu’Ubuntu (`build-essential`, `libssl-dev`, etc.). Tu peux tout vérifier avec le bloc suivant (copier-coller dans un terminal) :

```bash
# Shell (on suppose bash ; zsh/fish conviennent aussi pour ce guide)
echo "Shell: $SHELL"

# make + gcc (Toolchain C)
command -v make gcc >/dev/null && echo "make: OK" && make --version | head -1
command -v gcc >/dev/null && echo "gcc: OK" && gcc --version | head -1

# En-têtes + libs OpenSSL pour le link -lssl -lcrypto (si le projet les exige)
if dpkg -s libssl-dev >/dev/null 2>&1; then
  echo "libssl-dev: installé (dpkg)"
else
  echo "libssl-dev: paquet absent — installe avec: sudo apt install libssl-dev"
fi
test -r /usr/include/openssl/ssl.h && echo "OpenSSL headers (/usr/include/openssl/): OK"

# Python 3
command -v python3 >/dev/null && echo "python3: OK" && python3 --version || echo "python3: manquant"
```

**Interprétation :**

- **`make` / `gcc`** : les deux doivent imprimer une version (GCC GNU, GNU Make). Si `command: not found`, installe **`build-essential`** :
  ```bash
  sudo apt update
  sudo apt install -y build-essential
  ```
- **`libssl-dev`** : si le message indique que le paquet est absent ou si `make` échoue sur des références à `ssl`, `crypto`, `SSL_*` :
  ```bash
  sudo apt install -y libssl-dev
  ```
- **`python3`** : installé par défaut sur beaucoup d’installations ; sinon `sudo apt install -y python3`.

**Installation groupée** (une fois les droits sudo) si plusieurs éléments manquent :

```bash
sudo apt update
sudo apt install -y build-essential libssl-dev python3
```

Puis rejoue le bloc de vérification du haut pour confirmer.

---

## 2. Conventions — rôles des terminaux

### But de cette section

**Structurer tes fenêtres** : **`paroles_server`** doit souvent tourner pendant que **`paroles_client`** envoie des actions ; une commande peut **bloquer** (réception UDP, multicast). Les rôles **T1/T2/T3**, plus **`export H`** et **`export P`** (TCP du serveur), évitent de perdre une invite ou une notif parce que tout est dans une seule invite pleine.

Pour ne pas tout mélanger, on utilise des **noms** :

| Nom | Rôle conseillé |
|-----|----------------|
| **T1** | **Serveur** `paroles_server` — doit rester ouvert ; tu l’arrêtes avec **Ctrl+C** |
| **T2** | Commandes **`paroles_client`** « normales » (reg, invite, post, …) |
| **T3** | **Écoute** des notifications (UDP ou multicast), ou **`python3 tests/notif_recv.py`**, pendant que tu déclenches l’action dans T2 |
| **T4** | (Optionnel) `tail -f`, autre shell, lectures de fichiers `/tmp/` |

Les commandes suivantes utilisent deux **variables shell** pour éviter les fautes de frappe :

```bash
export H="::1"
export P="8899"
```

Ensuite tu peux écrire `./paroles_client "$H" "$P" …` sans retaper l’adresse et le port.  
Change **`8899`** par le port TCP que tu as choisi pour **ton** serveur (tant que personne d’autre ne l’utilise).

**Convention d’écriture dans ce guide :**

- Une ligne commence par **`$`** = à taper dans ton terminal (ne copie pas le `$`).
- Une ligne **`->`** ou bloc **« Attendu »** = ce que le programme affiche (les chiffres exacts peuvent varier selon l’état du serveur).
- **`...`** signifie d’autres lignes omitées.

---

## 3. Préparation unique

### But de cette section

**Une fois pour toutes avant les manip** : se placer dans le dossier **`PRCursor`** puis **`make`** pour produire **`paroles_server`** et **`paroles_client`**. C’est la base commune de tout scénario du guide.

### 3.1 Aller dans le dossier du projet

```bash
cd /home/ajinou/Bureau/Perso/L3/S6/Reseau/PRCursor
```

### 3.2 Compiler

```bash
make
```

Attendu : fin sans erreur, présence des fichiers **`paroles_server`** et **`paroles_client`** :

```bash
ls -la paroles_server paroles_client
```

---

## 4. La batterie automatique `make test`

### But de cette section

**Valider le dépôt automatiquement** (smoke, régression, ordre du fil, notifs, puis selon config TLS / clés / auth) **sans** tout retaper : chaque script utilise en général **un port différent** pour limiter les collisions. Si une étape **échoue**, le message indique **quel script** — utile pour corriger puis relancer.

Tu peux tout rejouer **sans rien faire à la main** avec :

```bash
make test
```

Voici ce que fait **schématiquement** chaque étape du `Makefile` (l’ordre importe peu pour toi tant que tout est vert).

| Étape script | Ce qu’elle vérifie |
|--------------|---------------------|
| `tests/verify_codereq_implemented.sh` | Présence / cohérence des constantes CODEREQ dans le code source |
| `PORT=4242 tests/smoke.sh` | Smoke : 2 inscrits, groupe, invitation, liste invités, acceptation, post, reply, feed minimum |
| `PORT=4245 tests/regression_codereq.sh` | 3 inscrits (deux fois « Alice »), invite multiple, refus (`ans 0`), `listmem 0`, post/reply ordre billet 1, erreur `newgroup` utilisateur inexistant, `ans 2` quitter |
| `PORT=4246 tests/feed_order_pdf.sh` | Ordre précis du fil après plusieurs posts/réponses |
| `PORT=4247 tests/notif_codereq.sh` | Notifs **22** UDP invitation, **18** multicast nouveau message, **31** erreur TCP mauvais premier octet |
| `PORT=4248 tests/tls_smoke.sh` | Inscription avec **TLS** (certificats dans `tests/fixtures/certs/`) |
| `PORT=4249 tests/stage3_cle_smoke.sh` | Réponse inscription avec **clé 113** (Étape 3) |
| `PORT=4251 tests/auth_full_smoke.sh` | TLS + **serveur/clients** avec clés Ed25519 (scénario intégré) |

Si **une** étape échoue, le terminal affiche où ça bloque ; corrige puis relance `make test`.

---

## 5. Scénario manuel A — le parcours « type smoke »

### But de cette section

**Comprendre Paroles « à la main »** sur le chemin **minimal** qui marche : inscriptions, groupe, invitation, **`listinv`**, acceptation **`ans 1`**, **`post`**, **`reply`**, **`feed … 0 0`**. Ça introduit **`id`**, **`idg`**, multicast, et le fait que **`invite`** peut être **muet** au succès (**`echo $?`**).

L’exemple **« Café du coin »** montre un **fil plus riche** : l’ordre des lignes du **`feed`** suit la **timeline globale** côté serveur ; ce n’est pas obligatoirement « lire l’arbre » du haut vers le bas.

Objectif : refaire **`tests/smoke.sh`** ligne par ligne, **à la main**.

### Étape A.1 — Variables et serveur sur T1

Dans **T1** :

```bash
cd /home/ajinou/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="8899"
./paroles_server "$H" "$P"
```

**Attendu (sur stderr) :**

```text
paroles_server ecoute [::1]:8899
```

**Ce qui se passe techniquement :** le programme démarre le serveur Paroles : il ouvre une **socket TCP en IPv6**, se **lie** (*bind*) à l’adresse **`::1`** (boucle locale IPv6, comme `127.0.0.1`) sur le **port TCP** indiqué par **`P`** (ici **8899** — tu peux choisir un autre port libre), puis passe en **`listen`** : le processus **attend** que des clients (`paroles_client`) se connectent sur cette adresse et ce port. Le shell a en réalité exécuté `./paroles_server ::1 8899` car **`"$H"`** et **`"$P"`** sont remplacés avant le lancement.

**Pourquoi T1 ne rend pas le prompt :** tant que le serveur tourne au **premier plan**, le terminal reste « occupé » — c’est **normal**. Ne ferme pas cette fenêtre pendant les tests ; pour arrêter le serveur plus tard : **Ctrl+C** dans **T1**.

Laisse **T1 ouvert**.

### Étape A.2 — Premier utilisateur (inscription)

Dans **T2** :

```bash
cd /home/ajinou/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="8899"
./paroles_client "$H" "$P" reg Alice
```

**Attendu (exemple) :**

```text
OK id=1 udp=20001
```

Note sur un papier **`U1=1`** et **`UDP pour Alice`** (ici `20001`). Ce port UDP dépend du **numéro d’utilisateur** assigné — **ne suppose pas** qu’il sera toujours 20001 après un reboot du serveur ou sur une autre machine.

### Étape A.3 — Second utilisateur

```bash
./paroles_client "$H" "$P" reg Bob
```

**Attendu (exemple) :**

```text
OK id=2 udp=20002
```

Note **`U2=2`** (et UDP associé si tu testes les notifs UDP plus tard).

### Étape A.4 — Création d’un groupe

```bash
./paroles_client "$H" "$P" newgroup 1 MonGroupe
```

Remplace **`1`** par **`U1`** si ton premier id n’est pas 1.

**Attendu (exemple) :**

```text
OK idg=1 mcast=ff0e::1:1 port=30001
```

**Sens de cette ligne :**

| Affichage | Signification |
|-----------|----------------|
| **`OK`** | La demande de création a été acceptée par le serveur. |
| **`idg=1`** | **Identifiant du groupe** (**IDG** dans le sujet) : « numéro » de ce groupe sur le serveur. Le **premier** groupe créé depuis le démarrage du serveur est souvent **`1`**. |
| **`mcast=ff0e::1:1`** | **Adresse IPv6 multicast** attribuée à **ce** groupe pour les notifications de groupe (nouveaux messages, etc.). Les membres s’abonnent à cette adresse pour recevoir ces notifs (**`ff0e::`** fait partie des plages multicast IPv6 du sujet ; la forme exacte peut varier selon l’implémentation). |
| **`port=30001`** | **Port** associé au multicast pour ce groupe (souvent **dérivé de `idg`** côté serveur — ex. une formule « base + modulo »). Ce n’est **pas** le port TCP **`P`** utilisé pour parler au serveur en **`paroles_client`** ; c’est le **port UDP multicast** pour le fil du groupe. |

À noter sur un papier : **`IDG`**, **`mcast`** et **`port`** multicast (le dernier peut varier selon l’`idg` et le code).

### Étape A.5 — Invitation

L’admin est **Alice** (**U1**) ; on invite **Bob** (**U2**) :

```bash
./paroles_client "$H" "$P" invite 1 1 2
```

Ici **`invite <admin_uid> <idg> <uid_invité>`** : adapte **`1`** (admin et idg peuvent être d’autres nombres chez toi).

**Attendu** : aucune erreur grave ; le serveur peut ne rien imprimer de visible côté client si la commande est **muette** après succès — dans ce projet, vérifie le **code de retour** :

```bash
echo $?
```

**`0`** = succès dans le shell Bash.

Si le client imprime quelque chose, c’est ok tant que tu n’as pas de message clair « impossible » ou `perror`.

### Étape A.6 — Liste des invitations en attente pour Bob

```bash
./paroles_client "$H" "$P" listinv 2
```

**Attendu** : lignes avec **`idg=`** et le nom du groupe / admin :

```text
idg=1 groupe=...
```

### Étape A.7 — Acceptation de l’invitation

```bash
./paroles_client "$H" "$P" ans 2 1 1
```

Syntaxe : **`ans <uid> <idg> <0|1|2>`** — ici **`1`** = accepter.

**Attendu** : quelque chose comme **`OK join`** avec les infos groupe (Multicast, membres…) selon l’implémentation client.

### Étape A.8 — Poster un billet (Alice, admin du groupe)

```bash
./paroles_client "$H" "$P" post 1 1 hello
```

**Attendu** :

```text
OK billet numb=0
```

Le **premier** billet d’un groupe a souvent **`numb=0`**.

### Étape A.9 — Répondre au billet 0

```bash
./paroles_client "$H" "$P" reply 2 1 0 reponse
```

**Réponse au billet** numéros : **`reply <uid> <idg> <num_billet> <texte>`**.

**Attendu** :

```text
OK reply
```

(parfois aussi des détails numb/numr selon versions.)

### Étape A.10 — Lire le fil (« derniers billets » depuis ancien curseur)

On demande à partir du billet **0**, réponse **0** :

```bash
./paroles_client "$H" "$P" feed 2 1 0 0
```

**Attendu (smoke minimal)** : lignes **`feed idg=`**, puis des lignes avec **`billet=`** et le texte des messages.

Cherche **`billet=0`** avec « hello », et **`billet=…`** avec « reponse ».

---

#### Exemple étendu — Groupe **« Café du coin »** (idg = 1) avec **Alice**, **Bob** et **Claire**

**Arborescence logique** (un **`post`** crée un **nouveau** `billet` ; un **`reply`** ajoute une **réponse** sous un `billet` existant, avec **`rep`** numéroté à partir de **1** par billet) :

```text
Groupe « Café du coin » (idg = 1)

  billet 0 — Alice (post)
           « Qui est dispo vendredi ? »
              ├─ rep 1 — Bob : « moi »
              ├─ rep 2 — Claire : « moi aussi »
              └─ rep 3 — Alice : « parfait, je réserve une table »

  billet 1 — Bob (post)
           « J’ai trouvé un resto rue X »
              ├─ rep 1 — Claire : « le menu est où ? »
              └─ rep 2 — Bob : « voir lien … »

  billet 2 — Claire (post)
           « Ne pas oublier les parapluies »
              └─ rep 1 — Alice : « +1 »
```

**Prérequis :** serveur **récent** (ou port **`P`** inutilisé) pour que les **uid** soient bien **1 = Alice**, **2 = Bob**, **3 = Claire** et **idg = 1** pour le premier groupe. Dans **T2**, **`export H`** et **`export P`** comme pour le smoke.

**Séquence complète à taper** (bloc unique, dans l’ordre) :

```bash
# Inscriptions + groupe
./paroles_client "$H" "$P" reg Alice
./paroles_client "$H" "$P" reg Bob
./paroles_client "$H" "$P" reg Claire
./paroles_client "$H" "$P" newgroup 1 CafeDuCoin

# Invitations + acceptations (admin Alice = 1, groupe idg = 1)
./paroles_client "$H" "$P" invite 1 1 2
./paroles_client "$H" "$P" invite 1 1 3
./paroles_client "$H" "$P" ans 2 1 1
./paroles_client "$H" "$P" ans 3 1 1

# Billet 0 + réponses
./paroles_client "$H" "$P" post 1 1 'Qui est dispo vendredi ?'
./paroles_client "$H" "$P" reply 2 1 0 'moi'
./paroles_client "$H" "$P" reply 3 1 0 'moi aussi'
./paroles_client "$H" "$P" reply 1 1 0 'parfait, je reserve une table'

# Billet 1 + réponses
./paroles_client "$H" "$P" post 2 1 'J ai trouve un resto rue X'
./paroles_client "$H" "$P" reply 3 1 1 'le menu est ou ?'
./paroles_client "$H" "$P" reply 2 1 1 'voir lien etc'

# Billet 2 + réponse
./paroles_client "$H" "$P" post 3 1 'Ne pas oublier les parapluies'
./paroles_client "$H" "$P" reply 1 1 2 '+1'

# Lecture du fil (Alice ; idem avec uid 2 ou 3 pour Bob / Claire)
./paroles_client "$H" "$P" feed 1 1 0 0
```

**Exemple de sortie observée** — copie d’une session réelle (PRCursor) ; **`nb=`** et l’**ordre des lignes** suivent l’**ordre global du fil** tel qu’implémenté (ce n’est pas forcément l’ordre de lecture de l’arbre ci-dessus). Si la **première** ligne contient encore **`reponse`**, c’est souvent qu’un **test smoke minimal** (`hello` / `reponse`) a été fait **avant** sur le **même** serveur sans le redémarrer — pour n’avoir que « Café du coin », **relance `paroles_server`** puis exécute uniquement le bloc ci-dessus.

```text
feed idg=1 nb=10
  [2 billet=0 rep=1] reponse
  [1 billet=1 rep=0] Qui est dispo vendredi ?
  [2 billet=0 rep=2] moi
  [3 billet=0 rep=3] moi aussi
  [1 billet=0 rep=4] parfait, je reserve une table
  [2 billet=2 rep=0] J ai trouve un resto rue X
  [3 billet=1 rep=1] le menu est ou ?
  [2 billet=1 rep=2] voir lien etc
  [3 billet=3 rep=0] Ne pas oublier les parapluies
  [1 billet=2 rep=1] +1
```

*(Adapte les textes si ton client affiche légèrement autrement ; **`nb=`** et le détail **`[uid billet=… rep=…]`** peuvent varier selon version.)*

### Étape A.11 — Arrêt du serveur

Dans **T1**, **Ctrl+C**. Le terminal redevient libre.

---

## 6. Scénario manuel B — « régression » (comme `regression_codereq.sh`)

### But de cette section

**Aller au-delà du happy path** : deux fois le même pseudo → deux **`uid`** ; plusieurs invités ; **refus** **`ans … 0`** ; **`listmem … 0`** (tous inscrits) vs **`listmem`** d’un groupe (admin souvent en tête selon PDF) ; **`feed`** sur un billet précis ; **`newgroup`** avec utilisateur inexistant (**`exit≠0`**) ; **`ans … 2`** pour quitter (**`OK ack`**). Référence automatique : **`regression_codereq.sh`**.

Ce scénario est **plus long**. Repars d’un serveur frais (**redémarre** `paroles_server` sur un nouveau port **`8901`** par exemple pour ne pas avoir d’état ancien si tu doutais).

### B.1 — Trois inscriptions avec deux fois le même pseudo

```bash
export P="8901"
```

**T1** : `./paroles_server "$H" "$P"`

**T2** :

```bash
./paroles_client "$H" "$P" reg Alice   # note U_a1 et udp
./paroles_client "$H" "$P" reg Alice   # note U_a2 différent !
./paroles_client "$H" "$P" reg Bob     # note U3
```

Vérifie **`U_a1 != U_a2`** : deux pseudo identiques ⇒ **IDs distincts**.

### B.2 — Groupe et invitation de deux personnes

```bash
./paroles_client "$H" "$P" newgroup "$U_a1" GReg
```

(Passe manuellement l’ID numérique à la place de `$U_a1` si tu n’utilises pas de variables.)

```bash
./paroles_client "$H" "$P" invite "$U_a1" "$IDG" "$U_a2" "$U3"
```

### B.3 — `listinv` pour les deux invités

Chaque liste doit contenir **`idg=$IDG`** avec le groupe attendu.

### B.4 — Premier invité refuse

```bash
./paroles_client "$H" "$P" ans "$U_a2" "$IDG" 0
```

ack est l’abréviation courante d’acknowledgement (en français : accusé de réception / accusé).

Dans ce contexte, OK ack signifie que le serveur a bien pris en compte ta réponse à l’invitation (ici le refus avec 0) et renvoie un accusé positif 

### B.5 — Second invité accepte

```bash
./paroles_client "$H" "$P" ans "$U3" "$IDG" 1
```

### B.6 — `listmem` avec idg **0**

```bash
./paroles_client "$H" "$P" listmem "$U_a1" 0
```

Attendu : mention des **« membres »** / liste de **tous** les utilisateurs enregistrés (comportement **liste tous inscrits** avec **IDG=0** dans le protocole local).


### B.7 — `listmem` du groupe précis

```bash
./paroles_client "$H" "$P" listmem "$U_a1" "$IDG"
```

L’**admin** doit être **en premier** dans l’affichage côté sujet PDF.

### B.8 — Posts et reply pour tester le feed vers le deuxième billet

```bash
./paroles_client "$H" "$P" post "$U_a1" "$IDG" "b1"
./paroles_client "$H" "$P" post "$U_a1" "$IDG" "b2"
./paroles_client "$H" "$P" reply "$U3" "$IDG" 1 "r-b2"
./paroles_client "$H" "$P" feed "$U3" "$IDG" 0 0
```

Vérifie que le **`feed`** contient bien **`billet=1`** (deuxième billet) avant la réponse, et le texte **`r-b2`**.

### B.9 — Erreur métier : `newgroup` avec utilisateur inexistant

```bash
./paroles_client "$H" "$P" newgroup 99999 Solo ; echo exit=$?
```

**Attendu** : échec côté client (souvent **code de sortie non nul**) — vérifie avec **`echo $?`** juste après (le `; echo` combiné doit montrer un exit≠0 ou message d’échec).

### B.10 — Quitter le groupe comme membre non-admin

```bash
./paroles_client "$H" "$P" ans "$U3" "$IDG" 2
```

**Attendu** : acquittement (**`OK ack`** selon flux) — Bob quitte sans fermer tout le groupe (l’admin est encore là).

---

## 7. Scénario manuel C — ordre du fil (`feed`)

### But de cette section

**Contrôler contre le PDF** que le **`feed`** renvoie les événements (**posts + réponses**) dans **l’ordre où le serveur les a reçus** ; après la séquence **b0 / b1 / deux replies** et un **`feed … 0 0`**, les trois premiers couples **`billet/rep`** extraits correspondent à **`1 0`** → **`0 1`** → **`1 1`** (détail **`sed`** et serveur **`&`** / **`kill`** plus bas dans ce chapitre). **Référence automatique** : **`tests/feed_order_pdf.sh`** (CHRONO **N°48**).

Référence automatique équivalente (une ligne) :
cd /CHEMIN/VERS/TonDepot/PRCursor && bash tests/feed_order_pdf.sh
```

Remplace **`/CHEMIN/VERS/TonDepot/PRCursor`** par **ton dossier réel** (sur ta machine : souvent quelque chose comme `~/Bureau/Perso/L3/S6/Reseau/PRCursor`). **`/chemin/vers/PRCursor`** dans d’anciens exemples était un **texte générique**, pas une vraie adresse : si tu le recopies tel quel, `cd` affiche une erreur (normal).

---

### 7.1 — Prérequis

- Répertoire compilé : **`make`** dans **`PRCursor`**.
- **Nouveau** serveur : soit arrêter l’ancien (`Ctrl+C` au terminal où il tourne, ou `kill` du PID vu avec `jobs`/`ps`), soit choisir un **autre port** (**`P`**) encore libre.
- Dans **ce** même terminal : **`export H="::1"`** (ou **`127.0.0.1`**) ; **`export P="4246"`** (adaptable).

---

### 7.2 — Lancement serveur (arrière‑plan)

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor   # adapte si ton clone est ailleurs
./paroles_server "$H" "$P" &
sleep 0.35
```

Le **`&`** lance **`paroles_server` en arrière‑plan** : le shell affiche vite une ligne du type **`[1] PID`** puis **reprend la invite de commandes** (« invite »). Ce n’est **pas** une attente : le processus qui écoute sur **`"$H:$P"`** continue en fond. Une ligne **`paroles_server ecoute [::1]:4246`** peut s’afficher **avant** le retour invite — c’est le serveur qui confirme le bind.

**Si **`cd`** échoue** mais **`./paroles_server`** passe quand même : soit tu étais **déjà** dans **`PRCursor`**, soit le binaire a été résolu depuis le dossier courant — mais il vaut mieux toujours **`cd`** vers **le bon** dossier pour éviter lancer un vieux binaire ou se tromper de répertoire.

**Si tu relances un second serveur sur le même **`"$H"`** / **`"$P"`** pendant que le premier tourne en fond** : le **`bind`** échoue, tu vois quelque chose comme **`listen: Address already in use`**, et le deuxième job se termine avec un code ≠ 0 (ex. ligne **`Termine 1`**). Le **premier** serveur (**`jobs`** : encore **En cours d’exécution**) garde toujours le port.

---

### 7.2 bis — Arrêter **`paroles_server`** quand il a été lancé avec **`&`**

**`Ctrl+C`** coupe surtout le **processus au premier plan**. En arrière-plan, utilise :

```bash
jobs -l                       # affiche [n°] et le PID (ex. [1]+ 106273 En cours…)
kill %1                       # tue le job n°1 (adapte le numéro affiché par jobs)
```

Ou avec le **PID** exact (celui affiché après **`[1]`** au lancement, ou la colonne PID de **`jobs -l`**) :

```bash
kill 106273                   # remplace par ton PID
```

**Variante** : remettre le job au premier plan puis **`Ctrl+C`** :

```bash
fg %1                         # le serveur repasse au premier plan
# puis Ctrl+C sur le clavier
```

Si le processus refuse de s’arrêter (rare ici) : **`kill -9 PID`** en dernier recours.

Vérifier qu’aucun programme n’écoute plus sur le port (ex. **`4246`**) :

```bash
ss -ltnp | grep 4246          # ou : ss -ltn 'sport = :4246'
```

---

### 7.3 — Deux utilisateurs **A**, **B** et variables **`U1`**, **`U2`**

Alice = **`U1`**, Bob = **`U2`** (numéros issus du **`OK id=`** après **`reg`**).

```bash
./paroles_client "$H" "$P" reg A | tee /tmp/fo_u1.txt
./paroles_client "$H" "$P" reg B | tee /tmp/fo_u2.txt
U1=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/fo_u1.txt | head -1)
U2=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/fo_u2.txt | head -1)
echo "U1=$U1 U2=$U2"
```

Ou à la main : note **`U1`** / **`U2`** après chaque **`reg`** (souvent **1** et **2** sur une base vide).

---

### 7.4 — Groupe **Gfo**, invitation, **`ans … 1`**

```bash
./paroles_client "$H" "$P" newgroup "$U1" Gfo | tee /tmp/fo_g.txt
IDG=$(sed -n 's/.*idg=\([0-9]*\).*/\1/p' /tmp/fo_g.txt | head -1)
echo "IDG=$IDG"

./paroles_client "$H" "$P" invite "$U1" "$IDG" "$U2"
./paroles_client "$H" "$P" ans "$U2" "$IDG" 1
```

**`ans "$U2" "$IDG" 1`** = Bob **accepte** l’invitation (il doit être membre pour la suite **`post`/`reply`/`feed`** selon les règles du serveur).

---

### 7.5 — Séquence de messages (ordre **strict**, comme le PDF‑type)

| Étape | Rôle dans le protocole | Commande (exemple **`paroles_client`**) |
|-------|-------------------------|-------------------------------------------|
| 1 | Billet **`b0`** (numéro **`0`**) | `post "$U1" "$IDG" b0` |
| 2 | Billet **`b1`** (numéro **`1`**) | `post "$U1" "$IDG" b1` |
| 3 | **1re** réponse sur billet **0** depuis **B** | `reply "$U2" "$IDG" 0 r-to-0` |
| 4 | **1re** réponse sur billet **1** depuis **A** | `reply "$U1" "$IDG" 1 r-to-1` |

En bloc :

```bash
./paroles_client "$H" "$P" post  "$U1" "$IDG" b0
./paroles_client "$H" "$P" post  "$U1" "$IDG" b1
./paroles_client "$H" "$P" reply "$U2" "$IDG" 0 r-to-0
./paroles_client "$H" "$P" reply "$U1" "$IDG" 1 r-to-1
```

---

### 7.6 — **`feed` depuis `(0,0)` et contrôle attendu**

```bash
./paroles_client "$H" "$P" feed "$U2" "$IDG" 0 0
```

Extraire dans l’ordre les couples **`billet` / `rep`** (pour comparer comme le script) :

```bash
./paroles_client "$H" "$P" feed "$U2" "$IDG" 0 0 \
  | sed -n 's/.*billet=\([0-9]*\) rep=\([0-9]*\).*/\1 \2/p'
```

**Structure attendue** pour les **3 premières** lignes ainsi extraites (après l’ancre **`(numb,numr)=(0,0)`**) :

1. **`1 0`** — événement lié au **2ᵉ billet**, **`rep=0`**
2. **`0 1`** — **1ᵉ billet**, **1ᵉ réponse**
3. **`1 1`** — **2ᵉ billet**, **1ᵉ réponse**

Ça reflète **l’ordre global du fil « historique événements »**, pas un simple tri trivialement **par numb** seul.

---

### 7.7 — Récap formes client

- **`post <uid> <idg> <texte>`**
- **`reply <uid> <idg> <numb> <texte>`** — **`numb`** = numéro du **billet** ciblé
- **`feed <uid> <idg> <numb> <numr>`** — ici **`0 0`** = depuis le tout début (logique curseur suivant le cours)

Si tu modifies le code du **`feed`** côté serveur sans avoir ce modèle en tête : **rejoue** **`tests/feed_order_pdf.sh`** — il dit **`feed_order_pdf OK (N°48)`** ou quelle ligne d’ordre ne colle pas.

---

## 8. Notifications (N°49)

### But de cette section

Vérifier **sur le fil réseau** que le serveur envoie bien :

- une **notification UDP** (**code 22**) vers le **port d’inscription** de l’utilisateur invité, au moment d’un **`invite`** ;
- une **notification multicast IPv6** (**code 18**) vers l’adresse / port du groupe, au moment d’un **`post`** (membres déjà dans le groupe) ;
- une **réponse TCP d’erreur** (**premier octet 31**) si le client envoie un **CODEREQ inconnu** (ici un premier octet **`99`**).

C’est le même objectif que **`tests/notif_codereq.sh`**, mais **à la main** avec plusieurs terminaux pour visualiser la **chronologie**.

### Comprendre la notif **22** — pourquoi tout ça ?

**But concret**  
Le sujet impose qu’à un **`invite`**, le serveur prévienne l’invité par une **notification UDP** distincte du simple échange TCP (acquittement). La manip vérifie sur le réseau qu’un **datagramme** arrive bien sur le **port UDP** annoncé à l’**inscription** de l’invité (Bob), avec le **bon code** (**22**) et le **`IDG`** du groupe — d’où une ligne **`22 <IDG>`**.

**Pourquoi le nombre `22` ?**  
Chaque notification a un **code** attribué par le projet (voir **`include/paroles_proto.h`** côté `PAROLES_NOTIF_*`). **`22`** = type « invitation », **`18`** = souvent nouveau message (multicast), etc. Quand tu vois **`22 1`**, ce n’est **pas** une erreur : c’est **code de notif** + **identifiant de groupe** (ex. **`1`** si ton premier **`newgroup`** a créé le groupe **1**).

**À quoi sert `tests/notif_recv.py` ?**  
Un **petit programme de test** (Python) pour ne pas réécrire tout en C : il ouvre une socket **UDP IPv6**, fait **`bind`** sur le port que tu donnes (**`UDP2`** = celui de **`OK id=… udp=…`** pour Bob), **attend** un paquet (jusqu’au **timeout**), lit les **6 premiers octets** (**`uint16` BE** + **`uint32` BE**) et affiche **`CODE IDG`**. Avec **`> /tmp/n22.out`**, tu conserves la ligne pour **`grep`**.

**Pourquoi un troisième terminal (T3) ?**  
La commande **`python3 … udp …`** **bloque** le shell tant qu’elle attend un paquet. Pendant ce temps il faut quand même pouvoir taper **`invite`** ailleurs : **T2** = client + variables ; **T3** = **récepteur** qui attend. Tu peux tout faire dans **un** terminal (**Python** en arrière-plan avec **`&`**, puis **`invite`**, puis **`wait`**) comme **`notif_codereq.sh`** — plusieurs fenêtres rendent la **chronologie** visible en TP.

**Pourquoi « Stop ici », écouter *avant* `invite` ?**  
En **UDP**, l’envoi est **une rafale** : au moment où le serveur traite **`invite`**, il envoie le datagramme **22**. S’il n’y a **pas encore** de socket en écoute sur **`UDP2`**, le paquet est en général **perdu** ; le serveur **ne réessaiera pas**. Il faut donc avoir lancé **`notif_recv.py`** (T3 qui **bloque**) **puis** seulement **`invite`** dans **T2**. Si tu **`invite`** trop tôt, il faudra souvent **`invite`** encore une fois après avoir monté l’écoute — ou **repartir** sur un serveur / scénario propre selon l’état du groupe.

### Format des paquets UDP / multicast (rappel)

Taille **≥ 6 octets** :

- **`uint16`** big-endian = **CODEREQ notification** (ex. **22**, **18**)
- **`uint32`** big-endian = **IDG**

Le script **`tests/notif_recv.py`** **`bind`** un socket UDP (toutes interfaces **`::`**, port choisi), **attend un datagramme**, puis affiche : **`CODE IDG`** (deux entiers séparés par un espace).  
Pour **22**, le serveur utilise l’**adresse IPv6** enregistrée à l’**inscription** TCP de Bob et le **PORTUDP** attribué à Bob — d’où l’obligation d’écouter exactement **`UDP2`** (ex. **`20002`** pour **`id=2`**).

### Piège n° 1 — variables shell **entre terminaux**

**`export U1=…`** dans **T2** **n’existe pas** dans **T3** : chaque fenêtre Konsole = un **`bash`** indépendant. Dans **T3**, il faut **recalculer** **`UDP2`** depuis **`/tmp/n2.txt`** (ou taper **`20002`** à la main en le lisant dans **T2**). Sinon **`$UDP2`** est vide → **`ValueError`** dans Python (**`int('')`**).

### Piège n° 2 — **ordre temps réel**

- Pour **22** : le paquet **part au moment du `invite`**. Si **`invite`** a lieu **avant** que **`notif_recv.py udp`** soit en écoute, le datagramme est **perdu** → **`TimeoutError`**. Toujours **lancer l’écoute (T3)**, puis **`invite` (T2)** pendant que Python **bloque** encore.
- Pour **18** : idem : **`post`** doit partir **pendant** que **`notif_recv.py mcast`** attend sur **`MPORT`** (sinon multicast envoyé sans récepteur ou timeout avant le **`post`**).

---

### 8.1 — Notif **22** (UDP invitation)

**Ce qu’on veut observer** : fichier **`/tmp/n22.out`** avec une ligne **`22 <IDG>`** après un **`invite`** vers Bob, alors qu’un récepteur UDP écoute sur le port **`udp=`** affiché par **`reg`** pour Bob (**`UDP2`**).

**Pourquoi on voit `22 1`** (exemple avec **`IDG=1`**) : **`22`** = notification **invitation UDP** (`PAROLES_NOTIF_*` dans le code) ; **`1`** est **l’identifiant du groupe** (ici premier **`newgroup`** sur serveur vide). Ce n’est **pas** une erreur : c’est **code de notif + IDG**.

#### Procédure **ordinaire** multi-terminaux (T1 serveur → T2 prépare → **T3 écoute → T2 invite**)

**Terminal 1 (T1) — uniquement le serveur**

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="4246"
./paroles_server "$H" "$P"
```

À la première ligne tu dois voir par ex. **`paroles_server ecoute [::1]:4246`**. Laisse ce terminal vivant (**`Ctrl+C`** pour arrêter quand tu as fini les tests).

**Terminal 2 (T2) — inscriptions + groupe, sans `invite` tant que T3 n’écoute pas**

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="4246"

./paroles_client "$H" "$P" reg Alice | tee /tmp/n1.txt
./paroles_client "$H" "$P" reg Bob   | tee /tmp/n2.txt

U1=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/n1.txt | head -1)
U2=$(sed -n 's/.*id=\([0-9]*\).*/\1/p' /tmp/n2.txt | head -1)
UDP2=$(sed -n 's/.*udp=\([0-9]*\).*/\1/p' /tmp/n2.txt | head -1)
echo "U1=$U1 U2=$U2 UDP2=$UDP2"

./paroles_client "$H" "$P" newgroup "$U1" MonGroupe | tee /tmp/ng.txt
IDG=$(sed -n 's/.*idg=\([0-9]*\).*/\1/p' /tmp/ng.txt | head -1)
MCAST=$(sed -n 's/.*mcast=\([^ ]*\).*/\1/p' /tmp/ng.txt | head -1)
MPORT=$(sed -n 's/.*port=\([0-9]*\)$/\1/p' /tmp/ng.txt | head -1)
echo "IDG=$IDG MCAST=$MCAST MPORT=$MPORT"
```

**Stop ici pour le test 22** (voir aussi **§ « Comprendre la notif 22 »** ci-dessus) : ne pas lancer encore **`invite`**, ou tu devras **`invite`** de nouveau après avoir raté une première écoute.

**Terminal 3 (T3) — réception avant `invite`**

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor

UDP2=$(sed -n 's/.*udp=\([0-9]*\).*/\1/p' /tmp/n2.txt | head -1)
echo "UDP2=$UDP2"

python3 tests/notif_recv.py udp "$UDP2" 30.0 > /tmp/n22.out
```

Quand **`python3 …`** **attend** (plus de nouveau prompt jusqu’à timeout ou réception), **immédiatement** dans **T2** :

```bash
./paroles_client "$H" "$P" invite "$U1" "$IDG" "$U2"
```

Puis dans **T3** (après fin de Python ou retour invite) :

```bash
grep '^22 ' /tmp/n22.out
```

**Attendu** : **`22 <IDG>`**, ex. **`22 1`** si **`IDG=1`**.

**Pourquoi `grep` peut colorer le `22` en rouge** : avec **`grep --color=auto`**, la **partie qui matche** le motif est **surlignée** (souvent en rouge). Ce n’est **pas** un code d’erreur du protocole, c’est un **effet visuel du terminal**.

**Équivalent** avec **`paroles_client listen_udp`** :

```bash
./paroles_client "$H" "$P" listen_udp "$UDP2" 5
```

Puis **`invite`** dans **T2** pendant la fenêtre de **5** secondes.

---

### 8.2 — Notif **18** (multicast nouveau message)

**Ce qu’on veut observer** : après que Bob a fait **`ans … 1`**, pendant qu’un **`notif_recv.py mcast`** écoute sur **`MCAST` / `MPORT`** donnés par **`newgroup`**, un **`post`** admin déclenche une ligne **`18 <IDG>`**.

**Flux** : multicast = message envoyé à l’adresse IPv6 **`ff0e::1:<idg>`** (forme projet) — le groupe est « abonné » uniquement après acceptation ; **`post`** pousse alors une notif **18**.

**Important** : même règle de **temps** — lancer **`python3 … mcast …`** (T3 ou T2 bis), puis **pendant le blocage** :

```bash
./paroles_client "$H" "$P" post "$U1" "$IDG" hello-mcast
```

Puis :

```bash
grep '^18 ' /tmp/n18.out
```

Exemple générique (**T3**), en reprenant les valeurs depuis **`/tmp/ng.txt`** :

```bash
MCAST=$(sed -n 's/.*mcast=\([^ ]*\).*/\1/p' /tmp/ng.txt | head -1)
MPORT=$(sed -n 's/.*port=\([0-9]*\)$/\1/p' /tmp/ng.txt | head -1)
python3 tests/notif_recv.py mcast "$MCAST" "$MPORT" 30.0 > /tmp/n18.out
```

(Sur premier groupe souvent **`ff0e::1:1`** et **`30001`** — vérifie toujours **`echo`** des variables.)

---

### 8.3 — Réponse erreur TCP **31** (premier octet invalide)

Le script ouvre une **connexion TCP** vers le serveur, envoie **`99`** comme **premier octet** (CODEREQ inconnu), lit la **première** réponse : le **premier octet** doit être **`31`** (**`PAROLES_CODEREQ_ERR`**).

**Utilise le même port que le serveur** (**`$P`**, ex. **`4246`**) — **pas** un port d’exemple **8899** si ton serveur n’écoute pas dessus.

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
python3 tests/notif_recv.py tcp_bad ::1 4246 > /tmp/n31.out
cat /tmp/n31.out
```

**Sortie attendue** : une ligne **`31`**.

---

### Exemple de session réussie (copie d’écran type)

Les extraits ci-dessous illustrent des **sorties cohérentes** (serveur sur **`4246`**, premier **`reg`** sur base vide → **`id` 1 et 2**, **`IDG=1`**).

**T1 — serveur**

```text
paroles_server ecoute [::1]:4246
```

**T2 — préparation puis (après écoute UDP dans T3) `invite`, puis `ans`, puis `post` pour 18 / test applicatif**

```text
OK id=1 udp=20001
OK id=2 udp=20002
U1=1 U2=2 UDP2=20002
OK idg=1 mcast=ff0e::1:1 port=30001
IDG=1 MCAST=ff0e::1:1 MPORT=30001
… (puis `invite` pendant que T3 reçoit le 22)
OK join
OK billet numb=0
```

**T3 — UDP 22 puis (optionnel) multicast 18**

```text
UDP2=20002
… (python se termine après réception ou timeout)
grep '^22 ' /tmp/n22.out
22 1
```

**T4 (ou T2/T3) — TCP 31**

```text
31
```

---

### Tout-en-un automatisé

Si tu ne veux pas gérer **T1/T2/T3** à la main :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
bash tests/notif_codereq.sh
```

Par défaut le script utilise un **port TCP propre** (souvent **4247** dans le fichier) pour **son** serveur : évite les conflits avec **4246**.

### Lien avec le § 9 — `feed` / `numb` / `numr`

Une fois les notifs comprises, enchaîne avec le **§ 9** : c’est le même **fil de discussion**, mais on détaille comment le **curseur (`NUMB`,`NUMR`)** du message **FEED** découpe les événements renvoyés au client (voir exemples du **`projet_PR6.pdf`**, p.ex. « depuis le billet b₂ » ou « depuis la première réponse… »).

---

## 9. Feed — derniers billets depuis **`numb` / `numr`**

**Où on en est dans le guide :** les sections précédentes préparent et valident le protocole étape par étape. Le tableau ci-dessous résume **leur rôle** ; le § 9 complète le § 7 sur **le même fil**, en détaillant **`numb`** et **`numr`** (pas une nouvelle fonctionnalité séparée).

| Section | Rôle dans le cours |
|--------|---------------------|
| **§ 1–3** | Outils, conventions de terminal, compilation |
| **§ 4** | Vérification globale **`make test`** |
| **§ 5** | Paroles « à deux personnes » (smoke manuel) |
| **§ 6** | Régression / cas limites du PDF |
| **§ 7** | Ordre **chronologique** du fil (**N°48**) |
| **§ 8** | Notifications **UDP / multicast / erreur TCP 31** |

### 9.1 — But de cette section

Tu sais déjà invoquer :

```text
feed <uid> <idg> <numb> <numr>
```

Ici tu apprends **ce que signifient** les deux derniers nombres, **pourquoi** ils existent dans le **PDF**, et comment **rejouer trois `feed`** sur **les mêmes** messages pour voir **`nb`** (nombre d’entrées renvoyées) **diminuer** quand tu **avances le curseur**.

### 9.2 — Ce que dit le **`projet_PR6.pdf`** (idée générale)

Dans les échanges fonctionnels (« Demander la liste des derniers billets… » puis message **CODEREQ 16**) :

1. La requête contient **`NUMB`** et **`NUMR`** : dernier événement de la **position de lecture** côté client (formulation PDF : dernier billet pris en compte + éventuelle **réponse à ce billet**).
2. Le serveur **garde tout le fil** dans **l’ordre où il a reçu** posts et réponses (§ 7).
3. La réponse **FEED_OK 17** contient **`NB`** puis **`NB`** blocs : **`rep=0`** = **corps du billet** ; **`rep≥1`** = **réponse**.

En pratique : **`(NUMB, NUMR)`** sert à **reprendre le fil après** tout ce qui était « déjà lu ».

### 9.3 — Comportement de référence **PRCursor** (**`server.c`**)

À l’instant du **`feed`**, le serveur **cherche** dans **`g->feed`** une entrée dont le couple (**`numb`, `numr`**) correspond **exactement** à **`(NUMB, NUMR)`**.

- Si la paire **n’existe pas** dans le fil → **échec** (réponse erreur ; **`paroles_client`** peut rester **muet** ⇒ **`echo $?`**).
- Si elle existe à l’**index i**, le serveur renvoie **uniquement** les entrées **strictement après** **`i`** (donc **`i+1`** … fin de liste).

Conséquences utiles :

- Une entrée **`billet=0`** **`rep=0`** représente **le corps** du **premier billet** (**numéro 0**) dans la liste globale ; après elle viennent toutes les **réponses** et autres billets **dans l’ordre temps réel** (voir § 7 pour un cas où ce n’est **pas** l’ordre « numéro de billet croissant »).
- **`feed uid idg 0 0`** utilise donc (**0**,**0**) comme **point d’accroche** sur **ce corps du billet 0** ; la réponse **ne réaffiche pas** ce premier événement, elle liste **la suite**.
- **`numr`** n’est pas « le bulletin précédent » : **`NUMR`** est le **numéro de réponse** **sur le billet `NUMB`** (**0** = le corps du billet lui‑même, **1** = première réponse, etc.) dans le projet.

### 9.4 — Une histoire courte pour les tests (sans relire tout le projet)

À partir de **Alice (uid 1)** et **Bob (uid 2)** sur un **nouveau serveur vide**, après **admin + groupe + Bob membre**, on envoie **dans l’ordre** :

1. **Post** A sur le groupe → événement **(billet 0, rep 0)**.  
2. **Reply** de Bob au billet **0** → **(billet 0, rep 1)**.  
3. **Post** C toujours Alice → **(billet 1, rep 0)**.

**File chronologique** : **`(0,0)`**, **`(0,1)`**, **`(1,0)`**.

Donc :

| Commande **`feed`** (exemple **`uid`** = **2**) | Ce que tu dois observer (ordre des grandeurs avec **PRCursor**) |
|--------------------------------------------------|------|
| **`feed … 0 0`** | **Après** **`(0,0)`** → au plus **`nb=2`** : **`(0,1)`** puis **`(1,0)`**. |
| **`feed … 0 1`** | **Après** **`(0,1)`** → au plus **`nb=1`** : **`(1,0)`** seulement. |
| **`feed … 1 0`** | **Après** **`(1,0)`** → **`nb=0`** (vide). |

Tu compares surtout la ligne **`feed idg=… nb=…`** ligne à ligne (**`nb`** change).

*(Si ton serveur n’est pas vide ou les **`id`** diffèrent (**1**/ **2**) / **`idg`≠1**, adapte tous les **`reg`**, **`newgroup`**, puis les **`feed …`** comme au § 5.)*

---

### 9.5 — **Copier-coller** : jeu de données (Terminal **T1** = serveur, **T2** = client)

**T1 —** lance ou garde déjà **`paroles_server`** (porte libre ; exemple **4255** pour limiter collision avec tes vieux **`P`**).

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="4255"
./paroles_server "$H" "$P"
```

**T2 — tout le bloc suivant**, une fois le serveur prêt :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="4255"

./paroles_client "$H" "$P" reg Alice
./paroles_client "$H" "$P" reg Bob

./paroles_client "$H" "$P" newgroup 1 MonEssaiFeedNR
./paroles_client "$H" "$P" invite 1 1 2
./paroles_client "$H" "$P" ans 2 1 1

./paroles_client "$H" "$P" post  1 1 TEXTE_POST_A_BILLET0
./paroles_client "$H" "$P" reply 2 1 0 TEXTE_REPLY_BOB_REP1
./paroles_client "$H" "$P" post  1 1 TEXTE_POST_C_BILLET1
```

Tu dois voir des **`OK billet numb=…`**, **`OK reply`**.

---

### 9.6 — **Copier-coller** : trois **`feed`** à comparer (toujours **T2**)

On lit avec **Bob (uid 2)** sur **`idg=1`** (premier groupe sur serveur frais comme ci‑dessus).

**a) Ancre (**0**,**0**) — après le corps du billet 0**

```bash
echo "===== feed après (0,0) ====="
./paroles_client "$H" "$P" feed 2 1 0 0
echo "exit=$?"
```

**Attends‑toi à** : **`feed idg=1 nb=2`** puis deux lignes avec **`rep=1`** puis **`billet 1`** (textes **Reply** puis **POST_C**) — vérifie **toi‑même** les étiquettes **`billet=`** / **`rep=`**.

**b) Ancre (**0**,**1**) — après la **première réponse** au billet 0**

```bash
echo "===== feed après (0,1) ====="
./paroles_client "$H" "$P" feed 2 1 0 1
echo "exit=$?"
```

**Attends‑toi à** : **`nb=1`** seule ligne de contenu (le **billet 1**, **`rep=0`**) environ.

**c)** ancrage **(1,0)** — après le corps du billet **1** (rien d’après dans ce micro‑scénario).

```bash
echo "===== feed après (1,0) ====="
./paroles_client "$H" "$P" feed 2 1 1 0
echo "exit=$?"
```

**Attends‑toi à** : **`feed idg=1 nb=0`** puis **pas** de ligne **`billet=`** suivante (**fil déjà tout lu** après ce point).

---

### 9.7 — **Copier-coller** : curseur **invalide**

Une paire **absente** du fil doit **échouer** (**`exit=1`** courant ; aucune erreur lisible garantie).

```bash
./paroles_client "$H" "$P" feed 2 1 99 0
echo "exit invalide doit être non-zero : ExitCode=$?"
```

---

### 9.8 — (Optionnel) Extraire vite les couples **`billet` / `rep`** comme au § 7

```bash
./paroles_client "$H" "$P" feed 2 1 0 0 \
  | sed -n 's/.*billet=\([0-9]*\) rep=\([0-9]*\).*/\1 \2/p'
```

À comparer **mentalement** avec **`feed … 0 1`** et **`sed`** identique.

---

### 9.9 — Rappel syntaxe cliente

```text
feed <uid> <idg> <numb> <numr>
```

Pour **« voir tout après le dernier message que tu as déjà assimilé »**, tu réutilises (**`numb`,`numr`**) lus depuis la sortie précédente (ou ton stockage client métier hors TD). **`feed … 0 0`** correspond au curseur attaché au **premier événement** **`(billet 0, rep 0)`** après démarrage **smoke**/tests sur base vide.

Pour avancer : après chaque fenêtre **`feed`** lue jusqu’à un couple **(Nb, Nr)** précis observé dans le **dernier** événement consommé, la **prochaine** requête reprend avec **ce même** **(Nb, Nr)** comme ancre : le serveur **ne renvoie pas** ce marqueur une seconde fois, il liste **après**.

### 9.10 — Si quelque chose cloche

- **Relance** **`feed_order_pdf.sh`** ou **`tests/smoke.sh`** sur **nouveau `PORT`** + **serveur vide** avant de juger (**état ancien** = anciens événements restent dans le fil sur un serveur qui n’a pas redémarré).
- Inspecte **`handle_feed`** / **`feed_index_after`** dans **`PRCursor/src/server.c`** si tu dois **aligner** ton **implémentation** perso avec la **référence**.

---

## 10. Erreurs, codes, comportements

**Où on en est dans le guide :** le protocole distingue des **réponses TCP de succès** (ex. **`REG_OK`** **2**, **`POST_OK`** **13**, **`FEED_OK`** **17**, **`ACK`** **24**) d’une **réponse unique d’erreur** **`CODEREQ ERR`** (**31**) quand le serveur **refuse** la requête. Les **notifications** (**18**, **19**, …) partent souvent **hors flux TCP métier**, en UDP ou multicast (**§ 8**). Ce chapitre regroupe ces codes pour les relier au **guide PDF** (`projet_PR6.pdf`) et à ce que **`paroles_client`** affiche (ou non) sur le terminal.

| Section connexe | Complément rapide |
|-----------------|-------------------|
| **§ 6** | Cas d’échec **régression** (refus invitation, erreurs métier…) |
| **§ 8** | **22**, **18**, **31** vus depuis la couche notifications |
| **§ 21** | **`exit`** shell plus en détail |

### 10.1 — But de cette section

- **Savoir lire** le **premier octet** d’une réponse TCP (succès versus **31** dans ce projet référence).
- **Ne pas croire** qu’un client silencieux = succès automatique : vérifie **`echo $?`**.
- **Savoir où chercher** côté code (**`dispatch`**, **`handle_*`**, erreurs **`send_err`**) pour aligner ton implémentation.

### 10.2 — Ce que dit le **`projet_PR6.pdf`** (erreurs succès vs refus)

Dans une architecture **CODEREQ** / **REPONSE_ASSOCIÉE**, chaque commande cliente a une forme : soit une **réponse positive** décrite par le polycopié (**`REG_OK`**, **`NEW_GROUP_OK`**, …), soit un **refus** modélisé par une **réponse d’erreur** (ici **31**). Les **motifs** de refus (utilisateur inconnu, pas membre, groupe fermé, curseur **`feed`** inconnu, etc.) font partie de la **logique métier** : le PDF fixe le **cadre** ; le squelette **PRCursor** matérialise les cas dans **`server.c`**.

### 10.3 — Tableau des codes les plus utiles à l’oral / au debug

| Code | Nom `paroles_proto.h` | Support | Sens courant dans **PRCursor** |
|------|----------------------|---------|--------------------------------|
| **31** | `PAROLES_CODEREQ_ERR` | TCP réponse | Requête rejetée (données invalides, état incompatible, hors droits…) |
| **22** | `PAROLES_NOTIF_INV_UDP` | UDP perso | Invitation à rejoindre un groupe (**IDG** dans la charge) |
| **18** | `PAROLES_NOTIF_NEW_MSG` | Multicast groupe | Nouveau billet ou réponse sur le groupe |
| **19** | `PAROLES_NOTIF_JOIN` | Multicast | Un membre a rejoint |
| **20** | `PAROLES_NOTIF_LEAVE` | Multicast | Un **membre non admin** quitte (**`ans … 2`**) |
| **21** | `PAROLES_NOTIF_CLOSE` | Multicast | **Groupe fermé** (ex. **`ans … 2`** par **l’admin**) |
| **23** | `PAROLES_NOTIF_FETCH` | Multicast | Rappel / fetch (voir spec et tests notifs) |
| autres | _(nombreux)_ | mixte | Ouverture fichier **`include/paroles_proto.h`** pour la liste exhaustive |

Ce ne sont **pas** tous des erreurs : **22**/**18**/… sont des **évènements**. Seul **31** (dans cette colonne) signale un **refus TCP** métier après une **`CODEREQ`**.

### 10.4 — Côté `paroles_client` : lignes **`OK`**, **`ACK`**, et **`echo $?`**

- Succès ⇒ souvent **`echo $?`** **= 0** et une ligne du type **`OK …`** ou **`OK join`** (selon commande **§ 15**).
- Échec ⇒ **`echo $?`** **≠ 0**. Le référentiel **`paroles_client`** peut alors **ne rien imprimer** sur la sortie standard même si le serveur a répondu **31** : **`$?`** reste ta **boussole** objet.

### 10.5 — **Copier-coller** : provoquer un **31** sans script (serveur vide)

Terminal **T1** (serveur) :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="4260"
./paroles_server "$H" "$P"
```

Terminal **T2** (sans groupe, sans inscription utile : **`post`** alors que personne n’est membre du **`idg` demandé**) :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="4260"

./paroles_client "$H" "$P" reg SeulPosteur
# suppose id=1 affiché par le serveur sur base vide
./paroles_client "$H" "$P" post 1 99 texte_sur_groupe_inexistant
echo "exit post sur groupe absent = $?"
```

**Attendu** (référence **PRCursor**) : **pas** de **`OK billet`** ; **`exit … ≠ 0`**.

**Variante** (curseur **`feed`** absent, comme **§ 9.7**) sur un **groupe valide** après smoke minimal :

```bash
./paroles_client "$H" "$P" reg A
./paroles_client "$H" "$P" newgroup 1 GFeedErr
./paroles_client "$H" "$P" feed 1 1 99 0
echo "exit feed invalide = $?"
```

### 10.6 — Où regarder dans le code référence

- **`PRCursor/src/server.c`** : chaque **`handle_*`** qui retourne **-1** sur condition métier finit en **réponse d’erreur** côté connexion (repère **`send_err`** si présent, ou le chemin commun vers **31**).
- **`PRCursor/include/paroles_proto.h`** : constantes **nommées** à citer en soutenance.

### 10.7 — Si quelque chose cloche

- Rejoue **`tests/regression_codereq.sh`** ou un scénario **§ 6** sur **serveur frais** et **`P`** unique.
- Compare ton **`echo $?`** avec **§ 21** si tu hésites entre **échec réseau** et **échec métier**.

---

## 11. Quitter groupe / fermeture

**Où on en est dans le guide :** les invitations et **`ans 0|1|2`** ont déjà été exercées en **§ 5–6**. Ici on **formalise** la commande **`ans <uid> <idg> 2`** (quitter) et la **différence capitale admin / non-admin**, telle que le **PDF** et **`handle_inv_ans`** dans **`PRCursor`** l’expriment.

### 11.1 — But de cette section

- Comprendre **`INV_ANS`** avec réponse **`2`** : pas seulement « quitter », mais **deux comportements**.
- Pouvoir **démontrer** **LEAVE** (**20**) vs **CLOSE** (**21**) au multicast (voir **§ 8**) si tu en as besoin pour un rapport.

### 11.2 — Ce que dit le **`projet_PR6.pdf`** (idée générale)

Le polycopié distingue le **membre qui quitte** (le groupe peut survivre) et la **fermeture de groupe** (plus d’activité sur cet **IDG**). Dans **PRCursor**, la **même** commande client **`ans … 2`** réalise l’un ou l’autre **selon** que **`uid`** est **admin** du groupe ou **simple membre**.

### 11.3 — Comportement de référence **PRCursor** (**`handle_inv_ans`**)

Synthèse (à croiser avec **`PRCursor/src/server.c`**) :

| Situation | Action serveur | Notif multicast typique |
|-----------|----------------|-------------------------|
| **Membre** (pas admin) envoie **`ans … 2`** | Retrait du membre, groupe **toujours actif** | **`PAROLES_NOTIF_LEAVE` (20)** |
| **Admin** envoie **`ans … 2`** | **`close_group`** : groupe **marqué fermé**, plus de vie métier nette dessus | **`PAROLES_NOTIF_CLOSE` (21)** (pas **`LEAVE`** pour ce cas dans la référence) |

Après fermeture (**21**), les **`post`**, **`invite`**, etc. sur cet **IDG** doivent **échouer** côté TCP (**31**) dans une implémentation conforme aux garde-fous serveur.

> **À ne pas confondre :** dans **§ 11.4** le **`post`** final par l’admin doit **réussir**. Dans **§ 11.5** le **`post`** final doit **échouer**. Si tu observes l’inverse, tu as très probablement exécuté l’autre bloc (ou **`ans … 2`** n’a pas été fait par le bon **`uid`**).

### 11.4 — **Copier-coller** — membre quitte (**LEAVE**), groupe reste vivant

**(Scénario A —)** seul **Bob** quitte ; Alice reste admin dans un groupe **toujours ouvert**.

**T1 — serveur** (**`P="4265"`**) :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="4265"
./paroles_server "$H" "$P"
```

**T2 —** Alice admin (**1**), Bob invité (**2**), groupe **1** :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="4265"

./paroles_client "$H" "$P" reg AliceQ
./paroles_client "$H" "$P" reg BobQ

./paroles_client "$H" "$P" newgroup 1 GLeave
./paroles_client "$H" "$P" invite 1 1 2
./paroles_client "$H" "$P" ans 2 1 1

./paroles_client "$H" "$P" ans 2 1 2
echo "Bob quitte, exit ans2 = $?"

./paroles_client "$H" "$P" post 1 1 Encore_possible_apres_LEAVE
echo "Alice post après départ Bob, exit = $?"
```

**Attends‑toi à** : **`OK ack`** sur le **`ans … 2`** de Bob ; le **`post`** d’Alice **réussi** (**`OK billet`**), car le groupe n’est **pas** fermé.

*(Si tes **`id`** ≠ **1**/ **2** sur une base non vide, reprends les mêmes idées mais en recopiant les **`id=`** depuis les sorties **`reg`**, comme au **§ 5**.)*

### 11.5 — **Copier-coller** — admin ferme (**CLOSE**), **`post`** échoue

**(Scénario B — différent du § 11.4 :)** c’est **Alice (admin)** qui envoie **`ans 1 1 2`** : le groupe se **ferme**. Le dernier **`post`** teste que **CLOSE** a bien été pris en compte.

Redémarre **T1** avec un **serveur vide** (ou change **`P`**) si tu as déjà joué **§ 11.4** sur la même instance : sinon les **`uid`** ne sont peut‑être plus **1**/ **2** et tout le bloc **§ 11.5** serait faux si tu gardes littéralement **`1`** et **`2`** dans les commandes.

**T1 — serveur** (**`P="4266"`**) :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="4266"
./paroles_server "$H" "$P"
```

**T2** :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="4266"

./paroles_client "$H" "$P" reg AliceClose
./paroles_client "$H" "$P" reg BobClose

./paroles_client "$H" "$P" newgroup 1 GClose
./paroles_client "$H" "$P" invite 1 1 2
./paroles_client "$H" "$P" ans 2 1 1

./paroles_client "$H" "$P" ans 1 1 2
echo "admin ans 2 ferme groupe, exit = $?"

./paroles_client "$H" "$P" post 1 1 doit_echouer
echo "post après CLOSE, exit = $?"
```

**Attends‑toi à** : dernier **`post`** ⇒ **pas** **`OK billet`** ; **code shell ≠ 0**.

### 11.6 — (Optionnel) Voir **20** ou **21** comme au § 8

Si tu veux **voir** passer **LEAVE**/**CLOSE**, enchaîne depuis **§ 11.4** ou **§ 11.5** avec **`listen_mcast`** (**T3**) avant le **`ans … 2`**, comme dans le tableau **notifications** (**§ 8**).

---

## 12. TLS / auth optionnel

**Où on en est dans le guide :** jusqu’ici le transport était **TCP clair**. Le cours autorise ou prescrit (**selon ta consigne**) un **tunnel TLS** et, dans les extensions **CHRONO**, une **auth** plus forte (signature **Ed25519**). **PRCursor** fournit **`--tls`**, **`--key`**, **`--server-pub`** et deux scripts : **`tests/tls_smoke.sh`**, **`tests/auth_full_smoke.sh`**.

| Ressource | Rôle |
|-----------|------|
| **`scripts/gencerts.sh`** | Génère **CA + serveur** dans **`tests/fixtures/certs/`** |
| **`scripts/gen_ed25519.sh`** | Clés **PEM Ed25519** pour **`auth_full_smoke`** |
| **`tests/tls_smoke.sh`** | Vérifie **TLS + `reg`** seul |
| **`tests/auth_full_smoke.sh`** | **TLS + AUTH (0)** + **`newgroup`** avec clés |

### 12.1 — But de cette section

- Réussir à **compiler** avec OpenSSL puis à lancer **`paroles_server` / `paroles_client`** en **mode TLS**
- Savoir **régénérer** les artefacts **pem** avant une campagne de tests (**CI** ou première machine).

### 12.2 — Ce que décrit **`projet_PR6.pdf`** (optionnel cours)

Réfère-toi à la partie **« sécurité / TLS / extensions »** de ton polycopié (intitulé exact selon année) : l’objectif métier reste identique (**CODEREQ**) mais la **couche sockets** passe par **`SSL_accept` / `SSL_connect`**. Pour le détail fichier par fichier (**`openssl_wrap.c`**, options CLI), **`Makefile`** et les scripts **`tests/`** sont la vérité opérationnelle du dépôt.

### 12.3 — Certificats : génération une fois (`gencerts.sh`)

À lancer depuis la racine **`PRCursor/`** :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
chmod +x scripts/gencerts.sh
scripts/gencerts.sh "$(pwd)/tests/fixtures/certs"
ls -la tests/fixtures/certs/
```

Les chemins suivants (**`ca.pem`**, **`server.pem`**, **`server.key`**) sont ceux utilisés par les exemples ci-dessous.

### 12.4 — **Copier-coller** — smoke **TLS** manuel (**`P="4270"`**)

**T1 — serveur** :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export CERTDIR="$(pwd)/tests/fixtures/certs"
export H="::1"
export P="4270"
./paroles_server --tls "$CERTDIR/server.pem" "$CERTDIR/server.key" "$H" "$P"
```

**T2 — client** (vérifie que **`REG_OK`** passe sur TLS) :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export CERTDIR="$(pwd)/tests/fixtures/certs"
export H="::1"
export P="4270"

./paroles_client --tls "$CERTDIR/ca.pem" "$H" "$P" reg TlsUserManuel
echo "exit TLS reg = $?"
```

Pour **rejouer** exactement ce que **`make test`** attend sur ce point :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
PORT=4248 bash tests/tls_smoke.sh
```

### 12.5 — **Copier-coller** — smoke **AUTH full** (**Ed25519** + **`newgroup`**)

Ce scénario exige **`gen_ed25519`** pour utilisateur **et** serveur (comme dans le script officiel).

**Automatisé (recommandé pour valider vite)** :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
PORT=4251 bash tests/auth_full_smoke.sh
```

Si tu dois **inspecter** les commandes brutes : ouvre **`tests/auth_full_smoke.sh`** ; le serveur y est lancé avec **`--tls … --signing-key …/srv/key.pem`** et le client avec **`--key`** + **`--server-pub`** en plus du **`ca.pem`**.

### 12.6 — Déroulé type (« étape 3 ») : **`gencerts`** + TLS + **`--signing-key`** + **REG** + **AUTH (0)** + métier (**invitation**, **post**, **feed**) + **31**

C’est **le même genre** que le script **`tests/auth_full_smoke.sh`** (**CHRONO 59–63**), développé **à la main** pour pouvoir tout montrer en soutenance.

> **Nuance TCP** : avec le **`paroles_client` ligne de commande**, **chaque** invocation ouvre une **nouvelle** connexion TLS. La séquence polycopiée « **CODEREQ 0 puis métier sur la même session TCP** » est **réalisée dans une seule exécution** du client : première trame **`AUTH`** (code **0**), puis **`NEW_GROUP`** (etc.) sans refaire TLS — voir **`one_cmd_authed`** dans **`client.c`**. Pour **enchaîner `reg` puis `newgroup`**, tu lances **deux processus client** ; ce qui compte au bandeau oral, c’est **la présence obligatoire d’AUTH** avant toute requête métier lorsque **`--signing-key`** est actif côté serveur.

#### Préparation (**T2**, une fois, depuis **`PRCursor/`**)

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
make -s
chmod +x scripts/gencerts.sh scripts/gen_ed25519.sh

scripts/gencerts.sh "$(pwd)/tests/fixtures/certs"
scripts/gen_ed25519.sh "$(pwd)/tests/fixtures/ed25519_u"    # Alice (utilisateur de démo)
scripts/gen_ed25519.sh "$(pwd)/tests/fixtures/ed25519_srv"  # Clé de signature serveur
scripts/gen_ed25519.sh "$(pwd)/tests/fixtures/ed25519_bob"  # Bob (deuxième membre)

export CERT="$(pwd)/tests/fixtures/certs"
export U="$(pwd)/tests/fixtures/ed25519_u"
export SRV="$(pwd)/tests/fixtures/ed25519_srv"
export BOB="$(pwd)/tests/fixtures/ed25519_bob"
export H="::1"
export P="4288"

rm -f "/tmp/paroles_nonce_${P}_1"
```

*(Le **`rm -f`** évite certains blocages **`AUTH`** décrits dans **`auth_full_smoke.sh`** après un arrêt brutal **Ctrl+C**.)*

#### Serveur (**T1**)

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export CERT="$(pwd)/tests/fixtures/certs"
export SRV="$(pwd)/tests/fixtures/ed25519_srv"
export H="::1"
export P="4288"

./paroles_server -v --tls "$CERT/server.pem" "$CERT/server.key" \
  --signing-key "$SRV/key.pem" "$H" "$P"
```

#### Client (**T2**), ordre conseillé

**1)** Inscriptions **sans** pré‑`**AUTH`** (toujours la **première** trame **REG** sur ces connexions) :

```bash
./paroles_client --tls "$CERT/ca.pem" "$H" "$P" reg Alice "$U/pub.pem"
./paroles_client --tls "$CERT/ca.pem" "$H" "$P" reg Bob   "$BOB/pub.pem"
```

**Attends-toi à** : **`OK id=1 …`** puis **`OK id=2 …`** (sur **serveur vide** comme en smoke clair).

**2)** **`newgroup`** avec **TLS + CODEREQ 0** puis métier (**même invocation** : handshake TLS → **`AUTH`** → **`NEW_GROUP`**) :

```bash
./paroles_client -v --tls "$CERT/ca.pem" --key "$U/key.pem" --server-pub "$SRV/pub.pem" \
  "$H" "$P" newgroup 1 MonGroupe
```

**Attends-toi à** : **`OK idg=1 … mcast=… port=…`**.

**3)** Scénario **invitation → acceptation → post → feed** (chaque ligne refait TLS + **`AUTH`** car **`--key`** est présent) :

```bash
./paroles_client --tls "$CERT/ca.pem" --key "$U/key.pem" --server-pub "$SRV/pub.pem" \
  "$H" "$P" invite 1 1 2

./paroles_client --tls "$CERT/ca.pem" --key "$BOB/key.pem" --server-pub "$SRV/pub.pem" \
  "$H" "$P" ans 2 1 1

./paroles_client --tls "$CERT/ca.pem" --key "$U/key.pem" --server-pub "$SRV/pub.pem" \
  "$H" "$P" post 1 1 hello_tls_auth

./paroles_client --tls "$CERT/ca.pem" --key "$BOB/key.pem" --server-pub "$SRV/pub.pem" \
  "$H" "$P" feed 2 1 0 0
```

**Attends-toi à** : ligne(s) **`billet=`** / texte **`hello_tls_auth`** (et en-tête **`feed … nb=…`** selon ta version cliente).

**4)** Erreur **31** côté métier : **`feed`** avec **ancre absente** (ex. **`99 0`** sur un fil qui ne contient pas ce couple) :

```bash
./paroles_client --tls "$CERT/ca.pem" --key "$BOB/key.pem" --server-pub "$SRV/pub.pem" \
  "$H" "$P" feed 2 1 99 0
echo "exit doit être non‑zero : $?"
```

**Attends-toi à** : pas de **`OK`/lignes `billet=`** lisibles ; **`echo $?`** **≠ 0**.

> **`tcp_bad`** (**§ 23**, serveur TCP **sans** TLS) envoie un octet **`99`** en **clair** : contre un serveur qui n’écoute **que** TLS, ce n’est généralement **pas** la bonne façon de montrer **31** ; préfère un **`feed`** invalide (**ci‑dessus**) ou un serveur **sans** TLS dédié à **`notif_recv.py tcp_bad`**.

### 12.7 — Si quelque chose cloche

- **Handshake TLS échoue** : vérifie que **`CERTDIR`** pointe bien vers **`tests/fixtures/certs`** après **`gencerts.sh`**, et que **`P`** coincide entre **T1** et **T2**.
- **Auth / signature** : régénère les paires **`tests/fixtures/ed25519_*`** via **`scripts/gen_ed25519.sh`** comme dans **`auth_full_smoke.sh`**.

---

## 13. Annexes

**(CODEREQ, scripts, aide-mémoire.)**

**Où on en est dans le guide :** après les scénarios **§ 5–12**, ce chapitre regroupe la **référence compacte** (codes, scripts) et des **commandes copier-coller** pour les retrouver dans le dépôt sans ouvrir dix fichiers.

### 13.1 — But de cette section

- Disposer d’une **liste de codes** alignée sur **`paroles_proto.h`** (même vocabulaire qu’au PDF / oral).
- Savoir **quel script** rejouer pour **quelle** famille de test.
- Pivoter vers **§ 15** (syntaxe cliente) et **§ 26** (checklist polycopié).

### 13.2 — Relation avec **`projet_PR6.pdf`**

Le polycopié formalise les **échanges** ; le fichier **`include/paroles_proto.h`** fixe les **constantes numériques**. Pour un binôme « message ↔ numéro », garde **les deux** sous la main en soutenance.

### 13.3 — CODEREQ TCP (extrait)

Inscription : **`REG` 1**, **`REG_OK` 2** ; groupe : **`NEW_GROUP` 3**, **`NEW_GROUP_OK` 4** ; invitations : **`INVITE` 5**, **`LIST_INV` 6** ; **`INV_ANS` 8** avec réponses **`JOIN_OK` 9** ou **`ACK` 24** ; **`LIST_MEM` 10** ; fil : **`POST` 12**, **`REPLY` 14**, **`FEED` 16** (réponse **`FEED_OK` 17**). Voir aussi **`AUTH` 0** / **`AUTH_OK` 25** si ton sujet couvre l’auth (**§ 12**).

**Copier-coller** — tout lister depuis le dépôt :

```bash
grep -E '^#define PAROLES_CODEREQ_' ~/Bureau/Perso/L3/S6/Reseau/PRCursor/include/paroles_proto.h
```

### 13.4 — Notifications (`PAROLES_NOTIF_*`)

- **18** `NEW_MSG`, **22** invitation UDP, **21** `CLOSE`, **19** / **20** join/leave, **23** `FETCH`.

```bash
grep -E '^#define PAROLES_NOTIF_' ~/Bureau/Perso/L3/S6/Reseau/PRCursor/include/paroles_proto.h
```

### 13.5 — Scripts **`tests/`** utiles

| Script | Rôle |
|--------|------|
| **`tests/smoke.sh`** | Parcours type **reg → groupe → post → feed** |
| **`tests/regression_codereq.sh`** | Erreurs / cas limites |
| **`tests/feed_order_pdf.sh`** | Ordre du fil (**§ 7**) |
| **`tests/notif_codereq.sh`** | **22**, **18**, TCP **31** |
| **`tests/tls_smoke.sh`** | TLS (**§ 12**) |
| **`tests/stage3_cle_smoke.sh`** | Clé **113** (**§ 20**) |
| **`tests/auth_full_smoke.sh`** | TLS + **Ed25519** (**§ 12**) |

### 13.6 — Pour aller plus loin

- Tableau **complet des sous-commandes** **`paroles_client`** : **§ 15**.
- Alignement **checklist PR6** : **§ 26**.

---

## 14. Dépannage

**Où on en est dans le guide :** quand quelque chose ne colle **pas au polycopié / au comportement attendu** alors que **`make test`** est vert : environnement (**`P`**, serveur encore vivant, ordre des notifs), ou détails **IPv6**/multicast local.

### 14.1 — But de cette section

- Débloquer vite (**check-list mentale**) sans relire tout le guide.
- Renvoyer aux **bons chapitres** (**§ 2** terminaux, **§ 19** temporalité UDP, **§ 8** multicast).

### 14.2 — Le serveur ne démarre pas

- Mauvaise adresse liaison : en local cours, **`::1`** (boucle IPv6).
- **`Address already in use`** → autre **`P`** (voir **§ 2**) ou ancien **`paroles_server`** encore actif (vérifie avec `ss -ltnp` puis filtre `paroles`, ou `lsof -i "TCP:$P"`).

**Copier-coller** diagnostic port :

```bash
export P="${P:?définis P avant}"
ss -ltnp "( sport = :$P )" 2>/dev/null || ss -ltn "sport = :$P"
```

### 14.3 — `make test` passe mais le manuel échoue

- **`export H`** / **`export P`** différent entre **T1** et **T2** (chaque shell a son env).
- Serveur **arrêté** entre deux commandes sans que tu t’en rendes compte.
- **`uid` / `idg` / `numb`** issus d’un **ancien** essai : **redémarre** serveur vide pour repartir sur **1**, **2**, **idg=1** comme dans les blocs du guide.

### 14.4 — Notifs UDP vides

- Écoute **après** **`reg`** et sur le **bon** **port UDP** affiché (**`OK id=… udp=…`**).
- **`invite`** **après** **`listen_udp`** / Python avec fenêtre de **timeout** assez longue (**§ 19**).

### 14.5 — Multicast ne reçoit rien

- Recopie **exactement** **`MCAST`** et **`MPORT`** de la ligne **`newgroup`** (ex. **`ff0e::1:1`**, **`30001`**).
- Le membre doit avoir **`ans … 1`** avant de « voir » **`18`** sur un **post** (**§ 8**).

### 14.6 — Copier-coller — reset « propre » avant de rejouer un scénario

```bash
# T1 : Ctrl+C sur le serveur, puis nouveau port pour éviter l’état fantôme
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="8800"
./paroles_server "$H" "$P"
```

Adapte **`8800`** si occupé (**§ 2**).

### 14.7 — Si rien ne va encore

Relance **`make clean && make && make test`**, puis **un seul** script (**§ 22.2**). Inspecte les traces **`/tmp/*.txt`** quand les scripts en créent (**`cat`**, **`grep`**) ou capture la sortie avec **`tee`**.

---

## 15. Référence — chaque commande du client

**Où on en est dans le guide :** tableau **de vérité** pour **`paroles_client`** (**ordre des arguments**, ce qu’il faut voir en **succès**). À utiliser quand tu **oublies** la syntaxe exacte ou quand tu compares ton **client maison** au **PRCursor**.

### 15.1 — But de cette section

- Ne plus **tâtonner** sur l’ordre **`host`**, **`port`**, **`cmd`**.
- Relier chaque commande au **polycopié** (même noms d’actions **REG**, **POST**, …).
- Savoir quand **`echo $?`** vaut **0** sans ligne **`OK`** (**`invite`**, **`ans … 2`**, etc.).

### 15.2 — Forme générale

Après les options facultatives (**`-v`**, **`--tls`**, **`--key`**, **`--server-pub`**) :

**`./paroles_client <host_tcp> <port_tcp> <cmd> ...`**

### 15.3 — Tableau des commandes

| Commande | Arguments | Succès attendu / remarques |
|---------|-----------|-----------------------------|
| **`reg`** | `<nom>` `[chemin_pub.pem]` | Une ligne **`OK id=… udp=…`** (clé PEM option selon compilation). Nom **10 octets** max dans le protocole (padded par le client). |
| **`newgroup`** | `<uid>` `<nom_grp>` | **`OK idg=… mcast=… port=…`**. **`uid`** doit être un utilisateur existant sinon **échec**. |
| **`invite`** | `<admin>` `<idg>` `<uid>` … | Succès si **ADMIN** uniquement peut inviter. **Pas** de ligne `OK` explicite côté client actuel si la réponse est un **ACK silencieux** ; utilise **`echo $?`** → **0** si ACK reçu. |
| **`listinv`** | `<uid>` | En-tête **`invitations N`** puis lignes **`idg=… groupe=… admin=…`**. |
| **`ans`** | `<uid>` `<idg>` `<0\|1\|2>` | **`0`** refus invitation ; **`1`** accepte → souvent **`OK join`** + détails ; **`2`** quitte le groupe (**admin** fermet groupe). |
| **`listmem`** | `<uid>` `<idg>` | **`idg=0`** = tous inscrits ; sinon membres du groupe ; admin en premier côté spec. |
| **`post`** | `<uid>` `<idg>` `<texte>` | **`OK billet numb=…`**. Numérotation **à partir de 0** au sein du groupe. |
| **`reply`** | `<uid>` `<idg>` `<numb>` `<texte>` | Réponse au billet **`numb`**. Réponses internes numérotées **à partir de 1**. |
| **`feed`** | `<uid>` `<idg>` `<numb>` `<numr>` | Entête puis lignes **`[… billet=X rep=Y]`** avec texte pour chaque événement fil. |
| **`listen_udp`** | `<port_udp>` `[sec]` | Affiche lignes **`CODE IDG`** (deux entiers décimaux) lors de réceptions UDP jusqu’à timeout. **`host/tcp`** ligne de cmd ignorés mais requis pour cohérence. |
| **`listen_mcast`** | `<ipv6>` `<port_mcast>` `[sec]` | Comme précédent pour multicast. |

### 15.4 — Serveur (**`paroles_server`**)

```bash
./paroles_server [-v] [::1|<ipv6>] <port_tcp>
# ou TLS :
./paroles_server [--tls server.pem server.key] ...
```

Verbose **`-v`** : traces utiles pendant le dev (**refus métier**, routage TCP).

### 15.5 — Rappels **`projet_PR6.pdf`**

- **Nom d’utilisateur** : jusqu’à **10 octets** utiles (**padding** du côté client référence).
- **Invitations / droits admin** : voir comportement **`invite`** / **`newgroup`** dans le cours — le tableau ci-dessus résume uniquement ce que **sort** le **`paroles_client`** de **PRCursor**.

---

## 16. Décomposer `smoke.sh`, script par script

**Où on en est dans le guide :** **`tests/smoke.sh`** est ce que **`make test`** rejoue en premier ; comprendre **ses douze étapes**, c’est avoir un **parcours TP** lisible même sans lire **`bash`** ligne à ligne.

### 16.1 — But de cette section

- Faire le lien **`smoke.sh` ↔ enchaînement humain** (**T1** / **T2**).
- Savoir **quoi extraire** (**`U1`**, **`IDG`**, …) si tu écris ton propre script.

### 16.2 — Ce que fait le script (équivalence)

Fichier : **`tests/smoke.sh`**.

1. **`./paroles_server ::1 "$PORT" &`** en arrière-plan ; **`PORT`** (défaut **4242** si vide).
2. **Pause ~0,3 s** pour le **bind**.
3. **`trap cleanup EXIT`** : **kill** du serveur en fin de script.
4. **`reg Alice`** → **U1** (ex. **`sed`** sur **`id=`** dans le script).
5. **`reg Bob`** → **U2**.
6. **`newgroup U1 MonGroupe`** → **IDG** (**`sed`** sur **`idg=`**).
7. **`invite U1 IDG U2`**.
8. **`listinv U2`** + **`grep -q idg=`**.
9. **`ans U2 IDG 1`** (accepte).
10. **`post U1 IDG "hello"`**.
11. **`reply U2 IDG 0 "reponse"`** (billet **0**).
12. **`feed U2 IDG 0 0`** + **`grep -q billet=`**.

### 16.3 — **Copier-coller** — rejouer l’esprit **`smoke.sh`** sans **`sed`** (serveur vivant **`P`**)

Une fois **`T1`** lancé et **`make`** fait :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="4242"   # même convention que smoke si tu veux coller aux scripts

./paroles_client "$H" "$P" reg Alice
./paroles_client "$H" "$P" reg Bob
./paroles_client "$H" "$P" newgroup 1 MonGroupe
./paroles_client "$H" "$P" invite 1 1 2
./paroles_client "$H" "$P" listinv 2
./paroles_client "$H" "$P" ans 2 1 1
./paroles_client "$H" "$P" post 1 1 hello
./paroles_client "$H" "$P" reply 2 1 0 reponse
./paroles_client "$H" "$P" feed 2 1 0 0
```

Adapte **`1`**, **`2`**, **`idg`** si ton serveur n’est pas vide (**§ 5**).

### 16.4 — Lance le script officiel tel quel

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
PORT=4242 bash tests/smoke.sh
```

---

## 17. Checklist « maison » (tout cocher)

**Où on en est dans le guide :** **auto-contrôle** avant oral ou rendu : même idée que **`checklist_projet_PR6.md`**, mais **orientée TP terminal** (**§ 26** pour l’alignement détaillé).

### 17.1 — But de cette section

- Ne **rien oublier** que les scripts automatiques auraient vu (et ce que seul ton œil voit encore).
- Imprimer ou cocher ligne par ligne.

### 17.2 — Liste à cocher

Imprime ou coche dans un éditeur :

- [ ] **Compilation** sans erreur.
- [ ] **`make test`** entièrement verte.
- [ ] Serveur **`::1` + port** correct affiché.
- [ ] **Inscription** d’au moins 2 utilisateurs ; **IPs/udp** différents par id.
- [ ] **Même pseudo** deux fois ⇒ **IDs distincts**.
- [ ] Création **groupe** ; relevé **IDG**, **mcast**, **port mcast**.
- [ ] **Invitation** simple + **liste invités**.
- [ ] **Refus** (`ans 0`) puis profil différent où **acceptation** (`ans 1`).
- [ ] **listmem idg 0**.
- [ ] **listmem** groupe précis (**admin premier** dans l’UI texte ou spec).
- [ ] Au moins deux **posts**, une **réponse**, un **feed** avec texte lisible.
- [ ] **`feed`** cas non trivial (ordre N°48) reproduit avec commandes données section 7.
- [ ] **Erreur** `newgroup` utilisateur inexistant.
- [ ] Membre **`ans 2`** (sortie) sans fermer tout le monde.
- [ ] Tentative après **CLOSE** groupe (invité parti / admin fermé selon protocole) — comportement erreur observable.
- [ ] Notif **22** UDP (Python ou **`listen_udp`**).
- [ ] Notif **18** multicast après post (Python ou **`listen_mcast`**).
- [ ] Mauvaise requête TCP → réponse première octet **31**.
- [ ] *(Optionnel)* TLS inscription.
- [ ] *(Optionnel)* Scénario `auth_full` avec PEM fournis fixtures.

### 17.3 — Après avoir tout coché

Compare avec **§ 26.2** pour les **notifs 19–23** que la checklist PDF mentionne parfois séparément ; garde une **copie d’écran** ou un **transcript** **`make test`** vert (**§ 25**).

---

## 18. Scénarios d’erreur supplémentaires

**Où on en est dans le guide :** prolonge **§ 6** et **§ 10** par des **erreurs métier** à provoquer à la main : elles ne sont pas toujours **bavardes** à l’écran.

### 18.1 — But de cette section

- **Cartographier** quelques refus hors **smoke** « tout vert ».
- Coupler **`echo $?`** et éventuellement **`paroles_server -v`**.

### 18.2 — Tableau (**PRCursor** comme référence)

| Tentative | Objectif observé |
|-----------|---------------------|
| `post` depuis un utilisateur **pas dans le groupe** | Pas de **`OK`** ; client **≠ 0**. |
| `reply` avec **numéro de billet inexistant** | Idem erreur métier. |
| `invite` par un utilisateur **non admin** | Rejet TCP (**31**). |
| `newgroup` avec nom **vide** ou contraintes longueur | À tester (`handle_new_group`). |
| **Deux** serveurs sur le **même** port TCP | Le second échoue au **`listen`**. |

### 18.3 — **Copier-coller** — deux refus (après un smoke minimal sur **`P=8801`**)

Prépare **T1** avec **`P=8801`**, puis **T2** (inscriptions + groupe + **`ans 1`** comme **§ 16.3** — adapte si besoin), puis :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="8801"

# uid 3 souvent absent sur base fraîche → post refusé
./paroles_client "$H" "$P" post 3 1 fantome
echo "exit post non-membre = $?"

./paroles_client "$H" "$P" reply 2 1 999 pas_de_billet
echo "exit reply billet absent = $?"
```

Adapte **`idg`** / **`uid`** si tes **`reg`** ne donnent pas **1** / **2**.

### 18.4 — Après chaque essai

```bash
echo $?
```

**0** = succès vu par le client (**§ 21**) ; pour un **refus** attendant **≠ 0**, compare **§ 10.4**.

---

## 19. Chronologie recommandée pour les notifications

**Où on en est dans le guide :** le **timing** (**T3** avant **T2**) explique **une grande partie** des faux « ça ne marche pas » sur **UDP 22** et **multicast 18** (**§ 8**, **`notif_codereq.sh`**).

### 19.1 — But de cette section

- Ordonner **T1 / T2 / T3** pour **ne pas perdre** une notif sans **TCP retry**.

### 19.2 — UDP **22** (invitation)

1. Serveur vivant (**T1**).
2. Inscriptions (**T2**) — noter **`udp=`** du destinataire.
3. **T3** : **`notif_recv.py udp …`** **ou** **`./paroles_client … listen_udp …`** (timeout **assez long**).
4. **T2** : **`invite`** **dans la foulée** après l’écoute.

Si **`invite`** part **avant** l’écoute, le **22** peut être **perdu**.

### 19.3 — Multicast **18**

1. **Membre** (**`ans … 1`** déjà fait).
2. **T3** : **`listen_mcast`** avec **`MCAST`** / **`MPORT`** du **`newgroup`**.
3. **T2** : **`post`**.

### 19.4 — Schéma mental

Trace **T1** (serveur), **T2** (actions), **T3** (écoute) : place **`listen ouvert` → `déclencheur` → `ligne CODE IDG`**.

---

## 20. Clés 113 / stage3

**Où on en est dans le guide :** extension **inscription** avec **clé publique** au format long du sujet (**113** octets utiles côté proto — voir **`PAROLES_CLE_LEN`**). Sans compilation **`PAROLES_ACCEPT_REAL_CLE_113`**, le serveur de référence peut **n’accepter** que des clés **nulles** (**README** / **`Makefile`**).

### 20.1 — But de cette section

- Valider **une fois** le script **`stage3_cle_smoke`** seul après un **`make`** propre.
- Savoir **où** sont les PEM de démo (**`tests/fixtures/`**).

### 20.2 — Lien polycopié / code

Référencée comme **CHRONO « stage3 »** dans le cours : inscription **avec matériel cryptographique** aligné grille **PR6**. Croise **`tests/stage3_cle_smoke.sh`**, **`client.c`** (**`cmd_reg`**), **`server.c`** (**`handle_reg`**).

### 20.3 — **Copier-coller**

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
PORT=4249 make -s tests/stage3_cle_smoke.sh
```

**Variante** (invocation shell directe comme avant) :  
`bash tests/stage3_cle_smoke.sh` depuis **`PRCursor`** avec **`PORT=4249`**.

Réplication **manuelle** : ouvre le script et recopie la ligne **`reg … pub.pem`**.

### 20.4 — Si la clé est rejetée

Vérifie les **flags de compilation** (**`Makefile`**) et les commentaires **`README`** / **`server.c`** autour de **`PAROLES_ACCEPT_REAL_CLE_113`**.

---

## 21. Code de sortie shell

**Où on en est dans le guide :** partout **dans** ce guide on cite **`echo $?`** — ce chapitre clarifie la **couche POSIX** (processus **`paroles_client`**) vs **octet réseau 31**.

### 21.1 — But de cette section

- Ne pas confondre **`exit`** du binaire et **contenu** de la réponse TCP.

### 21.2 — Rappels

| Motif | Commande simple |
|-------|------------------|
| Code retour **dernière** commande | `echo $?` |
| Afficher même si échec | `./paroles_client … \|\| echo "echec code=$?"` |

**`0`** = succès **du client** au sens shell ; **`1`** ou autre = **`main()`** / commande inconnue / **I/O** impossible (détail selon **`client.c`**).

### 21.3 — Lien **§ 10**

Le **31** est **sur le fil TCP** vu par ton client ; **`echo $?`** dit seulement si **`paroles_client`** a terminé « content ». Voir **§ 10.4** (sorties **muettes**).

---

## 22. Lancer les scripts un par un

**Où on en est dans le guide :** quand **`make test`** est **rouge**, tu **découpes** la batterie pour isoler **un** script (**§ 4**).

### 22.1 — But de cette section

- Rejouer **smoke**, **notif**, **TLS**, etc. **indépendamment**.
- Réutiliser les **ports** documentés ici pour éviter les **collisions** locales.

### 22.2 — **Copier-coller** (depuis **`PRCursor/`**)

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
make -s
chmod +x tests/*.sh scripts/*.sh 2>/dev/null || true

PORT=4242 tests/smoke.sh
PORT=4245 tests/regression_codereq.sh
PORT=4246 tests/feed_order_pdf.sh
PORT=4247 tests/notif_codereq.sh
PORT=4248 tests/tls_smoke.sh
PORT=4249 tests/stage3_cle_smoke.sh
PORT=4251 tests/auth_full_smoke.sh
tests/verify_codereq_implemented.sh
```

Chaque ligne peut échouer **seule** → message **localisé**.

### 22.3 — Si un script échoue

Relis la **fin** de sa sortie ; inspecte **`/tmp/*.txt`** éventuels. Voir **§ 14** et **§ 22.4**.

### 22.4 — Mémo — **`notif_codereq.sh`**

- **Invitation** **après** écoute Python **UDP**.
- **`ans 1`** invité puis **nouvelle** écoute **multicast** **avant** **`post`**.
- **`tcp_bad`** : Python envoie **`99`** en premier octet ; lecture **`31`**.

Sans Python : **`listen_udp`** / **`listen_mcast`** (**§ 8**).

### 22.5 — Journal de session type (notes perso)

```text
Date : _______________
Serveur : paroles_server ::1 port TCP _____
PID serveur ou terminal : _____

--- Inscriptions ---
U1 _____ udp _____  pseudo _____
U2 _____ udp _____  pseudo _____
U3 _____ …

--- Groupe ---
IDG _____  mcast _____  mport _____

--- Actions testées ---
[ ] invite / listinv / ans 0 / ans 1 / ans 2
[ ] listmem 0 / listmem idg
[ ] post / reply / feed 0 0 / feed autre ancre
[ ] notif 22 / 18 / 31

Problèmes rencontrés :
_________________________________________________________
```

### 22.6 — Pourquoi **deux** nombres dans **`feed`** ?

Rappel : **`0 0`** comme ancre de départ ; autres **`(numb,numr)`** = reprendre **après** cet événement (**§ 9**, **`handle_feed`**). Compare les sorties **`feed …`** avant/après **`post`**/**`reply`** pour **voir** le curseur bouger.

---

## 23. Parcours « marathon » (blocs K0–K8)

**Où on en est dans le guide :** enchaînement **long** façon **roman** — utile si tu veux **tout** rejouer avec **peu de réflexion** entre les étapes. Tu **notes** **`U1`**, **`IDG`**, **`UDP2`**, **`MCAST`**… après chaque bloc (**§ 5** / **§ 19** pour le sens des notifs).

### 23.1 — But de cette section

- Avoir un **fil conducteur** terminal par terminal (**T1** serveur, **T2** actions, **T3** notifs).
- Remplacer **`8899`** par un port **libre** si besoin.

### 23.2 — Prérequis

**T2** (build une fois dans la fenêtre où tu tapes les **`paroles_client`**) :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
make
export H="::1"
export P="8899"
```

**T1** (serveur) avec le **même** **`P`** (**`export`** obligatoire **dans ce shell aussi**) :

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
export H="::1"
export P="8899"
./paroles_server "$H" "$P"
```

### Bloc K0 — (rappel environnement)

Voir **§ 23.2** : **`export H`**, **`export P`** dans **T1** et **T2** avant la suite.

### Bloc K1 — deux personnes

Sur **T2**, une commande après l’autre :

```bash
./paroles_client "$H" "$P" reg Clara
./paroles_client "$H" "$P" reg David
```

Note **`U1`**, **`U2`**, **`UDP1`**, **`UDP2`** depuis **`OK id=`**.

### Bloc K2 — groupe et invitation double

```bash
./paroles_client "$H" "$P" newgroup 1 FilMarathon
```

Note **`IDG`**, **`MCAST`**, **`MPORT`**. Remplace **`1`** par **`U1`** si nécessaire.

```bash
./paroles_client "$H" "$P" invite 1 1 2
```

(exemple **U1=1**, **IDG=1**, **U2=2** — **adapte**.)

### Bloc K3 — voir l’invitation puis accepter

```bash
./paroles_client "$H" "$P" listinv 2
./paroles_client "$H" "$P" ans 2 1 1
```

### Bloc K4 — parler

```bash
./paroles_client "$H" "$P" post 1 1 premier-msg
./paroles_client "$H" "$P" reply 2 1 0 rep-sur-premier
./paroles_client "$H" "$P" feed 2 1 0 0
```

### Bloc K5 — notif invitation (**22**)

1. Repars d’un **serveur neuf** ou d’un autre **`P`** si tu veux un fil propre. Refais **K1–K2** sans **`ans`** encore.  
2. **T3** (remplace **`20002`** par **`UDP2`** du destinataire) :  
   `./paroles_client "$H" "$P" listen_udp 20002 20`  
3. **T2** :  
   `./paroles_client "$H" "$P" invite 1 1 2`  
4. **T3** doit afficher une ligne du type **`22 …`**.

### Bloc K6 — notif message (**18**, multicast)

Après **`ans 1`** et membre actif :

1. **T3** :  
   `./paroles_client "$H" "$P" listen_mcast ff0e::1:1 30001 20`  
   (remplace par **tes** **`MCAST`** / **`MPORT`**.)  
2. **T2** :  
   `./paroles_client "$H" "$P" post 1 1 declenche-18`  
3. **T3** attend une ligne **`18 …`**.

### Bloc K7 — erreur TCP **31** (rapide)

Pendant **T1** actif :

```bash
python3 tests/notif_recv.py tcp_bad ::1 8899
```

Sortie attendue : **`31`**.

### Bloc K8 — arrêt

**T1** : **Ctrl+C**.

### 23.3 — Si un bloc du marathon (**K0–K8**) bloque

Reviens à **§ 14** (ports, env) et **§ 19** (ordre **listen** avant **invite**/**post**).

---

## 24. Questions fréquentes (**FAQ**)

**Où on en est dans le guide :** réponses courtes — renvois **§ n** pour approfondir.

### 24.1 — But de cette section

- Débloquer en **lecture diagonale** (**§ 10**, **§ 19**, **§ 14**).

### 24.2 — Questions / réponses

**Q. Pourquoi `invite` n’affiche rien quand ça marche ?**  
R. **ACK** binaire côté client — utilise **`echo $?`** (**§ 10.4**, **§ 21**).

**Q. Puis-je lancer le serveur sur `0.0.0.0` ?**  
R. Dépôt orienté **IPv6** ; cours local : **`::1`**.

**Q. `make test` prend combien de temps ?**  
R. Souvent **~10–30 s** (**§ 4**).

**Q. Les tests échouent après mon commit : quoi faire ?**  
R. **`git diff`** / **`git stash`** puis **`make clean && make test`** (**§ 22**).

**Q. Où sont les fichiers `/tmp/` des scripts ?**  
R. Ex. **`/tmp/n22.out`** — **`cat`** après échec (**§ 14**).

**Q. `listen_udp` affiche deux nombres ?**  
R. Code notif + **IDG** (**§ 8**).

**Q. Le sujet « feed » revient à deux endroits dans le guide ?**  
R. **§ 9** = détail **`numb`/`numr`** ; **§ 22.6** = rappel express.

---

## 25. Tableau récap — **tout le cycle sans relire les 20 pages précédentes**

**Où on en est dans le guide :** **mémo une page** (actions, pas théorie).

### 25.1 — But

Synthèse avant oral / rendu — voir **§ 17** pour la checklist longue.

### 25.2 — Tableau

| # | Tu testes quoi ? | Commande / endroit principal |
|---|-------------------|------------------------------|
| 1 | Compilation | `make` |
| 2 | Tout automatique | `make test` |
| 3 | Inscription | `reg` |
| 4 | Groupe | `newgroup` |
| 5 | Inviter | `invite` |
| 6 | Voir invitations | `listinv` |
| 7 | Répondre à l’invite / quitter | `ans` |
| 8 | Membres | `listmem` |
| 9 | Billets | `post` |
| 10 | Réponses | `reply` |
| 11 | Fil | `feed` |
| 12 | Notif perso | `listen_udp` ou Python |
| 13 | Notif groupe | `listen_mcast` ou Python |
| 14 | Erreur proto | `notif_recv.py tcp_bad` |
| 15 | TLS / clés | **§ 12**, **§ 20** |

### 25.3 — **Copier-coller** — tout lancer

```bash
cd ~/Bureau/Perso/L3/S6/Reseau/PRCursor
make -s && make test
echo "exit suite = $?"
```

### 25.4 — Dernier kilomètre

- Transcript **`make test`** vert.
- Note perso **`uid` / `idg` / `P`** pour la démo.
- **Débogage express** : même **`P`** ? **T1** actif ? Ordre **listen → invite** (**§ 19**) ?

### 25.5 — Exemple fictif de transcript

```text
--- Exemple fictif (libellés possibles selon version client) ---
$ ./paroles_server ::1 8899
paroles_server ecoute [::1]:8899

$ ./paroles_client ::1 8899 reg Alice
OK id=1 udp=20001

$ ./paroles_client ::1 8899 reg Bob
OK id=2 udp=20002

$ ./paroles_client ::1 8899 newgroup 1 TestGrp
OK idg=1 mcast=ff0e::1:1 port=30001

# … suite au choix — trace cohérente pour l’oral.
```

---

## 26. Alignement avec `checklist_projet_PR6.md` (**vérification honnête**)

**Où on en est dans le guide :** ce chapitre distingue ce que tu **vérifies au terminal** de ce qui reste dans le **polycopié** / **README** cours (**wire**, rendu hors labo).

### 26.1 — Quatre familles dans le checklist

| Type | Exemple checklist | Dans ce guide ? |
|------|-------------------|-----------------|
| **A** Automatisable | `make test`, scénarios **§ 5–8** | **Oui** |
| **B** Manuel protocole | deux Alice, fermeture admin… | **Oui** (**§ 6–11** ; **§ 26.2** pour les notifs **19–23**) |
| **C** Détail binaire | `wire_*`, grilles d’octets | **Non remplacé** : lire code + polycopié + checklist |
| **D** Admin | mail UFR, GitLab, plagiat | **Non** (hors terminal) |

### 26.2 — `make test` et pas à pas manuels (notifs / cas rares)

**Alignement automatique (**§ 4**)** : **`smoke.sh`**, **`regression_codereq.sh`**, **`feed_order_pdf.sh`**, **`notif_codereq.sh`** (notifs **22**/**18**, TCP **31**), **`verify_codereq_implemented.sh`**, **`tls_smoke.sh`**, **`stage3_cle_smoke.sh`**, **`auth_full_smoke.sh`**. Les mentions **« Vérification (2026-04-27) »** dans **`checklist_projet_PR6.md`** documentent une relecture — **vérifie** qu’elles sont encore d’actualité si le dépôt a bougé depuis.

Ce qui figure **au checklist** mais **pas** tout dans un « un seul script grep » :

- **19 JOIN** : **`listen_mcast`** **avant** **`ans … 1`** pour l’**invité** ; attend **`19`** + **IDG**.
- **20 LEAVE** : membre **non admin**, **`listen_mcast`** ouvert, puis **`ans … 2`** — **§ 11.4**.
- **21 CLOSE** : **admin** **`ans … 2`** puis **`post`** même **IDG** → échec — **§ 11.5**.
- **23 FETCH (UDP)** : **`listen_udp`** sur le port de **l’auteur** du billet ; **autre** membre : **`reply`** sur ce billet (voir **`notif_codereq.sh`** / polycopié).
- **Même nom de groupe deux fois** : deux **`newgroup`** → **deux `idg`** différents sur une instance serveur continue.
- **Invite sans être admin** : **`echo $?` ≠ 0** (**§ 18**).

**Copier-coller** — voir **JOIN (19)** (remplace **`H`/`P`/`ff0e::1:1`/`30001`/`2`/`1`** par **tes** valeurs **`newgroup`/`reg`**) :

**T3** (écoute bloquante ou timeout long) :

```bash
./paroles_client "$H" "$P" listen_mcast ff0e::1:1 30001 35
```

**T2** (pendant que **T3** est encore en écoute) :

```bash
./paroles_client "$H" "$P" ans 2 1 1
```

Tu dois pouvoir lire une ligne avec **`19`** sur **T3**.

### 26.3 — Limites honnêtes

- **Timeout TCP 30 s** (ordre de grandeur) : défini dans le code serveur ; pas d’exercice GUIDE exprès.
- **Paragraphe checklist sur « client sans écoute UDP »** : **`listen_udp`** / **`listen_mcast`** du **`paroles_client`** le rend souvent **obsolète** (**§ 8**).

---

## Conclusion rapide — ordre conseillé pour « tout avoir testé »

1. **`make`** correct.
2. **`make test`** tout vert (**§ 4**, **§ 22–25**).
3. **Scénarios A / B / C** (**§ 5–7**) avec **serveur frais** ou **ports différents**.
4. **§ 8** et **§ 19** — notifications (**Python** ou **`listen_*`**).
5. **§ 9–12** — **`feed`**, erreurs, **CLOSE/LEAVE**, **TLS** si demandé.
6. **§ 13–26.3** — annexes, dépannage, marathon **K0–K8**, **FAQ**, **checklist** polycopié.

Tu peux cocher au crayon ou dans un éditeur (**§ 17**) : inscription, groupe, invitation, **`ans`**, **`listmem`**, **`post`**, **`reply`**, **`feed`**, erreurs, **LEAVE/CLOSE**, notifs **22**/**18**/**31**, **TLS** optionnel — le **§ 26** précise ce qui reste **hors terminal** (type **C**/**D**).

---

*Document mis à jour — sections 13–26.3 structurées comme les § 9–12.*
