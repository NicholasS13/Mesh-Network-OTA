
#include <Adafruit_NeoPixel.h>
#include "HttpsOTAUpdate.h"
#include "WiFi.h"
#include <Preferences.h>
#include "painlessMesh.h"

#define Wifi_SSID "wifi here"
#define Wifi_PASSWORD "password here"
//OTA update setup
static const char *url = "http://{Insert IP HERE}:8000/blink.ino.bin"; //state url of your firmware image
static const char *server_certificate = "";
static HttpsOTAStatus_t otastatus;



#define   MESH_PREFIX     "whateverYouLike"//this can be anything but whatever you edit here, all devices will need to have that change
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
   //empty method needed for ota update event to
}

void triggerUpdate(){
  preferences.putUInt("task",1);
  preferences.end();

  delay(5000);
  ESP.restart();
}

void changedConnectionCallback() {//seems to only run when connection gets added
  Serial.printf("Changed connections\n");
  pixels.setPixelColor(0, pixels.Color(255,140,0));//yellow
  pixels.show();
  
  triggerUpdate();
  
}
void otaSetup() {
  Serial.begin(115200);
  pixels.begin();
  pixels.clear();
  preferences.begin("my-app", false);
  task = preferences.getUInt("task",0);//defaults to 0 if task variable not found
  if (task==1){
    
    pixels.setPixelColor(0, pixels.Color(255,0,255)); // display magenta to show in update mode
    pixels.show();
    HttpsOTA.onHttpEvent(HttpEvent);
    WiFi.begin(Wifi_SSID, Wifi_PASSWORD);
    
  }else{
    
    pixels.setPixelColor(0, pixels.Color(255,0,0));//display red to show in mesh mode
    pixels.show();
    mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  
    mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
    mesh.onChangedConnections(&changedConnectionCallback);
  }
  
}


void otaStuff(){
  otastatus = HttpsOTA.status();
  if(otastatus == HTTPS_OTA_SUCCESS) {
    if(!task_ran){ 
      //tell mesh network update available
      Serial.println("Firmware written successfully. To reboot device, call API ESP.restart() or PUSH restart button on device");
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));//Green to show successful update
      pixels.show();
      preferences.putUInt("task",0);
      preferences.end();
     
    }
  } else if(otastatus == HTTPS_OTA_FAIL) { 
      Serial.println("Firmware Upgrade Fail");
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));//Red to show unsuccessful update
      pixels.show();
  }
  if(notRun){
    HttpsOTA.begin(url, server_certificate); 
    notRun = false;
    Serial.print("Waiting on OTA Update");
  }
}
void otaLoop(){
  if(task == 1){
    if(WiFi.status() == WL_CONNECTED){
      otaStuff();
    }
  }else{
    mesh.update();
  }
  
  
  
}
