This is the DragonFrame Serial Manager library for the Arduino.

# USAGE 
This library adds a simple callback pattern to allow DragonFrame Integration. One can add multiple callbacks to any Dragon message (Shoot, Delete, Frame Position Change, Capture Complete). A Default Callback is also available to parse all messages.   

```cpp
#include <DFSM.h>

DFSM df;

void onShoot(DFSM::MessageData messageData){
   Serial.print("Shoot: ");
   //Serial.println(messageData.command);
   //Serial.println(messageData.frame);
   //Serial.println(messageData.exposure);
   //Serial.println(messageData.exposureName);
   //Serial.println(messageData.stereoPosition);
}
void onDelete(DFSM::MessageData messageData){ }
void onPositionChange(DFSM::MessageData messageData){ }
void onCaptureComplete(DFSM::MessageData messageData){ }
void onDragonMessage(DFSM::MessageData messageData){
  Serial.print(messageData.command);
  Serial.print(" : ");
  Serial.println(messageData.frame);
}

void setup(){
  df.init();
  
  df.addCallback(DFSM::DF_SHOOT_MSG, onShoot);
  df.addCallback(DFSM::DF_DELETE_MSG, onDelete);
  df.addCallback(DFSM::DF_POSITION_MSG, onPositionChange);
  df.addCallback(DFSM::DF_CC_MSG, onCaptureComplete);
  
  df.addDefaultCallback(onDragonMessage);
}

void loop(){
  df.update();
}
```

Library also adds The ability to test arduino based animations without the use of dragongframe via the simulation engine.

```cpp
#include <DFSM.h>

DFSM df;

void onShoot(DFSM::MessageData messageData){
   Serial.print("Shoot: ");
}

void onDelete(DFSM::MessageData messageData){
   Serial.println("Delete");
}

void onPositionChange(DFSM::MessageData messageData){
   Serial.print("Frame Position: ");   
}

void onCaptureComplete(DFSM::MessageData messageData){
   Serial.print("Capture Complete: "); 
}

void onDragonMessage(DFSM::MessageData messageData){
  Serial.print("Default Handler for Command: "); 
  Serial.print(messageData.command);
  Serial.print(" : ");
  Serial.println(messageData.frame);
}

void setup(){
  df.init();
  
  df.addCallback(DFSM::DF_SHOOT_MSG, onShoot);
  df.addCallback(DFSM::DF_DELETE_MSG, onDelete);
  df.addCallback(DFSM::DF_POSITION_MSG, onPositionChange);
  df.addCallback(DFSM::DF_CC_MSG, onCaptureComplete);
  
  df.addDefaultCallback(onDragonMessage);
  
	//Simulated Delay for a Captured Frame
  //df.SIMULATION_CAPTURE_DELAY = 0;
	//Simulated Delay for a position Change
  //df.SIMULATION_POSITION_DELAY = 0;
  df.startSimulation(5, 100, true, false);
  //df.stopSimulation();
}

void loop(){
  df.update();
}
```
 
# Installation

To download from github (https://github.com/aphex/dfsm), click on the "Downloads" link in the upper right, click "Download as zip", and get a zip file.  Unzip it and rename the directory to DFSM

To install, move the downloaded DFSM directory to:
~/Documents/Arduino/libraries/DFSM

For details on the library see the Dragon Frame page for Arduino Integration at http://www.dragonframe.com/moco-3d.php#arduino

Copyright 2013 Ross Gerbasi