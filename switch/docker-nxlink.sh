#!/usr/bin/env bash
set -euo pipefail

IMAGE_NAME="${IMAGE_NAME:-redriver2-switch-builder:latest}"
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SWITCH_IP="${1:-}"
NRO_PATH="$ROOT_DIR/src_rebuild/bin/Release/switch/REDRIVER2.nro"

if [[ -z "$SWITCH_IP" ]]; then
    echo "usage: $0 <SWITCH_IP>"
    exit 1
fi

if ! command -v docker >/dev/null 2>&1; then
    echo "error: docker was not found in PATH" >&2
    exit 1
fi

if [[ ! -f "$NRO_PATH" ]]; then
    echo "$NRO_PATH not found. Building it now..."
    "$ROOT_DIR/switch/docker-build.sh"

    if [[ ! -f "$NRO_PATH" ]]; then
        echo "error: build completed but $NRO_PATH was not created" >&2
        exit 1
    fi
fi

if ! docker image exists "$IMAGE_NAME"; then
    echo "container image $IMAGE_NAME was not found. building it now..."
    docker build \
        -t "$IMAGE_NAME" \
        -f "$ROOT_DIR/switch/Dockerfile" \
        "$ROOT_DIR"
fi

echo "deploying REDRIVER2 to $SWITCH_IP..."
echo "========================================="
echo

docker run --rm -it \
    --network=host \
    -v "$ROOT_DIR:/src:Z" \
    -w /src \
    "$IMAGE_NAME" \
    nxlink -a "$SWITCH_IP" -s src_rebuild/bin/Release/switch/REDRIVER2.nro

echo
echo "========================================="
echo "nxlink session closed."
echo
