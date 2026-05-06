# client.c — carte de lecture

Fichier source : **`src/client.c`** (**648** lignes).  
Chaque **bloc** est un **même cadre ASCII** : d’abord **Lignes** (avec le nombre de lignes entre parenthèses), **Bloc**, **Rôle** ; puis **Explication simple** (**récit** : actions terminal / TP, **pourquoi** ce morceau **à ce moment**) ; si besoin un **sous-tableau à 3 colonnes** ; puis **Cmd**, **Effet**, **Fonct.** — comme `server.md`. **Entre deux blocs** : une ligne `---------------------------------------------------------------------------------`.

**Rôle dans le projet :** **Outil de TP** `./paroles_client` : construction des **CODEREQ** avec **`wire`**, transport **`net`/`tls_io`**, options **signature** facultative (**`auth_ed25519`**), et commandes **`listen_udp` / `listen_mcast`** pour voir les notifs UDP.

```mermaid
flowchart LR
  argv[main parse] --> Cmd[cmd_*]
  Cmd --> Oc[one_cmd]
  Oc --> Tcp[tcp6 + TLS?]
  Tcp --> CW[conn_read/write]
```

---

## Blocs détaillés

Chaque cadre : **Lignes** / **Bloc** / **Rôle**, puis **Explication simple** (**chronologie TP** : **quand**, **depuis quel terminal**, **après quelle action cliente** — peu de jargon si possible) ; si plusieurs fonctions/étapes, un **sous-tableau à 3 colonnes** (**Fonction** | **Ce qu'elle fait** | **Comment**) ; puis **Cmd**, **Effet**, **Fonct.** — lignes × 110 caractères. Entre blocs : tirets.

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 1–20 (20)                                                                                         |
| Bloc   : Includes + globals                                                                                |
| Rôle   : Outils cliente Paroles.                                                                           |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : Tu lances **`paroles_client …`** : cette zone garde ce qui dure **toute la commande** |
|                      — **`-v`**, contexte **TLS** (`g_tls_cli`, `g_io_ssl`), **clés Ed25519** si           |
|                      **`--key`** / **`--server-pub`** (option **signature** du cours). Moment : **dès que  |
|                      le binaire démarre**, **avant** le `switch` des sous-commandes dans `main`.           |
| Cmd : À chaque sous-commande invoquée depuis le shell.                                                     |
| Effet : Variables et contexte partagés (`verbose`, TLS, PEM).                                              |
| Fonct. : Déclarations `static` ; includes **proto, auth, tls, net, wire**.                                 |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 22–54 (33)                                                                                        |
| Bloc   : Helpers                                                                                           |
| Rôle   : Pseudo, SNI, nonce fichier.                                                                       |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **Trois utilitaires** : **`pad_nom`** remplit **`PAROLES_NOM_LEN`**,                  |
|                      **`tls_sni_for_host`** remplace **`::1`** par **`localhost`** (SNI pratique avec cert |
|                      locaux), puis **`nonce_*`** lit/écrit **`/tmp/paroles_nonce_<port>_<uid>`** pour les  |
|                      **numéros AUTH** optionnels. Pense-y quand le cours parle **preuve + anti-rejeu**.    |
| — Sous-tableau : Fonction │ Ce qu'elle fait │ Comment —                                                    |
| Fonction              │Ce qu'elle fait                         │Comment                                    |
| ──────────────────────│────────────────────────────────────────│───────────────────────────────────────────|
| pad_nom               │Pseudo fixe                             │**memset** + **strncpy**.                  |
| tls_sni_for_host      │Nom TLS                                 │::1 → localhost.                           |
| nonce_path + I/O      │Fichier nonce                           │chemin puis `fscanf`/`fprintf`.            |
| Cmd : **`cmd_reg`** (pseudo fixe) puis **`one_cmd_authed`** (nonce).                                       |
| Effet : Pseudo prêt ; SNI cohérent ; compteur rejouable sur disque.                                        |
| Fonct. : `snprintf`, `fopen`, lecture/écriture entier, fermeture fichier.                                  |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 56–104 (49)                                                                                       |
| Bloc   : one_cmd_plain                                                                                     |
| Rôle   : Une requête TCP sans AUTH.                                                                        |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : Scène **`reg`** ou commande **sans `--key`** : **connect** IPv6, **handshake TLS** si |
|                      `--tls`, **`conn_writen`** du buffer, **`conn_read_upto`** pour la réponse (taille    |
|                      libre), fermeture **`SSL_shutdown`/`close`**. `g_io_ssl` rappelle quel canal          |
|                      **`conn_*`** utilise.                                                                 |
| — Sous-tableau : Fonction │ Ce qu'elle fait │ Comment —                                                    |
| Fonction              │Ce qu'elle fait                         │Comment                                    |
| ──────────────────────│────────────────────────────────────────│───────────────────────────────────────────|
| tcp6_connect          │Canal TCP                               │socket + **connect**.                      |
| SSL_connect + SNI     │TLS cliente                             │facultatif.                                |
| conn_writen /         │Échange application                     │timeout **PAROLES_TCP_TIMEOUT_MS**.        |
| conn_read_upto        │                                        │                                           |
| Cmd : **`cmd_reg`** et toutes commandes **sans** PEM privée (hors chemin signé).                           |
| Effet : Retourne la taille de la réponse lue ou **-1**.                                                    |
| Fonct. : **`tcp6_connect`**, branche **OpenSSL**, **`conn_writen` / `conn_read_upto`**.                    |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 106–197 (92)                                                                                      |
| Bloc   : one_cmd_authed                                                                                    |
| Rôle   : TLS + AUTH puis métier.                                                                           |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : Avec **`--key`**, chaque commande (sauf inscription) **ouvre une session** : fabrique |
|                      le paquet **AUTH** (nonce lu sur disque, signature **Ed25519**), lit **AUTH_OK**      |
|                      taille fixe, **vérifie** la signature serveur si **`--server-pub`**, **incrémente** le|
|                      nonce, **puis envoie** le vrai message métier (**`conn_writen`** +                    |
|                      **`conn_read_upto`**). Label **`fail:`** nettoie SSL/socket.                          |
| — Sous-tableau : Fonction │ Ce qu'elle fait │ Comment —                                                    |
| Fonction              │Ce qu'elle fait                         │Comment                                    |
| ──────────────────────│────────────────────────────────────────│───────────────────────────────────────────|
| read_nonce + sign     │Preuve client                           │**wire** + **`paroles_ed25519_sign`**.     |
| conn_readn AUTH_OK    │Réponse fixe                            │**wire_get** successivement.               |
| verify serveur        │Confiance réciproque                    │**`paroles_ed25519_verify`** si clé pub    |
|                       │                                        │prof.                                      |
| write_nonce(+1)       │Anti-rejeu                              │mise à jour fichier **`/tmp`**.            |
| commande suivante     │Métier                                  │**`conn_writen`** puis                     |
|                       │                                        │**`conn_read_upto`**.                      |
| Cmd : Commandes après **`REG`** quand PEM chargée (`one_cmd`).                                             |
| Effet : Même type de retour que **`one_cmd_plain`** ou erreur.                                             |
| Fonct. : Enchaînement **wire + EVP + `goto fail`**.                                                        |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 199–203 (5)                                                                                       |
| Bloc   : one_cmd                                                                                           |
| Rôle   : Dispatcheur.                                                                                      |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **Si PEM privée ⇒ `one_cmd_authed`, sinon `one_cmd_plain`.** Point unique d’entrée    |
|                      réseau pour les **`cmd_*`**.                                                          |
| Cmd : **Toutes les commandes** sauf **`cmd_reg`** qui force le chemin plain.                               |
| Effet : Choix automatique AUTH ou non.                                                                     |
| Fonct. : Test **`g_auth_priv`** puis appel fonction adaptée.                                               |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 205–225 (21)                                                                                      |
| Bloc   : fill_ed25519_cle_from_pem                                                                         |
| Rôle   : PEM public → champ 113.                                                                           |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : Quand **`reg`** prend un fichier **PEM public**, ce helper lit la **clé brute 32      |
|                      octets** et **remplit les 113 positions** cours (**copie puis zéros**).               |
| — Sous-tableau : Fonction │ Ce qu'elle fait │ Comment —                                                    |
| Fonction              │Ce qu'elle fait                         │Comment                                    |
| ──────────────────────│────────────────────────────────────────│───────────────────────────────────────────|
| PEM_read_PUBKEY       │Charge PEM public                       │vérif type Ed25519.                        |
| memcpy + zeros        │Champ inscription                       │**32 premiers octets** + bourrage.         |
| Cmd : Branche **`pub_pem`** de **`cmd_reg`**.                                                              |
| Effet : Buffer **`cle`** prêt pour **`REG`**.                                                              |
| Fonct. : **PEM_read_PUBKEY** + contrôle Ed25519 + **`EVP_PKEY_get_raw_public_key`**.                       |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 227–253 (27)                                                                                      |
| Bloc   : cmd_reg                                                                                           |
| Rôle   : CODEREQ 1.                                                                                        |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **`./paroles_client host port reg Pseudo [pub.pem]`** : construit **`REG`** (CODEREQ +|
|                      nom + cle), passe par **`one_cmd_plain`**, contrôle **`REG_OK`**, affiche **`OK id=…  |
|                      udp=…`**.                                                                             |
| Cmd : Première étape utilisateur cours.                                                                    |
| Effet : Identifiant + port UDP attribués par le serveur.                                                   |
| Fonct. : **`wire_put_*`**, **`one_cmd_plain`**, parse réponse (**`wire_get`**).                            |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 255–278 (24)                                                                                      |
| Bloc   : cmd_newgroup                                                                                      |
| Rôle   : CODEREQ 3.                                                                                        |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **`newgroup uid nom`** après inscription : message **NEW_GROUP** via **`one_cmd`**    |
|                      (AUTH possible), vérif **NEW_GROUP_OK**, affiche **id multicast** avec                |
|                      **`inet_ntop`**.                                                                      |
| Cmd : Création de salon depuis le terminal cliente.                                                        |
| Effet : Numéro de groupe + adresse multicast + port.                                                       |
| Fonct. : Longueur nom **u16** + copie puis décodage réponse.                                               |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 280–295 (16)                                                                                      |
| Bloc   : cmd_invite                                                                                        |
| Rôle   : CODEREQ 5.                                                                                        |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **`invite admin idg uid …`** construit une table d’uids + **pads** cours puis attend  |
|                      un **ACK** simple.                                                                    |
| Cmd : Admin TP qui invite plusieurs comptes.                                                               |
| Effet : Succès si premier octet réponse = **ACK**.                                                         |
| Fonct. : Boucle **`wire_put_u32` + `wire_put_zeros`** pour chaque invité.                                  |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 297–327 (31)                                                                                      |
| Bloc   : cmd_listinv                                                                                       |
| Rôle   : CODEREQ 6.                                                                                        |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **`listinv uid`** : affiche combien d’invitations puis chaque **idg / nom / admin**   |
|                      selon le format serveur.                                                              |
| — Sous-tableau : Fonction │ Ce qu'elle fait │ Comment —                                                    |
| Fonction              │Ce qu'elle fait                         │Comment                                    |
| ──────────────────────│────────────────────────────────────────│───────────────────────────────────────────|
| Requête               │LIST_INV + uid                          │**`one_cmd`**.                             |
| Parse                 │Chaînes variables                       │**`wire_get_u32/u16`** + **memcpy**.       |
| Cmd : Savoir quels salons attendent une réponse.                                                           |
| Effet : Sortie **printf** structurée.                                                                      |
| Fonct. : Parse **`LIST_INV_OK`** : compteur puis boucle champs.                                            |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 329–347 (19)                                                                                      |
| Bloc   : cmd_ans                                                                                           |
| Rôle   : CODEREQ 8.                                                                                        |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **`ans uid idg code`** (accepter / refuser / quit selon le cours) ; affiche **ACK** ou|
|                      **JOIN** suivant premier octet retour.                                                |
| Cmd : Réponse cliente à une invitation.                                                                    |
| Effet : Message humain **`OK ack`** ou **`OK join`**.                                                      |
| Fonct. : Petit buffer fixe envoyé puis **switch** sur code retour.                                         |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 349–375 (27)                                                                                      |
| Bloc   : cmd_listmem                                                                                       |
| Rôle   : CODEREQ 10.                                                                                       |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **`listmem uid idg`** (ou idg global selon valeur) ; décode liste de **(uid,pseudo)**.|
| — Sous-tableau : Fonction │ Ce qu'elle fait │ Comment —                                                    |
| Fonction              │Ce qu'elle fait                         │Comment                                    |
| ──────────────────────│────────────────────────────────────────│───────────────────────────────────────────|
| Requête               │LIST_MEM                                │**wire** fixe.                             |
| Lecture tableau       │Boucles                                 │**noms longueurs fixes**.                  |
| Cmd : Inspecter membres après avoir rejoint.                                                               |
| Effet : **printf** lignes tableau.                                                                         |
| Fonct. : Parse après **`LIST_MEM_OK`** (+ compteurs cours).                                                |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 377–397 (21)                                                                                      |
| Bloc   : cmd_post                                                                                          |
| Rôle   : CODEREQ 12.                                                                                       |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **`post uid idg texte`** envoie longueur + texte puis attend **POST_OK** pour afficher|
|                      le **numéro de billet**.                                                              |
| Cmd : Publication dans un groupe.                                                                          |
| Effet : Confirmation **OK billet numb=…**.                                                                 |
| Fonct. : Tampon **`msg`** + **`memcpy`** texte cours.                                                      |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 399–416 (18)                                                                                      |
| Bloc   : cmd_reply                                                                                         |
| Rôle   : CODEREQ 14.                                                                                       |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **`reply`** référencée par **numb** puis texte comme **POST**.                        |
| Cmd : Ajouter une réponse à un billet existant.                                                            |
| Effet : **OK reply** après **REPLY_OK**.                                                                   |
| Fonct. : **`wire_put`** entiers puis chaîne UTF-8 taille cours.                                            |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 418–453 (36)                                                                                      |
| Bloc   : cmd_feed                                                                                          |
| Rôle   : CODEREQ 16.                                                                                       |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **`feed uid idg numb numr`** récupère le morceau de fil après l’ancre (**numb/numr**) |
|                      et affiche chaque ligne retournée.                                                    |
| Cmd : Rattraper l’historique sans tout retélécharger à la main.                                            |
| Effet : Sortie **printf** par événement.                                                                   |
| Fonct. : Parse **FEED_OK** avec boucle auteurs + textes.                                                   |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 455–504 (50)                                                                                      |
| Bloc   : Notifs UDP                                                                                        |
| Rôle   : listen_udp / listen_mcast.                                                                        |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : Deuxième terminal TP : **`listen_udp portUDP`** (notifs personnelles serveur ↔ client)|
|                      ou **`listen_mcast adresse port`** (radio du salon). **`write_notif_line`** imprime   |
|                      **`code`** + **`idg`** depuis **6 octets big-endian**, comme envoyé par               |
|                      **`server.c`**.                                                                       |
| — Sous-tableau : Fonction │ Ce qu'elle fait │ Comment —                                                    |
| Fonction              │Ce qu'elle fait                         │Comment                                    |
| ──────────────────────│────────────────────────────────────────│───────────────────────────────────────────|
| write_notif_line      │Décodage 6 octets                       │**u16 BE + u32 BE**.                       |
| cmd_listen_udp        │Unicast                                 │**bind_any** + timeouts.                   |
| cmd_listen_mcast      │Multicast                               │**JOIN + recv** jusqu’au temps imparti.    |
| Cmd : Tests UDP / multicast du polycopié.                                                                  |
| Effet : Une ligne **`code idg`** par datagramme reçu.                                                      |
| Fonct. : **`udp6_bind_any` / `udp6_mcast_recv_socket`** + **`udp6_recv`** en boucle temporelle.            |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 506–529 (24)                                                                                      |
| Bloc   : usage | client_tls_atexit                                                                         |
| Rôle   : Aide et nettoyage.                                                                                |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : **Sans bons arguments** : **`usage`** rappelle toutes sous-commandes (y compris       |
|                      **listen_udp/listen_mcast**). **`client_tls_atexit`** fermet **SSL_CTX** et           |
|                      **EVP_PKEY** via **`atexit`** pour sorties propres.                                   |
| — Sous-tableau : Fonction │ Ce qu'elle fait │ Comment —                                                    |
| Fonction              │Ce qu'elle fait                         │Comment                                    |
| ──────────────────────│────────────────────────────────────────│───────────────────────────────────────────|
| usage                 │Synopsis                                │**fprintf** multi-lignes.                  |
| client_tls_atexit     │Libère ressources                       │enregistré une seule fois.                 |
| Cmd : Erreur parsing ou fin de processus.                                                                  |
| Effet : Pas de fuite grossière OpenSSL.                                                                    |
| Fonct. : **fprintf** stderr + **`paroles_tls_ctx_free` / `EVP_PKEY_free`**.                                |
|------------------------------------------------------------------------------------------------------------|
```

---------------------------------------------------------------------------------

```
|------------------------------------------------------------------------------------------------------------|
| Lignes : 531–649 (119)                                                                                     |
| Bloc   : main                                                                                              |
| Rôle   : Analyse argv et dispatch.                                                                         |
|------------------------------------------------------------------------------------------------------------|
| Explication simple : Tu tapes **`paroles_client [-v] [--tls ca] [--key priv] [--server-pub pub] host port  |
|                      cmd args…`** : **`main`** lit les options, construit le contexte TLS/Ed25519, puis    |
|                      compare **`cmd`** avec **`reg`**, **`newgroup`**, etc. **Erreurs** ⇒ **usage** + code |
|                      1.                                                                                    |
| — Sous-tableau : Fonction │ Ce qu'elle fait │ Comment —                                                    |
| Fonction              │Ce qu'elle fait                         │Comment                                    |
| ──────────────────────│────────────────────────────────────────│───────────────────────────────────────────|
| Parse options         │TLS / clés                              │avance l’index **`i`** dans **`argv`**.    |
| Dispatch `cmd`        │Sous-commandes                          │appels **`cmd_*`**.                        |
| invite                │Liste variable                          │**malloc** + boucle **`atoi`**.            |
| Cmd : Point d’entrée binaire réel.                                                                         |
| Effet : Code retour **0** succès, **1** échec.                                                             |
| Fonct. : Chaîne de **`strcmp`** + conversions **`atoi`** ; **`malloc`** pour liste d’uids d’**invite**.    |
|------------------------------------------------------------------------------------------------------------|
```


---

## Régénérer ces cadres

```bash
cd "$(git rev-parse --show-toplevel 2>/dev/null)/PRCursor/src md"
python3 _gen_src_md.py
```

Voir aussi **`server.md`** (même style, script **`_gen_server_md_blocks.py`**).
