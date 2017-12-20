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


const char* ssid = "Brio-2.4";
const char* password = "Schliep14933387";

boolean notreceived = true;
boolean notAdded = true;
boolean startedYet=false;   //A normal boolean, just for you.
int timeout = 2000;

String top;
uint64_t topRaw;

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
  unsigned long long1 = (unsigned long)((topRaw & 0xFFFF0000) >> 16 );
  unsigned long long2 = (unsigned long)((topRaw & 0x0000FFFF));
  String received = String(long1, HEX) + String(long2, HEX);
  Serial.println("handleSave");
  server.send(200, "text/html", getSave(received));
  delay(100);
}

void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      int index = strtoul(server.arg(i).c_str(), NULL, 10);
      Serial.println(index);
      sendIrRaw(index);
    }
  }
  handleRoot();    //Making the code schmu again
}

void sendIrRaw(int i) {
  //Saves the code to send in variable
  String tmpIRString = resultList.get(i);
  Serial.println("Nr. " + i);         //Hello
  Serial.println("Sending: " + tmpIRString);
  //counts how much parts (divided by char ',') the String str has
  Serial.println("Temp is " + (String)tmpIRString.length() + " long");
  int rawSize = 0;
  for (int p = 0; p < tmpIRString.length(); p++) {
    //Comparison wont work... UPDATE: IT WORKS.ml
    String test = (String) tmpIRString.charAt(p);
    //Serial.println("comparison test , : "+test);
    yield();
    if (test == ",") {
      Serial.print(".");
      rawSize++;
    }
  }

  //creates a one-dimensional field with the required length to stoer every part of the string as int-sequence
  uint16_t tmpRaw[rawSize + 1];

  //Fills the field with values
  Serial.println("");
  Serial.print("raw[" + (String)rawSize + "] {");       //random comment
  Serial.println("Rawing the array: ");
  for (int x = 0; x < rawSize + 1; x++) {
    //tmpRaw[x] = strtoul(getValue(tmpIRString, ',', x).c_str(), NULL, 0);
    tmpRaw[x] = (uint16_t) getValue(tmpIRString, ',', x).toInt();
    //Serial.print(tmpRaw[x]+",");
    Serial.print(getValue(tmpIRString, ',', x).toInt());
    yield();
  }
  Serial.print("}");

  //finally the code is sent
  irsend.sendRaw(tmpRaw, rawSize + 1, 38);
  Serial.println("Sended!");
}

String getValue(String data, char separator, int index) {
  //a usefull method by an anonymus stackoverflow-user.
  //It simply realises Splitter in C --cool :p
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  //Serial.println("getting Value of: "+data);
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  String valueIs = found > index ? data.substring(strIndex[0], strIndex[1]) : "";
  //Serial.println("Value is: "+valueIs);
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void handleNewIr() {
  Serial.println(notreceived);
   if(notreceived&&!startedYet){
    startedYet=true;
    Serial.println("handleNewIr");
    server.send(200, "text/html", getIrWait());
    for (int i = 0; i < timeout; i++) {
      Serial.println(".");
      if (irrecv.decode(&results)) {
        top = getCode(&results);
        topRaw = results.value;
        // print() & println() can't handle printing long longs. (uint64_t)
        serialPrintUint64(results.value, HEX);
        Serial.println("");
        irrecv.resume();  // Receive the next value
        serialPrintUint64(results.value);
        Serial.println("");
        Serial.println("schmu received: " + top);
        notreceived = false;
        Serial.println("####");
      }
      if (!notreceived) {
        startedYet=false;
        break;
      }
      delay(10);
    }
  }
  else if(!notreceived) {
    notreceived = true;
    Serial.println("handleNewIR Test1");
    handleSave();
  }
  else{
    Serial.println("nothing");
  }
}

String getCode(decode_results *results) {
  // turns results in a field into a String
  String tmpOut;
  for (uint16_t i = 1; i < results->rawlen; i++) {
    uint32_t usecs;
    for (usecs = results->rawbuf[i] * RAWTICK; usecs > UINT16_MAX; usecs -= UINT16_MAX) {
    }
    tmpOut += (String)usecs;
    if (i < results->rawlen - 1) {
      tmpOut += ",";
    }
  }
  Serial.print(" ");
  serialPrintUint64(results->value, HEX);
  Serial.println("");
  return tmpOut;
}

void handleSaved() {
  String tmpName = server.arg("name");
  Serial.println("handleSaved");
  if (tmpName.equals("")) {
    tmpName = top;
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
      tmpName = "Choose a name you murderer";
    }
    nameList.add(tmpName);
    resultList.add(top);
    Serial.println("Added: " + nameList.get(nameList.size() - 1));
    Serial.println("Only top: " + top);
    Serial.println("Kind of Raw: " + resultList.get(resultList.size() - 1));
    notAdded = false;
  }

  String tmp = "<html>"\
               "<title>IR-SMART-HUB</title>"\
               "<style>"\
               "body {"\
               "padding-top: 80px;"\
               "text-align: center;"\
               "font-family: monaco, monospace;"\
               "background: url(https://media.giphy.com/media/lSzQjkthGS1gc/giphy.gif) 50%;"\
               "background-size: cover;"\
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
               "</style>"\
               "<h1>Welcome!<span> IR-SMART-HUB</span></h1><br>"\
               "<h2>You can send and record IR-Signals from here!</h2>"\
               "<br>"\
               "<h2>Please select an option!</h2>"\
               "<br>"\
               "<br>"\
               "<br>"\
               "<button><a href=\"addnew\">Add new IR-Signal</a></button>"\
               "</html>";

  for (int i = 1; i < resultList.size(); i++) {
    String tmpCode;
    Serial.println("Code for button " + nameList.get(i) + " is: " + resultList.get(i));
    tmpCode = i;
    tmp += "<p><a href=\"ir?code=" + tmpCode + "\"><font color=\"#ffffff\">Code " + nameList.get(i) + "</a></p>";
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
  //server.on("/saveHandle", handleSave);

  server.on("/secret", []() {
    server.send(200, "text/plain", "secret. Please don´t tell anyone!");
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
                 "background: url(https://i.imgur.com/XeR6gnH.gif) 50%;"\
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
                 "<h1>Please Wait...<span> Waiting for your IR Signal</span></h1><br>"\
                 "<h2>Please send a Signal now!</h2>"\
                 "<div class=\"loader\"></div>"\
                 "<script>"\
                 "function reloadMe() {"\
                 "location.reload(true);"\
                 "}"\
                 "setInterval(\"reloadMe()\", 1300);"\
                 "</script>"\
                 "</html>";
  return wait;
}

String getSave(String code) {
  String tmpSave = "<style>"\
                   "body {"\
                   "padding-top: 80px;"\
                   "text-align: center;"\
                   "font-family: monaco, monospace;"\
                   "background: url(https://media.giphy.com/media/l3q2Cy90VMhfoA9BC/giphy.gif) 50%;"\
                   "background-size: cover;"\
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
                   "</style>"\
                   "<h1>Received a <span>Signal</span> successfully</h1><br>"\
                   "<h2>"+code+"</h2>"\
                   "<h2>Name your Signal!</h2>"\
                   "<form action='name'><font color=\"#ffffff\"><p><input type='text' name='name' size=50 autofocus> <input type='submit' value='Submit'></form>"\
                   "<p><a href=\"notsave\"><font color=\"#ffffff\">Decline received signal.</a></p>"\
                   "<p><a href=\"maybe\"><font color=\"#ffffff\">Maybe save?</a></p>";
  return tmpSave;
}
void checkSchmu() {
  String s = (String)resultList.size();
  Serial.println("Wir haben " + s + " Einträge an IR - Signals");
  Serial.println("Der Name des letzten Elements ist : " + nameList.get(nameList.size()));
  }
