#!/usr/bin/env bash
# CHRONO N°68 — après commits sur la branche hotfix : merger dans main puis synchroniser develop
# Prérequis : git checkout -b hotfix/... depuis main, correctifs commités.
# Usage : ./extrait_git_hotfix_n68.sh hotfix/nom-du-fix
set -euo pipefail
HOTFIX="${1:?usage: $0 hotfix/<sujet>}"
git fetch origin
git checkout main
git pull origin main
git merge --no-ff "$HOTFIX" -m "hotfix: merge $HOTFIX (CHRONO N°68)"
git push origin main
git checkout develop
git pull origin develop
git merge --no-ff main -m "sync develop after $HOTFIX (CHRONO N°68)"
git push origin develop
echo "main et develop à jour avec $HOTFIX."
