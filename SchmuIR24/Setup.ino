#include "./DNSServer.h"
#include <EEPROM.h>
        
DNSServer         dnsServer;                // Create the DNS object
String password;
String ssid;

boolean remember = false;

void setupConn() {
  
  for(int i=0; i<64; i++){
    char c = EEPROM.read(i);
    if(c != 0xFF){
      ssid += c;
    }
  }
  //Serial.println(ssid);

  for(int i=64; i<128; i++){
    char c = EEPROM.read(i);
    if(c != 0xFF){
      password += c;
    }
  }
  Serial.println("SSID: "+ssid+" Password: "+password);
  /*Sets up an AP to connect to with simple password and then set the ssid and password and connect with the network*/
  //Setting up an DNS Server to make sure every request is redirected to the login screen
  const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
  IPAddress         apIP(10, 10, 10, 1);    // Private network for server
  dnsServer.start(DNS_PORT, "*", apIP);     // "*" makes the DNS respond to every f*cking request
  
  //Setting up the AP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("IR-HUB_Setup");
  
  //Setting up the webserver to get the page
  setupServer.on("/", handleSetup);
  setupServer.on("/setup",handleSetup);
  setupServer.on("/remember", handleRemember);
  Serial.println("Server gesettupped!");
  
  if(!serverNotNotOn){
    serverNotNotOn = true;
    setupServer.begin();
  }
  if(password != "" && ssid !=""){
    handleRemember();
    noData = false;
  }
  while (noData) {
    dnsServer.processNextRequest();
    setupServer.handleClient();
  }
  Serial.println("noData: "+noData);
  
  
}

void handleRemember(){
  remember = true;
  handleSetup();
}

void handleSetup(){
  Serial.println("Client requested website");
  String pass;
  String id;
  if(setupServer.args()>1){
  pass = setupServer.arg("password");
  id = setupServer.arg("ssid");
  Serial.println(id);
  Serial.println(pass);
  }
  if(id!="" && pass!="" ){
    saveLogin(id, pass);
    Serial.println("leaving the setup");
    WiFi.softAPdisconnect(true);
    dnsServer.stop();
    setupServer.stop();
    setupIrServer(id.c_str(), pass.c_str());
    noData=false;
  }
  if(remember){
    Serial.println("leaving the setup, i remembered.");
    WiFi.softAPdisconnect(true);
    dnsServer.stop();
    setupServer.stop();
    setupIrServer(ssid.c_str(), password.c_str());
    noData=false;
    remember=false;
  }
  setupServer.send(200, "text/html", getSetup(ssid));
}

void saveLogin(String id, String pass){
  Serial.println("id: "+id+ " pass: "+pass); 
  Serial.println("##############################################");
  for(int i=0; i<64; i++){
    if(id.length()>i){
      EEPROM.write(i, id.charAt(i));
      Serial.print((char)EEPROM.read(i));
    }
    else{
      EEPROM.write(i,  0xFF);
      Serial.print((char)EEPROM.read(i));
    }
  }
  Serial.println("");
  Serial.println(id);
    for(int i=64; i<128; i++){
    if(pass.length()>i-64){
      EEPROM.write(i, pass.charAt(i-64));
      Serial.print((char)EEPROM.read(i));
    }
    else{
      EEPROM.write(i,  0xFF);
      Serial.print((char)EEPROM.read(i));
    }
  }
  Serial.println("");
  Serial.println(pass);
  Serial.println("##############################################");
  EEPROM.commit();
}

