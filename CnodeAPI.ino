/*
  Cnode weareopen API ESP8266 (Walkingrobot)

  This sketch uses an ESP8266 chip to monitor the opening and closing of
  the Walkingrobot hackerspace.
  It serves a html page displaying the current status

  Arduino core for the ESP8266 WiFi chip which can be found here:
  https://github.com/esp8266/Arduino

  The circuit:
  * switchPin: Internal pullup enabled. Connected to GROUND with a switch (the
    space state lever in the space). When the circuit is closed, the pin is
    pulled low and we are considered open. When the circuit is
    opened, the pin is pulled high by the internal pullup and
    the state will switch to closed.

   * led1Pin: Red Led.
   * led2Pin: Green Led.

  Created 7 Aug 2016
  By Ilias Barthelemy
  http://lab.walkingrobot.be
  This software is licensed under MIT.

*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

boolean serialDebug = true;

const char* ssid = "WalkingrobotLab";
const char* password = "";
char dhcp_client_name[] = "spaceopen";

boolean st = false; 

ESP8266WebServer server(80);

int switchPin = 2;                     // Digital pin used to form circuit
int led1Pin = 4;                     
int led2Pin = 5; 

String close_text = "<div class=\"alert alert-danger\"><strong>We are closed.</strong> Check our website for opening hours.</div>";
String open_text = "<div class=\"alert alert-success\"><strong>We are open.</strong> Join us!</div>";

String text1 = "<!DOCTYPE html>"
"<html lang=\"en\">"
"<head>"
"    <meta charset=\"utf-8\">"
"    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"
"    <meta name=\"viewport\" content=\"width=device-width, shrink-to-fit=no, initial-scale=1\">"
"    <title>C-node weareopen API</title>"
"    <link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" rel=\"stylesheet\">"

"    <!--[if lt IE 9]>"
"        <script src=\"https://oss.maxcdn.com/libs/html5shiv/3.7.0/html5shiv.js\"></script>"
"        <script src=\"https://oss.maxcdn.com/libs/respond.js/1.4.2/respond.min.js\"></script>"
"    <![endif]-->"
"</head>"

"<body>"
"    <div id=\"wrapper\">"
"        <div id=\"page-content-wrapper\">"
"            <div class=\"container-fluid\"><br />";

String text2 = "            </div>"
"        </div>"
"    </div>"
"    <script src=\"https://code.jquery.com/jquery-2.2.4.min.js\"></script>"
"    <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>"
"</body>"
"</html>";

void handleRoot() {
  if (!st) {
    if (serialDebug) Serial.println("Space closed");
    server.send(200, "text/html", text1 + close_text + text2);
  }else{
    if (serialDebug) Serial.println("Space open");
    server.send(200, "text/html", text1 + open_text + text2);    
  }
}

void isOpen(){
  if (digitalRead(switchPin) == LOW) {
    st = false;
    digitalWrite(led1Pin, LOW);
    digitalWrite(led2Pin, HIGH);
  }else{
    st = true;
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, LOW);
  }
}

void setup(void){
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  digitalWrite(led1Pin, LOW);
  digitalWrite(led2Pin, LOW);

  if (serialDebug) Serial.begin(115200);
  WiFi.begin(ssid, password);
  if (serialDebug) Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (serialDebug) Serial.print(".");
  }
  if (serialDebug){ 
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  
  if (MDNS.begin(dhcp_client_name)) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.onNotFound(handleRoot);

  server.begin();
    if (serialDebug) Serial.println("HTTP server started");
}

void loop(void){
  isOpen();
  server.handleClient();
}
