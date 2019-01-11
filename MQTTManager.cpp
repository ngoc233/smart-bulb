#include "MQTTManager.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "WiFiManager.h"
#include "LedManager.h"

#define mqtt_server "m10.cloudmqtt.com" // could mqtt server
#define mqtt_topic_pub "pubLedBulb"   // could mqtt topic pub
#define mqtt_topic_sub "subLedBulb"   // could mqtt topic sub
#define mqtt_user "vpfiytsx"    // could mqtt user
#define mqtt_pwd "q2M1pcYye1IV" // could mqtt password
uint16_t mqtt_port = 10261; //could mqtt port

// tạo hằng để lưu new MQTT
const char* charMQTTServer;
const char* charMQTTPub;
const char* charMQTTSub;
const char* charMQTTUser;
const char* charMQTTPwd;

// biến để lưu độ dài
byte serverLength, pubLength, subLength, userLength, pwdLength, portLength;

// rgb value
int red, green, blue;
float white;

WiFiClient espClient;
PubSubClient client(espClient);

MQTTClass::MQTTClass(){
  
}

void MQTTClass::SETUP(){
 // if have length value on rom-95 -> write
  if (EEPROM.read(95) == 255 || EEPROM.read(95) == 0)
  {
    // để mặc định các giá trị
    charMQTTServer = mqtt_server;
    charMQTTPub = mqtt_topic_pub;
    charMQTTSub = mqtt_topic_sub;
    charMQTTUser = mqtt_user;
    charMQTTPwd = mqtt_pwd;
    Serial.println("set default MQTT");
    Serial.println(charMQTTServer);
  }
  else
  {
    portLength = EEPROM.read(94);
    serverLength = EEPROM.read(95);
    pubLength = EEPROM.read(96);
    subLength = EEPROM.read(97);
    userLength = EEPROM.read(98);
    pwdLength = EEPROM.read(99);

    //get server from eeprom
    String svString, pubString, subString, userString, pwdString, portString;

    // lấy độ dài và giá trị của new cloud
    int countMQTTLength = 100;
    for (int i = 0; i < serverLength; i++)
    {
      svString += char(EEPROM.read(countMQTTLength));
      countMQTTLength++;
    }
    for (int i = 0; i < pubLength; i++)
    {
      pubString += char(EEPROM.read(countMQTTLength));
      countMQTTLength++;
    }
    for (int i = 0; i < subLength; i++)
    {
      subString += char(EEPROM.read(countMQTTLength));
      countMQTTLength++;
    }
    for (int i = 0; i < userLength; i++)
    {
      userString += char(EEPROM.read(countMQTTLength));
      countMQTTLength++;
    }
    for (int i = 0; i < pwdLength; i++)
    {
      pwdString += char(EEPROM.read(countMQTTLength));
      countMQTTLength++;
    }
    for (int i = 0; i < portLength; i++)
    {
      portString += char(EEPROM.read(countMQTTLength));
      countMQTTLength++;
    }

    // make json object and set value for new cloud
    StaticJsonBuffer<500> jsonEEPBuffer;
    JsonObject& jsonEEPROM  = jsonEEPBuffer.createObject();
    jsonEEPROM["server"] = svString;
    jsonEEPROM["publish"] = pubString;
    jsonEEPROM["subcribe"] = subString;
    jsonEEPROM["user"] = userString;
    jsonEEPROM["pwd"] = pwdString;
    jsonEEPROM["port"] = portString;

    mqtt_port = portString.toInt();
    charMQTTServer = jsonEEPROM["server"];
    charMQTTPub = jsonEEPROM["publish"];
    charMQTTSub = jsonEEPROM["subcribe"];
    charMQTTUser = jsonEEPROM["user"];
    charMQTTPwd = jsonEEPROM["pwd"];
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
    Serial.print(receivedChar);
  }
  Serial.print("string mess la : ");
  Serial.println(stringMessage);
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(stringMessage);
  int modeMess = (int)root["mode"];
  String stringMode = root["mode"].asString();
  JsonArray& arrayMode = (root["id"]);
  for(int i=0; i<arrayMode.size();i++)
  {
    String deviceID = arrayMode[i]["idDevice"];
    if(deviceID == macID)
    {
      Serial.println("true device ID");
      if(modeMess == 1)
      {
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
      }
      else if (modeMess == 0)
      {
        ledManager.OFFANLED(RED_PIN);
        ledManager.OFFANLED(GREEN_PIN);
        ledManager.OFFANLED(BLUE_PIN);
        ledManager.OFFDILED(WHITE_PIN);
      } else if (modeMess == 2)
      {
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
    // Thực hiện kết nối với mqtt user và pass
    delay(500);
    Serial.println(charMQTTUser);
    delay(500);
    Serial.println(charMQTTPwd);
    delay(500);
    Serial.println(charMQTTSub);
    delay(500);
    Serial.println(charMQTTPub);
    delay(500);
    
    Serial.println("khong hieu");
    if (client.connect("Ngoc233", charMQTTUser, charMQTTPwd)) {
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
