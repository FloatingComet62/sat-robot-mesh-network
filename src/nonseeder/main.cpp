#include <Arduino.h>

#include "painlessMesh.h"

#define   MESH_PREFIX     "SATBOT"
#define   MESH_PASSWORD   "sat_ki_mkc"
#define   MESH_PORT       5555

Scheduler userScheduler;
painlessMesh  mesh;

// seeder -> device which will have internet access
// leacher -> device which will indirectly connect to seeder to get internet access, if it can't connect to internet

int INTERMEDIATE = 0;
int LEACHER = 2;

int label = INTERMEDIATE;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  if (label == LEACHER) {
    Serial.printf("LEACHER: %s\n", msg.c_str());
  }
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

void setup() {
  Serial.begin(115200);

  // mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE);
  mesh.setDebugMsgTypes(ERROR | STARTUP);

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

String activateLeacher = "LEACHER";
String activateIntermediate = "INTERMEDIATE";

void loop() {
  mesh.update();
  if (!Serial.available()) return;
  String msg = Serial.readStringUntil('\n');
  if (msg == activateLeacher) {
    label = LEACHER;
    Serial.println("Activated as Leacher");
  } else if (msg == activateIntermediate) {
    label = INTERMEDIATE;
    Serial.println("Activated as Intermediate");
  } else if (label == LEACHER && msg.startsWith("LEACHER_REQUEST: "))  {
    String request = msg.substring(17);
    bool done = mesh.sendBroadcast(request);
    Serial.printf("Broadcasting request %s result: %d\n", request.c_str(), done);
  }
}