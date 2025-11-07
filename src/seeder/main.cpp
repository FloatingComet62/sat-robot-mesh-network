#include <Arduino.h>
#include <painlessMesh.h>

#define   MESH_PREFIX     "SATBOT"
#define   MESH_PASSWORD   "sat_ki_mkc"
#define   MESH_PORT       5555

Scheduler userScheduler;
painlessMesh  mesh;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  // Serial.printf("SEEDER: %s\n", msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

static unsigned long lastMillis = 0;
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17); // RX=16, TX=17
  delay(100);

  // mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE);
  mesh.setDebugMsgTypes(ERROR | STARTUP);

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  Serial2.println("mkc");
  lastMillis = millis();
  Serial.println("Sent shit");
}

void loop() {
  mesh.update();

  if (Serial2.available()) {
    Serial.println("Got response shit");
    String msg = Serial2.readStringUntil('\n');
    Serial.printf("latency: %f", (millis() - lastMillis) / 1000.0);
    Serial.printf("SEEDER MACHINE RESPONSE: %s\n", msg.c_str());
  }

  // if (!Serial.available()) return;
  // String msg = Serial.readStringUntil('\n');
  // if (!msg.startsWith("SEEDER_REPLY: ")) return;
  // String reply = msg.substring(14);
  // Serial.printf("_SEEDER TO LEACHER: %s\n", reply.c_str());
  // bool done = mesh.sendBroadcast(reply);
  // Serial.printf("Broadcasting reply %s result: %d\n", reply.c_str(), done);
}