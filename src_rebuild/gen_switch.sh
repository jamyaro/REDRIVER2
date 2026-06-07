#!/usr/bin/env bash
set -euo pipefail

PREMAKE_BIN="./premake5"

if [[ ! -x "$PREMAKE_BIN" ]]; then
	PREMAKE_BIN="$(command -v premake5)"
fi

"$PREMAKE_BIN" gmake2 --os=switch