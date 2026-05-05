#!/usr/bin/env bash
# CHRONO N°57 — branche feature/etape3-ed25519-auth depuis develop
set -euo pipefail
git fetch origin
git checkout develop
git pull origin develop
git checkout -b feature/etape3-ed25519-auth
git push -u origin feature/etape3-ed25519-auth
echo "Branche feature/etape3-ed25519-auth créée et poussée."
