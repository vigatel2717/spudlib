
#ifndef SPUDNET_H
#define SPUDNET_H

#include "spudcore.h"
#include <stdbool.h>
#include <stdint.h>

#if __cplusplus
extern "C" {
#endif

/*
 * SpudNet  —  thin TCP socket wrapper (Winsock2 / BSD sockets)
 *
 * Pure transport, no protocol — a translation of the platform socket API
 * (connect/listen/accept/send/recv/close), not a networking framework. It
 * moves bytes and nothing else. Message framing, snapshot-vs-delta
 * semantics, host authority, "what does this payload mean" — all of that
 * is the caller's problem (ApCAD/Erethal), the same way SpudGPU never
 * decides what to draw.
 *
 * send/recv mirror the underlying socket calls: either can transfer fewer
 * bytes than requested. A caller that wants "exactly N bytes or nothing"
 * loops on these itself; SpudNet won't do it for you.
 */

typedef struct spudnet_socket_t *spudnet_socket;

/* Must be called once before any other spudnet_ function (Winsock needs
 * WSAStartup); spudnet_shutdown tears down the same global state. Cheap,
 * call once at app init/teardown. */
SPUDRESULT spudnet_startup(void);
void spudnet_shutdown(void);

/*
 * Server side — the device that opens first and holds the scene in memory.
 */

/* Opens a listening TCP socket bound to all interfaces on `port`. */
SPUDRESULT spudnet_listen_create(uint16_t port, spudnet_socket *out_socket);

/* Blocks until an incoming connection arrives (unless `listen_socket` is
 * non-blocking — see spudnet_set_blocking), then hands back a new socket
 * for that peer. `listen_socket` itself keeps listening for more. */
SPUDRESULT spudnet_accept(
    spudnet_socket listen_socket, spudnet_socket *out_client_socket);

/*
 * Client side — the devices joining an already-running scene.
 */

/* Resolves `host` (hostname or dotted IPv4) and connects to it on `port`. */
SPUDRESULT spudnet_connect(
    const char *host, uint16_t port, spudnet_socket *out_socket);

/*
 * I/O — both sides use the same socket handle once connected.
 */

/* `out_sent` may be less than `size` — a short write, same as a raw send(). */
SPUDRESULT spudnet_send(
    spudnet_socket socket, const void *data, uint64_t size, uint64_t *out_sent);

/* `out_received` may be less than `size`. 0 bytes with SPUD_SUCCESS means
 * the peer closed the connection cleanly (same as recv() returning 0). */
SPUDRESULT spudnet_recv(
    spudnet_socket socket, void *data, uint64_t size, uint64_t *out_received);

/* Non-blocking sockets return SPUDRESULT_SNET_WOULD_BLOCK from
 * send/recv/accept instead of blocking — needed so Erethal's render loop
 * can poll the network each frame without stalling on data that hasn't
 * arrived yet. Blocking (the default after connect/accept) is fine for a
 * dedicated network thread. */
SPUDRESULT spudnet_set_blocking(spudnet_socket socket, bool blocking);

void spudnet_close(spudnet_socket socket);

#if __cplusplus
}
#endif

#endif // SPUDNET_H
