#!/usr/bin/env bash
# CHRONO N°56 — merge develop → main + tag v0.2-tls
set -euo pipefail
git fetch origin
git checkout main
git pull origin main
git merge --no-ff develop -m "Merge develop into main — étape 2 TLS (CHRONO N°56)"
git push origin main
git tag -a v0.2-tls -m "PR6 étape 2 — TCP + TLS OpenSSL"
git push origin v0.2-tls
echo "main à jour ; tag v0.2-tls poussé."
