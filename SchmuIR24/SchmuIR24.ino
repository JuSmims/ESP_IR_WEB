#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRsend.h>
#include <WiFiClient.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <LinkedList.h>
//#include <boost/lexical_cast.hpp>

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).


const char* ssid = "UPCBD2D355";
const char* password = "Rt2Stdfhpcnd";

boolean notAdded = true;
int timeout = 200;

String top;

MDNSResponder mdns;

ESP8266WebServer server(80);

IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)

uint16_t RECV_PIN = 14;

IRrecv irrecv(RECV_PIN);

decode_results results;

unsigned long prevTime = 0;

LinkedList<String> resultList = LinkedList<String>();
LinkedList<String> nameList = LinkedList<String>();

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();  // Start the receiver
  setupIrServer();
  //ESP.wdtDisable();
}

void loop() {
  server.handleClient();
}

void log() {
  String s = (String)resultList.size();
  Serial.println("Wir haben " + s + " Einträge an IR - Signals");
  Serial.println("Der Name des letzten Elements ist : " + nameList.get(nameList.size()));
  }
