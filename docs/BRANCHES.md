# Branches Git — alignement `CHRONO_TABLEAU_PR6.md`

Ordre prévu (voir `Projets/CHRONO_TABLEAU_PR6.md` à la racine du dossier Reseau).

| Branche | Rôle | Quand |
|---------|------|--------|
| `main` | Version stable livrable (tags `v0.1-etape1`, etc.) | Après merge depuis `develop` |
| `develop` | Intégration continue du trinôme | Réception des `feature/*` |
| `feature/etape1-proto-tcp` | Pointeur sur l’état « proto + serveur + client » avant branche tests (commit `e16b224` ou équivalent) | Créée après N°9–43 |
| `feature/etape1-tests-notifs` | Tests non-régression, ordre fil PDF, réception UDP/mcast client | N°45–50 |
| `feature/etape2-tls` | TLS OpenSSL | N°52–55 |
| `feature/etape3-ed25519-auth` | Auth CODEREQ 0 + ED25519 | N°57–64 |
| `hotfix/*` | Correctif depuis `main` | N°68 |

Fusions obligatoires :  
`feature/etape1-proto-tcp` → `develop` (N°44) ;  
`feature/etape1-tests-notifs` → `develop` (N°50) ;  
`develop` → `main` (N°51, 56, 65).
