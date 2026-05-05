#!/usr/bin/env bash
# CHRONO N°50 — merge feature/etape1-tests-notifs → develop
set -euo pipefail
git fetch origin
git checkout develop
git pull origin develop
git merge --no-ff feature/etape1-tests-notifs -m "Merge branch 'feature/etape1-tests-notifs' into develop (CHRONO N°50)"
git push origin develop
echo "Merge N°50 poussé sur develop (vérifiez GitLab)."
