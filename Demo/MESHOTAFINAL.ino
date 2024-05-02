#include <Preferences.h>
#include <Adafruit_NeoPixel.h>
#include "HttpsOTAUpdate.h"
#include "WiFi.h"
#include "painlessMesh.h"

#define Wifi_SSID "DESKTOP-G9U7TU3"
#define Wifi_PASSWORD "24C/b851"
//OTA update setup
static const char *url = "http://10.203.132.147:8000/MESHOTAFINAL.ino.adafruit_feather_esp32s3.bin"; //state url of your firmware image
static const char *server_certificate = "";
static HttpsOTAStatus_t otastatus;


#define LED 10
#define B 6

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

//multicolor LED setup
#define PIN 33
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

unsigned int task = 0;
Preferences preferences;

bool task_ran = false;
bool notRun = true;
void HttpEvent(HttpEvent_t *event)
{
    switch(event->event_id) {
        case HTTP_EVENT_ERROR:
            Serial.println("Http Event Error");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            Serial.println("Http Event On Connected");
            break;
        case HTTP_EVENT_HEADER_SENT:
            Serial.println("Http Event Header Sent");
            break;
        case HTTP_EVENT_ON_HEADER:
            Serial.printf("Http Event On Header, key=%s, value=%s\n", event->header_key, event->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            break;
        case HTTP_EVENT_ON_FINISH:
            Serial.println("Http Event On Finish");
            break;
        case HTTP_EVENT_DISCONNECTED:
            Serial.println("Http Event Disconnected");
            break;
    }
}
uint32_t rootNode = -1;
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  pixels.setPixelColor(0, pixels.Color(255,0,0));//red
  pixels.show();
  mesh.sendSingle(from,"SG Recieved:"+msg);
  switch(toupper(msg.charAt(0))){
    case 'R':
      pixels.setPixelColor(0, pixels.Color(255,255,255));//white
      pixels.show();
      mesh.sendSingle(from, "M");//give root node this node's ID
      rootNode=from;
    case 'U':
      digitalWrite(LED,HIGH);
      break;
    case 'V':
      digitalWrite(LED,LOW);
      break;
    case 'O'://puts device into OTA mode
      preferences.putUInt("task",1);
      preferences.end();
    
      delay(5000);
      ESP.restart();
      break;//dont think this is necessary
    case 'Z'://so demo knows which device to not communicate with
      rootNode = from;
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
  //preferences.putUInt("task",1);
  //preferences.end();

  //delay(5000);
  //ESP.restart();
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}
void setup() {
  Serial.begin(115200);
  pixels.begin();
  pixels.clear();
  preferences.begin("my-app", false);
  task = preferences.getUInt("task",0);//defaults to 0 if task variable not found
  if (task==1){
    
    pixels.setPixelColor(0, pixels.Color(255,0,255));
    pixels.show();
    HttpsOTA.onHttpEvent(HttpEvent);
    WiFi.begin(Wifi_SSID, Wifi_PASSWORD);
    
  }else{
    
    pixels.setPixelColor(0, pixels.Color(255,0,0));
    pixels.show();
    mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  
    mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  }
  pinMode(LED,OUTPUT);
  pinMode(B,INPUT_PULLUP);

  
}


void otaStuff(){
  otastatus = HttpsOTA.status();
  if(otastatus == HTTPS_OTA_SUCCESS) {
    if(!task_ran){ 
      //tell mesh network update available
      Serial.println("Firmware written successfully. To reboot device, call API ESP.restart() or PUSH restart button on device");
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));//Pink to show successful update
      pixels.show();
      preferences.putUInt("task",0);
      preferences.end();
     
    }
  } else if(otastatus == HTTPS_OTA_FAIL) { 
      Serial.println("Firmware Upgrade Fail");
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));//Pink to show successful update
      pixels.show();
  }
  if(notRun){
    HttpsOTA.begin(url, server_certificate); 
    notRun = false;
    Serial.print("Waiting on OTA Update");
  }
}
void loop(){
  if(task == 1){
    if(WiFi.status() == WL_CONNECTED){
      otaStuff();
    }
  }else{
    mesh.update();
    if(digitalRead(B) == LOW){//reversed due to wiring
      mesh.sendSingle(rootNode,"B");
      pixels.setPixelColor(0, pixels.Color(0, 255, 255));//Pink to show successful update
      pixels.show();
    }
  }
  
  
  
  
}
