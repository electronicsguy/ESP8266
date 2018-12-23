/*  HTTPS on ESP8266 with follow redirects, chunked encoding support
 *  Version 3.0
 *  Author: Sujay Phadke
 *  Github: @electronicsguy
 *  Copyright (C) 2018 Sujay Phadke <electronicsguy123@gmail.com>
 *  All rights reserved.
 *
 *  Example Arduino program
 */

#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include "DebugMacros.h"

// Fill ssid and password with your network credentials
const char* ssid = "";
const char* password = "";

const char* host = "script.google.com";
// Replace with your own script id to make server side changes
const char *GScriptId = "AKfycbzYw5G-oxvnwHpAJfDsS0PWNrO0KTBMiCW78lHUcEO6ZnFHvSw";

const int httpsPort = 443;

// echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout
const char* fingerprint = "";
//const uint8_t fingerprint[20] = {};

// Write to Google Spreadsheet
String url = String("/macros/s/") + GScriptId + "/exec?value=Hello";
// Fetch Google Calendar events for 1 week ahead
String url2 = String("/macros/s/") + GScriptId + "/exec?cal";
// Read from Google Spreadsheet
String url3 = String("/macros/s/") + GScriptId + "/exec?read";

String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"Sheet1\", \
                    \"values\": ";
String payload = "";

HTTPSRedirect* client = nullptr;
// used to store the values of free stack and heap
// before the HTTPSRedirect object is instantiated
// so that they can be written to Google sheets
// upon instantiation
unsigned int free_heap_before = 0;
unsigned int free_stack_before = 0;

void setup() {
  Serial.begin(115200);
  Serial.flush();
  
  free_heap_before = ESP.getFreeHeap();
  free_stack_before = ESP.getFreeContStack();
  Serial.printf("Free heap: %u\n", free_heap_before);
  Serial.printf("Free stack: %u\n", free_stack_before);
  
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  // flush() is needed to print the above (connecting...) message reliably, 
  // in case the wireless connection doesn't go through
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }

/*  
  if (client->setFingerprint(fingerprint)) {
    Serial.println("Certificate match.");
  } else {
    Serial.println("Certificate mis-match");
  }
*/ 

  // Send memory data to Google Sheets
  payload = payload_base + "\"" + free_heap_before + "," + free_stack_before + "\"}";
  client->POST(url2, host, payload, false);
  payload = payload_base + "\"" + ESP.getFreeHeap() + "," + ESP.getFreeContStack() + "\"}";
  client->POST(url2, host, payload, false);
  
  // Note: setup() must finish within approx. 1s, or the the watchdog timer
  // will reset the chip. Hence don't put too many requests in setup()
  // ref: https://github.com/esp8266/Arduino/issues/34
  
  Serial.println("\nGET: Write into cell 'A1'");
  Serial.println("=========================");

  // fetch spreadsheet data
  client->GET(url, host);

  // Send memory data to Google Sheets
  payload = payload_base + "\"" + ESP.getFreeHeap() + "," + ESP.getFreeContStack() + "\"}";
  client->POST(url2, host, payload, false);
  
  Serial.println("\nGET: Fetch Google Calendar Data:");
  Serial.println("================================");

  // fetch spreadsheet data
  client->GET(url2, host);

  // Send memory data to Google Sheets
  payload = payload_base + "\"" + ESP.getFreeHeap() + "," + ESP.getFreeContStack() + "\"}";
  client->POST(url2, host, payload, false);
  
  Serial.println("\nSeries of GET and POST requests");
  Serial.println("===============================");
  
  Serial.printf("Free heap: %u\n", ESP.getFreeHeap());
  Serial.printf("Free stack: %u\n", ESP.getFreeContStack());

  // delete HTTPSRedirect object
  delete client;
  client = nullptr;
}

void loop() {
  static int error_count = 0;
  static int connect_count = 0;
  const unsigned int MAX_CONNECT = 20;
  static bool flag = false;
  //Serial.printf("Free heap: %u\n", ESP.getFreeHeap());
  //Serial.printf("Free stack: %u\n", ESP.getFreeContStack());
  
  if (!flag){
    free_heap_before = ESP.getFreeHeap();
    free_stack_before = ESP.getFreeContStack();
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }

  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
      payload = payload_base + "\"" + free_heap_before + "," + free_stack_before + "\"}";
      client->POST(url2, host, payload, false);
    }
  }
  else{
    DPRINTLN("Error creating client object!");
    error_count = 5;
  }
  
  if (connect_count > MAX_CONNECT){
    //error_count = 5;
    connect_count = 0;
    flag = false;
    delete client;
    return;
  }

  Serial.println("GET Data from cell 'A1':");
  if (client->GET(url3, host)){
    ++connect_count;
  }
  else{
    ++error_count;
    DPRINT("Error-count while connecting: ");
    DPRINTLN(error_count);
  }

  Serial.println("POST append memory data to spreadsheet:");
  payload = payload_base + "\"" + ESP.getFreeHeap() + "," + ESP.getFreeContStack() + "\"}";
  if(client->POST(url2, host, payload)){
    ;
  }
  else{
    ++error_count;
    DPRINT("Error-count while connecting: ");
    DPRINTLN(error_count);
  }

  /*
  if (!(client.reConnectFinalEndpoint())){
    ++error_count;
    DPRINT("Error-count while connecting: ");
    DPRINTLN(error_count);
  }
  else
    error_countunt = 0;
  */
  
  if (error_count > 3){
    Serial.println("Halting processor..."); 
    delete client;
    client = nullptr;
    Serial.printf("Final free heap: %u\n", ESP.getFreeHeap());
    Serial.printf("Final stack: %u\n", ESP.getFreeContStack());
    Serial.flush();
    ESP.deepSleep(0);
  }
  
  // In my testing on a ESP-01, a delay of less than 1500 resulted 
  // in a crash and reboot after about 50 loop runs.
  delay(4000);
                          
}
