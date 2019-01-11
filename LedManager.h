#ifndef setupMQTT_h
#include "Arduino.h"
#define setupMQTT_h

class LedManager{
  public:
    LedManager();
    // setup pinmod
    void SETUP();

    //turn on digital led
    void ONDILED(int gpio);

    //turn off digital led
    void OFFDILED(int gpio);

    //turn on analog led
    void ONANLED(int gpio, int value);

    //turn on analog led
    void OFFANLED(int gpio);
};

extern LedManager ledManager;
extern int WHITE_PIN;
extern int RED_PIN;
extern int GREEN_PIN;
extern int BLUE_PIN;
#endif
