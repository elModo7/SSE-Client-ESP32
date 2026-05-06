#include <WiFi.h>
#include <WiFiClient.h> // #include <WiFiClientSecure.h>

const char* ssid = "wifiname";
const char* password = "wifipassword";

const char* host = "192.168.1.100";
const int port = 8000;
const char* path = "/events";

WiFiClient client; // WiFiClientSecure client; -> Secure client requires this for non HTTPS: client.setInsecure();

String eventName = "message";
String eventData = "";

void connectSSE() {
  Serial.println("Connecting SSE...");

  if (!client.connect(host, port)) {
    Serial.println("Error connecting to the server");
    return;
  }

  client.print(String("GET ") + path + " HTTP/1.1\r\n");
  client.print(String("Host: ") + host + "\r\n");
  client.print("Accept: text/event-stream\r\n");
  client.print("Cache-Control: no-cache\r\n");
  client.print("Connection: keep-alive\r\n");
  client.print("\r\n");

  // Skip HTTP headers
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    line.trim();

    if (line.length() == 0) {
      break;
    }
  }

  Serial.println("SSE connected");
}

void handleSSELine(String line) {
  line.trim();

  // Empty line = end of event
  if (line.length() == 0) {
    if (eventData.length() > 0) {
      Serial.println("Evento: " + eventName);
      Serial.println("Data: " + eventData);

      // Here we set the event
      if (eventName == "temperature") {
        // parse JSON, update status etc
      }

      eventName = "message";
      eventData = "";
    }
    return;
  }

  // SSE comment, used sometimes as heartbeat
  if (line.startsWith(":")) {
    return;
  }

  if (line.startsWith("event:")) {
    eventName = line.substring(6);
    eventName.trim();
  } else if (line.startsWith("data:")) {
    String dataPart = line.substring(5);
    dataPart.trim();

    if (eventData.length() > 0) {
      eventData += "\n";
    }

    eventData += dataPart;
  } else if (line.startsWith("id:")) {
    String id = line.substring(3);
    id.trim();
    Serial.println("ID: " + id);
  } else if (line.startsWith("retry:")) {
    String retryMs = line.substring(6);
    retryMs.trim();
    Serial.println("Retry: " + retryMs);
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");

  connectSSE();
}

void loop() {
  if (!client.connected()) {
    Serial.println("SSE disconnected. Retrying...");
    client.stop();
    delay(3000);
    connectSSE();
    return;
  }

  while (client.available()) {
    String line = client.readStringUntil('\n');
    handleSSELine(line);
  }

  delay(10);
}