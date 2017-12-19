void sendIrRaw(int i) {
  //Saves the code to send in variable
  String tmpIRString = resultList.get(i);
  Serial.println("Nr. "+i);
  Serial.println("Sending: " + tmpIRString);
  //counts how much parts (divided by char ',') the String str has
  Serial.println("Temp is "+(String)tmpIRString.length()+" long");
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
  Serial.print("raw["+(String)rawSize+"] {");
  Serial.println("Rawing the array: ");
  for (int x = 0; x < rawSize+1; x++) {
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
  //It simply realises Splitter in C
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
