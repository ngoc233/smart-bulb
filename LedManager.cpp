#include "LedManager.h"
#include "Arduino.h"

#define RED_PIN_MANAGER 4 
#define GREEN_PIN_MANAGER 12
#define BLUE_PIN_MANAGER 14 
#define WHITE_PIN_MANAGER 5

LedManager::LedManager(){
  // convert hex color to rgb(Decimal)

}

//setup pin for device
void LedManager::SETUP(){
  pinMode(RED_PIN_MANAGER,OUTPUT);
  pinMode(GREEN_PIN_MANAGER,OUTPUT);
  pinMode(BLUE_PIN_MANAGER,OUTPUT);
  pinMode(WHITE_PIN_MANAGER,OUTPUT);
}

//turn on with digital type
void LedManager::ONDILED(int gpio){
  digitalWrite(gpio,HIGH);
}

//turn off with digital type
void LedManager::OFFDILED(int gpio){
  digitalWrite(gpio,LOW);
}

// turn on with analog type
void LedManager::ONANLED(int gpio,int value){
  analogWrite(gpio,value);
}

// turn off with analog type
void LedManager::OFFANLED(int gpio){
  analogWrite(gpio,0);
}

LedManager ledManager = LedManager();
int WHITE_PIN = WHITE_PIN_MANAGER;
int RED_PIN = RED_PIN_MANAGER;
int GREEN_PIN = GREEN_PIN_MANAGER;
int BLUE_PIN = BLUE_PIN_MANAGER;
