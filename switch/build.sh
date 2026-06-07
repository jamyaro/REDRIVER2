#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
PSYCROSS_MAIN="$ROOT_DIR/src_rebuild/PsyCross/src/PsyX_main.cpp"
BUILD_DIR="$ROOT_DIR/src_rebuild/bin/Release/switch"
OUTPUT_ELF="$BUILD_DIR/REDRIVER2.elf"
OUTPUT_NRO="$BUILD_DIR/REDRIVER2.nro"
NACP_FILE="$BUILD_DIR/REDRIVER2.nacp"
ICON_FILE="$ROOT_DIR/switch/icon.jpg"
NACP_VERSION_MAX_BYTES=8
APP_VERSION="${APP_VERSION-dev}"

app_version_bytes="$(printf "%s" "$APP_VERSION" | wc -c)"
app_version_bytes="${app_version_bytes//[[:space:]]/}"

if [[ -z "$APP_VERSION" ]]; then
    echo "error: APP_VERSION must not be empty" >&2
    exit 1
fi

if (( app_version_bytes > NACP_VERSION_MAX_BYTES )); then
    echo "error: APP_VERSION must be $NACP_VERSION_MAX_BYTES bytes or fewer for nacptool: $APP_VERSION" >&2
    exit 1
fi

if [[ ! -f "$PSYCROSS_MAIN" ]]; then
    echo "error: PsyCross submodule is missing. Run 'git submodule update --init --recursive'." >&2
    exit 1
fi

cd "$ROOT_DIR/src_rebuild"

./gen_switch.sh

cd build

make \
    -j"$(nproc)" \
    config=release_switch \
    REDRIVER2 \
    CC="${CC:-aarch64-none-elf-gcc}" \
    CXX="${CXX:-aarch64-none-elf-g++}" \
    AR="${AR:-aarch64-none-elf-ar}"

cd ..

if [[ ! -f "$OUTPUT_ELF" ]]; then
    echo "error: expected ELF output at $OUTPUT_ELF" >&2
    exit 1
fi

if [[ ! -f "$ICON_FILE" ]]; then
    echo "error: expected icon at $ICON_FILE" >&2
    exit 1
fi

if ! command -v nacptool >/dev/null 2>&1; then
    echo "error: nacptool was not found in PATH" >&2
    exit 1
fi

if ! command -v elf2nro >/dev/null 2>&1; then
    echo "error: elf2nro was not found in PATH" >&2
    exit 1
fi

nacptool \
    --create "REDRIVER2" "OpenDriver2" "$APP_VERSION" \
    "$NACP_FILE"

elf2nro "$OUTPUT_ELF" "$OUTPUT_NRO" --nacp="$NACP_FILE" --icon="$ICON_FILE"

echo "Created $OUTPUT_NRO"
