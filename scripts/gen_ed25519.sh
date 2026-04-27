#!/usr/bin/env bash
# CHRONO N°58 — paire ED25519 PEM (tests inscription CLE 113)
set -euo pipefail
DIR="${1:-tests/fixtures/ed25519}"
mkdir -p "$DIR"
openssl genpkey -algorithm ED25519 -out "$DIR/key.pem" 2>/dev/null
openssl pkey -in "$DIR/key.pem" -pubout -out "$DIR/pub.pem" 2>/dev/null
echo "ED25519 : $DIR/key.pem (priv) $DIR/pub.pem (pub)"
