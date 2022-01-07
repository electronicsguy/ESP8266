
/**
 * Example for reading temperature and humidity
 * using the DHT22 and ESP8266
 *
 * Copyright (c) 2016 Losant IoT. All rights reserved.
 * https://www.losant.com
 */

#define DHTPIN 4
// what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors
//#define DEBUG 

#include "DHT.h"
#include <DebugMacros.h>
#include <ESP8266WiFi.h>
#include <HTTPSRedirect.h>
#include <PubSubClient.h>

const char* mqttServer = "192.168.20.18";
const int mqttPort = 1883;
const char* mqttUser = "XXX";
const char* mqttPassword = "XXX";

// your wifi ap info
const char* ssid = "XXX";
const char* password = "XXX";

//your gsscript sheet id

const char *GScriptId = "XXX";
const char* host = "script.google.com";

const int dataPostDelay = 900000;     // 15 minutes = 15 * 60 * 1000
const int httpsPort =     443;

String url = String("/macros/s/") + GScriptId + "/exec?";


HTTPSRedirect client(httpsPort);
PubSubClient mqclient;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);
  // Wait for serial to initialize.
  while(!Serial) { }

  Serial.println("setup: Device Started");
  Serial.println("setup: -------------------------------------");
  Serial.println("setup: Running DHT!");
  Serial.println("setup: -------------------------------------");
  Serial.println(ssid);
  Serial.flush();
  dht.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
  }
  Serial.println("setup:IP address: ");
  Serial.println(WiFi.localIP());

  //Insecure for TLS cert checking
  client.setInsecure();
  client.setContentTypeHeader("application/json");
  
  Serial.print("setup: Connecting to ");
  Serial.println(host);
  
  bool flag = false;
  
  for (int i=0; i<5; i++){
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("setup: Initial https Connection failed. Retrying...");
  }
  
  if (!flag){
    Serial.print("setup: Could not connect to server: ");
    Serial.println(host);
    Serial.println("setup: Exiting...");
    return;
  }
  Serial.println("setup: Before MQTT");
  mqclient.setServer(mqttServer, mqttPort);
  mqclient.setCallback(callback);
  while (!mqclient.connected()) {
    Serial.println("setup: Connecting to MQTT...");
 
    if (mqclient.connect("ESP8266Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(mqclient.state());
      delay(2000);
 
    }
  }
 
  mqclient.publish("esp/test", "hello"); //Topic name
  mqclient.subscribe("esp/test");
}
void postData(String tag, float value){
    // this method is cruft and exist in case there is a desire for a Post payload, it was cloned from the Get fucntion, and never taken further
    if (!client.connected()){
            Serial.println("postData: Connecting to client again…");
            client.connect(host, httpsPort);
    }
    String payload =  "tag=" + tag + "&value=" + String(value);
    Serial.print("url");
    //Serial.print(url);
    Serial.print(" host ");
    //Serial.print(host);
    Serial.print("payload ");
    //Serial.print(payload);
    client.POST(url, host, payload);
    Serial.print("getStatusCode: ");
    Serial.print(client.getStatusCode());
    //Serial.print("getReasonPhrase");
    //Serial.print(client.getReasonPhrase());
    Serial.print(client.getResponseBody());
}

//void getData(String tag, float value){
void getData(String Sensorname, float Humidity, float Temperature){
    if (!client.connected()){
            Serial.println("getData:Connecting to client again…");
            client.connect(host, httpsPort);
    }
    //String payload =  "tag=" + tag + "&value=" + String(value);
    String payload =  "Sensorname=" + Sensorname + "&Humidity=" + String(Humidity)+ "&Temperature=" + String(Temperature);
    String url2 = url + payload;
    //Serial.print("GetData: url2\n");
    //Serial.print(url2);
    //Serial.print(" host ");
    //Serial.print(host);
    //Serial.print("payload ");
    //Serial.print(payload);
    client.GET(url2, host);
    Serial.print("getData: getStatusCode: ");
    Serial.print(client.getStatusCode());
    //Serial.print("getReasonPhrase");
    //Serial.print(client.getReasonPhrase());
    //Serial.print(client.getResponseBody());
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}

void loop() {
  // Wait a few seconds between measurements.
  

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("loop: Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("loop: Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
  getData("1710Office01" ,h, f);
  // postData("Humidity" ,h);
  //  #postData("TempC",t);
  //  #postData("TempF",f);
  delay(300000);
  

}
