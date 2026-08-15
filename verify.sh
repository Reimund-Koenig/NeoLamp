#!/bin/bash

# Projektverzeichnis (Sketch)
SKETCH_DIR="/c/Users/katha/Desktop/Reimund/Arduino/NeoLamp/neolamp"

# Lokale Bibliotheken
LIB_DIR="$SKETCH_DIR/libraries"

# FQBN für ESP32 Dev
BOARD="esp8266:esp8266:d1"

# Kompilieren mit lokalen Libs + Debug
arduino-cli compile \
  --fqbn "$BOARD" \
  --libraries "$LIB_DIR" \
  --build-path "$SKETCH_DIR/build" \
  --verbose \
  --log-level debug \
  "$SKETCH_DIR"
