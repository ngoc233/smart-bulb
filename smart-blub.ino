#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>

#include "WiFiManager.h"
#include "MQTTManager.h"
#include "LedManager.h"

MQTTClass mqtt;

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

//wemos D1
#define RED_PIN 4 
#define GREEN_PIN 12
#define BLUE_PIN 14 
#define WHITE_PIN 5 

//#define RED_PIN 16 
//#define GREEN_PIN 14
//#define BLUE_PIN 12
//#define WHITE_PIN 13 

// rgb value
//int red, green, blue;
//float white;
byte redRom, greenRom,blueRom;

// intersrup variable;
const byte interruptPin = 0;

long lastMsg = 0;
char msg[50];
int value = 0;

// check setting mode
bool stationStatus = false;
bool checkDefaul = false;

//variable for setup AP
const char APName[] = "apiotech";
const char APPwd[] = "iotech.vn";


//count toggle on-off
byte countToggle;
byte addCount = 23;
int timeAmountToggle = 0;

int blinkTime = 0;
bool statusBlinkOn, statusBlinkOff;

// Setup AP
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);

      if (payload[0] == '{') {
        Serial.println("changer MQTT this is mode stations");
        StaticJsonBuffer<500> jsonBuffer;
        JsonObject& clientData = jsonBuffer.parseObject(payload);

        //take data and count length for write to EEPROM
        String newServer = clientData["server"].asString();
        String newPublisher =  clientData["publisher"].asString();
        String newSubcriber =  clientData["subcriber"].asString();
        String newUser =  clientData["user"].asString();
        String newPwd =  clientData["pwd"].asString();
        String newPort = clientData["port"].asString();

        // log for debug data
        Serial.print("server la : ");
        Serial.println(newServer);
        Serial.print("pub la : ");
        Serial.println(newPublisher);
        Serial.print("sub la : ");
        Serial.println(newSubcriber);
        Serial.print("user la : ");
        Serial.println(newUser);
        Serial.print("pwd la : ");
        Serial.println(newPwd);
        Serial.print("port la : ");
        Serial.println(newPort);

        // write to eeproom
        EEPROM.write(94, (int)newPort.length());
        EEPROM.write(95, (int)newServer.length());
        EEPROM.write(96, (int)newPublisher.length());
        EEPROM.write(97, (int)newSubcriber.length());
        EEPROM.write(98, (int)newUser.length());
        EEPROM.write(99, (int)newPwd.length());
        int total = 100;
        Serial.println("write to eeprom");
        EEPROM.put(100, 'a');
        EEPROM.commit();
        //write server to eeproom
        for (int i = 0; i < newServer.length(); i++)
        {
          EEPROM.put(total, newServer[i]);
          total++;
          EEPROM.commit();
        }
        //write publlisher to eeprom
        for (int i = 0; i < newPublisher.length(); i++)
        {
          EEPROM.put(total, newPublisher[i]);
          total++;
          EEPROM.commit();
        }
        //write subcriber to eeprom
        for (int i = 0; i < newSubcriber.length(); i++)
        {
          EEPROM.put(total, newSubcriber[i]);
          total++;
          EEPROM.commit();
        }
        //write user to eeprom
        for (int i = 0; i < newUser.length(); i++)
        {
          EEPROM.put(total, newUser[i]);
          total++;
          EEPROM.commit();
        }
        //write pwd to eeprom
        for (int i = 0; i < newPwd.length(); i++)
        {
          EEPROM.put(total, newPwd[i]);
          total++;
          EEPROM.commit();
        }
        //write port to eeprom
        for (int i = 0; i < newPort.length(); i++)
        {
          EEPROM.put(total, newPort[i]);
          total++;
          EEPROM.commit();
        }
      }
      else
      {
        Serial.println("dữ liệu đẩy lên không phải json ");
      }
      break;
  }
}

void setupAP()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(APName, APPwd);
}

void setUpRGB()
{
//  //read rgb from EEPROM;
//  if((int)EEPROM.read(10) != 255 && (int)EEPROM.read(10) != 0)
//  {
////    red = EEPROM.read(10);
////    red = 1023 - (red*4) - 3;
////    wemos D1 
//    red = (red*4) + 3;
//    Serial.println();
//    Serial.print("set up red la :");
//    Serial.println(red);
//  }
//  if((int)EEPROM.read(11) != 255 && (int)EEPROM.read(11) != 0)
//  {
////    green = EEPROM.read(11);
////    green = 1023 - (green*4) - 3;
////    wemos D1
//    green = (green*4) + 3;
//    Serial.println();
//    Serial.print("set up green la :");
//    Serial.println(green);
//  }
//  if((int)EEPROM.read(12) != 255 && (int)EEPROM.read(12) != 0)
//  {
////    blue = EEPROM.read(12);
////    blue = 1023 - (blue*4) - 3;
////    wemos D1
//    blue = (blue*4) + 3;
//    Serial.println();
//    Serial.print("set up blue la :");
//    Serial.println(blue);
//  }else
//  {
//    Serial.println("khong co gi trong room");
//  }
//  
//  red = EEPROM.read(10);
//  red = (red*4) +3;
//  green = EEPROM.read(11);
//  green = (green*4) +3;
//  blue = EEPROM.read(12);
//  blue = (blue*4) +3;
//  Serial.println(red);
//  Serial.println(green);
//  Serial.println(blue);
//  analogWrite(RED_PIN, red);
//  analogWrite(GREEN_PIN, green);
//  analogWrite(BLUE_PIN, blue);
}

void resetToggle()
{
  EEPROM.write(addCount, 0);
  EEPROM.commit();
  checkDefaul  = false;
  stationStatus = false;
  countToggle = 0;
  Serial.println("reset");
}

void setup() {
  //Khởi tạo baud 115200
  Serial.begin(115200);
  
  //init calinterrup pin 
  pinMode(0, INPUT_PULLUP);


  EEPROM.begin(512);

  //setup RGB
  setUpRGB();
  
  // setup MQTT new or default
  mqtt.SETUP();
  mqtt.RUN();
  mqtt.SETCALLBACK();

//  get mac ID
  wifiManager.GETMACID();


  countToggle = EEPROM.read(addCount);
  switch (countToggle)
  {
    case 0 :
      EEPROM.write(addCount, 1);
      EEPROM.commit();
      countToggle = 1;
      break;
    case 1 :
      EEPROM.write(addCount, 2);
      EEPROM.commit();
      countToggle = 2;
      break;
    case 2 :
      EEPROM.write(addCount, 3);
      EEPROM.commit();
      countToggle = 3;
      stationStatus = true;
      //delete  the saved configuration of wifi in STA mode 
      delay(500);
      WiFi.disconnect(true);
      Serial.println("chinh xac ma");
      break;
    case 3 :
      EEPROM.write(addCount, 4);
      EEPROM.commit();
      countToggle = 4;
      stationStatus =true;
      break;
    case 4 :
      EEPROM.write(addCount, 5);
      EEPROM.commit();
      stationStatus =true;
      countToggle = 5;
      break;
    case 5 :
      EEPROM.write(addCount, 6);
      EEPROM.commit();
      countToggle = 6;
      checkDefaul = true;

      Serial.println("setup AP ......");
      setupAP();
      // start webSocket server
      webSocket.begin();
      webSocket.onEvent(webSocketEvent);

      if (MDNS.begin("esp8266")) {
        Serial.println("MDNS responder started");
      }

      // handle index
//      server.on("/", []() {
//        // send index.html
//        server.send(200, "text/html", "<html><head><script>var connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);connection.onopen = function () {  connection.send('Connect ' + new Date()); }; connection.onerror = function (error) {    console.log('WebSocket Error ', error);};connection.onmessage = function (e) {  console.log('Server: ', e.data);};function sendRGB() {  var r = parseInt(document.getElementById('r').value).toString(16);  var g = parseInt(document.getElementById('g').value).toString(16);  var b = parseInt(document.getElementById('b').value).toString(16);  if(r.length < 2) { r = '0' + r; }   if(g.length < 2) { g = '0' + g; }   if(b.length < 2) { b = '0' + b; }   var rgb = '#'+r+g+b;    console.log('RGB: ' + rgb); connection.send(rgb); }</script></head><body>LED Control:<br/><br/>R: <input id=\"r\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" onchange=\"sendRGB();\" /><br/>G: <input id=\"g\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" onchange=\"sendRGB();\" /><br/>B: <input id=\"b\" type=\"range\" min=\"0\" max=\"255\" step=\"1\" onchange=\"sendRGB();\" /><br/></body></html>");
//      });

      server.begin();

      // Add service to MDNS
//      MDNS.addService("http", "tcp", 80);
      MDNS.addService("ws", "tcp", 81);

      break;
    case 6:
      EEPROM.write(addCount, 7);
      EEPROM.commit();
      countToggle = 7;
      break;
    default:
      countToggle = 7;
      break;
  }

  //setup interrup
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), saveRGB, FALLING);

}

void saveRGB()
{
  EEPROM.write(10, redRom );
  EEPROM.write(11, greenRom);
  EEPROM.write(12, blueRom);
  EEPROM.commit();
}
void loop() {

  // if over 3 seconds
  if ((unsigned long) (millis() - timeAmountToggle) > 5000 )
  {
    if (countToggle >= 1)
    {
      if (countToggle >= 3 && countToggle < 6)
      {
        wifiManager.SETUP();
        wifiManager.RUN();
        if(wifiManager.CONNECTION())
        {
          resetToggle();
        }
      }
      else if (countToggle == 6)
      {
        webSocket.loop();
        server.handleClient();
      }
      else
      {
        resetToggle();
        countToggle = 0;
      }
    }

  }

  if(stationStatus)
  {
    wifiManager.RECONNECT();
  }
  if (wifiManager.CONNECTION())
  {
    mqtt.LOOP();
  }
  
}
