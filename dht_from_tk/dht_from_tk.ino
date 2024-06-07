// This code interacts with a T-Beam ESP8266 MCU and pulls information from a thingspeak channel
// These informations are regarding the temperature and humidity of two different regions of the
// robotics lab and assign an angle to a servo motor to adjust thermostat levels.


#include <Wire.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi credentials
const char* ssid = "**************";
const char* password = "**********";

// ThingSpeak details
unsigned long myChannelNumber = 123456;  // Replace with your ThingSpeak Channel Number
const char* myReadAPIKey = "**********";       // Replace with your ThingSpeak Read API Key

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiClient client;

// Function to calculate the predicted temperature using the linear regression model
float predict_temperature(float officeTemp, float labTemp) {
  // Coefficients and intercept from your trained model
  // Replace these values with your actual model's coefficients and intercept
  float coefficients[2] = {1.03315268e-16, 5.55111512e-17, 1.32218529e-01, 5.50687291e-17}; // Example coefficients
  float intercept = 64.195825; // Example intercept

  // Calculate the predicted temperature
  float predictedTemp = intercept +
                        coefficients[0] * officeTemp +
                        coefficients[1] * labTemp;

  return predictedTemp;
}

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
  float officeTemp = ThingSpeak.readFloatField(myChannelNumber, 2, myReadAPIKey);
  float labTemp = ThingSpeak.readFloatField(myChannelNumber, 4, myReadAPIKey);

  // Calculate predicted temperature
  float predictedTemp = predict_temperature(officeTemp, labTemp);

  // Clear the display
  display.clearDisplay();
  
  // Display Office Temperature
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0, 0);
  display.print("Office Temp: ");
  display.print(officeTemp);
  display.println(" C");

  // Display Lab Temperature
  display.setCursor(0, 10);
  display.print("Lab Temp: ");
  display.print(labTemp);
  display.println(" C");

  // Display Predicted Temperature
  display.setCursor(0, 20);
  display.print("Pred Temp: ");
  display.print(predictedTemp);
  display.println(" C");

  // Show the display
  display.display();

  // Wait before next update
  delay(20000); // Update every 20 seconds
}


