#include "./DNSServer.h"        

void setupConn() {
  /*Sets up an AP to connect to with simple password and then set the ssid and password and connect with the network*/
  //Setting up an DNS Server to make sure every request is redirected to the login screen
  const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
  IPAddress         apIP(10, 10, 10, 1);    // Private network for server
  DNSServer         dnsServer;              // Create the DNS object
  dnsServer.start(DNS_PORT, "*", apIP);     // "*" makes the DNS respond to every f*cking request
  
  //Setting up the AP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("ESP_Setup");
  
  //Setting up the webserver to get the page
  server.onNotFound(handleSetup);
  Serial.println("Server gesettupped!");
  server.begin();


  while (noData) {
    dnsServer.processNextRequest();
    server.handleClient();
  }
}

void handleSetup(){
  Serial.println("Client requested website");
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "ssid") {
      const char* ssid = server.arg(i).c_str();
      Serial.println(ssid);
    }
    else if(server.argName(i) == "password"){
      const char* password = server.arg(i).c_str();
      Serial.println(password);
      }
  }
  if(ssid!="ESP_Setup"){
    setupIrServer(ssid, password);
    noData=false;
    Serial.println("leaving the setup");
  }
  server.send(200, "text/html", getSetup());
}

