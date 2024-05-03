# Mesh-Network-OTA Demo
### This Demo is an example modification of the base code
This demo has an LED and a button connected to the ESP32S3 feathers. For this example I have the one esp device not connected to a breadboard act as a communicator between my computer and the meshnetwork. Similar functionality could be accomplished with a wireless serial library such as WebSerial, but the S3Feather did not have enough space onboard to support ota updates, the mesh network and the Webserial. How the demo works is that my communicator devices recieves any ascii character and then if it is in the american alphabet, then it sends that command to the meshnetwork as a broadcast or a direct message to a specific device. In this example to get all the devices the communicator needs to send the character R (to indicate that the sender node is the root node), then the nodes in the network reply and that allows the communicator node to know which node is which. 
#### Some additional commands that are built in to this demo:
  +  'U' : turns both LEDs on
  +  'V' : turns both LEDs off
  +  '1' : turns the LED on for node1 (first node that connects to network)
  +  '2' : turns the LED off for node1
  +  '3' : turn the LED on for node2
  +  '4' : turn the LED off for node2
  +  'O' : tells the mesh network that a wireless update is ready (O for OTA)
#### Additionally the buttons on the breadboard each make the builtin LED for the communicator device change color depending on which button is being pressed (button from node1 or node2)
