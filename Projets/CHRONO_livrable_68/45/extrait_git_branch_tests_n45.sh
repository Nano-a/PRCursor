#!/usr/bin/env bash
# CHRONO N°45 — branche feature/etape1-tests-notifs depuis develop
set -euo pipefail
git checkout develop
git pull origin develop
git checkout -b feature/etape1-tests-notifs
git push -u origin feature/etape1-tests-notifs
