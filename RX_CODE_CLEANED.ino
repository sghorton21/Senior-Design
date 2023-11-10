// UPDATED 11-10-2023 12:50pm - CC

#include <SPI.h>                  //SPI
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_DPS310.h>
// #include <Fonts/FreeMono9pt7b.h>
const byte address[6] = "00001";
const int DB_REGISTER = 0x0A;
U8G2_SSD1306_128X64_NONAME_F_SW_I2C OLED_1(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* reset=*/ 8);
RF24 radio(9, 10); // CE, CSN (MAKE SURE THESE ARE THE RIGHT PINS ON ARDUINO)
Adafruit_DPS310 dps;
TwoWire splRx;
float SPL_DRONE, SPL_GROUND, ALT_DRONE, ALT_GROUND, DELTA_ALT, SPL_PREDICT;

void setup() {
  Serial.begin(115200);
  if(!OLED_1.begin()) { 
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  delay(2000);
  // OLED_1.setFont(&FreeMono9pt7b);
  OLED_1.clearDisplay();
  OLED_1.setFont(u8g2_font_6x10_tf);
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
  GetSPL(0x46);
  UpdateOLED();
  DELTA_ALT = ALT_DRONE - ALT_GROUND;
  SPL_PREDICT = SPL_DRONE * 0.4; // placeholder equation
  // delay(500);
}

float GetAlt() {
  sensors_event_t temp_event, pressure_event;
  dps.getEvents(&temp_event, &pressure_event);
  ALT_GROUND = dps.readAltitude(1013.25);
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

float GetSPL(int SPL_ADDRESS) {
  splRx.beginTransmission(SPL_ADDRESS);
  splRx.write(DB_REGISTER);
  splRx.endTransmission(false);
  splRx.requestFrom(SPL_ADDRESS, 1);
  SPL_GROUND = splRx.read();
  return SPL_GROUND;
}
void UpdateOLED() {
  OLED_1.clearDisplay();  // Clear the previous content
  OLED_1.setCursor(0, 0);      // Set "SPL Value: " at this position on OLED
  // OLED_1.print("Drone SPL: ");  // Print "SPL Value: " on OLED
  // OLED_1.print(SPL_DRONE);       // Print SPL receiving value on OLED
  // OLED_1.print(" dB");          // Print SPL units (dB) on OLED
  // OLED_1.setCursor(0,10);        // Set "Drone Alt: " at this position on OLED
  // OLED_1.print("Drone Altitude: ");   // Print "Drone Alt: " on OLED
  // OLED_1.print(ALT_DRONE);    // Print altitude value on OLED
  // OLED_1.print(" m");
  // OLED_1.setCursor(0, 20);      // Set "SPL Value: " at this position on OLED
  OLED_1.print("Ground Altitude:: ");
  OLED_1.setCursor(0, 10);  // Print "SPL Value: " on OLED
  OLED_1.print(ALT_GROUND);       // Print SPL receiving value on OLED
  OLED_1.print(" m");          // Print SPL units (dB) on OLED
  OLED_1.setCursor(0, 30);      // Set "SPL Value: " at this position on OLED
  OLED_1.print("Ground SPL: ");  // Print "SPL Value: " on OLED
  OLED_1.print(SPL_GROUND);       // Print SPL receiving value on OLED
  OLED_1.print(" dB");          // Print SPL units (dB) on OLED
  OLED_1.display();  // Update the OLED display
}
