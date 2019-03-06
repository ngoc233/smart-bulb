#include "MQTTManager.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "WiFiManager.h"
#include "LedManager.h"

//#define mqtt_server "m10.cloudmqtt.com" // could mqtt server
//#define mqtt_topic_pub "pubLedBulb"   // could mqtt topic pub
//#define mqtt_topic_sub "subLedBulb"   // could mqtt topic sub
//#define mqtt_user "vpfiytsx"    // could mqtt user
//#define mqtt_pwd "q2M1pcYye1IV" // could mqtt password
uint16_t mqtt_port = 10261; //could mqtt port

// tạo hằng để lưu new MQTT
const char* charMQTTServer;
const char* charMQTTPub;
const char* charMQTTSub;
const char* charMQTTUser;
const char* charMQTTPwd;

// length of family topic on rom
byte topicLength;
String familyTopic = "";

// rgb value
int red, green, blue;
float white;


WiFiClient espClient;
PubSubClient client(espClient);

MQTTClass::MQTTClass(){
  
}

void MQTTClass::SETUP(){
 // if have length value on rom-95 -> write
  charMQTTServer = "m10.cloudmqtt.com";
  charMQTTPub = "pubLedBulb";
  charMQTTUser = "vpfiytsx";
  charMQTTPwd = "q2M1pcYye1IV";
  if (EEPROM.read(95) == 255 || EEPROM.read(95) == 0)
  {
    charMQTTSub = "subLedBulb";
    Serial.println("set default MQTT");
    Serial.println(charMQTTServer);
  }
  else
  {
    Serial.println("has family id");
    topicLength = EEPROM.read(95);
    int countFTopic = 100;
    for (int i = 0; i < topicLength; i++)
    {
      familyTopic += char(EEPROM.read(countFTopic));
      countFTopic++;
    }
    Serial.print("family topic : ");
    Serial.println(familyTopic);

    Serial.print("family topic is going to use :");
    Serial.println(charMQTTSub);
  }
}

void MQTTClass::RUN(){
    client.setServer(charMQTTServer, mqtt_port);       
}


// Hàm call back để nhận dữ liệu
void callback(char* topic, byte* payload, unsigned int length) {
  String stringMessage;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    char receivedChar = (char)payload[i];
    stringMessage += (String)receivedChar;
  }
  Serial.print("string mess : ");
  Serial.println(stringMessage);
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(stringMessage);
  int functionMess = (int)root["function"];
  String stringfunction = root["function"].asString();
  
  // make list devices 
  JsonArray& arrayfunction = (root["id"]);
  for(int i=0; i<arrayfunction.size();i++)
  {
    String deviceID = arrayfunction[i]["deviceID"];
    if(deviceID == macID)
    {
      Serial.println("true device ID");
      
      if(functionMess == 1)
      {
        Serial.println("turn on Device");
        //if open app and turn on/off will use color RGB on ROM which got on setup time
        
        //turn off white led
        ledManager.OFFDILED(WHITE_PIN);
        Serial.println("take a old colors");
        if(red == 2000)
        {
          ledManager.ONANLED(WHITE_PIN,white);
          Serial.println(white);
        }
        else
        {
          ledManager.ONANLED(RED_PIN,red);
          ledManager.ONANLED(GREEN_PIN,green);
          ledManager.ONANLED(BLUE_PIN,blue);
        }
      }
      else if (functionMess == 2)
      {
        Serial.println("turn off Device");
        ledManager.OFFANLED(RED_PIN);
        ledManager.OFFANLED(GREEN_PIN);
        ledManager.OFFANLED(BLUE_PIN);
        ledManager.OFFDILED(WHITE_PIN);
      }
      else if(functionMess == 3)
      {
        Serial.println("change rgb color of Device");
        //turn off white led
        ledManager.OFFDILED(WHITE_PIN);
        
        String checkColor = root["color"].asString();
        int countColor = checkColor.length();
        if (countColor > 0)
        {
          Serial.println("co mau");
          String hexColor = root["color"].asString();
          //make color for save to ROM
  //        redRom = HEXTODEC(hexColor.substring(0, 2));
  //        greenRom = HEXTODEC(hexColor.substring(2, 4));
  //        blueRom = HEXTODEC(hexColor.substring(4, 6));
          //make color value
          red =  mqttManager.HEXTODEC(hexColor.substring(0, 2)) * 4 + 3  ;
          green = mqttManager.HEXTODEC(hexColor.substring(2, 4)) * 4 + 3;
          blue =  mqttManager.HEXTODEC(hexColor.substring(4, 6)) * 4  + 3;
          //show color
          ledManager.ONANLED(RED_PIN,red);
          ledManager.ONANLED(GREEN_PIN,green);
          ledManager.ONANLED(BLUE_PIN,blue);
        }
        else
        {
          Serial.println("don't have color");
        }
      }
      else if (functionMess == 4)
      {
          Serial.println("change white color of Device");
          
          int brightness = (int)root["brightness"];
          analogWrite(RED_PIN, 0);
          analogWrite(GREEN_PIN, 0);
          analogWrite(BLUE_PIN, 0);
          // remove rgb
          red = 2000;
          white = (10.23 * brightness);
          ledManager.ONANLED(WHITE_PIN,white);
          Serial.print("den trang sang voi gia tri");
          Serial.println(white);
      }
//      else if(functionMess == 9)
//      {
//        Serial.println("setup new topic");
//        String rFamilyTopic = root["topic"].asString();
//        EEPROM.write(95, (int)rFamilyTopic.length());
//        EEPROM.commit();
//        Serial.println(rFamilyTopic);
//        int total = 100;
//        for (int i = 0; i < rFamilyTopic.length(); i++)
//        {
//          EEPROM.put(total, rFamilyTopic[i]);
//          total++;
//          EEPROM.commit();
//        }
//      }
      else{
        Serial.println("truong hop ngoai le");
      }
    }
  }
}

void MQTTClass::SETCALLBACK(){
  client.setCallback(callback);
}

// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect(){
  // Chờ tới khi kết nối
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (EEPROM.read(95) == 255 || EEPROM.read(95) == 0)
    {
      charMQTTSub = "subLedBulb";
      Serial.println("set default MQTT");
      Serial.println(charMQTTServer);
    }
    else
    {
      // make json object and set value for new cloud
      StaticJsonBuffer<500> jsonEEPBuffer;
      JsonObject& jsonEEPROM  = jsonEEPBuffer.createObject();
      jsonEEPROM["fTopic"] = familyTopic;
      charMQTTSub = jsonEEPROM["fTopic"];
    }
    delay(500);
    Serial.println(charMQTTUser);
    delay(500);
    Serial.println(charMQTTPwd);
    delay(500);
    Serial.println(charMQTTSub);
    delay(500);
    Serial.println(charMQTTPub);
    delay(500);
    
//    const char *id, const char *user, const char *pass, const char* willTopic, uint8_t willQos, boolean willRetain, const char* willMessage, boolean cleanSession
    if (client.connect("ngoc1995", charMQTTUser, charMQTTPwd)) {
      Serial.println("connected");
      // Khi kết nối sẽ publish thông báo
      client.publish(charMQTTPub, "ESP_reconnected");
      // ... và nhận lại thông tin này
      client.subscribe(charMQTTSub);
      Serial.println();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Đợi 5s
      delay(5000);
    }
  }
}

void MQTTClass::LOOP(){
  if (!client.connected()) {
      reconnect();
    }
    client.loop();
}

// convert hex color to rgb(Decimal)
unsigned int MQTTClass::HEXTODEC(String hexString) {

  unsigned int decValue = 0;
  int nextInt;

  for (int i = 0; i < hexString.length(); i++) {

    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);

    decValue = (decValue * 16) + nextInt;
  }

  return decValue;
}
MQTTClass mqttManager = MQTTClass();
