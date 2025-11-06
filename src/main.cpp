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
int SEEDER = 1;
int LEACHER = 2;
// 0 -> intermediate mode
// 1 -> connected to seeder
// 2 -> listener for leacher
int label = INTERMEDIATE;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  if (label == SEEDER) {
    Serial.printf("SEEDER: %s\n", msg.c_str());
  }
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

String activateSeeder = "SEEDER";
String activateLeacher = "LEACHER";
String activateIntermediate = "INTERMEDIATE";

void loop() {
  mesh.update();
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    if (msg == activateSeeder) {
      label = SEEDER;
      Serial.println("Activated as Seeder");
    } else if (msg == activateLeacher) {
      label = LEACHER;
      Serial.println("Activated as Leacher");
    } else if (msg == activateIntermediate) {
      label = INTERMEDIATE;
      Serial.println("Activated as Intermediate");
    } else if (label == SEEDER && msg.startsWith("SEEDER_REPLY: ")) {
      String reply = msg.substring(15);
      Serial.printf("SEEDER TO LEACHER: %s\n", reply.c_str());
      mesh.sendBroadcast(reply);
    } else if (label == LEACHER && msg.startsWith("LEACHER_REQUEST: "))  {
      String request = msg.substring(17);
      Serial.printf("LEACHER TO SEEDER: %s\n", request.c_str());
      mesh.sendBroadcast(request);
      // String test = "ACK";
      // receivedCallback(69, test);
    }
  }
}