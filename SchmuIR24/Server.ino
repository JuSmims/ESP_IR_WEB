
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
