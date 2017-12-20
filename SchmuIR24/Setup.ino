void setupConn() {
  /*Sets up an AP to connect to with simple password and then set the ssid and password and connect with the network*/
  WiFi.softAP(ssid, password);
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleSetup);
  //server.on("/debug2", handleDebug);
  server.onNotFound(handleSetup);
  Serial.println("Server gesettupped");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  while (noData) {
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

