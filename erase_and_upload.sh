#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
SKETCH_DIR="$SCRIPT_DIR/neolamp"
BUILD_DIR="$SKETCH_DIR/build"
BOARD="${BOARD:-esp8266:esp8266:d1}"
PORT="${PORT:-COM3}"

ARDUINO_PYTHON=""
ARDUINO_ESPTOOL=""
ESPTOOL_BIN=""
PYTHON_WITH_SERIAL=""

for python_candidate in \
  "/c/Users/Reimu/AppData/Local/Arduino15/packages/esp8266/tools/python3/3.7.2-post1/python3.exe" \
  "/c/Users/Reimu/AppData/Local/Arduino15/packages/esp8266/tools/python3/3.7.2-post1/python3" \
  "/c/Users/Reimu/AppData/Local/Arduino15/packages/esp8266/tools/python3/3.7.2-post1/python.exe" \
  /c/Users/*/AppData/Local/Arduino15/packages/esp8266/tools/python3/*/python3.exe \
  /c/Users/*/AppData/Local/Arduino15/packages/esp8266/tools/python3/*/python3 \
  /c/Users/*/AppData/Local/Arduino15/packages/esp8266/tools/python3/*/python.exe; do
  if [ -x "$python_candidate" ] || [ -f "$python_candidate" ]; then
    ARDUINO_PYTHON="$python_candidate"
    break
  fi
done

for esptool_candidate in \
  "/c/Users/Reimu/AppData/Local/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/tools/esptool/esptool.py" \
  /c/Users/*/AppData/Local/Arduino15/packages/esp8266/hardware/esp8266/3.1.2/tools/esptool/esptool.py; do
  if [ -f "$esptool_candidate" ]; then
    ARDUINO_ESPTOOL="$esptool_candidate"
    break
  fi
done

if [ -n "$ARDUINO_ESPTOOL" ]; then
  ESPTOOL_BIN="$ARDUINO_ESPTOOL"
fi

if [ -z "$ESPTOOL_BIN" ] && command -v esptool.py >/dev/null 2>&1; then
  ESPTOOL_BIN="$(command -v esptool.py)"
fi

if [ -z "$ESPTOOL_BIN" ] && command -v esptool >/dev/null 2>&1; then
  ESPTOOL_BIN="$(command -v esptool)"
fi

if command -v py >/dev/null 2>&1; then
  if py -3 -c "import serial" >/dev/null 2>&1; then
    PYTHON_WITH_SERIAL="py -3"
  fi
fi

if [ -z "$PYTHON_WITH_SERIAL" ] && command -v python3 >/dev/null 2>&1; then
  if python3 -c "import serial" >/dev/null 2>&1; then
    PYTHON_WITH_SERIAL="python3"
  fi
fi

if [ -z "$PYTHON_WITH_SERIAL" ] && command -v python >/dev/null 2>&1; then
  if python -c "import serial" >/dev/null 2>&1; then
    PYTHON_WITH_SERIAL="python"
  fi
fi

if [ -n "$ARDUINO_PYTHON" ] && [ -n "$ARDUINO_ESPTOOL" ] && "$ARDUINO_PYTHON" -c "import serial" >/dev/null 2>&1; then
  PYTHON_WITH_SERIAL="$ARDUINO_PYTHON"
  ESPTOOL_BIN="$ARDUINO_ESPTOOL"
fi

if [ -z "$PYTHON_WITH_SERIAL" ] || [ -z "$ESPTOOL_BIN" ]; then
  if command -v py >/dev/null 2>&1; then
    echo "❌ pyserial fehlt. Installiere es mit: py -3 -m pip install pyserial"
  else
    echo "❌ pyserial fehlt. Installiere es mit: python -m pip install pyserial"
  fi
  exit 1
fi

echo "🔎 Verifiziere und baue Sketch neu..."
./verify.sh
if [ $? -ne 0 ]; then
  echo "❌ Build/Verifizierung fehlgeschlagen."
  exit 1
fi

echo "🧽 Lösche Flash auf $PORT..."
if [ "$PYTHON_WITH_SERIAL" = "py -3" ]; then
  py -3 "$ESPTOOL_BIN" --port "$PORT" erase_flash
elif [ "$PYTHON_WITH_SERIAL" = "python3" ]; then
  python3 "$ESPTOOL_BIN" --port "$PORT" erase_flash
elif [ "$PYTHON_WITH_SERIAL" = "python" ]; then
  python "$ESPTOOL_BIN" --port "$PORT" erase_flash
else
  "$PYTHON_WITH_SERIAL" "$ESPTOOL_BIN" --port "$PORT" erase_flash
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
