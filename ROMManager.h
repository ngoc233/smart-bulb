#ifndef ROMManager_h

#define ROMManager_h

class ROMClass
{
  public:
    ROMClass();
    void SETUP();
    void RUN();
    void RECONNECT();
    bool CONNECTION();
    String GETMACID();
};

extern ROMClass ROMManager;
extern String macID;

#endif
