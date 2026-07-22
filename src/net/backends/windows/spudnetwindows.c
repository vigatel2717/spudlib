
#if SPUDLIB_PLATFORM_WIN32

#include "spudnet.h"

/* winsock2.h must come before any transitive <windows.h> pull-in (PCH or
 * otherwise) — <windows.h> alone drags in the legacy winsock.h and the two
 * headers conflict. This file never needs windows.h itself. */
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdlib.h>
#include <string.h>

#if __cplusplus
extern "C" {
#endif

struct spudnet_socket_t {
#if _DEBUG
	const char *debug_name;
#endif
	SOCKET handle;
};

static struct spudnet_socket_t *spudnet_wrap(SOCKET s) {
	struct spudnet_socket_t *sock = (struct spudnet_socket_t *)malloc(sizeof(struct spudnet_socket_t));
	if (!sock) {
		closesocket(s);
		return NULL;
	}
	sock->handle = s;
	return sock;
}

// --------------------------------------------------------------------------
// Lifecycle
// --------------------------------------------------------------------------

SPUDRESULT spudnet_startup(void) {
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != 0)
		return SPUDRESULT_SNET_STARTUP_FAILED;
	return SPUD_SUCCESS;
}

void spudnet_shutdown(void) { WSACleanup(); }

// --------------------------------------------------------------------------
// Server side
// --------------------------------------------------------------------------

SPUDRESULT spudnet_listen_create(
    uint16_t port,
    spudnet_socket *out_socket) {
	if (!out_socket)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
		return SPUDRESULT_SNET_LISTEN_FAILED;

	// Allow immediate rebind after a restart instead of sitting in TIME_WAIT.
	BOOL reuse = TRUE;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port        = htons(port);

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
		closesocket(s);
		return SPUDRESULT_SNET_LISTEN_FAILED;
	}

	if (listen(s, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(s);
		return SPUDRESULT_SNET_LISTEN_FAILED;
	}

	struct spudnet_socket_t *sock = spudnet_wrap(s);
	if (!sock)
		return SPUDRESULT_OUT_OF_MEMORY;

	*out_socket = sock;
	return SPUD_SUCCESS;
}

SPUDRESULT spudnet_accept(
    spudnet_socket listen_socket,
    spudnet_socket *out_client_socket) {
	if (!listen_socket)
		return SPUDRESULT_SNET_INVALID_SOCKET;
	if (!out_client_socket)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	SOCKET client = accept(listen_socket->handle, NULL, NULL);
	if (client == INVALID_SOCKET) {
		int err = WSAGetLastError();
		if (err == WSAEWOULDBLOCK)
			return SPUDRESULT_SNET_WOULD_BLOCK;
		return SPUDRESULT_SNET_ACCEPT_FAILED;
	}

	struct spudnet_socket_t *sock = spudnet_wrap(client);
	if (!sock)
		return SPUDRESULT_OUT_OF_MEMORY;

	*out_client_socket = sock;
	return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// Client side
// --------------------------------------------------------------------------

SPUDRESULT spudnet_connect(
    const char *host,
    uint16_t port,
    spudnet_socket *out_socket) {
	if (!host || host[0] == '\0')
		return SPUDRESULT_DESC_INVALID_PARAMETERS;
	if (!out_socket)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	char port_str[6];
	_itoa_s(port, port_str, sizeof(port_str), 10);

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	struct addrinfo *result = NULL;
	if (getaddrinfo(host, port_str, &hints, &result) != 0 || !result)
		return SPUDRESULT_SNET_RESOLVE_FAILED;

	SOCKET s          = INVALID_SOCKET;
	SPUDRESULT status = SPUDRESULT_SNET_CONNECT_FAILED;

	for (struct addrinfo *addr = result; addr != NULL; addr = addr->ai_next) {
		s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (s == INVALID_SOCKET)
			continue;

		if (connect(s, addr->ai_addr, (int)addr->ai_addrlen) == 0) {
			status = SPUD_SUCCESS;
			break;
		}

		closesocket(s);
		s = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (status != SPUD_SUCCESS)
		return status;

	struct spudnet_socket_t *sock = spudnet_wrap(s);
	if (!sock)
		return SPUDRESULT_OUT_OF_MEMORY;

	*out_socket = sock;
	return SPUD_SUCCESS;
}

// --------------------------------------------------------------------------
// I/O
// --------------------------------------------------------------------------

SPUDRESULT spudnet_send(
    spudnet_socket socket,
    const void *data,
    uint64_t size,
    uint64_t *out_sent) {
	if (!socket)
		return SPUDRESULT_SNET_INVALID_SOCKET;
	if (!data)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	// send() takes an int length — cap a single call at INT_MAX, same as the
	// chunking ReadFile/WriteFile already do for SpudFiles.
	int chunk = size > 0x7FFFFFFFULL ? 0x7FFFFFFF : (int)size;

	int sent = send(socket->handle, (const char *)data, chunk, 0);
	if (sent == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err == WSAEWOULDBLOCK)
			return SPUDRESULT_SNET_WOULD_BLOCK;
		return SPUDRESULT_SNET_SEND_FAILED;
	}

	if (out_sent)
		*out_sent = (uint64_t)sent;
	return SPUD_SUCCESS;
}

SPUDRESULT spudnet_recv(
    spudnet_socket socket,
    void *data,
    uint64_t size,
    uint64_t *out_received) {
	if (!socket)
		return SPUDRESULT_SNET_INVALID_SOCKET;
	if (!data)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	int chunk = size > 0x7FFFFFFFULL ? 0x7FFFFFFF : (int)size;

	int received = recv(socket->handle, (char *)data, chunk, 0);
	if (received == SOCKET_ERROR) {
		int err = WSAGetLastError();
		if (err == WSAEWOULDBLOCK)
			return SPUDRESULT_SNET_WOULD_BLOCK;
		return SPUDRESULT_SNET_RECV_FAILED;
	}

	// received == 0 here means the peer closed the connection cleanly —
	// that's a valid, successful result, not an error.
	if (out_received)
		*out_received = (uint64_t)received;
	return SPUD_SUCCESS;
}

SPUDRESULT spudnet_set_blocking(
    spudnet_socket socket,
    bool blocking) {
	if (!socket)
		return SPUDRESULT_SNET_INVALID_SOCKET;

	u_long mode = blocking ? 0 : 1;
	if (ioctlsocket(socket->handle, FIONBIO, &mode) != 0)
		return SPUDRESULT_SNET_SET_BLOCKING_FAILED;
	return SPUD_SUCCESS;
}

void spudnet_close(spudnet_socket socket) {
	if (!socket)
		return;
	closesocket(socket->handle);
	free(socket);
}

#if __cplusplus
}
#endif

#endif // SPUDLIB_PLATFORM_WIN32
