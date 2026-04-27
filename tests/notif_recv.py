#!/usr/bin/env python3
"""Réception notif UDP / multicast IPv6 ou envoi TCP invalide (N°49)."""
import socket
import struct
import sys


def recv_udp(port: int, timeout: float) -> tuple[int, int]:
    s = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(("::", port))
    s.settimeout(timeout)
    data, _ = s.recvfrom(256)
    s.close()
    if len(data) < 6:
        raise SystemExit("udp short")
    code = struct.unpack("!H", data[0:2])[0]
    idg = struct.unpack("!I", data[2:6])[0]
    return code, idg


def recv_mcast(maddr: str, port: int, timeout: float) -> tuple[int, int]:
    s = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(("::", port))
    group = socket.inet_pton(socket.AF_INET6, maddr)
    mreq = group + struct.pack("@I", 0)
    s.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_JOIN_GROUP, mreq)
    s.settimeout(timeout)
    data, _ = s.recvfrom(256)
    s.close()
    if len(data) < 6:
        raise SystemExit("mcast short")
    code = struct.unpack("!H", data[0:2])[0]
    idg = struct.unpack("!I", data[2:6])[0]
    return code, idg


def tcp_bad(host: str, port: int) -> int:
    s = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
    s.connect((host, port))
    s.send(bytes([99]))
    buf = s.recv(32)
    s.close()
    if not buf:
        raise SystemExit("tcp empty")
    return buf[0]


def main() -> None:
    if len(sys.argv) < 2:
        raise SystemExit("usage: notif_recv.py udp PORT | mcast ADDR PORT | tcp_bad HOST PORT")
    mode = sys.argv[1]
    if mode == "udp":
        port = int(sys.argv[2])
        timeout = float(sys.argv[3]) if len(sys.argv) > 3 else 2.0
        c, g = recv_udp(port, timeout)
        print(c, g)
    elif mode == "mcast":
        maddr = sys.argv[2]
        port = int(sys.argv[3])
        timeout = float(sys.argv[4]) if len(sys.argv) > 4 else 2.0
        c, g = recv_mcast(maddr, port, timeout)
        print(c, g)
    elif mode == "tcp_bad":
        host = sys.argv[2]
        port = int(sys.argv[3])
        b = tcp_bad(host, port)
        print(b)
    else:
        raise SystemExit("bad mode")


if __name__ == "__main__":
    main()
