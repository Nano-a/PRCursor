#!/usr/bin/env bash
# CHRONO N°64 — merge feature/etape3-ed25519-auth → develop
set -euo pipefail
git fetch origin
git checkout develop
git pull origin develop
git merge --no-ff feature/etape3-ed25519-auth -m "Merge branch 'feature/etape3-ed25519-auth' into develop (CHRONO N°64)"
git push origin develop
echo "Merge N°64 poussé sur develop."
