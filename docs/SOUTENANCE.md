# Soutenance PR6 — démo et relecture (CHRONO N°66)

## Avant la séance

- Relire `Projets/checklist_projet_PR6.md` et le sujet PDF : tous les CODEREQ, notifications 18–23, erreur 31.
- Vérifier `make` et `make test` sur une machine proche des **salles UFR** (IPv6, OpenSSL, `python3` pour les tests notifs).
- Préparer un terminal avec `./paroles_server -v …` pour montrer le **mode verbeux** (journal des inscriptions, erreurs auth, etc.).

## Déroulé type (étape 3)

1. Générer les certificats TLS : `scripts/gencerts.sh tests/fixtures/certs`.
2. Lancer le serveur avec TLS et clé de signature serveur :  
   `./paroles_server -v --tls …/server.pem …/server.key --signing-key …/srv/key.pem ::1 4242`
3. Inscription (sans auth préalable) :  
   `./paroles_client --tls …/ca.pem ::1 4242 reg Alice …/pub.pem`
4. Commande métier avec **TLS + CODEREQ 0** sur la **même** connexion TCP :  
   `./paroles_client -v --tls …/ca.pem --key …/alice/key.pem --server-pub …/srv/pub.pem ::1 4242 newgroup 1 MonGroupe`
5. Montrer un scénario complet (invitation, post, feed) et une **erreur 31** (requête invalide ou mauvais NUM).

## Livrables à avoir sous la main

- `authors.md` à jour, `README.md` / CREDITS, historique Git lisible pour chaque membre du trinôme.
