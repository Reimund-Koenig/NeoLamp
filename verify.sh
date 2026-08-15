#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SKETCH_DIR="$SCRIPT_DIR/neolamp"
LIB_DIR="$SKETCH_DIR/libraries"
BOARD="${BOARD:-esp8266:esp8266:d1}"

arduino-cli compile \
  --fqbn "$BOARD" \
  --libraries "$LIB_DIR" \
  --build-path "$SKETCH_DIR/build" \
  --verbose \
  --log-level debug \
  "$SKETCH_DIR"
