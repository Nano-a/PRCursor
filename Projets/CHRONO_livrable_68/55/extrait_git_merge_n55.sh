#!/usr/bin/env bash
# CHRONO N°55 — merge feature/etape2-tls → develop
set -euo pipefail
git fetch origin
git checkout develop
git pull origin develop
git merge --no-ff feature/etape2-tls -m "Merge branch 'feature/etape2-tls' into develop (CHRONO N°55)"
git push origin develop
echo "Merge N°55 poussé sur develop."
