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
#ifndef DFSM_h
#define DFSM_h

#include "Arduino.h"
#include <string.h>


class DFSM {
  public:
    static const int DF_SHOOT_MSG     = 1;
    static const int DF_DELETE_MSG    = 2;
    static const int DF_CC_MSG        = 3;
    static const int DF_POSITION_MSG  = 4;
    int SIMULATION_CAPTURE_DELAY;
    int SIMULATION_POSITION_DELAY;
  
    DFSM();
    void init();
    void update();
    
    // Sending Commands INTO Dragonframe
    void shootFrame(int frames);
    void deleteFrame();
    void togglePlay();
    void goToLive();
    
    // Message Data is sent back to callbacks
    typedef struct _messageData {
      int command;
      int frame;
      int exposure;
      char* exposureName;
      int stereoPosition;
    } MessageData;
    
    void addCallback(int command, void(*)(MessageData messageData));
    void addDefaultCallback(void (*function)(MessageData messageData)); 
    
    void startSimulation(int frames, int simulationDelay, boolean deleteBack, boolean shouldLoop);
    void stopSimulation();
  private:
    static const int STATE_START         = 0;
    static const int STATE_CH1           = 1;
    static const int STATE_PREFRAME      = 2;
    static const int STATE_FRAME         = 3;
    static const int STATE_EXP           = 4;
    static const int STATE_EXP_NAME      = 5;
    static const int STATE_EXP_STER      = 6;
   
    static const int MAXCALLBACKS        = 50;
    static const int DF_EXP_NAME_LENGTH  = 16;
  
    int commandFrame;
    int commandExposure;
    char commandExposureName[DF_EXP_NAME_LENGTH + 1];
    int commandStereoPosition;
    int inCommand;
    int serialState;
    int serialNumber;
    int serialLastValue;
    int exposureNameIndex;
    
    typedef struct _callback {
      int command;
      void (*function)(MessageData messageData);
    } SerialCommandCallback;
    int numCallbacks;
    SerialCommandCallback callbackList[MAXCALLBACKS];
    void (*defaultCallback)(MessageData messageData);
  
    void processSerial();
    void dispatchCallback();
    void dispatchCallbacks(int command, MessageData messageData);
    
    // Simulation Variables 
    static const int SIMULATION_IDLE           = 0;
    static const int SIMULATION_SHOOT          = 1;
    static const int SIMULATION_POSITION       = 2;
    static const int SIMULATION_CAPTURE        = 3;
    static const int SIMULATION_FRAME_COMPLETE = 4;
    static const int SIMULATION_DELETE         = 5;
    
    boolean isSimulating;
    int simulationTotalFrames;
    int simulationShootDelay;
    boolean simulationDeleteBack;
    boolean simulationShouldLoop;
    boolean simulationDeleting;
    int simulationCurrentFrame;
    int simulationState;
    unsigned long lastSimulationEventTime;
    
    void updateSimulation();
};

#endif
