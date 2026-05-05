#!/usr/bin/env bash
# CHRONO N°67 — contrôle rapide des auteurs de commits (à lancer à la racine du dépôt Git du projet)
set -euo pipefail
BR="${1:-main}"
echo "=== shortlog ($BR) ==="
git shortlog -sne "$BR" || true
echo ""
echo "=== derniers commits ($BR) ==="
git log "$BR" -15 --format='%h  %an  %s'
