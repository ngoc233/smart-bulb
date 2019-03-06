#ifndef setupEDB_h
#define setupEDB_h

class EDBManager{
  public:
    EDBManager();
    //create or opent DB
    void SETUP();
    
    //create all record first time
    void CREATERECORDS();
    
    void SELECTCORDS();
    
    void FINDRECORD();
    
    void UPDATERECORD();
    
    void INSERTRECORD();
};

extern EDBManager edbManager;
#endif
