#!/usr/bin/env bash
# CHRONO N°65 — merge develop → main + tag v1.0-sujet-complet
set -euo pipefail
git fetch origin
git checkout main
git pull origin main
git merge --no-ff develop -m "Merge develop into main — sujet PR6 complet (CHRONO N°65)"
git push origin main
git tag -a v1.0-sujet-complet -m "PR6 — Paroles : étapes 1–3 complètes"
git push origin v1.0-sujet-complet
echo "main à jour ; tag v1.0-sujet-complet poussé."
