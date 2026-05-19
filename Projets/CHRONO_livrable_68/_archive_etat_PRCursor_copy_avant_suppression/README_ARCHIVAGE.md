# Archive — état de **PRCursor copy** avant suppression (2026)

Ce répertoire est une **copie figée** (sans historique `.git`) du dépôt de travail **`PRCursor copy`**, au moment où il a été vidé volontairement : il servait de **bac à sable de vérification** des tâches CHRONO (retraits ciblés pour mettre en évidence ce qui manque par rapport au livrable).

## Où se trouve la « vraie » source ?

- **Code complet et opérationnel** : dépôt local **`PRCursor/`** (même arbre parent que ce projet).
- **Consignes et extraits par tâche (1–68)** : dossiers voisins **`1/`** … **`68/`** dans `Projets/CHRONO_livrable_68/`.
- **Handoff équipe** : `../CHRONO_LIVRABLE_EQUIPE_HANDOFF.md`

## Contenu typique ici

- Fichiers **stubs** ou tronqués, commentaires `CHRONO N°…` dans `include/paroles_proto.h`, etc.
- Binaires **`paroles_client`** / **`paroles_server`** s’ils avaient été générés dans le copy (référence d’époque seulement).
- Scripts de test en **`exit 1`**, `Makefile` minimal, etc.

## Usage

Ne pas **fusionner** aveuglément ce dossier dans le dépôt fac : s’y reporter uniquement pour comprendre *ce qui a été retiré* côté copy. Recoller le code se fait depuis **`PRCursor`** et les **extraits du livrable 68** en suivant chaque `README.md` de tâche.
