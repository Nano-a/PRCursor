# Validation et cas limites — Paroles (avant phase « codage ciblé »)

Document de travail pour **recenser** comportements observés (**succès**, **silencieux**, **erreur**) quand on teste **`paroles_server`** / **`paroles_client`**, puis pour **prioriser** des améliorations (messages explicites, codes retour, règles métier renforcées).

**Usage suggéré :** pendant ou après les tests (`make test`, scénarios A/B du guide), cocher ou noter dans la colonne **Observé** puis décider si le sujet impose un changement.

---

## 0. Succès **sans** message à l’écran — comment trancher « bon / pas bon » ?

Le client **ne garantit pas** une ligne de texte **à chaque** sous-commande réussie.  

**Règle systématique après chaque commande :**

```bash
./paroles_client …
echo $?
```

| `$?` | Interprétation usuelle (bash) |
|------|-------------------------------|
| **`0`** | Processus **terminé en « succès »** côté client (y compris réponse serveur conforme). |
| **≠ `0`** | **Échec** (argument, connexion, erreur métier côté protocole, etc.). |

**Important :** **pas de message d’erreur visible** ≠ succès : une commande **muette** peut tout de même **échouer** → **`echo $?`** est le **seul** indicateur fiable tant que le client n’affiche pas systématiquement `OK` / `erreur`.

### Tableau indicatif PRCursor (succès au vol — à compléter selon tes essais réels)

| Sous-commande | Affichage **souvent** vu au succès | Peut rester **muette** au succès ? |
|---------------|-----------------------------------|--------------------------------------|
| `reg` | `OK id=… udp=…` | Rarement si tout va bien |
| `newgroup` | `OK idg=… mcast=… port=…` | En général non |
| `invite` … | Rien ou peu | **Oui** — très courant → **`echo $?`** |
| `ans` … | `OK join`, `OK ack`, etc. | **Échec** possible **sans aucun** message (voir § `ans` ci‑dessous) → **`echo $?`** |
| `listinv` / `listmem` | Lignes de liste ou `invitations 0` | Dépend du contenu |
| `post` / `reply` | `OK billet…` / `OK reply…` | En général non si succès |
| `feed …` | Bloc lignes avec `feed` / `[uid billet=…]` | Rarement vide s’il y a des messages |

### `invite <admin> <idg> <uid> [<uid> …]`

Après **`idg`**, **chaque** nombre suivant est un **utilisateur à inviter** :  
ex. `invite 1 1 2` = une personne (`2`) ; `invite 1 1 2 3` = **deux** personnes (`2` et `3`).  
Succès possible **sans aucune** ligne affichée → toujours **`echo $?`**.

### `ans <uid> <idg> <0|1|2>` (PRCursor)

Sens usuel **`paroles_client … ans …`** :

| **`0`** | Refus de l’invitation (`OK ack` si tout va bien). |
| **`1`** | Acceptation → entrée dans le groupe (`OK join`, etc.). |
| **`2`** | **Quitter** le groupe (membre qui sort ; cas **admin** = fermeture du groupe selon implémentation). **N’a rien à voir** avec une invitation **en attente** : pour « répondre à l’invitation », ce sont **`0`** ou **`1`**. |

**Cas observé (équivalent « quitter alors qu’on n’est pas dans le groupe ») :**

Ex. après `invite … 3`, l’utilisateur **`3`** a encore **`listinv`** non vide, mais **`ans 3 … 2`** alors qu’il n’a **jamais accepté** avec **`… 1`** : côté serveur de référence **`2`** impose d’être **déjà membre** (`group_is_member`). La requête est **refusée** ; le client **n’affiche ni `OK ack` ni un message d’erreur lisible**, alors que **`echo $?`** vaut **`1`**. À ne **pas** interpréter comme un succès silencieux : c’est un **échec muet** tant que le client ne journalise pas l’erreur protocole.

---

### `listmem <uid> <idg>` — membres du **groupe** ou **tous les inscrits** au serveur

C’est **la** sous-commande dédiée (usage client : **`listmem <uid> <idg>`**, voir `paroles_client`). Au succès, le client affiche une ligne du type **`membres idg=… nb=…`** puis une ligne **`  <uid_affiché> <pseudo>`** par entrée.

| Valeur **`idg`** | Ce qui est listé | Remarque côté référence **PRCursor** |
|------------------|------------------|--------------------------------------|
| **`0`** | **Tous les utilisateurs** actuellement enregistrés sur le **serveur** (slots utilisés dans la table utilisateurs). | **`uid`** n’est pas utilisé pour le filtre d’accès dans ce cas particulier. |
| **`> 0`** | Les **membres du groupe** `idg` (souvent **admin en tête** si l’implémentation range `mem[0]` comme créateur). | Le serveur **refuse** si **`uid`** n’est **pas** membre du groupe → tester aussi un **non-membre** pour valider le rejet. |

Exemples (après `export H=…` et `P=…`) :

```bash
./paroles_client "$H" "$P" listmem 1 0    # tous les inscrits
./paroles_client "$H" "$P" listmem 1 1     # membres du groupe 1 (uid 1 doit être membre)
```

---

## 1. Chaîne vide / variables shell manquantes (hors protocole)

| Cas | Comportement possible | À vérifier côté code ? |
|-----|----------------------|-------------------------|
| **`$U_a1`** non défini → `newgroup "$U_a1" …` équivalent à **UID vide** | Client peut accepter un argc incorrect, erreur muette ou code retour ≠ 0 | ✅ Message d’usage / `fprintf(stderr)` + code retour ≠ 0 |
| **`"$H"`** ou **`"$P"`** vide | Connexion échoue (`Connection refused`, mauvais host, etc.) | Documenter dans README ; éventuelle vérif prévol |
| Mauvais **nombre** d’arguments pour une sous-commande (`invite`, `reply`, `feed`) | Comportement indéterminé ou crash | ✅ Validation **`argc`** systématique en client |

---

## 2. Inscription (**`reg`**)

| Cas | Ce que dit en général le sujet PR6 | À observer / coder |
|-----|------------------------------------|----------------------|
| **Deux fois le même pseudo** (*Alice*, *Alice*) | **IDs distincts** (régression codereq) — **pas** d’erreur « déjà pris » | Confirmer : deux entrées utilisateur avec même nom, **`id`** différents |
| « **Même personne physique** » qui **`reg`** deux fois avec le même pseudo | Comportement = **deux utilisateurs différents** côté serveur — c’est **voulu** dans le jeu de régression | Ne **pas** confondre avec « connexion OAuth » ; le sujet distingue **pseudo** ≠ **identité forte** à l’étape 1 |
| Tentative **`reg`** alors que pseudo **illégal** (trop long, caractères interdits selon PDF) | Rejet **`CODEREQ 31`** ou équivalent | Vérifier spec exacte PDF + erreur lisible |
| **Re-inscription exacte même socket / même client** après déjà avoir un **id** (le client refait **`reg`** à chaque fois) | En principe une **nouvelle** ligne utilisateur (**nouveau** `id`), sauf implémentation session persistante (hors MVP) | Décision produit : voulez-vous un message « déjà inscrit » ? **Non exigé** par défaut étape 1 si le sujet ne le demande pas explicitement |

---

## 3. Groupes (**`newgroup`**)

| Cas | Suje / cours | À coder / tester |
|-----|----------------|-------------------|
| **Deux groupes même nom `MonGroupe`** | **IDs de groupe différents** (**idg**) — autorisé | Confirmer : deux `newgroup … MonGroupe` → **idg** 1 puis 2 (ou équivalent) |
| Créateur **sans** être inscrit (uid inventé ou 0) | Rejet erreur métier | Vérifier `newgroup 9999 …` |
| Nom de groupe vide / longueur 0 selon protocole | Erreur 31 ou refus parsing | Tester |

---

## 4. Invitations / réponses (**`invite`**, **`ans`**)

| Cas | Commentaire | À prioriser |
|-----|----------------|---------------|
| **Invite** alors que **l’inviteur** n’est **pas admin** | Rejet contrôlé serveur | Tests `invite` depuis membre normal |
| **Invite un uid inconnu** | Erreur 31 ou ack négatif | Script manuel |
| **`ans`** sur invitation **non existente** ou **déjà traitée** | Erreur claire ou 31 | 
| **`ans 0`** (refus) puis nouvelle **`listinv`** vide sur l’invite | Comportement attendu régression |

---

## 5. Posts / réponses / feed

| Cas | À vérifier |
|-----|------------|
| **`post`** hors groupe (pas membre) | Erreur |
| **`reply`** sur **`num_billet`** inexistant | Erreur |
| **`reply`** hors droits | Erreur |
| **`feed`** avec curseurs **hors borne** ou `(numb,numr)` inconsistants selon pdf | À relire exemple PDF § fil |

---

## 6. Erreurs réseau (environnement)

| Symptôme | Cause typique |
|----------|----------------|
| **`Connection refused`** | Serveur non lancé, **mauvais `P`** dans ce terminal (`export P`) |
| Succès intermittent | Première salve **`reg`** collée avant **bind** réel du serveur |
| Silence **sans OK** après commande avec args cassés (`$U_*` vide) | Voir § 1 |

---

## 7. Pistes UX / robustesse **client** (optionnel projet)

À traiter après validation fonctionnelle **obligatoire** du PDF :

| Idée | But |
|------|-----|
| Afficher **`usage:`** si **`argc`** insuffisant pour chaque sous-commande | Éviter le « silence » trompeur |
| **`echo $?` documenté partout dans le guide équipe** | Savoir sans debugger si échec |
| Message explicite : **« Variable U_a1 vide — faites export U_a1= … »** | Uniquement si vous surchargez shell (peu courant ; mieux : document README) |

---

## 8. Suivi équipe — à remplir

Pour chaque ligne, noter au minimum : **commande**, **affichage** (ou « muet »), **`echo $?`**, et si c’est **conforme** au sujet.

| Fonctionnalité | Testé O/N | Observé (sortie + `echo $?`) | Bon / à corriger ? |
|----------------|-----------|------------------------------|----------------------|
| `reg` double pseudo | | | |
| `newgroup` nom dupliqué | | | |
| Args `invite` incomplets | | | |
| `invite` succès muet | | | |
| `ans uid idg 2` sans être membre (invitation encore en attente) | | | |
| `listmem` avec `idg=0` (tous inscrits) | | | |
| `listmem` avec `idg>0` (membres groupe) + non-membre | | | |
| … | | | |

---

## 9. Lien avec ce document pendant la prod

Après avoir **bouclé les tests automatiques et manuels du guide**, revenir ici ligne par ligne : ce qui reste comme **lacune utilisateur** (messages, validation d’entrée) devient la **liste de tâches** pour la phase **codage contrôlé**, **sans élargir** le scope au-delà du **projet PDF** sauf décision commune du trinôme.
