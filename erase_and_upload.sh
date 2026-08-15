#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
SKETCH_DIR="$SCRIPT_DIR/neolamp"
BUILD_DIR="$SKETCH_DIR/build"
BOARD="${BOARD:-esp8266:esp8266:d1}"
PORT="${PORT:-COM3}"

if command -v esptool.py >/dev/null 2>&1; then
  ESPTOOL_BIN="$(command -v esptool.py)"
elif command -v esptool >/dev/null 2>&1; then
  ESPTOOL_BIN="$(command -v esptool)"
elif command -v python3 >/dev/null 2>&1; then
  ESPTOOL_BIN="python3 -m esptool"
else
  echo "❌ Fehler: esptool wurde nicht gefunden. Installiere es oder setze ESPTOOL_BIN."
  exit 1
fi

echo "🧽 Lösche Flash auf $PORT..."
if [ "$ESPTOOL_BIN" = "python3 -m esptool" ]; then
  python3 -m esptool --port "$PORT" erase_flash
else
  "$ESPTOOL_BIN" --port "$PORT" erase_flash
fi

if [ $? -ne 0 ]; then
  echo "❌ Flash-Löschung fehlgeschlagen."
  exit 1
fi

echo "🚀 Lade Sketch-Dateien aus $BUILD_DIR auf $PORT hoch..."
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
