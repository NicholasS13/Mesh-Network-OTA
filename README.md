# Mesh-Network-OTA
## How to Use
  + Install [arduino-esp32](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) and [PainlessMesh](https://github.com/gmag11/painlessMesh/tree/master) libraries using libary manager
  + clone this repository
  + open MESHOTAFINAL.ino from root directory
  + Modify the MESHOTAFINAL File
    + Put your WIFI credentials on lines 4 and 5
      + This is required for OTA functionality  
    + Edit the code url on line 10, replace the IP address with your own IP address
      + If the new code is on a different webserver/website replace the entire url with that web address  
    + Replace the port number with the port you will be using (if applicable)
  + After the edits are made upload this file to 2 or more ESP32-S3 Feather devices
  +  Export this code as a compiled binary file
  +  Send the code to the webserver or self host it
      + You can self host the code by opening file directory in a command prompt and run the command ```python3 http.server```
  + once uploaded to multiple devices and started hosting the code you can now run the program
  + To run the program plug in all the esp32s
  + The LEDS will start with a red color to indicate that the devices are in the mesh network mode
    + if it starts out with a magenta that means it is getting an update, you can either allow the device to get the update or upload this sketch to put it into Mesh Mode
       ```
       #include <Preferences.h>
       Preferences preferences;
       void setup(){
         preferences.begin("my-app", false);
         preferences.putUInt("task",0);
         preferences.end();
       }
       void loop(){

       }
       ```
  + Once the ESP32 LED turns yellow that indicates that it connected to the meshnetwork (OnChangedConnectionCallback)
  + If it turns blue that means the same thing, only difference is that it means an additional device joined the network (newConnectionCallback)
  + once the devices LEDs turn yellow they will attempt to retrieve the update using OTA
    + This is indicated with the ESP restarting and then the LED color will be magenta
  + if the OTA update was a success after 15-20 seconds the LED should turn green, you can press the reset button on the devices and they will have the new code
    + If the LED turned red that means the code update was unsuccessful, verify that the device was able to connect to the network and that it made the request to get the update
      + Refer to demo video to learn about how to verify if the device made a request to access the code
