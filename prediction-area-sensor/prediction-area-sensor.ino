/*
 * Indicated Area Prediction System for Shopping
 * - Pressure Sensor
 * @author Thais Watanabe
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define LED 13

// router ssid & pass
const char* ssid = "KawalabStudy-g";
const char* password = "kawalab0621";

// setting server
ESP8266WebServer server(80);

// node.js server info
const char* nodeHost = "192.168.11.8";

// clinet
WiFiClient client;

// handling pages
void handleNotFound();
void handleRoot();

// working with esp-wroom
extern "C" {
#include "user_interface.h"
}

// variables
float R1=5.1,rf,fg;
int n;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  connectingToWiFi();
}

/*
 * Make connection with wifi
 */
void connectingToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ");
  Serial.print("Connected with IP address: ");
  Serial.println(WiFi.localIP());

  // setting handlers
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
}

void loop() {
  server.handleClient();
  n = system_adc_read(); // Arduino: n=analogRead(0);

  rf=R1*n/(1024.0-n);
  fg=880.79/rf + 47.962;

  if (fg > 800) {
    digitalWrite(LED, HIGH);
    sendSensorData(true);
  } else {
    digitalWrite(LED, LOW);
    sendSensorData(false);
  }

  delay(1000);
}

void handleNotFound() {
  server.send(404, "text/plain" "Not Found");
}

void handleRoot() {
  server.send(200, "text/plain", "Ok");
}

/*
 * Send information to Node.js server
 */
void sendSensorData(bool hasSomeone) {
  //Serial.println("\nSending information to server...\n");
  if(!client.connect(nodeHost, 3000))     // aqui conectamos ao servidor
  {
    Serial.print("Could not connect to host: " + String(nodeHost) + "\n");
  }
  else
  {
    // make HTTP POST request
    client.println("POST /person HTTP/1.1");
    client.println("Host: ") + String(nodeHost);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");

    String json = "";

    if (hasSomeone == true) {
      json = "{\"hasSomeone\":true}";
    } else {
      json = "{\"hasSomeone\":false}";
    }
    client.println(json.length());
    client.println();
    client.println(json);

    Serial.println("\nInformation sent with success!");
  }
}
