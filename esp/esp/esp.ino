#include "ESP8266WiFi.h"
#include "DHT.h"


//#define analog_pin 5
#define DHT11_PIN 2

const char* ssid = ""; //Enter SSID
const char* password = ""; //Enter 

DHT dht11(DHT11_PIN, DHT11);

void setup(void)
{ 
  Serial.begin(115200);
  //pinMode(analog_pin, OUTPUT);
  dht11.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
     delay(500);
     Serial.print("*");
  }
  
}

void loop() 
{
  delay(1000);
  // read humidity
  float humi  = dht11.readHumidity();
  // read temperature as Celsius
  float tempC = dht11.readTemperature();
  Serial.println(tempC);
  Serial.println(humi);

}
