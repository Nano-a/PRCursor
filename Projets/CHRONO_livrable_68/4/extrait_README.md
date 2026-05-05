# PR6 — Paroles

Projet réseau **IPv6** (protocole Paroles) — client et serveur en C : TCP, **TLS** (OpenSSL), authentification **ED25519** (CODEREQ 0 / AUTH_OK 25) sur la même session que le métier après l’inscription.

## Compilation

```bash
make
```

Binaires : `paroles_server`, `paroles_client`.

## Exemple (plain TCP, sans TLS)

Terminal 1 :

```bash
./paroles_server :: 4242
```

Terminal 2 :

```bash
./paroles_client ::1 4242 reg Alice
./paroles_client ::1 4242 newgroup 1 MonGroupe
```
