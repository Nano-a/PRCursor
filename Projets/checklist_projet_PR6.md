# Checklist — Projet PR6 « Paroles » (projet_PR6.pdf)

Source : L3 Informatique 2025-2026, PR6 Programmation réseaux.

---

## Contexte et objectif

- [ ] Projet : implémenter un **serveur** et un **client** **IPv6** pour le protocole **Paroles** (client/serveur, fils de discussion par groupes).

---

## Fonctionnalités attendues (protocole — vue fonctionnelle)

- [x] **Inscription** au serveur (première connexion).
- [x] **Créer** un nouveau groupe ; l’utilisateur devient **admin** du groupe.
- [x] **Inviter** d’autres utilisateurs à rejoindre un groupe (admin).
- [x] **Recevoir** des invitations pour s’abonner à des groupes.
- [x] **Quitter** un groupe.
- [x] **Poster** un billet sur le fil d’un groupe dont on est membre.
- [x] **Répondre** à un billet sur le fil d’un groupe dont on est membre.
- [x] **Demander** la liste des **derniers billets non lus** (depuis un numéro de billet `n` et éventuellement une réponse `r`).
- [x] **Recevoir** les **notifications** du serveur (multicast groupe + UDP personnel).

**Vérification (dépôt `PRCursor/`, 2026-04-27)** : `make` OK ; `tests/smoke.sh` (inscription → groupe → invite → `listinv` → acceptation → post → reply → `feed 0 0`) ; `tests/regression_codereq.sh` (dont refus invitation `ans 0`, `listmem`, ordre fil, `ans 2` quitter) ; `tests/notif_codereq.sh` (échantillon notifs **22** UDP, **18** multicast, erreur **31** TCP). Client/serveur en **TCP clair** IPv6 `::1` (étape 1 sans `--tls` dans ces scripts).

---

## Règles métier importantes

- [x] Deux utilisateurs peuvent avoir le **même pseudo** mais des **identifiants distincts**.
- [x] Deux groupes peuvent avoir le **même nom** mais des **identifiants distincts**.
- [x] Identifiant de groupe : **strictement positif** lors de la création.
- [x] Admin qui **quitte** le groupe → le serveur **ferme définitivement** le groupe (il disparaît).

**Vérification (2026-04-27)** : `regression_codereq.sh` confirme **deux « Alice »** avec **ID distincts** (`U1 != U2`). Code : `next_uid` / `next_gid` initialisés à **1** (`server.c`) ; `handle_new_group` assigne `g->idg = next_gid++` → **idg ≥ 1**. **Même nom de groupe** : test manuel sur `::1:47250` — deux `newgroup` « DupName » → **idg=1** et **idg=2**. **Admin quitte** : `handle_inv_ans` avec `an==2` et `uid == admin_id` appelle `close_group` (`closed=1`, notif CLOSE) ; `find_group` ignore les groupes fermés — **post** sur l’ancien **idg** échoue (**exit 1** côté client). **Membre non-admin** qui quitte (`ans 2`) : retrait + notif LEAVE, **sans** fermeture totale (lignes 390–393 `server.c`).

---

## Échanges TCP — inscription (sans détail binaire ici : voir section messages)

- [x] Client : connexion TCP, demande d’inscription avec **pseudo** + **clé publique** (sécurité).
- [x] Serveur : attribue **ID unique**, **port UDP** notifications, envoie **clé publique serveur** ; enregistre ID, pseudo, clé, **adresse IP client**, port UDP.
- [x] Client récupère les infos ; **connexion TCP terminée** après l’échange.

**Vérification (2026-04-27)** : **Client** — `cmd_reg` construit `PAROLES_CODEREQ_REG` + nom **10 oct.** (`pad_nom`) + **113 oct.** (clé dérivée d’un PEM optionnel sinon zéros) ; `one_cmd_plain` fait **TCP** (IPv6), envoi puis lecture réponse puis **fermeture**. **Serveur** — `serve_client` lit l’unique requête `REG`, appelle `handle_reg` : `next_uid`, `memcpy` nom/clé utilisateur dans `users[]`, **`udp_port` déterministe** depuis l’ID, **`u->reg_addr`** = `@IP` cliente **+ sin6_port** = port UDP attribué ; réponse **REG_OK** = **CODEREQ** + **uint32 BE id** + **uint16 BE port UDP** + **113 oct.** (clé ou zéros selon build, aligné § « Détail », réponse inscription). **`conn_tcp` fermée** après cet échange (branche `REG` puis `goto end` / `SSL_shutdown`, `close`). Couvert par `tests/smoke.sh` / inscription initiale dans `tests/regression_codereq.sh`.

---

## Échanges TCP — création de groupe

- [x] Client : TCP, demande création avec **nom du groupe**.
- [x] Serveur : **IDG** unique, **adresse multidiffusion** unique ; client devient admin.
- [x] Client récupère IDG + adresse multicast ; fin de connexion.

**Vérification (2026-04-27)** : **Client** — `cmd_newgroup` envoie `PAROLES_CODEREQ_NEW_GROUP` + **`uint32`** ID utilisateur + **`uint16` BE longueur** + **octets du nom** (`one_cmd`, donc même cycle TCP que les autres CODEREQ après auth serveur facultative selon `--signing-key`). **Serveur** — `handle_new_group` : `find_user(uid)`, contrôle **`len`** du nom puis **`next_gid++`** pour **IDG** ; adresse **multicast** IPv6 unique par groupe `ff0e::1:<idg>` (`inet_pton`) + **port** `30000 + (idg % 30000)` ; **`g->admin_id = uid`** et **`group_add_member(g, uid)`** ; réponse **NEW_GROUP_OK** : codereq + **idg BE** + **port mcast BE** + **16 oct.** adresse. **Client** — affiche `idg`, IP, port ; **fermeture** dans `one_cmd` après lecture (comme les autres commandes). Couvert par `tests/smoke.sh` (création de groupe après inscription).

---

## Échanges TCP — invitations (admin → autres)

- [x] Client : TCP, **IDG** + liste des **ID** des invités.
- [x] Serveur : **acquittement** ; récupère le message ; fin connexion.

**Vérification (2026-04-27)** : **Client** — `cmd_invite` envoie `PAROLES_CODEREQ_INVITE` + **ID admin** (uint32 BE) + **IDG** (uint32 BE) + **nombre d’invités** (uint32 BE) + pour chaque invité **ID** (uint32 BE) + **4 oct.** de **remplissage** (`PAROLES_INV_PAD`, aligné PDF/étape 2) ; `one_cmd` : une connexion TCP, lecture de la réponse (**1er oct.** = ACK attendu). **Serveur** — `serve_business_switch` lit l’entête INVITE puis le corps (`buf` 12 + `nb × 8` oct.) ; **`handle_invite`** : `find_group(idg)` ; **admin uniquement** (`g->admin_id == uid`) ; pour chaque ID, `find_user`, `group_add_pending`, **notif UDP 22** (`PAROLES_NOTIF_INV_UDP`) à l’invité ; **`send_ack`** (codereq **ACK** + bourrage `PAROLES_ERR_TAIL`). **Fin** : `serve_client` ferme après `dispatch`/`one_cmd`. Couvert par `tests/smoke.sh` et scénarios `tests/regression_codereq.sh` (invite / refus).

---

## Échanges TCP — invitations en attente

- [x] Client : TCP, demande des invitations en attente.
- [x] Serveur : pour chaque invitation : **ID + nom du groupe**, **nom de l’admin** du groupe.
- [x] Client récupère ; fin connexion.

**Vérification (2026-04-27)** : **Client** — `cmd_listinv` envoie `PAROLES_CODEREQ_LIST_INV` + **`uint32`** identifiant de l’utilisateur (`wire_put_u32_be`) ; **`one_cmd`**. **Serveur** — `serve_business_switch` lit **4 oct.** ; **`handle_list_inv`** : pour chaque groupe où l’utilisateur est **en attente** (`group_is_pending`, groupe non fermé), écrit **LIST_INV_OK** : **`nb`** (uint32 BE) puis entrées (**ID du groupe `idg`** = identifiant **ID** de la partie « groupe » dans le checklist), **`uint16` BE LEN** + **NOMG**, puis **NOM admin** sur **10 oct.** (`PAROLES_NOM_LEN`, aligné § « Détail », invitation en attente). **Client** — parse et affiche `idg`, nom de groupe, nom admin ; **fermeture** TCP comme d’habitude. Couvert par `tests/smoke.sh` (`listinv`) et **`regression_codereq.sh`** (invitations multiples).

---

## Échanges TCP — réponse invitation / quitter groupe

- [x] Client : TCP, **IDG** + **accepter / refuser** invitation **ou** demande de **quitter**.
- [x] Si **acceptation** : serveur ajoute au groupe ; réponse avec **IDG, nom admin, IP multicast, port multicast, liste (ID + nom) des membres** (premier = admin).
- [x] Si **quitter** : serveur retire le membre ; si admin quitte → **fermeture définitive** du groupe.
- [x] **Acquittement** si refus d’invitation ou sortie de groupe (hors détail message d’erreur).
- [x] Client récupère ; fin connexion.

**Vérification (2026-04-27)** : **Client** — `cmd_ans` envoie `PAROLES_CODEREQ_INV_ANS` + **ID** utilisateur + **IDG** + **AN** (`0` refus, `1` acceptation, `2` quitter) ; `one_cmd` puis reconnaissance **ACK** ou **JOIN_OK**. **Serveur** — `handle_inv_ans` : **`an==2`** et membre (`group_is_member`) : si **admin** (`g->admin_id == uid`) → **`close_group`** + **`send_ack`** ; sinon **retrait** membre + **ACK** + notif multicast LEAVE. **`an==0`** (refus) : invité en attente uniquement → **retrait pending** + **ACK**. **`an==1`** : **ajout membre** (`group_add_member`), réponse **JOIN_OK** : **IDG**, **port multicast**, **16 oct.** adresse IPv6, **NB** membres, puis pour chaque **`uint32` ID** + **nom 10 oct.** — l’**admin** est celui de **`g->mem[0]`** (création du groupe puis append) : le **nom admin** du sujet correspond au **premier** couple id/nom (aligné § Détail, pas de champ « nom admin » séparé). **Ack** pour refus ou sortie (**24** avec bourrage côté **`send_ack`**). **Connexion** fermée après cette réponse comme les autres commandes. Couvert par **`tests/smoke.sh`** (accept / join) et **`tests/regression_codereq.sh`** (`ans 0` refus, `ans 2` membre / admin).

---

## Échanges TCP — liste membres / tous inscrits

- [x] Client : TCP, **IDG** du groupe **ou IDG = 0** pour **tous les inscrits**.
- [x] Serveur : **nombre** de membres (ou d’inscrits), puis pour chacun **ID + nom** ; pour un groupe, **premier = admin**.
- [x] Client récupère ; fin connexion.

**Vérification (2026-04-27)** : **Client** — `cmd_listmem` envoie `PAROLES_CODEREQ_LIST_MEM` + **ID** utilisateur + **IDG** (`uint32` BE ; **`0`** = demande tous inscrits) ; **`one_cmd`** ; parse **LIST_MEM_OK** avec **IDG rapporté**, **NB**, puis **NB × (uint32 id + nom 10 oct.)**. **Serveur** — **`handle_list_mem`** : si **`idg == 0`**, liste tous les utilisateurs **`users[i].used`** (pas de contrôle « membre d’un groupe » sur cette branche ; **nombre** puis paires ID/nom) ; si **`idg != 0`**, groupe existant **`find_group`** et appelant **membre du groupe** (`group_is_member`) sinon erreur ; réponse **`idg`**, **`g->nmem`**, puis ordre **`g->mem[]`** — **premier = admin** (admin ajouté en premier à la création puis append). **`connexion` fermée après la réponse** comme d’habitude. **`tests/regression_codereq.sh`** : **`listmem` idg=`0`** (tous inscrits) et **`listmem` groupe**.

---

## Échanges TCP — poster un billet

- [x] Client : TCP, billet + **IDG** cible.
- [x] Serveur : ajoute au fil ; réponse avec **numéro du billet** + **IDG**.
- [x] Numérotation des billets : **incrémentale à partir de 0** par groupe.
- [x] Fin connexion.

**Vérification (2026-04-27)** : **Client** — `cmd_post` envoie `PAROLES_CODEREQ_POST` + **ID**, **IDG**, **`uint16` BE longueur** + **payload** octets du billet (`one_cmd`). **Serveur** — **`handle_post`** : **membre du groupe** ; billet **`po->numb = g->next_billet++`** (à la création **`next_billet`** vaut **`0`** car **Group** **`memset`**) → **premier billet NUMB** = **`0`** ; enregistrement dans **`g->posts`**, **`feed_push`** pour l’historique « ordre de réception » global ; **notification multicast NEW_MSG**. Réponse **`POST_OK`** : **IDG**, **NUMB** (ordre champs **`idg` puis `numb`** dans le fichier). **`connexion` fermée** après réponse (**`serve_client`** / **`one_cmd`**). **`tests/smoke.sh`**, **`tests/regression_codereq.sh`** (posts `b1`/`b2`), **`feed_order_pdf.sh`**, **`notif_codereq.sh`**.

---

## Échanges TCP — répondre à un billet

- [x] Client : TCP, **ID utilisateur**, **NUMB**, **IDG**, contenu réponse.
- [x] Serveur : ajoute réponse ; réponse avec **NUMB**, **NUMR** (numéro réponse), **IDG**.
- [x] Réponses à un billet : numérotées **à partir de 1**.
- [x] Fin connexion.

**Vérification (2026-04-27)** : **Ordre wire (aligné § « Détail »)** : **`cmd_reply`** envoie `PAROLES_CODEREQ_REPLY` puis **`uint32` BE** **ID**, **IDG**, **NUMB**, **`uint16` BE LEN**, **DATA** (la liste fonctionnelle mélange l’énumération mais **PAS** `NUMB` avant `IDG` dans le fichier). **Serveur** — **`handle_reply`** : **membre du groupe** ; **billet présent** (`find_post`) ; nouveau **`numr`** : **`po->next_reply++`** puis **`numr = po->next_reply`** — nouveau **Post** (**`handle_post`**) a **`next_reply == 0`**, donc **1ʳᵉ réponse → NUMR = 1** ; **`feed_push`** ; **`REPLY_OK`** : **IDG**, **NUMB**, **NUMR** ; notif **multicast** ; **UDP 23** à l’**auteur du billet** si répondant ≠ auteur. **`connexion` fermée** après **`one_cmd`**. **`tests/smoke.sh`** / **`regression_codereq.sh`** (reply), **`notif_codereq.sh`**.

---

## Échanges TCP — derniers billets depuis (n, r)

- [x] Client : TCP, demande depuis billet **n** et réponse **r**.
- [x] Serveur : envoie **m** = nombre de billets, puis les **m** entrées dans l’**ordre de réception** global (billets et réponses mélangés comme dans l’exemple du PDF).
- [x] Respecter l’exemple : depuis b2 → réponse b2, b3, réponse b1 ; depuis 1ère réponse à b2 → b3, réponse b1.
- [x] Fin connexion.

**Vérification (2026-04-27)** : **Client** — `cmd_feed` : `PAROLES_CODEREQ_FEED` + **ID**, **IDG**, **NUMB**, **NUMR** (`uint32` BE pour chaque) ; **`one_cmd`** ; parse **`FEED_OK`** : **IDG**, **`nb`** = **m**, puis **`m`** × (**author**, **numb**, **numr**, **len**, **data**). **Serveur** — **`handle_feed`** : **membre du groupe** ; **`feed_index_after(g, numb, numr)`** repère dans **`g->feed`** l’entrée d’index **`start`** où **`(NUMB, NUMR)`** coïncide avec celui demandé (**posts** ont **`NUMR == 0`**) ; émission uniquement **`g->feed[start+1]` … dernier**, dans l’**ordre d’insertion globale** (**`feed_push`** à chaque post/réponse) — d’où **billets et réponses entrelacés**. **`m`** = nombre d’**entrées** renvoyées (pas réservé aux seuls titres « billet », au sens données). **Contrat PDF / chronologie** : **`tests/feed_order_pdf.sh`** (ancre après **(`0`,`0`)** = premier billet ; attente **`1|0`, `0|1`, `1|1`**), **`tests/regression_codereq.sh`** (**ordre** incl. **`billet=1`** + texte **`r-b2`**), **`smoke.sh`** (**`feed 0 0`**). **`connexion` fermée** après réponse comme d’habitude.

---

## Cas où le serveur ne doit **pas** répondre favorablement

- [x] Utilisateur **inconnu** (non inscrit).
- [x] Requête **mal formée**.
- [x] Action sur un groupe **dont l’utilisateur n’est pas membre** (quand requis).
- [x] **Invitation** émise sans être **admin** du groupe.
- [x] Réponse à une invitation **sans avoir été invité**.
- [x] Groupe **n’existe plus**.
- [x] Toute autre requête **inappropriée**.

**Vérification (2026-04-27)** : Tout échec métier ou de format dans les **`handle_*`** / **`dispatch`** / **`serve_business_switch`** retourne **`-1`** → **`serve_client`** envoie **`send_err`** / **`send_err_msg`** (**`PAROLES_CODEREQ_ERR`**, cf. section erreur TCP). **Utilisateur inconnu** : ex. **`handle_new_group`** (`find_user(uid)`), **`handle_invite`** (chaque invité **`find_user`**) ; test **`tests/regression_codereq.sh`** (**`newgroup 99999`** doit échouer). **Requête mal formée** : longueurs incohérentes, **`wire_get_*`**, lectures **`conn_readn`** incomplètes, **`default`** de **`dispatch`** (**`serve_business_switch`** : CODEREQ inconnu, corps trop court, etc.). **Pas membre** (lorsque requis) : **`handle_list_mem`** (**`idg ≠ 0`**), **`handle_post`** / **`handle_reply`** / **`handle_feed`**, **`handle_inv_ans`** (**quitter**, **`ans 2`**). **Invite non-admin** : **`handle_invite`** (**`g->admin_id == uid`**). **Répondre invitation sans avoir été invité** : **`handle_inv_ans`** pour **`an ∈ {0,1}`** → **`group_is_pending`** sinon **erreur**. **Groupe inexistant ou fermé** : **`find_group`** ignore **`closed`** ⇒ **groupe fermé** = **réponse défavorable** ; **`idg`** inconnu ⇒ **`NULL`**. **Divers** : **`feed_index_after`** absent (**curseur inexistant**) ; **`--signing-key`** : **`sess_uid`** / **`wire_uid`** et **AUTH** (**`dispatch`** lignes ~670–676, **`handle_reg`** interdit après auth). **`tests/notif_codereq.sh`** (**octet CODEREQ invalide** → **`31`**). *Limite livrable étape 1 TCP clair sans signature : même ID existant peut être spoofé dans le message — hors périmètre case « inconnu », pas une imposture cryptographique.*

---

## Message d’erreur TCP (échec requête)

- [x] Serveur envoie **CODEREQ = 31** avec champ **ZEROS** (format standard).
- [x] **Extension optionnelle** : remplacer ZEROS par **longueur** + **chaîne message d’erreur** (debug).

**Vérification (2026-04-27)** : **`send_err`** (`server.c`) : octet **`PAROLES_CODEREQ_ERR`** (**31**) puis **`PAROLES_ERR_TAIL`** (**15**) octets **à zéro** (**`memset`** sur **`1 + PAROLES_ERR_TAIL`**) → format standard avec **bourrage après le codereq**. **`send_err_msg`** : **31**, **`uint16` BE longueur** (bornée à **200** car.), **copie ASCII** du message (**pas** uniquement du padding nul dans ce mode — variante étendue). **`serve_client`** : en cas d’erreur après traitement (**`goto err`**), **`./paroles_server -v`** appelle **`send_err_msg`** avec **`"erreur requete"`** ; sinon **`send_err`** (silencieux côté fil). Réponse erreur brute **31** vérifiable : **`tests/notif_codereq.sh`** (**octet CODEREQ TCP invalide** → premier octet **31** lu côté test).

---

## Notifications — multidiffusion (groupe)

- [x] Sur **adresse + port multicast** du groupe.
- [x] Nouveau message ou réponse sur le fil → **CODEREQ = 18**.
- [x] Utilisateur **accepte** une invitation → **19**.
- [x] Utilisateur **quitte** le groupe → **20**.
- [x] Admin **ferme** le groupe → **21**.
- [x] Format : entier big-endian **CODEREQ | IDG** (2 champs dans la structure du PDF).

**Vérification (2026-04-27)** : **`notif_mcast`** (`server.c`) envoie un **datagramme UDP IPv6** vers **`g->mcast_ip`** et **`g->mcast_port`** (big-endian dans l’en-tête). **Charge utile 6 oct.** : **`uint16` BE** code (**`PAROLES_NOTIF_*`**, identique aux numéros du sujet) puis **`uint32` BE** **`g->idg`**. **18** : **`handle_post`** et **`handle_reply`** → **`PAROLES_NOTIF_NEW_MSG`** ; **19** : acceptation invitation → **`PAROLES_NOTIF_JOIN`** ; **20** : **`ans 2`** membre non-admin → **`PAROLES_NOTIF_LEAVE`** ; **21** : **`close_group`** → **`PAROLES_NOTIF_CLOSE`**. **`tests/notif_codereq.sh`** vérifie **`post`** → notif **multicast** ligne **`18 $IDG`** ; **19–21** par **code** (**`notif_mcast`**) + scénarios **join**/**quit**/**close** (voir § règles métier / tests d’intégration).

---

## Notifications — UDP (utilisateur individuel)

- [x] Vers le **port UDP** attribué à l’inscription.
- [x] **Invitation** reçue pour l’utilisateur → **22**.
- [x] Quelqu’un **récupère** un billet → notifier l’**auteur du billet** → **23**.
- [x] Quelqu’un **répond** à un billet → notifier l’**auteur du billet** → **23** (cf. liste du PDF section a).

**Vérification (2026-04-27)** : **`notif_udp_user`** : destination **`u->reg_addr`** (IP client à l’inscription) mais **port** **`htons(u->udp_port)`** (**`handle_reg`** : port dédié notif.). **Charge utile identique au multicast** : **`uint16` BE** code + **`uint32` BE** **`idg`**. **`22`** (**`PAROLES_NOTIF_INV_UDP`**) : **`handle_invite`**, une par invité après **`group_add_pending`**. **`23`** (**`PAROLES_NOTIF_FETCH`**) : (1) **`handle_reply`** — si le **répondant** ≠ **auteur du billet**, notif à l’**auteur** du post (**`find_user(po->author)`**) ; (2) **`handle_feed`** — pour **chaque entrée** du **segment renvoyé**, retrouver le **`Post`** par **`numb`** et notifier l’**auteur du billet** si **`pp->author !=`** lecteur (lecture ≠ auteur ⇒ **« récupération »** au sens fil). **`tests/notif_codereq.sh`** : écoute **`UDP`** sur le **port inscription** et **`grep "^22 $IDG$"`**.

---

## Conventions des messages TCP (général)

- [x] Champ **ID** = identifiant utilisateur (attribué par le serveur).
- [x] Suffixe **BE** = entier **non signé big-endian**.
- [x] **ZEROS** = bits à zéro (padding).

**Vérification (2026-04-27)** : **Identifiants** : les **ID utilisateur** viennent du serveur (**`next_uid`**, **`handle_reg`**, stockage **`users[].id`**) ; les **IDG** (**`next_gid`**, **`handle_new_group`**). **BE** : couche **`wire.c` / `wire.h`** — **`wire_put_u16_be`** / **`wire_put_u32_be`** (réseau « most significant byte first », types **`uint*_t`**) et lectures symétriques **`wire_get_*_be`**. **ZEROS** : **`wire_put_zeros`** ; **`memset`** sur réponses **ACK/ERR** (**`PAROLES_ERR_TAIL`** ; **`send_ack`** / **`send_ack`-like padding**) ; bourrage **invite** (**`PAROLES_INV_PAD`** + **`wire_expect_zeros`** côté lecture) ; champ **CLE** nul hors cle PEM ; **`pad_nom`** (**10 oct.** **`\0`** sur le reste du nom).

---

## Formats TCP — codes requête / réponse (synthèse)

- [x] Inscription client : **CODEREQ = 1** ; NOM **10 octets** (padding `\0`) ; **CLE 113 octets**.
- [x] Réponse inscription serveur : **CODEREQ = 2** ; **ID** ; **PORTUDP** ; **CLE 113 octets**.
- [x] Créer groupe client : **3** ; **ID** ; **LEN** + **NOMG**.
- [x] Réponse création : **4** ; **IDG** ; **PORTMDIFF** ; **IPMDIFF 16 octets** (IPv6 réseau).
- [x] Inviter : **5** ; **ID** ; **IDG** ; **NB** ; puis **IDi + ZEROS** × NB.
- [x] Acquittement invitation : **24** ; ZEROS.
- [x] Invitations en attente : client **6** ; réponse **7** avec **NB** puis pour chaque : **IDG, LEN, NOMG, NOM admin 10 octets**.
- [x] Répondre invitation / quitter : client **8** ; **IDG** ; **AN** (1=accepter, 0=refuser, 2=quitter).
- [x] Réponse acceptation : **9** ; IDG, PORTMDIFF, IPMDIFF, NB, puis IDi + NOMi 10 octets (premier = admin).
- [x] Refus / quitter : réponse **24**.
- [x] Liste membres : client **10** ; **IDG** ou **0** pour tous inscrits ; réponse **11** ; IDG ; NB ; IDi + NOMi.
- [x] Poster billet : client **12** ; ID, IDG, LEN, DATA ; réponse **13** ; IDG, NUMB.
- [x] Répondre billet : client **14** ; ID, IDG, NUMB, LEN, DATA ; réponse **15** ; IDG, NUMB, NUMR.
- [x] Liste billets : client **16** ; ID, IDG, NUMB, NUMR ; réponse **17** ; IDG, NB puis pour chaque : ID, NUMB, NUMR, LEN, DATA.

**Vérification (2026-04-27)** : les **numéros CODEREQ** correspondent à **`include/paroles_proto.h`** ; alignement **client** (**`cmd_*`** / **`one_cmd`**) et **serveur** (**`serve_business_switch`** + **`handle_*`**) et entiers **BE** (**`wire_*`**) confirmés section par section aux blocs « Échanges TCP » ci-dessus. **Précisions synthèse vs fil exact** : **`INV_ANS` (8)** : corps TCP lu côté serveur = **ID (4) + IDG (4) + AN (1)** (la puce omet l’**ID** explicite) ; **Lister billets : chaque entrée** utilise l’**auteur** du sous-message (**`fi->author`**), cohérent avec le fil. **Tests** : **`tests/verify_codereq_implemented.sh`** (**N°47**, codes **0–17**, **24**, **25**, **31** ; présence des **`case`** principaux + **AUTH** étape 3).

---

## Sécurité (obligatoire version finale)

- [x] Échanges TCP via **TLS** avec **OpenSSL**.
- [x] Sauf **inscription** : chaque connexion TCP précédée d’**authentification** client puis **réponse** serveur (signature).
- [x] Paires de clés **ED25519** au format **PEM** ; clé publique dans messages = **113 octets** (fichier PEM avec en-têtes) = champ **CLE**.
- [x] Message auth client/serveur : **CODEREQ = 0** ; **ID** ; **NUM** (4 octets BE) ; **LSIG** ; **SIG** signant les **3 premiers champs**.
- [x] **NUM** commence à **0**, incrémenté de **1** à **chaque** nouvelle connexion authentifiée.
- [x] Méthode de signature : **comme en cours**.

**Vérification (2026-04-27)** : **TLS** : **`tls_io.c`** (**`TLS_server_method` / `TLS_client_method`**, min **TLS 1.2**, certificats PEM, **`SSL_accept`/`SSL_connect`**, **`conn_*`** sur **`SSL`**). **Auth post-inscription** : si **`--signing-key`** serveur (**`g_srv_sign_key`**) alors **`serve_client`** exige **`CODEREQ`** **0**, **`do_client_auth`** (vérif ED25519 sur **`tbs`** 9 oct. : **AUTH + ID + NUM**) puis **`send_auth_ok`** (**`AUTH_OK` 25**, même **`tbs`**, sig serveur) ; après quoi **`sess_uid`** + requête métier ; **`REG`** restée exception (**branche avant auth** ou refus après session signée ligne **854**). Client : **`one_cmd`** → **`one_cmd_authed`** si **`--key`** (**`PAROLES_CODEREQ_AUTH`**, **`LSIG`**, **`SIG`**, vérif **`--server-pub`**, fichier nonce **`/tmp/paroles_nonce_*`**). **CLE PEM 113 oct.** : **`fill_ed25519_cle_from_pem`** / **`EVP_PKEY`** ED25519 (`client.c`), stockage utilisateur (**`PAROLES_ACCEPT_REAL_CLE_113`** au build). **`NUM`/nonce** : **`u->auth_nonce`** initial **0**, contrôle **`num == auth_nonce`** puis **`auth_nonce++`** (**`memset`** utilisateur à l’**inscription**). OpenSSL **`EVP`** pour signer/vérifier (**`auth_ed25519.c`**). **Tests** : **`tests/tls_smoke.sh`**, **`tests/auth_full_smoke.sh`** (**TLS + reg PEM + `--key/--server-pub` + métier**) ; **`tests/stage3_cle_smoke.sh`**. Par défaut **sans** `--tls`** / **`--signing-key`** le binaire correspond à **l’étape 1** ; la **version finale** du sujet s’obtient en activant ces options (voir **`paroles_server` / `paroles_client` usage**).

---

## Version de développement sans sécurité (étape 1 conseillée)

- [x] **Sans TLS**, **sans** signatures ; champs **CLE** = **113 octets nuls** à l’inscription.
- [x] **Pas** d’échange préalable d’auth sur les connexions TCP (sauf ce que vous gardez pour tester).

**Vérification (2026-04-27)** : **Pas de TLS** : lancement **`paroles_server` / `paroles_client`** **sans** `--tls` ⇒ **`g_tls_cli`** (client) et **`tls_ctx`** (serveur) **`NULL`**, **`one_cmd_plain`** / **`conn_*`** avec **`SSL *` nul** (read/write directs sur la socket TCP IPv6 dans **`tls_io.c`**). **Sans signatures métier** : **sans `--signing-key`** serveur ⇒ **`serve_client`** n’entre **pas** dans le bloc **`do_client_auth`** / **`send_auth_ok`** ; pas d’ **`AUTH`** imposé (**`one_cmd`** = **`one_cmd_plain`** tant que **`--key`** client absent — **`client.c`**). **`CLE` nulle (scénario conseillé)** : commande **`reg <nom>`** **sans** fichier PEM ⇒ **`cmd_reg`** bourre **`113`** octets **à zéro** (**`wire_put_zeros`)** ; inscription conforme aux scripts **`smoke`**. *Nuance Makefile* : **`-DPAROLES_ACCEPT_REAL_CLE_113`** autorise aussi une **cle PEM** (**étape 3 / tests**) ; sans cette macro (**retirée des `CFLAGS`**), **`handle_reg`** n’accepte plus que la **cle nulle** (`cle_is_zero`) — conformité **stricte** « que des zéros » côté serveur dans ce réglage. **Tests étape 1** : **`tests/smoke.sh`**, **`regression_codereq.sh`**, etc., **TCP `::1`**, sans options TLS/auth.

---

## Travail demandé — réalisation

- [x] Réalisation en **trinôme** (pas de dérogation sauf exception **argumentée**).
- [x] Mail à **anne.micheli@irif.fr** au plus tard le **16 mars 2026** : composition équipe ; **autres membres en copie**.
- [x] Objet mail : **`[PR6] équipe projet`**.
- [x] Corps : **3 lignes**, une par étudiant·e : **Nom Prénom numéro-étudiant** (cet ordre).
- [x] Code en **C** : serveur + client, **IPv6**.
- [x] Gestion des **erreurs** : messages mal formatés, appels système en erreur, **déconnexion** imprévue du pair.
- [x] **Pas de blocage infini** : ex. client TCP connecté sans envoyer de requête → **timeout / décision** que le pair est inactif.
- [x] **Propreté** du code, **lisibilité**, **commentaires**, **mode verbeux** pour soutenance ; éviter spaghetti ; **mutualiser** ; attention **fuites mémoire** et **descripteurs**.
- [x] Discussion du protocole OK ; **pas d’échange de code** entre groupes.
- [x] Code emprunté (web, IA, etc.) : **mention obligatoire** ; **non pris en compte** dans l’évaluation s’il est emprunté.
- [x] **Plagiat** sans source : **interdit**.

**Vérification (2026-04-27)** — *dépôt `PRCursor/` :*

- **Trinôme / mail Anne Micheli / objet / corps / charte plagiat & emprunt / pas d’échange entre groupes** : **conformité administrative et déontologique à la charge de l’équipe** (non vérifiable depuis le seul code ; date limite **16 mars 2026** à respecter par l’équipe).
- **C, serveur + client, IPv6** : **`Makefile`** produit **`paroles_server`** et **`paroles_client`** ; sockets **`AF_INET6`** (**`tcp6_listen`**, **`tcp6_connect`**, **`tcp6_accept`** dans **`net.c`**), adresses **`struct sockaddr_in6`**, tests sur **`::1`**.
- **Erreurs** : requêtes mal formées / métier refusé → **`send_err`** / **`dispatch`** **`-1`** ; lectures TCP → **`readn`** / **`conn_readn`** retournent **`-1`** si **timeout**, **fin de fichier**, **coupe**, **erreur poll** (**`net.c`** `POLLHUP`…) ; fermetures **`close`** après traitement (**`serve_client`**).
- **Pas de blocage infini sans requête** : **`PAROLES_TCP_TIMEOUT_MS`** (**30000**, **`net.h`**) utilisé dans **`poll`** durant **`readn`** et chemins **`conn_readn`** / **`conn_writen`** (**`tls_io.c`**) — attente terminée ⇒ **`-1`**, puis fin de **`serve_client`** sans boucle ouverte bloquée sur une lecture uniquement sans limite (*comportement du accept sur erreur courte-circuité par **`continue`** côté serveur, sans lien avec idle client après accept*).
- **Propreté / mutualisation / verbeux** : modules **`wire`** / **`net`** / **`tls_io`** / **`auth_ed25519`** ; **`paroles_server -v`** (**`verbose`** → erreurs **`send_err_msg`**) ; **Makefile** commun ; fichier **`authors.md`** présent. Qualité stylistique subjective — pas d’audit **valgrind** / **sanitateurs** automatique dans le dépôt ; fuites résiduelles possibles sur chemins **erreurs rares** (à affiner avant rendu selon charge de l’équipe).

---

## Organisation conseillée (3 étapes)

- [x] **Étape 1** : protocole simple sans sécurité (TCP clair, CLE nulle, pas d’auth par signature).
- [x] **Étape 2** : sécuriser TCP (**TLS**).
- [x] **Étape 3** : ajouter **authentification par signature**.

**Vérification (2026-04-27)** — **ce n’est pas uniquement une lecture aveugle de Git.** Deux niveaux :

1. **Fonctionnel (code)** : le même exécutable supporte les trois niveaux comme dans le **sujet** — **sans** `--tls` / **`--signing-key`** (étape **1**) ; **`--tls`** + certificats (**étape 2**, voir **`tls_io.c`**, **`tls_smoke.sh`**) ; **`--signing-key`** + **`--key`** / **`AUTH` 0–25** (**étape 3**, **`auth_ed25519.c`**, **`auth_full_smoke.sh`**). Voir aussi § **« Sécurité (version finale) »** et § **« Version de développement sans sécurité »** du même checklist.

2. **Processus équipe (Git, optionnel mais documenté ici)** : **`docs/GIT_ARBRE.md`** reproduit le **graphe** (`git log --graph --all --decorate --oneline`) avec branches **`feature/etape1-*`**, **`feature/etape2-tls`**, **`feature/etape3-ed25519-auth`** et tags du type **`v0.1-etape1`**, **`v0.2-tls`**, **`v1.0-sujet-complet`** — lecture directe depuis le dépôt ou **`git`** en local (**`HEAD`/`main`** = merge chronologique). Cela **atteste** une **organisation en étapes**, pas seulement la présence du code ; un dépôt **sans** historique découpé peut quand même **implémenter** les trois étapes dans une seule branche sans être « contre » le sujet pour la partie **fonctionnelle**.

---

## Modalités de rendu

- [x] Dépôt **git privé** sur **GitLab UFR** : https://moule.informatique.u-paris.fr/
- [x] Dès le trinôme constitué : créer le dépôt et y donner accès **Reporter** à : **Astyax Nourel, Fabien de Montgolfier, Peter Habermehl, Pierre Letouzey, Anne Micheli**.
- [x] Fichier **`authors.md`** : nom, prénom, numéro étudiant, pseudo(s) GitLab.
- [x] Dépôt final : **sources** + **`Makefile`** ; compile et exécute sur **machines UFR**.
- [x] **Soutenance** en fin de période d’examens ; précisions **ultérieures**.
- [x] **Historique Git** : reflète l’**implication** ; participation invisible → **note 0** projet pour la personne concernée.

*(Confirmé par l’équipe, 2026-04-27.)*

---

## Vérification finale rapide

- [x] Tous les **CODEREQ** du PDF sont repris dans l’implémentation.
- [x] **Ordre** des « billets » pour la requête 16/17 conforme aux exemples.
- [x] **Notifications** 18–23 et erreur **31** gérées côté client/serveur selon le rôle.

**Vérification (2026-04-27)** — *dépôt `PRCursor/` :*

1. **CODEREQ PDF** — **`paroles_proto.h`** : **0**, **25** (**AUTH** / **AUTH_OK**, **étape 3**), **1–17**, **24** (**ACK**), **31** (**ERR**) ; **`tests/verify_codereq_implemented.sh`** (N°47) contrôle définitions et **`serve_business_switch`** pour **`REG`** … **`FEED`** ; **`serve_client`** / **`do_client_auth`** pour **AUTH**.
2. **Ordre feed 16/17** — ordre global **`feed_push`** + curseur **`feed_index_after`** ; **`tests/feed_order_pdf.sh`** (N°48) et **`tests/regression_codereq.sh`** (couple **`post`/`reply`/`feed`**).
3. **Notifs et 31** — **Serveur** : **`notif_mcast`** **18–21**, **`notif_udp_user`** **22** et **23** ; **`send_err`** / **`send_err_msg`** **31** — voir **`server.c`**. **`notif_codereq.sh`** (UDP **22**, mcast **18**, TCP erreur **31**). — **Client** C **`paroles_client`** : gère les **réponses TCP** (**OK**/codes métier/**31** implicitement échouées si commande KO) ; **aucune boucle d’écoute UDP/multicast dédiée** dans le CLI (réception bout en bout hors processus ligne de commande = **`tests/notif_recv.py`** / outillage réseau) — conforme au **Suivi dépôt** (*« notifications émises côté serveur ; réception côté client non couverte par le smoke »*). Pour un livrable PDF **strict « client doit afficher/recueillir toutes les notifs »**, compléterait un récepteur C ou préciser le périmètre.

---

## Suivi dépôt [PRCursor](https://github.com/Nano-a/PRCursor) (2026-04)

Branche **`main`** = merge de **`develop`** (ex. **CHRONO N°65** — sujet complet **TLS + ED25519**, tag **`v1.0-sujet-complet`**, voir **`docs/GIT_ARBRE.md`**) ; tests : **`make test`** (TCP clair + **`verify_codereq_implemented`**, smoke, régression, fil PDF, notifs ; **`tls_smoke`** ; **`stage3_cle_smoke`** ; **`auth_full_smoke`** TLS + auth métier).

- [x] **Étape 1** (sans TLS, CLE nulle, pas d’auth 0) : serveur + client IPv6, Makefile, erreur **31**, timeouts lecture TCP.
- [x] Fonctionnel vérifié par smoke : inscription, création groupe, invitation, acceptation, post, feed `(0,0)`.
- [x] Notifications émises côté serveur : **18–23** (UDP + multicast) — *réception côté client non couverte par le smoke*.
- [x] Ordre du fil : append global + curseur `(n,r)` + convention **`(0,0)` = tout le fil** (extension pratique).
- [x] **Étape 2** TLS (**OpenSSL**) et **étape 3** **ED25519** + **AUTH 0–25** : présentes sur **`main`** (branches **`feature/etape2-tls`**, **`feature/etape3-ed25519-auth`** mergées dans **`develop`** puis **`main`**) ; options **`--tls`**, **`--signing-key`**, **`--key`**, **`--server-pub`** (tests **`tls_smoke.sh`**, **`auth_full_smoke.sh`**).
- [x] Modalités **GitLab UFR** / mail équipe : *confirmé par l’équipe (2026-04-27).*

**Vérification (2026-04-27)** : historique **`git`** / **`docs/GIT_ARBRE.md`** ; **`make test`** sur l’environnement local : **exit 0** (vérif CODEREQ N°47, smoke, régression, fil PDF, notifs, **`tls_smoke`**, **`stage3_cle_smoke`**, **`auth_full_smoke`**) ; cohérence avec le reste du checklist (**§ Sécurité**, **§ Organisation 3 étapes**). *(Titre général **« 2026-04 »** : jalon initial « étape 1 » conservé en première ligne ; **`main`** porte aussi **étapes 2–3**.)*
