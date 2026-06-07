#!/usr/bin/env bash
set -euo pipefail

IMAGE_NAME="${IMAGE_NAME:-redriver2-switch-builder:latest}"
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

docker build \
    -t "$IMAGE_NAME" \
    -f "$ROOT_DIR/switch/Dockerfile" \
    "$ROOT_DIR"

docker run --rm \
    -e DEVKITPRO=/opt/devkitpro \
    -e DEVKITA64=/opt/devkitpro/devkitA64 \
    -v "$ROOT_DIR:/src:Z" \
    -w /src \
    "$IMAGE_NAME" \
    /src/switch/build.sh
