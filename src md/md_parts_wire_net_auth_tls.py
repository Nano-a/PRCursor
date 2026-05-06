# -*- coding: utf-8 -*-
"""Jeux PARTS pour wire.md, net.md, auth_ed25519.md, tls_io.md — import par _gen_src_md.py"""

PARTS_WIRE = [
    (
        ("1–18", 18, "wire_put_* BE", "Écrire u8,u16,u32 dans un tampon mémoire."),
        "Tu **construis un paquet** Paroles comme une **liste d’octets** dans un tableau C. Chaque fois que le cours impose **grand-boutiste (big-endian)** sur 16 ou 32 bits, ces trois **`wire_put_*`** avancent le curseur **`p`** (**double pointeur**).",
        "Chaque fois que `server.c` ou `client.c` **encode un message avant `conn_writen`**.",
        "Avance **`p`** de 1 / 2 / 4 octets avec les valeurs bien rangées (**MSB first** pour 16 et 32).",
        "Écrit octet **`v`** puis pour u16/u32 découpe bitwise `>>` puis masque `0xff`.",
        [
            ("wire_put_u8", "1 octet brut", "**p avance**, stocke v."),
            ("wire_put_u16_be", "16 bits BE", "**p += 2** octets haut puis bas."),
            ("wire_put_u32_be", "32 bits BE", "**p += 4** même idée cours."),
        ],
    ),
    (
        ("20–42", 23, "wire_get_*", "Lire depuis buffer + compteur `left`."),
        "Maintenant **le sens inverse** (**décoder**) : tu reçois un **bout de paquet** déjà dans la RAM après lecture TCP (**`resp` cours**) et **`left`** dit **combien d’octets il reste valides**. **Si quelqu’un tronque** le paquet (**left trop petit**), ces fonctions retournent **-1 au lieu d’inventer une valeur** (**garde cours** TP). Tu avances **`p`** et décrémentes **`left`** comme le polycopié pour **parcourir le fil wire** ligne par ligne.",
        "Parsage des réponses (`REG_OK`, `NEW_GROUP_OK`, liste invitations…) après lecture TCP (**`conn_readn` / lecture complète cours**).",
        "**`out`** reçoit valeur ou **erreur `-1`** ; **`p`** et **`left`** mis à jour quand ça passe.",
        "Lit 1 / 2 / 4 octets ; compare **`*left`** ; reconstruits shifts OR.",
        [
            ("wire_get_u8", "Lecture 8 bits", "**left≥1 sinon -1**, avance curseur."),
            ("wire_get_u16_be", "Lecture 16 bits BE", "**left≥2 sinon -1**."),
            ("wire_get_u32_be", "Lecture 32 bits BE", "**left≥4 sinon -1**."),
        ],
    ),
    (
        ("44–55", 12, "wire_put_zeros, wire_expect_zeros", "Padding / zones à zéro."),
        "**Deux aides** très **fréquentes** quand cours impose **pseudo fixe bourré zéros** ou **`PAROLES_INV_PAD`**. **`wire_put_zeros`** remplit vite une zone (**comme crayon tampon noir** mémoires). **`wire_expect_zeros`** **vérifie** qu’en lecture **couche reste encore zéros** (**sinon -1**) — très **pratique** pour **INVITE cours** où **zones réservées** doivent **rester neutres**.",
        "Utilisées partout inscription sans vraie clé (clé bourrées), invites avec padding lisible cours.",
        "Écrit un bloc de **`n`** zéros **ou** vérifie consommation **`n`** zéros et avance **`p`**.",
        "memset + avance **`p`** ; boucle **`for`** **compare octet après octet lors parse.",
    ),
]


PARTS_NET = [
    (
        ("1–8", 8, "En-têtes + macro timeout", "`net.h` / includes."),
        "Ce fichier **`net.c`** branche **`net.h`** où **timeout TCP standard** cours **`PAROLES_TCP_TIMEOUT_MS`** vit avec prototypes **IPv6** sockets. Compilation seule chose exécution — instant **compilation** projet.",
        "Inclusion avant toute fonction transport.",
        "**Constante lecture client TLS** commune.",
        "**#include sys/socket polli netinet** etc définitions.",
    ),
    (
        ("9–54", 46, "tcp6_listen | accept | connect", "TCP IPv6 cours."),
        "Moment **terminal serveur** : `./paroles_server` **doit être à l’écoute** (**bind + listen**) sur **`::`/ `::1` + port cours**. Moment **terminal client** : **`paroles_client host port`** appelle **`tcp6_connect`**. **`tcp6_accept`** remet le **nouveau descripteur** à **`serve_client`**.",
        "Serveur `main` puis client `one_cmd_*` boucles.",
        "Descripteurs entiers fichiers (**fd**) utilisables ensuite **read/write** ou **TLS**.",
        "**socket AF_INET6** ; erreurs fermetures **close** puis **`-1`** ; **`accept`** simple wrapper cours.",
        [
            ("tcp6_listen", " Écouter adresse cours", "**bind+listen`** ou fermet."),
            ("tcp6_accept", "Nouveau client", "**accept** POSIX."),
            ("tcp6_connect", "Connexion cliente", "**connect** vers **`host`** **port htons** cours."),
        ],
    ),
    (
        ("56–83", 28, "writen | readn", "Flux octets fiable avec timeout."),
        "**TCP** transporte bien les octets, mais un seul **`write`** peut être **court** → **`writen` boucle** jusqu’à épuisement (**idiome POSIX**). Pour lire **`n`** octets sans rester aveugle, **`readn` combine `poll`** + **`read`** + **timeout** (cf. fichier : **lecture fragile / connexion fermée**).",
        "**`conn_writen` sans TLS fallback** **`writen`**, **`conn_readn` sans TLS utilise `readn`.** ",
        "**`writen`** finit bloc complet ; **`readn`** accumule fragments **avec poll** timeouts.",
        "while (**left**) **write/read** jusqu’à zéro restant sinon **`-1`**.",
        [
            ("writen", "Écrit tout", "**while write** jusqu’à **`n`** octets cours."),
            ("readn", "Lit tout tout en timeout poll", "**poll POLLIN puis read** jusqu’à **`n`**."),
        ],
    ),
    (
        ("85–96", 12, "udp6_send | udp6_recv", "Datagrammes IPv6 + poll."),
        "UDP **pour les notifs cours** très **court** (**6 octets** code + **`idg`**) envoyés **en multicast ou en unicast**. **`udp6_send`** vérifie que **`sendto`** a bien tout envoyé. **`udp6_recv`** utilise **`poll`** avec **timeout** (ex. client **`listen_udp`** qui attend une **notif d’invite** sur **PORTUDP**).",
        "Serveur **`notif_mcast`** **`notif_udp_user`** cliente **`cmd_listen_udp`** cours.",
        "Retour **`0`** envoi bon ou **`int` octets lu** receptions.",
        "**sendto** **recvfrom** **poll** cours.",
        [
            ("udp6_send", "Envoyer datagrammes", "**sendto`** compare taille cours."),
            ("udp6_recv", "Réception timeout", "**poll** puis **recvfrom**."),
        ],
    ),
    (
        ("98–121", 24, "join_mcast | udp6_bind_any", "Réception groupe multicast cours."),
        "Pour **écouter salons multicast** cours (**adresse façon ff0x::**/ports), programme **JOIN_GROUP IPv6 cours** après **bind UDP port groupes** facultatif (`udp6_bind_any` **reuse** cours). Tu te places **mémoire cours** après **`NEW_GROUP`** retours **clients** veulent **voir petits pings radio multicast** salons.",
        "Client **`listen_mcast`**, serveur constructions **notifications multicast** cours.",
        "Socket UDP **bind** puis **abonnement** au groupe.",
        "**setsockopt(IPV6_JOIN_GROUP)** après **`udp6_bind_any`** sur le **port multicast** cours.",
        [
            ("join_mcast", "Joindre le groupe IPv6", "`struct ipv6_mreq` + `setsockopt`."),
            ("udp6_bind_any", "Prise UDP any", "**socket SOCK_DGRAM** **bind in6addr_any** cours."),
        ],
    ),
    (
        ("123–137", 15, "udp6_mcast_recv_socket", "Conveniences écouter multicast cours."),
        "Helper **combinant lignes cours** précédentes : **traduit textual IPv6** **`mcast_ipv6`**, puis **réutilise **`udp6_bind_any`**, puis **`join_mcast` cours**. **Une ligne API** niveau cours pour clients commandes **`listen_mcast`** **sans épeler trois étapes** à chaque démo cours.",
        "Client **`listen_mcast` terminal cours** après réception multicast **nouveau salons** cours.",
        "Retour **`fd`** prêt **`udp6_recv` cours.**",
        "**`inet_pton` → bind → join**, sinon **`close`** en cas d’erreur.",
    ),
]


PARTS_AUTH = [
    (
        ("1–5", 5, "En-tête", "EVP Ed25519 message brut cours."),
        "Alignement cours **signature Ed25519** via **EVP OpenSSL cours** (**messages bruts cours** niveau fichier commentaires cours). Ces fonctions **`paroles_*` permettents charger clés PEM binaire raw sign verify client serveur cours option **`--signing-key` cours** facultatif.",
        "Optionnel démarrage serveur cliente **avec fichiers PEM** cours.",
        "Expose loaders sign verify cours.",
        "**include auth header + openssl PEM stdio cours** ",
    ),
    (
        (
            "6–28",
            23,
            "load PEM private | public",
            "Charger clés disque cours.",
        ),
        "**Quand** le serveur est lancé avec **`--signing-key fichier.pem`** (ou que le client charge **`--server-pub`**), ces fonctions **ouvrent le PEM**, vérifient que c’est bien **Ed25519**, et renvoient une **`EVP_PKEY*`** exploitable.",
        "**Initialisation** dans `main` ou après parsing **`argv`** côté client.",
        "**Retour** pointeur **`EVP_PKEY`** ou **`NULL`** + libération si mauvais type.",
        "**fopen** ; **`PEM_read_PrivateKey` / PEM_read_PUBKEY** ; contrôle **`EVP_PKEY_ED25519`**.",
        [
            ("paroles_load_ed25519_private_pem", "Clé privée PEM", "**PEM_read_PrivateKey** + type."),
            ("paroles_load_ed25519_public_pem", "Clé publique PEM", "**PEM_read_PUBKEY** + type."),
        ],
    ),
    (
        ("30–32", 3, "paroles_ed25519_pubkey_from_cle", "113 octets → EVP public."),
        "**Inscription** : partie du client qui **fabrique une clé 113** depuis un PEM **doit aussi pouvoir signer** ensuite ; le serveur vérifie la **portion utile**. Ici **`paroles_ed25519_pubkey_from_cle`** reconstruit l’objet OpenSSL depuis **la zone déjà stockée en octets**.",
        "Utilisateurs **fill cle depuis PEM cours** cours.",
        "**`EVP_PKEY_new_raw_public_key`** avec **32 premiers octets** utiles (les autres sont **padding** selon ta branche cours).",
        "**Construire** une **`EVP_PKEY*`** depuis la **tronche 113** reçue à l’inscription.",
    ),
    (
        ("34–48", 15, "paroles_ed25519_sign", "Signer buffer cours."),
        "**Contexte fichier** : **EVP DigestSign avec digest `NULL`** (Ed25519 sur **message brut**). Le client utilise ça avant d’envoyer **AUTH** quand **`--key`** fournit la PEM privée.",
        "Préparation du paquet **AUTH** après lecture du **`nonce`** stocké **`/tmp/paroles_nonce_`** …",
        "Signature cours écrite buffer **`sig` avec longueur **`siglen` cours.**",
        "**EVP_MD_CTX_new SignInit DigestSign cours frees ctx cours** erreurs cours",
        [
            ("EVP_MD_CTX_new", "Context cours", "ctx OpenSSL cours"),
            ("EVP_DigestSignInit(...,NULL,...)", "Mode brut Ed25519 cours", "**NULL digest cours niveau cours**"),
            ("EVP_DigestSign", "Octets cours", "écrit cours signature."),
        ],
    ),
    (
        ("50–61", 12, "paroles_ed25519_verify", "Vérifier signature cours."),
        "Vérifications **AUTH ↔ AUTH_OK** : le serveur et le client s’échangent des **messages courts** puis **vérifient** la signature adverse avec **`EVP_PKEY` opposée** (**anti-usurpation**).",
        "Chemins cours **AUTH / AUTH_OK** cours.",
        "Retours **`0` ok **`-1` refuse cours.**",
        "**EVP_DigestVerifyInit Verify cours frees ctx cours** compare ok cours",
        [
            ("EVP_MD_CTX_new", "Context cours", "ctx verify cours"),
            ("EVP_DigestVerify", "Compare cours", "**==1 cours** succes cours"),
        ],
    ),
]


PARTS_TLS = [
    (
        ("1–5", 5, "En-têtes", "Bridger OpenSSL avec net cours."),
        "Relie fichiers cours **`tls_io.h` cours** définitions cours avec **`conn_readn` cours** cours — include **`net.h` cours poll unistd cours permettentes boucles cours **sans dupliquer** lowlevel POSIX cours niveau cours.",
        "Compilation cours modules serveur cliente TLS cours ",
        "**Prototypes cours OpenSSL cours** cours.",
        "**include cours** ",
    ),
    (
        ("6–18", 13, "paroles_tls_server_ctx", "Context serveur cours."),
        "Quand prof **`--tls cert key** cours `./paroles_server` cours** charge certificats PEM cours configure **TLS≥1 cours** niveau cours — **checks private key cours** cours cohérences cours cours.",
        "Serveur `main cours` initialisations cours.",
        "**SSL_CTX* cours cours** ou erreur cours fermet cours.",
        "**SSL_CTX_new TLS_server_method cours Certificates PrivateKey cours** check cours cours",
        [
            ("SSL_CTX_use_certificate_file", "Charge le cert PEM", "fichier + type PEM."),
            ("SSL_CTX_use_PrivateKey_file", "Charge la clé privée", "fichier + type PEM."),
            ("SSL_CTX_check_private_key", "Vérifie la paire cert/clé", "retour booléen OpenSSL."),
        ],
    ),
    (
        ("20–31", 12, "paroles_tls_client_ctx", "Context client cours CA."),
        "Cliente **`paroles_client --tls ca.pem cours` charge magasin confiance cours** avec **verification peer cours** cours — **vérif cours** certificats serveur cours pendant **`SSL_connect` cours**.",
        "Arguments **`--tls` cliente cours**.",
        "**SSL_CTX* cours** chargé CA cours.",
        "**SSL_CTX_load_verify_locations cours set_verify PEER cours** TLS1_2 minimum cours",
    ),
    (
        ("33–35", 3, "paroles_tls_ctx_free", "Libération mémoire cours."),
        "Petit wrapper **`SSL_CTX_free` cours niveau cours** pour **`atexit cours` fichier **`client cours` fermet contexts cours** cours propres fermet cours.",
        "Fin cours processus cours ou erreurs cours init cours.",
        "Libération OpenSSL cours.",
        "if ctx SSL_CTX_free cours",
    ),
    (
        (
            "37–93",
            57,
            "conn_readn | conn_read_upto",
            "Lecture TLS ou clair cours.",
        ),
        "**Cœur pratiques cours TLS** cours : si **`ssl` NULL cours** alors **delegue `readn` net cours timeouts POSIX cours sinon boucle `SSL_read` cours** gères **`WANT_READ` WANT_WRITE cours** cours avec **`poll` cours timeouts cours** — comportements non bloquantes cours cours. **`conn_read_upto` cours** usages messages **dont tail fin variable cours** cours — **boucle cours** jusqu'à timeout cours ou cap cours ou **SSL_ERROR_ZERO_RETURN cours**.",
        "Serveur **`serve_business_switch` cliente `one_cmd_*` cours**.",
        "Octets lus buffer cours retour longueur cours.",
        "**SSL_read poll SSL_ERROR cours** ou **read fallback cours**.",
        [
            ("conn_readn", "Exactement n octets cours", "**SSL_read loop WANT_* cours**"),
            ("conn_read_upto", "Jusqu’à capacité ou fin", "**SSL_ERROR_ZERO_RETURN** ou timeout."),
        ],
    ),
    (
        ("95–120", 26, "conn_writen", "Écriture TLS ou net cours."),
        "**Symétrique écrit cours** cours : **`SSL_write` cours retries WANT_WRITE WANT_READ cours** avec **`poll` cours timeouts `PAROLES_TCP_TIMEOUT_MS` cours** cours — fallback **`writen` cours lorsque pas TLS cours**.",
        "Toutes réponses serveur cours messages cliente cours cours.",
        "Envoie intégralité buffer cours cours.",
        "**SSL_write** en boucle avec **`poll`** (**WANT_WRITE / WANT_READ**) ou **`writen`** si **`ssl`** absent.",
    ),
]
