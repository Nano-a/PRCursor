#!/usr/bin/env bash
# CHRONO N°44 — merge feature/etape1-proto-tcp → develop (--no-ff)
set -euo pipefail
git fetch origin
git checkout develop
git pull origin develop
git merge --no-ff feature/etape1-proto-tcp -m "Merge branch 'feature/etape1-proto-tcp' into develop (CHRONO N°44)"
git push origin develop
