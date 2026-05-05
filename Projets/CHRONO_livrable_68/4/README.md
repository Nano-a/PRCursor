# Tâche 4 — Premier commit : README, `.gitignore`, squelette, `authors.md`

**CHRONO N°4** | Branche : **`main`** | Qui : **M3** (+ relecture tous)

## Source des extraits

Projet complet de référence : `PRCursor/` (racine : `.../Reseau/PRCursor`).

## Fichiers fournis dans ce dossier

| Fichier ici | À copier dans le dépôt fac sous… | Action |
|-------------|-----------------------------------|--------|
| `extrait_gitignore` | `.gitignore` | Remplacer / créer **tout** le fichier avec le contenu de `extrait_gitignore`. |
| `extrait_authors.md` | `authors.md` | Remplacer / créer **tout** le fichier ; **compléter** le tableau avec les 3 membres. |
| `extrait_README.md` | `README.md` | Pour un **premier** commit minimal, vous pouvez partir de cet extrait puis l’enrichir plus tard (étapes TLS, tests, etc.). |

## Squelette du projet

Le sujet demande en plus un **squelette** (arborescence minimale : `src/`, `include/`, éventuellement binaires stubs). Ce premier commit ne se limite pas aux trois fichiers ci-dessus : ajoutez les fichiers vides ou stubs convenus par l’équipe **en même temps** que README / gitignore / authors.

Référence complète de structure : dépôt `PRCursor` (dossiers `src/`, `include/`, `Makefile` au **N°7**).

## Après copie

```bash
git add README.md .gitignore authors.md
# + fichiers du squelette
git commit -m "chore: premier commit — README, gitignore, authors, squelette (CHRONO N°4)"
git push origin main
```

## Vérification

- Le dépôt GitLab contient bien ces fichiers ; `authors.md` est **rempli** (pas de lignes vides pour les noms).
