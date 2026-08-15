#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
SKETCH_DIR="$SCRIPT_DIR/neolamp"
LIB_DIR="$SKETCH_DIR/libraries"
BUILD_DIR="$SKETCH_DIR/build"
BOARD="${BOARD:-esp8266:esp8266:d1}"

if [ ! -d "$SKETCH_DIR" ]; then
  echo "❌ Fehler: Sketch-Verzeichnis nicht gefunden: $SKETCH_DIR"
  exit 1
fi

if [ ! -d "$LIB_DIR" ]; then
  echo "❌ Fehler: Bibliotheks-Verzeichnis nicht gefunden: $LIB_DIR"
  exit 1
fi

echo "🔎 Verifiziere Sketch in $SKETCH_DIR mit Board $BOARD..."
arduino-cli compile \
  --fqbn "$BOARD" \
  --libraries "$LIB_DIR" \
  --build-path "$BUILD_DIR" \
  --verbose \
  --log-level info \
  "$SKETCH_DIR"

if [ $? -ne 0 ]; then
  echo "❌ Verifizierung fehlgeschlagen."
  exit 1
fi

echo "✅ Verifizierung erfolgreich."
