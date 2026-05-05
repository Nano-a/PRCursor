# Tâche 12 — Conventions fil : champ **ID**, entiers **BE**, champs **ZEROS**

**CHRONO N°12** | Branche : **`feature/etape1-proto-tcp`** | Qui : **M1, M2**

## Contenu

Les identifiants (**ID**, **IDG**, **NUMB**, etc.) sont encodés en **uint32** ou **uint16** **big-endian** sur le fil. Les zones **ZEROS** du sujet sont produites avec `memset` / `wire_put_zeros` ou vérifiées avec `wire_expect_zeros`.

## Fichiers extraits (`PRCursor`)

| Fichier ici | Source |
|-------------|--------|
| `extrait_wire.c` | `src/wire.c` — `wire_put_u16_be`, `wire_put_u32_be`, `wire_get_*_be`, `wire_put_zeros`, `wire_expect_zeros`. |
| `extrait_wire.h` | `src/wire.h` — déclarations. |

## Copier dans le dépôt fac

Remplacer ou fusionner avec `src/wire.c` et `src/wire.h` (chemins habituels).

## Vérification

- Tous les entiers multi-octets du protocole passent par ces helpers (pas de cast « little-endian » implicite).
