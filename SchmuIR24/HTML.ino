
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
                 "<h1>Please Wait...<span> Waiting for yout IR Signal</span></h1><br>"\
                 "<h2>Please send a Signal now!</h2>"\
                 "<div class=\"loader\"></div>"\
                 "</html>";
  return wait;
}


String getMainLayout(String tmpName) {
  if (!tmpName.equals(nameList.get(nameList.size()))) {
    notAdded = true;
    Serial.println(nameList.get(nameList.size()));
  }
  if (notAdded) {
    Serial.println(tmpName);
    if (tmpName.equals("")){
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

  for (int i = 0; i < resultList.size(); i++) {
    String tmpCode;
    Serial.println("Code for button "+nameList.get(i)+" is: "+ resultList.get(i));
    tmpCode = i;
    tmp += "<p><a href=\"ir?code=" + tmpCode + "\">Code " + nameList.get(i) + "</a></p>";
  }

  tmp += "</body>" \
         "</html>";

  return tmp;
}
