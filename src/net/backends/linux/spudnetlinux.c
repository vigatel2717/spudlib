
#if SPUDLIB_PLATFORM_LINUX

#include "spudnet.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#if __cplusplus
extern "C" {
#endif

struct spudnet_socket_t {
#if _DEBUG
	const char *debug_name;
#endif
	int handle;
};

static struct spudnet_socket_t *spudnet_wrap(int fd) {
	struct spudnet_socket_t *sock = (struct spudnet_socket_t *)malloc(sizeof(struct spudnet_socket_t));
	if (!sock) {
		close(fd);
		return NULL;
	}
	sock->handle = fd;
	return sock;
}

// --------------------------------------------------------------------------
// Lifecycle
// --------------------------------------------------------------------------

/* BSD sockets need no global init/teardown — these exist purely so callers
 * can write one code path across platforms (Winsock2 needs WSAStartup). */
SPUDRESULT spudnet_startup(void) { return SPUD_SUCCESS; }

void spudnet_shutdown(void) {}

// --------------------------------------------------------------------------
// Server side
// --------------------------------------------------------------------------

SPUDRESULT spudnet_listen_create(
    uint16_t port,
    spudnet_socket *out_socket) {
	if (!out_socket)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;

	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd < 0)
		return SPUDRESULT_SNET_LISTEN_FAILED;

	// Allow immediate rebind after a restart instead of sitting in TIME_WAIT.
	int reuse = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port        = htons(port);

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		close(fd);
		return SPUDRESULT_SNET_LISTEN_FAILED;
	}

	if (listen(fd, SOMAXCONN) < 0) {
		close(fd);
		return SPUDRESULT_SNET_LISTEN_FAILED;
	}

	struct spudnet_socket_t *sock = spudnet_wrap(fd);
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

	int client = accept(listen_socket->handle, NULL, NULL);
	if (client < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN)
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
	snprintf(port_str, sizeof(port_str), "%u", (unsigned)port);

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	struct addrinfo *result = NULL;
	if (getaddrinfo(host, port_str, &hints, &result) != 0 || !result)
		return SPUDRESULT_SNET_RESOLVE_FAILED;

	int fd            = -1;
	SPUDRESULT status = SPUDRESULT_SNET_CONNECT_FAILED;

	for (struct addrinfo *addr = result; addr != NULL; addr = addr->ai_next) {
		fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (fd < 0)
			continue;

		if (connect(fd, addr->ai_addr, addr->ai_addrlen) == 0) {
			status = SPUD_SUCCESS;
			break;
		}

		close(fd);
		fd = -1;
	}

	freeaddrinfo(result);

	if (status != SPUD_SUCCESS)
		return status;

	struct spudnet_socket_t *sock = spudnet_wrap(fd);
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

	size_t chunk = size > SIZE_MAX ? SIZE_MAX : (size_t)size;

	// MSG_NOSIGNAL: a send() to a peer that already closed its end would
	// otherwise raise SIGPIPE and kill the process by default — we'd rather
	// get the error back through the return value like every other failure
	// here.
	ssize_t sent = send(socket->handle, data, chunk, MSG_NOSIGNAL);
	if (sent < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN)
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

	size_t chunk = size > SIZE_MAX ? SIZE_MAX : (size_t)size;

	ssize_t received = recv(socket->handle, data, chunk, 0);
	if (received < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN)
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

	int flags = fcntl(socket->handle, F_GETFL, 0);
	if (flags < 0)
		return SPUDRESULT_SNET_SET_BLOCKING_FAILED;

	flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
	if (fcntl(socket->handle, F_SETFL, flags) < 0)
		return SPUDRESULT_SNET_SET_BLOCKING_FAILED;

	return SPUD_SUCCESS;
}

void spudnet_close(spudnet_socket socket) {
	if (!socket)
		return;
	close(socket->handle);
	free(socket);
}

#if __cplusplus
}
#endif

#endif // SPUDLIB_PLATFORM_LINUX
