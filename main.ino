#define BLYNK_TEMPLATE_ID "TMPL6nU4VJl8s"
#define BLYNK_TEMPLATE_NAME "Air quality monitoring system"
#define BLYNK_AUTH_TOKEN "sIRcKdiGHTmzegN4XnM70PUkSMwIR81l"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include "MQ135.h"
#include <ESP8266WiFi.h>

String apiKey = "4E3YURQIVHZRVDLN";  // Enter your Write API key from ThingSpeak
const char *ssid = "manjilatmg15_dhrn_2.4";   // replace with your WiFi SSID_dhrn_
const char *pass = "CLEB2D7443"; // replace with your WiFi password
const char *server = "api.thingspeak.com";
const int sensorPin = 0;
int air_quality;

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

int gas = A0;
int sensorThreshold = 100;

#define DHTPIN 2     // Connect Out pin to D4 in NODE MCU
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

WiFiClient client;

void sendToThingSpeak(float temperature, float humidity, int airQuality)
{
  if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
  {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temperature);
    postStr += "&field2=";
    postStr += String(humidity);
    postStr += "&field3=";
    postStr += String(airQuality);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" degrees Celsius, Humidity: ");
    Serial.print(humidity);
    Serial.print("%, Air Quality: ");
    Serial.print(airQuality);
    Serial.println(" PPM. Sent to ThingSpeak.");
  }
  client.stop();
}

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int analogSensor = analogRead(gas);

  Blynk.virtualWrite(V2, analogSensor);
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  Serial.print("Gas Value: ");
  Serial.println(analogSensor);
  Serial.print("Temperature : ");
  Serial.print(t);
  Serial.print("    Humidity : ");
  Serial.println(h);

  // Send data to ThingSpeak
  sendToThingSpeak(t, h, analogSensor);

  // Predefined Advice based on Air Quality
  if (analogSensor > 120)
  {
    Blynk.logEvent("pollution_alert", "Bad Air");
    
  }
  else if (analogSensor > 80)
  {
    Blynk.logEvent("moderate_air_quality", "Moderate Air");
    
  }
  else
  {
    Blynk.logEvent("good_air_quality", "Good Air");
  }
}

void setup()
{
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(2000L, sendSensor);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop()
{
  Blynk.run();
  timer.run();
}