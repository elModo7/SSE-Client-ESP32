# ESP32 SSE Client

Minimal example of an ESP32 consuming Server-Sent Events for my [PokeT-Dex](https://github.com/elModo7/pokemon-crystal-research-toolkit) project.

For the server you can use my [Go SSE Server Template](https://github.com/elModo7/sse-server-go).

Most ESP32 examples show how to send SSE from the board. This one does the opposite: the ESP32 connects to an SSE endpoint, keeps the HTTP connection open, reads the stream line by line, and handles incoming events.

## What it does

- Connects to WiFi
- Opens an HTTP connection to an SSE endpoint
- Sends a `GET` request with `Accept: text/event-stream`
- Reads incoming SSE lines
- Handles:
  - `event:`
  - `data:`
  - `id:`
  - `retry:`
  - heartbeat/comment lines
- Reconnects automatically if the connection drops

## Example SSE stream

```text
event: temperature
data: {"value":23.5}

event: status
data: online
```

## Basic usage

Update these values in the sketch:

```cpp
const char* ssid = "wifiname";
const char* password = "wifipassword";

const char* host = "192.168.1.100";
const int port = 8000;
const char* path = "/events";
```

Then upload the sketch to your ESP32.

Open the serial monitor at `115200` baud to see incoming events.

## How SSE is parsed

SSE is a plain text stream.

Each event is made of one or more lines:

```text
event: my-event
data: hello
```

An empty line marks the end of the event:

```text
event: my-event
data: hello
```

The ESP32 stores the current event name and data until it receives that empty line, then processes the event.

## HTTPS

For HTTPS endpoints, replace `WiFiClient` with `WiFiClientSecure`.

For quick testing:

```cpp
client.setInsecure();
```

For real projects, use a valid root certificate instead.

## Notes

This is meant to be simple and easy to adapt.

For production use, consider:

- avoiding heavy use of `String`
- adding `Last-Event-ID` support
- storing the server-provided `retry:` value
- handling JSON payloads with ArduinoJson
- adding better timeout and watchdog handling

## Requirements

- ESP32 board
- Arduino IDE or PlatformIO
- WiFi connection
- An SSE server endpoint

## License

MIT
