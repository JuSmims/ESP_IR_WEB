
void handleRoot() {
  String tmpString = "No Name";
  tmpString = mainServer.arg("name");
  Serial.println("Accessed Root: " + tmpString);
  mainServer.send(200, "text/html", getMainLayout(tmpString));
}

void handleSave() {
  unsigned long long1 = (unsigned long)((topRaw & 0xFFFF0000) >> 16 );
  unsigned long long2 = (unsigned long)((topRaw & 0x0000FFFF));
  String received = String(long1, HEX) + String(long2, HEX);
  Serial.println("handleSave");
  mainServer.send(200, "text/html", getSave(received));
  delay(100);
}

void handleIr() {
  //for (uint8_t i = 0; i < mainServer.args(); i++) {
  //if (mainServer.argName(i) == "code") {
  //int index = strtoul(mainServer.arg(i).c_str(), NULL, 10);
  int index = strtoul(mainServer.arg("code").c_str(), NULL, 10);
  Serial.println(index);
  sendIrRaw(index);
  //}
  //}
  handleRoot();    //Making the code schmu again
}
void handleNewIr() {
  Serial.println(notreceived);
  if (notreceived && !startedYet) {
    startedYet = true;
    Serial.println("handleNewIr");
    mainServer.send(200, "text/html", getIrWait());
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
        startedYet = false;
        break;
      }
      delay(10);
    }
  }
  else if (!notreceived) {
    notreceived = true;
    Serial.println("handleNewIR Test1");
    handleSave();
  }
  else {
    Serial.println("nothing");
  }
}

void handleDeletion() {
  String indexToDelete = mainServer.arg("code").c_str();
  mainServer.send(200, "text/html", getIRDeletion(indexToDelete));
}

void handleDeleted() {
  int indexToDelete = strtoul(mainServer.arg("code").c_str(), NULL, 10);
  nameList.remove(indexToDelete);
  resultList.remove(indexToDelete);
  handleRoot();
}

void handleSaved() {
  String tmpName = mainServer.arg("name");
  Serial.println("handleSaved");
  if (tmpName.equals("")) {
    tmpName = top;
  }
  nameList.add(tmpName);
  Serial.println(mainServer.arg("name"));
  log();
  resultList.add(top);
  handleRoot();
}

void setupIrServer(const char* ssid, const char* password) {
  irsend.begin();
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  int timeoutConnect = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    timeoutConnect++;
    if (timeoutConnect == 20) {
      noData = true;
      timeoutConnect = 0;
      for (int i = 0; i < 128; i++) {
        EEPROM.write(i, 0xFF);
        Serial.print(EEPROM.read(i));
        EEPROM.commit();
      }
      Serial.println("");
      ESP.reset();
      break;
    }
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

  mainServer.on("/", handleRoot);
  mainServer.on("/ir", handleIr);
  mainServer.on("/addnew", handleNewIr);
  mainServer.on("/save", handleSaved);
  mainServer.on("/notsave", handleRoot);
  mainServer.on("/maybe", handleRoot);
  mainServer.on("/debug", handleSave);
  mainServer.on("/saveHandle", handleSave);
  mainServer.on("/delete", handleDeletion);
  mainServer.on("/ydelete", handleDeleted);

  mainServer.on("/inline", []() {
    mainServer.send(200, "text/plain", "this works as well");
  });

  mainServer.onNotFound(handleRoot);

  mainServer.begin();
  Serial.println("HTTP server started");
}
