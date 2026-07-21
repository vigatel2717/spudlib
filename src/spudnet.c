
#include "spudnet.h"

/* gethostname/getaddrinfo/inet_ntop are all standard sockets API present
 * identically on Winsock2 and BSD sockets, unlike everything in
 * src/net/backends/ (raw socket()/accept()/etc, which do differ enough
 * between the two to need separate backends) -- so this one lives here,
 * shared, rather than duplicated into both backends. */
#if SPUDLIB_PLATFORM_WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <stdbool.h>
#include <string.h>

#if __cplusplus
extern "C" {
#endif

SPUDRESULT spudnet_get_local_ipv4_addresses(
    char out_addresses[][SPUDNET_MAX_IPV4_STRING_LEN],
    uint32_t out_capacity,
    uint32_t *out_count) {
	if (!out_addresses || !out_count)
		return SPUDRESULT_NULL_OUTPUT_PARAMETER;
	*out_count = 0;
	if (out_capacity == 0)
		return SPUD_SUCCESS;

	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) != 0)
		return SPUDRESULT_SNET_RESOLVE_FAILED;

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo *result = NULL;
	if (getaddrinfo(hostname, NULL, &hints, &result) != 0 || !result)
		return SPUDRESULT_SNET_RESOLVE_FAILED;

	for (struct addrinfo *addr = result; addr != NULL && *out_count < out_capacity; addr = addr->ai_next) {
		struct sockaddr_in *sin = (struct sockaddr_in *)addr->ai_addr;
		char buf[SPUDNET_MAX_IPV4_STRING_LEN];
		if (!inet_ntop(AF_INET, &sin->sin_addr, buf, sizeof(buf)))
			continue;
		if (strncmp(buf, "127.", 4) == 0)
			continue; // loopback -- never useful to hand to another machine

		bool duplicate = false;
		for (uint32_t i = 0; i < *out_count; ++i) {
			if (strcmp(out_addresses[i], buf) == 0) {
				duplicate = true;
				break;
			}
		}
		if (duplicate)
			continue; // the resolver can list the same address more than once

		strncpy(out_addresses[*out_count], buf, SPUDNET_MAX_IPV4_STRING_LEN - 1);
		out_addresses[*out_count][SPUDNET_MAX_IPV4_STRING_LEN - 1] = '\0';
		(*out_count)++;
	}

	freeaddrinfo(result);
	return SPUD_SUCCESS; // *out_count == 0 (no non-loopback interface) is a valid result, not a failure
}

#if __cplusplus
}
#endif
