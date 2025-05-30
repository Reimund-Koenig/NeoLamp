# ESP Async WebServer

[![License: LGPL 3.0](https://img.shields.io/badge/License-LGPL%203.0-yellow.svg)](https://opensource.org/license/lgpl-3-0/)
[![Continuous Integration](https://github.com/mathieucarbou/ESPAsyncWebServer/actions/workflows/ci.yml/badge.svg)](https://github.com/mathieucarbou/ESPAsyncWebServer/actions/workflows/ci.yml)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/mathieucarbou/library/ESP%20Async%20WebServer.svg)](https://registry.platformio.org/libraries/mathieucarbou/ESP%20Async%20WebServer)

Asynchronous HTTP and WebSocket Server Library for ESP32.
Supports: WebSocket, SSE, Authentication, Arduino Json 7, File Upload, Static File serving, URL Rewrite, URL Redirect, etc.

This fork is based on [yubox-node-org/ESPAsyncWebServer](https://github.com/yubox-node-org/ESPAsyncWebServer) and includes all the concurrency fixes.

## Changes in this fork

- Removed SPIFFSEditor
- Deployed in PlatformIO registry and Arduino IDE library manager
- CI
- Some code cleanup
- `SSE_MAX_QUEUED_MESSAGES` to control the maximum number of messages that can be queued for a SSE client
- `write()` function public in `AsyncEventSource.h`
- Arduino Json 7 compatibility and backward compatible with 6 and 6 (changes in `AsyncJson.h`). The API to use Json has not changed. These are only internal changes.
- `WS_MAX_QUEUED_MESSAGES`: control the maximum number of messages that can be queued for a Websocket client
- Resurrected `AsyncWebSocketMessageBuffer` and `makeBuffer()` in order to make the fork API-compatible with the original library from me-no-dev regarding WebSocket.
- [#5](https://github.com/mathieucarbou/ESPAsyncWebServer/pull/5) ([@vortigont](https://github.com/vortigont)): set real "Last-Modified" header based on file's LastWrite time
- [#13](https://github.com/mathieucarbou/ESPAsyncWebServer/pull/13) ([@tueddy](https://github.com/tueddy)): Compile with Arduino 3 (ESP-IDF 5.1)
- [#14](https://github.com/mathieucarbou/ESPAsyncWebServer/pull/14) ([@nilo85](https://github.com/nilo85)): Add support for Auth & GET requests in AsyncCallbackJsonWebHandler
- Added `setAuthentication(const String& username, const String& password)`
- Added `StreamConcat` example to show how to stream multiple files in one response
- Remove filename after inline in Content-Disposition header according to RFC2183
- Depends on `mathieucarbou/Async TCP @ ^3.1.4`
- Arduino 3 / ESP-IDF 5.1 compatibility
- Added all flavors of `binary()`, `text()`, `binaryAll()` and `textAll()` in `AsyncWebSocket`
- Added `setCloseClientOnQueueFull(bool)` which can be set on a client to either close the connection or discard messages but not close the connection when the queue is full
- [#29](https://github.com/mathieucarbou/ESPAsyncWebServer/pull/29) ([@vortigont](https://github.com/vortigont)): Some websocket code cleanup
- Use `-D DEFAULT_MAX_WS_CLIENTS` to change the number of allows WebSocket clients and use `cleanupClients()` to help cleanup resources about dead clients

## Documentation

Usage and API stays the same as the original library.
Please look at the original libraries for more examples and documentation.

- [https://github.com/me-no-dev/ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) (original library)
- [https://github.com/yubox-node-org/ESPAsyncWebServer](https://github.com/yubox-node-org/ESPAsyncWebServer) (fork of the original library)

## `AsyncWebSocketMessageBuffer` and `makeBuffer()`

The fork from `yubox-node-org` introduces some breaking API changes compared to the original library, especially regarding the use of `std::shared_ptr<std::vector<uint8_t>>` for WebSocket.

This fork is compatible with the original library from `me-no-dev` regarding WebSocket, and wraps the optimizations done by `yubox-node-org` in the `AsyncWebSocketMessageBuffer` class.
So you have the choice of which API to use.

Here are examples for serializing a Json document in a websocket message buffer:

```cpp
void send(JsonDocument& doc) {
  const size_t len = measureJson(doc);

  // original API from me-no-dev
  AsyncWebSocketMessageBuffer* buffer = _ws->makeBuffer(len);
  assert(buffer); // up to you to keep or remove this
  serializeJson(doc, buffer->get(), len);
  _ws->textAll(buffer);
}
```

```cpp
void send(JsonDocument& doc) {
  const size_t len = measureJson(doc);

  // this fork (originally from yubox-node-org), uses another API with shared pointer
  auto buffer = std::make_shared<std::vector<uint8_t>>(len);
  assert(buffer); // up to you to keep or remove this
  serializeJson(doc, buffer->data(), len);
  _ws->textAll(std::move(buffer));
}
```

I recommend to use the official API `AsyncWebSocketMessageBuffer` to retain further compatibility.

## Stack size and queues

Here are some important flags to tweak depending on your needs:

```cpp
  // Async TCP queue size
  -D CONFIG_ASYNC_TCP_QUEUE_SIZE=128
  // Async TCP async task core
  -D CONFIG_ASYNC_TCP_RUNNING_CORE=1
  // Async TCP async stac ksize
  -D CONFIG_ASYNC_TCP_STACK_SIZE=8096
  // WebSocket queue size
  -D WS_MAX_QUEUED_MESSAGES=64
```
