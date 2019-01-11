#ifndef MQTTManager_h
#include "Arduino.h"
#define MQTTManager_h

class MQTTClass{
  public:
    MQTTClass();
    //get port,server,user... of mqtt server on rom
    void SETUP();
    
    // connect to server
    void RUN();
    
    //callback
    void SETCALLBACK();
    
    //keep alive
    void LOOP();
    
    unsigned int HEXTODEC(String hexString);
};

extern MQTTClass mqttManager;
#endif
