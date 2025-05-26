#!/bin/bash

SKETCH_DIR="$HOME/Desktop/Reimund/Arduino/NeoLamp/neolamp"
BUILD_DIR="$SKETCH_DIR/build"
FQBN="esp8266:esp8266:d1"
PORT="COM3"

ESPTOOL="$HOME/.arduino15/packages/esp8266/tools/esptool/0.5.0/esptool.exe"

echo "🧽 Lösche Flash auf $PORT..."
python3 -m esptool --port "$PORT" erase_flash
if [ $? -ne 0 ]; then
  echo "❌ Flash-Löschung fehlgeschlagen."
  exit 1
fi

echo "🚀 Lade Sketch-Dateien aus $BUILD_DIR auf $PORT hoch..."
arduino-cli upload \
  -p "$PORT" \
  --fqbn "$FQBN" \
  --input-dir "$BUILD_DIR" \
  --verbose \
  "$SKETCH_DIR"

if [ $? -ne 0 ]; then
  echo "❌ Upload fehlgeschlagen."
  exit 1
fi

echo "✅ Upload erfolgreich abgeschlossen."
