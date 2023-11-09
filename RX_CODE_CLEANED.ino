// UPDATED 11-09-2023 3:15pm - CC

#include <SPI.h>                  //SPI
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_DPS310.h>
// #include <Fonts/FreeMono9pt7b.h>
const byte address[6] = "00001";
Adafruit_SSD1306 OLED_1(128, 64, &Wire, -1);
RF24 radio(9, 10); // CE, CSN (MAKE SURE THESE ARE THE RIGHT PINS ON ARDUINO)
Adafruit_DPS310 dps;
float SPL_DRONE, ALT_DRONE, ALT_GROUND, DELTA_ALT, SPL_PREDICT;

void setup() {
  Serial.begin(115200);
  if(!OLED_1.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  delay(2000);
  OLED_1.clearDisplay();
  OLED_1.setTextSize(1);             
  OLED_1.setTextColor(WHITE);
  dps.begin_I2C(0x77, &Wire);
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() {
  GetString();
  GetAlt();
  DELTA_ALT = ALT_DRONE - ALT_GROUND;
  SPL_PREDICT = SPL_DRONE * 0.4; // placeholder equation
  UpdateOLED();
  delay(500);
}

float GetAlt() {
  sensors_event_t temp_event, pressure_event;
  dps.getEvents(&temp_event, &pressure_event);
  float ALT_GROUND = dps.readAltitude(1013.25);
  return ALT_GROUND;
}

float GetString() {
  char combinedMessage[32]; 
  radio.read(&combinedMessage, sizeof(combinedMessage));
  String combinedStr = String(combinedMessage);
  int separatorIndex = combinedStr.indexOf('|'); //Defines the index of | in the receiving string as an integer value
  String splStr = combinedStr.substring(4, separatorIndex - 3); //Create a new string from the receiving string that contains the SPL value
  String altitudeStr = combinedStr.substring(separatorIndex + 12, separatorIndex + 20); 
  SPL_DRONE = splStr.toFloat();
  ALT_DRONE = altitudeStr.toFloat();
  return SPL_DRONE, ALT_DRONE;
}

void UpdateOLED() {
  OLED_1.clearDisplay();  // Clear the previous content
  OLED_1.setCursor(0, 0);      // Set "SPL Value: " at this position on OLED
  OLED_1.print("Drone SPL: ");  // Print "SPL Value: " on OLED
  OLED_1.print(SPL_DRONE);       // Print SPL receiving value on OLED
  OLED_1.print(" dB");          // Print SPL units (dB) on OLED
  OLED_1.setCursor(0,10);        // Set "Drone Alt: " at this position on OLED
  OLED_1.print("Drone Altitude: ");   // Print "Drone Alt: " on OLED
  OLED_1.print(ALT_DRONE);    // Print altitude value on OLED
  OLED_1.print(" m");
  OLED_1.setCursor(0, 20);      // Set "SPL Value: " at this position on OLED
  OLED_1.print("Ground Altitude:: ");  // Print "SPL Value: " on OLED
  OLED_1.print(ALT_GROUND);       // Print SPL receiving value on OLED
  OLED_1.print(" m");          // Print SPL units (dB) on OLED
  OLED_1.setCursor(0, 30);      // Set "SPL Value: " at this position on OLED
  OLED_1.print("Predicted SPL: ");  // Print "SPL Value: " on OLED
  OLED_1.print(SPL_PREDICT);       // Print SPL receiving value on OLED
  OLED_1.print(" dB");          // Print SPL units (dB) on OLED
  OLED_1.display();  // Update the OLED display
}
