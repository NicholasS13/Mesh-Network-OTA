//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include "painlessMesh.h"
#include <Adafruit_NeoPixel.h>

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

//multicolor LED setup
#define PIN 33
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
uint32_t node1 = -1;
uint32_t node2 = -1;


// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  pixels.setPixelColor(0, pixels.Color(255,0,0));//red
  pixels.show();
  switch(toupper(msg.charAt(0))){
    case 'B':
      //
      if(from == node1){//node 1
        pixels.setPixelColor(0, pixels.Color(255,255,255));//red
        pixels.show();
        mesh.sendSingle(node2,"U");
      }else{//node 2's button was pressed so tell everyone OTA ready
             pixels.setPixelColor(0, pixels.Color(255,255,0));//red
            pixels.show();
      }
      break;
    case 'M':
      pixels.setPixelColor(0, pixels.Color(0,255,0));//red
      pixels.show();
      if(node1 == -1){
        node1 = from;
      }
      else{
        node2 = from;
      }
      break;
  }
  
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
    pixels.setPixelColor(0, pixels.Color(0,0,255));//blue
    pixels.show();
}

void changedConnectionCallback() {//seems to only run when connection gets added
  Serial.printf("Changed connections\n");
  pixels.setPixelColor(0, pixels.Color(255,140,0));//yellow
  pixels.show();
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  
  pixels.begin();
  pixels.clear();
  
//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void loop() {
  while (Serial.available()) {
    char temp = Serial.read();
    Serial.println("CHAR:"+String(temp));
    //if character recieved is valid ASCII capital letter then send that character to network
    if( toupper(temp)>64 && toupper(temp) <91){
      mesh.sendBroadcast(String(temp));
      pixels.setPixelColor(0, pixels.Color(255,255,255));//red
        pixels.show();
    }
    else if(temp == '1'){//turns LED on for device #1
      //false specifies to exclude node making request from list
      mesh.sendSingle(node1,"U");
      
    }
    else if(temp == '2'){//LED off for device 1
      mesh.sendSingle(node1,"V");
    }
    else if(temp == '3'){//turns LED on for device #2
      //false specifies to exclude node making request from list
      mesh.sendSingle(node2,"U");
      
    }
    else if(temp == '4'){//led off device #2
      mesh.sendSingle(node2,"V");
    }
    
  }    
  mesh.update();
}
