#!/usr/bin/env bash
# CHRONO N°52 — branche feature/etape2-tls depuis develop
set -euo pipefail
git fetch origin
git checkout develop
git pull origin develop
git checkout -b feature/etape2-tls
git push -u origin feature/etape2-tls
echo "Branche feature/etape2-tls créée et poussée."
