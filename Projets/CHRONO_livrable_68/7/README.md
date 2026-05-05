# Tâche 7 — Makefile + compilation sur machines UFR

**CHRONO N°7** | Branche : **`develop`** ou **`main`** | Qui : **M3**

## Source de l’extrait

Fichier : `PRCursor/Makefile` (lignes 1–32 dans la copie ci-dessous, état projet complet).

## Fichier fourni

| Fichier | Dans le dépôt fac |
|---------|-------------------|
| `extrait_Makefile` | Renommer / copier en **`Makefile`** à la racine du projet (remplacer tout le fichier). |

## Après copie

```bash
make clean
make
```

Tester **sur une machine UFR** (salles ou SSH) comme demandé dans les modalités de rendu.

## Attention

- Le Makefile actuel suppose la présence de tous les `.c` listés (`server.c`, `client.c`, `wire.c`, `net.c`, `tls_io.c`, `auth_ed25519.c`). Si votre squelette au **N°4** n’a pas encore ces fichiers, adaptez les règles **progressivement** jusqu’à ce que `make` réussisse.
- Flags : `-lssl -lcrypto` (OpenSSL) ; `-DPAROLES_ACCEPT_REAL_CLE_113` pour l’étape 3 (CLE réelle). Pour l’étape 1 seule, le sujet peut demander une variante sans OpenSSL : ajuster selon votre feuille de route.

## Vérification « c’est fait »

- `make` sans erreur sur UFR ; binaires `paroles_server` et `paroles_client` générés (ou stubs retirés une fois le code ajouté).
