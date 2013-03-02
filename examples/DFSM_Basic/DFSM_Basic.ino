#include <DFSM.h>

DFSM df;


void onShoot(DFSM::MessageData messageData){
   Serial.print("Shoot: ");
   //Serial.println(messageData.command);
   Serial.println(messageData.frame);
   //Serial.println(messageData.exposure);
   //Serial.println(messageData.exposureName);
   //Serial.println(messageData.stereoPosition);
}

void onDelete(DFSM::MessageData messageData){
   Serial.println("Delete");
}

void onPositionChange(DFSM::MessageData messageData){
   Serial.print("Frame Position: ");   
   Serial.println(messageData.frame);
}

void onCaptureComplete(DFSM::MessageData messageData){
   Serial.print("Capture Complete: "); 
  Serial.println(messageData.frame);
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
  
  //df.SIMULATION_CAPTURE_DELAY = 0;
  //df.SIMULATION_POSITION_DELAY = 0;
  df.startSimulation(5, 100, true, false);
  //df.stopSimulation();
}

void loop(){
  df.update();
}
