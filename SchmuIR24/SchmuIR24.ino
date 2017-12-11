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


const char* ssid = "AndroidAP";
const char* password = "12345679";

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

void handleRoot() {
  String tmpString = "No Name";
  tmpString = server.arg("name");
  Serial.println("Accessed Root: " + tmpString);
  server.send(200, "text/html", getMainLayout(tmpString));
}

void handleSave() {
  Serial.println("handleSave");
  server.send(200, "text/html", "<html>" \
              "<head><title>SIGNAL RECEIVED</title></head>" \
              "<body>" \
              "<h1>You received a signal succesfully</h1>" \
              "<form action='name'><p>Name your signal <input type='text' name='name' size=50 autofocus> <input type='submit' value='Submit'></form>"\
              "<p><a href=\"save\">Save the received signal.</a></p>" \
              "<p><a href=\"notsave\">Decline received signal.</a></p>" \
              "<p><a href=\"maybe\">Maybe save?</a></p>" \
              "</body>" \
              "</html>");
  delay(100);
}

void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      int index = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(i);
      sendIrRaw(i);
    }
  }
  handleRoot();
}

void sendIrRaw(int i){
  String tmp = resultList.get(i);
  Serial.println(tmp);
  int rawSize = 0;
  for(int p=0; p<tmp.length(); p++){
    if(tmp.charAt(i)==','){
      rawSize++;
    }
  }
  Serial.println(rawSize);
  uint16_t tmpRaw[rawSize+1];
//  for(int x=0; x<tmpRaw.length;x++){
//    tmpRaw[x]=getValue(tmp, ',', x);
 // }
}

void handleNewIr() {
  Serial.println("handleNewIr");
  server.send(200, "text/html", getIrWait());
  boolean notreceived = true;
  for(int i=0; i<timeout; i++) {
    Serial.println(".");
    if (irrecv.decode(&results)) {
      // print() & println() can't handle printing long longs. (uint64_t)
      serialPrintUint64(results.value, HEX);
      Serial.println("");
      irrecv.resume();  // Receive the next value
      top = getCode(&results);
      serialPrintUint64(results.value);
      Serial.println("");
      Serial.println("schmu received");
      notreceived = false;
      Serial.println("####");
    }
    if(!notreceived){
      break;
    }
    delay(100);
  }
  if(!notreceived){
  Serial.println("handleNewIR Test1");
  server.sendContent("<html>" \
                     "<head><title>Received Signal</title></head>" \
                     "<body>" \
                     "<p><a href=\"saveHandle\">Okay!</a></p>" \
                     "</body>" \
                     "</html>");
  }
  else{
    handleRoot();
  }
}

String getCode(decode_results *results) {
  String tmpOut;
  for (uint16_t i = 1; i < results->rawlen; i++) {
    uint32_t usecs;
    for (usecs = results->rawbuf[i] * RAWTICK; usecs > UINT16_MAX; usecs -= UINT16_MAX){
    }
    tmpOut+=(String)usecs;
    if (i < results->rawlen - 1){
      tmpOut+=",";
    }
  }
  // Comment
  //encoding(results);
  Serial.print(" ");
  serialPrintUint64(results->value, HEX);
  Serial.println("");

  return tmpOut;
}

void handleSaved() {
  String tmpName = server.arg("name");
  Serial.println("handleSaved");
  if (tmpName.equals("")) {
  /*  unsigned long long1 = (unsigned long)((top & 0xFFFF0000) >> 16 );
    unsigned long long2 = (unsigned long)((top & 0x0000FFFF));
    //May be the wrong conversion... should be tested soon.ml
    tmpName = String(long1, DEC) + String(long2, DEC);*/
    tmpName = "You are top";
  }
  nameList.add(tmpName);
  Serial.println(server.arg("name"));
  checkSchmu();
  resultList.add(top);
  handleRoot();
}

String getMainLayout(String tmpName) {
  if (!tmpName.equals(nameList.get(nameList.size()))) {
    notAdded = true;
    Serial.println(nameList.get(nameList.size()));
  }
  if (notAdded) {
    Serial.println(tmpName);
    if (tmpName.equals("")) {
      //unsigned long long1 = (unsigned long)((top & 0xFFFF0000) >> 16 );
      //unsigned long long2 = (unsigned long)((top & 0x0000FFFF));
      //May be the wrong conversion... should be tested soon.ml
      //tmpName = String(long1, DEC) + String(long2, DEC);
      tmpName = "Choose a name you murderer";
    }
    nameList.add(tmpName);
    resultList.add(top);
    notAdded = false;
  }
  String tmp = "<html>" \
               "<head><title>IR-SMART-REMOTE-ULTRA</title></head>" \
               "<body>" \
               "<h1>You can send and record signals from here" \
               "Please select your option!</h1>" \
               "<p><a href=\"addnew\">Add new IR-Signal</a></p>";

  for (int i = 0; i < resultList.size(); i++) {
    String codeResult = resultList.get(i);
    String tmpCode;
    //unsigned long long1 = (unsigned long)((codeResult & 0xFFFF0000) >> 16 );
    //unsigned long long2 = (unsigned long)((codeResult & 0x0000FFFF));
    //May be the wrong conversion... should be tested soon.ml
    //tmpCode = String(long1, DEC) + String(long2, DEC);
    tmpCode = i;
    tmp += "<p><a href=\"ir?code=" + tmpCode + "\">Code " + nameList.get(i) + "</a></p>";
  }

  tmp += "</body>" \
         "</html>";

  return tmp;
}

void setupIrServer() {
  irsend.begin();
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/ir", handleIr);
  server.on("/addnew", handleNewIr);
  server.on("/save", handleSaved);
  server.on("/notsave", handleRoot);
  server.on("/maybe", handleRoot);
  server.on("/debug", handleSave);
  server.on("/saveHandle", handleSave);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

String getIrWait() {
  String  wait = "<html>"\
         "<style>"\
         "body {"\
         "padding-top: 80px;"\
         "text-align: center;"\
         "font-family: monaco, monospace;"\
     /*    "background: url(https://i.imgur.com/XeR6gnH.gif) 50%;"\ */
         "background-size: auto;"\
       "}"\
         "h1, h2 {"\
         "display: inline-block;"\
         "background: #fff;"\
       "}"\
         "h1 {"\
         "font-size: 30px"\
       "}"\
         "h2 {"\
         "font-size: 20px;"\
       "}"\
         "span {"\
         "background: #fd0;"\
       "}"\
         ".loader {"\
       "position: absolute;"\
         "left: 50%;"\
         "top: 50%;"\
         "z-index: 1;"\
         "width: 150px;"\
         "height: 150px;"\
         "margin: -75px 0 0 -75px;"\
         "border: 16px solid #f3f3f3;"\
         "border-radius: 50%;"\
         "border-top: 16px solid #3498db;"\
         "width: 120px;"\
         "height: 120px;"\
         "-webkit-animation: spin 2s linear infinite;"\
         "animation: spin 2s linear infinite;"\
       "}"\

         "@keyframes spin {"\
         "0% { transform: rotate(0deg); }"\
         "100% { transform: rotate(360deg); }"\
       "}"\

         "</style>"\
         "<h1>Please Wait...<span> Waiting for yout IR Signal</span></h1><br>"\
         "<h2>Please send a Signal now!</h2>"\
         "<div class=\"loader\"></div>"\
         "</html>";
         return wait;
       }

         void checkSchmu(){
         String s=(String)resultList.size();
         Serial.println("Wir haben " + s + " Einträge an IR - Signals");
       Serial.println("Der Name des letzten Elements ist : " +nameList.get(nameList.size()));
       }


