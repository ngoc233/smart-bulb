#include "EDBManager.h"
#include "Arduino.h"
#define TABLE_SIZE 8192
#include <EDB.h>
#include <FS.h>

char* db_name = "/db/edb_test.db";
File dbFile;

// Arbitrary record definition for this table.
// This should be modified to reflect your record needs.
struct LogEvent {
    int id;
    int timestamp;
    int status;
}
logEvent;

// The read and write handlers for using the SPIFFS Library
// Also blinks the led while writing/reading
void writer (unsigned long address, byte data) {
    dbFile.seek(address, SeekSet);
    dbFile.write(data);
    dbFile.flush();
}

byte reader (unsigned long address) {
    dbFile.seek(address, SeekSet);
    return dbFile.read();
}

// Create an EDB object with the appropriate write and read handlers
EDB db(&writer, &reader);

void printError(EDB_Status err)
{
    Serial.print("ERROR: ");
    switch (err)
    {
        case EDB_OUT_OF_RANGE:
            Serial.println("Recno out of range");
            break;
        case EDB_TABLE_FULL:
            Serial.println("Table full");
            break;
        case EDB_OK:
        default:
            Serial.println("OK");
            break;
    }
}

EDBManager::EDBManager(){
  
}

void EDBManager::SETUP(){
  Serial.println(" Extended Database Library + SPIFFS storage demo");
    Serial.println();

    randomSeed(analogRead(0));

    SPIFFS.begin();
    delay(2000);

    if (SPIFFS.exists(db_name)) {

        dbFile = SPIFFS.open(db_name, "r+");

        if (dbFile) {
            Serial.print("Opening current table... ");
            EDB_Status result = db.open(0);
            if (result == EDB_OK) {
                Serial.println("DONE");
            } else {
                Serial.println("ERROR");
                Serial.println("Did not find database in the file " + String(db_name));
                Serial.print("Creating new table... ");
                db.create(0, TABLE_SIZE, (unsigned int)sizeof(logEvent));
                Serial.println("DONE");
                return;
            }
        } else {
            Serial.println("Could not open file " + String(db_name));
            return;
        }
    } else {
        Serial.print("Creating table... ");
        // create table at with starting address 0
        dbFile = SPIFFS.open(db_name, "w+");
        db.create(0, TABLE_SIZE, (unsigned int)sizeof(logEvent));
        Serial.println("DONE");
    }

    edbManager.SELECTCORDS();
}

void EDBManager::SELECTCORDS(){
  for (int recno = 1; recno <= db.count(); recno++)
    {
        EDB_Status result = db.readRec(recno, EDB_REC logEvent);
        if (result == EDB_OK)
        {
            Serial.print("Recno: ");
            Serial.print(recno);
            Serial.print(" ID: ");
            Serial.print(logEvent.id);
            Serial.print(" Temp: ");
            Serial.println(logEvent.timestamp);
        }
        else printError(result);
    }
}

void EDBManager::INSERTRECORD(){
  
}

EDBManager edbManager = EDBManager();
