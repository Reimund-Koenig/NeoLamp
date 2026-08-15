#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
SKETCH_DIR="$SCRIPT_DIR/neolamp"
BUILD_DIR="$SKETCH_DIR/build"
BOARD="${BOARD:-esp8266:esp8266:d1}"
PORT="${PORT:-COM3}"

echo "🚀 Lade bereits kompilierte Sketch-Dateien aus $BUILD_DIR auf $PORT hoch..."

arduino-cli upload \
  -p "$PORT" \
  --fqbn "$BOARD" \
  --input-dir "$BUILD_DIR" \
  --verbose \
  "$SKETCH_DIR"

if [ $? -ne 0 ]; then
  echo "❌ Upload fehlgeschlagen."
  exit 1
fi

echo "✅ Upload erfolgreich abgeschlossen."
