// This code interacts with a T-Beam ESP8266 MCU and pulls information from a thingspeak channel
// These informations are regarding the temperature and humidity of two different regions of the
// robotics lab and assign an angle to a servo motor to adjust thermostat levels.


#include <Wire.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi credentials
const char* ssid = "MDX welcomes you";
const char* password = "MdxL0vesyou";

// ThingSpeak details
unsigned long myChannelNumber =  2556720;  // Replace with your ThingSpeak Channel Number
const char* myReadAPIKey = "RB2FDXS6LTL8JA4V";       // Replace with your ThingSpeak Read API Key

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiClient client;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(100);

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Retrieve data from ThingSpeak
  float officehumidity = ThingSpeak.readFloatField(myChannelNumber, 1, myReadAPIKey);
  float officeTemp = ThingSpeak.readFloatField(myChannelNumber, 2, myReadAPIKey);
  float labhumidity = ThingSpeak.readFloatField(myChannelNumber, 3, myReadAPIKey);
  float labTemp = ThingSpeak.readFloatField(myChannelNumber, 4, myReadAPIKey);

  // Clear the display
  display.clearDisplay();
  
  // Display Humidity
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0,0);
  display.print("Office HMDT: ");
  display.print(officehumidity);
  display.println(" %");

  // Display Outdoor Temperature
  display.setCursor(0, 10);
  display.print("Office Temp: ");
  display.print(officeTemp);
  display.println(" C");

  display.setCursor(0, 20);
  display.print("Lab HMDT: ");
  display.print(labhumidity);
  display.println(" C");
  // Display Indoor Temperature
  display.setCursor(0, 30);
  display.print("Lab Temp: ");
  display.print(labTemp);
  display.println(" C");

  // Show the display
  display.display();

  // Wait before next update
  delay(20000); // Update every 20 seconds
}

