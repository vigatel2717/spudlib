#!/bin/sh
# Starts tests/spudnet_test_server.py, runs spudnet_test against it, and
# stops the server again.
#
#   tests/run_spudnet_test.sh <path to spudnet_test> [part]
#
# Needs python3 and the openssl command. Exits with spudnet_test's status.

set -u
if [ $# -lt 1 ]; then
	echo "usage: $0 <path to spudnet_test> [instance|tcp|wait|resolver|http|tls|proxy|websocket]" >&2
	exit 2
fi
here=$(cd "$(dirname "$0")" && pwd)
work=$(mktemp -d "${TMPDIR:-/tmp}/spudnet_test.XXXXXX")
info="$work/info"

python3 "$here/spudnet_test_server.py" "$info" "$work" &
server=$!
trap 'kill $server 2>/dev/null; rm -rf "$work"' EXIT

# The server writes the info file once everything is listening.
tries=0
while [ ! -f "$info" ]; do
	tries=$((tries + 1))
	if [ $tries -gt 100 ] || ! kill -0 $server 2>/dev/null; then
		echo "run_spudnet_test: the server didn't start" >&2
		exit 2
	fi
	sleep 0.1
done

program=$1
shift
"$program" "$info" "$@"
