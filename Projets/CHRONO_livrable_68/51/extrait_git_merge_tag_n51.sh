#!/usr/bin/env bash
# CHRONO N°51 — merge develop → main + tag v0.1-etape1 (optionnel)
set -euo pipefail
git fetch origin
git checkout main
git pull origin main
git merge --no-ff develop -m "Merge develop into main — étape 1 complète (CHRONO N°51)"
git push origin main
git tag -a v0.1-etape1 -m "PR6 étape 1 — proto TCP/IPv6, tests CODEREQ/notifs"
git push origin v0.1-etape1
echo "main à jour ; tag v0.1-etape1 poussé."
