#include "ESP8266WiFi.h"
#include <DHT.h>

#define DHT11_PIN 13
#define DHT_SENSOR_TYPE DHT11
const char* ssid = "TELUS7A66";             //Enter SSID
const char* password = "y7YXdupttvhJ";      //Enter password
const char* server_name = "http:://:8080";  //Enter server name

DHT dht11(DHT11_PIN, DHT_SENSOR_TYPE);

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {}
  dht11.begin();
  delay(5000);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //    delay(500);
  //    Serial.print("*");
  // }
  // read humidity
  float humi = dht11.readHumidity();
  // read temperature as Celsius
  float temp = dht11.readTemperature();
  Serial.println(temp);
  Serial.println(humi);
  // if(WiFi.status()== WL_CONNECTED){
  //   WiFiClient client;
  //   HTTPClient http;

  //   // Your Domain name with URL path or IP address with path
  //   http.begin(client, serverName);

  //   // If you need an HTTP request with a content type: application/json, use the following:
  //   //http.addHeader("Content-Type", "application/json");
  //   //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

  //   // If you need an HTTP request with a content type: text/plain
  //   //http.addHeader("Content-Type", "text/plain");
  //   //int httpResponseCode = http.POST("Hello, World!");

  //   Serial.print("HTTP Response code: ");
  //   Serial.println(httpResponseCode);

  //   // Free resources
  //   http.end();
  // }
  Serial.println("I'm awake, but I'm going into deep sleep mode until RESET pin is connected to a LOW signal");
  ESP.deepSleep(10 * 100000);  // 10 seconds = 10 million microseconds
  //ESP.deepSleep(60000000);  // 60 million microseconds = 1 minute
}

void loop() {
}
