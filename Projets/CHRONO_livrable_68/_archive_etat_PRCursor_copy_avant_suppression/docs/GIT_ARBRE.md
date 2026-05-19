# Graphe Git (identique à la commande `git log --graph --all --decorate --oneline`)

Ce fichier reproduit l’historique tel qu’affiché en terminal. La page **Network** de GitHub ne dessine pas le même graphe ; utilisez ce fichier ou la commande ci-dessous pour la vue complète.

Régénérer après nouveaux commits :
```bash
git log --graph --all --decorate --oneline
```

---

*   a482351 (HEAD -> main, tag: v1.0-sujet-complet, origin/main, origin/HEAD) merge(CHRONO N°65): develop → main, sujet complet TLS+ED25519
|\  
| *   86b68b0 (origin/develop, develop) merge(CHRONO N°64): feature/etape3-ed25519-auth → develop
| |\  
| | * 1842ee2 (origin/feature/etape3-ed25519-auth, feature/etape3-ed25519-auth) feat(CHRONO): N°59–63 auth ED25519, TLS+métier, docs N°66–68
| | * 8c117f7 feat(CHRONO): N°58 inscription CLE 113 depuis clé publique ED25519 PEM
| | * 17e3bf1 git(CHRONO): N°57 branche feature/etape3-ed25519-auth
| |/  
* | 943e7f8 (tag: v0.2-tls) git(CHRONO): N°56 merge develop → main (TLS)
|\| 
| *   1ba2915 git(CHRONO): N°55 merge feature/etape2-tls → develop
| |\  
| | * 4db64b6 (origin/feature/etape2-tls, feature/etape2-tls) feat(CHRONO): N°52–54 TCP TLS OpenSSL (--tls), gencerts.sh, tls_smoke
| |/  
* | 1bccd60 (tag: v0.1-etape1) git(CHRONO): N°51 merge develop → main (étape 1)
|\| 
| *   b55c3dc git(CHRONO): N°50 merge feature/etape1-tests-notifs → develop
| |\  
| | * 20bcbab (origin/feature/etape1-tests-notifs, feature/etape1-tests-notifs) test(CHRONO): N°49 notifs 18/22 + erreur TCP 31 (python3)
| | * 39405ac test(CHRONO): N°48 ordre fil 16/17 (scénario type exemple PDF)
| | * 492f692 test(CHRONO): N°47 vérification statique CODEREQ 1–17, 24, 31
| | * 499252c test(CHRONO): N°46 scénarios CODEREQ + make test (smoke + régression)
| | * 5ae7100 git(CHRONO): N°45 création branche feature/etape1-tests-notifs depuis develop
| |/  
| *   47e3448 Merge branch 'feature/etape1-proto-tcp' into develop (CHRONO N°44)
| |\  
| | * fc46f5a (feature/etape1-proto-tcp) CHRONO N°42: doc mode verbeux
| | * 2f1cba9 CHRONO N°41: doc timeouts TCP
| | * 2c7a1a5 CHRONO N°40: doc gestion erreurs réseau
| | * 99927fa CHRONO N°39: doc notifications multicast/UDP
| | * 63e0339 CHRONO N°38: doc erreur 31 étendue -v
| | * 671b469 CHRONO N°37: doc rejets serveur (commentaire)
| | * 0c3c232 CHRONO N°36: tests smoke.sh
| | * 26ce6e7 CHRONO N°35: client — usage et main
| | * 3c6ac74 CHRONO N°34: client — reply, feed
| | * 51339e7 CHRONO N°33: client — listmem, post
| | * 36f8b87 CHRONO N°32: client — listinv, ans
| | * 7fcb770 CHRONO N°31: client — reg, newgroup, invite
| | * ccaeeaf CHRONO N°30: serveur — main
| | * 231eb72 CHRONO N°29: serveur — serve_client
| | * 821ac8a CHRONO N°28: serveur — dispatch
| | * 0d07f84 CHRONO N°27: serveur — fil (n,r)
| | * 4d003dd CHRONO N°26: serveur — répondre
| | * 3caf3ef CHRONO N°25: serveur — poster
| | * 55b962d CHRONO N°24: serveur — liste membres
| | * e25a733 CHRONO N°23: serveur — réponse invitation / quitter
| | * f93c59b CHRONO N°22: serveur — liste invitations
| | * 9c0e647 CHRONO N°21: serveur — invitation
| | * 1d523de CHRONO N°20: serveur — création groupe
| | * 2e72240 CHRONO N°19: serveur — close_group + inscription
| | * 03235a0 CHRONO N°18: serveur — structures jusqu'à find_post
| | * 743a429 CHRONO N°17: net.c + Makefile objets
| | * e473ceb CHRONO N°16: net.h
| | * ce1f653 CHRONO N°15: wire.c
| | * 6d0a3ef CHRONO N°14: wire.h
| | * 11387ec CHRONO N°13: formats CODEREQ complets (en-tête)
| | * 535347e CHRONO N°12: conventions — premiers CODEREQ
| | * d26187f CHRONO N°11: étape 1 — constantes CLE / NOM
| | * c43d713 CHRONO N°10: objectif IPv6 — début en-tête protocole
| |/  
| * e47f295 CHRONO N°9: doc branches feature/etape1-proto-tcp
| * 7f62262 CHRONO N°8: CREDITS et README
| * 833f40c CHRONO N°7: Makefile + binaires stubs compilables
| * d3c9a11 CHRONO N°6: option protection branche main GitLab
|/  
| * 02c235a (backup/linear-main-20260427) CHRONO N°42: doc mode verbeux
| * ff44434 CHRONO N°41: doc timeouts TCP
| * ebe905d CHRONO N°40: doc gestion erreurs réseau
| * caf1961 CHRONO N°39: doc notifications multicast/UDP
| * 8445b42 CHRONO N°38: doc erreur 31 étendue -v
| * 768322c CHRONO N°37: doc rejets serveur (commentaire)
| * f85f118 CHRONO N°36: tests smoke.sh
| * f9c8e32 CHRONO N°35: client — usage et main
| * d0e9a76 CHRONO N°34: client — reply, feed
| * 8b3e01d CHRONO N°33: client — listmem, post
| * dcc67ad CHRONO N°32: client — listinv, ans
| * 2db0063 CHRONO N°31: client — reg, newgroup, invite
| * 4d39c24 CHRONO N°30: serveur — main
| * fd47d49 CHRONO N°29: serveur — serve_client
| * 6394b9b CHRONO N°28: serveur — dispatch
| * 81ac465 CHRONO N°27: serveur — fil (n,r)
| * 3334f35 CHRONO N°26: serveur — répondre
| * 5faa594 CHRONO N°25: serveur — poster
| * a7f115c CHRONO N°24: serveur — liste membres
| * 84b42b1 CHRONO N°23: serveur — réponse invitation / quitter
| * dc30534 CHRONO N°22: serveur — liste invitations
| * aedaa0c CHRONO N°21: serveur — invitation
| * 5d4f323 CHRONO N°20: serveur — création groupe
| * a2d7e2a CHRONO N°19: serveur — close_group + inscription
| * b8dc1fa CHRONO N°18: serveur — structures jusqu'à find_post
| * f838a25 CHRONO N°17: net.c + Makefile objets
| * 10de2a9 CHRONO N°16: net.h
| * fb892b5 CHRONO N°15: wire.c
| * 8a44c1b CHRONO N°14: wire.h
| * d2d1229 CHRONO N°13: formats CODEREQ complets (en-tête)
| * 8ff211e CHRONO N°12: conventions — premiers CODEREQ
| * 119a8df CHRONO N°11: étape 1 — constantes CLE / NOM
| * d61ece7 CHRONO N°10: objectif IPv6 — début en-tête protocole
| * fbf48e2 CHRONO N°9: doc branches feature/etape1-proto-tcp
| * 8a29be6 CHRONO N°8: CREDITS et README
| * c9b2957 CHRONO N°7: Makefile + binaires stubs compilables
| * 72356c1 CHRONO N°6: option protection branche main GitLab
|/  
* 3a7e927 chore: premier commit — README, .gitignore, squelette, authors.md
* 6a6eb06 (origin/feature/etape1-proto-tcp) feat(client): CLI interactive, binaire smoke et script tests/smoke.sh
* da2bbff feat(server): serveur Paroles IPv6 (TCP, UDP/mcast notifications, étape 1 sans TLS)
* 5a74413 feat(proto): en-têtes protocole et utilitaires wire/net (big-endian, timeouts)
* f3ff536 chore: Makefile, docs/BRANCHES et README complet (CHRONO N°7–8)
* b794353 chore: dépôt initial — README, authors, gitignore, workflow SVG (CHRONO N°4)
