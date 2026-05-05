#!/usr/bin/env bash
# CHRONO N°54 — CA + certificat serveur (tests TLS locaux / UFR)
set -euo pipefail
DIR="${1:-tests/fixtures/certs}"
mkdir -p "$DIR"
cd "$DIR"
openssl genrsa -out ca.key 4096 2>/dev/null
openssl req -new -x509 -days 3650 -key ca.key -out ca.pem -subj "/CN=Paroles-Test-CA/O=PR6" 2>/dev/null
openssl genrsa -out server.key 2048 2>/dev/null
openssl req -new -key server.key -out server.csr -subj "/CN=localhost/O=PR6" 2>/dev/null
openssl x509 -req -days 3650 -in server.csr -CA ca.pem -CAkey ca.key -CAcreateserial -out server.pem 2>/dev/null
rm -f server.csr
echo "certificats dans $(pwd) : ca.pem server.pem server.key"
