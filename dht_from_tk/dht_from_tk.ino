// This code interacts with a T-Beam ESP8266 MCU and pulls information from a thingspeak channel
// These informations are regarding the temperature and humidity of two different regions of the
// robotics lab and assign an angle to a servo motor to adjust thermostat levels.


#include <Wire.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

Servo tempKnob;

// Wi-Fi credentials
const char* ssid = "NAME";
const char* password = "PASS";

// ThingSpeak details
unsigned long myChannelNumber =  CHAN NUM;  // Replace with your ThingSpeak Channel Number
const char* myReadAPIKey = "READ KEY";       // Replace with your ThingSpeak Read API Key

// Servo GPIO pin
const int servoPin = 2;

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define MAXTEMP 280
#define MINTEMP 180
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiClient client;

int angle;

void setup() {

  tempKnob.attach(servoPin);

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
  float humidity = ThingSpeak.readFloatField(myChannelNumber, 1, myReadAPIKey);
  float outdoorTemp = ThingSpeak.readFloatField(myChannelNumber, 2, myReadAPIKey);
  float indoorTemp = ThingSpeak.readFloatField(myChannelNumber, 4, myReadAPIKey);

  angle = map(((int)(outdoorTemp*10.0)), MAXTEMP, MINTEMP, 0, 180);

  // Clear the display
  display.clearDisplay();
  
  // Display Humidity
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0,0);
  display.print("Humidity: ");
  display.print(humidity);
  display.println(" %");

  // Display Outdoor Temperature
  display.setCursor(0, 20);
  display.print("Outdoor Temp: ");
  display.print(outdoorTemp);
  display.println(" C");

  // Display Indoor Temperature
  display.setCursor(0, 40);
  display.print("Indoor Temp: ");
  display.print(indoorTemp);
  display.println(" C");

  // Show the display
  display.display();

  // Set servo angle
  tempKnob.write(angle);

  // Wait before next update
  delay(20000); // Update every 20 seconds
}

