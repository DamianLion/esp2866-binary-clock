#include <Arduino.h>
#include <FS.h>                   //this needs to be first, or it all crashes and burns...

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

//Time of the day
#include <NTPClient.h>
#include <time.h>
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>

//for LED status
#include <Ticker.h>
Ticker ticker;

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

// Number of Pins
const byte numPins = 5;

// Pinout
const int LED_PIN_1 = 15; // D8
const int LED_PIN_2 = 13; // D7
const int LED_PIN_4 = 12; // D6
const int LED_PIN_8 = 14; // D5
const int LED_PIN_16 = 16; // D0 - High at boot

const byte pins[] = {LED_PIN_1, LED_PIN_2, LED_PIN_4, LED_PIN_8, LED_PIN_16};

WiFiUDP ntpUDP;

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionally you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void tick()
{
  //toggle state
  int state = digitalRead(LED_BUILTIN);  // get the current state of GPIO1 pin
  digitalWrite(LED_BUILTIN, !state);     // set pin to the opposite state
  
  digitalWrite(LED_PIN_1, !state);
  digitalWrite(LED_PIN_2, !state);
  digitalWrite(LED_PIN_4, !state);
  digitalWrite(LED_PIN_8, !state);
  digitalWrite(LED_PIN_16, !state);
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}


// monitor serial
// pio device monitor -b 115200 -p /dev/cu.usbserial-14310

// clear the esp
// esptool.py --chip esp8266 --port /dev/cu.usbserial-14310 erase_flash

// upload
// pio run --target upload
void setup() {
  //pio device monitor -b 115200 -p 4
  Serial.begin(115200);
  Serial.println();

  //set led pin as output
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(LED_PIN_8, OUTPUT);
  pinMode(LED_PIN_16, OUTPUT);

  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  // esptool.py --chip esp8266 --port /dev/cu.usbserial-14310 erase_flash
  // wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  ticker.detach();


  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  // get current time
  timeClient.begin();

  //keep LED on
  digitalWrite(LED_BUILTIN, LOW);

  //turn all other LED off
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_4, LOW);
  digitalWrite(LED_PIN_8, LOW);
  digitalWrite(LED_PIN_16, LOW);
}

void loop() {
  timeClient.update();
  
  Serial.println(timeClient.getFormattedTime());

  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);

  uint8_t day = ti->tm_mday;

  Serial.println(day);

  byte num = day;

  for (byte i = 0; i < numPins; i++) {
    byte state = bitRead(num, i);
    digitalWrite(pins[i], state);
    Serial.print(state);
  }
  
  delay(1000);
}