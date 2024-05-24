/*
This program is designed to read temperature and humidity data from a DHT20 sensor
and upload the readings to a ThingSpeak channel. The program also retrieves the 
current time from an NTP (Network Time Protocol) server and logs the sensor data
with timestamps.
 
The main components used in this program are:
DHT20 Sensor: Used for reading temperature and humidity data.
ESP8266 Microcontroller: Connects to Wi-Fi, reads sensor data, and uploads data to ThingSpeak.
ThingSpeak: An IoT platform to visualize and analyze live data.
NTPClient: Retrieves the current time from an NTP server.
*/

#include <Wire.h>
#include "DHT20.h"
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

DHT20 DHT;

const char* ssid = "**********";         // Replace with your network SSID (name)
const char* password = "***********"; // Replace with your network password

unsigned long myChannelNumber = 012345;  // Replace with your ThingSpeak Channel Number
const char* myWriteAPIKey = "*****************";           // Replace with your ThingSpeak Write API Key

WiFiClient client;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Update every 60 seconds

uint8_t count = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("DHT20 LIBRARY VERSION: ");
  Serial.println(DHT20_LIB_VERSION);
  Serial.println();

  Wire.begin();
  DHT.begin();    //  ESP8266 default pins

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");

  ThingSpeak.begin(client);

  timeClient.begin();
  timeClient.setTimeOffset(0); // Adjust according to your timezone offset in seconds

  delay(1000);
}

void loop()
{
  timeClient.update();

  if (millis() - DHT.lastRead() >= 1000)
  {
    //  READ DATA
    uint32_t start = micros();
    int status = DHT.read();
    uint32_t stop = micros();

    if ((count % 10) == 0)
    {
      count = 0;
      Serial.println();
      Serial.println("Type\tHumidity (%)\tTemp (°C)");
    }
    count++;

    float OfficeHumidity = DHT.getHumidity();
    float OfficeTemperature = DHT.getTemperature();

    Serial.print("DHT20 \t");
    //  DISPLAY DATA, sensor has only one decimal.
    Serial.print(OfficeHumidity, 1);
    Serial.print("\t\t");
    Serial.print(OfficeTemperature, 1);
    Serial.print("\t\t");
    Serial.print(stop - start);
    Serial.print("\t\t");
    switch (status)
    {
      case DHT20_OK:
        Serial.print("OK");
        break;
      case DHT20_ERROR_CHECKSUM:
        Serial.print("Checksum error");
        break;
      case DHT20_ERROR_CONNECT:
        Serial.print("Connect error");
        break;
      case DHT20_MISSING_BYTES:
        Serial.print("Missing bytes");
        break;
      case DHT20_ERROR_BYTES_ALL_ZERO:
        Serial.print("All bytes read zero");
        break;
      case DHT20_ERROR_READ_TIMEOUT:
        Serial.print("Read time out");
        break;
      case DHT20_ERROR_LASTREAD:
        Serial.print("Error read too fast");
        break;
      default:
        Serial.print("Unknown error");
        break;
    }
    Serial.print("\n");

    // Send data to ThingSpeak
    ThingSpeak.setField(1, OfficeHumidity);
    ThingSpeak.setField(2, OfficeTemperature);

    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if(x == 200)
    {
      Serial.println("Channel update successful.");
    }
    else
    {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
}
