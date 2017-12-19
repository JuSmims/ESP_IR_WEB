
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
      Serial.println(index);
      sendIrRaw(index);
    }
  }
  handleRoot();
}


void handleNewIr() {
  Serial.println("handleNewIr");
  server.send(200, "text/html", getIrWait());
  boolean notreceived = true;
  for (int i = 0; i < timeout; i++) {
    Serial.println(".");
    if (irrecv.decode(&results)) {
      top = getCode(&results);
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
      break;
    }
    delay(100);
  }
  if (!notreceived) {
    Serial.println("handleNewIR Test1");
    server.sendContent("<html>" \
                       "<head><title>Received Signal</title></head>" \
                       "<body>" \
                       "<p><a href=\"saveHandle\">Okay!</a></p>" \
                       "</body>" \
                       "</html>");
  }
  else {
    handleRoot();
  }
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


void handleSaved() {
  String tmpName = server.arg("name");
  Serial.println("handleSaved");
  if (tmpName.equals("")) {
    tmpName = top;
  }
  nameList.add(tmpName);
  Serial.println(server.arg("name"));
  log();
  resultList.add(top);
  handleRoot();
}

