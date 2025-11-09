#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

const char* ssid     = "TU";
const char* password = "tu@inet1";

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
  delay(100);

  Serial.printf("Connecting to %s\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // wait for connection
  pinMode(LED_BUILTIN, OUTPUT);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }
}

static int connected = 0;
static String msg;

void runRequest() {
  if (!connected) return;

  // update this to return gemini response, or whatever the
  // LLM team is doing

  HTTPClient http;
  http.begin("http://example.com/api/data");
  http.addHeader("Content-Type", "application/json");
  int code = http.POST("{\"mesh_msg\":\"" + msg + "\"}");
  Serial2.printf("ACK: %d\n", code);
  http.end();


  msg = "";
}

void loop() {
  // indicate connection state
  static unsigned long last = 0;
  if (millis() - last > 2000) {
    last = millis();
    if (WiFi.status() == WL_CONNECTED) {
      connected = 1;
    } else {
      connected = 0;
      WiFi.reconnect();
    }
  }

  digitalWrite(LED_BUILTIN, connected ? HIGH : LOW);

  if (msg.length() > 0) runRequest();
  if (!Serial2.available()) return;
  msg = Serial2.readStringUntil('\n');
  runRequest();
}
