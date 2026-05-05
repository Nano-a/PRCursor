# Tableau chronologique — checklist projet PR6 « Paroles »

**Colonnes :** ordre d’exécution conseillé | **Fait** (case à cocher) | tâche (alignée sur `checklist_projet_PR6.md`) | branche Git où la tâche vit (ou cible du merge) | **Qui** (blocs **AJINOU → CHABIRA → SERROUKH** ; **lignes 1–13** attribuées au **chef de projet** ; sur **14–68** on reprend l’**ancienne** alternance par blocs, pour **ne pas retirer** le chef des tâches qui lui étaient déjà assignées — par ex. **20–22**, **28–31**, **38–39**, **47–48**, **64–66** ; totaux **27 / 20 / 21**).

**Répartition — succession de blocs** : les **13** premières lignes portent le nom du chef de projet (attribution explicite du socle et des CODEREQ **1–2**). Ensuite, même logique qu’avant la variante « chef seulement sur 1–13 » : blocs **CHABIRA** / **SERROUKH** / **AJINOU** selon le cycle, ce qui **réattribue** aussi au chef les paquets de lignes qui figuraient déjà sous son nom.

| Segments (N°) | Nb. | Qui |
|---------------|-----|-----|
| 1–13 | 13 | AJINOU Abderrahman (chef de projet) |
| 14–15 | 2 | CHABIRA Ahmed |
| 16–19 | 4 | SERROUKH Jabir |
| 20–22 | 3 | AJINOU Abderrahman (chef de projet) |
| 23–24 | 2 | CHABIRA Ahmed |
| 25–27 | 3 | SERROUKH Jabir |
| 28–31 | 4 | AJINOU Abderrahman (chef de projet) |
| 32 | 1 | CHABIRA Ahmed |
| 33–37 | 5 | SERROUKH Jabir |
| 38–39 | 2 | AJINOU Abderrahman (chef de projet) |
| 40–43 | 4 | CHABIRA Ahmed |
| 44–46 | 3 | SERROUKH Jabir |
| 47–48 | 2 | AJINOU Abderrahman (chef de projet) |
| 49–51 | 3 | CHABIRA Ahmed |
| 52–55 | 4 | SERROUKH Jabir |
| 56–61 | 6 | CHABIRA Ahmed |
| 62–63 | 2 | SERROUKH Jabir |
| 64–66 | 3 | AJINOU Abderrahman (chef de projet) |
| 67–68 | 2 | CHABIRA Ahmed |

*Compter **27** lignes pour le chef (dont **14** supplémentaires après la ligne 13 : **20–22**, **28–31**, **38–39**, **47–48**, **64–66**), **20** pour **CHABIRA**, **21** pour **SERROUKH** — légèrement différent de l’équilibre **23/23/22** car tout le bloc **1–13** est au chef.*

**Trinôme :** **AJINOU Abderrahman** (@ajinou) — chef de projet · **CHABIRA Ahmed** (@achabira) · **SERROUKH Jabir** (@serroukh).

## Règle de cochage (colonne **Fait**)

- Cocher **dans l’ordre strict des numéros** : ne pas cocher la ligne **n** tant que la ligne **n−1** n’est pas déjà cochée `[x]`.
- Ne passer à la ligne suivante qu’une fois la tâche **terminée et fonctionnelle** (validée dans les conditions du sujet), pas « en cours ».
- **Lignes 1 à 3** (trinôme, mail enseignante, dépôt GitLab UFR) : à cocher **par l’équipe uniquement**. Tant qu’au moins une de ces cases est vide, **ne pas** cocher les lignes **4 et suivantes** — le travail sur un dépôt GitHub miroir ou local ne remplace pas ces prérequis officiels.

Syntaxe : `[ ]` = non fait ; `[x]` = fait et validé.

| N° | Fait | Tâche à faire | Branche | Qui |
|----|------|---------------|---------|-----|
| 1 | [x] | Constituer le trinôme (obligatoire, pas de dérogation sans argument) | — | AJINOU Abderrahman (chef de projet) |
| 2 | [x] | Envoyer le mail équipe à anne.micheli@irif.fr (avant 16 mars 2026, objet `[PR6] équipe projet`, 3 lignes Nom Prénom n° étudiant, copie aux coéquipiers) | — | AJINOU Abderrahman (chef de projet) |
| 3 | [x] | Créer le dépôt **privé** GitLab UFR + donner accès **Reporter** aux enseignants (liste sujet) | GitLab (hors branche) | AJINOU Abderrahman (chef de projet) |
| 4 | [x] | Cloner le dépôt ; premier commit : README, `.gitignore`, squelette + **`authors.md`** complet | `main` | AJINOU Abderrahman (chef de projet) |
| 5 | [x] | Créer et pousser la branche **`develop`** depuis `main` | `develop` | AJINOU Abderrahman (chef de projet) |
| 6 | [x] | (Option) Protéger `main` sur GitLab, merges via MR depuis `develop` | GitLab | AJINOU Abderrahman (chef de projet) |
| 7 | [x] | Ajouter **Makefile** + vérifier compilation sur **machines UFR** (modalités rendu) | `develop` ou `main` | AJINOU Abderrahman (chef de projet) |
| 8 | [x] | Documenter **CREDITS** / sources externes (web, IA) dans README | `develop` | AJINOU Abderrahman (chef de projet) |
| 9 | [x] | Créer **`feature/etape1-proto-tcp`** depuis `develop` | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 10 | [x] | **Objectif** : serveur + client **IPv6** protocole Paroles (contexte checklist) | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 11 | [x] | **Étape 1** : sans TLS, sans auth 0 ; **CLE** = 113 octets nuls à l’inscription | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 12 | [x] | Conventions fil : champ **ID**, entiers **BE**, champs **ZEROS** | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 13 | [x] | Formats **CODEREQ 1–2** : inscription (NOM 10, CLE 113) + réponse ID, PORTUDP, CLE | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 14 | [x] | Échange inscription : client TCP pseudo + clé ; serveur ID unique, port UDP, clé serveur, IP client ; fin connexion | `feature/etape1-proto-tcp` | CHABIRA Ahmed |
| 15 | [x] | Règle : **même pseudo** possible, **IDs distincts** | `feature/etape1-proto-tcp` | CHABIRA Ahmed |
| 16 | [x] | Formats **3–4** : création groupe (LEN + nom) + réponse IDG, PORTMDIFF, IPMDIFF 16 octets | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 17 | [x] | Création groupe : IDG **unique strictement positif** ; **même nom** de groupe → IDG distincts | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 18 | [x] | Création groupe : adresse **multidiffusion unique** ; créateur = **admin** | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 19 | [x] | Formats **5** et **24** : invitation (IDG, NB, IDs) + acquittement | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 20 | [x] | Formats **6–7** : liste invitations en attente + réponse (IDG, LEN, nom, admin 10 octets) | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 21 | [x] | Formats **8**, **9**, **24** : réponse invitation (AN 1/0/2) + réponse jointure ou ack | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 22 | [x] | Règle : **admin quitte** → **fermeture définitive** du groupe | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 23 | [x] | Formats **10–11** : liste membres (IDG ou **0** = tous inscrits) ; **admin en premier** pour un groupe | `feature/etape1-proto-tcp` | CHABIRA Ahmed |
| 24 | [x] | Formats **12–13** : post + réponse NUMB ; billets **numérotés à partir de 0** par groupe | `feature/etape1-proto-tcp` | CHABIRA Ahmed |
| 25 | [x] | Formats **14–15** : réponse à billet + NUMR ; réponses **à partir de 1** | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 26 | [x] | Formats **16–17** : demande fil depuis (n, r) + réponse ; **ordre de réception global** (exemple PDF b2, réponses) | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 27 | [x] | Fonctionnel : **inscription** | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 28 | [x] | Fonctionnel : **créer groupe** (admin) | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 29 | [x] | Fonctionnel : **inviter** | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 30 | [x] | Fonctionnel : **recevoir / lister invitations** | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 31 | [x] | Fonctionnel : **accepter / refuser / quitter** groupe | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 32 | [x] | Fonctionnel : **poster** et **répondre** | `feature/etape1-proto-tcp` | CHABIRA Ahmed |
| 33 | [x] | Fonctionnel : **liste derniers billets** depuis (n, r) | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 34 | [x] | Rejet serveur : utilisateur inconnu, mal formé, pas membre, pas admin pour invite, pas invité, groupe absent, etc. | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 35 | [x] | Réponse erreur **CODEREQ 31** + ZEROS | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 36 | [x] | (Option sujet) Erreur 31 avec **longueur + message** texte pour debug | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 37 | [x] | Notifications **multicast** : adresse + port groupe ; codes **18–21** ; format **CODEREQ \| IDG** BE | `feature/etape1-proto-tcp` | SERROUKH Jabir |
| 38 | [x] | Notifications **UDP** perso : port à l’inscription ; **22** invitation ; **23** auteur billet (récup / réponse) | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 39 | [x] | Fonctionnel : **recevoir** notifs (côté client : joindre mcast + écouter UDP) | `feature/etape1-proto-tcp` | AJINOU Abderrahman (chef de projet) |
| 40 | [x] | Qualité : gestion erreurs syscall, messages mal formés, **déconnexion** pair | `feature/etape1-proto-tcp` | CHABIRA Ahmed |
| 41 | [x] | Qualité : **timeouts** (pas de blocage infini si client idle) | `feature/etape1-proto-tcp` | CHABIRA Ahmed |
| 42 | [x] | Qualité : code lisible, commentaires, **mode verbeux**, mutualisation, pas de fuites mémoire / FD | `feature/etape1-proto-tcp` | CHABIRA Ahmed |
| 43 | [x] | Rappel : pas d’échange de **code** entre groupes ; **plagiat** interdit | — | CHABIRA Ahmed |
| 44 | [x] | **Merge** `feature/etape1-proto-tcp` → **`develop`** (revue + tests) | `develop` ← merge | SERROUKH Jabir |
| 45 | [x] | Créer **`feature/etape1-tests-notifs`** depuis `develop` | `feature/etape1-tests-notifs` | SERROUKH Jabir |
| 46 | [x] | Tests auto / manuels : scénarios complets + non-régression **CODEREQ** | `feature/etape1-tests-notifs` | SERROUKH Jabir |
| 47 | [x] | Vérif : **tous les CODEREQ** 1–17, 24, 31 implémentés (0 en étape 3 seulement) | `feature/etape1-tests-notifs` | AJINOU Abderrahman (chef de projet) |
| 48 | [x] | Vérif : ordre fil **16/17** = exemple PDF | `feature/etape1-tests-notifs` | AJINOU Abderrahman (chef de projet) |
| 49 | [x] | Vérif : **18–23** et **31** OK selon rôle client/serveur | `feature/etape1-tests-notifs` | CHABIRA Ahmed |
| 50 | [x] | **Merge** `feature/etape1-tests-notifs` → **`develop`** | `develop` ← merge | CHABIRA Ahmed |
| 51 | [x] | **Merge** `develop` → **`main`** ; tag optionnel `v0.1-etape1` | `main` ← `develop` | CHABIRA Ahmed |
| 52 | [x] | Créer **`feature/etape2-tls`** depuis `develop` | `feature/etape2-tls` | SERROUKH Jabir |
| 53 | [x] | **Étape 2** : TCP sécurisé avec **TLS** (OpenSSL) | `feature/etape2-tls` | SERROUKH Jabir |
| 54 | [x] | Adapter client/serveur + certificats / script génération ; tests sur **UFR** | `feature/etape2-tls` | SERROUKH Jabir |
| 55 | [x] | **Merge** `feature/etape2-tls` → **`develop`** | `develop` ← merge | SERROUKH Jabir |
| 56 | [x] | **Merge** `develop` → **`main`** ; tag `v0.2-tls` | `main` ← `develop` | CHABIRA Ahmed |
| 57 | [x] | Créer **`feature/etape3-ed25519-auth`** depuis `develop` | `feature/etape3-ed25519-auth` | CHABIRA Ahmed |
| 58 | [x] | **Étape 3** : paires clés **ED25519** PEM ; **CLE 113** octets dans inscription (vraies clés) | `feature/etape3-ed25519-auth` | CHABIRA Ahmed |
| 59 | [x] | Session TCP : **sauf inscription**, échange **auth** client puis réponse serveur (signatures) | `feature/etape3-ed25519-auth` | CHABIRA Ahmed |
| 60 | [x] | Message **CODEREQ 0** : ID, NUM 4 octets BE, LSIG, SIG (signature des 3 premiers champs) | `feature/etape3-ed25519-auth` | CHABIRA Ahmed |
| 61 | [x] | Compteur **NUM** : 0 puis +1 à chaque nouvelle connexion authentifiée | `feature/etape3-ed25519-auth` | CHABIRA Ahmed |
| 62 | [x] | Aligner signatures sur le **cours** | `feature/etape3-ed25519-auth` | SERROUKH Jabir |
| 63 | [x] | Client : enchaînement TLS + auth + requête métier sur même connexion | `feature/etape3-ed25519-auth` | SERROUKH Jabir |
| 64 | [x] | **Merge** `feature/etape3-ed25519-auth` → **`develop`** | `develop` ← merge | AJINOU Abderrahman (chef de projet) |
| 65 | [x] | **Merge** `develop` → **`main`** ; tag `v1.0-sujet-complet` (ou équivalent) | `main` ← `develop` | AJINOU Abderrahman (chef de projet) |
| 66 | [x] | Relecture finale checklist complète + démo **soutenance** (mode verbeux) | `main` / `develop` | AJINOU Abderrahman (chef de projet) |
| 67 | [x] | Vérifier **historique Git** : commits visibles pour chaque membre (éviter note 0) | toutes branches | CHABIRA Ahmed |
| 68 | [x] | Si bug sur `main` : branche **`hotfix/<sujet>`** depuis `main`, corriger, merger dans **`main`** **et** **`develop`** | `hotfix/*` | CHABIRA Ahmed |

---

## Correspondance rapide checklist → lignes du tableau

| Thème (checklist) | Lignes N° dans ce tableau |
|-------------------|-----------------------------|
| Contexte & objectif | 10 |
| Fonctionnalités (vue liste) | 11, 27–33, 39 |
| Règles métier (pseudo, nom groupe, IDG, admin) | 15, 17–18, 22 |
| Échanges TCP (toutes sections) | 14, 16, 18–26 |
| Cas erreurs + 31 | 34–36 |
| Notifications mcast / UDP | 37–39 |
| Conventions + formats synthèse | 12–13, 16–26, 47 |
| Version sans sécu (étape 1) | 11, 9–50 |
| Sécurité TLS + ED25519 + 0 | 52–63 |
| Travail demandé (mail, C, qualité, plagiat, crédits) | 1–2, 7–8, 40–43, 66–67 |
| Organisation 3 étapes | 11, 52–63 |
| Modalités rendu (GitLab, authors, Makefile, soutenance) | 3–7, 66 |
| Vérif finale | 47–49 |
| Suivi type PRCursor (étape 1 faite, suite TLS/auth) | 44–65 |

---

*Branche `—` = hors Git ou avant premier commit. Utiliser la colonne **Fait** selon la règle de cochage ci-dessus.*
