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
  WiFi.softAP("IR-HUB_Setup");
  
  //Setting up the webserver to get the page
  server.on("/", handleSetup);
  server.on("/setup",handleSetup);
  Serial.println("Server gesettupped!");
  if(!serverNotNotOn){
  serverNotNotOn = true;
  server.begin();
  }

  while (noData) {
    dnsServer.processNextRequest();
    server.handleClient();
  }
}

/*void handleRootSetup(){
  server.send(200, "text/html", "<p><a href=\"setup\">Start Setup</a></p>");
}*/

void handleSetup(){
  Serial.println("Client requested website");
  String password;
  String ssid;
  if(server.args()>1){
  password = server.arg("password");
  ssid = server.arg("ssid");
  Serial.println(ssid);
  Serial.println(password);
  }
  if(ssid!="" && password!="" ){
    Serial.println("leaving the setup");
    WiFi.softAPdisconnect(true);
    setupIrServer(ssid.c_str(), password.c_str());
    noData=false;
  }
  server.send(200, "text/html", getSetup());
}

