/**
Copyright (c) <2013> <Ross Gerbasi>

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the
  Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
  Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


/**
 DRAGONFRAME SERIAL MANAGER
 Simple interface for communication with Dragonframe for an arduino. 
 Uses a simple callback pattern to capture events.
 Allows for basic serial communication into Dragonframe.
 
 @author Ross Gerbasi
 @version 1.0.0
*/

#include "DFSM.h"

/**
* Constructor will set default values for all Variables
*/ 
DFSM::DFSM() {
  serialState = 0;
  serialNumber = 0;
  serialLastValue = 0;
  
  inCommand = 0;
  commandFrame = 0;
  commandExposure = 0;
  commandStereoPosition = 0;
  
  SIMULATION_CAPTURE_DELAY = 500;
  SIMULATION_POSITION_DELAY = 200;
  
}

/**
* Initialize the Serial Connection
*/
void DFSM::init() {
    Serial.begin(57600);
}

/**
* Should be running on loop
*/
void DFSM::update() {
  processSerial();
  updateSimulation();
}

/** ----------------------------------------------------
*
* Functions to Handle Serial messages sent FROM Dragonframe
*
---------------------------------------------------- **/

/**
* Add a Message Callback
*/
void DFSM::addCallback(int command, void (*function)(MessageData messageData)) {
  if (numCallbacks < MAXCALLBACKS) {
    callbackList[numCallbacks].command = command;
    callbackList[numCallbacks].function = function; 
    numCallbacks++; 
  }
}

/**
* Add a default handler, will be called for every message
*/
void DFSM::addDefaultCallback(void (*function)(MessageData messageData)) {
  defaultCallback = function;
}

/**
* Dispatches callbacks for a specific command
*/
void DFSM::dispatchCallback() {
  MessageData messageData;
  messageData.command          = inCommand;
  if(inCommand != DF_DELETE_MSG) {
    messageData.frame            = commandFrame;
    messageData.exposure         = commandExposure;
    messageData.exposureName     = commandExposureName;
    messageData.stereoPosition   = commandStereoPosition;
  }
  
  switch(inCommand) {
    case DF_SHOOT_MSG:
      dispatchCallbacks(DF_SHOOT_MSG, messageData);
      break;
    case DF_POSITION_MSG:
      dispatchCallbacks(DF_POSITION_MSG, messageData);
      break;
    case DF_DELETE_MSG:
      dispatchCallbacks(DF_DELETE_MSG, messageData);
      break;
    case DF_CC_MSG:
      dispatchCallbacks(DF_CC_MSG, messageData);
      break;
  }
  
  if(defaultCallback) defaultCallback(messageData);
}

/**
* Dispatches all the callbacks for a specific command
*/
void DFSM::dispatchCallbacks(int command, MessageData messageData) {  
  for (int i=0;i<numCallbacks;i++) {
    if(callbackList[i].command == command) {
      (*callbackList[i].function)(messageData); 
    }
  }
}

/**
* Processes Serial data parsing it into variables to be sent to callbacks
*/
void DFSM::processSerial(){
  if(!(Serial.available() > 0)) return; 
  
  int inByte = Serial.read();
  
  if (inByte == '\n' && serialLastValue == '\r') {
    if (serialState == STATE_EXP_STER) {
      commandStereoPosition = serialNumber;
    }
    serialState = STATE_START;
    dispatchCallback();
    return;
  }
  
  if (serialState == STATE_START) {
    inCommand = 0;
    commandFrame = 0;
    commandExposure = 0;
    exposureNameIndex = 0;
    commandExposureName[0] = 0;
    serialState = STATE_CH1;
  } else if (serialState == STATE_CH1) {
    if (serialLastValue == 'S' && inByte == 'H') {
      inCommand = DF_SHOOT_MSG;
      serialState = STATE_PREFRAME;
      serialNumber = 0;
    } else if (serialLastValue == 'D' && inByte == 'E') {
      inCommand = DF_DELETE_MSG;
    } else if (serialLastValue == 'P' && inByte == 'F') {
      inCommand = DF_POSITION_MSG;
      serialState = STATE_PREFRAME;
      serialNumber = 0;
    } else if (serialLastValue == 'C' && inByte == 'C') {
      inCommand = DF_CC_MSG;
      serialState = STATE_PREFRAME;
      serialNumber = 0;
    } else {
      serialState = STATE_START;
    }
  } else if (serialState == STATE_PREFRAME) {
    if (inByte == ' ') {
      serialState = STATE_FRAME;
    } else {
      inCommand = 0;
      serialState = STATE_START;
    }
  } else if (serialState == STATE_FRAME) {
    if (inByte == ' ') {
      serialState = STATE_EXP;
      commandFrame = serialNumber;
      serialNumber = 0;
    } else if (inByte >= '0' && inByte <= '9') {
      serialNumber = (10 * serialNumber) + (inByte - '0');
    }
  } else if (serialState == STATE_EXP) {
    if (inByte == ' ') {
      serialState = STATE_EXP_NAME;
      commandExposure = serialNumber;
      serialNumber = 0;
    } else if (inByte >= '0' && inByte <= '9') {
      serialNumber = (10 * serialNumber) + (inByte - '0');
    }
  } else if (serialState == STATE_EXP_NAME) {
    if (inByte == ' ') {
      serialState = STATE_EXP_STER;
      serialNumber = 0;
    } else if (exposureNameIndex < DF_EXP_NAME_LENGTH) {
      commandExposureName[exposureNameIndex] = inByte;
      exposureNameIndex++;
      commandExposureName[exposureNameIndex] = 0;
    }
  } else if (serialState == STATE_EXP_STER) {
    if (inByte >= '0' && inByte <= '9') {
      serialNumber = (10 * serialNumber) + (inByte - '0');
    }
  }
  serialLastValue = inByte;
}


/** ----------------------------------------------------
*
* Functions to Send Commands TO Dragonframe
*
---------------------------------------------------- **/

/**
* Shoots (frame) number of frames
*/
void DFSM::shootFrame(int frames) {
  Serial.print("S ");
  Serial.print(frames);
  Serial.print("\r\n");
}

/**
* Deletes a frame
*/
void DFSM::deleteFrame() {
  Serial.print("D\r\n");
}

/**
* Toggles playback
*/
void DFSM::togglePlay() {
  Serial.print("P\r\n");
}
/**
* Sets Dragon to live feed
*/
void DFSM::goToLive() {
  Serial.print("L\r\n");
}


/** ----------------------------------------------------
*
* Simulation Functions
*
---------------------------------------------------- **/

void DFSM::startSimulation(int frames, int simulationDelay, boolean deleteBack, boolean shouldLoop) {
  isSimulating               = true;
  simulationDeleting         = false;
  simulationTotalFrames      = frames;
  simulationShootDelay       = simulationDelay;
  simulationDeleteBack       = deleteBack;
  simulationShouldLoop       = shouldLoop;
  simulationCurrentFrame     = 1;
  simulationState            = SIMULATION_IDLE;
  lastSimulationEventTime    = millis();
}

void DFSM::stopSimulation() {
  isSimulating = false;
}

void DFSM::updateSimulation() {
  if(!isSimulating) return;
  
  MessageData messageData;
  if(inCommand != DF_DELETE_MSG) {
    messageData.frame            = simulationCurrentFrame <= 0 ? 1 : simulationCurrentFrame;
  }
  
 switch (simulationState) {
  case SIMULATION_IDLE:
    if(!simulationDeleting && simulationCurrentFrame <= simulationTotalFrames) {
        simulationState = SIMULATION_SHOOT;
    }else if(simulationDeleting && simulationCurrentFrame > 0) {       
        simulationState = SIMULATION_DELETE;      
    }else if(simulationDeleteBack){
     simulationDeleting = true;
    }else if (simulationShouldLoop){
      simulationCurrentFrame = 1;
      simulationDeleting = false;
    }
    break;
  case SIMULATION_SHOOT:
    messageData.command = DF_SHOOT_MSG;
    dispatchCallbacks(DF_SHOOT_MSG, messageData);
    lastSimulationEventTime = millis();
    simulationState = SIMULATION_CAPTURE;
    break;
  case SIMULATION_CAPTURE:
    if(millis() - lastSimulationEventTime >= SIMULATION_CAPTURE_DELAY){
      messageData.command = DF_SHOOT_MSG;
      dispatchCallbacks(DF_CC_MSG, messageData);
      simulationCurrentFrame++;
      lastSimulationEventTime = millis();
      simulationState = SIMULATION_POSITION;
    }
    break;
  case SIMULATION_POSITION:
    if(millis() - lastSimulationEventTime >= SIMULATION_POSITION_DELAY){
      messageData.command = DF_POSITION_MSG;
      dispatchCallbacks(DF_POSITION_MSG, messageData);
      lastSimulationEventTime = millis();
      simulationState = SIMULATION_FRAME_COMPLETE;
    }
    break;
  case SIMULATION_FRAME_COMPLETE:
    if(millis() - lastSimulationEventTime >= simulationShootDelay){
      lastSimulationEventTime = millis();
      simulationState = SIMULATION_IDLE;
    }
    break;
  case SIMULATION_DELETE:
    messageData.command = DF_DELETE_MSG;
    dispatchCallbacks(DF_DELETE_MSG, messageData);
    simulationCurrentFrame--;
    lastSimulationEventTime = millis();
    simulationState = SIMULATION_POSITION;
    break;
 }
  
  
  
}
